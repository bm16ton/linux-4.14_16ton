/*
    Simple, QaD tool to splice a file into another file
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
    int f1,f2,r;
    long size;
    unsigned int offset;
    char *buff;
    struct stat sbuff;
    
    if (argc!=4) {
	printf("Usage: %s file1 file2 offset\nOverwrites the byte in file1 with those of file2, starting at offset.\n", argv[0]);
	exit(0);
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
    f1=open(argv[1],O_RDONLY);
    if (f1<0) {
	perror("Couldn't open file1.\n");
	exit(1);
    }
    read(f1,buff,size);
    close(f1);
    
    offset= strtol(argv[3], (char **)NULL, 0);
    if (offset>size) {
	printf("Offset bigger than size of file!\n");
	exit(1);
    }
    printf("Splicing in at offset 0x%X.\n",offset);
    f2=open(argv[2],O_RDONLY);
    if (f2<0) {
	perror("Couldn't open file2.\n");
	exit(1);
    }
    read(f2,buff+offset,size);
    close(f2);
    f1=open(argv[1],O_CREAT|O_TRUNC|O_WRONLY);
    if (f1<0) {
	perror("Couldn't open file1 for writing.\n");
	exit(1);
    }
    write(f1,buff,size);
    close(f1);
    printf("All done. Thank you, come again.\n");
    exit(0);
}
