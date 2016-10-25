#ifndef _GLOBAL_FEATUREWEIGHT_DB_INTERFACE_HEADER_
#define _GLOBAL_FEATUREWEIGHT_DB_INTERFACE_HEADER_
#include <stdlib.h> 
#include <vector>
#include <string>
#include "utility.h"
#include "base_define.h"
#include "global_db_interface.h"
#ifdef __GNUC__
#include <ext/hash_map>
#else
#include <hash_map>
#endif

class RPM_Model{
	public:
        vector<double> para;
        map<int,vector<double> > mm;
        vector<int> needDisFea;
};


class GlobalFeatureWeightDbInterface : public GlobalDbInterface{
	public:
		GlobalFeatureWeightDbInterface(const GlobalDbInfoStruct& global_db_info_struct):
		GlobalDbInterface(global_db_info_struct){
		}
		~GlobalFeatureWeightDbInterface(){
		}
	private:
		RPM_Model lanv;
		RPM_Model chengv;
		RPM_Model puhu;
	
	public:
		//RPM_Model lanv;
		//RPM_Model chengv;
		//RPM_Model puhu;
	
		bool is_exist(uint64_t id){
			return true;
		}
		RPM_Model getChengV()
		{
			return chengv;
		}

		RPM_Model getLanV()
		{
			return lanv;
		}

		RPM_Model getPuhu()
		{
			return puhu;
		}

		void loadModel(const char *file)
		{
			FILE *fp  = fopen(file,"r");
			int chengv_len = 74 , lanv_len = 72 , puhu_len = 70;
			for(int i = 0 ; i < chengv_len ; i++) chengv.para.push_back(0);
			for(int i = 0 ; i < lanv_len ; i++) lanv.para.push_back(0);
			for(int i = 0 ; i < puhu_len ; i++) puhu.para.push_back(0);
			
			char buf[8092];
			while(fgets(buf,sizeof(buf),fp))
			{
                buf[strlen(buf)-1] = '\0';

                char *buf1 = strtok(buf,"_");
                char *buf2 = strtok(NULL,"_");

                if(isdigit(buf2[0]))
                {
					int i = atoi(strtok(buf2,":"));
					double j = atof(strtok(NULL,":"));
					if(!strcmp(buf1,"chengv"))
						chengv.para[i] = j;
					else
					{
						if(!strcmp(buf1,"lanv"))
							lanv.para[i] = j;
						else
							puhu.para[i] = j;
					}
                }
                else
                {
					strtok(buf2,":");
					char *p = strtok(NULL,":");
					char *q = strtok(p," ");
					
					int index = atoi(q);
					if(!strcmp(buf1,"chengv"))
						chengv.needDisFea.push_back(index);
					else
					{
						if(!strcmp(buf1,"lanv"))
							lanv.needDisFea.push_back(index);
						else
							puhu.needDisFea.push_back(index);
					}
					while((q = strtok(NULL, " ")))
					{
						if(!strcmp(buf1,"chengv"))
							chengv.mm[index].push_back(atof(q));
						else
						{
							if(!strcmp(buf1,"lanv"))
								lanv.mm[index].push_back(atof(q));
							else
								puhu.mm[index].push_back(atof(q));
						}
					}
				}
				memset(buf,'\0',sizeof(buf));
			}
			LOG_DEBUG("loaded feature weight info sucess");
			fclose(fp);
		}
		int load_db_config(){
			char* db_file_name = global_db_info_struct_.db_file_name_;
			LOG_DEBUG("feature weight info file:%s",db_file_name);
			if(NULL == db_file_name)
				return -1;
			loadModel(db_file_name);
			return 1;
		}
};

#endif
