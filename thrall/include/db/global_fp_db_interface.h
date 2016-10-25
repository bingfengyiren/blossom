#ifndef _GLOBAL_FP_DB_INTERFACE_HEADER_
#define _GLOBAL_FP_DB_INTERFACE_HEADER_
#include <stdlib.h> 
#include <vector>
#include <string>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"
#include "api_featurepool.h"

using namespace std;

class GlobalFpDbInterface : public GlobalDbInterface{
	public:
		GlobalFpDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct){
		}
		~GlobalFpDbInterface(){
		}
	private:
		ApiFeaturePool api_fp_;
	public:
		bool is_exist(uint64_t id){
			return false;
		}

		bool is_exist(uint64_t id, const char* dbid){

			bool flag = false;
			api_fp_.contains(flag, (MAIN_KEY_TYPE)id, dbid);

			return flag;
		}

		int size(const char* dbid){
			int n_size = 0;
			api_fp_.size(n_size, dbid);
			return n_size;
		}

		//get(ITEM_VALUE_TYPE& return_value, MAIN_KEY_TYPE key,
		//        ITEM_KEY_TYPE code, const char* dbid)
		bool get_value(uint64_t& return_value, uint64_t id, 
				uint64_t item_id, const char* dbid){
			ITEM_VALUE_TYPE item_value = 0;
			int flag = api_fp_.get(item_value, 
					(MAIN_KEY_TYPE)id, (ITEM_KEY_TYPE)item_id, dbid);

			return_value = item_value;
			if(flag >= 1)
				return true;
			else
				return false;
		}

		//gets(CodeItemMap& map_return_value, MAIN_KEY_TYPE key,
		//        const ItemVec& vec_code, const char* dbid)
		bool gets_value(std::map<uint64_t, uint64_t>& map_return_value, uint64_t key,
				const std::vector<uint64_t>& vec_item_key, const char* dbid){
			
			int flag = api_fp_.gets(map_return_value, (MAIN_KEY_TYPE)key,
					vec_item_key, dbid);

			return flag;
		}

		//mgets(KeyCodeItemMap& map_return_value, const KeyVec& vec_key,
		//        const ItemVec& vec_code, const char* dbid)
		bool mgets_value(KeyCodeItemMap& map_return_value, const std::vector<uint64_t>& vec_key,
				const std::vector<uint64_t>& vec_item_key, const char* dbid){
			int flag = api_fp_.mgets(map_return_value, vec_key,
					vec_item_key, dbid);
			if(flag >= 1)
				return true;
			else
				return false;
		}

		//mgets_i(KeyCodeItemMap& map_return_value,
		//        const KeyItemVec& key_item_vec, const char* dbid)
		bool mgets_i_value(KeyCodeItemMap& map_return_value,
				const KeyItemVec& key_item_vec, const char* dbid){
			
			int flag = api_fp_.mgets_i(map_return_value, key_item_vec, dbid);
			if(flag >= 1)
				return true;
			else
				return false;

		}

		int load_db_config(){
			char* db_file_name = global_db_info_struct_.db_file_name_;
			if(NULL == db_file_name)
				return -1;

			api_fp_.initialize(db_file_name);
			return 1;
		}
};

#endif
