#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

static int verbose=0;

#define MAX_DSTAT_COLS 100
#define MAX_DSTAT_NAME_LEN 16
#define MAX_DSTAT_VALUE_LEN 16
static char dstat_column_name[MAX_DSTAT_COLS][MAX_DSTAT_NAME_LEN]={{0},{0},{0}};
static char dstat_column_value[MAX_DSTAT_COLS][MAX_DSTAT_VALUE_LEN]={{0},{0},{0}};
static int dstat_columns=0;

int dstat_field_columns(char *name)
{
    if (*name=='t' && strcmp(name,"time")==0) return 2;
    if (*name=='d' && strcmp(name,"date/time")==0) return 2;
    return 1;
}

void read_dstat(char *line)
{
    static int want_header=1;
    if (want_header)
    {
        if (*line=='-' || *line=='\033') return;
        if (strncmp(line,"You did not",11)==0) return;
        while (*line>=' ')
        {
            while (*line==' ' || *line=='|') line++;
            char *n=dstat_column_name[dstat_columns];
            while (*line>' ' && *line!='|') *n++=*line++;
            *n=0;
            dstat_columns++;
        }
        want_header=0;
        return;
    }
    
    int i=0;
    while (i<dstat_columns && *line>=' ')
    {
        while (*line==' ' || *line=='|') line++;
        if (*line<' ') break;
        char *v=dstat_column_value[i];

        *v=0;
        int j,c=dstat_field_columns(dstat_column_name[i]);
        for (j=0;j<c;j++)
        {
            if (j) *v++=' ';
            while (*line>' ' && *line!='|') *v++=*line++;
            while (*line==' ' || *line=='|') line++;
        }
        *v=0;
if (verbose) fprintf(stderr,"%s=%s\n",dstat_column_name[i],dstat_column_value[i]);
        i++;
    }
if (verbose) fprintf(stderr,"---\n");
}

char *builtin_statistic(char *name,int name_len)
{
    switch (*name)
    {
        case 'h':
            if (name_len==4 && strncmp(name,"host",4)==0)
            {
                static char hostname[128];
                if (gethostname(hostname,sizeof(hostname))<0) break;
                return hostname;
            }
            break;
        case 'i':
            if (strncmp(name,"ip_",3)==0)
            {
                static char ip[NI_MAXHOST];
                struct ifaddrs *ifa;
                if (getifaddrs(&ifa)<0) break;
                while (ifa!=NULL)
                {
                    if (ifa->ifa_addr!=NULL)
                    {
                        int family=ifa->ifa_addr->sa_family;
                        if (family==AF_INET || family==AF_INET6)
                        {
                            if (strncmp(ifa->ifa_name,name+3,name_len-3)==0 && ifa->ifa_name[name_len-3]==0)
                            {
                                int s=getnameinfo(ifa->ifa_addr,family==AF_INET?sizeof(struct sockaddr_in):sizeof(struct sockaddr_in6),
                                                  ip,sizeof(ip),NULL,0,NI_NUMERICHOST);
                                if (s==0) return ip;
                            }
                        }
                    }
                    ifa=ifa->ifa_next;
                }
            }
        default:
            break;
    }
    return NULL; // not found
}

char *get_statistic(char *name,int name_len)
{
    int i;
    char *v=builtin_statistic(name,name_len);
    if (v==NULL)
    {
        v="";
        for (i=0;i<dstat_columns;i++)
        {
            if (dstat_column_name[i][0]==*name)
            {
                if (strncmp(dstat_column_name[i],name,name_len)==0) 
                { 
                    v=dstat_column_value[i]; 
                    break; 
                }
            }
        }
        if (verbose && i==dstat_columns) { fprintf(stderr,"WARNING: no data for dstat column %.*s\n",name_len,name); }
    }
    return v;
}

void output_stats(char *template)
{
    static WINDOW *mainwin=NULL;
    if (mainwin==NULL)
    {
        mainwin=initscr();
        if (mainwin==NULL) { fprintf(stderr, "Error initialising ncurses.\n"); exit(1); }
    }

    clear();
    int y=0,justify=0;
    char *t=template,line[1024],*o=line,*oe,*s,*e,*v;
    while (*t)
    {
        while (*t && *t!='$') 
        {
            if (*t<32) { *o=0; mvaddstr(y++,0,line); o=line; t++; continue; }
            *o++=*t++;
        }
        if (*t=='$')
        {
            s=++t; e=s;
            while ((*e>='a' && *e<='z') || (*e>='0' && *e<='9') || (*e>='A' && *e<='Z') || *e=='/' || *e=='_') e++;
            t=e;
            
            while (*t=='-' || *t=='<' || *t=='>') 
            { 
                if (*t=='<') justify=1;
                if (*t=='>') justify=2;
                t++; 
            }
            oe=o+(t-s+1);
            v=get_statistic(s,e-s);

            if (justify==0) /* no justify */
            {
                while (*v) *o++=*v++;
            }
            else if (justify==1) /* left */
            {
                while (*v && o<oe) *o++=*v++;
                while (o<oe) *o++=' ';
            }
            else /* right */
            {
                int pad=((int)(oe-o))-strlen(v);
                while (pad-->0) *o++=' ';
                while (*v) *o++=*v++;
            }
        }
    }
    *o=0; mvaddstr(y++,0,line);
    refresh();
}

int main(int argc,char **argv)
{
    int i;
    char *template=NULL;
    char *dstat_options="mndcvt --ntp";
    int template_len=0;
    for (i=1;i<argc;i++)
    {
        if (strncmp(argv[i],"-t=",3)==0) 
        { 
            FILE *fd=fopen(argv[i]+3,"r");
            if (!fd)  { fprintf(stderr,"ERROR: cant open template file %s\n",argv[i]+3); exit(1); }
            char line[1024];
            while (fgets(line,sizeof(line)-1,fd))
            {
                template=(char*)realloc(template,template_len+strlen(line)+1);
                strcpy(template+template_len,line);
                template_len+=strlen(line);
            }
            fclose(fd);
        }
        else if (strncmp(argv[i],"-d=",3)==0)
            dstat_options=argv[i]+3;
        else if (strcmp(argv[i],"-v")==0)
            verbose=1;
        else break;
    }
    if (i!=argc || template==NULL)
    { fprintf(stderr,"usage: %s [-v] -d=<dstat-options> -t=<template-file>\n",argv[0]); exit(1); }

    char dstat_command[256];
    sprintf(dstat_command,"dstat -%s",dstat_options);
    FILE *dfd=popen(dstat_command,"r");
    if (!dfd) { fprintf(stderr,"ERROR: cant start dstat command : %s\n",dstat_command); exit(1); }

    time_t last=time(NULL);
    char dstat_input[4096];
    while(fgets(dstat_input,sizeof(dstat_input)-1,dfd))
    {
        read_dstat(dstat_input);
        time_t now=time(NULL);
        if (now!=last) { output_stats(template); last=now; }
    }

    return 0;
}

