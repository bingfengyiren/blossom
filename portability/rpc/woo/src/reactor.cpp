/**
 * @file
 * @author  Li Jiong <lijiong@staff.sina.com.cn>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * woo framework 
 */
#include <sys/epoll.h>
#include <deque>
#include <errno.h>
#include <pthread.h>
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
#include <fcntl.h>
#include <inttypes.h>
#include <netinet/tcp.h>
#include "woo/reactor.h"
#include "woo/tcpserver.h"
#include "woo/log.h"

namespace woo {

typedef struct _reactor_event_data_t reactor_event_data_t;

struct _reactor_t {
	int epoll_fd;
	int max_event_num;
	struct epoll_event *events;
	int pool_thread_num;
	pthread_t *pool_thread;
	bool quit;
	pthread_mutex_t queue_mutex;
	pthread_cond_t queue_cond;
	std::deque<reactor_event_data_t *> *queue;
	size_t max_queue_size;
	pthread_t pool_thread_id;
};

struct _reactor_event_data_t {
	void *handle_data;
	reactor_event_handle_t handle;
	uint64_t timeout;
	int fd;
	int flag;
}; 

reactor_t* reactor_create(int pool_thread_num) {
	reactor_t *reactor = (reactor_t *)malloc(sizeof(reactor_t));
	if (! reactor) {
		return NULL;
	}
	reactor->epoll_fd = epoll_create(4096);
	if (reactor->epoll_fd  < 0) {
		LOG_ERROR("epoll_create error[%d][%m]", errno);
		return NULL;
	}
	reactor->pool_thread_num = pool_thread_num;
	if (pool_thread_num) {
		reactor->pool_thread = (pthread_t *)malloc(sizeof(pthread_t) * pool_thread_num);
		if (! reactor->pool_thread) {
			LOG_ERROR("malloc reactor thread error[%d][%m]", errno);
			return NULL;
		}
	} else {
		reactor->pool_thread = NULL;
	}
	reactor->quit = false;
	reactor->max_event_num = 4096;
	reactor->events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * reactor->max_event_num);
	if (! reactor->events) {
		LOG_ERROR("malloc reactor event error[%d][%m]", errno);
		return NULL;
	}

	reactor->max_queue_size = 256;
	reactor->queue = new std::deque<reactor_event_data_t *>();
	if (! reactor->queue) {
		LOG_ERROR("create queue error");
		return NULL;
	}

	int ret;
	ret = pthread_mutex_init(&reactor->queue_mutex, NULL);
	if (ret) {
		LOG_ERROR("pthread_mutex_init ret[%d] errno[%d][%m]", ret, errno);
		return NULL;
	}

	ret = pthread_cond_init(&reactor->queue_cond, NULL);
	if (ret) {
		LOG_ERROR("pthread_cond_init ret[%d] errno[%d][%m]", ret, errno);
		return NULL;
	}

	return reactor;
}

