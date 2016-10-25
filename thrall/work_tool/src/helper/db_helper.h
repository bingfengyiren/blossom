#ifndef _DB_HELPER_H
#define _DB_HELPER_H
//C header
#include<cstdint>
//C++ header
#include<iosteam>

class Db_helper
{
public:
	Db_helper();
	~Db_helper();

public:
	/*************************************************
	* @get string data from redis
	* @param get_key [IN] the key of redis
	* @param get_value [OUT] 
	* @param prefer_redis [IN] the first choice
	* @param reserve_reids [IN] the bak redis
	* @return the status of this function
	**************************************************/
	int32_t GetStrFromRedis(const string get_key,
							const string& get_value,
						 	HiRedisDbInterface* prefer_redis,
						 	HiRedisDbInterface* reserve_redis = NULL);

	/*************************************************
	* @brief set string data to redis
	* @param set_key [IN] the key for redis
	* @param set_value [IN] the value will store to redis
	* @param prefer_redis [IN] the first choice
	* @param reserve_reids [IN] the bak redis
	* @return the status of this function
	**************************************************/
	int32_t SetStrToRedis(const string set_key,
						  string set_value,
						  HiRedisDbInterface* prefer_redis,
						  HiRedisDbInterface* reserve_redis= NULL);

	/*************************************************
	* @brief get hash data from redis
	**************************************************/
	int32_t GetHashFromRedis(const string get_key,
							 const vector<string> field_vtr,
						  	 HiRedisDbInterface* prefer_redis,
						  	 HiRedisDbInterface* reserve_redis= NULL);
							
	/*************************************************
	* @brief set hash data to redis
	**************************************************/
	int32_t SetHashToRedis(const string set_key,
						   const map<pair<string,string>> field_val,
						   HiRedisDbInterface* prefer_redis,
						   HiRedisDbInterface* reserve_reids = NULL);
}

#endif
