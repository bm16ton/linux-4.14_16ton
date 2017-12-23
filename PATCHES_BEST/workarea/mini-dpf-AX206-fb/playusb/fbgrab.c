/*
 * fbgrab.c
 *
 *  Created on: Sep 26, 2010
 *      Author: Andre Puschmann
 *
 *  this is a stripped version of fbgrab.c without for instance PNG support
 *  originally written by Stephan Beyer <PH-Linex@gmx.net>
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/vt.h>
#include <png.h>
#include <jpeglib.h>
#include <linux/fb.h>
#include "playusb.h"


static int waitbfg=0; /* wait before grabbing (for -C )... */

/* some conversion macros */
#define RED565(x)    ((((x) >> (11 )) & 0x1f) << 3)
#define GREEN565(x)  ((((x) >> (5 )) & 0x3f) << 2)
#define BLUE565(x)   ((((x) >> (0)) & 0x1f) << 3)

#define ALPHA1555(x) ((((x) >> (15)) & 0x1 ) << 0)
#define RED1555(x)   ((((x) >> (10)) & 0x1f) << 3)
#define GREEN1555(x) ((((x) >> (5 )) & 0x1f) << 3)
#define BLUE1555(x)  ((((x) >> (0 )) & 0x1f) << 3)

struct picture{
  int xres,yres;
  char *buffer;
  struct fb_cmap *colormap;
  char bps,gray;
};

void FatalError(char* err){
  fprintf(stderr,"An error occured: %s %s\nExiting now...\n",err,strerror(errno));
  fflush(stderr);
  exit (1);
}

void chvt(int num){
  int fd;

  if(!(fd = open("/dev/console", O_RDWR)))
    FatalError("cannot open /dev/console");
  if (ioctl(fd, VT_ACTIVATE, num))
    FatalError("ioctl VT_ACTIVATE ");
  if (ioctl(fd, VT_WAITACTIVE, num))
    FatalError("ioctl VT_WAITACTIVE");
  close(fd);
  if (waitbfg)
    sleep (3);
}

int read_fb(char *device, int vt_num, unsigned char *buffer){
  int fd, vt_old, i,j;
  struct fb_fix_screeninfo fb_fixinfo;
  struct fb_var_screeninfo fb_varinfo;
  struct vt_stat vt_info;
  //char * buffer;

  if (vt_num!=-1){
    if ((fd = open("/dev/console", O_RDONLY)) == -1)
      FatalError("could not open /dev/console");
    if (ioctl(fd, VT_GETSTATE, &vt_info))
      FatalError("ioctl VT_GETSTATE");
    close (fd);
    vt_old=vt_info.v_active;
  }

  if(!(fd=open(device, O_RDONLY)))
    FatalError("Couldn't open framebuffer device");

  if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_fixinfo))
    FatalError("ioctl FBIOGET_FSCREENINFO");

  if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_varinfo))
    FatalError("ioctl FBIOGET_VSCREENINFO");


  if (vt_num!=-1)
    chvt(vt_old);



  //if(!(buffer=malloc(fb_varinfo.xres * fb_varinfo.yres * (3))))
  //  FatalError("couldnt malloc");

#if 0
  fprintf(stdout, "Framebuffer %s is %i bytes.\n", device,
                    (fb_varinfo.xres * fb_varinfo.yres * i));
  fprintf(stdout, "Grabbing %ix%i ... \n", fb_varinfo.xres, fb_varinfo.yres);
#endif

#ifdef DEBUG
/* Output some more information bout actual graphics mode
 */
  fprintf(stdout, "%ix%i [%i,%i] %ibps %igr\n",
  	fb_varinfo.xres_virtual, fb_varinfo.yres_virtual,
  	fb_varinfo.xoffset, fb_varinfo.yoffset,
  	fb_varinfo.bits_per_pixel, fb_varinfo.grayscale);
  fprintf(stdout, "FIX: card:%s mem:0x%.8X mem_len:%d visual:%i type:%i type_aux:%i line_len:%i accel:%i\n",
  fb_fixinfo.id,fb_fixinfo.smem_start,fb_fixinfo.smem_len,fb_fixinfo.visual,
  fb_fixinfo.type,fb_fixinfo.type_aux,fb_fixinfo.line_length,fb_fixinfo.accel);
