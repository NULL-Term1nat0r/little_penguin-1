savedcmd_fix.mod := printf '%s\n'   fix.o | awk '!x[$$0]++ { print("./"$$0) }' > fix.mod
