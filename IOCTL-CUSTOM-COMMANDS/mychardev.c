#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>       // for struct file_operations
#include <linux/uaccess.h>  // for copy_to_user, copy_from_user
#include "mychardev.h"

static int counter = 0;
static int major;

// ---------- file operations ----------
int my_open(struct inode *inode, struct file *file) {
    int minor = iminor(inode);
    printk(KERN_INFO "Device major: %d, minor: %d opened\n", imajor(inode), minor);
    return 0;
}

int my_release(struct inode *inode, struct file *file) {
    int minor = iminor(inode);
    printk(KERN_INFO "Device major: %d, minor: %d released\n", imajor(inode), minor);
    return 0;
}

// Minimal ioctl implementation
long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_CMD_INCREMENT:
            counter++;
            printk(KERN_INFO "IOCTL: Increment counter -> %d\n", counter);
            break;
        case IOCTL_CMD_DECREMENT:
            counter--;
            printk(KERN_INFO "IOCTL: Decrement counter -> %d\n", counter);
            break;
        case IOCTL_CMD_RESET:
            counter = 0;
            printk(KERN_INFO "IOCTL: Reset counter -> %d\n", counter);
            break;
        default:
            printk(KERN_WARNING "IOCTL: Unknown command 0x%x\n", cmd);
            return -EINVAL;
    }
    return 0;
}

// file_operations struct
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .unlocked_ioctl = my_ioctl,
};

// module init
static int __init test_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register char device: %d\n", major);
        return major;
    }
    printk(KERN_INFO "Module loaded with major number %d\n", major);
    printk(KERN_INFO "Create device nodes using:\n");
    printk(KERN_INFO "  sudo mknod /dev/%s0 c %d 0\n", DEVICE_NAME, major);
    printk(KERN_INFO "  sudo mknod /dev/%s1 c %d 1\n", DEVICE_NAME, major);
    return 0;
}

// module exit
static void __exit test_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Module unloaded\n");
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Ali");
MODULE_DESCRIPTION("Minimal char device with ioctl custom commands");
