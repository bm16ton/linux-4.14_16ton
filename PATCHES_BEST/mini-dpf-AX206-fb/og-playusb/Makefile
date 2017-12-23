CC = gcc
CFLAGS = -O2
LDFLAGS = -ljpeg -lusb -lrt

OBJ=playusb

all: $(OBJ)

playusb: playusb.o fbgrab.o
	${CC} ${CFLAGS} ${LDFLAGS} playusb.o fbgrab.o -o playusb


playusb.o: playusb.c
	${CC} ${CFLAGS} ${LDFLAGS} -c playusb.c

fbgrab.o: fbgrab.c
	${CC} ${CFLAGS} ${LDFLAGS} -c fbgrab.c

clean:
	rm -rf *o playusb
