/** @file test1.c
 *
 *  @brief This file demonstrates basic graphics primitive features of libdlo.
 *
 *  DisplayLink Open Source Software (libdlo)
 *  Copyright (C) 2009, DisplayLink
 *  www.displaylink.com
 *
 *  This library is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU Library General Public License as published by the Free
 *  Software Foundation; LGPL version 2, dated June 1991.
 *
 *  This library is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "sys/time.h"
#include "../src/libdlo.h"
#include "../src/dlo_defs.h"


/** Default horizontal resolution we will use for tests (pixels).
 */
#define SCREEN_X (1280)

/** Default vertical resolution we will use for tests (pixels).
 */
#define SCREEN_Y (1024)

/** Default screen refresh rate we will use for tests (Hz).
 */
#define SCREEN_RATE (0)

/** Default screen colour depth we will use for tests (bits per pixel).
 */
#define SCREEN_BPP (24)

/** Number of random dots to plot.
 */
#define NUM_DOTS (299)

/** Number of random rectangles to plot.
 */
#define NUM_RECTS (99)

/** Number of steps (gradient) in colour blended rectangles.
 */
#define NUM_GRAD (99)

/** Division factor for scale of central rectangle.
 */
#define MID_REC_DIV (5)

/** Windows BMP magic ID bytes.
 */
#define BMP_MAGIC (0x4D42)

/** Given a bits per pixel value, return the bytes per pixel.
 */
#define BYTES_PER_PIXEL(bpp) (unsigned int)(((bpp) + 7) / 8)

/** Horizontal subdivision of the screen for overlapping rectangle copy tests.
 */
#define ZONE_X (SCREEN_X / 3)

/** Vertical subdivision of the screen for overlapping rectangle copy tests.
 */
#define ZONE_Y (SCREEN_Y / 3)

/** Horizontal size of the rectangle to copy in the overlapping rectangle copy tests.
 */
#define COPY_X (ZONE_X / 2)

/** Vertical size of the rectangle to copy in the overlapping rectangle copy tests.
 */
#define COPY_Y (ZONE_Y / 2)

/** Size of a component of a rectangle copy offset (horizontal or vertical).
 */
#define STEP (80)


/** Array of offsets for overlapping rectangle copy test.
 */
const dlo_dot_t overlap[9] =
{
  { -STEP, -STEP },
  {     0, -STEP },
  {  STEP, -STEP },
  { -STEP,     0 },
  {     0,     0 },
  {  STEP,     0 },
  { -STEP,  STEP },
  {     0,  STEP },
  {  STEP,  STEP }
};


/** The main header block from a Windows BMP file.
 */
typedef struct __packed bmp_header_s
{
  uint16_t magic;          /**< Magic ID bytes for Windows BMP format. */
  uint32_t file_sz;        /**< Total bitmap file size (bytes). */
  uint16_t reserved1;      /**< Unused. */
  uint16_t reserved2;      /**< Unused. */
  uint32_t pix_offset;     /**< Offset from start of file to start of pixel data (bytes). */
} bmp_header_t;            /**< A struct @a bmp_header_s. */


/** The DIB header block from a Windows BMP file.
 */
typedef struct __packed dib_header_s
{
  uint32_t dib_hdr_sz;     /**< Size of the DIB header block (bytes). */
  uint32_t width;          /**< Width of the bitmap (pixels). */
  uint32_t height;         /**< Height of the bitmap (pixels). */
  uint16_t col_planes;     /**< Number of colour planes. */
  uint16_t bpp;            /**< Bits per pixel. */
  uint32_t compression;    /**< Compression, pixel format. */
  uint32_t raw_size;       /**< Size of the raw pixel data. */
  uint32_t x_pix_meter;    /**< Horizontal resolution (pixels per meter). */
  uint32_t y_pix_meter;    /**< Vertical resolution (pixels per meter). */
  uint32_t pal_entries;    /**< Number of palette entries. */
  uint32_t imp_cols;       /**< Important colours (ignored). */
} dib_header_t;            /**< A struct @a dib_header_s. */


/** A Windows BMP file.
 */
