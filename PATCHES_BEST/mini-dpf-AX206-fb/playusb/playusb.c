/*
 * playusb.c
 *
 *  Created on: Aug 2, 2010
 *      Author: Andre Puschmann
 *
 *		based on code from Grace Woo: http://web.media.mit.edu/~gracewoo/stuff/picframe/
 *
 *		- refresh rate of 100msec: ~35% CPU load
 *		- refresh rate of 1sec: ~7% CPU load
 */


#include "playusb.h"

void show_help(void)
{
   printf("./playusb [OPTIONS]\n"\
          "\t-f dev\tread from linux framebuffer\n"\
          "\t-i \tFB refresh interval (in milliseconds)\n"\
          "\t-e \twait for update events from debugfs (for mmap'ed FBs)\n"\
          "\t-r \tretry to find frame\n"\
          "\t-h \tthis text\n\n");
}


/* tries to find a photo frame connected to your computer
 *
 * \param pointer to a struct usb_device
 * \return 0  if frame was found
 * 		   -1 if no frame was found
 * 		   -2 if system rights are too low to work with device
 */

int send_data(DPFHANDLE frame, unsigned char *data, int len)
{
	int ret,m,n;
	unsigned char r,g,b;
	//int size=((frame->width)*(frame->height)*3);
	//unsigned char buffer[size];
	short rect[4];
	int filesize,i;
        unsigned char * pixels=(unsigned char*)malloc((frame->width)*(frame->height)*(2));
	rect[0]=0;
	rect[1]=0;
	rect[2]=frame->width;
	rect[3]=frame->height;
		// get file size
		//fseek(file, 0, SEEK_END);
		//filesize = ftell(file);
		//rewind (file);
		i=0;
		memset(pixels,0,(frame->width)*(frame->height)*2);
		//while(!feof(file)&& ret<(size))
		//{
			// read file into buffer
			//if ((ret = fread(buffer , 1, filesize,file)) < 1){
                        //        printf("Error while reading file, fread returned: %d\n", ret);
                        //        break;
                        //}
			for (m=0,n=0;m<((frame->width)*(frame->height)*3);){
				r = data[m++];
          			g = data[m++];
                                b = data[m++];
			        pixels[n++] = (unsigned char)(RGB565_0(r,g,b)); //R (5 bits) + G (upper 3 bits)
            			pixels[n++] = (unsigned char)(RGB565_1(r,g,b)); //G (lower 3 bits) + B (5 bits)
			}
			// pad bytes
			//break;
	               // memset(buffer + ret, 0, URBBUF_MAX - ret);
                        // send buffer to picture frame
                       

              //}
		 if ((ret = dpf_screen_blit(frame, pixels, rect)) < 0)
                        {
                                // error occurred
                                printf("Error while writing to USB device. Please check your system rights.\n");
                        }
		/* rewind file */
		//fseek(file, 0, SEEK_SET);

	
	
	return 0;
}


