#include <iostream>
using namespace std;
#include "TimeFmt.h"
using namespace base;

int main(int argc,char** argv)
{
	//TimeFmt t;
	cout<<TimeFmt::CurTimeStamp()<<endl;	
	cout<<TimeFmt::CurTime_asStr(_TIME_FMT_NOBLANK)<<endl;
	cout<<TimeFmt::DateConv_Uint32ToStr(TimeFmt::CurTimeStamp())<<endl;
}



