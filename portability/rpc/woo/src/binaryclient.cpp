/**
 * @file
 * @author  Li Jiong <lijiong@staff.sina.com.cn>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * woo framework 
 */
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include "woo/log.h"
#include "woo/tcpserver.h"
#include "woo/binaryclient.h"
#include <netinet/tcp.h>

namespace woo {

/*
int connect_to (int sfd, struct sockaddr *addr, int addrlen, struct timeval *timeout) {
	struct timeval sv;
	int svlen = sizeof(sv);
	int ret;

	if (!timeout)
		return connect (sfd, addr, addrlen);
	if (getsockopt (sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&sv, &svlen) < 0)
		return -1;
	if (setsockopt (sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)timeout, sizeof(timeout)) < 0)
		return -1;
	ret = connect (sfd, addr, addrlen);
	setsockopt (sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&sv, sizeof(sv));
	return ret;
}
*/
const int MAX_SERVERS_NUM = 35;

typedef struct _server_addr_t {
	struct in_addr addr;
	uint16_t port;
} server_addr_t;

struct _binary_client_t {
	server_addr_t servers[MAX_SERVERS_NUM];
	int first_num;
	int second_num;
	int sock;
	struct timeval send_to;
	struct timeval recv_to;
};

int
binary_client_init(binary_client_t* client, const char *servers, int send_to, int recv_to) {
	const int MAX_SERVERS_LEN = 1024;
	char buf[MAX_SERVERS_LEN];

	client->sock = -1;

	char *section;
	char *token;
	const char *delim1 = ";";
	char *saveptr1;
	const char *delim2 = ",";
	char *saveptr2;
	int i = 0;
	char *p;

	client->second_num = client->first_num = 0;
	strncpy(buf, servers, sizeof(buf));
	section = strtok_r(buf, delim1, &saveptr1);
	while (section) {
		token = strtok_r(section, delim2, &saveptr2);
		while (token) {
			p = strchr(token, ':');
			if (p) {
				*p = '\0';
				++ p;
				if (i < MAX_SERVERS_NUM) {
					//LOG_DEBUG("add client addr[%d] %s:%s", i, token, p);
					inet_pton(AF_INET, token, &client->servers[i].addr);
					client->servers[i].port = htons(atoi(p));
					++ i;
				} else {
					LOG_WARN("ignore client addr[%d] %s:%s", i, token, p);
				}
			}
			token = strtok_r(NULL, delim2, &saveptr2);
		}
		if (i > 0 && client->first_num == 0) {
			client->first_num = i;
		}
		section = strtok_r(NULL, delim1, &saveptr1);
	}
	client->second_num = i - client->first_num;

	client->send_to.tv_usec = send_to % 1000000;
	client->send_to.tv_sec = send_to / 1000000;
	client->recv_to.tv_usec = recv_to % 1000000;
	client->recv_to.tv_sec = recv_to / 1000000;
	return 0;
}

binary_client_t*
binary_client_create(const char *servers, int send_to, int recv_to) {
	binary_client_t *client = (binary_client_t *)calloc(1, sizeof(binary_client_t));
	if (! client) {
		return NULL;
	}
	int ret = binary_client_init(client, servers, send_to, recv_to);
	if (ret) {
		free(client);
		client = NULL;
		return NULL;
	}
	return client;
}

binary_client_t*
binary_client_create(const char *host, int port, int send_to, int recv_to) {
	char servers[100];
	snprintf(servers, sizeof(servers), "%s:%d", host, port);
	return binary_client_create(servers, send_to, recv_to);
}

ssize_t 
safe_send(int s, const char *buf, size_t len, int flags) {
	size_t c = 0;
	while (c < len) {
		ssize_t n = ::send(s, buf + c, len - c , flags);
		if (n == 0) {
			LOG_DEBUG("sock[%d] send ret[%zd] has been closed, have send[%zu]", 
					s, n, c);
			return -1;
		} else if (n < 0) {
			LOG_DEBUG("sock[%d] send ret[%zd]  error[%d][%m], have send[%zu]", 
					s, n, errno, c);
			return -1;
		}
		c += n;
	}
	return c;
}

void
binary_client_close(binary_client_t *client) {
	if (client->sock >= 0) {
		close(client->sock);
		client->sock = -1;
	}
}

void
binary_client_destroy(binary_client_t *client) {
	binary_client_close(client);
	free(client);
}

int
binary_client_sock(binary_client_t *client) {
	if (! client) {
		return -1;
	}
	return client->sock;
}

int binary_client_connect(struct in_addr server_addr, uint16_t server_port,
		struct timeval *send_to, struct timeval *recv_to) {
	char ipstr[128];
	struct sockaddr_in addr;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	int nodelay = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, 
				recv_to, sizeof(*recv_to)) < 0) {
		LOG_TRACE("set sock recv timeout error");
		close(sock);
		sock = -1;
		return -1;
	}
	if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, 
				send_to, sizeof(*send_to)) < 0) {
		LOG_TRACE("set sock recv timeout error");
		close(sock);
		sock = -1;
		return -1;
	}
	if (setsockopt(sock, IPPROTO_TCP,  TCP_NODELAY, 
				(char *)&nodelay, sizeof(nodelay)) < 0) {
		LOG_TRACE("set sock nodelay error");
		close(sock);
		sock = -1;
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_addr = server_addr;
	addr.sin_port = server_port;
	int ret = connect(sock, (const struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		LOG_TRACE("connect %s:%d error[%d][%m]", 
				inet_ntop(AF_INET, &server_addr, ipstr, INET_ADDRSTRLEN), 
				htons(server_port), errno);
		close(sock);
		sock = -1;
		return -1;
	} else {
		//LOG_TRACE("connect %s:%d succ", inet_ntop(AF_INET, &server_addr, ipstr, INET_ADDRSTRLEN), htons(server_port));
	}

	return sock;
}

int binary_client_connect(const char *host, int port,
		struct timeval *send_to, struct timeval *recv_to) {
	struct in_addr inp;
	inet_aton(host, &inp);
	return binary_client_connect(inp, htons(port), send_to, recv_to);
}

int
binary_client_connect(server_addr_t *servers, int server_num,
		struct timeval *send_to, struct timeval *recv_to, long hash) {
	int n;
    if (hash >= 0) {
		n = hash;
	} else {
		n = rand();
	}
	//LOG_DEBUG("hash[%ld] server_num[%d] n[%d]", hash, server_num, n);
	int sock = -1;
	int idx = 0;
	int retry_times = 3;
	int i;

	if (retry_times > server_num) {
		retry_times = server_num;
	}
	for (i = 0; i < retry_times; ++ i) {
		idx = (i + n) % server_num;
		//LOG_DEBUG("hash[%ld] server_num[%d] idx[%d]", hash, server_num, idx);
		sock = binary_client_connect(servers[idx].addr, servers[idx].port, 
				send_to, recv_to);
		if (sock >= 0) {
			return sock;
		}	
	}
	return -1;
}

int
binary_client_connect(server_addr_t *first_servers, int first_num,
		server_addr_t *second_servers, int second_num,
		struct timeval *send_to, struct timeval *recv_to, long hash) {
	//LOG_DEBUG("connect with first");
	int sock = binary_client_connect(first_servers, first_num,
			send_to, recv_to, hash);
	if (sock < 0 && second_num > 0) {
		LOG_DEBUG("connect first failed, retry second ...");
		sock = binary_client_connect(second_servers, second_num,
				send_to, recv_to, hash);
	}
	return sock;
}

ssize_t 
binary_client_send(binary_client_t *client, const char *data, uint32_t data_len, uint32_t log_id, bool reset_conn, long req_hash, bool auto_conn) {
	if (reset_conn) {
		binary_client_close(client);
	}
	if (client->sock < 0 && auto_conn) {
		//LOG_DEBUG("re connect ...");
		int sock = binary_client_connect(client->servers, client->first_num,
				client->servers + client->first_num, client->second_num,
			   	&client->send_to, &client->recv_to, req_hash);
		if (sock < 0) {
			LOG_TRACE("connect server error");
			return -1;
		}
		client->sock = sock;
		//LOG_DEBUG("re connect succ"); 
	}
	binary_head_t head;
	memset(&head, 0, sizeof(head));
	head.log_id = log_id;
	head.body_len = data_len;
	ssize_t n = safe_send(client->sock, (const char *)&head, sizeof(head), 0);
	if (n != sizeof(head)) {
		LOG_TRACE("send head error");
		close(client->sock);
		client->sock = -1;
		return -1;
	}
	n = safe_send(client->sock, data, data_len, 0);
	if (n != data_len) {
		LOG_TRACE("send data error");
		close(client->sock);
		client->sock = -1;
		return -1;
	}
	return 0;
}

ssize_t 
binary_client_recv(binary_client_t *client, char *buf, uint32_t *data_len, 
		uint32_t buf_size, uint32_t *log_id, bool long_conn) {
	if (client->sock < 0) {
		LOG_TRACE("sock error");
		return -1;
	}
	binary_head_t head;
	ssize_t ret = recv(client->sock, &head, sizeof(binary_head_t), 0);
	if (ret != sizeof(binary_head_t)) {
		LOG_TRACE("recv error");
		close(client->sock);
		client->sock = -1;
		return -1;
	}
	if (log_id) {
		*log_id = head.log_id;
	}
	if (head.body_len + 1 + sizeof(binary_head_t) > buf_size)  {
		LOG_TRACE("size not enough, body len[%u] buf_size[%u]", head.body_len, buf_size);
		close(client->sock);
		client->sock = -1;
		return -1;
	}

	ssize_t recv_body_len = safe_recv(client->sock, buf, head.body_len, 0);
	if (recv_body_len != ssize_t(head.body_len)) {
		LOG_TRACE("recv error");
		close(client->sock);
		client->sock = -1;
		return -1;
	}
	buf[head.body_len] = '\0';
	*data_len = head.body_len;

	if (! long_conn) {
		close(client->sock);
		client->sock = -1;
	}	

	return 0;
}

ssize_t 
binary_client_talk(binary_client_t *client, const char *req, uint32_t req_len,
		char *buf, uint32_t *data_len, uint32_t buf_size, uint32_t log_id, bool long_conn) {
	ssize_t	ret = binary_client_send(client, req, req_len, log_id);
	if (ret) {
		LOG_DEBUG("send failed");
		return -1;
	}
	return binary_client_recv(client, buf, data_len, buf_size, NULL, long_conn);
}

struct _binary_client_pool_t {
	binary_client_t *clients;
	size_t max_client_num;
	size_t client_num;
};

binary_client_pool_t*
binary_client_pool_create(size_t max_client_num) {
	binary_client_pool_t *pool = (binary_client_pool_t *)calloc(1, sizeof(binary_client_pool_t));
	if (! pool) {
		LOG_TRACE("malloc pool error");
		return NULL;
	}
	pool->client_num = 0;
	pool->max_client_num = max_client_num;
	pool->clients = (binary_client_t *)calloc(max_client_num, sizeof(binary_client_t));
	if (! pool->clients) {
		LOG_TRACE("malloc clients[%zu] error", max_client_num);
		free(pool);
		pool = NULL;
		return NULL;
	}
	return pool;
}

int
binary_client_pool_add(binary_client_pool_t *pool, const char *host, int port, 
		int send_to, int recv_to) {
	char servers[1024];
	if (pool->client_num >= pool->max_client_num) {
		return -1;
	}
	binary_client_t *client = pool->clients + pool->client_num; 
	++ pool->client_num;

	snprintf(servers, sizeof(servers), "%s:%d", host, port);
	binary_client_init(client, servers, send_to, recv_to);

	return 0;
}

void
binary_client_pool_destroy(binary_client_pool_t *pool) {
	for (size_t i = 0; i < pool->client_num; ++ i) {
		binary_client_close(pool->clients + i);
	}
	free(pool->clients);
	free(pool);
}

ssize_t 
binary_client_pool_talk(binary_client_pool_t *pool, const char *req, uint32_t req_len,
		char *buf, uint32_t *data_len, uint32_t buf_size, uint32_t log_id, uint32_t hash,
		int retry_times, int poll_times, bool long_conn) {
	int first = hash % pool->client_num;
	int idx;
	int ret;

	for (int i = 0; i < poll_times; ++ i) {
		idx = (i + first) % pool->client_num;
		for (int j = 0; j < retry_times; ++ j) {
			LOG_DEBUG("user client %d", idx);
			ret = binary_client_talk(pool->clients + idx, req, req_len, buf, data_len, 
					buf_size, log_id, long_conn);
			if (0 == ret) {
				return ret;
			}
		}
	}
	return ret;
}


}
