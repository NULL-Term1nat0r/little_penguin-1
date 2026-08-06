obj-m += debugfs.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	dmesg -C
	rmmod debugfs.ko


re: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

test: re
	insmod debugfs.ko
	cat /dev/fortytwo
	rmmod debugfs.ko
inject: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	insmod debugfs.ko

install:
	cp debugfs.ko /lib/modules/$(shell uname -r)/extra/
	sudo depmod -a
	sudo modprobe debugfs