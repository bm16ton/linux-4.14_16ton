#!/bin/sh
# patch -p1 < PATCHES_BEST/88W8897/8897-less-restrictive-nfc-bind.patch
patch -p1 < PATCHES_BEST/88W8897/mwifi_8897_usb.c.patch
patch -p1 < PATCHES_BEST/88W8897/mwifi_8897_usb.h.patch
patch -p1 < PATCHES_BEST/88W8897/mwifi_8897_Kconfig.patch
patch -p0 < PATCHES_BEST/Go_Temp/hid-ids_4.8.patch 
patch -p0 < PATCHES_BEST/Go_Temp/ldusb.patch
patch -p0 < PATCHES_BEST/Go_Temp/vernier_ldusb_hid-core_ignore.patch
# patch -p1 < PATCHES_BEST/openwrt-402-ath_regd_optional.patch
# patch -p1 < PATCHES_BEST/openwrt-404-regd_no_assoc_hints.patch 
patch -p0 < PATCHES_BEST/prolific_plusb_patches/plusb_add_laplink_device.patch
patch -p0 < PATCHES_BEST/prolific_plusb_patches/plusb_add_PL-2501_support.patch
# patch -p0 < PATCHES_BEST/atheros_work-in-progress_patches/406-ath_relax_default_regd-KERNEL-4.8.patch
# patch -p1 < PATCHES_BEST/current_best_txpower/Max-TXpower_base_min_NO-CTL.patch
# patch -p0 < PATCHES_BEST/ath9k-linux-4.8_dump-txpower-info_debugfs.patch
# patch -p1 < PATCHES_BEST/hwe/BFQ.patch
# patch -p1 < PATCHES_BEST/hwe/BFS.patch 
# patch -p1 < PATCHES_BEST/hwe/MISC-GOOD-MODED-4-hwe-patch.patch
# cp PATCHES_BEST/config-4.8.17-all-patched-good-txpower ./.config
