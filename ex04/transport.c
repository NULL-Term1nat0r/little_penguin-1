#include <linux/hid.h>
#include <linux/string.h>

int function(void){


        struct hid_device *hid;
	int ret;

	hid = hid_allocate_device();
	if (IS_ERR(hid)) {
		ret = PTR_ERR(hid);
		return 1;
	}

	strlcpy(hid->name, <device-name-src>, 127);
	strlcpy(hid->phys, <device-phys-src>, 63);
	strlcpy(hid->uniq, <device-uniq-src>, 63);

	hid->ll_driver = &custom_ll_driver;
	hid->bus = <device-bus>;
	hid->vendor = <device-vendor>;
	hid->product = <device-product>;
	hid->version = <device-version>;
	hid->country = <device-country>;
	hid->dev.parent = <pointer-to-parent-device>;
	hid->driver_data = <transport-driver-data-field>;

	ret = hid_add_device(hid);
	if (ret)
		return 1;
        return 0;
}