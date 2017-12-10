/***
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
***/

// To compile, choose one of the following depending on what display libraries you have...
// libst2205:            gcc -Wall -g -O2 -DHAVE_ST2205 -o st2205term st2205term.c -lrote -lst2205 
// libdlo:               gcc -Wall -g -O2 -DHAVE_DLO -o st2205term st2205term.c -lrote -ldlo
// libdpf:            gcc -Wall -g -O2 -DHAVE_ST2205 -o st2205term st2205term.c -lrote -ldpf 
// libdlo:               gcc -Wall -g -O2 -DHAVE_DLO -o st2205term st2205term.c -lrote -ldlo
// libst2205 and libdlo: gcc -Wall -g -O2 -DHAVE_ST2205 -DHAVE_DLO -o st2205term st2205term.c -lrote -lst2205 -ldlo

#define _GNU_SOURCE
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>

#include <errno.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <rote/rote.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>

#ifdef HAVE_ST2205
#include <st2205.h>
static st2205_handle *picframe=NULL; // libst2205 instance handle
#endif 
char *st2205dev=NULL; // name of picframe device (commandline)

#ifdef HAVE_DLO
#include "libdlo.h"
static dlo_dev_t uid=0;
static int minx,maxx,miny,maxy;
static unsigned char colormap[8]={0,0,0};
static int dlo_broken=0;
static int *source_row=NULL;
static unsigned char *bad_row=NULL;
static unsigned int *lasthash=NULL,*hash=NULL; // 32bit hash of each terminal row
#endif
#ifdef HAVE_DPF
#include "dpf.h"
static DPFHANDLE picframe;
static unsigned char colormap[8][2];
#endif

static int sw=0,sh=0; // screen width, height
static unsigned char *pixels=NULL; // pixel buffer that is copied to device
typedef struct _font_t {
    int w,h;
    char data[1]; // stretchy
} font_t;
static font_t *font=NULL;

static char *command="/bin/bash --login"; // command to run on main terminal
static char *command2=NULL; // command to run as screen saver

static RoteTerm *rt1=NULL; // main terminal (command)
static RoteTerm *rt2=NULL; // terminal for screensaver (command2)
static int tw=0,th=0; // terminal width, height in chars
static unsigned char *lastbuf=NULL,*lastattr=NULL; // last terminal image
static int last_cy=0,last_cx=0; // last cursor position

// st2205term supports 8 colors (same as Linux console):
static unsigned char rgb_colors[8][3]=
{
    { 0,0,0,     }, // black
    { 255,0,0,   }, // red
    { 0,255,0,   }, // green
    { 255,255,0, }, // yellow
    { 0,0,255,   }, // blue
    { 255,0,255, }, // magenta
    { 0,255,255, }, // cyan
    { 255,255,255}  // white
};

// array that maps evdev keycode => linux key
typedef struct _keymap {
    char keyname[16];     // name of key
    int  ch;              // TERM=linux key code
    int  shift_ch;        // TERM=linux key code while SHIFT is pressed
    int  ctrl_ch;         // TERM=linux key code while CONTROL is pressed
    int  alt_ch;          // TERM=linux key code while ALT is pressed
} keymap;

#define MAX_EVKEYS 256
static keymap kmap[MAX_EVKEYS]={{"",0},{"",0},{"",0}};
static int shift_active=0,ctrl_active=0,alt_active=0;

// type of output device selected on cmdline
static int use_st2205=0,use_dlo=0,use_dpf=0;

// blinking cursor?
static int blinking_cursor=0,blink_toggle=1;
static struct timeval blink_last={0,0};

