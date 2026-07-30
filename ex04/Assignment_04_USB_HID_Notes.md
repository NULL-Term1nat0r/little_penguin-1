# Assignment 04

> You survived the coding style chaos—well done!
>
> Now, let’s tackle something more "real" because I can tell you’re getting a bit bored.

## To Do

- Modify the kernel module you wrote in Assignment 01 so that it is automatically loaded when any USB keyboard is plugged in.
- This should be triggered by the appropriate userspace hotplug tools, which may include:
  - `depmod`
  - `kmod`
  - `udev`
  - `mdev`
  - `systemd`
  - depending on your distribution.

## Turn In

- A rules file, appropriate for your system.
- Your updated code.
- Proof that your module works as intended.

> Yes, it seems simple, but it’s actually a bit tricky.

For a hint, consult **Chapter 14** of the book *Linux Device Drivers, 3rd Edition*. It’s freely available online—no purchase required.

---

# How It Works on the Hardware Level — HID (Human Interface Devices)

- Most peripheral controllers use a hardware interrupt to inform the OS or device driver that some (usually urgent) action is required.
- Examples include:
  - Network adapters (NICs)
  - Block devices (disks)
  - USB devices
  - AV devices
  - Human Interface Devices (HIDs), such as keyboards, mice, touchscreens, and video screens
  - Clock/timer chips
  - DMA controllers
  - And so on
- The idea behind interrupts is **efficiency**. Instead of constantly polling the chip—which would waste CPU resources and rapidly drain a battery—the interrupt is a means to have the low-level software run only when required.
- Modern motherboards have an interrupt controller chip of some sort, which is often called the **I/O APIC**, short for **I/O Advanced Programmable Interrupt Controller**, on x86 systems.
- The kernel documentation can be found here:
  - https://www.kernel.org/doc/html/latest/x86/i386/IO-APIC.html#io-apic
- It can also be a **Generic Interrupt Controller (GIC)** on ARM (Advanced RISC — Reduced Instruction Set Computer) machines.

- The PIC has one line to the CPU's interrupt pin. Onboard peripherals capable of asserting interrupts will have an **IRQ (Interrupt Request)** line to the PIC.
- IRQ is the common abbreviated term for **Interrupt Request**; it denotes the interrupt line (or lines) allocated to a peripheral device.

## Mental Model — USB, HID, and the Input Subsystem

Here is a mental model that clarifies the different layers. You can see the position of HID in the hierarchy and where it interacts with the input system and `/dev/input`.

```text
             USB hardware
                  │
                  │ USB packets
                  ▼
        ┌────────────────────┐
        │ USB Host Controller│
        │    xHCI / EHCI     │
        └─────────┬──────────┘
                  │
                  ▼
        ┌────────────────────┐
        │      USB Core      │
        │                    │
        │ device enumeration │
        │ configuration      │
        │ interfaces         │
        │ driver matching    │
        └─────────┬──────────┘
                  │
                  │ USB interface
                  ▼
        ┌────────────────────┐
        │    HID Driver      │
        │      usbhid        │
        └─────────┬──────────┘
                  │
                  ▼
        ┌────────────────────┐
        │   HID subsystem    │
        └─────────┬──────────┘
                  │
                  ▼
        ┌────────────────────┐
        │  Input subsystem   │
        └─────────┬──────────┘
                  │
                  ▼
              /dev/input/
```

---

# 1. Step 1 — USB Device Enumeration

- After you plug in a USB device, the USB host controller notices that something happened and sends information to the USB subsystem so it can discover the new device.
- The USB Core performs a process called **enumeration**. It basically wants to know:

> **"Who are you?"**

- The device responds with descriptors containing information such as:
  - Vendor ID
  - Product ID
  - Device class
  - Configuration
  - Interfaces
  - Endpoints
  - HID descriptor

### Example

```text
VID      = 046d
PID      = c31c
Class    = HID
Subclass = Boot
Protocol = Keyboard
```

The USB Core is now aware that there is a USB device containing an interface that looks like a HID keyboard.

---

# 2. Step 2 — USB Interfaces

Another important concept is that USB devices can contain **multiple interfaces**.

Let's say you have a music system connected to your PC. You need an interface for the keys you press—for example, on a JBL box—to handle the volume, and you also need to send the data containing the music to it.

A device could contain the following:

```text
USB Device
│
├── Interface 0 → Keyboard
│
├── Interface 1 → Mouse
│
└── Interface 2 → Vendor-specific device
```

Different drivers can therefore bind to different interfaces.

For a normal keyboard, like in our case, the interface is described like this:

```text
USB device
    │
    └── USB interface
             │
             ▼
         usbhid
```

---

# 3. Step 3 — USB Driver Matching

## How Does the USB Core Know Which Driver Should Handle the Device?

This is where the **USB ID table** comes in.

A USB driver can tell the kernel:

```c
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

So you are saying:

```text
My driver supports:

