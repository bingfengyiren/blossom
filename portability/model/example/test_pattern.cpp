//C header
#include <cstring>
#include <stdio.h>

//C++ header
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//local header
#include "Timer.h"
#include "Config.h"
using namespace base;
#include "neural_network/TypeDefs.h"
#include "neural_network/Activation.h"
#include "neural_network/Pattern.h"
#include "neural_network/FM.h"
using namespace model_nn;
#include <gflags/gflags.h>
#include <iostream>
using namespace std;

bool ReadPatterns(vector<Pattern*>& vtrPatts,const char* sPattFile)
{
	ifstream ifs(sPattFile);
	if(!ifs.is_open())
		return false;
	string str;
	Pattern* ppatt = NULL;
	while(!ifs.eof())
	{
		getline(ifs,str);
		if(str.empty())
			continue;
		ppatt = new Pattern();
		if(!ppatt->FromString(str.c_str()))
		{
			delete ppatt;
			continue;
		}
		vtrPatts.push_back(ppatt);
	}	
	ifs.close();
	if(vtrPatts.empty())
		return false;
	return true;
}

int main(int argc ,char** argv)
{
	string patt_file = "./sample.txt";
	vector<Pattern*> vtr_patts;
	ReadPatterns(vtr_patts,patt_file.c_str());		
	for(vector<Pattern*>::iterator it = vtr_patts.begin(); it != vtr_patts.end(); ++it)
	{
		string str = (*it)->ToString();
		cout<<str<<endl;
	}
}
