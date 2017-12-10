/** @file dlo_usb.c
 *
 *  @brief Implements the USB-specific connectivity functions.
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

#include <string.h>
#include "netinet/in.h"
#include "dlo_defs.h"
#include "dlo_usb.h"
#include "dlo_base.h"
#include "dlo_mode.h"


/* File-scope defines ------------------------------------------------------------------*/


#define NR_USB_REQUEST_STATUS_DW  (0x06)  /**< USB control message: request type. */
#define NR_USB_REQUEST_CHANNEL    (0x12)  /**< USB control message: request type. */
#define NR_USB_REQUEST_I2C_SUB_IO (0x02)  /**< USB control message: request type. */

/** USB VendorID for a DisplayLink device.
 */
#define VENDORID_DISPLAYLINK (0x17E9)

/** Number of milliseconds to wait before timing-out a USB control message.
 */
#define CTRL_TIMEOUT (100u)

/** Number of milliseconds to wait before timing-out channel selection message.
 */
#define CHANSEL_TIMEOUT (5000u)

/** Number of milliseconds to wait before timing-out a USB bulk transfer.
 */
#define WRITE_TIMEOUT (10000u)

/** Number of milliseconds to wait before timing-out a request for the device type.
 */
#define ID_TIMEOUT (1000u)

/** Byte sequence to send to the device to select the default communication channel.
 */
#define STD_CHANNEL "\x57\xCD\xDC\xA7\x1C\x88\x5E\x15\x60\xFE\xC6\x97\x16\x3D\x47\xF2"


/* File-scope types --------------------------------------------------------------------*/


/* External scope variables ------------------------------------------------------------*/


int32_t usberr = 0;


/* File-scope variables ----------------------------------------------------------------*/


/** Pointer to a copy of the last error message string read out of libusb (or NULL).
 */
static char *usb_err_str = NULL;


/* File-scope function declarations ----------------------------------------------------*/


/** Connect to a device, read information about it and then disconnect again.
 *
 *  @param  dev  USB device structure pointer.
 *
 *  @return  Return code, zero for no error.
 */
static dlo_retcode_t check_device(struct usb_device *dev);


/** Attempt to read the EDID structure from the monitor attached to the specified device.
 *
 *  @param  dev    Device structure pointer.
 *  @param  uhand  USB device handle.
 *
 *  @return  Return code, zero for no error.
 */
static dlo_retcode_t read_edid(dlo_device_t * const dev, usb_dev_handle *uhand);


/** Make a note of any error returned by libusb.
 *
 *  @return  Return code to indicate a USB-related error.
 *
 *  Often, by the time we have returned from a function, the error code stored in libusb
 *  has been lost because our tidy-up code will call more libusb functions after an error
 *  is spotted. As such, this function is called automatically by the @c UERR() and
 *  @c UERR_GOTO() macros in order to make a note of the error number and message from
 *  libusb at the moment the error is caught. This information is stored locally for
 *  retrieval by @c dlo_usb_strerror().
 */
static dlo_retcode_t usb_error_grab(void);


/* Public function definitions ---------------------------------------------------------*/


char *dlo_usb_strerror(void)
{
  //DPRINTF("usb: error lookup %d\n", usberr);
  return usb_err_str;
}


dlo_retcode_t dlo_usb_init(const dlo_init_t flags)
{
  /* Initialise libusb */
  //DPRINTF("usb: init\n");
  usb_init();

  /* Add nodes onto the device list for any DisplayLink devices we find */
  //DPRINTF("usb: init: enum\n");
  ERR(dlo_usb_enumerate(true));

  return dlo_ok;
}


dlo_retcode_t dlo_usb_final(const dlo_final_t flags)
{
  if (usb_err_str)
    dlo_free(usb_err_str);

  return dlo_ok;
}

dlo_retcode_t dlo_usb_enumerate(const bool init)
{
  struct usb_bus    *bus;
  struct usb_device *dev = NULL;
  int32_t            db  = usb_find_busses();
  int32_t            dd  = usb_find_devices();
  DPRINTF("usb: dlo_usb_enumerate\n");

  /* We should do this even if it looks like there are no changes on the bus because
   * an open() call might call the above functions just to check it's safe to use the
   * USB device structure pointer. Yuck!
   *
   * If this isn't the first enumeration and there are no changes on the bus, don't touch the list
   *
   * if (!init && !db && !dd)
   *   return dlo_ok;
   */
  IGNORE(db);
  IGNORE(dd);
  
  /* Look for all DisplayLink devices on the USB busses */
  for (bus = usb_get_busses(); bus; bus = bus->next) 
    for (dev = bus->devices; dev; dev = dev->next) 
      ERR(check_device(dev)); /* Check to see if it's a DisplayLink device. If it is, add to or update the dev_list */
  
  return dlo_ok;
}


