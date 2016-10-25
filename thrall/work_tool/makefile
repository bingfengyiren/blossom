ifeq ($(ARCH),i686)
    CC=gcc -m32
    CFLAGS=-Wall -shared -fPIC -g -O3 -fopenmp 
else
    CC=gcc
    CFLAGS=-Wall -shared -fPIC -g -O3 -fopenmp
endif


LAB_COMMON_ROOT_PATH = ../
EROS_LIB_ROOT_PATH = ../eros/lib/


LINK_PATH = -L$(LAB_COMMON_ROOT_PATH)/depend/xbson/lib 
LINK_PATH += -L$(LAB_COMMON_ROOT_PATH)/depend/feature_pool_bin/lib 
LINK_PATH += -L$(LAB_COMMON_ROOT_PATH)/depend/mapdb/lib 
LINK_PATH += -L$(LAB_COMMON_ROOT_PATH)/depend/woo/lib 
LINK_PATH += -L$(LAB_COMMON_ROOT_PATH)/depend/metis/lib 
LINK_PATH += -L/usr/local/lib
LINK_PATH += -L$(EROS_LIB_ROOT_PATH)/eros_lib/lib
LINK_PATH += -L$(EROS_LIB_ROOT_PATH)/jsoncpp-0.6.0/lib
LINK_PATH += -L$(EROS_LIB_ROOT_PATH)/fengyoung_api/lib


LINK = -leros -ljsoncpp -lFengyoungAPI
LINK += -lxbson -lmemcached -lapifp -lmapdb -lcurl -lssl -ljson -lmetis -lwoo -lstdc++ -lz -lhiredis -lpthread


INC_PATH = -I$(LAB_COMMON_ROOT_PATH)/depend/xbson/include
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/depend/feature_pool_bin/include
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/depend/uthash
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/depend/mapdb/include
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/depend/woo/include
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/depend/metis/include
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/depend/metis/include/neural_network
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/depend/metis/include/utility
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/depend/metis/include/prediction
INC_PATH += -I/usr/local/include/json
INC_PATH += -I/usr/local/include/hiredis
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/include
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/include/db
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/include/utility
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/include/work
INC_PATH += -I$(LAB_COMMON_ROOT_PATH)/include/algorithm
INC_PATH += -I$(EROS_LIB_ROOT_PATH)/eros_lib/include
INC_PATH += -I$(EROS_LIB_ROOT_PATH)/jsoncpp-0.6.0/include
INC_PATH += -I$(EROS_LIB_ROOT_PATH)/fengyoung_api/include
INC_PATH += -I./src


OUT_PATH = ../lib

ALL_BIN = $(OUT_PATH)/Video_Manager_WorkInterface.so


all:$(ALL_BIN)

$(OUT_PATH)/Video_Manager_WorkInterface.so : ./src/Video_Manager_WorkInterface.o $(LAB_COMMON_ROOT_PATH)/src/ini_file.o
	test -d $(OUT_PATH) || mkdir $(OUT_PATH)
	$(CC) $(CFLAGS) -g -o $@ $^ $(LINK_PATH) $(LINK)

%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg

clean:
	rm $(ALL_BIN) ./src/*.o


