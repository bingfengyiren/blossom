#include <iostream>
#include <unistd.h>
#include "Timer.h"

using namespace std;
using namespace base;


int main(int argc,char** argv)
{
	Timer t;
	t.Start();
	sleep(2);
	t.Stop();
	t.Start();
	sleep(1)
	t.Stop();

	cout<<t.Get_asMSec(0)<<endl;
	cout<<t.SegCnt()<<endl;
}
