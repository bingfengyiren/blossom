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


void TrainDemo(const char* sConfigFile, const char* sPattFile, const char* sModelFile)
{
	cout<<"== FMSNN Example: Training =="<<endl; 
	Timer timer;
	
	vector<Pattern*> vtr_patts; 
	timer.Start();
	if(!ReadPatterns(vtr_patts, sPattFile))
	{
		cout<<"failed to open training patterns file "<<sPattFile<<endl; 
		return; 
	}
	timer.Stop();
	cout<<"Load "<<vtr_patts.size()<<" patterns successfully!"<<endl; 
	printf("time_cost(s): %.3f\n", timer.GetLast_asSec()); 
	cout<<"--"<<endl; 

	FMSNN fms_nn
	if(!fms_nn.InitFromConfig(sConfigFile, vtr_patts[0]->m_nXCnt, vtr_patts[0]->m_nYCnt))
	{
		cout<<"failed to initialize the FM from config file "<<sConfigFile<<endl; 
		return; 
	}
	TypeDefs::Print_PerceptronLearningParamsT(cout, fm.GetLearningParams()); 
	cout<<"--"<<endl; 
	TypeDefs::Print_FMParamsT(cout, fm.GetArchParams()); 
	cout<<"==========================="<<endl; 

	fm.TrainAndValidate(vtr_patts); 

	if(fm.Save(sModelFile) != _METIS_NN_SUCCESS)
		cout<<"failed to save the FM model to "<<sModelFile<<endl; 

	for(size_t i = 0; i < vtr_patts.size(); i++)
		delete vtr_patts[i]; 
	vtr_patts.clear(); 
}

DEFINE_string(train,"./data/fmsnn_config.txt","config file of train");
DEFINE_string(model,"./data/fmsnn_model.txt","save mode parameters");
DEFINE_string(patt,"./data/sample.txt","sample file");

int main(int argc,char** argv)
{
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	TrainDemo(FLAGS_train.c_str(), FLAGS_patt.c_str(), FLAGS_model.c_str());
	gflags::ShutDownCommandLineFlags();
}