static int kmap2int(char *str)
{
    int n;
    if (*str=='\'' || *str=='"') return str[1];
    if (*str=='0' && str[1]=='x')
        if (sscanf(str+2,"%x",&n)==1) 
            return n;
    if (*str>='0' && *str<='9')
       return atoi(str);
    if (*str) fprintf(stderr,"Warning: kmap2int(%s) - number conversion failed\n",str);
    return 0; 
}
static void load_kmap(char *file)
{
    fprintf(stderr,"load_kmap(%s) - loading key defintions\n",file);
    FILE *f=fopen(file,"r");
    if (!f) { perror("fopen"); fprintf(stderr,"ERROR: could not open file : %s\n",file); exit(1); }
    char line[256];
    int count=0;
    char keyname[16],code[8],base[8],shift[8],ctrl[8],alt[8];
    while (fgets(line,sizeof(line)-1,f))
    {
        if (sscanf(line,"%15s %7s %7s %7s %7s %7s",keyname,code,base,shift,ctrl,alt)==6)
        {
            if (keyname[0]=='#') continue;
            int k=kmap2int(code);
            if (k>=MAX_EVKEYS)
            { fprintf(stderr,"load_kmap(%s): Warning ignoring keycode out-of-range: %s",file,line); continue; }

            strcpy(kmap[k].keyname,keyname);
            kmap[k].ch=kmap2int(base);
            kmap[k].shift_ch=kmap2int(shift);
            kmap[k].ctrl_ch=kmap2int(ctrl);
            kmap[k].alt_ch=kmap2int(alt);
            count++;
        }
        else if (sscanf(line,"%1s",keyname)==1) // ignore blank lines
            fprintf(stderr,"load_kmap(%s): ERROR Syntax: %s",file,line);
    }
    fclose(f);
    fprintf(stderr,"load_kmap(%s) - %d key defintions loaded\n",file,count);
}
// return codes:
//  0 => no mapping
// -1 => special key (SHIFT,CTRL,ALT)
// >0 => Linux key code
static int evkey2linuxkey(int code,int pressed)
{
    if (code>=MAX_EVKEYS) return 0;
    if (kmap[code].keyname[0]==0) return 0;

    if (strcmp(kmap[code].keyname,"SHIFT_LEFT")==0 || strcmp(kmap[code].keyname,"SHIFT_RIGHT")==0)
    { shift_active=pressed; return -1; }
    if (strcmp(kmap[code].keyname,"ALT_LEFT")==0 || strcmp(kmap[code].keyname,"ALT_RIGHT")==0)
    { alt_active=pressed; return -1; }
    if (strcmp(kmap[code].keyname,"CTRL")==0)
    { ctrl_active=pressed; return -1; }

    int linuxkey=0;
    if (pressed)
    {
        linuxkey=kmap[code].ch;
        if (alt_active) linuxkey=kmap[code].alt_ch;
        if (ctrl_active) linuxkey=kmap[code].ctrl_ch;
        if (shift_active) linuxkey=kmap[code].shift_ch;
    }
    return linuxkey;
}

static font_t *load_font(char *file,int num)
{
    fprintf(stderr,"load_font(%s,%d) - loading font defintion\n",file,num);
    FILE *fd=fopen(file,"r");
    if (!fd) { perror("fopen"); fprintf(stderr,"ERROR: could not open file : %s\n",file); exit(1); }
    int font_size[4][2];
    if (fscanf(fd,"%d %d %d %d %d %d %d %d",
               &font_size[0][0],&font_size[0][1],&font_size[1][0],&font_size[1][1],
               &font_size[2][0],&font_size[2][1],&font_size[3][0],&font_size[3][1])!=8)
    { fprintf(stderr,"ERROR: could read font sizes from file : %s\n",file); exit(1); }
     
    int f,sz;
    for (f=0;f<num;f++)
    {
        sz=95*(font_size[f][0]+1)*font_size[f][1];
        if (fseek(fd,sz,SEEK_CUR)<0)
        { perror("fseek "); fprintf(stderr,"ERROR: could read font %d (sz=%d) from file : %s\n",f,sz,file); exit(1); }
    }

    sz=95*(font_size[f][0]+1)*font_size[f][1];
    font_t *d=(font_t *)malloc(sizeof(font_t)+sz);
    d->w=font_size[f][0];
    d->h=font_size[f][1];
    
    if (fread(d->data,1,sz,fd)!=sz)
    { fprintf(stderr,"ERROR: could read font %d (sz=%d) from file : %s\n",f,sz,file); exit(1); }
    fclose(fd);

    return d;
}

