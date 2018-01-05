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

#include "../config.h"

/*	Basic includes.	*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/io.h>
#include <errno.h>
#include <sys/stat.h>

/*	Our includes.	*/

#include "defines.h"
#include "defext.h"
#include "pperror.h"

struct timespec *nano;
const int podat[] = { 0, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 255 };

enum bool
{
  false = 0,
  true
} serial = false;

/*	Main signal processing routine.	*/

int
send (int value, ...)
{

  int addr;
  int nofork;
  int pid, status;
  va_list argp;

  va_start (argp, value);

  nofork = va_arg (argp, int);

  if (iopl (3))
    {
      fprintf (stdout, DOCS, strerror (errno));
      exit (1);
    }
  if ((pid = fork ()) < 0)
    {
      fprintf (stdout, "%s\n", strerror (errno));
      exit (1);
    }
  else if (pid == 0)
    {
      signal (SIGTERM, shandler);
      signal (SIGINT, shandler);
      if (nano != NULL)
	{
	  nanosleep (nano, NULL);
	}
      if (serial)
	{
	  outb (value, SBASE);
	  exit (0);
	}
      if (value == 11 || value == 0)
	{
	  outb (podat[value], PBASE);
	  shandler (0);
	  exit (0);
	}
      else
	{
	  outb (value, PBASE);
	  shandler (0);
	  exit (0);
	}
    }
  if (nofork == 1)
    while (wait (&status) != pid);
  return 0;
}

/*	Signal handler and pin saving routine.	*/

void
shandler (int signal)
{

  FILE *fp;
  char *shm = NULL;
  char *tpins = NULL;
  int itra = 0;

  tpins = (char *) malloc (8 * sizeof (char));

  shm = getmem (KEY);

  if (mkdir (__PPDIR__, S_IRWXU | S_IRWXG) < 0)
    if (errno != EEXIST)
      {
	fprintf (stderr, DOCS, strerror (errno));
	exit (1);
      }

  if ((fp = fopen (OUTPUT, "w")) == NULL)
    {
      fprintf (stderr, NORS, PACKAGE_NAME);
      exit (1);
    }
  else

    fprintf (fp, "ON: ");
  for (itra = 0; itra < strlen (shm); itra++)
    fprintf (fp, " %d ", shm[itra] - '0');
  fprintf (fp, "\nOFF: ");
  for (itra = 0; itra < strlen (ALL); itra++)
    if (strchr (shm, ALL[itra]) == NULL)
      fprintf (fp, " %d ", ALL[itra] - '0');
  fprintf (fp, "\n");
  fclose (fp);

  return;
}

/*	Insert sort with integrated pin checker */

char *
spurify (char *array)
{
  register int itra, itrb;
  char tmp;
  char *tmparray;

  for (itra = 1; itra <= strlen (array) - 1; itra++)
    {
      tmp = array[itra];
      itrb = itra;
      while (tmp < array[itrb - 1])
	{
	  array[itrb] = array[itrb - 1];
	  itrb--;
	}
      array[itrb] = tmp;
    }

  for (itra = 0; itra <= strlen (array) - 1; itra++)
    if (array[itra] == array[itra + 1])
      array[itra] = '0';

  tmparray = (char *) malloc (strlen (array));

  for (itra = 0, itrb = 0; itra <= strlen (array); itra++)
    if (array[itra] - '0' >= 2 && array[itra] - '0' <= 9)
      tmparray[itrb++] = array[itra];

  memset (array, 0, strlen (ALL));
  strcpy (array, tmparray);

  return array;
}

/*	Grab the delay	*/

int
_getdelay (const char *_delay)
{

  int itra = 0, itrb = 0;
  char *multyp;

  nano = (struct timespec *) malloc (1 * sizeof (nano));
  multyp = (char *) malloc (strlen (ALL) * sizeof (char));

  for (itra = 0, itrb = 0, itrb = 0; itra <= 2; itra++)
    {
      if (isdigit (_delay[itra]))
	multyp[itrb++] = _delay[itra];
      else if (isalpha (_delay[itra]))
	switch (_delay[itra])
	  {
	  case 'd':
	    nano->tv_sec = (time_t) ( atoi (multyp) * DAY );
	    break;
	  case 'h':
	    nano->tv_sec = (time_t) ( atoi (multyp) * HOUR );
	    break;
	  case 'm':
	    nano->tv_sec = (time_t) ( atoi (multyp) * MINUTE );
	    break;
	  case 's':
	    nano->tv_sec = (time_t) atoi (multyp);
	    break;
	  default:
	    fprintf (stderr, ERDL, PACKAGE_NAME);
	    break;
	  }
      else
	break;
    }
  return 0;
}


