savedcmd_task.ko := ld -r -m elf_x86_64 -z noexecstack --no-warn-rwx-segments --build-id=sha1  -T /usr/src/linux/scripts/module.lds -o task.ko task.o task.mod.o .module-common.o