// try to open keyboard device
static int check_for_keyboard(char *kbdev)
{
    char device[PATH_MAX],path[PATH_MAX];

    char *dev=kbdev;
    if (dev==NULL)
    {
        DIR *d=opendir("/dev/input/by-id/");
        if (!d) return -997;
        
        struct dirent *e;
        while ((e=readdir(d))!=NULL)
        {
            if (strstr(e->d_name,"Keyboard-event-kbd")!=NULL)
            { sprintf(path,"/dev/input/by-id/%s",e->d_name); break; }
        }
        closedir(d);
        if (e==NULL) return -998; 
        dev=path;
    }

    if (realpath(dev,device)==NULL)
    { fprintf(stderr,"WARN: realpath failed to resolve device %s\n",kbdev); return -999; }
        
    int fd=open(device,O_RDONLY);
    if (fd>=0)
    {
        fcntl(fd,F_SETFL,O_NONBLOCK);
        char name[256]= "Unknown";
        if (ioctl(fd,EVIOCGNAME(sizeof(name)),name)>=0)
            fprintf(stderr,"Opened device %s (%s) => %s\n",kbdev!=NULL?kbdev:path,device,name);
    }
    return fd;
}

// draw a character on the pixel buffer
static void draw_char(unsigned char *pixels,int x,int y,int ch,int bg,int fg,font_t *f,int sh,int sw)
{
    x*=f->w; y*=f->h; // char offsets => pixel offsets

#ifdef HAVE_ST2205
    if (use_st2205)
    {
        char *d=f->data;
        if (ch>32 && ch<127) d+=(f->w+1)*f->h*(ch-32);
        unsigned char *rgb,*rgb_fg=rgb_colors[fg],*rgb_bg=rgb_colors[bg];
        int m,n;
        for (m=0;m<f->h;m++) {
            unsigned char *p=pixels+(y+m)*(sw+sw+sw)+x+x+x;
            for (n=0;n<f->w;n++) {
                rgb=(*d++=='#')?rgb_fg:rgb_bg;
                *p++=*rgb++;
                *p++=*rgb++;
                *p++=*rgb++;
            }
            d++; // skip linefeed
        }
    }
#endif
#ifdef HAVE_DPF
    if (use_dpf)
    {
        char *d=f->data; 
        if (ch>32 && ch<127) d+=(f->w+1)*f->h*(ch-32);
        unsigned char rgb_fg=colormap[fg][0],rgb_bg=colormap[bg][0];
        unsigned char rgb_fgh=colormap[fg][1],rgb_bgh=colormap[bg][1];
        int m,n;
        for (m=0;m<f->h;m++) { //Each font character if a rectangular array of pixels. 
//m is the y-coordinate within the pixel array which ranges from 0..font height (f->h-1) 
            unsigned char *p=pixels+(y+m)*(sw+sw)+x+x;
	    	
            for (n=0;n<f->w;n++) {
// n is the x-coordinate within the pixel array which ranges from 0..font width (f->w-1)
                
		if (*d++=='#'){
                *p++=rgb_fg;
                *p++=rgb_fgh;
		} else {
		*p++=rgb_bg;
		*p++=rgb_bgh;
		}
            }
            d++; // skip linefeed
        }
    }	
#endif
#ifdef HAVE_DLO
    if (use_dlo)
    {
        if (x<minx) minx=x;
        if (x+f->w>maxx) maxx=x+f->w;
        if (y<miny) miny=y;
        if (y+f->h>maxy) maxy=y+f->h;

        char *d=f->data;
        if (ch>32 && ch<127) d+=(f->w+1)*f->h*(ch-32);
        unsigned char dlo_fg=colormap[fg],dlo_bg=colormap[bg];
        int m;
        for (m=0;m<f->h;m++) 
        {
            unsigned char *p=pixels+(m+y)*sw+x,*pe=p+f->w;
            while (p<pe)
                *p++=*d++=='#'?dlo_fg:dlo_bg;
            d++; // skip linefeed
        }
    }
#endif
}

