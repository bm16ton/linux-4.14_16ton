/*
 * Driver for the Winchiphead CH341A USB to I2C chip
 *
 * Datasheet: http://www.winchiphead.com/download/CH341/CH341DS1.PDF
 *
 * Copyright (C) 2016 Angelo Compagnucci <angelo.compagnucci@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 *
 */

#define DEBUG 1

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/usb.h>
#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/platform_device.h>

#define TIMEOUT 500

#define CH341A_CONTROL_I2C        0xAA

#define CH341A_I2C_CMD_STA        0x74
#define CH341A_I2C_CMD_STO        0x75
#define CH341A_I2C_CMD_OUT        0x80
#define CH341A_I2C_CMD_IN         0xC0
#define CH341A_I2C_CMD_MAX_LENGTH 32
#define CH341A_I2C_CMD_SET        0x60
#define CH341A_I2C_CMD_US         0x40
#define CH341A_I2C_CMD_MS         0x50
#define CH341A_I2C_CMD_DLY        0x0f
#define CH341A_I2C_CMD_END        0x00

#define SEND_PAYLOAD_LENGTH       29
#define RECV_PAYLOAD_LENGTH       32

struct i2c_ch341a {
	u8 in_buf[CH341A_I2C_CMD_MAX_LENGTH];
	u8 out_buf[CH341A_I2C_CMD_MAX_LENGTH];
	struct usb_device *usb_dev;
	struct usb_interface *interface;
	struct i2c_adapter adapter;
	unsigned int clock_speed;
	int ep_in, ep_out;
};

static int ch341a_usb_i2c_command(struct i2c_adapter *adapter, u8 *cmd, u8 len)
{
	struct i2c_ch341a *dev = (struct i2c_ch341a *)adapter->algo_data;
	int sent, ret;

	ret = usb_bulk_msg(dev->usb_dev,
		usb_sndbulkpipe(dev->usb_dev, dev->ep_out),
		cmd, len, &sent, TIMEOUT);
	if (ret != 0) return ret;

	return 0;
}

static int ch341a_usb_i2c_start(struct i2c_adapter *adapter)
{
	u8 I2C_CMD_START[] = {CH341A_CONTROL_I2C, CH341A_I2C_CMD_STA, CH341A_I2C_CMD_END};
	print_hex_dump_bytes(__func__, DUMP_PREFIX_OFFSET, I2C_CMD_START, sizeof(I2C_CMD_START));
	return ch341a_usb_i2c_command(adapter, I2C_CMD_START, sizeof(I2C_CMD_START));
}

static int ch341a_usb_i2c_stop(struct i2c_adapter *adapter)
{
	u8 I2C_CMD_STOP[] = {CH341A_CONTROL_I2C, CH341A_I2C_CMD_STO, CH341A_I2C_CMD_END};
	print_hex_dump_bytes(__func__, DUMP_PREFIX_OFFSET, I2C_CMD_STOP, sizeof(I2C_CMD_STOP));
	return ch341a_usb_i2c_command(adapter, I2C_CMD_STOP, sizeof(I2C_CMD_STOP));
}

static int ch341a_usb_i2c_delay(struct i2c_adapter *adapter)
{
	u8 I2C_CMD_DELAY[] = {CH341A_CONTROL_I2C, CH341A_I2C_CMD_MS, CH341A_I2C_CMD_END};
	print_hex_dump_bytes(__func__, DUMP_PREFIX_OFFSET, I2C_CMD_DELAY, sizeof(I2C_CMD_DELAY));
	return ch341a_usb_i2c_command(adapter, I2C_CMD_DELAY, sizeof(I2C_CMD_DELAY));
}

static int ch341a_usb_i2c_set_speed(struct i2c_adapter *adapter, unsigned int data)
{
	struct i2c_ch341a *ch341a_data = (struct i2c_ch341a *)adapter->algo_data;
	u8 I2C_CMD_SET[] = {CH341A_CONTROL_I2C, CH341A_I2C_CMD_SET , CH341A_I2C_CMD_END};
	u8 speed;

	switch (data) {
		case 100:
			speed = 0;
			break;
		case 400:
			speed = 1;
			break;
		case 750:
			speed = 2;
			break;
		case 1000:
			speed = 3;
			break;
		default:
			return -EINVAL;
	}

	ch341a_data->clock_speed = data;

	I2C_CMD_SET[1] |= speed;

	print_hex_dump_bytes(__func__, DUMP_PREFIX_OFFSET, I2C_CMD_SET, sizeof(I2C_CMD_SET));
	return ch341a_usb_i2c_command(adapter, I2C_CMD_SET, sizeof(I2C_CMD_SET));
}

