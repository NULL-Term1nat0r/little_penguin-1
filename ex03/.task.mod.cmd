savedcmd_task.mod := printf '%s\n'   task.o | awk '!x[$$0]++ { print("./"$$0) }' > task.mod
