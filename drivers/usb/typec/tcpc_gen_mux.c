/*
 * Generic TCPC mux driver using the mux subsys
 *
 * Copyright (c) 2017 Hans de Goede <hdegoede@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation, or (at your option)
 * any later version.
 */

#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mux/consumer.h>
#include <linux/mux/usb.h>
#include <linux/usb/tcpm.h>

struct tcpc_gen_mux_data {
	struct tcpc_mux_dev mux;
	struct device *dev;
	struct mux_control *type_c_mode_mux; /* Type-C cross switch / mux */
	struct mux_control *usb_role_mux;    /* USB Device / Host mode mux */
	bool muxes_set;
};

static int tcpc_gen_mux_set(struct tcpc_mux_dev *mux_dev,
			    enum tcpc_mux_mode mux_mode,
			    enum tcpc_usb_switch usb_config,
			    enum typec_cc_polarity polarity)
{
	struct tcpc_gen_mux_data *data =
		container_of(mux_dev, struct tcpc_gen_mux_data, mux);
	unsigned int typec_state = MUX_TYPEC_USB;
	unsigned int usb_state = MUX_USB_DEVICE;
	int ret;

	/* Put the muxes back in their open (idle) state */
	if (data->muxes_set) {
		mux_control_deselect(data->type_c_mode_mux);
		mux_control_deselect(data->usb_role_mux);
		data->muxes_set = false;
	}

	switch (mux_mode) {
	case TYPEC_MUX_NONE:
		/* Muxes are in their open state, done. */
		return 0;
	case TYPEC_MUX_USB_DEVICE:
		typec_state = MUX_TYPEC_USB;
		usb_state = MUX_USB_DEVICE;
		break;
	case TYPEC_MUX_USB_HOST:
		typec_state = MUX_TYPEC_USB;
		usb_state = MUX_USB_HOST;
		break;
	case TYPEC_MUX_DP:
		typec_state = MUX_TYPEC_DP;
		break;
	case TYPEC_MUX_DOCK:
		typec_state = MUX_TYPEC_USB_AND_DP;
		usb_state = MUX_USB_HOST;
		break;
	}

	if (polarity)
		typec_state |= MUX_TYPEC_POLARITY_INV;

	ret = mux_control_select(data->type_c_mode_mux, typec_state);
	if (ret) {
		dev_err(data->dev, "Error setting Type-C mode mux: %d\n", ret);
		return ret;
	}

	ret = mux_control_select(data->usb_role_mux, usb_state);
	if (ret) {
		dev_err(data->dev, "Error setting USB role mux: %d\n", ret);
		mux_control_deselect(data->type_c_mode_mux);
		return ret;
	}

	data->muxes_set = true;
	return 0;
}

struct tcpc_mux_dev *devm_tcpc_gen_mux_create(struct device *dev)
{
	struct tcpc_gen_mux_data *data;
	int ret;

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return ERR_PTR(-ENOMEM);

	/* The use of either mux is optional */
	data->type_c_mode_mux =
		devm_mux_control_get_optional(dev, "type-c-mode-mux");
	if (IS_ERR(data->type_c_mode_mux)) {
		ret = PTR_ERR(data->type_c_mode_mux);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Error getting Type-C mux: %d\n", ret);
		return ERR_PTR(ret);
	}

	data->usb_role_mux = devm_mux_control_get_optional(dev, "usb-role-mux");
	if (IS_ERR(data->usb_role_mux)) {
		ret = PTR_ERR(data->usb_role_mux);
		if (ret != -EPROBE_DEFER)
			dev_err(dev, "Error getting USB role mux: %d\n", ret);
		return ERR_PTR(ret);
	}

	data->dev = dev;
	data->mux.set = tcpc_gen_mux_set;

	return &data->mux;
}
EXPORT_SYMBOL_GPL(devm_tcpc_gen_mux_create);

MODULE_AUTHOR("Hans de Goede <hdegoede@redhat.com>");
MODULE_DESCRIPTION("Generic Type-C mux driver using the mux subsys");
MODULE_LICENSE("GPL");
