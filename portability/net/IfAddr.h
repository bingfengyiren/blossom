/////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IfAddr.h : 获取本地ip地址的类声明
//
// 版本：v1.0 - 2014.04.18 by 冯扬(fengyoung82@gmail.com) 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _FENGYOUNG_API_IF_ADDR_H 
#define _FENGYOUNG_API_IF_ADDR_H 

#include <string>
#include <map>
#include <vector>
using namespace std; 
#include <stdint.h>


namespace fengyoung
{

enum EInetType
{
	_EINET_V4,
	_EINET_V6
};


class IfAddr
{
public: 
	IfAddr(); 
	virtual ~IfAddr(); 
	
	string LocalAddr(const char* sIfaName, const EInetType eInetType = _EINET_V4); 
	int32_t AllLocalAddr(vector<pair<string,string> >& vtrLocalAddrs, const EInetType eInetType = _EINET_V4); 

private: 
	map<string,string> m_mapIfaNameToAddrsV4;	// ip v4 
	map<string,string> m_mapIfaNameToAddrsV6;	// ip v6 
}; 

}

#endif /* _FENGYOUNG_API_LOCAL_IP_ADDR_H */

