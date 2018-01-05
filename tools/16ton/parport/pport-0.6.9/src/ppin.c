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
#include <syslog.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*	Our includes.	*/

#include "defext.h"
#include "defines.h"


struct config
{
  int status[LINEZ];
  char *enable[LINEZ];
  char *disable[LINEZ];
} *cfo, *cfs;

int fd = -1;

/*	Rehashing subroutine	*/

void
dhash (int signal)
{

  struct flock *locker;
  locker = (struct flock *) malloc (1 * sizeof (struct flock));

  locker->l_type = F_UNLCK;
  locker->l_start = 0;
  locker->l_whence = SEEK_END;
  locker->l_len = 0;

  cfo = malloc (1 * sizeof (struct config));

  if (signal == SIGHUP)
    {
      syslog (LOG_INFO, "Rehashing...");
      if ((cfs = parse_cf ()) == NULL)
	syslog (LOG_INFO, "ERROR: Bad config file.");
      else
	cfo = cfs;
    }
  if (signal == SIGTERM)
    {
      if (fcntl (fd, F_SETLKW, locker) != EBADF)
	{
	  close (fd);
	  syslog (LOG_INFO, "Released files.");
	}
      else
	syslog (LOG_INFO, "ERROR: %s", strerror (errno));
      syslog (LOG_INFO, "Shutting down...");
      exit (0);
    }
  if (signal == SIGINT)
    {
      if (fcntl (fd, F_SETLKW, locker) != EBADF)
	{
	  close (fd);
	  syslog (LOG_INFO, "Released files.");
	}
      else
	syslog (LOG_INFO, "ERROR: %s", strerror (errno));
      syslog (LOG_INFO, "Shutting down...");
      exit (0);
    }
}

/*	Print pin status for external use	*/

int
external_status (int save[])
{

  FILE *fp = NULL;
  int itra = 0;
  char *shm;
  struct flock *locker;

  locker = (struct flock *) malloc (1 * sizeof (struct flock));
  shm = getmem (KEY + 1);

  locker->l_type = F_WRLCK;
  locker->l_start = 0;
  locker->l_whence = SEEK_END;
  locker->l_len = 0;

  if (mkdir (__PPDIR__, S_IRWXU | S_IRWXG) < 0)
    if (errno != EEXIST)
      {
	fprintf (stderr, "%s", strerror (errno));
	exit (1);
      }
  if (fp == NULL || fd < 0)
    if ((fp = fopen (INPUT, "w")) == NULL)
      {
	fprintf (stderr, "%s", strerror (errno));
	exit (1);
      }

  fd = fileno (fp);
  if (fcntl (fd, F_SETLKW, locker) == -1)
    {
      fprintf (stderr, "%s", strerror (errno));
      exit (1);
    }

  fprintf (fp, "ON:");
  for (itra = 0; itra < 5; itra++)
    if (save[itra] != 0)
      fprintf (fp, " %d", save[itra]);
  fprintf (fp, "\n");
  fprintf (fp, "OFF:");
  for (itra = 0; itra < 5; itra++)
    if (save[itra] == 0)
      fprintf (fp, " %d", cfo->status[itra]);
  fprintf (fp, "\n");

  locker->l_type = F_UNLCK;
  fcntl (fd, F_SETLKW, locker);

  fclose (fp);
  return 0;

}

void
run (void *arg)
{
  if (system (arg) < 0)
    syslog (LOG_INFO, "ERROR: %s", strerror (errno));
  pthread_exit (0);
}

/*	Parser function.	*/

struct config *
parse_cf (void)
{

  FILE *fp;
  register int itra = 0, itrb = 0;
  register int n_lines = 0;
  char c;
  char *status, *disable, *store, *enable;
  char *c_data[LINEZ], *s_data[4];
  char **ptrptra;

  cfs = (struct config *) malloc (1 * sizeof (struct config));
  cfo = (struct config *) malloc (1 * sizeof (struct config));
  ptrptra = (char **) malloc (500 * sizeof (char **));
  status = (char *) malloc (200 * sizeof (char *));
  disable = (char *) malloc (200 * sizeof (char *));
  store = (char *) malloc (500 * sizeof (char *));
  enable = (char *) malloc (500 * sizeof (char *));


  if ((fp = fopen (CFGFILE, "r")) == NULL)
    {
      fprintf (stderr, "%s : %s\n", CFGFILE, strerror (errno));
      exit (1);
    }

  while ((c = fgetc (fp)) != EOF)
    if (c == '\n')
      n_lines++;
  rewind (fp);
  while (n_lines-- >= 0)
    {
      c_data[n_lines] = (char *) malloc (500 * sizeof (char *));
      fgets (c_data[n_lines], LINESZ, fp);
      if (isdigit (c_data[n_lines][1]))
	{
	  if (strchr (c_data[n_lines], '#') != NULL)
	    {
	      for (itra = 0; itra < strlen (c_data[n_lines]); itra++)
		if (c_data[n_lines][itra] == '#')
		  {
		    c_data[n_lines][itra] = '\0';
		  }
	    }
	  else if (c_data[n_lines][0] == '\n')
	    n_lines--;
	}
    }

  rewind (fp);
  while ((c = fgetc (fp)) != EOF)
    if (c == '\n')
      n_lines++;
  fclose (fp);

  for (itra = 0, itrb = 0; itra <= n_lines; itra++)
    if (isdigit (c_data[itra][0]))
      {
	s_data[itrb] = (char *) malloc (LINESZ * sizeof (char *));
	s_data[itrb++] = c_data[itra];
      }

  for (itra = 0; itra < 5; itra++)
    for (itrb = 0; itrb < 4; itrb++)
      if (s_data[itrb][1] > s_data[itrb + 1][1])
	{
	  store = s_data[itrb];
	  s_data[itrb] = s_data[itrb + 1];
	  s_data[itrb + 1] = store;
	}

  for (itra = 0; itra <= 4; itra++)
    {
      status = (char *) strtok (s_data[itra], ":");
      if (status != NULL)
	{
	  if (atoi (status) >= 10 && atoi (status) <= 15)
	    cfo->status[itra] = atoi (status);
	  else
	    return (void *) NULL;
	}
      enable = strtok (NULL, ":");
      if (enable != NULL)
	{
	  if (isascii (enable[0]) && !isspace (enable[0]))
	    {
	      cfo->enable[itra] = (char *) malloc (strlen (enable));
	      strcpy (cfo->enable[itra], enable);
	    }
	  else
	    cfo->enable[itra] = NULL;
	}
      disable = strtok (NULL, ":");
      if (disable != NULL)
	{
	  if (isascii (disable[0]) && !isspace (disable[0]))
	    {
	      cfo->disable[itra] = (char *) malloc (strlen (disable));
	      strcpy (cfo->disable[itra], disable);
	    }
	  else
	    cfo->disable[itra] = NULL;
	}
    }

  free (cfs);
  return (cfo);

}

/*	EOF	*/
