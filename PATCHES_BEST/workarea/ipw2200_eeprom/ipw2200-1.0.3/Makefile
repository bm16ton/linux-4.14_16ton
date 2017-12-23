#
# Makefile for the Linux Wireless network device drivers.
# 
# Original makefile by Peter Johanson
#
# NOTE: This make file can serve as both an external Makefile (launched
#       directly by the user), or as the sub-dir Makefile used by the kernel
# 	build system.

# If CONFIG_IPW2200 isn't set, we'll assume the user has never configured
# their kernel to include this module and set up some defaults.
#
# NOTE: If you have previously added IPW2200 to your kernel and configured it 
#       for inclusion, these settings will be overridden by your kernel
#       configuration.
ifndef CONFIG_IPW2200
EXTERNAL_BUILD=y
CONFIG_IPW2200=m
CONFIG_IPW_DEBUG=y
# NOTE: Do not enable this declaration -- monitor mode is not yet
#       supported.  This is here so its easier to add the feature once 
#       we have all the hooks in place.
# CONFIG_IPW_MONITOR=y
endif

# We have to add drivers/net/wireless until ieee802_11.h is in the default
# include path
ifneq ($(CONFIG_IPW_DEBUG),)
	EXTRA_CFLAGS += -g -Wa,-adhlms=$@.lst
endif

EXTRA_CFLAGS += -I${SUBDIRS}

list-m :=
list-$(CONFIG_IPW2200) += ipw2200

obj-$(CONFIG_IPW2200) += ipw2200.o

ifndef ($(CONFIG_IEEE80211))
CONFIG_IEEE80211=$(CONFIG_IPW2200)
CONFIG_IEEE80211_DEBUG=y

# If you do not want any encryption/decryptoin code in the driver, comment out
# this line:
CONFIG_IEEE80211_CRYPT=$(CONFIG_IEEE80211)

# If you want to build WPA support, uncomment this line:
CONFIG_IEEE80211_CRYPT_WEP=$(CONFIG_IEEE80211)
CONFIG_IEEE80211_CRYPT_TKIP=$(CONFIG_IEEE80211)
CONFIG_IEEE80211_CRYPT_CCMP=$(CONFIG_IEEE80211)

endif

list-$(CONFIG_IEEE80211) += ieee80211 
list-$(CONFIG_IEEE80211) += ieee80211_crypt
list-$(CONFIG_IEEE80211_CRYPT_WEP) += ieee80211_crypt_wep
list-$(CONFIG_IEEE80211_CRYPT_CCMP) += ieee80211_crypt_ccmp
list-$(CONFIG_IEEE80211_CRYPT_TKIP) += ieee80211_crypt_tkip

obj-$(CONFIG_IEEE80211) += ieee80211.o ieee80211_crypt.o
obj-$(CONFIG_IEEE80211_CRYPT_WEP) += ieee80211_crypt_wep.o
obj-$(CONFIG_IEEE80211_CRYPT_CCMP) += ieee80211_crypt_ccmp.o
obj-$(CONFIG_IEEE80211_CRYPT_TKIP) += ieee80211_crypt_tkip.o
ieee80211-objs := \
	ieee80211_module.o \
	ieee80211_tx.o \
	ieee80211_rx.o \
	ieee80211_wx.o

#
# Begin dual Makefile mode here.  First we provide support for when we
# are being invoked by the kernel build system
#
ifneq ($(KERNELRELEASE),)

#
# If you receive a compile message about multiple definitions of
# CONFIG_IPW_DEBUG, then you have IPW2200 into the full kernel build, and 
# these definitions are now being set up by the kernel build system.  
#
#
# To correct this, remove any CONFIG_{IPW,IEEE80211}* lines from 
# $(KSRC)/.config and $(KSRC)/include/linux/autoconf.h
# 

ifeq ($(EXTERNAL_BUILD),y)
ifdef CONFIG_IPW_DEBUG	
EXTRA_CFLAGS += -DCONFIG_IPW_DEBUG=$(CONFIG_IPW_DEBUG)
endif
ifdef CONFIG_IPW_MONITOR
EXTRA_CFLAGS += -DCONFIG_IPW_MONITOR=$(CONFIG_IPW_MONITOR)
endif
ifdef CONFIG_IEEE80211_DEBUG
EXTRA_CFLAGS += -DCONFIG_IEEE80211_DEBUG=$(CONFIG_IEEE80211_DEBUG)
endif
ifdef CONFIG_IEEE80211_CRYPT_WEP
EXTRA_CFLAGS += -DCONFIG_IEEE80211_CRYPT_WEP=$(CONFIG_IEEE80211_CRYPT_WEP)
endif
ifdef CONFIG_IEEE80211_CRYPT_TKIP
EXTRA_CFLAGS += -DCONFIG_IEEE80211_CRYPT_TKIP=$(CONFIG_IEEE80211_CRYPT_TKIP)
endif
ifdef CONFIG_IEEE80211_CRYPT_CCMP
EXTRA_CFLAGS += -DCONFIG_IEEE80211_CRYPT_CCMP=$(CONFIG_IEEE80211_CRYPT_CCMP)
endif
endif