typedef struct __packed bmp_s
{
  bmp_header_t hdr;        /**< Windows BMP header block. */
  dib_header_t dib;        /**< DIB header block. */
  uint8_t      data[];     /**< Pixel data. */
} bmp_t;                   /**< A struct @a bmp_s. */


/** Report an error and exit.
 *
 *  @param  str  Pointer to the error message string.
 */
static void my_error(const char * const str)
{
  printf("test: ERROR: %s\n", str);
  exit(1);
}


/** Return the microsecond time, as an unsigned 64 bit integer.
 *
 *  @return  Number of microseconds since the start of the Epoch.
 */
static uint64_t now(void)
{
  static struct timeval  unix_time     = { 0 };
  static struct timeval *unix_time_ptr = &unix_time;

  gettimeofday(unix_time_ptr, NULL);
  return ((uint64_t)unix_time.tv_sec * 1000000ll) + (uint64_t)unix_time.tv_usec;
}


/** Pause execution for the specified number of centiseconds.
 *
 *  @param  from      Time to start the pause interval.
 *  @param  millisec  Number of milliseconds to pause for.
 */
static void wait_ms(const uint64_t from, const uint64_t millisec)
{
  uint64_t to = from + (millisec * 1000);

  while (to > now()) ;
}


/** Plot a rectangular outline in the specified colour.
 *
 *  @param  uid   Unique ID of the device.
 *  @param  view  Viewport for plot destination.
 *  @param  rec   Rectangle co-ordinates within the viewport.
 *  @param  col   Colour of rectangle.
 *
 *  @return  Return code, zero for no error.
 */
static dlo_retcode_t box(const dlo_dev_t uid, const dlo_view_t * const view, const dlo_rect_t * const rec, const dlo_col32_t col)
{
  dlo_rect_t line;

  line.origin.x = rec->origin.x;
  line.origin.y = rec->origin.y;
  line.width    = rec->width;
  line.height   = 1;
  ERR(dlo_fill_rect(uid, view, &line, col));

  line.width    = 1;
  line.height   = rec->height;
  ERR(dlo_fill_rect(uid, view, &line, col));

  line.origin.y += line.height - 1;
  line.width     = rec->width;
  line.height    = 1;
  ERR(dlo_fill_rect(uid, view, &line, col));

  line.origin.x += line.width;
  line.origin.y  = rec->origin.y;
  line.width     = 1;
  line.height    = rec->height;
  ERR(dlo_fill_rect(uid, view, &line, col));

  return dlo_ok;
}


/** Test the basic graphics primitives (filled rectangle and rectangle copy).
 *
 *  @param  uid  Unique ID of the device.
 *
 *  @return  Return code, zero for no error.
 */
