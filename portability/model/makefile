DEBUG = 0
CXX = g++
ifneq ($DEBUG,1)
	CFLAGS = -Wall -W -fpic -O1
else
	CFLAGS = -DDEBUG -g -Wall -W -fpic -O1
endif

INC_OUT_PATH = ./
LIB_OUT_PATH = ./

INC = -I./prediction
INC += -I../math
INC += -I./neural_network
INC += -I../utility
LIB = -L./prediction
LIB += -L../math
LIB += -L./neural_network
LIB += -L../utility

SRC_PRED_PATH = ./prediction
SRC_NN_PATH = ./neural_network

OBJS = $(SRC_PRED_PATH)/Model.o
OBJS += $(SRC_PRED_PATH)/Model_FM.o
OBJS += $(SRC_PRED_PATH)/Model_FMSNN.o
OBJS += $(SRC_PRED_PATH)/Model_MLP.o
OBJS += $(SRC_PRED_PATH)/Model_Perceptron.o
OBJS += $(SRC_NN_PATH)/Activation.o
OBJS += $(SRC_NN_PATH)/FM.o
OBJS += $(SRC_NN_PATH)/FMSNN.o
OBJS += $(SRC_NN_PATH)/MLP.o
OBJS += $(SRC_NN_PATH)/Pattern.o
OBJS += $(SRC_NN_PATH)/Perceptron.o
OBJS += $(SRC_NN_PATH)/TypeDefs.o

LIB_OUT = $(LIB_OUT_PATH)/libmodel.a

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