ifneq ($(PATCHLEVEL),6) # If we are not on a 2.6, then do 2.4 specific things

ifdef CONFIG_IEEE80211
export-objs := \
	ieee80211_module.o \
	ieee80211_wx.o \
	ieee80211_tx.o \
	ieee80211_rx.o \
	ieee80211_crypt.o \
	ieee80211_crypt_wep.o
endif

export-objs += ipw2200.o
#O_TARGET := ipw2100.o

include $(TOPDIR)/Rules.make

ieee80211.o: $(ieee80211-objs)
	$(LD) -r -o $@ $(ieee80211-objs)

endif # End if 2.4 specific settings

else 
# Here we begin the portion that is executed if the user invoked this Makefile
# directly.

# KSRC should be set to the path to your sources
# modules are installed into KMISC
KVER  := $(shell uname -r)
KSRC := /lib/modules/$(KVER)/build
KMISC := /lib/modules/$(KVER)/drivers/net/wireless/

# KSRC_OUTPUT should be overridden if you are using a 2.6 kernel that
# has it's output sent elsewhere via KBUILD_OUTPUT= or O=
KSRC_OUTPUT := $(KSRC)

# If we find Rules.make, we can assume we're using the old 2.4 style building
OLDMAKE=$(wildcard $(KSRC)/Rules.make)
PWD=$(shell pwd)

VERFILE := $(KSRC_OUTPUT)/include/linux/version.h
KERNELRELEASE := $(shell \
	if [ -r $(VERFILE) ]; then \
		(cat $(VERFILE); echo UTS_RELEASE) | \
		$(CC) -I$(KSRC_OUTPUT) $(CFLAGS) -E - | \
		tail -n 1 | \
		xargs echo; \
        else \
		uname -r; \
	fi)

MODPATH := $(DESTDIR)/lib/modules/$(KERNELRELEASE)

all: modules

clean:
	rm -f *.mod.c *.mod *.o *.ko .*.cmd .*.flags *.lst *~ .#*
	rm -rf $(PWD)/tmp .tmp_versions
	for file in *.{c,h} net/*.h; do \
		sed -i -e "s:\ *$$::g" -e "s:\t*$$::g" $$file; \
	done

ifeq ($(OLDMAKE),)

TMP=$(PWD)/tmp
MODVERDIR=$(TMP)/.tmp_versions

modules:
ifdef ($(KSRC_OUTPUT)/.tmp_versions)
	mkdir -p $(MODVERDIR)
	-cp $(KSRC_OUTPUT)/.tmp_versions/*.mod $(MODVERDIR)
endif
ifeq ($(KSRC),$(KSRC_OUTPUT)) # We're not outputting elsewhere
ifdef ($(KSRC)/.tmp_versions)
	-cp $(KSRC)/.tmp_versions/*.mod $(MODVERDIR)
endif
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) MODVERDIR=$(PWD) modules
else # We've got a kernel with seperate output, copy the config, and use O=
	mkdir -p $(TMP)
	cp $(KSRC_OUTPUT)/.config $(TMP)
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) MODVERDIR=$(PWD) O=$(PWD)/tmp modules
endif

else # We're on 2.4, and things are slightly different

modules:
	$(MAKE) -C $(KSRC) SUBDIRS=$(PWD) BUILD_DIR=$(PWD) modules

endif # End 2.4 / 2.6 specific

install: modules
	install -d $(KMISC)
	install -m 644 -c $(addsuffix .ko,$(list-m)) $(KMISC)
	/sbin/depmod -a
	@echo "Don't forget to copy firmware to /usr/lib/hotplug/firmware/ and have the "
	@echo "hotplug tools in place."
	@echo "See INSTALL for more information."


uninstall:
	rm -rf $(addprefix $(KMISC),$(addsuffix .ko,$(list-m)))
	/sbin/depmod -a

endif # End of internal build

.PHONY: TAGS tags

RCS_FIND_IGNORE := \( -name SCCS -o -name BitKeeper -o -name .svn -o -name CVS \) -prune -o
define all-sources
	( find . $(RCS_FIND_IGNORE) -name '*.[chS]' -print )
endef

TAGS:
	$(all-sources) | etags -
tags:
	rm -f $@
	CTAGSF=`ctags --version | grep -i exuberant >/dev/null && echo "-I __initdata,__exitdata,EXPORT_SYMBOL,EXPORT_SYMBOL_NOVERS"`; \
	$(all-sources) | xargs ctags $$CTAGSF -a