static dlo_retcode_t basic_grfx_test(const dlo_dev_t uid)
{
  dlo_mode_t     mode;
  dlo_mode_t    *mode_info;
  dlo_devinfo_t *dev_info;
  dlo_view_t    *view;
  dlo_rect_t      rec;
  dlo_dot_t      dot;
  uint32_t        i;
  uint64_t        start;

  /* Read some information on the device */
  dev_info = dlo_device_info(uid);
  NERR(dev_info);
  printf("test: device info: uid &%X\n", (int)dev_info->uid);
  printf("test: device info: serial %s\n", dev_info->serial);
  printf("test: device info: type  &%X\n", (uint32_t)dev_info->type);

  /* Read current mode information (if we're already in the display's native mode) */
  mode_info = dlo_get_mode(uid);
  NERR(mode_info);
  printf("test: native mode info...\n");
  printf("  %ux%u @ %u Hz %u bpp base &%X\n", mode_info->view.width, mode_info->view.height, mode_info->refresh, mode_info->view.bpp, (int)mode_info->view.base);

  /* Select the monitor's preferred mode, based on EDID */
  printf("test: set_mode...\n");
  ERR(dlo_set_mode(uid, NULL));
      
  /* Read current mode information */
  mode_info = dlo_get_mode(uid);
  NERR(mode_info);
  printf("test: mode info...\n");
  printf("  %ux%u @ %u Hz %u bpp base &%X\n", mode_info->view.width, mode_info->view.height, mode_info->refresh, mode_info->view.bpp, (int)mode_info->view.base);
  view = &(mode_info->view);

  /* Clear screen */
  printf("test: cls...");
  start = now();
  ERR(dlo_fill_rect(uid, NULL, NULL, DLO_RGB(0, 0, 0)));
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  /* Plot some random white dots */
  printf("test: random white dots...");
  start      = now();
  rec.width  = 1;
  rec.height = 1;
  for (i = 0; i < NUM_DOTS; i++)
  {
    rec.origin.x = rand() % view->width;
    rec.origin.y = rand() % view->height;
    ERR(dlo_fill_rect(uid, view, &rec, DLO_RGB(0xFF, 0xFF, 0xFF)));
  }
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  /* Plot random filled rectangles */
  printf("test: random rectangles...");
  start = now();
  for (i = 0; i < NUM_RECTS; i++)
  {
    rec.width    = rand() % (view->width  / 4);
    rec.height   = rand() % (view->height / 4);
    rec.origin.x = (rand() % (uint32_t)(1.25 * view->width))  - (view->width  / 8);
    rec.origin.y = (rand() % (uint32_t)(1.25 * view->height)) - (view->height / 8);
    ERR(dlo_fill_rect(uid, view, &rec, DLO_RGB(rand() % 0xFF, rand() % 0xFF, rand() % 0xFF)));
  }
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  /* Plot a set of rectangles, one over the other (colour gradient) */
  printf("test: central rectangles...");
  start = now();
  for (i = 0; i < NUM_GRAD; i++)
  {
    dlo_col32_t col = DLO_RGB((i*3) % 256, (i*5) % 256, 255 - ((i*7) % 256));

    rec.width    = (view->width  / MID_REC_DIV) - 2*i;
    rec.height   = (view->height / MID_REC_DIV) - 2*i;
    rec.origin.x = i + (view->width  / 2) - (view->width / MID_REC_DIV / 2);
    rec.origin.y = i + (view->height / 2) - (view->height/ MID_REC_DIV / 2);
    ERR(dlo_fill_rect(uid, view, &rec, col));
  }
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  /* Plot the outline of the box we're going to copy */
  printf("test: white box outline...");
  start = now();
  rec.width    = view->width  / MID_REC_DIV;
  rec.height   = view->height / MID_REC_DIV;
  rec.origin.x = (view->width  / 2) - (rec.width  / 2);
  rec.origin.y = (view->height / 2) - (rec.height / 2);
  ERR(box(uid, view, &rec, DLO_RGB(0xFF, 0xFF, 0xFF)));
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  /* Copy a rectangle from the centre of the screen to other locations */
  printf("test: copy central box...");
  start = now();
  rec.origin.x++;
  rec.origin.y++;
  rec.width  -= 2;
  rec.height -= 2;
  dot.x       = 8;
  dot.y       = 8;
  ERR(dlo_copy_rect(uid, view, &rec, view, &dot));
  dot.x = view->width - 8 - rec.width;
  ERR(dlo_copy_rect(uid, view, &rec, view, &dot));
  dot.y = view->height - 8 - rec.height;
  ERR(dlo_copy_rect(uid, view, &rec, view, &dot));
  dot.x = 8;
  ERR(dlo_copy_rect(uid, view, &rec, view, &dot));
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  /* Check that basic clipping works */
  printf("test: copy central box (off edges of viewport)...");
  start = now();
  dot.x = -(rec.width / 2);
  dot.y = (view->height / 2) - (rec.height / 2) - 32;
  ERR(dlo_copy_rect(uid, view, &rec, view, &dot));
  dot.x += view->width;
  dot.y += 64;
  ERR(dlo_copy_rect(uid, view, &rec, view, &dot));
  dot.x = (view->width / 2) - (rec.width / 2) - 128;
  dot.y = -(rec.height / 2);
  ERR(dlo_copy_rect(uid, view, &rec, view, &dot));
  dot.x += 256;
  dot.y += view->height;
  ERR(dlo_copy_rect(uid, view, &rec, view, &dot));
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  return dlo_ok;
}


/** Plot a few coloured rectangles one above another.
 *
 *  @param  uid    Unique ID of the device.
 *  @param  inrec  Input rectangle (the outer bounding box of the design).
 *
 *  @return  Return code, zero for no error.
 */
