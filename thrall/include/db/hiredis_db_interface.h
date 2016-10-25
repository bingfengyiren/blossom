#ifndef _HIREDIS_DB_INTERFACE_HEADER_
#define _HIREDIS_DB_INTERFACE_HEADER_
#include "hiredis.h"
#include <stdlib.h> 
#include <vector>
#include <string>
#include "utility.h"
#include "base_define.h"
#include "db_interface.h"

using namespace std;

typedef struct _Redis_Struct{
	char ip_[IP_WORD_LEN];	
	uint32_t port_;
	redisContext *redis_;
} Redis_Struct;

class HiRedisDbInterface;

typedef struct _RedisThreadPara{
	HiRedisDbInterface* p_redis_db_interface_;
	ReqStruct* p_req_struct_;
	SvrIpPort* p_svr_ip_port_;	
    VecMResult* p_vec_result_;
	char (*p_uncompress_) [COMPRESS_LEN];
	int n_spend_time_[4];
} RedisThreadPara;

//template<typename Hash_Func>
class HiRedisDbInterface : public DbInterface{
	public:
		HiRedisDbInterface(const Db_Info_Struct& db_info_struct):
					DbInterface(db_info_struct){
			// load configure

			for(vector<string>::iterator port_it = vec_str_port_.begin();
                    port_it != vec_str_port_.end(); port_it++){
				uint32_t n_port = strtoul((*port_it).c_str(), NULL, 10);

				for(vector<string>::iterator ip_it = vec_str_ip_.begin(); 
					ip_it != vec_str_ip_.end(); ip_it++){

					Redis_Struct redis_struct;
					strncpy(redis_struct.ip_, (*ip_it).c_str(), IP_WORD_LEN);
					redis_struct.port_ = n_port;
					redis_struct.redis_ = NULL;
					vec_redis_server_.push_back(redis_struct);
				}
			}
		}	
		~HiRedisDbInterface(){
			for(vector<Redis_Struct>::iterator it = vec_redis_server_.begin(); 
				it != vec_redis_server_.end(); it ++){
				if((*it).redis_ != NULL)
					redisFree((*it).redis_);
			}
			vec_redis_server_.clear();
		}
	private:

		redisContext* get_redis_raw(SvrIpPort& svr_ip_port){
			/*uint16_t total_index = svr_ip_port.ip_index_ + svr_ip_port.port_index_ * ip_num_;
            redisContext *redis = vec_redis_server_[total_index].redis_;
            if(NULL == redis || redis->err){
				if(redis != NULL){
					redisFree(redis);
				}
				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = db_info_struct_.timeout_ * 1000; //usecond
                redis = redisConnectWithTimeout(vec_redis_server_[total_index].ip_,
                    vec_redis_server_[total_index].port_, tv);
				
				vec_redis_server_[total_index].redis_ = redis;

				LOG_DEBUG("redis connect:%s:%d", vec_redis_server_[total_index].ip_, 
					vec_redis_server_[total_index].port_);
            
			}
				
			if (NULL == redis || redis->err){
				if(redis != NULL){
					redisFree(redis);
					vec_redis_server_[total_index].redis_ = NULL;
				}
			}
            
			return vec_redis_server_[total_index].redis_;*/
			
			uint16_t total_index = svr_ip_port.ip_index_ + svr_ip_port.port_index_ * ip_num_;
			struct timeval tv;
			tv.tv_sec = db_info_struct_.timeout_ / 1000; //seconds
			tv.tv_usec = db_info_struct_.timeout_ * 1000 - tv.tv_sec * 1000 * 1000; //useconds
			redisContext *redis = redisConnectWithTimeout(vec_redis_server_[total_index].ip_,
					vec_redis_server_[total_index].port_, tv);

			if(NULL == redis)
				return NULL;

			if(redis->err){
				LOG_ERROR("%s:%"PRIu32" err:%s", vec_redis_server_[total_index].ip_, vec_redis_server_[total_index].port_, redis->errstr);
				redisFree(redis);
				return NULL; 
			}

			redisSetTimeout(redis, tv);
			return redis;
		}

