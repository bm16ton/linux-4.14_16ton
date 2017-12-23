obj-m += ax206fb.o

KVERSION := $(shell uname -r)
KHDIR := /usr/src/linux-headers-$(KVERSION)
KDIR := .
all:
	make -C $(KHDIR) M=$(PWD) modules

clean:
	make -C $(KDIR)/ M=$(PWD) clean
