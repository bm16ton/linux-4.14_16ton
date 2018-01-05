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

/* System configuration */

#include "config.h"

/*	Basic includes.	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <popt.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

/*	Our includes.	*/

#include "defines.h"

int
main (const int argc, const char **argv)
{

  enum bool
  {
    false = 0,
    true
  } verboose = false;

  int itra=0;
  int result=0;
  int pid=0, status=0;
  char *optBuf=NULL;
  const char *_port_;
  const char *optData=NULL;
  const char *delay=NULL;

  poptContext optCon;

  const struct poptOption optionT[] = {

    {"\b\b  \nFlippers:", 0, 0, NULL, 0, NULL},
    {"set", 's', POPT_ARG_STRING, &optData, 's',
     "set pins", "[P|S]"},
    {"toggle", 't', POPT_ARG_STRING, &optData, 't',
     "toggle pins", "[P]"},
    {"restore", 'r', 0, NULL, 'r',
     "restore status", NULL},
    {"\b\b  \nModifiers:", 0, 0, NULL, 0, NULL},
    {"wait", 'w', POPT_ARG_STRING, &delay, 'w',
     "delay", "[ALL]"},
    {"\b\b  \nInformation:", 0, 0, NULL, 0, NULL},
    {"status", 0, 0, NULL, 'm',
     "print status", NULL},
    {"debug", 0, 0, NULL, 'd',
     "debugging", NULL},
    {"\b\b  \nExtensions:", 0, 0, NULL, 0, NULL},
    {"raw", 0, POPT_ARG_STRING, &optData, 'k',
      "send raw data", "[STRING]"},
    {"\b\b  \nHelp:", 0, 0, NULL, 0, NULL},
    {"help", 'h', 0, NULL, 'h',
     "display quick help", NULL},
    {"man", 0, 0, NULL, 'i',
     "display manual page", NULL},
    {NULL, 0, 0, NULL, 0}

  };

  _port_ = (char *) malloc (10 * sizeof (char));
  optData = (char *) malloc (strlen (ALL) * sizeof (char));
  delay = (char *) malloc (strlen (ALL) * sizeof (char));

  optBuf = (char *) malloc (strlen (*argv) * sizeof (char));


  optCon = poptGetContext (NULL, argc, argv, optionT, 0);

  if (argc < 2)
    {

      poptPrintUsage (optCon, stderr, 0);
      exit (1);

    }

  while ((result = poptGetNextOpt (optCon)) >= 0)
    switch (result)
      {
      case 'h':
        optBuf[itra++] = 'h';
        break;
      case 'i':
	    optBuf[itra++] = 'i';
	    break;
      case 'y':
        optBuf[itra++] = 'y';
        break;
      case 's':
        optBuf[itra++] = 's';
        break;
      case 't':
        optBuf[itra++] = 't';
        break;
      case 'o':
        optBuf[itra++] = 'o';
        break;
      case 'w':
        optBuf[itra++] = 'w';
        break;
      case 'r':
        optBuf[itra++] = 'r';
        break;
      case 'm':
        optBuf[itra++] = 'm';
        break;
      case 'd':
        optBuf[itra++] = 'd';
        break;
      case 'k':
        optBuf[itra++] = 'k';
        break;
	}

  if (result < -1)
    {
      fprintf (stderr, "%s: %s\n",
	       poptBadOption (optCon, POPT_BADOPTION_NOALIAS),
	       poptStrerror (result));
      return 1;
    }
  if(strchr (optBuf, 'i') != NULL)
    {	
  		if((pid=vfork())==0)
				execlp("man", "-a", PACKAGE_NAME, NULL);
			else if(pid<0) fprintf(stdout, "%s", strerror(errno));
			while(wait(&status)<0);
			exit(0);
	} 
  if (strchr (optBuf, 'h') != NULL)
    {
      poptPrintHelp (optCon, stderr, 0);
      fprintf (stderr, "\n");
      fprintf (stderr, "\tP\tvalid: all, none, even, odd,\n\t\trandom or 2,3,4, etc...\n");
      fprintf (stderr, "\tS\tfor serial port: dtr, rts\n");
      fprintf (stderr, "\tALL\tNd N days, Nh N hours,\n\t\tNm N minutes, Ns N seconds.\n");
      exit (0);
    }   

  if (strchr (optBuf, 'd') != NULL)
    {
      verboose = true;
    }
  if (strchr (optBuf, 'w') != NULL)
    {
      result = _getdelay (delay);
    }
  if (strchr (optBuf, 's') != NULL)
    {
      if (verboose)
	    fprintf (stdout, "Set mechanism: %s\n", optData);
      if (strstr(optData, "dst")!=NULL || strstr(optData, "rts")!=NULL)
	    result = _set (optData, SBASE);
      else
	    result = _set (optData);
    }
  else if (strchr (optBuf, 'r') != NULL)
    {
      _restore ();
    }

  else if (strchr (optBuf, 't') != NULL)
    {

      if (verboose == true)
	fprintf (stdout, "Toggle pins: %s\n", optData);
      result = _toggle (optData);

    }
  else if (strchr (optBuf, 'm') != NULL)
    {

      result = _status ();

    }
  else if (strchr (optBuf, 'k') != NULL)
    {
      result = raw_snd (optData);
    }
  else
    poptPrintHelp (optCon, stderr, 0);

  if (optCon)
    poptFreeContext (optCon);

  return 0;

}

/*	EOF	*/