		redisContext* get_redis(uint64_t n_key, SvrIpPort& svr_ip_port){

			get_ip_port(n_key, svr_ip_port);
			return get_redis_raw(svr_ip_port);
		}

	public:
		int s_get(uint16_t type_id, const char* p_str_key, char* &p_result, 
				char& split_char, char& second_split_char, uint64_t n_key){ 
			initialize();

			return get_i(type_id, n_key, 0, p_result, split_char, second_split_char, p_str_key, true);
		}

		int s_set(uint16_t type_id, const char* p_str_key, const char* p_value, uint64_t n_key){
			initialize();
			
			return set_i(type_id, n_key, 0, p_value, p_str_key, true);
		}

        int s_del(uint16_t type_id, const char * p_str_key, uint64_t n_key){
            initialize();

			SvrIpPort svr_ip_port;
			redisContext* redis = get_redis(n_key, svr_ip_port);
			if(NULL == redis)
				return -1;
			
			redisReply *reply = NULL;
			if(db_info_struct_.db_index_ != 0){
				reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
				if(NULL == reply){
					if(redis->err){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						LOG_ERROR("SELECT error:%s", redis->errstr);
						redisFree(redis);
						return -3;
					}

					reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
					if(reply == NULL){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						redisFree(redis);
						return -2;
					}
				}
				freeReplyObject(reply);
			}

            reply = (redisReply*)redisCommand(redis, "DEL %s", p_str_key);

			if(reply == NULL){
				LOG_ERROR("DEL redis reply is NULL!");
				redisFree(redis);
				return -2;
			}

			if(redis->err){
				LOG_ERROR("DEL error:%s", redis->errstr);
				freeReplyObject(reply);
				redisFree(redis);
				return -3;
			}

			freeReplyObject(reply);
			redisFree(redis);

			return 1;

		}

        int set_expire(uint16_t type_id, const char * p_str_key, uint32_t expire_time, uint64_t n_key){
            initialize();

			SvrIpPort svr_ip_port;
			redisContext* redis = get_redis(n_key, svr_ip_port);
			if(NULL == redis)
				return -1;
			
			redisReply *reply = NULL;
			if(db_info_struct_.db_index_ != 0){
				reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
				if(NULL == reply){
					if(redis->err){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						LOG_ERROR("SELECT error:%s", redis->errstr);
						redisFree(redis);
						return -3;
					}

					reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
					if(reply == NULL){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						redisFree(redis);
						return -2;
					}
				}
				freeReplyObject(reply);
			}

            reply = (redisReply*)redisCommand(redis, "EXPIRE %s %d", p_str_key, expire_time);

			if(reply == NULL){
				LOG_ERROR("EXPIRE redis reply is NULL!");
				redisFree(redis);
				return -2;
			}

			if(redis->err){
				LOG_ERROR("SET error:%s", redis->errstr);
				freeReplyObject(reply);
				redisFree(redis);
				return -3;
			}

			freeReplyObject(reply);
			redisFree(redis);

			return 1;

		}


		// deal with item_key by char*
		int get_i(uint16_t type_id, const char* &p_str_key, uint32_t n_item_key, char* &p_result,
				char& split_char, char& second_split_char){
			return 1;
		}

		int get(uint16_t type_id, uint64_t n_key, char* &p_result, 
				char& split_char, char& second_split_char){ 
			initialize();
			return get_i(type_id, n_key, 0, p_result, split_char, second_split_char);
		}
	
