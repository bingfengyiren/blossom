#include <iostream>
using namespace std;
#include <RocAnalyzer.h>
using namespace metis_uti;

int main(int argc,char** argv)
{
	RocAnalyzer* roc = new RocAnalyzer();
	roc->Insert(_NEGATIVE,1);
	roc->Insert(_NEGATIVE,0);
	roc->Insert(_POSITIVE,0);
	cout<<roc->Auc()<<endl;
	cout<<roc->Count(_NEGATIVE)<<endl;
}
