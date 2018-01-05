/*
        pport: Small program to interface your computer with the
               parallel port. Inspired from the Coffee-Machine HOWTO
        
        Copyright (C) 2003  Corcalciuc V. Horia

        This library is free software; you can redistribute it and/or
        modify it under the terms of the GNU Lesser General Public
        License as published by the Free Software Foundation; either
        version 2.1 of the License, or (at your option) any later version.
                
        This library is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
        Lesser General Public License for more details.
                                
        You should have received a copy of the GNU Lesser General Public
        License along with this library; if not, write to the Free Software
        Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA      

*/

#include "defines.h"

/*	Output processing.	*/
extern int send(int value, ...);

/*	Purification function.	*/
extern char *spurify(char * array);

/*	Signal handler.		*/
extern void shandler(int signal);

/*	Memory accesser.	*/
extern char *getmem(int key);

/*	Get delay function.	*/
extern int _getdelay(const char *_delay);

/*	Parity pin functon.		*/
extern char *getprime(int parity);

/*	Random numbers.	*/
extern char *getrandom(void);

/*	Maximum routine.	*/
extern int max(int frst, int lst);

/*	Encryption routines.	*/
extern char *aes_encrypt(char *string);
extern char *aes_decrypt(char *string);

/*	Parser function.	*/
extern struct config *parse_cf(void);

/*	Syslog formater.	*/
extern char *sysformat(int pin, char *comm);

/*	Daemon signal handler.	*/
extern void dhash(int signal);

/*	Daemon execution routine.	*/
extern void run(void *arg);

/*	Status writes.			*/
extern int external_status(int save[]);

/*	Direct pin setting routine.	*/
extern int _set(const char *_pins, ... );

/*	Pin strobe function.		*/
extern int _strobe(const char *_pins, const char *arga, const char *argb);

/*	Pin toggle function.		*/
extern int _toggle(const char *_pins);

/*	Status report routine.		*/
extern int _status(void);

/*	Restoring routine.		*/
extern void _restore(void);

/*	EOF	*/