/*	Get parity pins.	*/

char *
getprime (int prime)
{

  char *all, *result;
  int itra = 0, itrb = 0;

  all = (char *) malloc (strlen (ALL) * sizeof (char));
  result = (char *) malloc (strlen (ALL) * sizeof (char));

  strcpy (all, ALL);

  for (itra = 0, itrb = 0; itra < strlen (ALL); itra++)
    if (prime == 0)
      {
	if ((all[itra] - '0') % 2 == 0)
	  result[itrb++] = all[itra];
      }
    else
      {
	if ((all[itra] - '0') % 2 != 0)
	  result[itrb++] = all[itra];
      }

  return result;
}

/*	Get random pins.	*/

char *
getrandom (void)
{

  char *result, *temp;
  int itra = 0;

  result = (char *) malloc (strlen (ALL) * sizeof (char));
  temp = (char *) malloc (1 * sizeof (char));

  srand ((unsigned int) time (NULL) + rand ());

  for (itra = 1; itra < strlen (ALL); itra++)
    {
      sprintf (temp, "%x", rand () % 9);
      strcat (result, temp);
    }
  result = spurify (result);

  return result;
}


/*	Direct pin setting routine.	*/

int
_set (const char *_pins, ...)
{
  va_list argp;
  int itrb = 0, itra = 0;
  int pad = 0;
  int sum = 0;
  char *ofpins;
  char *shm = NULL;

  ofpins = (char *) malloc (strlen (ALL) * sizeof (char));

  shm = getmem (KEY);

  va_start (argp, _pins);

  switch (va_arg (argp, int))
    {
    case SBASE:
      serial = true;
      break;
    }

  if (!strcmp (_pins, "none"))
    {

      if (serial)
	send (0);
      else
	{
	  strcpy (shm, "");
	  send (sum = 0);
	}
      return 0;

    }
  else if (!strcmp (_pins, "all"))
    {

      if (serial)
	send (3);
      else
	{
	  strcpy (shm, ALL);
	  send (sum = 11);
	}
      return 0;

    }
  else if (!strcmp (_pins, "even") && !serial)
    {

      strcpy (shm, getprime (0));
      send (sum = 85);
      return 0;

    }
  else if (!strcmp (_pins, "odd") && !serial)
    {

      strcpy (shm, getprime (1));
      send (sum = 170);
      return 0;

    }
  else if (!strcmp (_pins, "random") && !serial)
    {

      strcpy (ofpins, getrandom ());
      for (itra = 0; itra < strlen (ofpins); itra++)
	sum += podat[ofpins[itra] - '0'];
      strcpy (shm, ofpins);
      free (ofpins);
      send (sum);
      return 0;

    }
  else
    {

      if (!serial)
	{
	  for (pad = 0, itrb = 0; pad <= strlen (_pins); pad += 2, itrb++)
	    ofpins[itrb] = _pins[pad];
	  ofpins = spurify (ofpins);

	  if (strlen (ofpins) <= 0)
	    {
	      fprintf (stderr, NOPN, PACKAGE_NAME);
	      exit (1);
	    }
	}

      if (serial)
	{
	  if (strstr (_pins, "dtr") != NULL && strstr (_pins, "rts") != NULL)
	    {
	      send (1);
	    }
	  else if (strstr (_pins, "dtr") != NULL)
	    {
	      send (2);
	    }
	  else if (strstr (_pins, "rts") != NULL)
	    {
	      send (3);
	    }
	  else
	    {
	      fprintf (stderr, NOPN, PACKAGE_NAME);
	      exit (1);
	    }
	}

      if (!serial)
	{
	  for (itra = 0, sum = 0; itra < strlen (ofpins); itra++)
	    sum += podat[ofpins[itra] - '0'];
	  strcpy (shm, ofpins);
	  free (ofpins);
	  send (sum);
	}
    }

  return 0;

}

/*	Pin toggle function.		*/

