#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h> // for copy_to_user, copy_from_user

#define DEVICE_NAME "hello_cdev"
#define BUFFER_SIZE 64

static char device_buffer[BUFFER_SIZE];
static int major;

// -------------------- READ --------------------
static ssize_t hello_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    printk(KERN_INFO "hello_cdev: read requested (len=%zu, offset=%lld)\n", len, *offset);

    if (*offset >= BUFFER_SIZE) {
        printk(KERN_INFO "hello_cdev: end of buffer reached\n");
        return 0; // EOF
    }

    if (len > BUFFER_SIZE - *offset)
        len = BUFFER_SIZE - *offset;

    if (copy_to_user(buf, device_buffer + *offset, len)) {
        printk(KERN_ERR "hello_cdev: failed to copy data to user\n");
        return -EFAULT;
    }

    *offset += len;
    printk(KERN_INFO "hello_cdev: read %zu bytes, new offset=%lld\n", len, *offset);

    return len;
}

// -------------------- WRITE --------------------
static ssize_t hello_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    printk(KERN_INFO "hello_cdev: write requested (len=%zu, offset=%lld)\n", len, *offset);

    if (*offset >= BUFFER_SIZE) {
        printk(KERN_WARNING "hello_cdev: no space left in buffer\n");
        return -ENOSPC; // No space left
    }

    if (len > BUFFER_SIZE - *offset)
        len = BUFFER_SIZE - *offset;

    if (copy_from_user(device_buffer + *offset, buf, len)) {
        printk(KERN_ERR "hello_cdev: failed to copy data from user\n");
        return -EFAULT;
    }

    *offset += len;
    printk(KERN_INFO "hello_cdev: wrote %zu bytes, new offset=%lld\n", len, *offset);

    return len;
}

// -------------------- OPEN --------------------
static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "hello_cdev: device opened (major=%d, minor=%d)\n",
           imajor(inode), iminor(inode));
    return 0;
}

// -------------------- RELEASE --------------------
static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "hello_cdev: device closed (major=%d, minor=%d)\n",
           imajor(inode), iminor(inode));
    return 0;
}

// -------------------- FILE OPS --------------------
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = hello_read,
    .write = hello_write,
};

// -------------------- INIT --------------------
static int __init hello_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "hello_cdev: failed to register character device\n");
        return major;
    }
    printk(KERN_INFO "hello_cdev: registered successfully with major number %d\n", major);
    return 0;
}

// -------------------- EXIT --------------------
static void __exit hello_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "hello_cdev: unregistered character device\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Ali Hussnain");
MODULE_DESCRIPTION("A simple character device driver with logging");
MODULE_VERSION("1.0");
