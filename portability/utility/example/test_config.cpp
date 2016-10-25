#include <iostream>
using namespace std;
#include "Config.h"
using namespace base;

int main(int argc,char** argv)
{
	Config conf;
	conf.Read("./test.conf");
	//conf.Read("./fm_config.txt");
	cout<<conf.ValCnt("TEST_ARRAY")<<endl;	
	cout<<conf.GetVal_asInt("TEST_INT")<<endl;
	//cout<<conf.GetVal_asFloat("Epsilon")<<endl;
}
