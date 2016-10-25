#ifndef _RET_HELPER_H
#define _RET_HELPER_H

#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <map>

using namespace std;

namespace helper{

enum RET_CODE{
	// base 
	_OK = 200, 
	_FAILED = 400, 

	//异常
	_EXCEPTION_OCCUR  = 4001,
	_TRANSF_JSON_FAILED, 
	_MANAGER_DB_FAIL,
	_RANKER_DB_FAIL,
	_PREDICTOR_DB_FAIL,
	_RECOMSERV_DB_FAIL,

	//请求参数异常
	_PARSE_REQUEST_FAILED = 4100,
	_REQUEST_IS_IMCOMPLETED,
	_REQUEST_VAL_INNORMAL,
	_UNSUPPORTED_REQUEST_CMD,

	//redis请求异常
	_SET_TO_RDS_FAILED = 4200, //key异常
	_GET_FROM_RDS_FAILED,

	//模型处理异常
	_FIND_MODEL_FAILED = 4500,//查找模型失败
	_PARSE_MODEL_DATA_FAILED,
	_BROADCAST_SET_MODEL_FAILED,
	_BROADCAST_DEL_MODEL_FAILED,

	//分发规则处理异常
	_FIND_DISTRULE_FAILED = 4600,
	_PARSE_DISTRULE_DATA_FAILED,
	_BROADCAST_SET_DISTRULE_FAILED,
	_BROADCAST_DEL_DISTRULE_FAILED,
	
	//计算异常
	_PREDICT_FAILED = 4700,
	_RANK_FAILED,
	_MERGE_IN_RANKING_FAILED,
};

class RetHelper
{
private:
	RetHelper();
	~RetHelper();

public:
	string Msg(RET_CODE code);
	string ReturnLog(string serv,
			string cmd,
			int32_t ret_code,
			string business_id,
			string ret_msg,
			float cost);
	void Release();
	static RetHelper* GetInstance();

public:
	static RetHelper* _instance;

private:
	map<RET_CODE,string> m_retMsg;
};
}
#endif
