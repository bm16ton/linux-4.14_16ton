#!/bin/sh
patch --dry-run -p1 < PATCHES_BEST/88W8897/8897-less-restrictive-nfc-bind.patch
patch --dry-run -p1 < PATCHES_BEST/88W8897/mwifi_8897_usb.c.patch
patch --dry-run -p1 < PATCHES_BEST/88W8897/mwifi_8897_usb.h.patch
patch --dry-run -p1 < PATCHES_BEST/88W8897/mwifi_8897_Kconfig.patch
patch --dry-run -p0 < PATCHES_BEST/Go_Temp/hid-ids_4.8.patch 
patch --dry-run -p0 < PATCHES_BEST/Go_Temp/ldusb.patch
patch --dry-run -p0 < PATCHES_BEST/Go_Temp/vernier_ldusb_hid-core_ignore.patch
patch --dry-run -p1 < PATCHES_BEST/openwrt-402-ath_regd_optional.patch
patch --dry-run -p1 < PATCHES_BEST/openwrt-404-regd_no_assoc_hints.patch 
patch --dry-run -p0 < PATCHES_BEST/prolific_PL-2501_usb2usb_network_adadapter/plusb.patch
# patch --dry-run -p0 < PATCHES_BEST/atheros_work-in-progress_patches/406-ath_relax_default_regd-KERNEL-4.8.patch
patch --dry-run -p1 < PATCHES_BEST/current_best_txpower/Max-TXpower_base_min_NO-CTL.patch
