#!/bin/bash
#./reghack.x86_64.elf /lib/modules/$(uname -r)/kernel/drivers/net/wireless/ath/ath.ko
#./reghack.x86_64.elf /lib/modules/$(uname -r)/kernel/net/wireless/cfg80211.ko
cp /lib/modules/$(uname -r)/kernel/drivers/net/wireless/ath/ath.ko /lib/modules/$(uname -r)/kernel/drivers/net/wireless/ath/ath.ko.backup
cp /lib/modules/$(uname -r)/kernel/net/wireless/cfg80211.ko /lib/modules/$(uname -r)/kernel/net/wireless/cfg80211.ko.backup
mkdir /opt/atheros_reg_fix/$(date -I)-$(uname -r)
cp /lib/modules/$(uname -r)/kernel/drivers/net/wireless/ath/ath.ko /opt/atheros_reg_fix/$(date -I)-$(uname -r)/
cp /lib/modules/$(uname -r)/kernel/net/wireless/cfg80211.ko /opt/atheros_reg_fix/$(date -I)-$(uname -r)/
/opt/atheros_reg_fix/reghack2 /lib/modules/$(uname -r)/kernel/drivers/net/wireless/ath/ath.ko
/opt/atheros_reg_fix/reghack2 /lib/modules/$(uname -r)/kernel/net/wireless/cfg80211.ko
