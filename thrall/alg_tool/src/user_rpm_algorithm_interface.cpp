#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <map>
#include <vector>
#include <iostream>
#include "user_rpm_algorithm_interface.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif


using namespace std;

DYN_ALGORITHMS(UserRpmAlgorithmInterface);

// 获取特征并保存
void UserRpmAlgorithmInterface::makeFeature(const UserFeature* userfeature, 
		const UserFeature* recyfeature, const uint32_t bridgeNum, const uint64_t bridgeUid, 
		const QMD_MAP& qmd_result, vector<double> &feature){
	// member or not  1
	int8_t ismember  = recyfeature->ismember;
	feature.push_back((ismember <=0 ? 0 : ismember));

	//user age , remove user age out range of (0,100) 2	
	int8_t age = recyfeature->age;
	if(age < 0 || age > 100)
		feature.push_back(0);
	else
		feature.push_back(age);

	//gender 3
	int gender = recyfeature->gender;
	feature.push_back((gender == -1 ? 1 : gender));

	//new V or not 4
	int t = recyfeature->regday;
	feature.push_back( (t > 7 ? 0 :1 ) );

	//same province or not 5
	int p = ( (userfeature->province == recyfeature->province) ? 1 : 0);
	feature.push_back(p);

	//same city or not 6
	int c = ( (userfeature->province == recyfeature->province)&&(userfeature->city == recyfeature->city)? 1 :0 );
	feature.push_back(c);
	
	// fans 7
	double fa = recyfeature->fanscount;
    feature.push_back((fa <= 0 ? 0 : log(fa)));		

	//followers 8
	double fo = recyfeature->followcount;
	feature.push_back((fo <= 0 ? 0 : log(fo)));
	
	//weibo count 9
	double w = recyfeature->weibocount;
    feature.push_back((w <= 0 ? 0 : log(w)));

	//same corp 10
	int co = 0;
	for(int i = 0 ; i < recyfeature->corp_count ; i++)
		for(int j = 0 ; j< userfeature->corp_count ; j++)
			if(recyfeature->corps[i] == userfeature->corps[j])
				co++;
	feature.push_back(co);

	//same school 11
	int sc = 0;
        for(int i = 0 ; i < recyfeature->school_count ; i++)
                for(int j = 0 ; j < userfeature->school_count ; j++)
                        if(recyfeature->schools[i] == userfeature->schools[j])
                                sc++;
    feature.push_back(sc);
	
	//qin mi du 12
	uint32_t qmd = 0;
	QMD_MAP::const_iterator iter = qmd_result.find(bridgeUid);
	if(iter != qmd_result.end()) qmd = iter->second;	
	feature.push_back(qmd);
	
	//bridge num 13
	feature.push_back(bridgeNum);
	
	//V level 14
	int vl = recyfeature->viplevel;
	feature.push_back((vl <= 0 ? 0 : vl));

	// 15	
	int ir = recyfeature->isred;
	feature.push_back((ir <= 0 ? 0 : ir));

	// 16
	int iv = recyfeature->isvip;
	feature.push_back((iv <= 0 ? 0 : iv));	

	// 17
	int cg = 0;
	if(userfeature->category1 == recyfeature->category1) cg++;
	if(userfeature->category2 == recyfeature->category2) cg++;
	if(userfeature->category3 == recyfeature->category3) cg++;
	feature.push_back(cg);

	// 18
	feature.push_back(recyfeature->yesrpm1);
    // 19
	feature.push_back(recyfeature->yesrpm2);
	// 20
	feature.push_back(recyfeature->yesrpm3);

}

// 特征离散化
void UserRpmAlgorithmInterface::featureDiscrete(vector<double>& origin_feature, 
		const map<int,vector<double> >& mm, const vector<int>& needDisFea, vector<double>& res_feature){
	vector<double> disFea;
	for(int i = 0; i < origin_feature.size(); i ++){	// 遍历初始特征集合
		vector<int>::const_iterator dit = find(needDisFea.begin(), needDisFea.end(), i+1);		
		if(dit == needDisFea.end()){					// 若初始特征不需要离散化
			res_feature.push_back(origin_feature[i]);	// 加入到离散化特征集中
		}
		else{											// 初始特征需要离散化
			vector<double> fea(Discrete_NUM, 0);	
			vector<double> mm_tmp;
			map<int,vector<double> >::const_iterator mit = mm.find(i+1);
			if(mit != mm.end()) mm_tmp = mit->second;
			int poss = binarySearch(origin_feature[i], mm_tmp);
			poss ++;
			if (poss >= Discrete_NUM) poss = Discrete_NUM -1;       // 防止越界
			fea[poss] = 1;
			disFea.insert(disFea.end(), fea.begin(), fea.end());
		}
	}
	// 离散化的特征追加到末尾
	res_feature.insert(res_feature.end(), disFea.begin(), disFea.end());

}

