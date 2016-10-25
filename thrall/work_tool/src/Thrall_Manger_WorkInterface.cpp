#include "Thrall_Manger_WorkInterface.h"

Thrall_Manager_WorkInterface::Thrall_Manger_WorkInterface(DbCompany*& p_db_company, int interface_id):WorkInterface(p_db_company,interface_id)
{	

}

Thrall_Manager_WorkInterface::~Thrall_Manager_WorkInterface()
{

}

int32_t Thrall_Manger_WorkInterface::work_core(json_object* req_json,char* &p_out_string,int& n_out_len,int64_t req_id)
{
	int32_t iRet = 0;
	char* req_json_str = (char*)json_object_to_json_string(req_json);

	//get request para
	Json::Value value;
	Json::Value res_value;
	Json::Reader reader;
	Json::FastWriter writer;

	reader.parse(req_json_str,value);
	string cmd = value["cmd"].asString();

	transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);

	//get db handler
	HiRedisDbInterface* redisRead = (HiRedisDbInterface*)GetDbInterface("MANAGER_REDIS_READ");
	HiRedisDbInterface* redisWrite = (HiRedisDbInterface*)GetDbInterface("MANAGER_REDIS_WRITE");
	WooDbInterface* recomServ = (WooDbInterface*)GetDbInterface("RECOM_SERV");
	WooDbInterface* predictServ = (WooDbInterface*)GetDbInterface("PREDICT_SERV");

	try
	{
		if(0 == cmd.compare("set_rule"))
		{
			iRet = set_rule(value,res_value,redisRead,redisWrite,recomServ);
		}else if(0 == cmd.compare("get_rule"))
		{
			iRet = get_rule(value,res_value,redisRead,redisWrite,recomServ);
		}else if(0 == cmd.compare("set_model"))
		{
			iRet = set_model(value,res_value,redisRead,redisWrite,predictServ);
		}else if(0 == cmd.compare("get_model"))
		{
			iRet = get_model(value,res_value,redisRead,redisWrite,predictServ);
		}else if(0 == cmd.compare("get_rule_list"))
		{
			iRet = get_rule_list(value,res_value,redisRead,redisWrite,recomServ)
		}else if(0 == cmd.compare("get_model_list"))
		{
			iRet = get_model_list(value,res_value,redisRead,redisWrite,predictServ)		
		}else
		{
			sprintf(p_out_string,"{\"code\":%d,\"msg\":\"unkown command: %s\"}",iRet,cmd);
		}
		if(iRet != 0)
		{
			sprintf(p_out_string,"{\"code\":%d,\"msg\":\"execute command: %s error\"}",iRet,cmd);
		}else
		{
			Json::Value root;
			root["code"] = iRet;
			root["msg"] = "OK";
			root["payload"] = res_value;
			strncpy(p_out_string,root.c_str(),sizeof(),sizeof(root.c_str()));	
		}
	catch(...)
	{
		sprintf(p_out_string,"{\"code\":%d,\"msg\":\"unkown command: %s\"}",iRet);
	}
}

int32_t Thrall_Manager_WorkInterface::set_rule(
				Json::Value& reqJson,
				Json::Value& respJson,
				HiRedisDbInterface* redisRead,
				HiRedisDbInterface* redisWrite,
				WooDbInterface* recomServ)  
{
	int32_t iRet = _OK;
	respJson.clear();
	
	base::Timer timer;
	timer.Start();
	//字段完整性检查
	if(reqJson["body"].isNull())
	{
		iRet = _REQUEST_IS_IMCOMPLETED;
	}else if(reqJson["body"]["rule"].isNull())
	{
		iRet = _REQUEST_IS_IMCOMPLETED;
	}else if(reqJson["body"]["business_id"].isNull())
	{
		iRet = _REQUEST_IS_IMCOMPLETED;
	}
	//数据正确性检查
	else 
	{
		string business_id = reqJson["body"]["business_id"].asString();
		Json::Value ruleJson = reqJson["body"]["rule"];
		string ruleStr = ruleJson.asString();
		
		if(business_id == "")
		{
			iRet = _REQUEST_VAL_INNORMAL;
		}
		iRet = check_rule(ruleJson);
		if(iRet == _OK)
		{
			string sKey = "RULES_ALL";
			string ruleListOld;  
			string ruleListNew;
			iRet = get_str_from_redis(ruleListOld,sKey,prefer_redis_r,reserve_redis_r);
			if(iRet == _OK)
			{
				if(string::npos != ruleOld.find(business_id))
				{
					ruleListNew = ruleListOld;
				}
			}
			//1.get rule list & update it
			//2.set rule to redis
		}
	}
	respJson["cmd"] =  reqJson["cmd"].asString();
	respJson["code"] = iRet;
	respJson["msg"] = RetHelper::GetInstance()->Msg(iRet);
	timer.Stop();
	
	if(iRet == _OK)
	{
		LOG_INFO("SERV:manager`CMD:%s`RET_CODE:%d`BUSINESS:%s`TOTAL_TIME:%s",
				 reqJson["cmd"].asString(),
				 iRet,
				 reqJson["business_id"].asString(),
				 timer.GetTotal_asMSec().c_str())
	}else
	{
		LOG_ERROR("SERV:manager`CMD:%s`RET_CODE:%d`RET_MSG:%s`TOTA_TIME:%s",
					reqJson["cmd"].asString(),
					iRet,
					respJson["msg"],
					timer.GetTotal_asMSec().c_str())
	}
	return iRet;
}