static void create_terminal()
{
    tw=sw/font->w; th=sh/font->h;
    fprintf(stderr,"create_terminal %dx%d pixels => %dx%d chars\n",sw,sh,tw,th);
    rt1=rote_vt_create(th,tw);
    pid_t pid=rote_vt_forkpty(rt1,command);
    fprintf(stderr,"rote_vt_forkpty: command='%s' PID=%d\n",command,pid);
    lastbuf=(unsigned char *)calloc(th*tw,1);
    lastattr=(unsigned char *)calloc(th*tw,1);
    last_cy=0; last_cx=0;

    if (command2!=NULL)
    {
        rt2=rote_vt_create(th,tw);
        pid_t pid=rote_vt_forkpty(rt2,command2);
        fprintf(stderr,"rote_vt_forkpty: screensaver command='%s' PID=%d\n",command2,pid);
    }

    #ifdef HAVE_DLO
        #define DLO_MAP(R,G,B) ((((R)/32)<<5)+(((G)/64)<<3)+((B)/32))
        int i;
        for (i=0;i<8;i++)
            colormap[i]=DLO_MAP(rgb_colors[i][0],rgb_colors[i][1],rgb_colors[i][2]);
        lasthash=(unsigned int *)calloc(th,sizeof(int));
        hash=(unsigned int *)calloc(th,sizeof(int));
        source_row=(int*)calloc(th,sizeof(int));
        bad_row=(unsigned char*)calloc(th,sizeof(unsigned char));
    #endif
    #ifdef HAVE_DPF
        int i;
        for (i=0;i<8;i++){
            colormap[i][0]=(unsigned char)(RGB565_0(rgb_colors[i][0],rgb_colors[i][1],rgb_colors[i][2]));
            colormap[i][1]=(unsigned char)(RGB565_1(rgb_colors[i][0],rgb_colors[i][1],rgb_colors[i][2]));}
    #endif
}
static void destroy_terminal()
{
    fprintf(stderr,"Destroying terminal - output device unplugged\n");
    rote_vt_destroy(rt1); rt1=NULL;
    tw=0; th=0; 
    free(lastbuf); lastbuf=NULL;
    free(lastattr); lastattr=NULL;
    last_cy=0; last_cx=0;

    if (rt2!=NULL) { rote_vt_destroy(rt2); rt2=NULL; }

#ifdef HAVE_DLO
    free(lasthash); lasthash=NULL;
    free(hash); hash=NULL;
    free(source_row); source_row=NULL;
    free(bad_row); bad_row=NULL;
#endif
}