		int set_i(uint16_t type_id, uint64_t n_key, uint32_t item_key,
				const char* value, const char* other_key = NULL, bool other_flag = false){ 
		// -1 is NULL, -2 is error, 1 is success, 0 is not exist
		//
			if(value == NULL){
				LOG_ERROR("value is NULL!");
				return -1;
			}

			SvrIpPort svr_ip_port;
			redisContext* redis = get_redis(n_key, svr_ip_port);
			if(NULL == redis)
				return -1;
			
			redisReply *reply = NULL;
			if(db_info_struct_.db_index_ != 0){
				reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
				if(NULL == reply){
					if(redis->err){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						LOG_ERROR("SELECT error:%s", redis->errstr);
						redisFree(redis);
						return -3;
					}

					reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
					if(reply == NULL){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						redisFree(redis);
						return -2;
					}
				}
				freeReplyObject(reply);
			}

			if(other_flag){
				if(other_key == NULL){
					redisFree(redis);
					LOG_ERROR("other key is NULL!");
					return -1;
				}
				reply = (redisReply*)redisCommand(redis, "SET %s %s", other_key, value);
			}else{
				reply = (redisReply*)redisCommand(redis, "SET %"PRIu64" %s", n_key, value);
			}
			

			if(reply == NULL){
				LOG_ERROR("SET redis reply is NULL!");
				redisFree(redis);
				return -2;
			}

			if(REDIS_REPLY_ERROR == reply->type){
				LOG_ERROR("SET reply error:%s", reply->str);
				if(other_flag){
					if(other_key == NULL){
						freeReplyObject(reply);
						redisFree(redis);
						LOG_ERROR("other key is NULL!");
						return -1;
					}
					reply = (redisReply*)redisCommand(redis, "SET %s %s", other_key, value);
				}else{
					reply = (redisReply*)redisCommand(redis, "SET %"PRIu64" %s", n_key, value);
				}
			}
			
			if(reply == NULL){
				LOG_ERROR("SET redis reply is NULL!");
				redisFree(redis);
				return -2;
			}

			if(redis->err){
				LOG_ERROR("SET error:%s", redis->errstr);
				freeReplyObject(reply);
				redisFree(redis);
				return -3;
			}

			if(REDIS_REPLY_ERROR == reply->type){
				LOG_ERROR("SET reply error:%s", reply->str);
				freeReplyObject(reply);
				redisFree(redis);
				return -3;
			}

			freeReplyObject(reply);
			redisFree(redis);

			return 1;

		}

		int get_i(uint16_t type_id, uint64_t n_key, uint32_t item_key,
				char* &p_result, char& split_char, char& second_split_char, 
				const char* other_key = NULL, bool other_flag = false){ 
		// -1 is NULL, -2 is error, 1 is success, 0 is not exist
			SvrIpPort svr_ip_port;
			redisContext* redis = get_redis(n_key, svr_ip_port);
			if(NULL == redis)
				return -1;
			
			// 选定数据库
			redisReply *reply = NULL;
			if(db_info_struct_.db_index_ != 0){
				reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
				if(NULL == reply){
					if(redis->err){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						LOG_ERROR("SELECT redis error:%s", redis->errstr);
						redisFree(redis);
						return -3;
					}

					reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
					if(reply == NULL){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						redisFree(redis);
						return -2;
					}
				}
				freeReplyObject(reply);
			}

			// 发送获取数据命令
			char key_str_temp[WORD_LEN];
			if (other_flag){
				snprintf(key_str_temp, WORD_LEN, "GET %s", other_key);
			}else{
				snprintf(key_str_temp, WORD_LEN, "GET %"PRIu64, n_key);
			}
			reply = (redisReply*)redisCommand(redis, key_str_temp);
			if(NULL == reply){
				if(redis->err){
					LOG_ERROR("GET redis reply is NULL!");
					LOG_ERROR("GET redis error:%s", redis->errstr);
					redisFree(redis);
					return -3;
				}

				reply = (redisReply*)redisCommand(redis, key_str_temp);
				if(reply == NULL){
					LOG_ERROR("GET redis reply is NULL!");
					redisFree(redis);
					return -2;
				}
			}

			if (reply->type != REDIS_REPLY_STRING){
				freeReplyObject(reply);
				redisFree(redis);
				return -3;
			}

			const char* redis_result = reply->str;
			if(NULL == redis_result){
				freeReplyObject(reply);
				redisFree(redis);
				return 0;
			}

			if(db_info_struct_.key_value_type_ == 0 && db_info_struct_.value_string_type_ == 0){
				if(db_info_struct_.compress_flag_){
					uLongf un_compr_len = REDIS_COMPRESS_LEN;
					int err = uncompress((Bytef*)un_compress_[un_compress_index_], &un_compr_len, (Bytef*)redis_result, reply->len);
					if (err != Z_OK)
					{
						LOG_ERROR("uncompress error!:%d", err);
						freeReplyObject(reply);
						redisFree(redis);
						return -1;
					}
					un_compress_[un_compress_index_][un_compr_len] = '\0';
					p_result = un_compress_[un_compress_index_];
					un_compress_index_ ++;
				}else{
					strncpy(un_compress_[un_compress_index_], redis_result, REDIS_COMPRESS_LEN);
					//memcpy(un_compress_[un_compress_index_], redis_result, REDIS_COMPRESS_LEN);
					p_result = un_compress_[un_compress_index_];
					un_compress_index_ ++;
				}
				split_char = db_info_struct_.value_split_first_char_;
				second_split_char = db_info_struct_.value_split_second_char_;
			}

			freeReplyObject(reply);
			redisFree(redis);
			return 1;
		}

