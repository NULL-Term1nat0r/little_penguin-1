# Assignment 07
We’ll return to the linux-next kernel in a later task, so don’t delete that directory—you’ll
need it again. But for now, enough kernel building. Let’s write more code!
This task is similar to Assignment 05, where you created a misc device. This time,
we’ll explore a different interface between user and kernel: debugfs.
Rumor has it the creator of debugfs said the only rule is: "There are no rules." Let’s test
that theory.
Your distribution should mount debugfs at /sys/kernel/debug/. If it doesn’t, mount
it with:
mount -t debugfs none /sys/kernel/debug/
Make sure it is enabled in your kernel with the CONFIG_DEBUG_FS option, as you will need
it for this task.
To Do
• Modify the module from Assignment 01 to create a debugfs subdirectory named
fortytwo.
• In this directory, create three virtual files: id, jiffies, and foo.
• id: Behaves exactly as in Assignment 05. It must be readable and writable by all
users.
• jiffies: Read-only by any user. When read, it should return the current value of
the kernel jiffies timer.
• foo: Writable only by root; readable by everyone. Data written should be stored
(up to one page). Reading should return the stored data. Implement proper locking
to handle concurrent read/write operations.
• When the module is unloaded, all debugfs files must be cleaned up and any allocated memory freed.
• Note: The debug directory must be globally readable. Since there’s no option for
that, use good old chown.
14
Little Penguin Linux Kernel Development
Turn In
• Your code.
• Proof that the module works as expected.

# Research about debugfs

## what is debugfs ?

<linux/debugfs.h>

# create a debug directory
struct dentry *debugfs_create_dir(const char *name, struct dentry *parent);

#create a file in the debug directory
# mode describes access permissions
# fops is a set of fileoperations whoch implement the files behavior. At a minium the read() and or write() operations should be provided
# return value will be  a dentry pointer to the created file, ERR_PTR(-EEROR) on error or ERR_PTR(_ENODEV) if debugfs support is missing
# Create a file with aninitial size, the following function can be used instead:
struct dentry *debugfs_create_file(const char *name, umode_t mode,
                                   struct dentry *parent, void *data,
                                   const struct file_operations *fops);

# different functions for different integer types:

void debugfs_create_u8(const char *name, umode_t mode,
                       struct dentry *parent, u8 *value);
void debugfs_create_u16(const char *name, umode_t mode,
                        struct dentry *parent, u16 *value);
void debugfs_create_u32(const char *name, umode_t mode,
                        struct dentry *parent, u32 *value);
void debugfs_create_u64(const char *name, umode_t mode,
                        struct dentry *parent, u64 *value);

# more helper functions for files:

void debugfs_create_x8(const char *name, umode_t mode,
                       struct dentry *parent, u8 *value);
void debugfs_create_x16(const char *name, umode_t mode,
                        struct dentry *parent, u16 *value);
void debugfs_create_x32(const char *name, umode_t mode,
                        struct dentry *parent, u32 *value);
void debugfs_create_x64(const char *name, umode_t mode,
                        struct dentry *parent, u64 *value);

# if a file should not be written to simplen chnage the umode_t node value accrodingly

struct debugfs_blob_wrapper {
    void *data;
    unsigned long size;
};

struct dentry *debugfs_create_blob(const char *name, umode_t mode,
                                   struct dentry *parent,
                                   struct debugfs_blob_wrapper *blob);

