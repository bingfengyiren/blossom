#include "MasterTrainer_Perceptron.h"
using namespace metis_plat; 
#include "Timer.h"
using namespace metis_uti; 
#include <iostream>
using namespace std; 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


MasterTrainer_Perceptron::MasterTrainer_Perceptron() : MasterTrainer()
{
}


MasterTrainer_Perceptron::~MasterTrainer_Perceptron()
{
}


bool MasterTrainer_Perceptron::InitByLearningConfig(const char* sLearningConfFile)
{
	if(m_nInput < 0 || m_nOutput < 0)
		return false; 
	return m_perceptron.InitFromConfig(sLearningConfFile, m_nInput, m_nOutput); 
}


bool MasterTrainer_Perceptron::InitByModel(const char* sModelFile)
{
	if(m_perceptron.Load(sModelFile) != _METIS_NN_SUCCESS)
		return false;
	if(m_nInput > 0 && m_nOutput > 0)
	{
		if(m_perceptron.GetArchParams().input - 1 != m_nInput || m_perceptron.GetArchParams().output != m_nOutput)
			return false; 
	}
	return true; 
}


// 执行训练
void MasterTrainer_Perceptron::Train(const char* sOutModelFile)
{
	printf("** Perceptron Parallel Training **\n"); 

	PerceptronLearningParamsT learning_params_t = m_perceptron.GetLearningParams(); 
	double learning_rate = learning_params_t.learning_rate; 
	int32_t batch_num = learning_params_t.mini_batch;// / (int32_t)m_vtrSlaves.size(); 
	string str_tmp_file = TempFile(sOutModelFile);	// 临时文件 

	TypeDefs::Print_PerceptronLearningParamsT(cout, learning_params_t); 
	printf("--\n");
	TypeDefs::Print_PerceptronParamsT(cout, m_perceptron.GetArchParams()); 
	printf("=====\n");

	int32_t iter = 0;
	double rmse, last_rmse = -1.0;
	Timer timer; 
	bool tmp_flag = false, finish_flag = false; 

	while(iter < learning_params_t.iterations)
	{
		timer.Start(); 
		// 同步模型
		if(!SyncModel())
		{
			printf("failed to sync model to slaves, break loop!\n");
			break; 	
		}
		// 更新slave上的模型
		if(!UpdateModel(batch_num, learning_rate, learning_params_t.regula))
		{
			printf("failed to update model, break loop!\n");
			break; 	
		}
		// 从slave收集模型
		if(!CollectModel())
		{
			printf("fialed to collect model, break loop!\n");
			break; 
		}
		// 将模型保存到临时文件中
		if(m_perceptron.Save(str_tmp_file.c_str()) != _METIS_NN_SUCCESS)
		{
			printf("failed to save model to template file, break loop!\n");
			break; 
		}
		tmp_flag = true; 

		// 计算rmse
		rmse = sqrt(m_dAvgError); 
		timer.Stop(); 

		if(rmse <= learning_params_t.epsilon)
		{
			printf("iter %d | learning_rate: %.6g | rmse: %.6g | time_cost(s): %.3f\n", 
					iter+1, learning_rate, rmse, timer.GetLast_asSec());
			finish_flag = true;
			break;
		}
/*
		if(last_rmse > 0.0)
		{
			if(fabs(rmse - last_rmse) * 10000.0 < last_rmse)
			{
				printf("iter %d | learning_rate: %.6g | rmse: %.6g | time_cost(s): %.3f\n", 
						iter+1, learning_rate, rmse, timer.GetLast_asSec());
				finish_flag = true;
				break; 
			}
		}
*/
		last_rmse = rmse; 

		printf("iter %d | learning_rate: %.6g | rmse: %.6g | time_cost(s): %.3f\n", 
				iter+1, learning_rate, rmse, timer.GetLast_asSec());
		learning_rate = learning_rate * (learning_rate / (learning_rate + (learning_rate * learning_params_t.rate_decay)));	
		iter++; 
	}

	if(iter >= learning_params_t.iterations || finish_flag)
		printf("training done! time cost %.3f sec\n", timer.GetTotal_asSec());
	if(tmp_flag)
	{
		char scmd[1024*8];
		sprintf(scmd, "mv %s %s", str_tmp_file.c_str(), sOutModelFile); 
		system(scmd); 
	}
}

// 从所有slave上收集更新后的模型数据
bool MasterTrainer_Perceptron::CollectModel()
{
	string str_model;
	double avg_error; 
	Perceptron perc; 

	for(size_t i = 0; i < m_vtrSlaves.size(); i++) 
	{
		if(!GetModelFromSlave(m_vtrSlaves[i].c_str(), str_model, avg_error))
			return false; 
		if(!perc.SetByModelString(str_model.c_str()))
			return false;
		if(i == 0)
		{
			if(!m_perceptron.CombineWith(perc, 0.0, 1.0 / (double)m_vtrSlaves.size()))
				return false; 
			m_dAvgError = avg_error; 
		}
		else	
		{
			if(!m_perceptron.CombineWith(perc, 1.0, 1.0 / (double)m_vtrSlaves.size()))
				return false; 	
			m_dAvgError += avg_error; 
		}
	}
	m_dAvgError /= (double)m_vtrSlaves.size(); 
	return true; 
}


string MasterTrainer_Perceptron::GetModelAsString()
{
	return m_perceptron.ConvToModelString(); 
}


