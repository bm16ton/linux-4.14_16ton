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
#include "dpf.h"

struct frame_info
{
	char name[64];
	int vendorid;
	int productid_massstorage;
	int productid_photo;
	int xres;
	int yres;
};


int send_data(DPFHANDLE frame, unsigned char *data, int len);
void sigint(int signum);
int grab_from_fb(char *fb_device, unsigned char * buf);


#define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define URBBUF_MAX (128*128*3)
#define USB_HDR_LEN 12
#define MAX_JPEG_SIZE (2 * 1024 * 1024)
#define NSEC_PER_SEC    1000000000 /* The number of nsecs per sec. */
#define USEC_PER_SEC    1000000

#define DEBUGFS_ENTRY "/sys/kernel/debug/ax206fb"
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