		int mget(uint16_t type_id, uint64_t n_keys[], uint32_t key_num, MapMResult& map_result,
				char& split_char, char& second_split_char){
			initialize();
			uint32_t n_item_keys[1];
			if(db_info_struct_.mget_type_ == 1)
				return mget_i_multi(type_id, n_keys, key_num, n_item_keys, 0, map_result,
					split_char, second_split_char); 
			else
				return mget_i(type_id, n_keys, key_num, n_item_keys, 0, map_result, 
					split_char, second_split_char);
		}

		int mget_i_multi(uint16_t type_id, uint64_t n_keys[], uint32_t key_num, 
				uint32_t n_item_keys[], uint32_t item_key_num,
				MapMResult& map_result, char& split_char, char& second_split_char){
			
			//int spend_msec = -1;
            //struct timeval tv_temp;
            //tv_temp = calc_spend_time(tv_temp, "start", spend_msec, true);

			MapSplitIpPort map_ip_port;
			gets_ip_port(n_keys, key_num, map_ip_port);
		
			int num = map_ip_port.size();
			pthread_t thread_arr[num];
			RedisThreadPara redis_thread_para_arr[num];
			
			un_compress_index_  = 0;
			int index = 0;
			int step = db_info_struct_.mget_thread_num_; //这里是最大的一个坑
			//对不起大家了，在redis并行获取过程中，需要限定每个线程最大访问数，主要是
			//因为原有uncompress_这个多维数据是有限制的。

			if(map_ip_port.size() > 0){
				if ((REDIS_COMPRESS_NUM / int(map_ip_port.size())) < step){
					step = REDIS_COMPRESS_NUM / map_ip_port.size();
				}
			//}else{
			//	LOG_DEBUG("%"PRIu32, key_num);
			}
			//上面的逻辑保证不越界

			for(MapSplitIpPort::iterator it = map_ip_port.begin(); it != map_ip_port.end(); it++){

				//SvrIpPort svr_ip_port = (*it).first;	
				//ReqStruct* p_req_struct = &(*it).second;

				redis_thread_para_arr[index].p_redis_db_interface_ = this;
				redis_thread_para_arr[index].p_req_struct_ = new ReqStruct();
				redis_thread_para_arr[index].p_req_struct_->num_ = (*it).second.num_;
				redis_thread_para_arr[index].p_req_struct_->step_ = step;

				for(uint32_t i = 0; i < (*it).second.num_; i ++){
					redis_thread_para_arr[index].p_req_struct_->n_requsts_[i] = (*it).second.n_requsts_[i];
				}

				redis_thread_para_arr[index].p_svr_ip_port_ = new SvrIpPort();
				redis_thread_para_arr[index].p_svr_ip_port_->ip_index_ = (*it).first.ip_index_;
				redis_thread_para_arr[index].p_svr_ip_port_->port_index_ = (*it).first.port_index_;

				redis_thread_para_arr[index].p_vec_result_ = new VecMResult;
				redis_thread_para_arr[index].p_uncompress_ = &this->un_compress_[un_compress_index_];

				un_compress_index_ = un_compress_index_ + step;

				index ++;
			}

			map_ip_port.clear();
			//tv_temp = calc_spend_time(tv_temp, "prepare:", spend_msec);

			//LOG_DEBUG("begin create pthread"); 
			for(int i = 0 ; i < index; i ++){
				pthread_create(&thread_arr[i], NULL, &HiRedisDbInterface::mget_i_internal_pthread, 
						&redis_thread_para_arr[i]);
			}

			for(int i = 0 ; i < index; i ++){
				pthread_join(thread_arr[i], NULL);
			}

			//tv_temp = calc_spend_time(tv_temp, "get process:", spend_msec);

			for(int i = 0; i < index; i ++){
				//LOG_DEBUG("pthread init:%d:spend time:%d:%d:%d", i, redis_thread_para_arr[i].n_spend_time_[0],
						//redis_thread_para_arr[i].n_spend_time_[1], redis_thread_para_arr[i].n_spend_time_[2]); 

				for(VecMResult::iterator it = redis_thread_para_arr[i].p_vec_result_->begin();
						it != redis_thread_para_arr[i].p_vec_result_->end(); it++){
					map_result.insert(MapMResult::value_type((*it).first, (*it).second));
				}

				redis_thread_para_arr[i].p_vec_result_->clear();
				delete redis_thread_para_arr[i].p_vec_result_;

				delete redis_thread_para_arr[i].p_req_struct_;
				delete redis_thread_para_arr[i].p_svr_ip_port_;
			}

			split_char = db_info_struct_.value_split_first_char_;
			second_split_char = db_info_struct_.value_split_second_char_;

			//tv_temp = calc_spend_time(tv_temp, "process end:", spend_msec);
			return 1;
		}

