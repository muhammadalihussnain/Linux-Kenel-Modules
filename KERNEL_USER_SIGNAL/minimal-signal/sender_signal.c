#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/workqueue.h>
#include <linux/signal.h>         // for kernel_siginfo
#include <linux/kdev_t.h>
#include <linux/cdev.h>

#define DEVICE_NAME "sigdev"
#define CLASS_NAME "sigclass"

static int pid = -1;
static int major;                  // dynamic major
static struct class *sig_class;
static struct device *sig_device;

// ioctl command number
#define IOCTL_SET_PID _IOW('a', 'a', int32_t *)

// ioctl handler
static long sigdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    if (cmd == IOCTL_SET_PID) {
        if (copy_from_user(&pid, (int32_t *)arg, sizeof(pid))) {
            return -EFAULT;
        }
        printk(KERN_INFO "Kernel: Registered user process PID = %d\n", pid);
    }
    return 0;
}

static struct file_operations fops = {
    .unlocked_ioctl = sigdev_ioctl,
};

// Send signal to user process
static void send_signal_to_user(void)
{
    struct task_struct *task;

    if (pid <= 0) {
        printk(KERN_INFO "Kernel: No PID registered yet. Will retry...\n");
        return;
    }

    rcu_read_lock();
    task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (task) {
        struct kernel_siginfo info;

        memset(&info, 0, sizeof(struct kernel_siginfo));
        info.si_signo = SIGUSR1;
        info.si_code  = SI_QUEUE;
        info.si_int   = 1234;

        printk(KERN_INFO "Kernel: Sending SIGUSR1 to PID %d\n", pid);
        send_sig_info(SIGUSR1, &info, task);
    } else {
        printk(KERN_WARNING "Kernel: PID %d not found\n", pid);
    }
    rcu_read_unlock();
}

static struct delayed_work my_work;

// Workqueue handler (repeats every 5 seconds)
static void work_handler(struct work_struct *work)
{
    send_signal_to_user();
    schedule_delayed_work(&my_work, 5 * HZ); // reschedule
}

static int __init sigdev_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);  // 0 → let kernel pick a free major
    if (major < 0) {
        printk(KERN_ALERT "Kernel: Failed to register device\n");
        return major;
    }

    sig_class = class_create(DEVICE_NAME);   
    if (IS_ERR(sig_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Kernel: Failed to create class\n");
        return PTR_ERR(sig_class);
    }

    sig_device = device_create(sig_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(sig_device)) {
        class_destroy(sig_class);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Kernel: Failed to create device\n");
        return PTR_ERR(sig_device);
    }

    printk(KERN_INFO "Kernel: Module loaded with major number %d. Use ioctl to register PID.\n", major);

    // Schedule first work
    INIT_DELAYED_WORK(&my_work, work_handler);
    schedule_delayed_work(&my_work, 5 * HZ);

    return 0;
}

static void __exit sigdev_exit(void)
{
    cancel_delayed_work_sync(&my_work);
    device_destroy(sig_class, MKDEV(major, 0));
    class_destroy(sig_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Kernel: Module unloaded\n");
}

module_init(sigdev_init);
module_exit(sigdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Ali Hussnain");
MODULE_DESCRIPTION("Kernel module to send signals to user-space process periodically");
MODULE_VERSION("1.0");
