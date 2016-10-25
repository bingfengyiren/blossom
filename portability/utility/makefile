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
LIB = -L./ 

OBJS = Timer.o
OBJS += TimeFmt.o
OBJS += StringArray.o
OBJS += Config.o
LIB_OUT = $(LIB_OUT_PATH)/libbase.a

EXAMPLES_PATH = ./example  
EXAMPLES_BIN = $(EXAMPLES_PATH)/test_timer

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