		static void* mget_i_internal_pthread(void* thread_para){
			
			RedisThreadPara* p_thread_para = (RedisThreadPara*)thread_para;

			if(NULL == p_thread_para || NULL == p_thread_para->p_redis_db_interface_){
				return NULL;
			}
			p_thread_para->p_redis_db_interface_->mget_i_internal(p_thread_para->p_svr_ip_port_,
					p_thread_para->p_req_struct_, p_thread_para->p_vec_result_, 
					p_thread_para->p_uncompress_, p_thread_para->n_spend_time_);

			return NULL;
		}

		int mget_i_internal(SvrIpPort* &p_svr_ip_port,  ReqStruct* &p_req_struct,
				VecMResult*& p_vec_result, char un_compress[][COMPRESS_LEN], int spend_time[]){

			//struct timeval tv_temp;
			//struct timeval tv_begin;
			//gettimeofday(&tv_temp, NULL);
			//tv_begin = tv_temp;

			if (NULL == p_req_struct || NULL == p_svr_ip_port || NULL == p_vec_result)
				return -1;

			if(p_req_struct->num_ == 0)
				return -1;
			
			redisContext* redis = get_redis_raw(*p_svr_ip_port);
			if(redis == NULL){
				LOG_DEBUG("redis is NULL!");
				return -1;
			}
				
			// 选定数据库
			redisReply *reply = NULL;
			if(db_info_struct_.db_index_ != 0){
				reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
				if(NULL == reply){
					if(redis->err){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						LOG_ERROR("SELECT error:%s", redis->errstr);
						redisFree(redis);
						return -3;
					}

					reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
					if(reply == NULL){
						LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
						redisFree(redis);
						return -2;
					}
				}
				freeReplyObject(reply);
			}

			// 发送获取数据命令
			//gettimeofday(&tv_temp, NULL);
        	//int sec_spend_time_0 = (tv_temp.tv_sec - tv_begin.tv_sec) * 1000 + (tv_temp.tv_usec - tv_begin.tv_usec) / 1000;
			//tv_begin = tv_temp;
			//printf("%d", sec_spend_time_0);
			//spend_time[0] = sec_spend_time_0;

			int un_compress_index = 0;

			char mget[REDIS_COMPRESS_LEN];
			char* p_mget = mget;
			int len = snprintf(p_mget, WORD_LEN, "%s",  "MGET");
			p_mget = p_mget + len;

			uint32_t i = 0;
			for(i = 0; i < p_req_struct->num_; i ++){
				len = snprintf(p_mget, WORD_LEN, " %"PRIu64, p_req_struct->n_requsts_[i]);
				p_mget = p_mget + len;
			}
			*p_mget = '\0';

			reply = (redisReply*)redisCommand(redis, mget);
			
			//gettimeofday(&tv_temp, NULL);
        	//int sec_spend_time_1 = (tv_temp.tv_sec - tv_begin.tv_sec) * 1000 + (tv_temp.tv_usec - tv_begin.tv_usec) / 1000;
			//tv_begin = tv_temp;
			//printf("%d", sec_spend_time_1);
			//spend_time[1] = sec_spend_time_1;


			if(reply == NULL){
				uint16_t total_index = p_svr_ip_port->ip_index_ + p_svr_ip_port->port_index_ * ip_num_;
				char* ip = vec_redis_server_[total_index].ip_;
				uint32_t port = vec_redis_server_[total_index].port_;
				LOG_DEBUG("err:%d, errstr:%s", redis->err, redis->errstr);
				LOG_DEBUG("MGET %s %"PRIu32" reply is NULL %s", ip, port, mget);
				redisFree(redis);
				return -1;
			}

			int step = p_req_struct->step_;
			for(i = 0; i < reply->elements; i ++) {
				char *result = reply->element[i]->str;
				long result_len = reply->element[i]->len;

				if (result_len == 0 || result == NULL)
					continue;

				if(un_compress_index >= step){
					LOG_DEBUG("out of num!");
					break;
				}
				if(db_info_struct_.key_value_type_ == 0 && db_info_struct_.value_string_type_ == 0){
					if(db_info_struct_.compress_flag_){
						uLongf un_compr_len = REDIS_COMPRESS_LEN;
						try{
							int err = uncompress((Bytef*)un_compress[un_compress_index], &un_compr_len, 
								(Bytef*)result, result_len);

							if (err != Z_OK){
								LOG_DEBUG("%"PRIu64":%s", p_req_struct->n_requsts_[i], result);
								LOG_DEBUG("uncompress error!:%d:%d:%d:%d:%"PRIu64":%d", sizeof(char), sizeof(Bytef), 
										err, un_compress_index, p_req_struct->n_requsts_[i], result_len);
								continue;
							}
						}catch(...){
							LOG_ERROR("uncompress error!");
						}

						un_compress[un_compress_index][un_compr_len] = '\0';
						char* p_result = un_compress[un_compress_index];
						un_compress_index ++;
			
						p_vec_result->push_back(pair<uint64_t, const char*>(p_req_struct->n_requsts_[i], p_result));

					}else{
						strncpy(un_compress[un_compress_index], result, REDIS_COMPRESS_LEN);
						//memcpy(un_compress[un_compress_index], result, REDIS_COMPRESS_LEN);
						char* p_result = un_compress[un_compress_index];
						un_compress_index ++;

						p_vec_result->push_back(pair<uint64_t, const char*>(p_req_struct->n_requsts_[i], p_result));
					}
				}
				else{
					strncpy(un_compress[un_compress_index], result, REDIS_COMPRESS_LEN);
					//memcpy(un_compress[un_compress_index], result, REDIS_COMPRESS_LEN);
					char* p_result = un_compress[un_compress_index];
					un_compress_index++;

					p_vec_result->push_back(pair<uint64_t, const char*>(p_req_struct->n_requsts_[i], p_result));
				}
			}
			freeReplyObject(reply);
			redisFree(redis);

			//gettimeofday(&tv_temp, NULL);
        	//int sec_spend_time_2 = (tv_temp.tv_sec - tv_begin.tv_sec) * 1000 + (tv_temp.tv_usec - tv_begin.tv_usec) / 1000;
			//printf("%d", sec_spend_time_2);
			//spend_time[2] = sec_spend_time_2;
			//tv_begin = tv_temp;

			return 1;

		}

