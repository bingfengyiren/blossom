DEBUG = 0
CXX = g++
ifneq ($DEBUG,1)
	CFLAGS = -Wall -W -fpic -O1
else
	CFLAGS = -DDEBUG -g -Wall -W -fpic -O1
endif

INC_OUT_PATH = ./
LIB_OUT_PATH = ./

INC = -I./
INC += -I../utility
LIB = -L./ 
LIB += -L../utility

LINK = -lbase

OBJS = RocAnalyzer.o
OBJS += Random.o
OBJS += Matrix.o

LIB_OUT = $(LIB_OUT_PATH)/libmath.a

EXAMPLES_PATH = ./example  
EXAMPLES_BIN = $(EXAMPLES_PATH)/test_roc

.PHONY:all

all:$(LIB_OUT)

$(LIB_OUT):$(OBJS)
	mkdir -p $(LIB_OUT_PATH)
	ar -rv $@ $(OBJS)

#$(EXAMPLES_PATH)/test_timer:$(EXAMPLES_PATH)/test_timer.cpp
#	$(CXX) $(CFLAGS) -o $@ $< -I$(INC_OUT_PATH) -L$(LIB_OUT_PATH) -lbase

.SUFFIXES: .o .cpp .h

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $< $(INC)
