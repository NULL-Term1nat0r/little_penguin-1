savedcmd_my_mount.mod := printf '%s\n'   my_mount.o | awk '!x[$$0]++ { print("./"$$0) }' > my_mount.mod