		int mget_i(uint16_t type_id, uint64_t n_keys[], uint32_t key_num, 
				uint32_t n_item_keys[], uint32_t item_key_num,
				MapMResult& map_result, char& split_char, char& second_split_char){

			MapSplitIpPort map_ip_port;
			gets_ip_port(n_keys, key_num, map_ip_port);
	
			for(MapSplitIpPort::iterator it = map_ip_port.begin(); it != map_ip_port.end(); it++){

				SvrIpPort svr_ip_port = (*it).first;	
				ReqStruct& req_struct = (*it).second;

				if(req_struct.num_ == 0)
					continue;

				redisContext* redis = get_redis_raw(svr_ip_port);
                if(redis == NULL){
					LOG_DEBUG("redis is NULL!");
					continue;
				}
				
				// 选定数据库
				redisReply *reply = NULL;
				if(db_info_struct_.db_index_ != 0){
					reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
					if(NULL == reply){
						if(redis->err){
							LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
							LOG_ERROR("SELECT error:%s", redis->errstr);
							redisFree(redis);
							return -3;
						}

						reply = (redisReply*)redisCommand(redis, "SELECT %d", db_info_struct_.db_index_);
						if(reply == NULL){
							LOG_ERROR("SELECT %d redis reply is NULL!", db_info_struct_.db_index_);
							redisFree(redis);
							return -2;
						}
					}
					freeReplyObject(reply);
				}

				// 发送获取数据命令
				char mget[REDIS_COMPRESS_LEN];
				char* p_mget = mget;
				int len = snprintf(p_mget, WORD_LEN, "%s",  "MGET");
				p_mget = p_mget + len;

				uint32_t i = 0;

				for(i = 0; i < req_struct.num_; i ++){
					len = snprintf(p_mget, WORD_LEN, " %"PRIu64, req_struct.n_requsts_[i]);
					p_mget = p_mget + len;
				}

				*p_mget = '\0';

				reply = (redisReply*)redisCommand(redis, mget);
				
				if(reply == NULL){
					uint16_t total_index = svr_ip_port.ip_index_ + svr_ip_port.port_index_ * ip_num_;
					char* ip = vec_redis_server_[total_index].ip_;
					uint32_t port = vec_redis_server_[total_index].port_;
					LOG_DEBUG("MGET %s %"PRIu32" reply is NULL %s", ip, port, mget);
					redisFree(redis);
					continue;
				}

				for(i = 0; i < reply->elements; i ++) {
					char *result = reply->element[i]->str;
					long len = reply->element[i]->len;

					if (len == 0 || result == NULL)
						continue;

                    if(un_compress_index_ >= REDIS_COMPRESS_NUM){
						LOG_DEBUG("out of num!");
                        break;
					}
					if(db_info_struct_.key_value_type_ == 0 && db_info_struct_.value_string_type_ == 0){
						if(db_info_struct_.compress_flag_){
							uLongf un_compr_len = REDIS_COMPRESS_LEN;
							try{
								int err = uncompress((Bytef*)un_compress_[un_compress_index_], &un_compr_len, 
									(Bytef*)result, len);

								if (err != Z_OK){
									LOG_DEBUG("%"PRIu64":%s", req_struct.n_requsts_[i], result);
									LOG_DEBUG("uncompress error!:%d:%d", err, un_compress_index_);
									un_compress_index_ ++;
									continue;
								}
							}catch(...){
								LOG_ERROR("uncompress error!");
							}

							un_compress_[un_compress_index_][un_compr_len] = '\0';
							char* p_result = un_compress_[un_compress_index_];
							un_compress_index_ ++;
						
							map_result.insert(MapMResult::value_type(req_struct.n_requsts_[i], p_result));

						}else{
							strncpy(un_compress_[un_compress_index_], result, REDIS_COMPRESS_LEN);
							//memcpy(un_compress_[un_compress_index_], result, REDIS_COMPRESS_LEN);
							char* p_result = un_compress_[un_compress_index_];
							un_compress_index_ ++;

                            map_result.insert(MapMResult::value_type(req_struct.n_requsts_[i], p_result));
						}
					}
					else{
						strncpy(un_compress_[un_compress_index_], result, REDIS_COMPRESS_LEN);
						//memcpy(un_compress_[un_compress_index_], result, REDIS_COMPRESS_LEN);
                        char* p_result = un_compress_[un_compress_index_];
						un_compress_index_ ++;

                        map_result.insert(MapMResult::value_type(req_struct.n_requsts_[i], p_result));
					}
				}
				freeReplyObject(reply);
				redisFree(redis);
            	///tv_temp = calc_spend_time(tv_temp, "mget", spend_msec);
			}
			
			split_char = db_info_struct_.value_split_first_char_;
			second_split_char = db_info_struct_.value_split_second_char_;

			return 1;

		}

	private:
		vector<Redis_Struct> vec_redis_server_;	
};

#endif
