include Makefile.in
LIB=../lib/libwoo.a
all:$(LIB)

$(LIB):tcpserver.o log.o scgi.o config.o binaryclient.o string.o crc.o reactor.o
	test -d ../lib || mkdir ../lib
	ar -rc $@ $^
	ranlib $@
	
%.o:%.cpp
	$(CC) $(CFLAGS) -D__STDC_FORMAT_MACROS -c -o $@ $^ -I../include/

clean:
	rm $(LIB) *.o
