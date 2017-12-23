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
          "\t-j \tshow JPEG image\n"\
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
int find_photo_frame(struct usb_device **new_dev)
{
	struct usb_bus *bus;
	struct usb_device *dev;
	int i;


scan_bus:
	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (bus = usb_busses; bus; bus = bus->next)
	{
		for (dev = bus->devices; dev; dev = dev->next)
		{
			/* check whether device is a photo frame */
			for (i = 0; i < num_frames; i++)
			{
				if (dev->descriptor.idVendor == photo_frames[i].vendorid)
				{
					if (dev->descriptor.idProduct == photo_frames[i].productid_photo)
					{
						/* frame in photo mode found */
						printf("%s in photo mode found.\n", photo_frames[i].name);
						*new_dev = dev;
						return 0;
					}
					else if (dev->descriptor.idProduct == photo_frames[i].productid_massstorage)
					{
						/* frame in mass storage mode found, try to set photo mode */
						printf("%s in mass storage mode found, try to switch mode.\n", photo_frames[i].name);
						if (switch_usb_mode(dev) == 0)
						{
							printf("Frame succesfully switched to photo mode. Rescan bus ..\n");
							/* sleep awhile until the new device is set up, then do all this again in order
							 * to initialize the new usb device */
							sleep(1);
							goto scan_bus;
						}

						/* switch_usb_mode() returned an error, so we might not have the right rights */
						return -2;
					}
					else
					{
						/* new frame found? */
						printf("USB device with product id %d was not found in database.\n", dev->descriptor.idProduct);
					}
				}
			}
		}
	}

	return -1;
}

int switch_usb_mode(struct usb_device* dev)
{
	usb_dev_handle *udev;
	char buf[256];
	int ret;


	/* get usb device handle */
	udev = usb_open(dev);

	/* try to read manufacturer */
	if (usb_get_string_simple(udev, dev->descriptor.iManufacturer, buf, sizeof(buf)) == -1)
	{
		printf("Error while reading from USB device. Please check your system rights.\n");
		return -1;
	}

	/* send control msg and switch to photo mode */
	if ((ret = usb_control_msg(udev, USB_TYPE_STANDARD | USB_ENDPOINT_IN,
								USB_REQ_GET_DESCRIPTOR,
								0xfe, 0xfe, 0x0, 0xfe, 1000)) < 0)
	{
		/* usb_control_msg returns -108 in my case, however things seem to work anyway */
	}

	usb_close(udev);

	return 0;
}


int send_jpeg(usb_dev_handle *dev, FILE *file, char *data, int len)
{
	char usb_hdr[USB_HDR_LEN] = {0xa5, 0x5a, 0x18, 0x04, 0xff, 0xff, 0xff, 0xff, 0x48, 0x00, 0x00, 0x00};
	char buffer[URBBUF_MAX];
	int usb_timeout = 1000;
	int usb_endpoint = 0x2;
	int filesize, offset;
	int ret;


	/* sanity check */
	if (file && data)
	{
		printf("Please specify either file or data input.\n");
		return -1;
	}

	if (file)
	{
		// get file size
		fseek(file, 0, SEEK_END);
		filesize = ftell(file);
		fseek(file, 0, SEEK_SET);

		// insert filesize into command
		*(int *)(usb_hdr + 4) = filesize;
	}
	else
	{
		*(int *)(usb_hdr + 4) = len;
	}

	// copy header into usb buffer
	memcpy(buffer, usb_hdr, USB_HDR_LEN);
	offset = USB_HDR_LEN;

	if (file)
	{
		while(!feof(file))
		{
			// read file into buffer
			if ((ret = fread(buffer + offset, 1, URBBUF_MAX - offset, file)) < 1)
			{
				printf("Error while reading file, fread returned: %d\n", ret);
				break;
			}

		    // pad bytes
		    memset(buffer + offset + ret, 0, URBBUF_MAX - offset - ret);

			// send buffer to picture frame
			if ((ret = usb_bulk_write(dev, usb_endpoint, buffer, URBBUF_MAX, usb_timeout)) < 0)
			{
				// error occurred
				printf("Error while writing to USB device. Please check your system rights.\n");
				//printf("usb_bulk_write returned: %d\n", ret);
			}

			// no header needed on subsequent chunks
			offset = 0;
		}

		/* rewind file */
		fseek(file, 0, SEEK_SET);
	}
	else
	{
		/* FIXME: that needs to be tested */
		int bytes_sent = 0;
		int bytes_to_send = 0;

		printf("total len is %d\n", len);

		while (bytes_sent < len)
		{
			// copy chunks into usb_buffer
			bytes_to_send = min(URBBUF_MAX - offset, len);
			printf("bytes_to_send: %d, offset: %d\n", bytes_to_send, offset);

			// pad bytes
			memset(buffer + offset, 0, URBBUF_MAX - offset);
			memcpy(buffer + offset, data + bytes_sent, bytes_to_send);
			bytes_sent += bytes_to_send; // these counters are just for the data packets

			printf("bytes_sent: %d, len: %d\n", bytes_sent, len);

			// send chunk
			if ((ret = usb_bulk_write(dev, usb_endpoint, buffer, URBBUF_MAX, usb_timeout)) < 0)
			{
				// error occurred
				printf("Error occurred while writing data to device.\n");
				printf("usb_bulk_write returned: %d\n", ret);
			}

			printf("usb_bulk_write returned: %d\n", ret);

			// no header needed on subsequent chunks
			offset = 0;
		}
	}

	return 0;
}