int32_t Thrall_Manager_WorkInterface::get_rule(
				Json::Value& reqJson,
				Json::Value& respJson,
				HiRedisDbInterface* redisRead,
				HiRedisDbInterface* redisWrite,
				WooDbInterface* recomServ)  
{
}


int32_t Thrall_Manger_WorkInterface::get_model(Json::Value& reqJson,
				Json::Value& respJson,
				HiRedisDbInterface* redisRead,
				HiRedisDbInterface* redisWrite,
				WooDbInterface* predictServ)
{
	int32_t iRet;
	base::Timer timer;
	respJson.clear()
	
	timer.Start();
	if(reqJson["body"].isNull())
	{
		iRet = _REQUEST_IS_IMCOMPLETED;
	}else if(reqJson["body"]["model_id"].isNull())
	{
		iRet = _REQUEST_IS_IMCOMPLETED;
	}else
	{
		int32_t model_id = reqJson["body"]["model_id"].asInt();
		if(model_id < 0)
		{
			iRet = _REQUEST_VAL_INNORMAL;
		}else
		//
		{
				
		}
	}
		
}


int32_t Thrall_Manger_WorkInterface::set_model(Json::Value& reqJson,
				Json::Value& respJson,
				HiRedisDbInterface* redisRead,
				HiRedisDbInterface* redisWrite,
				WooDbInterface* predictServ)
{

}


int32_t Thrall_Manger_WorkInterface::get_rule_list(Json::Value& reqJson,
				Json::Value& respJson,
				HiRedisDbInterface* redisRead,
				HiRedisDbInterface* redisWrite,
				WooDbInterface* predictServ)
{
	int iRet = _OK;
	base::Timer timer;
	respJson.clear();

	timer.Start();
	string rule_all_str = "";
	iRet = get_str_from_redis(rule_all_str,"RULES_ALL",redisRead,redisWrite);
	if(iRet == _OK)
	{	string sSep = ",";
		base::StringArray* rule_all_array = new base::StringArray(rule_all_str.c_str(),sSep.c_str());
		respJson["rules_list"].resize(0)
		for(int32_t idx =0 ;idx < rule_all_array->Count();++idx)
		{
			respJson["rules_list"].append(rule_all_array.GetString(idx).c_str());
		}
	}
	timer.Stop();

	respJson["req_cmd"] = reqJson["cmd"].asString();
	respJson["ret_code"] = iRet;
	respJson["ret_msg"] = RetHelper::GetInstance()->Msg(iRet);

	//log	
	string logStr = RetHelper::GetInstance()->ReturnLog("manager",respJson["req_cmd"],respJson["ret_code"],reqJson["business_id"],respJson["ret_msg"],timer.GetTotal_asMSec());
	if(iRet == _OK) 
		LOG_INFO(logStr);
	else
		LOG_ERROR(logStr);
}

