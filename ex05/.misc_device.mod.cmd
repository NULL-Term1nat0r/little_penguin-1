savedcmd_misc_device.mod := printf '%s\n'   misc_device.o | awk '!x[$$0]++ { print("./"$$0) }' > misc_device.mod