// 计算RPM得分
uint64_t UserRpmAlgorithmInterface::countRpmScore(const vector<double>& feature, 
		const vector<double>& para){
	double score = 0;
	if(feature.size() != para.size()) LOG_ERROR("number of discrete feature not equal para\n");	

	for(size_t i = 0 ; i < feature.size() ; i++)
		score += feature[i] * para[i];
	return (uint64_t)(1.0/(1+exp(-score))*100000);
}

void UserRpmAlgorithmInterface::split_string_map_qmd(const char*& str_input, char sep_char, 
		char second_sep_char, QMD_MAP& map_ids, uint32_t limit){
	const char* p_temp = str_input;

	uint32_t id_num = 0;
	uint64_t id = 0;
	float score = 0.0f;
	int score_index = -1;
	bool score_flag = false;
	while((*p_temp) != '\0'){
		if(limit != 0 && id_num >= limit)
			return;

		if ((*p_temp) >= '0' && (*p_temp) <= '9'){
			if (score_flag){
				score = score * 10 + (*p_temp) - '0';
				score_index ++ ;
			}else{
				id = id * 10 + (*p_temp) - '0';
			}
		}else if((*p_temp) == sep_char){
			if(score_index == -1)
				score = score;
			else
				score = score / float(pow(10, score_index));

			map_ids.insert(QMD_MAP::value_type(id, uint32_t(score * MAX_QMD_MAGIC_NUM)));
			id_num ++;
			
			id = 0;
			score = 0.0f;
			score_flag = false;
			score_index = -1;
		}else if((*p_temp) == '.'){
			score_index = 0;
		}else if((*p_temp) == second_sep_char){
			score_flag = true;
		}
			p_temp ++;
	}
	if(score_index == -1)
		score = score;
	else
		score = score / float(pow(10, score_index));

	map_ids.insert(QMD_MAP::value_type(id, uint32_t(score * MAX_QMD_MAGIC_NUM)));
	id_num ++;
}

int UserRpmAlgorithmInterface::get_qmd_data(DbCompany* p_db_company, uint64_t uid, QMD_MAP& qmd_result){
    	//int spend_msec = -1;
	//struct timeval tv_temp;
	//tv_temp = calc_spend_time(tv_temp, "qmd start", spend_msec, true);
	if(NULL == p_db_company){
		LOG_ERROR("global db_company is NULL!");
		return -1;
	}
	DbInterface* p_qmd_db_interface = p_db_company->get_db_interface("USER_QMD_DB");
	if (NULL == p_qmd_db_interface){
		LOG_ERROR("qmd db_interface is NULL!");
		return -1;
	}
	// 构建请求字符串
	char req[MAX_BUFFER];
	sprintf(req, "{\"cmd\":\"query\",\"id\":%"PRIu64"}", uid);
	char* result_str = NULL;
	char split_char, second_split_char;
	int result = p_qmd_db_interface->s_get(0, req, result_str, split_char, second_split_char);
	if(result <= 0){
		LOG_ERROR("%s", "get_qmd_data fail!");
		return result;
	}
	if (strcmp(result_str, "not found") == 0){
		LOG_ERROR("%"PRIu64": qmd not found!", uid);
		return 0; // 未取到结果，返回0
	} 

	split_string_map_qmd((const char* &)result_str, split_char, second_split_char, qmd_result);
	//tv_temp = calc_spend_time(tv_temp, "qmd finish", spend_msec);
	return 1;
}

