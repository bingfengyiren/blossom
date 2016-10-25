#ifndef _REDIS_H
#define _REDIS_H

#include <cstdint>
#include "hiredis.h"

#define IP_WORD_LEN 128

namespace db
{
	struct RedisDbInfo_
	{
		char host_[IP_WORD_LEN]; // ip
		uint32_t port_; // port
		uint32_t timeout_; // mic seconds
		uint32_t db_num_; //db_num
		
	}

	enum _DB_REDIS_CONN_T
	{
		_NORMAL,
		_WITH_TIMEOUT,
		_NON_BLOCK,
	}
	class Redis
	{
		public:
			Redis(const char* host, const uint32_t port,const uint32_t timeout = 10000,const uint32_t db_num);
			~Redis();

		//member
		private:
			redisContext* get_context();
			release();

		//meta
		private:
			RedisStruct* m_dbinfo;	
	}
}

