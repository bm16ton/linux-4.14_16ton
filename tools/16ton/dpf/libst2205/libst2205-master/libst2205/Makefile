SRC	=	st2205.c
OBJ	=	st2205.o
HEADERS	=	st2205.h
CFLAGS	=	-W -Wall -g -fPIC -O2
LIBS	=	

all:	libst2205.so

libst2205.so:	$(OBJ) $(SRC) $(HEADERS) 
	ar -rv libst2205.a $(OBJ) 
	gcc -shared -Wl,-soname,libst2205.so.1 -o libst2205.so $(OBJ) $(LIBS)

clean:	
	rm -f $(OBJ) test *.a *.so

install: libst2205.so
	test -z "/usr/local/lib" || /bin/mkdir -p "/usr/local/lib"
	test -z "/usr/local/include" || /bin/mkdir -p "/usr/local/include"
	install libst2205.so /usr/local/lib
	install st2205.h /usr/local/include
	ldconfig
