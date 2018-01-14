UPDATE: JAN 14TH: Just added the as of yet not mainlined retpoline patches to help mitigate the spectre vulnerability.
	It requires a patched gcc which is also not commited yet so use the one in my repo for now.
	https://github.com/bm16ton/gcc-retpoline
	Ill be able to remove these patches as soon as the finished code is mainlined

UPDATE: Jan 11th 2018: Brought it up to 4.14.13 which has patches for "Meltdown" just enable CONFIG_PAGE_TABLE_ISOLATION
	in your kernel config. UNFORTUNATELY Spectre doesn't appear to have patches in mainline kernel yet. Tho I have
	read sum distros have implemented there own out of tree patches. I will hopefully find time to look into
	those and maybe use them until an official mainline fix is out.
	INTEL MICROCODE FOR GPD-WIN;
		https://downloadcenter.intel.com/download/27431/Linux-Processor-Microcode-Data-File?product=93362
	GENERIC DOWNLOAD SITE TO SELECT DIFFERENT CPU
		https://downloadcenter.intel.com/download/27431/Linux-Processor-Microcode-Data-File


UPDATE: Jan 8th 2018: Added new xboxdrv userland driver in tools/16ton, patched with support for wireless chatpad
	(from https://github.com/xboxdrv/xboxdrv/tree/8a0b802bfcbc8ae48d803ea61d60ff6b2ddc42c7) plus my own
	minor patches/changes for dpkg-buildpackage to work

UPDATE: Jan 5th 2018: new usb-acm-mtp-setup script in tools/16ton/mtp_adb this should work on OTG ports.
	That said thanks to the work of HansGeode we have automatic port host/client switching, if you dont
	may need to adjust accordingly.
	MTP ttyACM and Mass storage WORK! just compile the required modules in. Use config-newist_use_this
	"cp config-newist_use_this .config"  then compile and install "make deb-pkg" sudo dpkg -i ../*.deb"
	"sudo cp tools/16ton/mtp_adb/usb-acm-mtp-setup /usr/local/sbin"      reboot
    "sudo usb-acm-mtp-setup"  then plug usb into PC.

UPDATE Jan 4th 2018; added two more pairs of dev/prod ids to uss720 (belkin F5UOO2 and D-Link DSB-P36)
	Added Android usb gadget/configfs/function  USB_F_FS USB_F_MTP USB_F_PTP USB_CONFIGFS_UEVENT
	also function_accessory but its currently in a dependency cycle and i havent looked at it.
	So far only mtp is tested (on gpd-win but should work with other otg devices) and works good
	as far as i can tell. in tools/16ton you will find a heavily patched mtp-server which is hard coded
	to share /root and no longer has the android/unity dependencies.
	Ubuntu's repo version of adbd would segfault (same as mtp-server) and a diff one is on tools, it
	doesnt segfault and the host machine can see it, havent gotten beyind that yet.
	The script in tools/16ton usb-mtp-setup run as root then start mtp-server as root

	In tools/16ton/parport is program called pport wich allows you to use parport and serial
	ports as GPIO. Even works with usb to parallel with uss720 chipsets. Ill add schematic sometime
	to show how to wire transistors and relays to controll bigger things.

UPDATE Dec 27 2017; brought up to 4.14.9
	nvidia 384 doesnt compile against 4.14.9, patches to fix this in CURRENTLY_APPLIED_PATCHES
	directory out-of-tree now has the following out of tree kernel modules (I believe they all have dkms support)
			i2c-ch341-usb    kernel i2c driver for ch341a
			spi-ch341-usb	 kernel spi driver for ch341a
			xtables-addons   iptables addons

UPDATE Dec 22 2017; Brought up to 4.14.8

NEW:
	Removed NO-IR from middle band
	applied Krack patches to wpa/hostapd in tools/16ton
    updated mini DPF framebuffer driver

FIXES:
	SDCARD adapter now patched works with bigger, newer and faster cards.
	5ghz AP mode works now, edit brcmfmac4356-pcie.txt change ccode from X2 to 00
	mwifiex on newer network-manager versions requires;

[device]
wifi.scan-rand-mac-address=no

	at end of /etc/NetworkManager/NetworkManager.conf



ADDITIONS:
	increased max txpower for the bradcom wifi to 30db MUCH HIGHER THEN THE CARD CAN HANDLE MOST LIKELY SET MAX AT BOOT!
	added pl-25A1 usb-2-usb network adapter
	added laplink usb2usb network adapter (orange body)
	added Marvell Avastar 88W8897 BACK into the kernel

Im sures there more, and more is on the way

in tools/16ton

tools/16ton/iwleeprom
	forked from https://github.com/andyvand/AtherosROMKit iwleeprom allows reading/writting to certain intel and
	atheros cards. I patched it to enable all channels on all frequencies and increase txpower to 23db 2.4ghz and
	22 on 5ghz on Intel cards and for Atheros cards it changes the eeprom reg domain to 0x6A tho ive seen multiple
    sources say 65 unlocks 5ghz a quick look at linux driver makes me think 6A will be less restrictive, if im wrong
	edit ath9kio.c search for 0x6A and replace it with 0x65, recompile. THO FOR ATHEROS CARDS THIS IS NOT REQUIRED AT
	ALL WHEN USING MY KERNEL. This might open things for windows people, i dunno i hate microsoft products.

tools/16ton/iw-3.17-16ton
	patched iw tools needed for my kernel patches

tools/16ton/hostapd-2.9
	patched hostapd needed for my kernel patches

tools/16ton/dpf/st2205term*
	terminal emulator for hacked st2205 based tiny digital photo frames

tools/16ton/dpf/FRAMEBUFFER/*
	The userland application required for the tiny usb digital photo frame
	based off the ax206 chipsets. and the stand alone kernel module source (maybe older then in tree)
