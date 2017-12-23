#!/bin/sh
modprobe sysfillrect
modprobe sysimgblt
modprobe syscopyarea
modprobe fb_sys_fops
insmod /opt/dpf/ax206/mini-dpf-AX206-fb/spfb/ax206fb.ko video=vfb