int UserRpmAlgorithmInterface::get_qmd_data_lushan(DbCompany* p_db_company, uint64_t uid, 
		QMD_MAP& qmd_result){
	if(NULL == p_db_company){
		LOG_ERROR("global db_company is NULL!");
		return -1;
	}
	DbInterface* p_qmd_db_interface = p_db_company_->get_db_interface("QMD_NEW_DB");
	if (NULL == p_qmd_db_interface){
		LOG_ERROR("qmd db_interface is NULL!");
		return -1;
	}
	int db_num = 20;
	if((uid%12)%2 == 0){
		db_num = 19;
	}

	char key_str[20];
	snprintf(key_str, 20, "%d-%"PRIu64, db_num, uid);		// MC协议，构建key
	//LOG_ERROR("get qmd data:%s", key_str);
	char* p_result = NULL;
	char split_char, second_split_char;
	int result = p_qmd_db_interface->s_get(0, key_str, p_result, split_char, second_split_char, uid);
	
	if (result <= 0 ){
		LOG_ERROR("%s", "get_qmd_data fail!");
		return result;
	}
	if (NULL == p_result){
		LOG_ERROR("%s", "qmd data is NULL!");
		return 0;
	}
	qmd_data_pack_t *qmd_data_pack = (qmd_data_pack_t *)p_result;
	uint32_t length = reversebytes_uint32t(qmd_data_pack->num);
	
	char *qmd_data_unit = p_result + 4;
	
	uint64_t base[64];
	base[0] = 1;
	for(int i = 1; i < 64; i++){
		base[i] = base[i-1] * 2;
	}

	for(uint32_t i = 0; i < length; i++){
		uint64_t uid = 0;
		for(int j = 0; j < 8; j++){
			uid += ((uint8_t)qmd_data_unit[j])*base[8*(8-j-1)];
		}
		//LOG_ERROR("uid:%lu",uid);

		uint32_t weight = 0;
		for(int j = 0; j < 4; j++){
			weight += ((uint8_t)qmd_data_unit[j+9])*base[8*(4-j-1)];
		}
		//LOG_ERROR("qmd_weight:%d",weight);
		qmd_result.insert(std::make_pair<uint64_t,uint32_t>(uid,weight));
		qmd_data_unit += 13;
	}
	return 1;
}

