#include <iostream>
#include <string>
using namespace std;
#include <chrono>
#include <ctime>

#include "hiredis.h"

int main(int argc,char** argv)
{
	string ip = "rs20043.mars.grid.sina.com.cn";
	int port = 20043;
	int timeout = 10000;
	struct timeval tv;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = timeout * 1000;

	redisContext* c = redisConnect(ip.c_str(),port);
	//redisContext* ct = redisConnectWithTimeout(ip.c_str(),port,tv);
	
	cout<<c->errstr<<endl;

	//redisReply* r = (redisReply*)redisCommand(ct,"randomkey");

	//cout<<r->str<<endl;
	return 0;
}