static dlo_retcode_t squares(const dlo_dev_t uid, const dlo_rect_t * const inrec)
{
  dlo_rect_t rec = *inrec;

  /* Plot the outer (red) rectangle */
  ERR(dlo_fill_rect(uid, NULL, &rec, DLO_RGB(0xCC, 0, 0)));

  /* Plot the middle (white) rectangle */
  rec.origin.x += 20;
  rec.origin.y += 20;
  rec.width    -= 40;
  rec.height   -= 40;
  ERR(dlo_fill_rect(uid, NULL, &rec, DLO_RGB(0xCC, 0xCC, 0xCC)));

  /* Plot the inner (blue) rectangle */
  rec.origin.x += 20;
  rec.origin.y += 20;
  rec.width    -= 40;
  rec.height   -= 40;

  return dlo_fill_rect(uid, NULL, &rec, DLO_RGB(0, 0, 0xCC));
}


/** Test copying rectangles to and from overlapping regions (in various directions).
 *
 *  @param  uid  Unique ID of the device.
 *
 *  @return  Return code, zero for no error.
 */
static dlo_retcode_t overlap_test(const dlo_dev_t uid)
{
  dlo_rect_t rec;
  dlo_dot_t mid, dot;
  uint32_t   idx = 0;

  /* Clear screen to black */
  ERR(dlo_fill_rect(uid, NULL, NULL, DLO_RGB(0, 0, 0)));

  rec.width  = COPY_X;
  rec.height = COPY_Y;
  for (mid.y = ZONE_Y / 2; mid.y < SCREEN_Y; mid.y += ZONE_Y)
  {
    for (mid.x = ZONE_X / 2; mid.x < SCREEN_X; mid.x += ZONE_X)
    {
      rec.origin.x = mid.x - (COPY_X / 2);
      rec.origin.y = mid.y - (COPY_Y / 2);
      dot.x        = rec.origin.x + overlap[idx].x;
      dot.y        = rec.origin.y + overlap[idx].y;
      idx++;
      ERR(squares(uid, &rec));
      ERR(dlo_copy_rect(uid, NULL, &rec, NULL, &dot));
    }
  }
  return dlo_ok;
}


/** Convert a bits per pixel value into a bytes per pixel value.
 *
 *  @param  bpp  Bits per pixel.
 *
 *  @return  Bytes per pixel.
 */
static uint8_t bpp_to_bytes(const uint32_t bpp)
{
  return (uint8_t)((bpp + 7) / 8);
}


/** Test the use of viewports as screen banks (ensure clipping is working).
 *
 *  @param  uid  Unique ID of the device.
 *
 *  @return  Return code, zero for no error.
 *
 *  Create three viewports (screen banks) and switch the display between them.
 *  While we're at it, test the filled rectangle plotting clips correctly and
 *  doesn't overflow into surrounding screen banks or off the edges of the
 *  screen.
 */
