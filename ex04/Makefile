obj-m += detect_hotplug_event.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	cp detect_hotplug_event.ko /lib/modules/$(shell uname -r)/extra/
	sudo depmod -a
	sudo modprobe detect_hotplug_event