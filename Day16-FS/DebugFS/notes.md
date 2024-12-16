## Understanding the DebugFS

- Debugfs exists as a simple way for kernel developers to make information available to user space.
- Unlike others, debugfs has no rules at all, Developers can put any information they want there.
- [DebugFS Linux Kernel Documentation](https://docs.kernel.org/filesystems/debugfs.html#debugfs)

## Questions/Doubts
##### How does the DebugFS work exactly?
- The <linux/debugfs.h> header contains dependency on <linux/fs.h> file and also a header file, <linux/seq_file.h>

##### What is seq_file?
- Write your answer here!

##### What is dentry? Understand what exactly is dentry, and how it is working with debugFS?
- The struct dentry itself contains the pointer to the ```struct dentry *d_parent;   /* parent directory */```
- Also, contains the pointer to the ```struct inode *d_inode;```
and pointer to ```struct super_block *d_sb;   /* The root of the dentry tree */```

##### What does the API ```debugfs_create_dir()``` do?
- This call, if successful, will make a directory called name underneath the indicated parent directory. If parent is **NULL**, the directory will be created in the debugfs root.
- On **SUCCESS**, the return value is a struct dentry pointer which can be used to create files in the directory

##### Why is the user writing to the debugFS instead of my device driver writing the debug information to the debug file?   