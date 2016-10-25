RedisDbInterface* redisRead#ifndef _THREALL_WORKINTERFACE_H
#define _THREALL_WORKINTERFACE_H

//C header
#include <cstring>

//C++ STL header
#include <string>
#include <map>
#include <chrono>
using namespace std;

//local header
#include "work_interface.h"
#include "ini_file.h"
#include "woo/log.h"
#include "json.h"
//#include "helper/db_helper.h"
#include "helper/ret_helper.h"

class Thrall_Manager_WorkInterface: public WorkInterface
{
// construct & destructor
public:
	Thrall_Manager_WorkInterface(DbCompany*& p_db_company, int interface_id);
	~Thrall_Manager_WorkInterface();
private:
	/*************************************************
	* @brief set the rule to redis & broadcast to recom server 
	* @param reqJson [IN] request parameter
	* @param respJson [OUT] return result
	* @param redisRead [IN] read redis resource
	* @param redisWrite [OUT] write redis resource
	* @param recomServ [IN] broadcaset to server
	**************************************************/
	int32_t set_rule(Json::Value& reqJson,
			Json::Value& respJson,
			HiRedisDbInterface* redisRead,
			HiRedisDbInterface* redisWrite,
			WooDbInterface* recomServ)  

	/*************************************************
	* @brief get the rule from redis 
	* @param reqJson [IN] request parameter
	* @param respJson [OUT] return result
	* @param redisRead [IN] read redis resource
	* @param redisWrite [OUT] write redis resource
	* @param recomServ [IN] broadcaset to server
	**************************************************/
	int32_t get_rule(Json::Value& reqJson,
			Json::Value& respJson,
			HiRedisDbInterface* redisRead,
			HiRedisDbInterface* redisWrite,
			WooDbInterface* recomServ)

	/*************************************************
	* @brief set the model to redis & broadcast to recom server 
	* @param reqJson [IN] request parameter
	* @param respJson [OUT] return result
	* @param redisRead [IN] read redis resource
	* @param redisWrite [OUT] write redis resource
	* @param recomServ [IN] broadcaset to server
	**************************************************/
	int32_t get_model(Json::Value& reqJson,
			Json::Value& respJson,
			HiRedisDbInterface* redisRead,
			HiRedisDbInterface* redisWrite,
			WooDbInterface* predictServ)

	/*************************************************
	* @brief get the model from redis
	* @param reqJson [IN] request parameter
	* @param respJson [OUT] return result
	* @param redisRead [IN] read redis resource
	* @param redisWrite [IN] write redis resource
	* @param recomServ [IN] broadcaset to server
	**************************************************/
	int32_t set_model(Json::Value& reqJson,
			Json::Value& respJson,
			HiRedisDbInterface* redisRead,
			HiRedisDbInterface* redisWrite,
			WooDbInterface* predictServ)

	/*************************************************
	* @brief get the rule list on server
	* @param reqJson [IN] request parameter
	* @param respJson [OUT] return result
	* @param redisRead [IN] read redis resource
	* @param redisWrite [OUT] write redis resource
	* @param recomServ [IN] broadcaset to server
	**************************************************/
	int32_t get_rule_list(Json::Value& reqJson,
			Json::Value& respJson,
			HiRedisDbInterface* redisRead,
			HiRedisDbInterface* redisWrite,
			WooDbInterface* predictServ)

	/*************************************************
	* @brief set the model list on server 
	* @param reqJson [IN] request parameter
	* @param respJson [OUT] return result
	* @param redisRead [IN] read redis resource
	* @param redisWrite [OUT] write redis resource
	* @param recomServ [IN] broadcaset to server
	**************************************************/
	int32_t get_model_list(Json::Value& reqJson,
			Json::Value& respJson,
			HiRedisDbInterface* redisRead,
			HiRedisDbInterface* redisWrite,
			WooDbInterface* predictServ)
	
	int32_t check_rule(Json::Value& reqJson);

	int32_t get_str_from_redis(string& strData,
			string sKey,
			HiRedisDbInterface* prefer_redis,
			HiRedisDbInterface* reserve_redis = NULL);

	int32_t set_str_to_redis(string& strData,
			string& sKey,
			HiRedisDbInterface* prefer_redis,
			HiRedisDbInterface* reserve_redis = NULL);
public:
	// start here
	virtual int work_core(json_object* req_json, char* &p_out_string,int& n_out_len,int64_t req_id);
}
