#include <linux/module.h>
#include <linux/hid.h>

/* Your Logitech keyboard from dmesg */
#define MY_VENDOR_ID  0x046d
#define MY_PRODUCT_ID 0xc31c

static int my_hid_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	int ret;

	printk(KERN_INFO "MYHID DRIVER: Probing %s (Vendor: %04x, Product: %04x)\n",
	       hdev->name, hdev->vendor, hdev->product);

	/* Parse the report descriptor so the HID layer understands the device */
	ret = hid_parse(hdev);
	if (ret) {
		printk(KERN_ERR "MYHID DRIVER: hid_parse failed (%d)\n", ret);
		return ret;
	}

	/* Start the hardware and wire it up to the input subsystem.
	 * Without this, the keyboard would be claimed by us but dead. */
	ret = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (ret) {
		printk(KERN_ERR "MYHID DRIVER: hid_hw_start failed (%d)\n", ret);
		return ret;
	}

	printk(KERN_INFO "MYHID DRIVER: Keyboard active and monitored\n");
	return 0;
}

static void my_hid_remove(struct hid_device *hdev)
{
	printk(KERN_INFO "MYHID DRIVER: Removing %s\n", hdev->name);
	hid_hw_stop(hdev);
}

static const struct hid_device_id my_hid_ids[] = {
	/* Match only your specific Logitech USB keyboard */
	{ HID_USB_DEVICE(MY_VENDOR_ID, MY_PRODUCT_ID) },
	{},
};
MODULE_DEVICE_TABLE(hid, my_hid_ids);

static struct hid_driver my_hid_driver = {
	.name     = "my_hid_keyboard",
	.id_table = my_hid_ids,
	.probe    = my_hid_probe,
	.remove   = my_hid_remove,
};

/* This macro replaces module_init / module_exit for HID drivers */
module_hid_driver(my_hid_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlo Cattano");
MODULE_DESCRIPTION("HID-layer keyboard detector that keeps input working");



