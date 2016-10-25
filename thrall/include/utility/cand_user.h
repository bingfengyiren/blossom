#ifndef __CANDUSER_H__
#define __CANDUSER_H__
/*用户关系推荐候选集存储。包含候选用户数量及候选用户信息。
 * 用户候选信息包含了uid、总得分。四种桥梁关系各自的得分、桥梁数量、top3桥梁等信息。
 * 四种桥梁：0-关注关注 1-关注好友 2-好友关注 3-好友好友
 * hadoop平台生成的文件采用二进制格式存储。本文件用于解析二进制流，生成用户候选数据结构体。
 * 注意：非线程安全，使用时请为每个线程分别申请candUser对象。
 * 
 * Author:xiaohu8
 * Date:20140507
 */
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "zlib.h"
#include <string>
#include <vector>
/*最多保存3个桥梁，4种理由。
 * 候选用户不超过3000个。
 */
const static int MAX_BRIDGE_NUM = 3;
const static int REASON_NUM = 4; //	和hadoop生成数据相关，不可修改
const static int MAX_CAND_NUM = 3000;
const static int MAX_BUF_SIZE = 1024*1024*10;

//桥梁元素信息，包含得分、桥梁数目、桥梁
struct bridge_item_t {
	uint64_t score;
	uint64_t bs[MAX_BRIDGE_NUM];
	uint32_t bnum;
};
//推荐候选，用户id，总得分，四种桥梁理由
struct candidate_item_t {
	uint64_t uid;
	uint64_t tscore;
	uint64_t rpmscore;					// RPM得分，20140903新增
	bridge_item_t reason[REASON_NUM];
	uint32_t utype;                     // 用户类型，20140903新增，0表示普户，1表示橙V，2表示蓝V
};
typedef std::vector<candidate_item_t*> VEC_CAND; //定义算法的入口
//候选集合
struct candidate_user_t {
	int num;
	candidate_item_t cands[0];
};

// 按照RPM分值排序，相等时按照tscore排序
bool cmp_Candidate_item(const candidate_item_t* a, const candidate_item_t* b){
	if(a != NULL && b != NULL){
	   	if( a->rpmscore == b->rpmscore) return a->tscore > b->tscore;
		return a->rpmscore > b->rpmscore;
	}
	return true;
}	

// 支持C语言形式的接口，但是不保证data的空间够用，使用前，请分配足够的空间。建议使用以下封装的c++接口。
int candidate_bintostruct(const char* buf, candidate_user_t *data, int& num);
/*
 * 候选用户类，初始分配存储空间。对传入的buf进行解析得到候选集合结构体,如果有压缩，压缩使用zlib压缩。
 * 
 */

class CandUser {
public :
   	CandUser(bool iscompress = true) : data_(NULL), compress_(iscompress), zbuf_(NULL) {
		data_ = (candidate_user_t*) malloc(sizeof(candidate_user_t) + \
			   	MAX_CAND_NUM * sizeof(candidate_item_t));
		if (!data_) {
			printf("malloc data_ error\n");
		}
		if (compress_) {
			zbuf_ = (char*)malloc(MAX_BUF_SIZE);
			if (!zbuf_) {
				printf("malloc zbuff error\n");
			}
		}
	}
    ~CandUser() {
		if (data_) {
			free(data_);
		}

		//remove memleak caused by xiaohu
		if(zbuf_) {
			free(zbuf_);
		}
	}	
	candidate_user_t* data() {
		return data_;
	}
	
	int get_num() {
		if (data_) {
			return data_->num;
		}
		return 0;
	}

