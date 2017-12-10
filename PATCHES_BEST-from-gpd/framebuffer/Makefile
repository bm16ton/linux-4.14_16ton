obj-m += ax206fb.o

all:
	make -C /usr/src/linux-headers-4.8.11-hipower-plain/ M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