static dlo_retcode_t check_device(struct usb_device *udev)
{
  static char     string[255];
  usb_dev_handle *uhand    = usb_open(udev);
  dlo_retcode_t   err      = dlo_ok;
  dlo_device_t   *dev      = NULL;
  bool            not_root = false;
  uint8_t         buf[4];
  dlo_devtype_t   type;

  //DPRINTF("usb: check: check dev &%X\n", (int)dev);

  if (!uhand) {
    // this may not be our device. We just can't open it
    return dlo_ok;
  }
  
  //DPRINTF("usb: check: uhand &%X vendorID &%X\n", (int)uhand, udev->descriptor.idVendor);

  /* Reject devices that don't have the DisplayLink VendorID */
  if (udev->descriptor.idVendor != VENDORID_DISPLAYLINK)
  {
    UERR(usb_close(uhand));
    return dlo_ok;
  }
  //DPRINTF("usb: check: get type\n");

  /* Ask the device for some status information */
  not_root = true; /* Special case error handling here */
  UERR_GOTO(usb_control_msg(/* handle */      uhand,
                            /* requestType */ USB_ENDPOINT_IN | USB_TYPE_VENDOR,
                            /* request */     NR_USB_REQUEST_STATUS_DW,
                            /* value */       0,
                            /* index */       0,
                            /* bytes */       (char *)buf,
                            /* size */        sizeof(buf),
                            /* timeout */     ID_TIMEOUT));
  not_root = false; /* Back to normal error handling */
  //DPRINTF("usb: check: type buf[3] = &%X\n", buf[3]);

  /* Determine what type of device we are connected to */
  switch ((buf[3] >> 4) & 0xF)
  {
    case dlo_dev_base:
      type = dlo_dev_base;
      break;
    case dlo_dev_alex:
      type = dlo_dev_alex;
      break;
    default:
      if (buf[3] == dlo_dev_ollie)
        type = dlo_dev_ollie;
      else
        type = dlo_dev_unknown;
  }

  /* Read the device serial number as a string */
  UERR_GOTO(usb_get_string_simple(uhand, udev->descriptor.iSerialNumber, string, sizeof(string)));
  //DPRINTF("usb: check: type &%X serial '%s'\n", (int)type, string);

  /* See if this device is already in our device list */
  dev = dlo_device_lookup(string);
  if (dev)
  {
    /* Use this opportunity to update the USB device structure pointer, just in
     * case it has moved.
     */
    dev->cnct->udev = udev;
    //DPRINTF("usb: check: already in list\n");
  }
  else
  {
    /* Add a new device to the device list */
    //DPRINTF("usb: check: create new device\n");
    dev = dlo_new_device(type, string);
    NERR_GOTO(dev);

    /* It's not. Create and initialise a new list node for the device */
    dev->cnct = (dlo_usb_dev_t *)dlo_malloc(sizeof(dlo_usb_dev_t));
    NERR_GOTO(dev->cnct);
    dev->cnct->udev = udev;
    dev->cnct->uhand = NULL;
  }
  //DPRINTF("usb: check: dlpp node &%X\n", (int)dev);

  /* Close our temporary handle for the device. If this errors, we'll have a duff entry in
   * the device list but at least the list integrity will be OK.
   */
  UERR_GOTO(usb_close(uhand));

  return dlo_ok;

error:
  /* Free our dev->cnct USB information structure */
  if (dev)
  {
    if (dev->cnct)
      dlo_free(dev->cnct);
    dev->cnct = NULL;
  }

  /* Close our temporary handle for the device */
  (void) usb_close(uhand);

  /* If the executable wasn't run as root, this is where it normally falls over.
   * So we'll special case that particular error to help indicate this problem.
   */
  if (not_root)
    return dlo_err_not_root;

  return err;
}


