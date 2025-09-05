#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/jiffies.h>

static struct hrtimer my_hrtimer;
u64 start_t;

/* Timer handler */
static enum hrtimer_restart test_hrtimer_handler(struct hrtimer *timer)
{
    u64 now_t = jiffies;
    printk(KERN_INFO "High-res timer fired! Elapsed = %u ms\n",
           jiffies_to_msecs(now_t - start_t));

    // Do not restart timer (one-shot)
    return HRTIMER_NORESTART;
}

/* Init function (called when module is loaded) */
static int __init ModuleInit(void)
{
    printk(KERN_INFO "Hello, Kernel! Starting high-res timer...\n");

    /* Init high-resolution timer */
    hrtimer_init(&my_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_hrtimer.function = &test_hrtimer_handler;

    /* Save start time */
    start_t = jiffies;

    /* Start timer: 100 ms */
    hrtimer_start(&my_hrtimer, ms_to_ktime(100), HRTIMER_MODE_REL);

    return 0;
}

/* Exit function (called when module is removed) */
static void __exit ModuleExit(void)
{
    hrtimer_cancel(&my_hrtimer);
    printk(KERN_INFO "Goodbye, Kernel! High-res timer canceled.\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Muhammad Ali Hussnain");
MODULE_DESCRIPTION("A simple Linux kernel module using high-resolution timers");
MODULE_VERSION("1.0");
