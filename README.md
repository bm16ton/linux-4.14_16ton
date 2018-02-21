## Do to a fire I no longer own a GPD win :( I will continue to apply the patches but will not have a  way to test if they are still working.  
## Also do to health issues my updates will be seldom, and new patches rare over next few months  Ill be back on it as soon as i can and will try to update to newest 4.14 releases as fast as i can
### Ben Maddocks 2/20/2018

WARNING/DISCLAIMER I am not a kernel developer. I don't even know C. I use logical deduction, trial and error etc  
to create my patches or to edit older patches to work on newer kernels. Ive been very lucky so far. So that said  
if you use my kernel, family and friends will most likely die randomly, your house will burn down and maybe on the  
the far reaches of possibility your PC will work funny or not at all. You have been warned.  

MOVED UPDATES TO changelog  

ADDITIONS:  
-	CFG80211 no longer changes regdomain based on associated AP  
-	Atheros ath9k all 5ghz channels are open for AP etc  
-	Atheros ath9k now has extra channels;  
    *		 0 thru -19 and random channels 16 thru 65 (got bored will add the rest)  
    *		 requires this kernel and my hostapd/iw patches in tools/16ton  
-	Atheros has CTL's removed, so cards reported maximum is now available. various other little things.  
-	Atheros debugfs options added,  
    *		chanbw   echo 5 > chanbw  brings channel bandwidth to 5. echo 10 brings to 10 etc  
    *		diag	 enable/disable tx/rx etc  
    *		dump_eep_power  shows all tx power related info  
    *	    probably more i forgot  
-	Atheros modparams added  
    -	ath.ko  
    *			16ton   (enables all bands and power if u forgot to compile with correct options  
-		ath9k_hw.ko  
    *			txpower:Manual TX power setting  
    *			cwmin  
    *			cwmax  
    *			aifs  
    *			cck_sifs  
    *			ofdm_sifs  
    *			slottime  
    *			thresh62  
-		Will add modparams for rx_intr_mitigation rxbuf and ani_interval  
-	increased max txpower for the brcm wifi to 30db MUCH HIGHER THEN THE CARD CAN PROBABLY HANDLE SET MAX AT BOOT!  
-	added pl-25A1 usb-2-usb network adapter  
-	added laplink usb2usb network adapter (orange body)  
-	added Marvell Avastar 88W8897 BACK into the kernel  
-	added more vids/pids for uss720 and plusb  
-	applied Krack patches to wpa/hostapd in tools/16ton, also 80/80 vht patch  
-	in tree framebuffer kernel for ax206 hacked mini digital picture frames, userland utility in tools/16ton  
-	Newer/larger sdcard support on intel sdcard readers  
-	automatic bringup on acpi connected uart broadcom bt adapters  
-	more usb type C mux support  
-	Added new cpu compile optimizations for alot of cpu's. The atom option is using the old non -march atom  
    * standard prescott etc i believe is also for older atoms, Sometime ill manually add optimizations for the x7-Z8750 found in the gpd win, where i dont believe theres -march -mtune for the newer atoms.  
-	added kali-wifi-injection patches  
-	removed watchdog from rt2800usb (ralink)  

-	Im sure there more, and more is on the way  

FIXES:  
-	mwifiex on newer network-manager versions requires editing /etc/NetworkManager/NetworkManager.conf 
'''
[device]  
wifi.scan-rand-mac-address=no  
,,,

in tools/16ton  

tools/16ton/mtp_adb  
-	De-Android-ified mtp-server with setup scripts, my kernel allready has the required modules ported  

tools/16ton/iwleeprom  
-	forked from https://github.com/andyvand/AtherosROMKit iwleeprom allows reading/writing to certain intel and atheros cards. I patched it to enable all channels on all frequencies and increase txpower to 23db 2.4ghz and 22 on 5ghz on Intel cards and for Atheros cards it changes the eeprom reg domain to 0x6A tho Ive seen multiple sources say 65 unlocks 5ghz a quick look at Linux driver makes me think 6A will be less restrictive.If im wrong edit ath9kio.c search for 0x6A and replace it with 0x65, recompile. THO FOR ATHEROS CARDS THIS IS NOT REQUIRED AT ALL WHEN USING MY KERNEL. This might open things for windows people, i dunno i hate microsoft products.  

tools/16ton/reghack  
-	The original and a newer version of reghack to remove ath9k restrictions from compiled driver. Most usefull on embedded devices.  

tools/16ton/iw-3.17-16ton  
-	patched iw tools needed for my kernel patches  

tools/16ton/hostapd-wpa  
-	patched hostapd needed for my kernel patches, use 2.4 compiles with version 2.9 so system repos wont overwrite  

tools/16ton/dpf/st2205term*  
-	terminal emulator for hacked st2205 based tiny digital photo frames  

tools/16ton/dpf/FRAMEBUFFER/*  
-	The userland application required for the tiny usb digital photo frame based off the ax206 chipsets. and the stand alone kernel module source (maybe older then in tree)  

tools/16ton/beta-brite_alpha_led_displays  
-	Sum basic scripts for controlling Alpha/Betabrite LED signs  

tools/16ton/xboxdrv  
-	xboxdrv patched to add wireless support for chatpad 360  

SPECIAL THANKS OR CREDIT GOES TO (list may not be comprehensive will keep working on it)  
hansdegoede https://hansdegoede.livejournal.com/17445.html   (a real kernel developer, and talented programmer)  
TINY DIGITAL PHOTO FRAME STUFF;  
https://github.com/dreamlayers/dpf-ax    (A combonation of patches and code from all around)  
http://sourceforge.net/projects/dpf-ax/files   (ax206 files for hacking the firmware)  
http://picframe.spritesserver.nl/wiki/index.php/  
http://picframe.spritesserver.nl/wiki/index.php/DPF_with_AppoTech_AX206  (seems to go up and down)  
https://forum.doozan.com  (found the beginning of a framebuffer for ax206 for much older kernels here based on one  
for samsung dpf's found on this site;)  
https://ssl.webpack.de/puschmann.net/page/  
Atheros txpower module parameters i found from;  
https://github.com/bortek/EZ-WifiBroadcast   (this uses wifi cards in promisc mode for tx/rx making it better for  
 analog tx IE video. The extra channels needed alotta work)  
Atheros ignore eeprom and dont take on the AP's regdomain patches from openwrt  
https://github.com/openwrt/openwrt/tree/master/package/kernel/mac80211/patches   (tho mine came from much older  
versions just realized everythings updated will check)  
The marvel 8897 wireless card used to be in the kernel but was removed I simply reversed the removal patch and have  
slightly edited over the kernel versions to keep it being applied. I guess noone will get this chipset for reg usb  
wifi cards (mine came from broken xboxone wifi/bluetooth and maybe nfc its a good card)  
hrm probly more to follow.  
