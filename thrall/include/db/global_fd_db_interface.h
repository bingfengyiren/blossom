#ifndef _GLOBAL_FEATUREDATA_DB_INTERFACE_HEADER_
#define _GLOBAL_FEATUREDATA_DB_INTERFACE_HEADER_
#include <stdlib.h> 
#include <vector>
#include <string>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

//using namespace std;
#define MAX_READ_BUFFER_SIZE 2000
#define MAX_SUB_BUFFER_SIZE 400
#define MAX_UNIT_LENGTH 4

struct UserFeature {
	
	int32_t corps[MAX_UNIT_LENGTH];
	int32_t schools[MAX_UNIT_LENGTH];
	
	uint64_t uid;	

	uint32_t fanscount;
	int32_t category1;
	int32_t category2;
	int32_t category3;
	
	uint16_t province;
	uint16_t city;
	uint16_t followcount;
	uint16_t weibocount;
	uint16_t regday;
	uint16_t age;			// 年龄8位应该就够了？
    int16_t yesrpm1;
	int16_t yesrpm2;
	int16_t yesrpm3;
	
	int8_t viplevel;
	int8_t viptype;

	/* 使用位域 */
	int8_t school_count:4;	// 学校和公司的数量不会都超过4个，每个需要三位
	int8_t corp_count:4;
	
	// 下面是四个bool类型
	int8_t gender:1;			// 性别只需要1位
	int8_t ismember:1;		// 是否会员只需要1位
	int8_t isred:1;			// 是否加红只需要1位
	int8_t isvip:1;			// 是否vip只需要1位
    /* 总共使用了两个字节，之前是6字节 */
};

const int BINARY_BYTES = sizeof(UserFeature);		// 74字节？

class GlobalFeatureDataDbInterface : public GlobalDbInterface{
	public:

		GlobalFeatureDataDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct){
		}
		~GlobalFeatureDataDbInterface(){
			for(__gnu_cxx::hash_map<uint64_t,UserFeature *>::iterator it = set_global_data_.begin();it!= set_global_data_.end();it++){
				if (NULL != it->second)
					delete it->second;
			}
			set_global_data_.clear();
		}
	private:
		 __gnu_cxx::hash_map<uint64_t,UserFeature *> set_global_data_;
	public:
		bool is_exist(uint64_t id){
			__gnu_cxx::hash_map<uint64_t,UserFeature *>::iterator it = set_global_data_.find(id);
			if(it != set_global_data_.end())
				return true;
			else
				return false;
		}
/*
		bool getfeature_test(uint64_t uid,char *featureinfo){
			__gnu_cxx::hash_map<uint64_t,UserFeature *>::iterator it = set_global_data_.find(uid);
			if(it != set_global_data_.end()){
				sprintf(featureinfo,"uid:%lu,corp_count:%d,school_count:%d\n",it->second->uid,it->second->corp_count, it->second->school_count);
				//LOG_DEBUG("getdata:%lu\n",uid);
				return true;
			}
			else{
				//LOG_DEBUG("getdata not exist:%lu\n",uid);
				return false;
			}
		}
*/
		UserFeature * getfeature(uint64_t uid){
			__gnu_cxx::hash_map<uint64_t,UserFeature *>::iterator it = set_global_data_.find(uid);
			if(it != set_global_data_.end()){
				return it->second;
			}
			else{
				return NULL;
			}
		}

		char *get_sep(char *start, char **next, char sep){
			if (start == NULL){
				return NULL;
			}
			char *s = start;
			char *p = strchr(s, sep);
			if (p != NULL){
				*next = p + 1;
				*p = '\0';
			}else{
				*next = NULL;
			}
			return s;
		}
		
		uint8_t getinfo(char *info,int32_t *info_value){
			char *info_unit = info;
			char *info_unit_next = info;
			uint8_t length = 0;			
			while ((info_unit = get_sep(info_unit_next, &info_unit_next, '/')) != NULL && length < MAX_UNIT_LENGTH){
				if(info_unit == NULL){
					break;
				}
				int info_unit_value = atoi(info_unit);
				if(info_unit_value == 0){
					break;
				}
				info_value[length++] = info_unit_value;
			}
			return length;
		}

		int load_db_config(){
			char* db_file_name = global_db_info_struct_.db_file_name_;
			if(NULL == db_file_name)
				return -1;

			FILE *fd = NULL;  
			char buf[MAX_READ_BUFFER_SIZE];  

			char corps[MAX_SUB_BUFFER_SIZE];
			char schools[MAX_SUB_BUFFER_SIZE];
			
			fd = fopen(db_file_name, "r");  
			if(NULL == fd){
				LOG_ERROR("%s read is error!", db_file_name);
				return 0;         
			}  

			uint32_t load_count = 0;
			while(fgets(buf, MAX_READ_BUFFER_SIZE , fd)){
				UserFeature *userfeature = new UserFeature();
				if (NULL == userfeature){
					break;
				}
				int8_t gender, ismember, isred, isvip;
				sscanf(buf, "%"SCNu64" %"SCNd8" %"SCNu16" %"SCNu16" %"SCNu16" %"SCNu16" %s %s %"SCNu16" %"SCNu32" %"SCNu16" %"SCNd8" %"SCNd8" %"SCNd8" %"SCNd8" %"SCNd8" %"SCNd16" %"SCNd16" %"SCNd16" %"SCNd32" %"SCNd32" %"SCNd32"\n",
						&userfeature->uid, &gender, &userfeature->province, 
						&userfeature->city, &userfeature->regday, &userfeature->age, 
						corps, schools, &userfeature->followcount, &userfeature->fanscount, 
						&userfeature->weibocount, &ismember, &userfeature->viplevel, 
						&isred, &isvip, &userfeature->viptype, &userfeature->yesrpm1, &userfeature->yesrpm2, &userfeature->yesrpm3, 
						&userfeature->category1, &userfeature->category2, &userfeature->category3);
				userfeature->gender = (gender > 0 ? 1 : 0);
				userfeature->ismember = (ismember > 0 ? 1: 0);
				userfeature->isred = (isred > 0 ? 1 : 0);
				userfeature->isvip = (isvip > 0 ? 1 : 0);
				userfeature->corp_count = getinfo(corps,userfeature->corps);
				userfeature->school_count = getinfo(schools,userfeature->schools);
				set_global_data_.insert(std::make_pair<uint64_t,UserFeature *>(userfeature->uid,userfeature));
				
				if(load_count%1000000 == 0){
					LOG_DEBUG("have loaded %d feature data:%s,uid:%lu,isvip:%d\n",load_count,buf,userfeature->uid,userfeature->isvip);
				}

				load_count++;
			}  
			fclose(fd);  

			return 1;
		}
	
		// read binary file
		/*
		int load_db_config(bool binary){
			char* db_file_name = global_db_info_struct_.db_file_name_;
			if(NULL == db_file_name)
				return -1;

			char buf[MAX_READ_BUFFER_SIZE];  

			char corps[MAX_SUB_BUFFER_SIZE];
			char schools[MAX_SUB_BUFFER_SIZE];
		    
			FILE* fd = NULL;
			fd = fopen(db_file_name, "rb");		// 以二进制形式打开
			if(NULL == fd){
				LOG_ERROR("%s read is error!", db_file_name);
				return 0;         
			}  

			uint32_t load_count = 0;
			while(fgets(buf, MAX_READ_BUFFER_SIZE , fd)){
				UserFeature *userfeature = new UserFeature();
				if (NULL == userfeature){
					break;
				}
				load_count++;
			}  
			fclose(fd);  
			return 1;
		}*/
};

#endif
