DEBUG = 0
CXX = g++
ifneq ($DEBUG,1)
	CFLAGS = -Wall -W -fpic -O1 -std=c++14
else
	CFLAGS = -DDEBUG -g -Wall -W -fpic -O1 -std=c++14
endif

INC_PATH = -I./
INC_PATH += -I/usr/local/include

LIB_PATH = -L./
LIB_PATH += -L/usr/local/lib

LINK = -lfolly -lwangle -lpthread

EXES = EchoServer
EXES += EchoClient

.PHONY:all

all:$(EXES)

./EchoServer:EchoServer.cpp
	$(CXX) $(CFLAGS) -o $@ $< $(INC_PATH) $(LIB_PATH) $(LINK)

./EchoClient:EchoClient.cpp
	$(CXX) $(CFLAGS) -o $@ $< $(INC_PATH) $(LIB_PATH) $(LINK)

.SUFFIXES: .o .cpp .h

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $< $(INC)