static void check_for_output_device()
{
    if (use_dlo)
    {
#ifdef HAVE_DLO
        if (uid)
        { // we think we're connected but are we really?
            if (dlo_broken)
            {
                destroy_terminal();
                dlo_release_device(uid);
//                dlo_final_t fin_flags = { 0 };
//                dlo_final(fin_flags);
                uid=0;
            }
        }
        else
        { // we're disconnected - see if we can connect
            static int init=0;
            dlo_init_t ini_flags={ 0 };
            dlo_claim_t cnf_flags={ 0 };
            if (!init) // only dlo_init() once - multiple init+final is buggy
            {
                if (dlo_init(ini_flags)!=dlo_ok)
                { fprintf(stderr,"ERROR: dlo_init() failed!\n"); exit(1); }
                init=1;
            }
            uid=dlo_claim_first_device(cnf_flags,0);
            if (uid)
            {
                dlo_mode_t *info=dlo_get_mode(uid);
                sw=info->view.width;
                sh=info->view.height;
/*     // trying to set device to 8bpp seems to have no effect - so dont bother
        dlo_mode_t desc;
        desc.view.base=info->view.base;
        desc.view.width=info->view.width;
        desc.view.height=info->view.height;
        desc.view.bpp=8;
        desc.refresh=0;
        if (dlo_set_mode(uid,&desc)!=dlo_ok)
            fprintf(stderr,"Warning: could not det displaylink mode to 8bpp\n");
        info=dlo_get_mode(uid);
        sw=info->view.width;
        sh=info->view.height; */
                fprintf(stderr,"Displaylink device: %d x %d, %d bpp color\n",sw,sh,info->view.bpp);
                pixels=(unsigned char*)calloc(1,sw*sh); // 8bpp RRRGGBBB bits
                dlo_fill_rect(uid,NULL,NULL,DLO_RGB(0,0,0)); // clear screen black
                dlo_broken=0;
                create_terminal();
            }
//            else { dlo_final_t fin_flags = { 0 }; dlo_final(fin_flags); }
        }
#else
        fprintf(stderr,"ERROR: displaylink support is not compiled into binary!\n"); exit(1);
#endif
    }
    if (use_dpf)
    {
#ifdef HAVE_DPF
        static char device[PATH_MAX];
        if (picframe==NULL)
        {
            char *dev=NULL;
            if (dev==NULL)
            {
                static char path[PATH_MAX];
                dev="usb0";
            }
	    strcpy(device,dev);
	    int r=dpf_open(device,&picframe);
                if (picframe) 
                { 
                    sw=picframe->width;
                    sh=picframe->height;
                    pixels=(unsigned char*)malloc(sw*sh*2);
                    create_terminal();
                    fprintf(stderr,"INFO: successfully connected to picframe\n");
                }
                else fprintf(stderr,"WARN: failed to connect to picframe\n");
            //}
        }
#else
        fprintf(stderr,"ERROR: dpf support is not compiled into binary!\n"); exit(1);
#endif
    }
    if (use_st2205)
    {
#ifdef HAVE_ST2205
        static char device[PATH_MAX];
        if (picframe)
        { // we think we're connected but are we really?
            struct stat s; // stat device to see if it is still there
            int rc=stat(device,&s);
            if (rc<0)
            {
                destroy_terminal();
                st2205_close(picframe);
                free(pixels); pixels=NULL;
                picframe=NULL;
                fprintf(stderr,"WARN: picframe disconnected\n");
            }
        }

        if (picframe==NULL)
        {
            char *dev=st2205dev;
            if (dev==NULL)
            {
                static char path[PATH_MAX];
                DIR *d=opendir("/dev/disk/by-id/");
                if (!d) return;
        
                struct dirent *e;
                while ((e=readdir(d))!=NULL)
                {
                    if (strstr(e->d_name,"SITRONIX")!=NULL)
                    { sprintf(path,"/dev/disk/by-id/%s",e->d_name); break; }
                }
                closedir(d);
                if (e==NULL) return; 
                dev=path;
            }

            if (realpath(dev,device)==NULL)
            { fprintf(stderr,"WARN: realpath failed to resolve device %s\n",dev); return; }

            fprintf(stderr,"INFO: connecting to picframe device %s (%s)\n",dev,device);
        
            struct stat s; // stat device 
            int rc=stat(device,&s);
            if (rc>=0)
            { // we're disconnected but device is there - see if we can connect
                picframe=st2205_open(device);
                if (picframe) 
                { 
                    sw=picframe->width;
                    sh=picframe->height;
                    pixels=(unsigned char*)malloc(sw*sh*3);
                    create_terminal();
                    fprintf(stderr,"INFO: successfully connected to picframe\n");
                }
                else fprintf(stderr,"WARN: failed to connect to picframe\n");
            }
        }
#else
        fprintf(stderr,"ERROR: st2205 support is not compiled into binary!\n"); exit(1);
#endif
    }
}

#ifdef HAVE_DLO
static void check_for_scrolling(unsigned int *hash,unsigned int *lasthash,
                                unsigned char *lastbuf,unsigned char *lastattr,
                                int cx,int cy,font_t *font,RoteTerm *rt)
{
    int i,j,k,l;
    for (k=-1,i=0;i<th;i++) // go through new screen
    {
        bad_row[i]=0; source_row[i]=-1;
        if (lasthash[i]==hash[i]) continue; // line hasnt changed

        for (l=0;l<th;l++) // try to locate the row from the old screen for the new one
        {
            if (++k==th) k=0;
            if (hash[i]!=lasthash[k]) continue;
            if (k==i || k==cy) continue; // avoid self and cursor row

            unsigned char *b=lastbuf+k*tw,*a=lastattr+k*tw;
            for (j=0;j<tw && rt->cells[i][j].ch==*b && rt->cells[i][j].attr==*a;j++,a++,b++) { }
            if (j==tw) { source_row[i]=k; break; }
        }
    }
    for (i=0;i<th;i++)
    {
        k=source_row[i];
        if (k<0 || bad_row[k]) continue;

        int rows=1;
        while (i+rows<th && source_row[i+rows]==k+rows) rows++;
        
        dlo_rect_t rec;
        dlo_dot_t dot;
        rec.origin.x=0;
        rec.origin.y=k*font->h;
        rec.width=tw*font->w;
        rec.height=rows*font->h;
        dot.x=0;
        dot.y=i*font->h;
        dlo_copy_rect(uid,NULL,&rec,NULL,&dot);

        unsigned char *bo=lastbuf+k*tw,*ao=lastattr+k*tw,*b=lastbuf+i*tw,*a=lastattr+i*tw;
        memmove(b,bo,rows*tw);
        memmove(a,ao,rows*tw);

        unsigned char *s=pixels+k*font->h*sw,*p=pixels+i*font->h*sw;
        memmove(p,s,rows*font->h*sw);

        memset(&bad_row[i],1,rows);

//fprintf(stderr,"copy %d rows from y=%d to y=%d\n",rows,k,i);
        i+=rows-1;
    }
}
#endif