dlo_retcode_t dlo_usb_open(dlo_device_t * const dev)
{
  dlo_retcode_t   err;
  usb_dev_handle *uhand;
  char*		  driver_name;
  int             usb_configuration;
  int             i;
  int32_t         db = usb_find_busses();
  int32_t         dd = usb_find_devices();

  /* Do we trust the USB device pointer? Not if the structures may have changed under us... */
  if (db || dd)
    return dlo_err_reenum;

  /* Open the device */
  uhand = usb_open(dev->cnct->udev);
  DPRINTF("usb: open: uhand &%X\n", (int)uhand);

  if (!uhand)
    return dlo_err_open;

  /* Store the USB device handle in our dev->cnct word */
  dev->cnct->uhand = uhand;

  /* Establish the connection with the device */
  //DPRINTF("usb: open: setting config...\n");

  /*
   * Because some displaylink devices may report 
   * a class code (like HID or MSC) that gets
   * matched by a kernel driver, we must detach
   * those drivers before libusb can successfully
   * set configuration or talk to those devices.
   * For now, we kick everyone off our device, 
   * but that includes cases we're intentionally
   * a composite device with HID interfaces that
   * control something (e.g. a button on a dock).
   * And the code below is blindly unloading
   * the kernel HID drivers for those.  May want
   * to get more sophisticated in the future.
   */
  driver_name = dlo_malloc(128);
  for (i=0; i < dev->cnct->udev->config->bNumInterfaces; i++) 
  {
    memset(driver_name, 0, 128);
    if (usb_get_driver_np(uhand, i, driver_name, 128) == 0)
    {
      DPRINTF("usb: driver (%s) already attached to device\n", driver_name);

      // Reports are that this call can return error even if successful
      usb_detach_kernel_driver_np(uhand,i);
    }
  }
   
  UERR(usb_set_configuration(uhand, 1));

  //DPRINTF("usb: open: claiming iface...\n");
  UERR(usb_claim_interface(uhand, 0));

  /* Mark the device as claimed */
  dev->claimed = true;

  /* Allocate a buffer to hold commands before they are sent to the device */
  if (!dev->buffer)
  {
    //DPRINTF("usb: open: alloc buffer...\n");
    dev->buffer = dlo_malloc(BUF_SIZE);
    NERR(dev->buffer);
    dev->bufptr = dev->buffer;
    dev->bufend = dev->buffer + BUF_SIZE;
  }
  //DPRINTF("usb: open: buffer &%X, &%X, &%X\n", (int)dev->buffer, (int)dev->bufptr, (int)dev->bufend);

  /* Use the default timeout if none was specified */
  if (!dev->timeout)
    dev->timeout = WRITE_TIMEOUT;
  //DPRINTF("usb: open: timeout %u ms\n", dev->timeout);

  /* Initialise the supported modes array for this device to include all our pre-defined modes */
  use_default_modes(dev);

  /* Attempt to read the EDID information, to refine the supported modes array contents */
  err = read_edid(dev, uhand);
#ifdef DEBUG
  if (err != dlo_ok)
    DPRINTF("usb: open: edid error %u '%s'\n", (int)err, dlo_strerror(err));
#endif

  return dlo_ok;
}


dlo_retcode_t dlo_usb_close(dlo_device_t * const dev)
{
  if (dev->claimed)
  {
    if (dev->buffer)
    {
      dlo_free(dev->buffer);
      dev->buffer = NULL;
      dev->bufptr = NULL;
      dev->bufend = NULL;
    }
    dev->claimed = false;
    UERR(usb_release_interface(dev->cnct->uhand, 0));
    UERR(usb_close(dev->cnct->uhand));
  }
  return dlo_ok;
}


dlo_retcode_t dlo_usb_chan_sel(const dlo_device_t * const dev, const char * const buf, const size_t size)
{
  if (size)
    UERR(usb_control_msg(/* handle */      dev->cnct->uhand,
                         /* requestType */ USB_TYPE_VENDOR,
                         /* request */     NR_USB_REQUEST_CHANNEL,
                         /* value */       0,
                         /* index */       0,
                         /* bytes */       (char *)buf,
                         /* size */        size,
                         /* timeout */     CHANSEL_TIMEOUT));
  return dlo_ok;
}


