#ifndef MYCHARDEV_H
#define MYCHARDEV_H

#include <linux/ioctl.h>    // for ioctl macros

#define DEVICE_NAME "mychardev"
#define MY_IOCTL_MAGIC 'M'

// Minimal custom commands
#define IOCTL_CMD_INCREMENT _IO(MY_IOCTL_MAGIC, 0)
#define IOCTL_CMD_DECREMENT _IO(MY_IOCTL_MAGIC, 1)
#define IOCTL_CMD_RESET     _IO(MY_IOCTL_MAGIC, 2)

// Function prototypes
int my_open(struct inode *inode, struct file *file);
int my_release(struct inode *inode, struct file *file);
long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

#endif // MYCHARDEV_H