Class    = HID
Subclass = Boot
Protocol = Keyboard
```

The USB Core has a driver/device matching mechanism. It compares a new USB interface against the available drivers:

```text
New USB interface
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
```

When a driver matches, the kernel calls the driver's `probe()` function.

You will find the relevant code in the kernel files, where you will see the `MODULE_DEVICE_TABLE`.

### Inspect the `usbhid` Driver

```bash
grep -A 100 "static const struct usb_device_id hid_usb_ids\[\]" 	/lib/modules/$(uname -r)/build/drivers/hid/usbhid/hid-core.c
```

A relevant section looks like:

```c
static const struct usb_device_id hid_usb_ids[] = {
	{
		.match_flags = USB_DEVICE_ID_MATCH_INT_CLASS,
		.bInterfaceClass = USB_INTERFACE_CLASS_HID
	},
	{ }						/* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, hid_usb_ids);

static struct usb_driver hid_driver = {
	.name = "usbhid",
	.probe = usbhid_probe,
	.disconnect = usbhid_disconnect,
	.suspend = pm_ptr(hid_suspend),
	.resume = pm_ptr(hid_resume),
	.reset_resume = hid_reset_resume,
	.pre_reset = hid_pre_reset,
	.post_reset = hid_post_reset,
	.id_table = hid_usb_ids,
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

---

# 4. Step 4 — Communication Between USB Core and HID

## How Does the Communication Between USB Core and HID Work?

The USB Core provides the HID driver with abstractions such as:

```c
struct usb_device
struct usb_interface
struct usb_driver
struct urb
```

The HID driver doesn't need to manually manipulate the USB hardware.

Instead, it tells the USB Core:

> **"Send this USB request."**

The USB Core then handles the lower-level USB machinery.

---

# 5. Step 5 — USB Request Blocks (URBs)

One of the most important mechanisms to understand is the **URB**:

> **USB Request Block**

Think of a URB as a kernel object representing:

> **"I want to perform this USB transfer."**

For example, the HID driver can create an interrupt URB.

Conceptually:

```text
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

When the keyboard sends data back:

```text
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

---

# 6. Step 6 — What Does the Keyboard Actually Send?

- When you press `A`, the keyboard doesn't send the character `'a'` directly.
- It sends a **HID report**, like:

```text
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

- The HID driver receives that report.
- It interprets it according to the HID report descriptor.
- Then it passes the resulting information into the Linux HID/input layers.

---

# 7. Step 7 — USB Interrupt Transfers vs. Hardware Interrupts

- USB keyboards use what USB calls an **interrupt transfer**.
- But that does **not** mean:

```text
Keyboard
   │
   │ hardware IRQ
   ▼
Your driver
```

Instead, an interrupt USB transfer is a **USB transfer type**.

The actual hardware interrupt might look more like:

```text
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

> **USB interrupt transfer ≠ Linux hardware interrupt handler.**

This distinction is very important when reading Billimoria's material about interrupts.

---

# 8. Where Does `usbhid` Fit?

Linux has a USB HID driver commonly referred to as:

```text
usbhid
```

It sits between the USB subsystem and the generic HID subsystem:

```text
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
```

So `usbhid` is effectively translating:

```text
USB transfers
      ↓
HID reports
      ↓
Linux HID events
```

---

# 9. Where Your Assignment 04 Module Fits

This is where things get interesting.

Your assignment doesn't necessarily require you to replace `usbhid`.

Instead, you can have your own module get automatically loaded when the keyboard appears.

The chain becomes:

```text
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
```

That's different from your module being the keyboard's HID driver.

---

# 10. The Distinction Your Assignment Wants You to Understand

There are actually **two separate problems**.

## Problem A — Automatic Module Loading

```text
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
```

## Problem B — Actually Writing a USB Keyboard Driver

```text
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
```

Assignment 04 primarily asks for **Problem A**.

Chapter 14 of *Linux Device Drivers, 3rd Edition* helps you understand **Problem B**, which explains why the assignment points you there.

And Billimoria's book helps you understand the kernel-side concepts underneath this, such as:

- Module
- Driver
- Interrupts
- Workqueues
- Synchronisation
- Character devices

---

# The Most Useful Mental Model

Keep these four layers separate in your head:

```text
┌──────────────────────────────┐
│            udev              │
│        userspace hotplug     │
└──────────────┬───────────────┘
               │
             uevent
               │
┌──────────────▼───────────────┐
│          USB Core            │
│                              │
│ enumeration                  │
│ device/driver matching       │
│ USB interfaces               │
│ URBs                         │
└──────────────┬───────────────┘
               │
          USB interface
               │
┌──────────────▼───────────────┐
│           usbhid             │
│                              │
│      USB ↔ HID translation   │
└──────────────┬───────────────┘
               │
            HID report
               │
┌──────────────▼───────────────┐
│      HID / Input subsystem    │
└──────────────────────────────┘
```

---

# What We Should Do Next

For the assignment, the next step should be to trace one real keyboard plug-in on the Linux machine with commands such as:

```bash
udevadm monitor
udevadm info
lsusb -v
modprobe -c
```

That will let you see the exact:

```text
uevent → modalias → udev → modprobe
```

path rather than just learning it theoretically.
