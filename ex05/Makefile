obj-m += misc_device.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rmmod misc_device.ko

re: clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

test: re
	insmod misc_device.ko
	cat /dev/fortytwo
	rmmod misc_device.ko
inject: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	insmod misc_device.ko
	echo "Hello World" > /dev/fortytwo
	cat /dev/fortytwo

install:
	cp misc_device.ko /lib/modules/$(shell uname -r)/extra/
	sudo depmod -a
	sudo modprobe misc_device