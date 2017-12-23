/*
 * playusb.h
 *
 *  Created on: Sep 25, 2010
 *      Author: Andre Puschmann
 */

#ifndef PLAYUSB_H_
#define PLAYUSB_H_

#include <usb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <getopt.h>

struct frame_info
{
	char name[64];
	int vendorid;
	int productid_massstorage;
	int productid_photo;
	int xres;
	int yres;
};


static struct frame_info photo_frames[] = {
	{
		.name					= "Samsung SPF-85H",
		.vendorid				= 0x04e8,
		.productid_massstorage	= 0x2012,
		.productid_photo		= 0x2013,
		.xres					= 800,
		.yres					= 600,
	},
	{
		.name					= "Samsung SPF-87H",
		.vendorid				= 0x04e8,
		.productid_massstorage	= 0x2033,
		.productid_photo		= 0x2034,
		.xres					= 800,
		.yres					= 480,
	},
	{
		.name					= "Samsung SPF-107H",
		.vendorid				= 0x04e8,
		.productid_massstorage	= 0x2027,
		.productid_photo		= 0x2028,
		.xres					= 1024,
		.yres					= 600,
	},
};
static int num_frames = sizeof(photo_frames) / sizeof(photo_frames[0]);

int find_photo_frame(struct usb_device **new_dev);
int switch_usb_mode(struct usb_device* dev);
int send_jpeg(usb_dev_handle *dev, FILE *file, char *data, int len);
void sigint(int signum);
int grab_from_fb(char *fb_device, FILE *tmp_file);


#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define URBBUF_MAX 0x20000
#define USB_HDR_LEN 12
#define MAX_JPEG_SIZE (2 * 1024 * 1024)
#define NSEC_PER_SEC    1000000000 /* The number of nsecs per sec. */
#define USEC_PER_SEC    1000000

#define DEBUGFS_ENTRY "/sys/kernel/debug/spfb"
#define PLAYUSB_TEMPLATE "/tmp/playusb"


enum opmode_t {
    ONCE = 0,
    TIMER,
    EVENT
};


/* the struct timespec consists of nanoseconds
 * and seconds. if the nanoseconds are getting
 * bigger than 1000000000 (= 1 second) the
 * variable containing seconds has to be
 * incremented and the nanoseconds decremented
 * by 1000000000.
 */
static inline void tsnorm(struct timespec *ts)
{
   while (ts->tv_nsec >= NSEC_PER_SEC) {
      ts->tv_nsec -= NSEC_PER_SEC;
      ts->tv_sec++;
   }
}

FILE *tmpfile_handle;
FILE *sysfs_handle;
usb_dev_handle *dev_handle;


#endif /* PLAYUSB_H_ */
