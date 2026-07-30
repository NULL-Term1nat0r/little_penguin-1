# Assignment 04
You survived the coding style chaos—well done!
Now, let’s tackle something more "real" because I can tell you’re getting a bit bored.
To Do
• Modify the kernel module you wrote in Assignment 01 so that it is automatically
loaded when any USB keyboard is plugged in. This should be triggered by the appropriate userspace hotplug tools, which may include depmod, kmod, udev, mdev,
or systemd, depending on your distribution.
Turn In
• A rules file, appropriate for your system.
• Your updated code.
• Proof that your module works as intended.
Yes, it seems simple, but it’s actually a bit tricky.
For a hint, consult Chapter 14 of the book Linux Device Drivers, 3rd Edition. It’s freely
available online—no purchase required.

# How it works on hardware level HID (Human Interface Devices)

- Most peripheral controllers use a hardware interrupt to inform the OS or device driver that some (usually uregent) action is required. 
- Examples are network adapters (NIC), block devices (disks), USB devices, AV devices, human interface devices (HID's) such as keyboard, mice, touchscreens and video screens, clocks/timer chips, DMA controllers and so on
- The idea behind interrrupts is efficiency. Instead of constantly polling the chip (it would rapidly drain the battery) the interrupt is a means to have the low level software run only as and when required.
- modern motherboards have an interrupt controller chip of some sort, which is often called the [IO][A]PIC, short for IO-[Advanced]Programmable Interrupt Controller on x86
- the kernel documents can be found here: https://www.kernel.org/doc/html/latest/x86/i386/IO-APIC.html#io-apic
- it can also be a generic interrupt controller (GIC) on ARM (Advanced RISC (Reduced Instruction Set Computer) Machines).

- the PIC has one line to the CPU's interrupt pin . Onboard peripherals capable of asserting interrupts will have an IRQ (Interrupt ReQuest) line to the PIC
- IRQ is the commom abbreviated term for Interrupt ReQuest; it denotes the interrupt line (or lines) that's allocated to a peripheral device

- here is a mental model that clarifies the different layers. You see the position of HID in the hierachy and where it interacts with the input system /dev/input

             USB hardware
                  │
                  │ USB packets
                  ▼
        ┌───────────────────┐
        │ USB Host Controller│
        │   xHCI / EHCI      │
        └─────────┬─────────┘
                  │
                  ▼
        ┌───────────────────┐
        │     USB Core      │
        │                   │
        │ device enumeration│
        │ configuration     │
        │ interfaces        │
        │ driver matching   │
        └─────────┬─────────┘
                  │
                  │ USB interface
                  ▼
        ┌───────────────────┐
        │    HID Driver     │
        │     usbhid        │
        └─────────┬─────────┘
                  │
                  ▼
        ┌───────────────────┐
        │   HID subsystem   │
        └─────────┬─────────┘
                  │
                  ▼
        ┌───────────────────┐
        │ Input subsystem   │
        └─────────┬─────────┘
                  │
                  ▼
              /dev/input/


 # 1.) step

 - after you plug in a USB the USB host controller notices that something happened and sends information to the USB subsystem so it discovers the new device
 - the USB Core performs a process called "enumeration". it basically wants to know who are you ?
 - the device responds with descriptors containing information such as:
 Vendor ID
Product ID
Device class
Configuration
Interfaces
Endpoints
HID descriptor

example:
VID = 046d
PID = c31c
Class = HID
Subclass = Boot
Protocol = Keyboard

the USB Core is aware now that there is a USB device containing an interface that looks like a HID keyboard

# 2.) step

another important concept is that USB devices can contain multiple interfaces. Let's say you have a music system connected to your pc. you need an interface for the keys you press for example on a jbl box to handle the volume and you also need to send the data containing the music to it.
a device could contain the following:
USB Device
│
├── Interface 0 → Keyboard
│
├── Interface 1 → Mouse
│
└── Interface 2 → Vendor-specific device

different drivers can therefore bind to different interfaces. for a normal keyboard like in our case the interace is describes like this:
USB device
    │
    └── USB interface
             │
             ▼
         usbhid

# step 3

- how does the USB core know which driver shouldhandle it ? This is where the USB ID table comes in
- a USB driver can tell the kernel:

```
static const struct usb_device_id my_ids[] = {
    {
        USB_INTERFACE_INFO(
            USB_INTERFACE_CLASS_HID,
            USB_INTERFACE_SUBCLASS_BOOT,
            USB_INTERFACE_PROTOCOL_KEYBOARD
        ),
    },
    { }
};

MODULE_DEVICE_TABLE(usb, my_ids);
```

so you are saying:

My driver supports:

Class    = HID
Subclass = Boot
Protocol = Keyboard

the usb core has a driver/device matching mechanism. It compares
new USB interface
       │
       ▼
┌─────────────────────┐
│ USB driver matching │
└─────────────────────┘
       │
       ├── usbhid matches
       │
       ├── your driver matches
       │
       └── other drivers don't

- when a driver matches the kernel calls the probe()

you will find the code in kernel files where you will see the MODULE DEVICE TABLE

```
grep -A 100 "static const struct usb_device_id hid_usb_ids\[\]" /lib/modules/$(uname -r)/build/drivers/hid/usbhid/hid-core.c
static const struct usb_device_id hid_usb_ids[] = {
	{ .match_flags = USB_DEVICE_ID_MATCH_INT_CLASS,
		.bInterfaceClass = USB_INTERFACE_CLASS_HID },
	{ }						/* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, hid_usb_ids);

static struct usb_driver hid_driver = {
	.name =		"usbhid",
	.probe =	usbhid_probe,
	.disconnect =	usbhid_disconnect,
	.suspend =	pm_ptr(hid_suspend),
	.resume =	pm_ptr(hid_resume),
	.reset_resume =	pm_ptr(hid_reset_resume),
	.pre_reset =	hid_pre_reset,
	.post_reset =	hid_post_reset,
	.id_table =	hid_usb_ids,
	.supports_autosuspend = 1,
};

struct usb_interface *usbhid_find_interface(int minor)
{
	return usb_find_interface(&hid_driver, minor);
}

static int __init hid_init(void)
{
	int retval;

	retval = hid_quirks_init(quirks_param, BUS_USB, MAX_USBHID_BOOT_QUIRKS);
	if (retval)
		goto usbhid_quirks_init_fail;
	retval = usb_register(&hid_driver);
	if (retval)
		goto usb_register_fail;
	pr_info(KBUILD_MODNAME ": " DRIVER_DESC "\n");

	return 0;
usb_register_fail:
	hid_quirks_exit(BUS_USB);
usbhid_quirks_init_fail:
	return retval;
}

static void __exit hid_exit(void)
{
	usb_deregister(&hid_driver);
	hid_quirks_exit(BUS_USB);
}

module_init(hid_init);
module_exit(hid_exit);

MODULE_AUTHOR("Andreas Gal");
MODULE_AUTHOR("Vojtech Pavlik");
MODULE_AUTHOR("Jiri Kosina");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
```
# step 4

How does the coomunication between USB Core and HID work ?

The USB core provides the HID driver with abstractions such as:

```
struct usb_device
struct usb_interface
struct usb_driver
struct urb
```

The HID driver doesn't need to manually manipulate USB hardware.

Instead, it tells USB Core:

"Send this USB request."

USB Core then handles the lower-level USB machinery.

# step 5

One of the most important mechanisms to understand is the URB:

USB Request Block

Think of a URB as a kernel object representing:

"I want to perform this USB transfer."

For example, the HID driver can create an interrupt URB.

Conceptually:
```
HID driver
    │
    │ submit URB
    ▼
USB Core
    │
    ▼
Host controller driver
    │
    ▼
xHCI hardware
    │
    ▼
USB keyboard
```
```
When the keyboard sends data back:

USB keyboard
    │
    ▼
USB hardware
    │
    ▼
xHCI
    │
    ▼
USB Core
    │
    ▼
URB completion callback
    │
    ▼
HID driver
```

That's one of the major communication paths.

# step 6 

- what does the keyboard actually send ?
- when you press 'A' the keyboard doesn't send the character 'a' directly
- it sends a HID report like:
```
┌─────────────────────────┐
│ HID report              │
│                         │
│ modifiers               │
│ reserved                │
│ key 1                   │
│ key 2                   │
│ key 3                   │
│ key 4                   │
│ key 5                   │
│ key 6                   │
└─────────────────────────┘
```

-the HID driver receives that report.
-it interprets it according to the HID report descriptor.
-then it passes the resulting information into the Linux HID/input layers.\

# step 7

- USB keyboards use what USB calls an interrupt transfer
- but that does not mean
```
keyboard
   │
   │ hardware IRQ
   ▼
your driver
```
Instead, an interrupt USB transfer is a USB transfer type.

The actual hardware interrupt might look more like:
```
USB controller
      │
      │ hardware IRQ
      ▼
xHCI driver
      │
      ▼
USB Core
      │
      ▼
completed URB
      │
      ▼
HID driver
```

So:

USB interrupt transfer ≠ Linux hardware interrupt handler.

This distinction is very important when reading Billimoria's material about interrupts.

8. Where does usbhid fit?

Linux has a USB HID driver commonly referred to as:

usbhid

It sits between the USB subsystem and the generic HID subsystem:

              USB Core
                  │
                  │ USB interface
                  ▼
              ┌───────┐
              │usbhid │
              └───┬───┘
                  │
                  │ HID reports
                  ▼
             HID subsystem
                  │
                  ▼
             input subsystem
                  │
                  ▼
             keyboard layer

So usbhid is effectively translating:

USB transfers
      ↓
HID reports
      ↓
Linux HID events
9. Where your Assignment 04 module fits

This is where things get interesting.

Your assignment doesn't necessarily require you to replace usbhid.

Instead, you can have your own module get automatically loaded when the keyboard appears.

The chain becomes:

                 USB keyboard plugged in
                           │
                           ▼
                     USB Core
                           │
                           ▼
                  device/interface found
                           │
                           ▼
                     uevent generated
                           │
                           ▼
                         udev
                           │
                           ▼
                        modprobe
                           │
                           ▼
                  your_module.ko
                           │
                           ▼
                       module_init()

That's different from your module being the keyboard's HID driver.

10. This is probably the distinction your assignment wants you to understand

There are actually two separate problems:

Problem A — automatic module loading
USB keyboard
      ↓
USB Core
      ↓
uevent
      ↓
udev
      ↓
modprobe
      ↓
your_module
Problem B — actually writing a USB keyboard driver
USB keyboard
      ↓
USB Core
      ↓
USB driver matching
      ↓
your_driver.probe()
      ↓
URB
      ↓
HID reports

Assignment 04 primarily asks for A.

Chapter 14 of Linux Device Drivers 3 helps you understand B, which explains why the assignment points you there.

And Billimoria's book helps you understand the kernel-side concepts underneath this, such as:

module
driver
interrupts
workqueues
synchronisation
character devices
The most useful mental model

Keep these four layers separate in your head:

┌──────────────────────────────┐
│          udev                 │
│      userspace hotplug        │
└──────────────┬───────────────┘
               │
             uevent
               │
┌──────────────▼───────────────┐
│         USB Core              │
│                               │
│ enumeration                   │
│ device/driver matching        │
│ USB interfaces                │
│ URBs                          │
└──────────────┬───────────────┘
               │
          USB interface
               │
┌──────────────▼───────────────┐
│          usbhid               │
│                               │
│ USB ↔ HID translation         │
└──────────────┬───────────────┘
               │
            HID report
               │
┌──────────────▼───────────────┐
│       HID / Input subsystem   │
└──────────────────────────────┘

For the assignment, we should next trace one real keyboard plug-in on theLinux machine with commands like udevadm monitor, udevadm info, lsusb -v, and modprobe -c. That will let you see the exact uevent → modalias → udev → modprobe path rather than just learning it theoretically.