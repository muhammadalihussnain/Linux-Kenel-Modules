#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>



static int major;

static int hello_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "hello_cdev: Device opened   with major %d and minor  %d\n",imajor(inode), iminor(inode));
    printk(KERN_INFO "file position is %s",file->f_pos==0?"at the beginning":"not at the beginning");
    printk(KERN_INFO "file flags are %d\n",file->f_flags);
    printk(KERN_INFO "file mode is %d\n",file->f_mode);
    return 0;
}

static int hello_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "hello_cdev: Device closed with major %d and minor %d\n ", imajor(inode), iminor(inode) );
    return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
};


static int __init hello_init(void) {
    major = register_chrdev(0, "hello_cdev", &fops);
    if (major < 0) {
        printk(KERN_ALERT "hello_cdev: Registering char device failed with %d\n", major);
        return major;
    }
    printk(KERN_INFO "hello_cdev: Registered char device with major number %d\n", major);
    return 0;
}

static void __exit hello_exit(void) {
    unregister_chrdev(major, "hello_cdev");
    printk(KERN_INFO "hello_cdev: Unregistered char device with major number %d\n", major);
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Ali Hussnain");
MODULE_DESCRIPTION("A simple Hello World character device");