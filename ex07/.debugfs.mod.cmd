savedcmd_debugfs.mod := printf '%s\n'   debugfs.o | awk '!x[$$0]++ { print("./"$$0) }' > debugfs.mod
