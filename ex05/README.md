# Assignment 05
Nice job getting module auto-loading to work. Those hotplug macros can be tricky, but
mastering them is a useful skill, especially in real kernel development.
Speaking of real development—let’s write some proper code.
To Do
• Take the kernel module you wrote for Assignment 01, and modify it to become
a misc character device driver. The misc interface is a super simple way to create a character device without getting caught up in the mess of sysfs and device
registration. Trust me, it’s way easier this way!
• The misc device should be created with a dynamic minor number—no need to go
crazy trying to reserve a static one for your test module. That would be a hassle
for no reason.
• Implement both the read and write operations for the misc device.
• The misc device node should appear in /dev/fortytwo.
• When the device is read from, it should return your student login to the caller.
• When written to, the input data should be compared to your student login. If it
matches, return a successful write response. If not, return an "invalid value" error.
• Register the misc device when your module is loaded, and unregister it when it’s
unloaded.
Turn In
• Your updated code.
• Some form of proof.

# step 1 Research - What is a misc character device ?

- misc stands for miscancellaneous device. It's a a single character driver grouping in operating systems like Linux that aggregate unique or simple hard/software hooks sharing a single major device number (10). It's bypassing the need to request a distinct major number for every minor utility.

core characteristics for misc devices:
- shared major number: all miscancellaneous devices share the fixed major number 10'
- simplified registration: handled via the API fucntion misc_register() insted of full character device setups.
- dynamic minor numbers: kernels can automatically assign minor number using MISC_DYNAMIC_MINOR

examples:
- random number generators:  interfaces like /dev/random or /dev/urandom
- axiomatic/virtual sensors: speciaised minor components or debugging interfaces that do not really netaly fit standard bloc or network categories