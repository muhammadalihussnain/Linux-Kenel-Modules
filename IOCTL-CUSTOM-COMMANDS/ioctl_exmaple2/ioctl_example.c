#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define DEVICE_NAME "ioctl_example"
#define MY_IOCTL_MAGIC 'M'

// Commands
//This IOCTL command means user-space will pass an int32_t to the kernel driver.
#define WR_VALUE   _IOW(MY_IOCTL_MAGIC, 0, int32_t) 

//This IOCTL command means the kernel driver will pass an int32_t back to user-space.
#define RD_VALUE   _IOR(MY_IOCTL_MAGIC, 1, int32_t) 

//This IOCTL command means user-space will pass a struct mystruct to the kernel driver.
#define GREETER    _IOW(MY_IOCTL_MAGIC, 2, struct mystruct) 
int32_t answer = 42;  // Global variable for reading/writing

//below is the structure which i want to pass from user space to kernel space
struct mystruct {
    int repeat;
    char name[32];
};

// -------- ioctl handler --------
static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct mystruct test;

    switch (cmd) {
    case WR_VALUE:

    // Copy an integer value from user space into the kernel variable 'answer'.
    // This allows the user process to update the kernel's stored value.
    // the user space arguemnt is being copied into the kernel space variable answer
        if (copy_from_user(&answer, (int32_t *)arg, sizeof(answer))) {
            printk(KERN_ERR "ioctl_example: Error copying data from user\n");
            return -EFAULT;
        }
        printk(KERN_INFO "ioctl_example: Updated answer to %d\n", answer);
        break;

    case RD_VALUE:

    // Copy the kernel variable 'answer' to user space.
    // This allows the user process to read the value currently stored in the kernel.
        if (copy_to_user((int32_t *)arg, &answer, sizeof(answer))) {
            printk(KERN_ERR "ioctl_example: Error copying data to user\n");
            return -EFAULT;
        }
        printk(KERN_INFO "ioctl_example: The answer was copied to user\n");
        break;
    // Copy the user-provided struct (mystruct) from user space to kernel space.
    // This ensures the kernel has a safe local copy of 'repeat' and 'name'
    // fields that the user passed through ioctl.
    case GREETER:
        if (copy_from_user(&test, (struct mystruct *)arg, sizeof(test))) {
            printk(KERN_ERR "ioctl_example: Error copying struct from user\n");
            return -EFAULT;
        }
        printk(KERN_INFO "ioctl_example: %d greets to %s\n", test.repeat, test.name);
        break;

    default:
        return -EINVAL;
    }
    return 0;
}

// -------- File ops --------
static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "ioctl_example: device opened\n");
    return 0;
}
static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "ioctl_example: device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    //to handle ioctl commands we use .unlocked_ioctl in file_operations structure
    .unlocked_ioctl = my_ioctl,
};

static int major;
static int __init ioctl_init(void) {
    //in the function register_chrdev, we are registering our device with the kernel
    //0 means we want the kernel to allocate a major number dynamically     if we pass a specific major number, the kernel will try to use that number and 
    // best practice is to pass 0 and let the kernel allocate a free major number for us.
    //&fops is a pointer to the file_operations structure that defines how our device behaves
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ERR "ioctl_example: failed to register device\n");
        return major;
    }
    printk(KERN_INFO "ioctl_example: module loaded, major = %d\n", major);
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, major);
    return 0;
}
static void __exit ioctl_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "ioctl_example: module unloaded\n");
}

module_init(ioctl_init);
module_exit(ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Ali");
MODULE_DESCRIPTION("IOCTL Example with int and struct exchange");