static dlo_retcode_t viewport_test(const dlo_dev_t uid)
{
  dlo_mode_t  mode;
  dlo_mode_t *desc;
  dlo_view_t  view[3];
  dlo_rect_t   rec;
  uint32_t     i;
  uint64_t     start;

  /* Read current mode information */
  desc = dlo_get_mode(uid);
  NERR(desc);

  /* Create three viewports - each representing a screen bank */
  view[0]      = desc->view;
  view[1]      = view[0];
  view[1].base = view[0].base + (view[1].width * view[1].height * bpp_to_bytes(view[1].bpp));
  view[2]      = view[1];
  view[2].base = view[1].base + (view[2].width * view[2].height * bpp_to_bytes(view[2].bpp));

  /* Clear screens to different colours */
  printf("test: cls (three banks)...");
  start = now();
  ERR(dlo_fill_rect(uid, NULL,     NULL, DLO_RGB(0, 0, 0xFF)));
  ERR(dlo_fill_rect(uid, &view[1], NULL, DLO_RGB(0, 0xFF, 0)));
  ERR(dlo_fill_rect(uid, &view[2], NULL, DLO_RGB(0xFF, 0, 0)));
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  /* Plot a couple of rectangles in each bank to test the clipping */
  printf("test: plot crosses (three banks)...");
  start = now();
  rec.width    = view[0].width / 8;
  rec.height   = view[0].height * 1.5;
  rec.origin.x = (view[0].width / 2) - (rec.width / 2);
  rec.origin.y = -(view[0].height / 4);
  ERR(dlo_fill_rect(uid, NULL,     &rec, DLO_RGB(0, 0xFF, 0xFF)));
  ERR(dlo_fill_rect(uid, &view[1], &rec, DLO_RGB(0xFF, 0xFF, 0)));
  ERR(dlo_fill_rect(uid, &view[2], &rec, DLO_RGB(0xFF, 0, 0xFF)));
  rec.width    = view[0].width * 1.5;
  rec.height   = view[0].height / 8;
  rec.origin.x = -(view[0].width / 4);
  rec.origin.y = (view[0].height / 2) - (rec.height / 2);
  ERR(dlo_fill_rect(uid, NULL,     &rec, DLO_RGB(0, 0xFF, 0xFF)));
  ERR(dlo_fill_rect(uid, &view[1], &rec, DLO_RGB(0xFF, 0xFF, 0)));
  ERR(dlo_fill_rect(uid, &view[2], &rec, DLO_RGB(0xFF, 0, 0xFF)));
  printf(" took %u ms\n", (uint32_t)(now() - start) / 1000);

  /* Switch through the screen banks */
  for (i = 1; i < 4; i++)
  {
    wait_ms(now(), 1000);

    printf("test: switching to screen bank %u...\n", i % 3);
    mode.view.width  = view[i % 3].width;
    mode.view.height = view[i % 3].height;
    mode.view.bpp    = view[i % 3].bpp;
    mode.view.base   = view[i % 3].base;
    mode.refresh     = 0;
    ERR(dlo_set_mode(uid, &mode));
  }

  return dlo_ok;
}


/** Load a Windows BMP file into memory.
 *
 *  @param  bmpfile  Pointer to the filename.
 *
 *  @return  Pointer to the loaded bitmap's structure (or NULL if failed).
 *
 *  Load a bitmap from a file and create a @a dlo_fbuf_t structure to suit.
 *  If the bitmap has a palette, we need to swap the red/blue order of the
 *  colour components in order to convert the palette entries into
 *  @a dlo_col32_t values.
 */
static bmp_t *load_bmp(const char * const bmpfile)
{
  long int      size;
  bmp_t        *bmp;
  FILE         *fp;
  char         filename[255];
  const char   dir1[] = "images/";
  const char   images[] = "test/images/";

  strcpy(filename, "images/");
  strncat(filename, bmpfile, sizeof(filename) - 8);
  fp = fopen(filename, "rb");
  if (!fp) {
    strcpy(filename, "test/images/");
    strncat(filename, bmpfile, sizeof(filename) - 14);
    fp = fopen(filename, "rb");
    if (!fp) {
      return NULL;
    }
  }

  if (fseek(fp, 0, SEEK_END))
    return NULL;

  size = ftell(fp);
  if (size == -1L)
    return NULL;

  if (fseek(fp, 0, SEEK_SET))
    return NULL;

  bmp = malloc(size);
  if (!bmp)
    return NULL;

  if (1 != fread(bmp, size, 1, fp))
    goto error;

  if (bmp->hdr.magic != BMP_MAGIC)
    goto error;

  /* If there is a palette, we need to reverse the RGB component order */
  if (bmp->dib.pal_entries)
  {
    dlo_col32_t *palette;
    uint32_t      i;

    palette = (dlo_col32_t *)((unsigned long)bmp + sizeof(bmp_header_t) + sizeof(dib_header_t));

    for (i = 0; i < bmp->dib.pal_entries; i++)
    {
      dlo_col32_t col = palette[i];

      palette[i] = DLO_RGB(DLO_RGB_GETBLU(col), DLO_RGB_GETGRN(col), DLO_RGB_GETRED(col));
    }
  }
  return bmp;

error:
  free(bmp);

  return NULL;
}