static int ch341a_usb_i2c_write(struct i2c_adapter *adapter, u8 len, u8 *data)
{
	struct i2c_ch341a *dev = (struct i2c_ch341a *)adapter->algo_data;
	int ret, actual;

	dev->out_buf[0] = CH341A_CONTROL_I2C;
	dev->out_buf[1] = CH341A_I2C_CMD_OUT | len;
	memcpy(&dev->out_buf[2], data, len);
	dev->out_buf[2+len] = CH341A_I2C_CMD_END;

	print_hex_dump_bytes(__func__, DUMP_PREFIX_OFFSET, dev->out_buf, len+3);

	ret = usb_bulk_msg(dev->usb_dev,
		usb_sndbulkpipe(dev->usb_dev, dev->ep_out),
		dev->out_buf, len+3, &actual, TIMEOUT);
	if (ret != 0) return ret;

	return 0;
}

static int ch341a_usb_write_bytes(struct i2c_adapter *adapter, u8 addr, u16 len, u8 *data)
{
	int i = 0, ret;

	ret = ch341a_usb_i2c_start(adapter);
	if (ret != 0) return ret;

	ret = ch341a_usb_i2c_write(adapter, 1, &addr);
	if (ret != 0) return ret;

	for (i = 0; i < len; i += SEND_PAYLOAD_LENGTH) {
		ret = ch341a_usb_i2c_write(adapter,
			len-i >= SEND_PAYLOAD_LENGTH ? SEND_PAYLOAD_LENGTH : len-i,
			&data[i]);
		if (ret != 0) return ret;
		if (i + SEND_PAYLOAD_LENGTH < len) {
			ret = ch341a_usb_i2c_delay(adapter);
			if (ret != 0) return ret;
		}
	}

	ret = ch341a_usb_i2c_stop(adapter);
	if (ret != 0) return ret;
	ret = ch341a_usb_i2c_delay(adapter);
	return ret;
}

static int ch341a_usb_i2c_read(struct i2c_adapter *adapter, u8 len, u8 *data)
{
	struct i2c_ch341a *dev = (struct i2c_ch341a *)adapter->algo_data;
	u8 I2C_CMD_READ_BYTE[] = {CH341A_CONTROL_I2C, 0, CH341A_I2C_CMD_END};
	int ret, actual;

	I2C_CMD_READ_BYTE[1] = CH341A_I2C_CMD_IN | (u8) len;

	ret = usb_bulk_msg(dev->usb_dev,
			usb_sndbulkpipe(dev->usb_dev, dev->ep_out),
			I2C_CMD_READ_BYTE, sizeof(I2C_CMD_READ_BYTE), &actual, TIMEOUT);
	if (ret != 0) return ret;

	ret = usb_bulk_msg(dev->usb_dev,
			usb_rcvbulkpipe(dev->usb_dev, dev->ep_in),
			dev->in_buf, RECV_PAYLOAD_LENGTH, &actual, TIMEOUT);
	if (ret != 0) return ret;

	memcpy(data, dev->in_buf, len);

	return 0;
}

static int ch341a_usb_i2c_read_bytes(struct i2c_adapter *adapter, u8 addr, u16 len, u8 *data)
{
	int i = 0, ret;

	ret = ch341a_usb_i2c_start(adapter);
	if (ret != 0) return ret;
	ret = ch341a_usb_i2c_write(adapter, 1, &addr);
	if (ret != 0) return ret;

	for (i = 0; i < len; i += RECV_PAYLOAD_LENGTH) {
		ret = ch341a_usb_i2c_read(adapter,
			len-i >= RECV_PAYLOAD_LENGTH ? RECV_PAYLOAD_LENGTH : len-i,
			&data[i]);
		if (ret != 0) return ret;
	}

	print_hex_dump_bytes(__func__, DUMP_PREFIX_OFFSET, data, len);

	ret = ch341a_usb_i2c_stop(adapter);
	if (ret != 0) return ret;
	ch341a_usb_i2c_write(adapter, 1, &addr); //should give error so don't check here

	return 0;
}

static int ch341a_usb_xfer(struct i2c_adapter *adapter, struct i2c_msg *msgs, int num)
{
	struct i2c_msg *pmsg;
	int i;

	for (i = 0; i < num; i++) {
		pmsg = &msgs[i];
		dev_dbg(&adapter->dev, "%s: (addr=%x, flags=%x, len=%d)", __func__,
			pmsg->addr, pmsg->flags, pmsg->len);
		if (pmsg->flags & I2C_M_RD) {
			if (ch341a_usb_i2c_read_bytes(adapter,
				(pmsg->addr<<1) + 1,
				pmsg->len, pmsg->buf) != 0)
					return -EREMOTEIO;
		} else {
			if (ch341a_usb_write_bytes(adapter,
				(pmsg->addr<<1), pmsg->len, pmsg->buf) != 0)
					return -EREMOTEIO;
		}
	}
	return i;
}

