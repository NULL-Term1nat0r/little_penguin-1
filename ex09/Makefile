MODULE_NAME = my_mount.ko

obj-m += my_mount.o

ccflags-y += -I/usr/src/linux-next/fs

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	dmesg -C
	rmmod my_mount.ko


re: clean inject

test: re
	insmod my_mount.ko
	cat /dev/fortytwo
	rmmod my_mount.ko
inject: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	insmod my_mount.ko

install:
	cp my_mount.ko /lib/modules/$(shell uname -r)/extra/
	sudo depmod -a
	sudo modprobe debugfs