int main(int argc, char *argv[])
{
	char * dev="usb0";
	DPFHANDLE picframe;
	char *fb_device = NULL;
	FILE *file_handle;
	struct timespec t;
	long fb_refresh_interval = 1000; /* 1000ms*/
	char *tmp_filename;
	int ret = -1;
	int retry = 0;
	int opt;
	enum opmode_t opmode = ONCE;
	char buf;
	unsigned char * pixels;
	

	if (argc == 1)
	{
		show_help();
		return -1;
	}

	while((opt = getopt(argc, argv, "erhi:f:")) != -1)
	{
		switch (opt)
		{
			case 'h':
				show_help();
				return 0;
				break;
			case 'e':
				opmode = EVENT;
				break;
			case 'f':
				fb_device = optarg;
				break;
			case 'i':
				opmode = TIMER;
				fb_refresh_interval = atol(optarg);
				break;
			case 'r':
				retry = 1;
				break;
			default:
				return -1;
		}
	}


	/* sanity checks */
	if (fb_refresh_interval < 100)
		printf("WARNING: refresh rates smaller then 100msecs may cause heavy system load\n");

	/* convert to nanoseconds */
	fb_refresh_interval *= USEC_PER_SEC;

	/* install signal handler */
	signal(SIGINT, sigint);

	/* start */
	while ((ret = dpf_open(dev,&picframe)) != 0)
	{
		if (ret == -2)
		{
			/* just exit, message was already printed by find_photo_frame() */
			return -1;
		}
		else if (!retry)
		{
			printf("Sorry, no supported photo frame is connected to your computer.\n");
			return -1;
		}

		/* sleep a while and try again */
		sleep(1);
	}

	pixels=(unsigned char*)malloc((picframe->width)*(picframe->height)*(3));	
	if (fb_device)
	{
		/*
		 * workflow:
		 * - open tmp file
		 * - while (condition)
		 *     - grab_from_fb into tmpfile_handle (incl. jpeg compression)
		 *	   - send jpeg to device
		 * - close tmp file
		 */

		/* check wheter framebuffer device node exist */
		if (!(file_handle = fopen(fb_device, "r+")))
		{
			perror("Couldn't open framebuffer device");
			goto exit;
		}
		fclose(file_handle);

		/* create and open temporary file */
		//if (!(tmp_filename = tmpnam(NULL)))
		//{
		//	printf("error while creating temporary file name.\n");
		//	return -1;
		//}

		//if (!(tmpfile_handle = fopen(tmp_filename, "w+")))
		//{
		//	printf("error while opening temporary file.\n");
		//	return -1;
		//}

		/* do work depending on operation mode */
		if (opmode == ONCE)
		{
			printf("Writing content of FB device %s to picture frame ..\n", fb_device);

			/* just read from fb once and exit */
			grab_from_fb(fb_device, pixels);

			/* rewind file, so send_jpeg can read from it straight away */
		//	fseek(tmpfile_handle, 0, SEEK_SET);

			/* send image to frame */
			if ((ret = send_data(picframe, pixels, 0)) != 0)
			{
				printf("Error occurred while sending jpeg image to device.\n");
			}
		}
		else if (opmode == TIMER)
		{
			printf("Reading from FB device %s every %d milliseconds ..\n", fb_device, (int)(fb_refresh_interval / USEC_PER_SEC));

			/* prepare cyclic mode */
			clock_gettime(CLOCK_MONOTONIC ,&t);
			t.tv_sec++;

			while (1)
			{
				/* wait until next shot */
				clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

				/* get image from fb and compress to jpeg */
				grab_from_fb(fb_device, pixels);

				/* rewind file, so send_jpeg can read from it straight away */
				//fseek(tmpfile_handle, 0, SEEK_SET);

				/* send image to frame */
				if ((ret = send_data(picframe, pixels, 0)) != 0)
				{
					printf("Error occurred while sending image to device.\n");
				}

				/* calculate next shot */
				t.tv_nsec += fb_refresh_interval;
				tsnorm(&t);
			}
		}
		else if (opmode == EVENT)
		{
			printf("Waiting for update events from FB device %s ..\n", fb_device);

			/* open debugfs entry */
			if ((sysfs_handle = fopen(DEBUGFS_ENTRY, "r")) == NULL)
			{
				printf("File %s was not found.\n", DEBUGFS_ENTRY);
				return -1;
			}

			int ret;

			/* do the work */
			while ((ret = fread(&buf, 1, 1, sysfs_handle)) > -1)
			{
				/* get image from fb and compress to jpeg */
				grab_from_fb(fb_device, pixels);

				/* rewind file, so send_jpeg can read from it straight away */
				//fseek(tmpfile_handle, 0, SEEK_SET);

				/* send image to frame */
				if ((ret = send_data(picframe, pixels, 0)) != 0)
				{
					printf("Error occurred while sending image to device.\n");
				}

			}

			printf("fread ret: %d\n", ret);

			/* in case we reach this point, cleanup */
			fclose(sysfs_handle);
			free(pixels);
		}

		if (tmpfile_handle)
			fclose(tmpfile_handle);
		tmpfile_handle = NULL;
	}

exit:
	/* clean up and exit */
	usb_close(dev_handle);

	return ret;
}


void sigint(int signum)
{
	printf("Aborting operation ..\n");

	if (tmpfile_handle)
		fclose(tmpfile_handle);

	if (sysfs_handle)
		fclose(sysfs_handle);

	if (dev_handle)
		usb_close(dev_handle);

	exit(0);
}
