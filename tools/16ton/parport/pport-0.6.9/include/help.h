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

#include <string.h>
#include <stdlib.h>

#include "defext.h"

extern int help(const char *help)
{

	if(!strcmp(help, "invalid"))
	{
		fprintf(stderr, "Invalid pin numbers.						\n");
		exit(1);
	}

return 0;
}

/*	EOF	*/
