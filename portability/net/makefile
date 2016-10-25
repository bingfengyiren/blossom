include ../make.inc

OBJECTS = IfAddr.o

LIB_OUT = libfyNet.a


.PHONY: all 

all: $(LIB_OUT)

$(LIB_OUT): $(OBJECTS)
	ar -rv $@ $(OBJECTS)

.SUFFIXES: .o .cpp .h

.cpp.o: 
	$(CXX) $(CFLAGS) -c $<


.PHONY: clean

clean: 
	rm -rf $(LIB_OUT)
	rm -rf $(OBJECTS)


