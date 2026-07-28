savedcmd_detect_hotplug_event.mod := printf '%s\n'   detect_hotplug_event.o | awk '!x[$$0]++ { print("./"$$0) }' > detect_hotplug_event.mod
