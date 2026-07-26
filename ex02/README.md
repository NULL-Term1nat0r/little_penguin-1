## Assignment 02

# To Do
• Take the kernel Git tree from Assignment 00 and modify the Makefile to change
the EXTRAVERSION field. Do it so the running kernel, after modifying, rebuilding,
and rebooting, includes "-thor_kernel" in its version string.
# Turn In
• Kernel boot log.
• A patch to the original Makefile, following Linux submission standards (Documentation/SubmittingPatches).

# change .config in your kernel source folder

You can choose the graphical approach by calling 
```
make menuconfig
```
 Here you go to General setup select it and type your value for EXTRAVERSION naming

# step 1:

![alt text](<menuconfig_general_settings.png>)

# step 2:

![alt text](menuconfig_general_setup_settings.png)

# step 3:

![alt text](menuconfig_general_setup_extraversion_settings.png)

# install new with new kernel configuration

after saving the .config file we run:
```
make -j($nproc)
sudo make modules install
sudop make install
```

# update boot loader config

Now the kernel is installed with new extraversion tag. But we are not done yet. we also need to reconfigure our boot loader entries. In my case i use grub bootoader. Unless you have custom rules after doing
```
sudo grub-mkconfig
```
you should see the next entry in bootlaoder menu on start. Otherwise grub will choose the kernel with the hightest version automatically. After applying those changes we can reboot
```
reboot
```

![alt text](bootloader_menu_entries.png)

![alt text](dmesg_log.png)