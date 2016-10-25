#ifndef _USER_RPM_ALGORITHM_USER_RPMFACE_HEADER_
#define _USER_RPM_ALGORITHM_USER_RPMFACE_HEADER_

#include "algorithm_interface.h"
typedef __gnu_cxx::hash_map<uint64_t, uint32_t> QMD_MAP;
const int MAX_QMD_MAGIC_NUM = 10000;
const int Discrete_NUM = 10;		// 离散化参数

typedef struct _qmd_data_pack{
	int32_t num;
	char qmd_data[0];
}qmd_data_pack_t;

class UserRpmAlgorithmInterface : public AlgorithmInterface{
	
	public:
		UserRpmAlgorithmInterface(DbCompany*& p_db_company, int interface_id):
			AlgorithmInterface(p_db_company, interface_id){
		}

		~UserRpmAlgorithmInterface(){
		}

		int algorithm_core(int64_t req_id, const AccessStr& access_str, VEC_CAND& vec_cand);

		static int binarySearch(const double key, vector<double>& data){
			int low = 0;
			int high = data.size() - 1;
			while(low <= high){
				int mid = (low+high) / 2;
				if(key == data[mid]) return mid;
				if(key > data[mid]) low = mid + 1;
				else high = mid - 1;
			}
			return high;
		}
		uint32_t reversebytes_uint32t(uint32_t value){
			return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
		           (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;
		}
	private:
		void makeFeature(const UserFeature* userfeature, const UserFeature* recyfeature, 
				const uint32_t bridgeNum, const uint64_t bridgeUid, 
				const QMD_MAP& qmd_result, vector<double> &feature);
		
		void featureDiscrete(vector<double>& origin_feature, const map<int,vector<double> >& mm,
				const vector<int>& needDisFea, vector<double>& res_feature);
		
		uint64_t countRpmScore(const vector<double>& feature, const vector<double>& para);
		
		void split_string_map_qmd(const char*& str_input, char sep_char, char second_sep_char,
			   	QMD_MAP& map_ids, uint32_t limit = 0);
		
		int get_qmd_data(DbCompany* p_db_company, uint64_t uid, QMD_MAP& qmd_result);
		int get_qmd_data_lushan(DbCompany* p_db_company, uint64_t uid, QMD_MAP& qmd_result);
};
#endif
