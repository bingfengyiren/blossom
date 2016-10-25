#ifndef _GLOBAL_DB_INTERFACE_FACTORY_HEADER_
#define _GLOBAL_DB_INTERFACE_FACTORY_HEADER_

#include "global_set_db_interface.h"
#include "global_kv_db_interface.h"
#include "global_map_db_interface.h"
#include "global_fp_db_interface.h"
#include "global_fd_db_interface.h"
#include "global_fw_db_interface.h"
#include "global_ad_db_interface.h"


class GlobalDbInterfaceFactory{

	public:
		GlobalDbInterfaceFactory(){}
		~GlobalDbInterfaceFactory(){}

	public:
		GlobalDbInterface* get_global_db_interface(const GlobalDbInfoStruct& db_info_struct){
			GlobalDbInterface* p_db_interface = NULL;
			switch(db_info_struct.db_type_){
				case GLOBAL_SET_DB_TYPE:
					p_db_interface = new GlobalSetDbInterface(db_info_struct);
					break;
				case GLOBAL_KV_DB_TYPE:
					p_db_interface = new GlobalKVDbInterface(db_info_struct);
					break;
				case GLOBAL_MAP_DB_TYPE:
					p_db_interface = new GlobalMapDbInterface(db_info_struct);
					break;
				case GLOBAL_FP_DB_TYPE:
					p_db_interface = new GlobalFpDbInterface(db_info_struct);
					break;
				case GLOBAL_FD_DB_TYPE:
					p_db_interface = new GlobalFeatureDataDbInterface(db_info_struct);
					break;
				case GLOBAL_FW_DB_TYPE:
					p_db_interface = new GlobalFeatureWeightDbInterface(db_info_struct);
					break;
				case GLOBAL_AD_DB_TYPE:
					p_db_interface = new GlobalAdDbInterface(db_info_struct);
					break;
				default:
					break;	
			}
			return p_db_interface;
		}
};

#endif
