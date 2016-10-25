#! /bin/sh

if [ $# -ne 1 ]
then
	echo "Usage: $0 <root path>"
	exit -1
fi

root_path=$1

serv_array=(Manager Ranker Predictor RecomServ)

for serv_name in ${serv_array[@]}
do
	bin_path=$root_path/$serv_name/bin
	conf_path=$root_path/$serv_name/conf

	mkdir -p $bin_path
	cp ./bin/lab_common_svr $bin_path
	
	mkdir -p $conf_path
done

log_path=$root_path/log
mkdir -p $log_path

lib_path=$root_path/lib
mkdir -p $lib_path
cp ./lib/Eros_*.so $lib_path


