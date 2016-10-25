#include <iostream>
#include "ret_helper.h"

using namespace helper;
using namespace std;

int main(int argc,char** argv)
{
	string msg = RetHelper::GetInstance()->Msg(_BROADCAST_DEL_DISTRULE_FAILED);
	string log = RetHelper::GetInstance()->ReturnLog("manager","get_rule",4001,"901",msg,"0.82");
	cout<<log<<endl;
}
