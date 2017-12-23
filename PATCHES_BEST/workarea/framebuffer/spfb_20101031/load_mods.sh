#!/bin/sh
modprobe sysfillrect
modprobe sysimgblt
modprobe syscopyarea
modprobe fb_sys_fops
insmod ./spfb.ko