/** Given a bitmap pointer, check various aspects of it and return a framebuffer structure pointer.
 *
 *  @param  bmp  Pointer to a loaded bitmap structure.
 *
 *  @return  Pointer to a framebuffer structure associated with the bitmap.
 *
 *  NOTE: the bitmap plotting code requires some special-case Windows BMP format
 *  bitmaps as input; they must include a DIB header (very common) and their width
 *  must be such that they don't require any padding bytes at the end of each pixel
 *  row.
 *
 *  The padding issue could be fixed very simply by calling the dlo_copy_host_bmp()
 *  for each pixel row individually but for the sake of a simple demo, we impose
 *  the constraint and make only one call to dlo_copy_host_bmp() here.
 */
static dlo_fbuf_t *bmp_to_fbuf(const bmp_t * const bmp)
{
  static dlo_fbuf_t fbuf;

  printf("bmp->hdr.magic       %04X\n"
         "bmp->hdr.file_sz     %08X (%u)\n"
         "bmp->hdr.reserved1   %04X\n"
         "bmp->hdr.reserved2   %04X\n"
         "bmp->hdr.pix_offset  %08X\n"
         "bmp->dib.dib_hdr_sz  %08X\n"
         "bmp->dib.width       %08X (%u)\n"
         "bmp->dib.height      %08X (%u)\n"
         "bmp->dib.col_planes  %04X\n"
         "bmp->dib.bpp         %04X (%u)\n"
         "bmp->dib.compression %08X\n"
         "bmp->dib.raw_size    %08X (%u)\n"
         "bmp->dib.x_pix_meter %08X\n"
         "bmp->dib.y_pix_meter %08X\n"
         "bmp->dib.pal_entries %08X (%u)\n"
         "bmp->dib.imp_cols    %08X\n",
         bmp->hdr.magic,
         bmp->hdr.file_sz, bmp->hdr.file_sz,
         bmp->hdr.reserved1,
         bmp->hdr.reserved2,
         bmp->hdr.pix_offset,
         bmp->dib.dib_hdr_sz,
         bmp->dib.width, bmp->dib.width,
         bmp->dib.height, bmp->dib.height,
         bmp->dib.col_planes,
         bmp->dib.bpp, bmp->dib.bpp,
         bmp->dib.compression,
         bmp->dib.raw_size, bmp->dib.raw_size,
         bmp->dib.x_pix_meter,
         bmp->dib.y_pix_meter,
         bmp->dib.pal_entries, bmp->dib.pal_entries,
         bmp->dib.imp_cols);

  if (bmp->dib.compression)
    my_error("Unsupported bitmap compression mode");
  if (bmp->dib.col_planes != 1)
    my_error("Unsupported bitmap colour plane specification");
  if ((bmp->dib.width * BYTES_PER_PIXEL(bmp->dib.bpp)) & 3)
    my_error("Bitmap width must be whole multiple of four bytes (no padding)");

  fbuf.width  = bmp->dib.width;
  fbuf.height = bmp->dib.height;
  fbuf.base   = bmp->hdr.pix_offset + (uint8_t *)bmp;
  fbuf.stride = fbuf.width;
  switch (bmp->dib.bpp)
  {
    case 8:
    {
      fbuf.fmt = (dlo_pixfmt_t)(sizeof(bmp_header_t) + sizeof(dib_header_t) + (uint8_t *)bmp);
      if (bmp->dib.pal_entries != 256)
        my_error("Unsupported bitmap palette size");
      break;
    }
    case 16:
    {
      fbuf.fmt = dlo_pixfmt_srgb1555;
      break;
    }
    case 24:
    {
      fbuf.fmt = dlo_pixfmt_rgb888;
      break;
    }
    case 32:
    {
      fbuf.fmt = dlo_pixfmt_argb8888;
      break;
    }
    default:
      my_error("Unsupported bitmap colour depth");
  }
  return &fbuf;
}


/** Run some tests involving the loading, scraping and plotting of a bitmap.
 *
 *  @param  uid      Unique ID of the device.
 *  @param  bmpfile  Pointer to a Windows BMP filename.
 *
 *  @return  Return code, zero for no error.
 *
 *  Load a bitmap, clear the screen and plot the bitmap at the centre of the
 *  screen as well as off each edge of the screen.
 */