	int parse(const char* buf, int buff_len, int& num) {
		if (compress_) {
			uLong dec_buf_size = MAX_BUF_SIZE;
			int ret = uncompress((Bytef*)zbuf_, &dec_buf_size, (Bytef*)buf, buff_len);
			if (ret != Z_OK) {
				zbuf_[0] = '\0';
				printf("fail to zlib uncompress:ret:%d\n", ret);
				return -2;
			}
			zbuf_[dec_buf_size] = '\0';
			return candidate_bintostruct((char*)zbuf_, data_, num);
		}
		return candidate_bintostruct(buf, data_, num);
	}
	std::string to_string() {
		std::string rs;
		candidate_user_t *ct = data_;
		if (ct == NULL) {
			return "";
		}
		char buf[1024];
		snprintf(buf, 1024, "num:%d", ct->num);
		rs += buf;
		for (int i = 0; i < ct->num; ++ i) {
			snprintf(buf, sizeof(buf), "order:%d\tuid:%lu\ttscore:%lu\n", i, ct->cands[i].uid,
					ct->cands[i].tscore);
			rs += buf;
			for (int j = 0; j < REASON_NUM; ++ j) {
			    snprintf(buf, sizeof(buf), "type:%d\tbnum:%d\tbscore:%lu\t", j, \
						ct->cands[i].reason[j].bnum, ct->cands[i].reason[j].score);
				rs += buf;
				for (uint32_t m = 0; m < ct->cands[i].reason[j].bnum && \
						m < (uint32_t)MAX_BRIDGE_NUM; ++ m) {
					snprintf(buf, sizeof(buf), "%lu\t", ct->cands[i].reason[j].bs[m]);
					rs += buf;
				}
			}
			rs += "\n";
		}
		return rs;
	} 

	int print() {
		candidate_user_t *ct = data_;
		if (ct == NULL) {
			return -1;
		} 
		for (int i = 0; i < ct->num; ++ i) {
			printf("order:%d\tuid:%lu\ttscore:%lu\n", i, ct->cands[i].uid, \
					ct->cands[i].tscore);
			for (int j = 0; j < REASON_NUM; ++ j) {
				printf("type:%d\tbnum:%d\tbscore:%lu\t", j, \
						ct->cands[i].reason[j].bnum, ct->cands[i].reason[j].score);
				for (uint32_t m = 0; m < ct->cands[i].reason[j].bnum && \
						m < (uint32_t)MAX_BRIDGE_NUM; ++ m) {
					printf("%lu\t", ct->cands[i].reason[j].bs[m]);
				}
			}
			printf("\n");
		}
		return 0;
	}

private : 
	candidate_user_t *data_;
	bool compress_;
	char* zbuf_;
};

int candidate_bintostruct(const char* buf, candidate_user_t *data, int& num) {
		candidate_user_t *ct = data;
		if (!ct) {
			return -1;
		}
		ct->num = 0;
		if (!buf) {
			return -1;
		}
		int cand_num;
		memcpy(&cand_num, buf, sizeof(int));
		if (cand_num > MAX_CAND_NUM) {
			cand_num = MAX_CAND_NUM;
		}

		if(cand_num > num) {
			cand_num = num;
		}else
			num = cand_num;

		buf += sizeof(int);
		ct->num = cand_num;
		//printf("candidate nums : %d\n", cand_num);
		for (int i = 0; i < cand_num; ++ i) {
			candidate_item_t *item = &(ct->cands[i]);
			item->rpmscore = 0;				// 初始化rpm得分为0
			memcpy(&(item->uid), buf, sizeof(uint64_t));
			buf += sizeof(uint64_t);
			memcpy(&(item->tscore), buf, sizeof(uint64_t));
			buf += sizeof(uint64_t);
			//printf("uid:%lu\ttscore:%lu\n", item->uid, item->tscore);
			if (item->tscore == 0) {
				continue;
			}
			for (int j = 0; j < REASON_NUM; ++ j) {
				int bridge_num = 0;
				int type = 0;
				memcpy(&type, buf, sizeof(int));
				buf += sizeof(int);
				memcpy(&bridge_num, buf, sizeof(int));
				buf += sizeof(int);
				item->reason[j].bnum = bridge_num;
				if (bridge_num == 0) {
					continue;
				}
				memcpy(&(item->reason[j].score), buf, sizeof(uint64_t));
				buf += sizeof(uint64_t);
				for (int k = 0; k < bridge_num && k < MAX_BRIDGE_NUM; ++ k) {
					memcpy(&(item->reason[j].bs[k]), buf, sizeof(uint64_t));
					buf += sizeof(uint64_t);
				}	
			}
		}
		return 0;

	}


#endif
