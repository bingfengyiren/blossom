#! /bin/sh

if [ $# -eq 1 ]
then
	if [ "$1" = "clean" ]
	then
		#echo "cd ./artemis/src/; make clean; cd -"
		#cd ./artemis/src/; make clean; cd -
		echo "cd ./artemis_work_tool/; make clean; cd -"
		cd ./artemis_work_tool/; make clean; cd -
		echo "make clean"
		make clean
	else
		echo "Usage: $0 [option]"
		exit -1
	fi
elif [ $# -eq 0 ]
then
	#echo "cd ./artemis/src/; make clean; make; cd -"
	#cd ./artemis/src/; make clean; make; cd -
	echo "cd ./artemis_work_tool/; make clean; make; cd -"
	cd ./artemis_work_tool/; make clean; make; cd -
	echo "make clean; make"
	make clean; make
else
	echo "Usage: $0 [option]"
	exit -1
fi