static dlo_retcode_t bitmap_test(const dlo_dev_t uid, const bool cross, const char * const bmpfile)
{
  dlo_bmpflags_t flags = { 0 };
  dlo_retcode_t  err;
  dlo_mode_t    *desc;
  dlo_fbuf_t    *fbuf;
  dlo_view_t     view;
  dlo_dot_t      dot;
  bmp_t          *bmp;

  printf("\ntest: bitmap file '%s'\n", bmpfile);

  /* Read current mode information */
  desc = dlo_get_mode(uid);
  NERR(desc);
  view = desc->view;

  /* Clear screen to black */
  ERR(dlo_fill_rect(uid, NULL, NULL, DLO_RGB(0, 0, 0)));

  /* Load the Windows BMP bitmap file into memory */
  bmp = load_bmp(bmpfile);
  NERR(bmp);

  /* Initialise a dlo_fbuf structure from our loaded bitmap file  */
  fbuf = bmp_to_fbuf(bmp);
  NERR_GOTO(fbuf);

  /* Test plotting of bitmap in centre of screen */
  dot.x = (view.width  / 2) - (fbuf->width  / 2);
  dot.y = (view.height / 2) - (fbuf->height / 2);
  flags.v_flip = 1;
  ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));

  /* Now a few plots that lie a little off the edges of the screen */
  flags.v_flip = 0;
  if (cross)
  {
    dot.y = -fbuf->height / 2;
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));
    dot.y += view.height;
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));
    dot.x = -fbuf->width / 2;
    dot.y = (view.height / 2) - (fbuf->height / 2);
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));
    dot.x += view.width;
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));
  }
  else
  {
    dot.x  = -fbuf->width  / 2;
    dot.y  = -fbuf->height / 2;
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));
    dot.y += view.height;
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));
    dot.x += view.width;
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));
    dot.y -= view.height;
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, NULL, &dot));
  }

  /* Discard the bitmap */
  free(bmp);

  return dlo_ok;

error:
  free(bmp);
  return err;
}


/** Run some screen scraping tests with a set of input bitmaps.
 *
 *  @param  uid  Unique ID of the device.
 *
 *  @return  Return code, zero for no error.
 *
 *  This test sequence checks that the @c dlo_copy_host_bmp() call works correctly for
 *  a number of source pixel formats by loading some different bitmaps and scraping
 *  directly from them (using their palette where required).
 */
static dlo_retcode_t scrape_tests(const dlo_dev_t uid)
{
  ERR(bitmap_test(uid, false, "test08.bmp"));

  wait_ms(now(), 1000);
  ERR(bitmap_test(uid, true, "test16.bmp"));

  wait_ms(now(), 1000);
  ERR(bitmap_test(uid, false, "test24.bmp"));

  wait_ms(now(), 1000);
  return bitmap_test(uid, true, "test32.bmp");
}


/** Test the clipping of bitmaps relative to other screen banks.
 *
 *  @param  uid  Unique ID of the device.
 *
 *  @return  Return code, zero for no error.
 *
 *  The point of this test is to plot lots of bitmaps in random positions (some
 *  completely off-screen) into the middle of three screen banks. We switch between
 *  each bank to check that the bitmaps only appear on the middle one and haven't
 *  spilled over into the surrounding memory.
 */