int
_toggle (const char *_pins)
{

  int itra = 0, itrb = 0;
  int pad = 0;
  int sum = 0;
  char *ofpins, *tpins;
  char *shm = NULL;

  tpins = (char *) malloc (strlen (ALL) * sizeof (char));
  ofpins = (char *) malloc (strlen (ALL) * sizeof (char));

  shm = getmem (KEY);

  if (!strcmp (_pins, "all"))
    strcpy (tpins, ALL);
  else if (!strcmp (_pins, "even"))
    strcpy (tpins, getprime (0));
  else if (!strcmp (_pins, "odd"))
    strcpy (tpins, getprime (1));
  else if (!strcmp (_pins, "random"))
    strcpy (tpins, getrandom ());
  else
    {
      for (pad = 0, itrb = 0; pad <= strlen (_pins); pad += 2, itrb++)
	tpins[itrb] = _pins[pad];
      tpins = spurify (tpins);

      if (strlen (tpins) <= 0)
	{
	  fprintf (stderr, NOPN, PACKAGE_NAME);
	  exit (1);
	}
    }
  memset (ofpins, 0, strlen (ALL));
  if (strlen (shm) == 0)
    strcpy (ofpins, tpins);
  else if (!strcmp (shm, tpins))
    ofpins[0] = '0';
  else
    {

      for (itra = 0, itrb = 0; itra <= strlen (tpins) || itra <= strlen (shm);
	   itra++)
	{
	  if (strchr (shm, tpins[itra]) == NULL)
	    ofpins[itrb++] = tpins[itra];
	  if (strchr (tpins, shm[itra]) == NULL)
	    ofpins[itrb++] = shm[itra];

	}
    }

  free (tpins);
  ofpins = spurify (ofpins);

  for (itra = 0; itra < strlen (ofpins); itra++)
    sum += podat[ofpins[itra] - '0'];
  strcpy (shm, ofpins);
  free (ofpins);
  send (sum);
  return 0;
}

/*	Status report routine.		*/

int
_status (void)
{

  FILE *fp;
  char *shm = NULL;
  char *ofpins;

  ofpins = (char *) malloc (5 * strlen (ALL) * sizeof (char));
  shm = getmem (KEY);

  fprintf (stdout, "PPort %s: Status Report\n\n", PACKAGE_VERSION);

  if ((fp = fopen (OUTPUT, "r")) == NULL)
    {
      fprintf (stdout, "No pins active or pending.\n");
    }
  else
    {
      fgets (ofpins, 5 * strlen (ALL), fp);
      ofpins = spurify (ofpins);
      fprintf (stdout, "Pending: ");
      if (strlen (shm) != 0 && strcmp (shm, ofpins))
	fprintf (stdout, "%s\n", shm);
      else
	fprintf (stdout, "none\n");
      fprintf (stdout, "Activated: ");
      if (fp == NULL)
	fprintf (stdout, "none\n");
      else
	{

	  if (strlen (ofpins) >= 1)
	    fprintf (stdout, "%s\n", ofpins);
	  else
	    fprintf (stdout, "none\n");

	}
      memset (ofpins, 0, 5 * strlen (ALL));
      fprintf (stdout, "Deactivated: ");
      fgets (ofpins, 5 * strlen (ALL), fp);
      ofpins = spurify (ofpins);
      if (fp == NULL)
	fprintf (stdout, "none\n");
      else
	{
	  if (strlen (ofpins) >= 1)
	    fprintf (stdout, "%s\n", ofpins);
	  else
	    fprintf (stdout, "none\n");
	}
      fclose (fp);
    }
  fprintf (stdout, "\n");

  return 0;
}

/*	Restoring routine.		*/

void
_restore (void)
{

  int itra = 0;
  int sum = 0;
  char *tpins;
  FILE *fp;
  char *shm = NULL;

  tpins = (char *) malloc (5 * strlen (ALL) * sizeof (char));

  shm = getmem (KEY);

  if ((fp = fopen (OUTPUT, "r")) == NULL)
    {
      fprintf (stderr, NORS, PACKAGE_NAME);
      exit (1);
    }
  else
    {

      fgets (tpins, 5 * strlen (ALL), fp);
      tpins = spurify (tpins);
      if (tpins == NULL || strlen (tpins) <= 0)
	{
	  fprintf (stderr, NORS, PACKAGE_NAME);
	  exit (1);
	}
      strcpy (shm, tpins);
      for (itra = 0; itra < strlen (tpins) - 1; itra++)
	sum += podat[tpins[itra] - '0'];
      free (tpins);
      send (sum);
    }

}

/* Raw data */
int
raw_snd(char *data)
{
  int itra=0;
  
  if (iopl(3))
    {
      fprintf (stdout, DOCS, strerror(errno));
      exit(1);
    }

  for(itra=0; itra<=strlen(data); itra++)
    {
      outb(data[itra], PBASE);
    }

  fprintf(stdout, RAWS, PACKAGE_NAME, itra);
  return 0;
}
  

/*	EOF	*/