dlo_retcode_t dlo_usb_std_chan(const dlo_device_t * const dev)
{
  dlo_retcode_t err;

  ASSERT(strlen(STD_CHANNEL) == 16);
  err = dlo_usb_chan_sel(dev, STD_CHANNEL, DSIZEOF(STD_CHANNEL));

  return err;
}


dlo_retcode_t dlo_usb_write(dlo_device_t * const dev)
{
  dlo_retcode_t err = dlo_usb_write_buf(dev, dev->buffer, dev->bufptr - dev->buffer);

  dev->bufptr = dev->buffer;

  return err;
}


dlo_retcode_t dlo_usb_write_buf(dlo_device_t * const dev, char * buf, size_t size)
{
#ifdef DEBUG_DUMP
  static char     outfile[64];
  static uint32_t outnum = 0;
  FILE           *out    = NULL;
#endif

  if (!dev->claimed)
    return dlo_err_unclaimed;

  if (!size)
    return dlo_ok;

#ifdef WRITE_BUF_BODGE
  /* If the buffer to write is fewer than 513 bytes in size, copy into 513 byte buffer and pad with zeros */
  if (size < WRITE_BUF_BODGE)
  {
    dlo_retcode_t err;
    uint32_t       rem = WRITE_BUF_BODGE - size;
    char          *cpy = dlo_malloc(WRITE_BUF_BODGE);

    NERR(cpy);
    dlo_memcpy(cpy, buf, size);
    dlo_memset(cpy + size, 0, rem);
    err = dlo_usb_write_buf(dev, cpy, size + rem);
    dlo_free(cpy);

    return err;
  }
#endif

  while (size)
  {
    size_t num = size > BUF_SIZE ? BUF_SIZE : size;

#ifdef DEBUG_DUMP
    (void) snprintf(outfile, sizeof(outfile), "dump/%02X/bulk%03X.dat", outnum & 0xFF, outnum >> 8);
    outnum++;
    out = fopen(outfile, "wb");
    if (out)
    {
      (void) fwrite(buf, num, 1, out);
      (void) fclose(out);
      out = NULL;
    }
#endif

    UERR(usb_bulk_write(/* handle */   dev->cnct->uhand,
                        /* endpoint */ 1,
                        /* bytes */    buf,
                        /* size */     num,
                        /* timeout */  dev->timeout));
    buf  += num;
    size -= num;
  }
  return dlo_ok;
}


/* File-scope function definitions -----------------------------------------------------*/


static dlo_retcode_t usb_error_grab(void)
{
  char *str = usb_strerror();

  /* If we have a previous USB error message stored, free it */
  if (usb_err_str)
    dlo_free(usb_err_str);

  if (str)
  {
    /* Allocate memory for the new error message and store that */
    usb_err_str = dlo_malloc(1 + strlen(str));
    if (usb_err_str)
      strcpy(usb_err_str, str);
  }

  /* Always return the generic USB error code */
  return dlo_err_usb;
}



static dlo_retcode_t read_edid(dlo_device_t * const dev, usb_dev_handle *uhand)
{
  dlo_retcode_t err;
  uint32_t      i;
  uint8_t       buf[2];
  uint8_t      *edid;

  /* Allocate a buffer to hold the EDID structure */
  edid = dlo_malloc(EDID_STRUCT_SZ);
  NERR(edid);

  /* Attempt to read the EDID structure from the device */
  for (i = 0; i < EDID_STRUCT_SZ; i++)
  {
    UERR_GOTO(usb_control_msg(/* handle */      uhand,
                              /* requestType */ USB_ENDPOINT_IN | USB_TYPE_VENDOR,
                              /* request */     NR_USB_REQUEST_I2C_SUB_IO,
                              /* value */       i << 8,
                              /* index */       0xA1,
                              /* bytes */       (char *)buf,
                              /* size */        sizeof(buf),
                              /* timeout */     dev->timeout));
    if (buf[0])
      ERR_GOTO(dlo_err_iic_op);
    //DPRINTF("usb: edid[%u]=&%02X\n", i, buf[1]);
    edid[i] = buf[1];
  }

  /* Supply the prospective EDID structure to the parser */
  ERR_GOTO(dlo_mode_parse_edid(dev, edid, EDID_STRUCT_SZ));

error:
  dlo_free(edid);

  return err;
}


/* End of file -------------------------------------------------------------------------*/