int main(int argc, char *argv[])
{
	struct usb_device *dev = NULL;
	FILE *file_handle;
	char *filename = NULL;
	char *fb_device = NULL;
	struct timespec t;
	long fb_refresh_interval = 1000; /* 1000ms*/
	char *tmp_filename;
	int ret = -1;
	int retry = 0;
	int opt;
	enum opmode_t opmode = ONCE;
	char buf;


	if (argc == 1)
	{
		show_help();
		return -1;
	}

	while((opt = getopt(argc, argv, "erhi:j:f:")) != -1)
	{
		switch (opt)
		{
			case 'h':
				show_help();
				return 0;
				break;
			case 'j':
				filename = optarg;
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
	if (!filename && !fb_device)
	{
		printf("both specified\n");
		show_help();
		return -1;
	}

	if (filename && fb_device)
	{
		printf("Choose either to read from file or from a frame buffer device.\n");
		return -1;
	}

	if (fb_refresh_interval < 100)
		printf("WARNING: refresh rates smaller then 100msecs may cause heavy system load\n");

	/* convert to nanoseconds */
	fb_refresh_interval *= USEC_PER_SEC;

	/* install signal handler */
	signal(SIGINT, sigint);

	/* start */
	while ((ret = find_photo_frame(&dev)) != 0)
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

	if ((dev_handle = usb_open(dev)) == NULL)
	{
		printf("Error while creating usb device handle.\n");
		return -1;
	}

	if (filename)
	{
		printf("displaying %s on photo frame ..\n", filename);

		if ((file_handle = fopen(filename, "r+")) == NULL)
		{
			printf("File %s was not found.\n", filename);
			ret = -1;
			goto exit;
		}

		if ((ret = send_jpeg(dev_handle, file_handle, NULL, 0)) != 0)
		{
			printf("Error occurred while sending jpeg image to device.\n");
		}

		fclose(file_handle);
	}
	else if (fb_device)
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
		if (!(tmp_filename = tmpnam(NULL)))
		{
			printf("error while creating temporary file name.\n");
			return -1;
		}

		if (!(tmpfile_handle = fopen(tmp_filename, "w+")))
		{
			printf("error while opening temporary file.\n");
			return -1;
		}

		/* do work depending on operation mode */
		if (opmode == ONCE)
		{
			printf("Writing content of FB device %s to picture frame ..\n", fb_device);

			/* just read from fb once and exit */
			grab_from_fb(fb_device, tmpfile_handle);

			/* rewind file, so send_jpeg can read from it straight away */
			fseek(tmpfile_handle, 0, SEEK_SET);

			/* send image to frame */
			if ((ret = send_jpeg(dev_handle, tmpfile_handle, NULL, 0)) != 0)
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
				grab_from_fb(fb_device, tmpfile_handle);

				/* rewind file, so send_jpeg can read from it straight away */
				fseek(tmpfile_handle, 0, SEEK_SET);

				/* send image to frame */
				if ((ret = send_jpeg(dev_handle, tmpfile_handle, NULL, 0)) != 0)
				{
					printf("Error occurred while sending jpeg image to device.\n");
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
				grab_from_fb(fb_device, tmpfile_handle);

				/* rewind file, so send_jpeg can read from it straight away */
				fseek(tmpfile_handle, 0, SEEK_SET);

				/* send image to frame */
				if ((ret = send_jpeg(dev_handle, tmpfile_handle, NULL, 0)) != 0)
				{
					printf("Error occurred while sending jpeg image to device.\n");
				}

			}

			printf("fread ret: %d\n", ret);

			/* in case we reach this point, cleanup */
			fclose(sysfs_handle);
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
