# Assignment 06
Great job with the misc device driver—pretty clean and simple, right?
Just when you thought this challenge was only about coding in the kernel, we’re throwing
it back to a previous topic. That’s right: building kernels again!
This reflects real life. Kernel developers often spend more time rebuilding than writing new code. It’s not glamorous, but it’s a vital skill.
To Do
• Download the latest linux-next kernel. It changes daily, so just use the most recent version. Build it and boot it.
Turn In
• Kernel boot log.
What is the linux-next kernel?
That’s part of the challenge.
For a hint, check the excellent documentation on the kernel development process in
Documentation/development-process/ within the kernel source tree. It’s a good read—you’ll
learn a lot about how kernel developers work.
For a hint, you should read the excellent documentation about how the Linux kernel
is developed in Documentation/development-process/ in the kernel source itself.
It’s a great read and should tell you everything you never wanted to know about what
Linux kernel developers do and how they do it.

# step 1

- Firts we have to get the kenrel_next tree from the kernel.org website:
https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git

- we can clone it into our /usr/src folder
```bash
git clone --depth=1 https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git
```

- It is better to use depth for a maximum of 3 to avoid cloning all commits from the history throughout it's development in order to save disk usage.

After that  we build the kernel. You can copy your old config file from a previous kernel built into your current kernek source folder when you dont wanna use the default configuration.

# step 2 What os the kernel-next tree ?

It is a daily updated testing and integration repository. It helps to combine disparate subsystem changes for testing compiler issues, bugs and code conflicts ebfore the code enters Linus Torvalds main mainline tree.

It collects code from hundred of individual subsystem trees like networking drives and file systems.
- It is recreated daily and build fresh every day by test maintainers to reflect the absolute latest state of upcoming development.
- It works as an early warning system  that exposes code and APi breakages between different developer groups weeks before official merging.

## what is it for ?

- integration testing: it lets maintainers see if different patches break each other when combined
- automated testing: feeds continous intergration systems and build bots to catch bugs early.
- developers have a chance to catch early developments ahead of time


## how is the linux development organised ?

- there is a herachy n linus development. we have subsystem maintainers that maintain a certain part of tge linux kernel like networking subsystem, graphic subsystem etc.
-during the test phase any developer has the chance to write a patch and send it to a certain mailing list by a subsystem maintainer. there is even a command to find the mailing list for a certain patch or foir a certain file or subssystem maintainer: 
```bash
scripts/get_maintainer.pl -f drivers/net/ethernet/intel/e1000/e1000_main.c
scripts/get_maintainer.pl --nom --nol -f drivers/usb/core/hub.c
bash-5.3# cd /usr/src/
bash-5.3# ls
linux  linux-next
bash-5.3# cd linux
bash-5.3# ls
0001-Makefile-Add-thor_kernel-to-EXTRAVERSION.patch  fs           Makefile                 security
arch                                                 include      mm                       sound
block                                                init         modules.builtin          System.map
built-in.a                                           io_uring     modules.builtin.modinfo  tools
built-in-fixup.a                                     ipc          modules.order            usr
certs                                                Kbuild       Module.symvers           virt
COPYING                                              Kconfig      net                      vmlinux
CREDITS                                              kernel       README                   vmlinux.a
crypto                                               lib          rust                     vmlinux.o
Documentation                                        LICENSES     samples                  vmlinux.unstripped
drivers                                              MAINTAINERS  scripts
bash-5.3# grep -A 5 "USB SUBSYSTEM" MAINTAINERS
USB SUBSYSTEM
M:      Greg Kroah-Hartman <gregkh@linuxfoundation.org>
L:      linux-usb@vger.kernel.org
S:      Supported
W:      http://www.linux-usb.org
T:      git git://git.kernel.org/pub/scm/linux/kernel/git/gregkh/usb.git
```

Also you can checkout the Documentation inside your kernel src-folder:
/usr/src/linux-next/Documentation/process
``bash 
bash-5.3# ls | grep maintainer
maintainer-handbooks.rst
maintainer-kvm-x86.rst
maintainer-netdev.rst
maintainer-pgp-guide.rst
maintainer-soc-clean-dts.rst
maintainer-soc.rst
maintainers.rst
maintainer-tip.rst
```

- there is also a file called submitting-patches.rst if you want know more about handing patches in yourself
- each subssystem maintainer collects the patches and decides on certain aspects like code quality, security etc whether the patch makes it to Linus Torvalds. Linus Torvalds then has the last say whether a patch makes it into the kernel