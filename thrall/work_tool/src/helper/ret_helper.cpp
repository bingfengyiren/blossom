#include  "ret_helper.h"

using namespace helper;

//sinleton
RetHelper* RetHelper::_instance = NULL;

RetHelper::RetHelper()
{
	m_retMsg[_OK] = "ok";
	m_retMsg[_FAILED] = "failed";

	m_retMsg[_EXCEPTION_OCCUR] = "exception catched by try module";
	m_retMsg[_TRANSF_JSON_FAILED] = "transform json-c to json-cpp error";
	m_retMsg[_MANAGER_DB_FAIL] = "get manager db failed";
	m_retMsg[_RANKER_DB_FAIL] = "get ranker db failed";
	m_retMsg[_PREDICTOR_DB_FAIL] = "get predictor db failed";
	m_retMsg[_RECOMSERV_DB_FAIL] = "get recom db failed";

	m_retMsg[_PARSE_REQUEST_FAILED] = "parse request parameter error";
	m_retMsg[_REQUEST_IS_IMCOMPLETED] = "request parameter is not completed";
	m_retMsg[_REQUEST_VAL_INNORMAL] = "some wrong with request data";
	m_retMsg[_UNSUPPORTED_REQUEST_CMD] = "unsupported requet command";

	m_retMsg[_SET_TO_RDS_FAILED] = "set data to redis failed";
	m_retMsg[_GET_FROM_RDS_FAILED] = "get data from redis failed";
	
	m_retMsg[_FIND_MODEL_FAILED] = "can't find the model";
	m_retMsg[_PARSE_MODEL_DATA_FAILED] = "parse model parameter failed";
	m_retMsg[_BROADCAST_SET_MODEL_FAILED] = "failed to broadcast the set model to others";
	m_retMsg[_BROADCAST_DEL_MODEL_FAILED] = "failed to broadcast the del model to others";

	m_retMsg[_FIND_DISTRULE_FAILED] = "can't find the distribute rule";
	m_retMsg[_PARSE_DISTRULE_DATA_FAILED] = "parse distribute parameter failed";	
	m_retMsg[_BROADCAST_SET_DISTRULE_FAILED] = "failed to broadcast the set rule to others";
	m_retMsg[_BROADCAST_DEL_DISTRULE_FAILED] = "failed to broadcast the del rule to others";	

	m_retMsg[_PREDICT_FAILED] = "predict error";
	m_retMsg[_RANK_FAILED] = "ranking failed";
	m_retMsg[_MANAGER_DB_FAIL] = "merging failed";
}

RetHelper::~RetHelper(){}

RetHelper* RetHelper::GetInstance()
{
	if(!_instance)
		_instance = new RetHelper();
		return _instance;
	return _instance;
}

void RetHelper::Release()
{
	if(_instance)
	{
		delete _instance;
		_instance = NULL;
	}
}

string RetHelper::Msg(RET_CODE code)
{
	map<RET_CODE,string>::iterator iter = m_retMsg.find(code);	
	if(iter != m_retMsg.end())
		return iter->second;
	else
		return string("unknown error!");
}

string RetHelper::ReturnLog(string serv,string cmd,
							 int32_t ret_code,string business_id,
							 string ret_msg,
							 float cost)
{
	char* p_result;
	sprintf(p_result,"SERV:%s`CMD:%s`RET_CODE:%d`BUSINESS:%s`MSG:%s`COST:%.2fms",serv.c_str(),cmd.c_str(),ret_code,business_id.c_str(),ret_msg.c_str(),cost);
	return string(p_result);
}