static u32 ch341a_usb_func(struct i2c_adapter *adapter)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}
static ssize_t ch341a_attr_clock_store_value(struct device *dev,
			struct device_attribute *attr,
			const char *buf, size_t count) {

	struct i2c_adapter *adapter = to_i2c_adapter(dev);
	unsigned int value;
	int ret;

	ret = kstrtouint(buf, 10, &value);

	ret = ch341a_usb_i2c_set_speed(adapter, value);
	if (ret != 0) return -EINVAL;

	return count;
}

static ssize_t ch341a_attr_clock_show_value(struct device *dev,
				struct device_attribute *attr,
				char *buf) {
	struct i2c_adapter *adapter = to_i2c_adapter(dev);
	struct i2c_ch341a *ch341a_data = (struct i2c_ch341a *)adapter->algo_data;
	return sprintf(buf, "%d\n", (ch341a_data->clock_speed));
}

static ssize_t ch341a_attr_clock_show_freqs(struct device *dev,
				struct device_attribute *attr,
				char *buf) {
	return sprintf(buf, "100 400 750 1000\n");
}

static DEVICE_ATTR(clock_frequency, S_IWUSR | S_IRUGO,
			ch341a_attr_clock_show_value,
			ch341a_attr_clock_store_value);
static DEVICE_ATTR(clock_frequency_available, S_IRUGO,
		ch341a_attr_clock_show_freqs, NULL);

static struct attribute *ch341a_attrs[] = {
	&dev_attr_clock_frequency.attr,
	&dev_attr_clock_frequency_available.attr,
	NULL
};
ATTRIBUTE_GROUPS(ch341a);

static const struct i2c_algorithm usb_algorithm = {
	.master_xfer	= ch341a_usb_xfer,
	.functionality	= ch341a_usb_func,
};

static const struct usb_device_id ch341a_table[] = {
	{ USB_DEVICE(0x1a86, 0x5512) },
	{ }
};

MODULE_DEVICE_TABLE(usb, ch341a_table);

static void ch341a_free(struct i2c_ch341a *dev)
{
	usb_put_dev(dev->usb_dev);
	kfree(dev);
}

static int ch341a_probe(struct usb_interface *interface,
			const struct usb_device_id *id)
{
	struct usb_host_interface *hostif = interface->cur_altsetting;
	struct i2c_ch341a *dev;
	int ret = -ENOMEM;

	dev_dbg(&interface->dev, "probing usb device\n");

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL) {
		dev_err(&interface->dev, "Out of memory\n");
		goto error;
	}

	dev->usb_dev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	dev->ep_out = hostif->endpoint[1].desc.bEndpointAddress;
	dev->ep_in = hostif->endpoint[0].desc.bEndpointAddress;

	usb_set_intfdata(interface, dev);

	dev->adapter.owner = THIS_MODULE;
	dev->adapter.class = I2C_CLASS_HWMON;
	dev->adapter.algo = &usb_algorithm;
	dev->adapter.algo_data = dev;
	dev->adapter.dev.groups = ch341a_groups;
	i2c_set_adapdata(&dev->adapter, dev);
	snprintf(dev->adapter.name, sizeof(dev->adapter.name),
		 "i2c-ch341a at bus %03d device %03d",
		 dev->usb_dev->bus->busnum, dev->usb_dev->devnum);

	dev->adapter.dev.parent = &dev->interface->dev;

	ret = i2c_add_adapter(&dev->adapter);
	if (ret < 0) {
		goto error;
	}

	ret = ch341a_usb_i2c_set_speed(&dev->adapter, 100);
	if (ret != 0) return ret;

	dev_info(&dev->adapter.dev, "connected i2c-ch341a device\n");

	return 0;

 error:
	if (dev)
		usb_set_intfdata(interface, NULL);
		ch341a_free(dev);

	return ret;
}

static void ch341a_disconnect(struct usb_interface *interface)
{
	struct i2c_ch341a *dev = usb_get_intfdata(interface);

	i2c_del_adapter(&dev->adapter);
	usb_set_intfdata(interface, NULL);
	ch341a_free(dev);

	dev_dbg(&interface->dev, "disconnected\n");
}

#ifdef CONFIG_OF
static const struct of_device_id ch341a_of_match[] = {
	{ .compatible = "ch341a" },
	{ }
};
MODULE_DEVICE_TABLE(of, ch341a_of_match);
#endif

static struct usb_driver ch341a_driver = {
	.name		= "i2c-ch341a",
	.probe		= ch341a_probe,
	.disconnect	= ch341a_disconnect,
	.id_table	= ch341a_table,
};

module_usb_driver(ch341a_driver);

MODULE_AUTHOR("Angelo Compagnucci <angelo.compagnucci@gmail.com>");
MODULE_DESCRIPTION("i2c-ch341a driver");
MODULE_LICENSE("GPL v2");
