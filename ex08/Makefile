MODULE_NAME = fix.ko

obj-m += fix.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	dmesg -C
	rmmod fix.ko


re: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

test: re
	insmod fix.ko
	cat /dev/fortytwo
	rmmod fix.ko
inject: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	insmod fix.ko

install:
	cp fix.ko /lib/modules/$(shell uname -r)/extra/
	sudo depmod -a
	sudo modprobe debugfs