#endif

  fflush(stdout);
  if (vt_num!=-1)
    chvt(vt_num);

  j= (read(fd, buffer, (fb_varinfo.xres * fb_varinfo.yres *(3))) )!=
  	(fb_varinfo.xres * fb_varinfo.yres *(3));
#ifdef DEBUG
  printf("to read:%i read:%i\n",(pict->xres* pict->yres * i), j);
#endif
  //fwrite (buffer , 1 , fb_varinfo.xres * fb_varinfo.yres *(3) , tmp_file );
  if (vt_num!=-1)
    chvt(vt_old);

  if(j)
    FatalError("couldn't read the framebuffer");
#if 0
  else
    fprintf(stdout,"done.\n");
#endif
  close (fd);
  return 0;
}


void convert8to32(struct picture *pict){
  int i;
  int j=0;
  __u8 c;
  char *out=(char*)malloc(pict->xres*pict->yres*4);
  for (i=0; i<pict->xres*pict->yres; i++)
  {
    c = ((__u8*)(pict->buffer))[i];
    out[j++]=(char)(pict->colormap->red[c]);
    out[j++]=(char)(pict->colormap->green[c]);
    out[j++]=(char)(pict->colormap->blue[c]);
    out[j++]=(char)(pict->colormap->transp[c]);
  }
  free(pict->buffer);
  pict->buffer=out;
}

void convert1555to32(struct picture *pict){
  int i;
  int j=0;
  __u16 t,c;
  char *out=(char*)malloc(pict->xres*pict->yres*4);
  for (i=0; i<pict->xres*pict->yres; i++)
  {
    c = ( (__u16*)(pict->buffer))[i];
    out[j++]=(char)RED1555(c);
    out[j++]=(char)GREEN1555(c);
    out[j++]=(char)BLUE1555(c);
    out[j++]=(char)ALPHA1555(c);
  }
  free(pict->buffer);
  pict->buffer=out;
}

void convert565to24(struct picture *pict){
  int i;
  int j=0;
  __u16 t,c;
  char *out=(char*)malloc(pict->xres*pict->yres*3);
  for (i=0; i<pict->xres*pict->yres; i++)
  {
    c = ( (__u16*)(pict->buffer))[i];
    out[j++]=(char)RED565(c);
    out[j++]=(char)GREEN565(c);
    out[j++]=(char)BLUE565(c);
  }
  free(pict->buffer);
  pict->buffer=out;
}


/* please note that in-memory compression doesn't work satisfactory */
int Write_JPG(struct picture *pict, FILE *tmp_file)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;
	int row_stride;						/* physical row width in buffer */
	int ret;
	JSAMPROW row_pointer[0];				/* pointer to a single row */


	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, tmp_file);

	cinfo.image_width      = pict->xres;
	cinfo.image_height     = pict->yres;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;

	/* call some jpeg helpers */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, 75, 1);	/*set the quality [0..100]  */
	jpeg_start_compress(&cinfo, 1);

    row_stride = cinfo.image_width;

	switch(pict->bps) {
	    case 8:
	      convert8to32(pict);
	      row_stride *= 4;
	      break;

	    case 15:
	      convert1555to32(pict);
	      row_stride *= 4;
	      break;

	    case 16:
	      convert565to24(pict);
	      row_stride *= 3;
	      break;

	    case 24:
	      //bit_depth=8;
	      row_stride *= 3;
	      break;

	    case 32:
	      //bit_depth=8;
	      row_stride *= 4;
	      break;

	    default:
	    	fprintf (stderr, "%d bits per pixel are not yet supported! ", pict->bps);
	    	fprintf (stderr, "But you may write it...\n");
			exit(1);
	}

    while (cinfo.next_scanline < cinfo.image_height)
	{
    	row_pointer[0] = pict->buffer + (cinfo.next_scanline * row_stride);
    	ret = jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
}


int grab_from_fb(char *fb_device, unsigned char * buf)
{
	struct picture pict;
	int vt_num = -1;


	/* read framebuffer into pict */
	read_fb(fb_device, vt_num, buf);

	/* compress to jpeg */
	// Write_JPG(&pict, tmp_file);

	/* cleanup */


	/* rewind file */
	//fseek(tmp_file, 0, SEEK_SET);
}
