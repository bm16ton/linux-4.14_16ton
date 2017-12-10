Forked from https://github.com/jwrdegoede/linux-sunxi/tree/v4.14-footrail

FIXES:
	SDCARD adapter now patched works with bigger, newer and faster cards.
	5ghz AP mode works now, edit brcmfmac4356-pcie.txt change ccode from X2 to 00

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
