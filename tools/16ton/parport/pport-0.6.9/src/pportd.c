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

#include <popt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <syslog.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/io.h>

/*	Our includes.	*/

#include "defines.h"
#include "defext.h"
#include "errorp.h"

struct config
{
  int status[LINEZ];
  char *enable[LINEZ];
  char *disable[LINEZ];
} *cfo;

int
main (const int argc, const char **argv)
{

  enum bool
  {
    false = 0,
    true
  } dofork = true, sylog = true;

  struct timespec *nano;
  const static int pidat[] = { 63, 127, 31, 15, 7 };

  /* Global variables. */

  register int itra = 0;
  int resolution = 1;
  int pid = 0, status = 0;
  int ipin[5], pipin[5], save[5];
  char result;
  volatile int data;
  poptContext optCon;

  /* Variables governing thread creation. */

  pthread_t tida, tidb;

  /* Cmd-line parser. */

  const struct poptOption optionT[] = {
    {"nosyslog", 'y', 0, NULL, 'y',
     "syslog will not be used for logging", NULL},
    {"foreground", 'f', 0, NULL, 'f',
     "daemon will run in the foreground", NULL},
    {"refresh", 'r', POPT_ARG_INT, &resolution, 'r',
     "status check interval in ms", NULL},
    POPT_AUTOHELP {NULL, 0, 0, NULL, 0, NULL}
  };

  optCon = poptGetContext (NULL, argc, argv, optionT, 0);

  while ((result = poptGetNextOpt (optCon)) >= 0)
    switch (result)
      {
      case 'y':
	    sylog = false;
	    break;
      case 'f':
	    dofork = false;
	    break;
      }

  if (result < -1)
    {
      fprintf (stderr, "%s: %s\n",
	       poptBadOption (optCon, POPT_BADOPTION_NOALIAS),
	       poptStrerror (result));
      return 1;
    }

  /* Setup variables accordingly. */

  nano = (struct timespec *) malloc (1 * sizeof (nano));

  nano->tv_sec = 0.001 * resolution;
  nano->tv_nsec = 0;

  /* Get configuration and get the party going. */

  if ((cfo = parse_cf ()) == NULL)
    {
      fprintf (stderr, PARS, PACKAGE_NAME);
      exit (1);
    }

  if (iopl (3))
    {
      fprintf (stderr, DOCS, strerror (errno));
      exit (1);
    }
  else
    {
      fprintf (stdout, "Port 0x%x availible.\n", PBASE);
    }

  fprintf (stdout, "Switch to syslog: ");
  if (sylog == true)
    {
      openlog ("pportd", LOG_PID | LOG_NDELAY, LOG_DAEMON);
      syslog (LOG_INFO, "Starting up...");
      fprintf (stdout, "yes\n");
    }
  else
    fprintf (stdout, "no\n");

  fprintf (stdout, "Switch to daemon mode: ");
  if ((pid = fork ()) < 0)
    {
      fprintf (stderr, "%s", strerror (errno));
      exit (1);
    }
  else if (pid == 0)
    {
      signal (SIGHUP, dhash);
      signal (SIGINT, dhash);
      fprintf (stdout, "done\n");
      while ((data = inb (PBASE + 1)))
	{
	  nanosleep (nano, NULL);
	  for (itra = 0; itra <= 4; itra++)
	    {
	      if (itra == 1)
		ipin[itra] = 0;
	      else
		ipin[itra] = 1;
	    }
	  for (itra = 0; itra <= 4; itra++)
	    {
	      if (data > pidat[itra])
		{
		  if (itra == 1)
		    ipin[itra] = 1;
		  else
		    ipin[itra] = 0;
		  data -= pidat[itra] + 1;
		}
	    }
	  for (itra = 0; itra <= 4; itra++)
	    {
	      if (ipin[itra] == 1 && pipin[itra] != ipin[itra])
		{
		  if (cfo->enable[itra] != NULL)
		    {
		      if (pthread_create
			  (&tida, NULL, (void *) run, cfo->enable[itra]))
			{
			  if (sylog)
			    syslog (LOG_INFO, "%s", strerror (errno));
			  else
			    fprintf (stderr, "%s", strerror (errno));
			  exit (1);
			}
		      if (!dofork)
			fprintf (stdout, "enable: %s\n", cfo->enable[itra]);
		      if (sylog)
			syslog (LOG_INFO, "enable: %s", cfo->enable[itra]);
		    }
		  save[itra] = cfo->status[itra];
		  pipin[itra] = ipin[itra];
		}
	      else if (ipin[itra] == 0)
		{
		  if (cfo->disable[itra] != NULL)
		    {
		      if (pthread_create
			  (&tidb, NULL, (void *) run, cfo->disable[itra]))
			{
			  if (sylog)
			    syslog (LOG_INFO, "%s", strerror (errno));
			  else
			    fprintf (stderr, "%s", strerror (errno));
			  exit (1);
			}
		      if (!dofork)
			fprintf (stdout, "disable: %s\n", cfo->disable[itra]);
		      if (sylog)
			syslog (LOG_INFO, "disable: %s", cfo->disable[itra]);
		    }
		  pipin[itra] = 0;
		  save[itra] = 0;
		}
	    }
	  external_status (save);
	}
    }
  if (!dofork)
    while (wait (&status) != pid);
  return 0;
}

/*	EOF	*/
