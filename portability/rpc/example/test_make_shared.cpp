#include <iostream>
#include <string>
//#include <boost/make_shared.hpp>
#include <memory>

using namespace std;
//using namespace boost;

int main()
{
	auto sp = make_shared<string>("make_shared");
	cout<<*sp<<endl;
	return 0;
}