int reactor_add(reactor_t *reactor, int events, 
		reactor_event_handle_t handle, void *handle_data, int fd, int flag, size_t timeout) {
	reactor_event_data_t *event_data
	   	= (reactor_event_data_t *)malloc(sizeof(reactor_event_data_t));
	if (! event_data) {
		LOG_ERROR("malloc reactor_event_data_t error");
		return -1;
	}	
	event_data->fd = fd;
	event_data->timeout = timeout;
	event_data->handle = handle;
	event_data->handle_data = handle_data;
	event_data->flag = flag;

	struct epoll_event ev;
	ev.events = events;
	ev.data.ptr = event_data;
	if (epoll_ctl(reactor->epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
		LOG_ERROR("epoll set insertion error: fd=%d\n", fd);
		return -1;
	}
	return 0;
}

int reactor_remove(reactor_t *reactor, int fd) {
	struct epoll_event ev;
	return epoll_ctl(reactor->epoll_fd, EPOLL_CTL_DEL, fd, &ev);
}

void reactor_quit(reactor_t *reactor) {
	reactor->quit = true;
	pthread_cond_broadcast(&reactor->queue_cond);
}

void reactor_destroy(reactor_t *reactor) {
	if (reactor->events) {
		free(reactor->events);
		reactor->events = NULL;
	}

	if (reactor->pool_thread) {
		free(reactor->pool_thread);
		reactor->pool_thread = NULL;
	}
	if (reactor->queue) {
		delete reactor->queue;
		reactor->queue = NULL;
	}
	if (reactor->epoll_fd >= 0) {
		close(reactor->epoll_fd);
	}
	free(reactor);
}

static
void *reactor_pool_thread(void *arg) {
	reactor_t *reactor = (reactor_t *)arg;
	int ret;
	reactor_event_data_t *event;
	while (! reactor->quit) {
		ret = pthread_mutex_lock(&reactor->queue_mutex);
		if (ret == 0) {
			while ((! reactor->quit) && reactor->queue->empty()) {
				//LOG_DEBUG("queue is empty, goto wait");
				pthread_cond_wait(&reactor->queue_cond, &reactor->queue_mutex);
			}
			if (reactor->quit) {
				pthread_mutex_unlock(&reactor->queue_mutex);
				break;
			}

			size_t queue_size_before = reactor->queue->size();
			size_t queue_size_after;
			if (queue_size_before * 4 <= reactor->max_queue_size) {
				event = reactor->queue->front();
				reactor->queue->pop_front();
				queue_size_after = reactor->queue->size();
				LOG_DEBUG("have got a sock from queue front, %zu - %zu", 
						queue_size_before, queue_size_after);
				event->flag &= ~REACTOR_EVENT_FLAG_QUEUE_LONG;
			} else {
				event = reactor->queue->back();
				reactor->queue->pop_back();
				queue_size_after = reactor->queue->size();
				LOG_TRACE("have got a sock from queue back, %zu - %zu", 
						queue_size_before, queue_size_after);
				event->flag |= REACTOR_EVENT_FLAG_QUEUE_LONG;
			}

			pthread_mutex_unlock(&reactor->queue_mutex);
		} else {
			LOG_ERROR("pop sock from queue, lock mutex error[%d]", ret);
			continue;
		}
		//LOG_DEBUG("get event from queue ptr[%p] handle[%p]", event, event->handle_data);
		ret = event->handle(reactor, event->handle_data, event->fd, &event->flag);
		LOG_TRACE("pool thread handle event, ret[%d] fd[%d]", ret, event->fd);
		free(event);
		event = NULL;
	}
	return NULL;
}

void reactor_wait(reactor_t *reactor) {
	void *thread_ret;
	for (int i = 0; i < reactor->pool_thread_num; ++ i) {
		pthread_join(reactor->pool_thread[i], &thread_ret);
	}
	if (reactor->pool_thread_id) {
		pthread_join(reactor->pool_thread_id, &thread_ret);
	}
}

int reactor_loop_poll(reactor_t *reactor) {
	int nfds;
	int i;
	int ret;
	struct epoll_event ev;
	reactor_event_data_t *ptr;

	while (! reactor->quit) {
		nfds = epoll_wait(reactor->epoll_fd, reactor->events, reactor->max_event_num, 500);
		//LOG_DEBUG("epoll_wait ret[%d]", nfds);
		for (i = 0; i < nfds; ++ i) {
			ptr = (reactor_event_data_t *)reactor->events[i].data.ptr;
			//LOG_DEBUG("flag [%d]", ptr->flag);
			if ((ptr->flag & REACTOR_EVENT_FLAG_FAST_LOOP) == 0) {
				//LOG_DEBUG("del fd[%d] from sock", ptr->fd);
				memset(&ev, 0, sizeof(ev));
				if (epoll_ctl(reactor->epoll_fd, EPOLL_CTL_DEL, ptr->fd, &ev) < 0) {
					LOG_ERROR("epoll del error: fd=%d", ptr->fd);
					return -1;
				}
			}

			ptr->flag &= ~ REACTOR_EVENT_FLAG_QUEUE_FULL;
			if ((ptr->flag & REACTOR_EVENT_FLAG_MAIN_THREAD_EXEC) 
					|| (ptr->flag & REACTOR_EVENT_FLAG_FAST_LOOP)
					|| (reactor->pool_thread_num == 0)) {
				ret = ptr->handle(reactor, ptr->handle_data, ptr->fd, &ptr->flag);
				if ((ptr->flag & REACTOR_EVENT_FLAG_FAST_LOOP) == 0) {
					LOG_TRACE("main thread handle event, ret[%d] fd[%d] flag[%d] %p, free", ret, ptr->fd, ptr->flag, ptr);
					free(ptr);
					ptr = NULL;
				} else {
					LOG_TRACE("main thread handle event, ret[%d] fd[%d] flag[%d] %p", ret, ptr->fd, ptr->flag, ptr);
				}
			} else {
				ret = pthread_mutex_lock(&reactor->queue_mutex);
				if (ret == 0) {
					//LOG_DEBUG("queue size[%zu]", reactor->queue->size());
					if (reactor->queue->size() >= reactor->max_queue_size) {
						pthread_mutex_unlock(&reactor->queue_mutex);
						LOG_ERROR("queue size[%zu] is full", reactor->queue->size());
						ptr->flag |= REACTOR_EVENT_FLAG_QUEUE_FULL;
						ret = ptr->handle(reactor, ptr->handle_data, ptr->fd, &ptr->flag);
						LOG_TRACE("main thread queue full, handle event, ret[%d] fd[%d]", ret, ptr->fd);
						free(ptr);
						ptr = NULL;
					} else {
						//LOG_DEBUG("push event to queue ptr[%p] handle[%p]", ptr, ptr->handle_data);
						reactor->queue->push_back(ptr);
						//pthread_cond_broadcast(&reactor->queue_cond);
						pthread_cond_signal(&reactor->queue_cond);
						pthread_mutex_unlock(&reactor->queue_mutex);
					}
				} else {
					LOG_ERROR("push sock to queue lock mutext error[%d]", ret);
				}
			}
		}
		//TODO:timeout queue
	}

	return 0;
}

void *reactor_thread_run(void* arg) {
	reactor_loop_poll((reactor_t *)arg);
	return NULL;
}

int reactor_run(reactor_t *reactor, bool new_thread) {
	int ret;

	LOG_TRACE("create reactor thread pool, size[%d] ...", reactor->pool_thread_num);
	for (int i = 0; i < reactor->pool_thread_num; ++ i) {
		ret = pthread_create(reactor->pool_thread + i, NULL, reactor_pool_thread, reactor);
		if (ret) {
			LOG_ERROR("pthread_create error");
			return -1;
		}
		//LOG_TRACE("create reactor_pool_thread succ %u", reactor->pool_thread[i]);
	}

	if (new_thread) {
		ret = pthread_create(&reactor->pool_thread_id, NULL, reactor_thread_run, reactor);
		if (ret) {
			LOG_ERROR("create reactor run thread error");
			return -1;
		} else {
			LOG_TRACE("create reactor run thread succ");
			return 0;
		}
	} else {
		return reactor_loop_poll(reactor);
	}
}

}
