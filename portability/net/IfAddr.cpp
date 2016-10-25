#include "IfAddr.h"
using namespace fengyoung; 
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>



IfAddr::IfAddr() 
{
	struct ifaddrs * ifAddrStruct = NULL;
	void * tmpAddrPtr = NULL;

	getifaddrs(&ifAddrStruct);

	while(ifAddrStruct != NULL) 
	{
		if(ifAddrStruct->ifa_addr->sa_family == AF_INET) 
		{ // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			m_mapIfaNameToAddrsV4.insert(pair<string,string>(string(ifAddrStruct->ifa_name), string(addressBuffer))); 
		} 
		else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6) 
		{ // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			m_mapIfaNameToAddrsV6.insert(pair<string,string>(string(ifAddrStruct->ifa_name), string(addressBuffer))); 
		} 
		ifAddrStruct=ifAddrStruct->ifa_next;
	}
}


IfAddr::~IfAddr()
{
}


string IfAddr::LocalAddr(const char* sIfaName, const EInetType eInetType)
{
	if(eInetType == _EINET_V4)
	{
		map<string,string>::iterator iter_map = m_mapIfaNameToAddrsV4.find(string(sIfaName));	
		if(iter_map != m_mapIfaNameToAddrsV4.end())
			return iter_map->second; 
	}
	else if(eInetType == _EINET_V6)
	{
		map<string,string>::iterator iter_map = m_mapIfaNameToAddrsV6.find(string(sIfaName));	
		if(iter_map != m_mapIfaNameToAddrsV6.end())
			return iter_map->second; 
	}
	return string(""); 
}


int32_t IfAddr::AllLocalAddr(vector<pair<string,string> >& vtrLocalAddrs, const EInetType eInetType)
{
	vtrLocalAddrs.clear(); 
	if(eInetType == _EINET_V4)
	{
		map<string,string>::iterator iter_map; 
		for(iter_map = m_mapIfaNameToAddrsV4.begin(); iter_map != m_mapIfaNameToAddrsV4.end(); iter_map++)
			vtrLocalAddrs.push_back(pair<string,string>(iter_map->first, iter_map->second)); 	
	}
	else if(eInetType == _EINET_V6)
	{
		map<string,string>::iterator iter_map; 
		for(iter_map = m_mapIfaNameToAddrsV6.begin(); iter_map != m_mapIfaNameToAddrsV6.end(); iter_map++)
			vtrLocalAddrs.push_back(pair<string,string>(iter_map->first, iter_map->second)); 	
	}
	return vtrLocalAddrs.size(); 
}


