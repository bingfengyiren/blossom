DEBUG = 0
CXX = g++
ifneq ($DEBUG,1)
	CFLAGS = -Wall -W -fpic -std=c++11 -O1
else
	CFLAGS = -DDEBUG -g -Wall -W -fpic -std=c++11 -O1
endif

INC_OUT_PATH = ./
LIB_OUT_PATH = ./

INC = -I./
INC += -I/usr/local/include
LIB = -L./ 
LIB += -L/usr/local/lib

OBJS = RpcServer.o
OBJS += ThriftTest.o
OBJS += ThriftTest_types.o

LINK = -lfolly -lgflags -lwangle

EXES = RpcServer

.PHONY:all

all:$(EXES)

./RpcServer : $(OBJS)
	$(CXX) -o $@ $<  $(CFLAGS) $(INC) $(LIB) $(LINK)

.SUFFIXES: .o .cpp .h

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $< $(INC)
