#!/bin/sh

CONFIGFS="/sys/kernel/config"
GADGET="$CONFIGFS/usb_gadget"
VID="0x18d1"
PID="0x4E26"
SERIAL="0123456789"
MANUF="16ton"
PRODUCT="tridev"

case "$1" in
    start)
        echo "Creating the USB gadget"

        echo "Loading composite module"
		modprobe usb_f_fs
		modprobe usb_f_mtp
		modprobe usb_f_ptp
        modprobe libcomposite

        echo "Creating gadget directory"
        mkdir gadget
        cd gadget
        if [ $? -ne 0 ]; then
            echo "Error creating usb gadget in configfs"
            exit 1;
        fi

        echo "Setting Vendor and Product ID's"
        echo $VID > idVendor
        echo $PID > idProduct

        echo "Setting English strings"
        mkdir strings/0x409
        echo $SERIAL > strings/0x409/serialnumber
        echo $MANUF > strings/0x409/manufacturer
        echo $PRODUCT > strings/0x409/product

        echo "Setting configuration"
        mkdir configs/c.1
        mkdir configs/c.1/strings/0x409
        echo "CDC ACM + MTP + Mass Storage" > configs/c.1/strings/0x409/configuration
        echo 120 > configs/c.1/MaxPower

        echo "Creating ACM interface"
        mkdir functions/acm.GS0
        ln -s functions/acm.GS0 configs/c.1

        echo "Creating MTP interface"
        mkdir functions/mtp.mtp
        ln -s functions/mtp.mtp configs/c.1
        mkdir /dev/mtp

        echo "Creating Mass Storage interface"
        mkdir functions/mass_storage.ms0
        echo "/dev/mmcblk0" > functions/mass_storage.ms0/lun.0/file
        echo "1" > functions/mass_storage.ms0/lun.0/removable
        ln -s functions/mass_storage.ms0 configs/c.1/mass_storage.ms0

        echo "Binding USB Device Controller"
        echo `ls /sys/class/udc` > UDC

        echo "Starting the MTP responder daemon"
        mtp-server &
        ;;
    stop)
        echo "Stopping the USB gadget"

        echo "Killing MTP responder daemon"
        killall mtp-server

        cd $GADGET/gadget

        if [ $? -ne 0 ]; then
            echo "Error: no configfs gadget found"
            exit 1;
        fi

        echo "Unbinding USB Device Controller"
        echo "" > UDC

        echo "Removing Mass Storage interface"
        rm configs/c.1/mass_storage.ms0
        rmdir functions/mass_storage.ms0

        echo "Removing MTP interface"
        umount /dev/mtp
        rmdir /dev/mtp
        rm configs/c.1/mtp.mtp
        rmdir functions/mtp.mtp

        echo "Removing ACM interface"
        rm configs/c.1/acm.GS0
        rmdir functions/acm.GS0

        echo "Clearing English strings"
        rmdir strings/0x409

        echo "Cleaning up configuration"
        rmdir configs/c.1/strings/0x409
        rmdir configs/c.1

        echo "Removing gadget directory"
        cd $GADGET
        rmdir gadget

        cd /

        echo "Unmounting ConfigFS"
        umount $CONFIGFS
        ;;
esac
