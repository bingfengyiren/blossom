DEBUG = 0
CXX = g++
ifneq ($(DEBUG),1)
	CFLAGS = -Wall -W -fpic -std=c++11 -O1
else
	CFLAGS = -DDEBUG -g -Wall -W -fpic -std=c++11  -O1
endif

INC_PATH = -I../
INC_PATH += -I../../utility
INC_PATH += -I../../math
INC_PATH += -I/usr/local/include

LIB_PATH = -L../
LIB_PATH += -L../../utility
LIB_PATH += -L../../math
LIB_PATH += -L/usr/local/lib

LINK = -lmodel -lmath -lbase -lglog -lgflags

OBJS = test_pattern.o
OBJS += test_FM.o

EXES = ./test_FM
EXES += ./test_pattern

.PHONY: all

all:$(EXES)

./test_FM:test_FM.o
	$(CXX) $(CFLAGS) -o $@ $^ $(INC_PATH) $(LIB_PATH) $(LINK)
./test_pattern:./test_pattern.o
	$(CXX) $(CFLAGS) -o $@ $^ $(INC_PATH) $(LIB_PATH) $(LINK)

.SUFFIXES: .o .cpp .h

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $< $(INC_PATH) $(LIB_PATH) $(LINK)

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf $(EXES)