static dlo_retcode_t bmp_clip_test(const dlo_dev_t uid)
{
  dlo_bmpflags_t flags = { 0 };
  dlo_retcode_t  err;
  dlo_mode_t     mode;
  dlo_mode_t    *desc;
  dlo_fbuf_t    *fbuf;
  dlo_view_t     view[3];
  dlo_dot_t      dot;
  bmp_t          *bmp;
  uint32_t        i;

  /* Read current mode information */
  desc = dlo_get_mode(uid);
  NERR(desc);

  /* Create three viewports - each representing a screen bank */
  view[0]      = desc->view;
  view[1]      = view[0];
  view[1].base = view[0].base + (view[1].width * view[1].height * bpp_to_bytes(view[1].bpp));
  view[2]      = view[1];
  view[2].base = view[1].base + (view[2].width * view[2].height * bpp_to_bytes(view[2].bpp));

  /* Clear screen banks */
  wait_ms(now(), 2000);
  ERR(dlo_fill_rect(uid, NULL,     NULL, DLO_RGB(0, 0, 0x50)));
  ERR(dlo_fill_rect(uid, &view[1], NULL, DLO_RGB(0, 0, 0)));
  ERR(dlo_fill_rect(uid, &view[2], NULL, DLO_RGB(0, 0x50, 0)));

  /* Load the Windows BMP bitmap file into memory */
  bmp = load_bmp("test08.bmp");
  NERR(bmp);

  /* Initialise a dlo_fbuf structure from our loaded bitmap file  */
  fbuf = bmp_to_fbuf(bmp);
  NERR_GOTO(fbuf);

  /* Switch to middle bank */
  wait_ms(now(), 2000);
  mode.view.width  = view[1].width;
  mode.view.height = view[1].height;
  mode.view.bpp    = view[1].bpp;
  mode.view.base   = view[1].base;
  mode.refresh     = 0;
  ERR_GOTO(dlo_set_mode(uid, &mode));

  /* Plot lots of bitmaps into the second screen bank */
  for (i = 0; i < 399; i++)
  {
    flags.v_flip = rand() % 2;
    dot.x        = -fbuf->width  + (rand() % (view[1].width  + fbuf->width));
    dot.y        = -fbuf->height + (rand() % (view[1].height + fbuf->height));
    ERR_GOTO(dlo_copy_host_bmp(uid, flags, fbuf, &view[1], &dot));
  }
    
  /* Switch to third bank */
  wait_ms(now(), 2000);
  mode.view.width  = view[2].width;
  mode.view.height = view[2].height;
  mode.view.bpp    = view[2].bpp;
  mode.view.base   = view[2].base;
  mode.refresh     = 0;
  ERR_GOTO(dlo_set_mode(uid, &mode));

  /* Switch to middle bank */
  wait_ms(now(), 2000);
  mode.view.width  = view[1].width;
  mode.view.height = view[1].height;
  mode.view.bpp    = view[1].bpp;
  mode.view.base   = view[1].base;
  mode.refresh     = 0;
  ERR_GOTO(dlo_set_mode(uid, &mode));
  wait_ms(now(), 1000);

  /* Discard the bitmap */
  free(bmp);

  return dlo_ok;

error:
  free(bmp);
  return err;
}


/**********************************************************************/
int main(int argc, char *argv[])
{
  dlo_init_t     ini_flags = { 0 };
  dlo_final_t    fin_flags = { 0 };
  dlo_claim_t    cnf_flags = { 0 };
  dlo_retcode_t  err;
  dlo_dev_t      uid = 0;

  IGNORE(argc);
  IGNORE(argv);

  printf("test: argv[0]: %s\n",argv[0]);

  /* Initialise the random number generator with the microsecond time as a seed */
  srand((unsigned int)now());

  /* Initialise libdlo */
  printf("test: init...\n");
  ERR_GOTO(dlo_init(ini_flags));

  /* Look for a DisplayLink device to connect to */
  uid = dlo_claim_first_device(cnf_flags, 0);

  /* If we found one, perform some tests with it */
  if (uid)
  {
    printf("\ntest: basic graphics tests...\n");
    ERR_GOTO(basic_grfx_test(uid));
    wait_ms(now(), 3000);

    printf("\test: overlapping copy tests...\n");
    ERR_GOTO(overlap_test(uid));

    wait_ms(now(), 3000);

    printf("\ntest: viewport tests...\n");
    ERR_GOTO(viewport_test(uid));

    // These tests may fail if running from
    // the wrong directory, and bitmap images
    // aren't present. TODO: fix and restore
    // error checking.
    printf("\ntest: screen scraping tests...\n");
    scrape_tests(uid);

    printf("test: bitmap clipping test...\n");
    bmp_clip_test(uid);

    printf("test: release &%X...\n", (uintptr_t)uid);
    dlo_release_device(uid);
  } else {
    printf("test: no DisplayLink devices found\n");
    goto error;
  }

  /* Finalise libdlo, free up resources */
  printf("test: final...\n");
  ERR_GOTO(dlo_final(fin_flags));
  printf("test: finished.\n");
  return 0;

error:
  printf("test: error %u '%s'\n", (int)err, dlo_strerror(err));
  return 1;
}