int32_t Thrall_Manger_WorkInterface::get_model_list(Json::Value& reqJson,
				Json::Value& respJson,
				HiRedisDbInterface* redisRead,
				HiRedisDbInterface* redisWrite,
				WooDbInterface* predictServ)
{
	int iRet = _OK;
	base::Timer timer;
	respJson.clear();

	timer.Start();
	string model_all_str = "";
	iRet = get_str_from_redis(model_all_str,"MODELS_ALL",redisRead,redisWrite);
	if(iRet == _OK)
	{	string sSep = ",";
		base::StringArray* model_all_array = new base::StringArray(model_all.c_str(),sSep.c_str());
		respJson["model_list"].resize(0)
		for(int32_t idx =0 ;idx < model_all_array->Count();++idx)
		{
			respJson["model_list"].append(model_all_array.GetString(idx).c_str());
		}
	}
	timer.Stop();

	respJson["req_cmd"] = reqJson["cmd"].asString();
	respJson["ret_code"] = iRet;
	respJson["ret_msg"] = RetHelper::GetInstance()->Msg(iRet);

	//log	
	string logStr = RetHelper::GetInstance()->ReturnLog("manager",respJson["req_cmd"],respJson["ret_code"],reqJson["business_id"],respJson["ret_msg"],timer.GetTotal_asMSec());
	if(iRet == _OK) 
		LOG_INFO(logStr);
	else
		LOG_ERROR(logStr); 	
}

int32_t Thrall_Manager_WorkInterface::check_rule(Json::Value& reqJson)
{
	int32_t iRet = _OK;
	if(reqJson["business_id"].isNull())
	{
		iRet = _REQUEST_VAL_INNORMAL;
	}else if(reqJson["strategies"].isNull())
	{
		iRet = _REQUEST_VAL_INNORMAL;
	}else if(reqJson["default"].isNull())
	{
		iRet = _REQUEST_VAL_INNORMAL;
	}else if(reqJson["dist_type"].isNull())
		iRet = _REQUEST_VAL_INNORMAL;
	}else if(reqJson["tail_offset"].isNull())
	{	
		iRet = _REQUEST_VAL_INNORMAL;
	}else
	{
		Json::Value strategies = reqJson["strategies"];
		Json::Value _default = reqJson["default"];

		for(int32_t idx = 0;idx < strategies.size();idx++)
		{
			if(strategies[idx]["cand_id"].isNull() || strategies[idx]["cand_id"].size() <= 0)
				iRet = _REQUEST_VAL_INNORMAL;					

			if(strategies[idx]["cand_prop"].isNull() || strategies[idx]["cand_prop"].size() != strategies[idx]["cand_id"].size())
				iRet = _REQUEST_VAL_INNORMAL;
			
			if(strategies[idx]["cmp_vals"].isNull() || strategies[idx]["cmp_vals"].size() != strategies[idx]["cand_id"].size())
				iRet = _REQUEST_VAL_INNORMAL;
			
			if(strategies[idx]["model_id"].isNull())
				iRet = _REQUEST_VAL_INNORMAL;	
		}
		
		if(_default["cand_id"].isNull() || _default["cand_id"].size() <= 0 )
			iRet = _REQUEST_VAL_INNORMAL;
		
		if(_default["cand_prop"].isNull() || _default["cand_prop"].size() != _default["cand_id"].size() )
			iRet = _REQUEST_VAL_INNORMAL;
		
		if(_default["cmp_vals"].isNull() || _default["cmp_vals"].size() != _default["cand_id"].size())
			iRet = _REQUEST_VAL_INNORMAL;

		if(_default["model_id"].isNull())
			iRet = _REQUEST_VAL_INNORMAL;
	}
	return iRet;
}

int32_t Thrall_Manger_WorkInterface::get_str_from_redis(
						   string& strData,
						   string sKey,
						   HiRedisDbInterface* prefer_redis,
						   HiRedisDbInterface* reserve_redis = NULL);
{
	char* p_result = NULL;
	char split_char,sec_splt_char;
	strData = ""
	if(prefer_redis)
	{
		if(prefer_redis->s_get(0,sKey.c_str(),p_result,split_char,sec_splt_char,1) >= 0)	
		{
			strData = string(p_result);
			return _OK;
		}	
	}
	if(reserve_redis)
	{
		if(reserve_redis->s_get(0,sKey.c_str(),p_result,split_char,sec_splt_char,1) >= 0)
		{
			strData = string(p_result);
			return _OK;
		}
	}
}

int32_t Thrall_Manager_WorkInterface::set_str_to_redis(
									string& strData,
									string& sKey,
									HiRedisDbInterface* prefer_redis,
									HiRedisDbInterface* reserve_redis)
{
	int32_t iRet = _OK;
	if(prefer_redis)
	{
		if(prefer_redis->s_set(0,sKey.c_str(),strData.c_str(),1) < 0)
			iRet = _SET_TO_RDS_FAILED;
	}
	if(reserve_redis)
	{
		if(reserve_redis->s_set(0,sKey.c_str(),strData.c_str(),1) < 0)
			iRet = _SET_TO_RDS_FAILED;
	}
	return iRet;
}
