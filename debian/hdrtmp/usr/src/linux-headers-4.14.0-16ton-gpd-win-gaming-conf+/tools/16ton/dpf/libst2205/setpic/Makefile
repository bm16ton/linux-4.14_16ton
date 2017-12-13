SRC	=	main.c
OBJ	=	main.o
CFLAGS	=	-g -Wall -I../libst2205
LIBS	=	-lgd -L../libst2205 -lst2205 

all:	setpic

setpic:	$(OBJ) $(SRC)
	gcc -o $(@) $(OBJ) $(LIBS) $(FLAGS)

clean:	
	rm -f $(OBJ) setpic