int UserRpmAlgorithmInterface::algorithm_core(int64_t req_id, const AccessStr& access_str,
		VEC_CAND& vec_cand){
	LOG_ERROR("logid:%"PRIu64" user:%"PRIu64, req_id, access_str.uid_);
	GlobalDbCompany* p_global_db_company = p_db_company_->get_global_db_company();
	if(NULL == p_global_db_company){
		LOG_ERROR("global db company is error!");
		return -1;
	}

	GlobalDbInterface* userFD_db_interface = p_global_db_company->get_global_db_interface("USER_FD_DB");
	if(NULL == userFD_db_interface){
		LOG_ERROR("USER_FD db_interface is NULL!");
		return -1;
	}

	UserFeature * userfeature =  ((GlobalFeatureDataDbInterface*)userFD_db_interface)->getfeature(access_str.uid_);
	if(NULL == userfeature){
		LOG_ERROR("user %"PRIu64" UserFeature is NULL!", access_str.uid_);
		return -1;
	}	
	QMD_MAP qmd_result;
	get_qmd_data_lushan(p_db_company_, access_str.uid_, qmd_result);
		
	GlobalDbInterface* userFW_db_interface = p_global_db_company->get_global_db_interface("USER_FW_DB");
	if(NULL == userFW_db_interface){
		LOG_ERROR("user %"PRIu64" USER_FW db_interface is NULL!", access_str.uid_);
		return -1;
	}
	// 根据推荐用户类型使用不同的RMP模型
	const RPM_Model &chengv =  ((GlobalFeatureWeightDbInterface*)userFW_db_interface)->getChengV();
	const RPM_Model &lanv = ((GlobalFeatureWeightDbInterface*)userFW_db_interface)->getLanV();
    	const RPM_Model &puhu = ((GlobalFeatureWeightDbInterface*)userFW_db_interface)->getPuhu();
	
	VEC_CAND chengv_cand, lanv_cand, puhu_cand;
	//int spend_msec = -1;
	//struct timeval tv_temp;
	//tv_temp = calc_spend_time(tv_temp, "rpm model start", spend_msec, true);
	
	for(size_t i = 0 ; i < vec_cand.size();i++)
	{
		vector<double> feature;
		vector<double> disFeature;
		//LOG_ERROR("cand uid:%"PRIu64, vec_cand[i]->uid);
		UserFeature *recyfeature =  ((GlobalFeatureDataDbInterface*)userFD_db_interface)->getfeature(vec_cand[i]->uid);
				if(recyfeature == NULL) 
		{
			if(vec_cand[i]->utype == 1)
				chengv_cand.push_back(vec_cand[i]);
			else
			{
				if(vec_cand[i]->utype == 2)
					lanv_cand.push_back(vec_cand[i]);
				else
					puhu_cand.push_back(vec_cand[i]);
			}	
			continue;
		}
		uint64_t bridgeUid = 0;
	    	uint32_t bridgeNum = 0;
		// 将推荐用户根据类型分布到不同的容器中
		if(vec_cand[i]->utype == 1)
		{	
			for(int j = 0 ;j < REASON_NUM - 1 ; j++)
			{
				if(vec_cand[i]->reason[j].bnum != 0 && bridgeUid == 0) bridgeUid = vec_cand[i]->reason[j].bs[0]; 
				bridgeNum += vec_cand[i]->reason[j].bnum;
			}
			//LOG_ERROR("yellow v make feature!");
			makeFeature(userfeature, recyfeature, bridgeNum, bridgeUid, qmd_result, feature);
			//LOG_ERROR("yellow v feature discrete!");
			featureDiscrete(feature, chengv.mm, chengv.needDisFea, disFeature);
			//LOG_ERROR("yellow v compute rpm score!");
			vec_cand[i]->rpmscore = countRpmScore(disFeature, chengv.para);
			chengv_cand.push_back(vec_cand[i]);
		}else{
			if(vec_cand[i]->utype == 2){
				for(int j = 0 ;j < REASON_NUM - 1 ; j++){
                			if(vec_cand[i]->reason[j].bnum != 0 && bridgeUid == 0) bridgeUid = vec_cand[i]->reason[j].bs[0];
                    			bridgeNum += vec_cand[i]->reason[j].bnum;
                		}
				//LOG_ERROR("blue v make feature!");
				makeFeature(userfeature, recyfeature, bridgeNum, bridgeUid, qmd_result, feature);
				vector<double> temp;
				for(size_t j = 0 ; j< feature.size();j++) 
					if(j!= 14 && j != 15)
						temp.push_back(feature[j]);
				//LOG_ERROR("blue v feature discrete!");
				featureDiscrete(temp, lanv.mm, lanv.needDisFea, disFeature);
				//LOG_ERROR("blue v compute rpm score!");
                		vec_cand[i]->rpmscore = countRpmScore(disFeature, lanv.para);
				lanv_cand.push_back(vec_cand[i]);
			}else{
                		if(vec_cand[i]->reason[3].bnum != 0) bridgeUid = vec_cand[i]->reason[3].bs[0];
                		bridgeNum+=vec_cand[i]->reason[3].bnum;
				//LOG_ERROR("not v make feature!");
                		makeFeature(userfeature, recyfeature, bridgeNum, bridgeUid, qmd_result, feature);
				vector<double> temp;
                		for(size_t j = 0 ; j< feature.size();j++)
                			if(j!= 3 && j!=12 && j!=13 && j != 14)
                				temp.push_back(feature[j]);
				//LOG_ERROR("not v feature discrete!");
               	 		featureDiscrete(temp, puhu.mm, puhu.needDisFea, disFeature);
				//LOG_ERROR("not v compute rpm score!");
				vec_cand[i]->rpmscore = countRpmScore(disFeature, puhu.para);
				puhu_cand.push_back(vec_cand[i]);
			}
		}		
	}
	sort(chengv_cand.begin(), chengv_cand.end(), cmp_Candidate_item);
	sort(lanv_cand.begin(), lanv_cand.end(), cmp_Candidate_item);
	sort(puhu_cand.begin(), puhu_cand.end(), cmp_Candidate_item);
	int c = 0, l = 0 , p= 0 ;
	for(size_t i = 0 ; i < vec_cand.size() ; i++){
		if(vec_cand[i]->utype == 1)
			vec_cand[i] = chengv_cand[c++];
		else{
			if(vec_cand[i]->utype == 2)
				vec_cand[i] = lanv_cand[l++];
			else
				vec_cand[i] = puhu_cand[p++];
		}
	}
	//tv_temp = calc_spend_time(tv_temp, "rpm model finish", spend_msec);
	return 1;
}