int main(int argc, char **argv)
{
    int verbose=0;
    int fd=-1;
    int font_num=0;
    int fast_scroll=0;

    char *kbdev=NULL;
    char *keymapfile=NULL;
    char *fontsfile=NULL;

    int i,bad=0;
    for (i=1;i<argc;i++)
    {
        if (strcmp(argv[i],"-v")==0 || strcmp(argv[i],"--verbose")==0) { verbose=1; continue; }
        if (strcmp(argv[i],"--font=tiny")==0)  { font_num=0; continue; }
        if (strcmp(argv[i],"--font=small")==0) { font_num=1; continue; }
        if (strcmp(argv[i],"--font=large")==0) { font_num=2; continue; }
        if (strcmp(argv[i],"--font=giant")==0) { font_num=3; continue; }
        if (strncmp(argv[i],"--keymap=",9)==0) { keymapfile=argv[i]+9; continue; }
        if (strncmp(argv[i],"--fontdata=",9)==0) { fontsfile=argv[i]+11; continue; }
        if (strncmp(argv[i],"--picframe=",11)==0) { st2205dev=argv[i]+11; use_st2205=1; continue; }
        if (strcmp(argv[i],"--dpf")==0) { use_dpf=1; continue; }
        if (strcmp(argv[i],"--dlo")==0) { use_dlo=1; continue; }
        if (strcmp(argv[i],"--dlo-fast-scroll=yes")==0) { fast_scroll=1; continue; }
        if (strcmp(argv[i],"--dlo-fast-scroll=no")==0) { fast_scroll=0; continue; }
        if (strncmp(argv[i],"--keyboard=",11)==0) { kbdev=argv[i]+11; continue; }
        if (strcmp(argv[i],"--blink")==0) { blinking_cursor=1; continue; }
        if (strncmp(argv[i],"--command=",10)==0) { command=argv[i]+10; continue; }
        if (strncmp(argv[i],"--screensaver=",14)==0) { command2=argv[i]+14; continue; }
        fprintf(stderr,"unrecognized command line option '%s'\n",argv[i]);
        bad=1;
        break;
    }
    if (use_st2205==0 && use_dlo==0) use_dpf=1;
    if (keymapfile==NULL)
    {
        static char path[PATH_MAX];
        if (realpath("/proc/self/exe",path)==NULL)
        { fprintf(stderr,"ERROR: realpath(/proc/self/exe) failed!\n"); exit(1); }
        int len=strlen(path);
        while (--len>=0) { if (path[len]=='/') break; }
        path[len--]=0;
        strcat(path,"/us.kmap");
        keymapfile=path;
        fprintf(stderr,"INFO: using keymap from file %s\n",keymapfile);
    }
    if (fontsfile==NULL)
    {
        static char path[PATH_MAX];
        if (realpath("/proc/self/exe",path)==NULL)
        { fprintf(stderr,"ERROR: realpath(/proc/self/exe) failed!\n"); exit(1); }
        int len=strlen(path);
        while (--len>=0) { if (path[len]=='/') break; }
        path[len--]=0;
        strcat(path,"/fonts.txt");
        fontsfile=path;
        fprintf(stderr,"INFO: using fontdata from file %s\n",fontsfile);
    }
    if (use_dlo)
        fprintf(stderr,"INFO: attempting to use Displaylink screen for output\n");
    else if (st2205dev==NULL)
        fprintf(stderr,"INFO: picframe selection automatic (polling /dev/disk/by-id)\n");

    if (keymapfile==NULL || fontsfile==NULL) bad=1;
    if (bad)
    {
        fprintf(stderr,"usage: %s [--font=tiny|small|large|giant] "
                       "[--command=<command>] [--screensaver=<command>] "
                       "[--dlo|--picframe=/dev/sdb] [-dlo-fast-scroll=[yes|no]] [--blink] "
                       "[--keyboard=/dev/input/event0] "
                       "[--keymap=us.kmap] [--fontdata=fonts.txt]\n",argv[0]);
        exit(1);
    }

    load_kmap(keymapfile);

    font=load_font(fontsfile,font_num);

    struct input_event ev[64];
    fd_set readset,*rs=NULL,exceptset,*es=NULL;
    int ss_active=0;
    time_t last_keypress=time(NULL);
    for(;;)
    {
        check_for_output_device();

        if (fd<0) // no current keyboard => check if one was plugged in 
        {
            fd=check_for_keyboard(kbdev);
            if (fd>=0) { rs=&readset; FD_ZERO(rs); es=&exceptset; FD_ZERO(es); }
        }

        // wait 200ms for keystrokes, or 1sec if keyboard or output device is unplugged
        struct timeval timeout={2,0};
        if (rs!=NULL && rt1!=NULL) { FD_SET(fd,rs); FD_SET(fd,es); timeout.tv_sec=0; timeout.tv_usec=200000; } 
        if (select(rs?fd+1:0,rs,NULL,es,&timeout)>0)
        {
            // something happened - keypress or keyboard disconnect
            int rb=read(fd,ev,sizeof(ev));
            if (rb<0 || FD_ISSET(fd,es)) 
            { 
                fprintf(stderr,"Keyboard unplugged\n");
                close(fd); fd=-1; rs=es=NULL; 
                continue; 
            }

            // map and forward all the keystrokes to the terminal
            if (rb<(int)sizeof(struct input_event)) { perror("evtest: short read"); exit (1); }
            int e;
            for (e=0;e<(int)(rb/sizeof(struct input_event));e++)
            {
                if (ev[e].type!=EV_KEY) continue;

                int code=ev[e].code;
                int pressed=ev[e].value;

                int linuxkey=evkey2linuxkey(code,pressed);

                if (verbose)
                {
                    char *kn="????";
                    if (code<MAX_EVKEYS) kn=kmap[code].keyname;
                    fprintf(stderr,"%ld.%06ld evdev keycode %d keyname=%s is %s\n",
                           ev[e].time.tv_sec,ev[e].time.tv_usec,code,kn,pressed?"down":"up");
                    switch (linuxkey)
                    {
                    case -1: fprintf(stderr,"    special key\n"); break;
                    case 0:  fprintf(stderr,"    no mapping\n"); break;
                    default: 
                        fprintf(stderr,"    maps => %s linux key code 0x%02x (%c)\n",
                               kmap[code].keyname,linuxkey,linuxkey>=32 && linuxkey<126?linuxkey:'?');
                        break;
                    }
                }
                if (linuxkey>0 && rt1!=NULL) rote_vt_keypress(rt1,linuxkey); 
                ss_active=0;
                last_keypress=time(NULL);
            }
        }
        if (rt1==NULL) continue;

        if (rt2!=NULL && !ss_active)
            if (time(NULL)-last_keypress>=5) 
                ss_active=1;

        RoteTerm *rt=ss_active?rt2:rt1;

        // redraw the terminal screen in memory
        rote_vt_draw(rt,NULL,1,1,NULL);

        int i,j;
#ifdef HAVE_DLO
        // create hash of each terminal row to speed up finding changes
        int h=0;
        for (i=0;i<th;i++)
        {
            h=0;
            for (j=0;j<tw;j++)
                h=(h<<5) + h + rt->cells[i][j].ch + rt->cells[i][j].attr;
            hash[i]=h;
        }
        // displaylink supports moving areas on display
        if (fast_scroll) check_for_scrolling(hash,lasthash,lastbuf,lastattr,last_cx,last_cy,font,rt); 
        // bounding box for screen changes
        minx=sw; maxx=0; miny=sh; maxy=0;
#endif

        // find screen changes
        int count=0;
        int cy=rt->crow,cx=rt->ccol;
        for (i=0;i<th;i++)
        {
#ifdef HAVE_DLO
            if (hash[i]==lasthash[i]) continue; // we'll risk hash collisions for efficiency
#endif
            unsigned char *b=lastbuf+i*tw,*a=lastattr+i*tw;
            for (j=0;j<tw;j++)
            {
                int ch=rt->cells[i][j].ch,attr=rt->cells[i][j].attr;
                if (ch!=*b || attr!=*a)
                {
                    if (j==cx && i==cy)
                        draw_char(pixels,j,i,ch,ROTE_ATTR_FG(attr),ROTE_ATTR_BG(attr),font,sh,sw); // inverse video for cursor
                    else
                        draw_char(pixels,j,i,ch,ROTE_ATTR_BG(attr),ROTE_ATTR_FG(attr),font,sh,sw);
                    *b=ch; *a=attr;
                    count++;
                }
                b++; a++;
            }
        }
        int draw_cursor=0;
        if (blinking_cursor)
        {
            struct timeval tv;
            gettimeofday(&tv,NULL);
            unsigned int elapsed_us=(tv.tv_sec-blink_last.tv_sec)*1000000+(tv.tv_usec-blink_last.tv_usec);
            if (elapsed_us>500000) 
            { 
                draw_cursor=1; 
                blink_toggle=1-blink_toggle; 
                blink_last.tv_sec=tv.tv_sec;
                blink_last.tv_usec=tv.tv_usec;
            }
        }
        if (draw_cursor || cy!=last_cy || cx!=last_cx)
        {
            draw_char(pixels,last_cx,last_cy,rt->cells[last_cy][last_cx].ch,
                      ROTE_ATTR_BG(rt->cells[last_cy][last_cx].attr),ROTE_ATTR_FG(rt->cells[last_cy][last_cx].attr),
                      font,sh,sw);
            if (!blinking_cursor || blink_toggle)
                draw_char(pixels,cx,cy,rt->cells[cy][cx].ch,
                          ROTE_ATTR_FG(rt->cells[cy][cx].attr),ROTE_ATTR_BG(rt->cells[cy][cx].attr),
                          font,sh,sw); // inverse video for cursor
            last_cy=cy; last_cx=cx;
            count++;
        }
#ifdef HAVE_DLO
        memcpy(lasthash,hash,th*sizeof(lasthash[0]));
#endif
        if (!count) continue;
//fprintf(stderr,"count=%d\n",count);

        #ifdef HAVE_ST2205
        if (use_st2205 && picframe!=NULL) // push changes to picframe - copies entire display
        {
            if (picframe->oldpix) { free(picframe->oldpix); picframe->oldpix=NULL; } // avoid libst2205 incremental update bug
            st2205_send_data(picframe,pixels);
	    

        }
        #endif
	#ifdef HAVE_DPF
	if (use_dpf && picframe!=NULL) // push changes to picframe - copies entire display
        {
		short rect[4];
		rect[0]=0;
		rect[1]=0;
		rect[2]=sw;
		rect[3]=sh;	
		dpf_screen_blit(picframe,pixels,rect);
	}
	#endif
        #ifdef HAVE_DLO
        if (use_dlo && !dlo_broken) // push changes to displaylink device
        {
            dlo_bmpflags_t flags = { 0 };
            dlo_fbuf_t fbuf;
            dlo_dot_t dot;

            fbuf.width=maxx-minx;
            fbuf.height=maxy-miny;
            fbuf.base=pixels+miny*sw+minx;
            fbuf.stride=sw;
            fbuf.fmt=dlo_pixfmt_rgb323;
            dot.x=minx;
            dot.y=miny;
            if (dlo_copy_host_bmp(uid,flags,&fbuf,NULL,&dot)!=dlo_ok)

                dlo_broken=1;
        }
        #endif
    }
    return 0;
}
