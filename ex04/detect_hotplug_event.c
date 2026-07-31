#include <linux/hid.h>
#include <linux/module.h>

static int my_hid_probe(struct hid_device *hdev,
			const struct hid_device_id *id)
{
	pr_info("my_hid: device detected\n");

	return 0;
}

static void my_hid_remove(struct hid_device *hdev)
{
	pr_info("my_hid: device removed\n");
}

static const struct hid_device_id my_hid_ids[] = {
	{ HID_USB_DEVICE(0x046d, 0xc31c) },
	{ }
};

MODULE_DEVICE_TABLE(hid, my_hid_ids);

static struct hid_driver my_hid_driver = {
	.name = "my_hid",
	.id_table = my_hid_ids,
	.probe = my_hid_probe,
	.remove = my_hid_remove,
};

module_hid_driver(my_hid_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("My USB HID keyboard driver");