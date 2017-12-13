CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lrt -lusb -ljpeg -ldpf

OBJ=playusb

all: $(OBJ)

playusb: playusb.o fbgrab.o
	${CC} ${CFLAGS} playusb.o fbgrab.o -o playusb ${LDFLAGS}


playusb.o: playusb.c
	${CC} ${CFLAGS} -c playusb.c  ${LDFLAGS}

fbgrab.o: fbgrab.c
	${CC} ${CFLAGS} -c fbgrab.c ${LDFLAGS}

clean:
	rm -rf *o playusb
