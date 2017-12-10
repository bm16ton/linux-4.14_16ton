/*
    Simple, QaD tool to look for the contents of a file inside another file.
    Copyright (C) 2008 Jeroen Domburg <jeroen@spritesmods.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int f1,f2,r,size,size2,x,y;
    char *buff, *lookfor;
    struct stat sbuff;
    int outfmt=0;
    
    if (argc!=3 && argc!=4) {
	printf("Usage: %s file1 file2 [-h]\nReturns the address of the occurences of the contents of file2 in file1. -h makes it return the address in hex instead of dec.\n",argv[0]);
	exit(0);
    }
    
    if (argc==4) {
	if (strcmp(argv[3],"-h")==0) {
	    outfmt=1;
	} else {
	    printf("Invalid switch %s\n",argv[3]);
	    exit(1);
	}
    }
    
    r=stat(argv[1],&sbuff);
    if (r!=0) {
	perror("Couldn't stat file1.\n");
	exit(1);
    }
    size=sbuff.st_size;
    buff=malloc(size);
    if (buff==NULL) {
	perror("Couldn't malloc bytes for file1.\n");
    }
    
    r=stat(argv[2],&sbuff);
    if (r!=0) {
	perror("Couldn't stat file2.\n");
	exit(1);
    }
    size2=sbuff.st_size;
    lookfor=malloc(size2);
    if (lookfor==NULL) {
	perror("Couldn't malloc bytes for file2.\n");
    }
    
    
    f1=open(argv[1],O_RDONLY);
    if (f1<0) {
	perror("Couldn't open file1.\n");
	exit(1);
    }
    read(f1,buff,size);
    close(f1);
    
    f2=open(argv[2],O_RDONLY);
    if (f2<0) {
	perror("Couldn't open file2.\n");
	exit(1);
    }
    read(f2,lookfor,size2);
    close(f2);

//    fprintf(stderr,"Looking for 0x%x bytes...\n",size2);
    for (x=0; x<(size-size2); x++) {
	y=0;
	while (y<size2 && buff[x+y]==lookfor[y]) {
	    y++;
//	    printf("%x",y);
	}
	if (y==size2) {
	    if (outfmt==1) {
		printf("%04x\n",x);
	    } else {
		printf("%i\n",x);
	    }
	}
    }
    
    exit(0);
}
