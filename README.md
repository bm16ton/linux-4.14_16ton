Forked from https://github.com/jwrdegoede/linux-sunxi/tree/v4.14-footrail

UPDATE Dec 22 2017; Brought up to 4.14.8

ISSUES: At first non of the 5ghz band was available in Master (AP) mode on the brcmfmac (especially on GPD Win
	the only brcmfmac device i own) When I first applied my patches and the restrictions where removed from the
	channel list, I tried an AP on 36 and it worked. I foolishly assumed I was done. I now realized even tho
	hostapd starts and runs without errors on channels above 36 a stupidly quick test today showed those channels
	are not in fact working. I will investigate this further. I quickly tried channels above 140. Which should
	be outside the fake X2 regulatory domains restricitons and it still didnt "seem" to work.

ISSUES: Currently the Krack patches i applied to 2.3 keep the middle band from correctly working in AP mode and
	all kinds on notworking in Managed mode. Currently working on applying my patches to 2.4
UPDATE: Theres the 2.4 source and my patches in tools/16ton now. Only did a quick test channel 36 worked fine
	on GPD Win ill test it against my atheros patches later.

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
