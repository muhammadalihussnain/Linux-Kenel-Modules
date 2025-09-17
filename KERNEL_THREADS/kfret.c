#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>   // for kthread functions
#include <linux/sched.h>     // for task_struct
#include <linux/delay.h>     // for msleep()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tutorial Example");
MODULE_DESCRIPTION("Simple example of using threads in a Linux kernel module");

/*
Global variables for threads
Every thread/process in Linux is represented by a task_struct structure.
The kthread_create function creates a thread but does not start it immediately.
To start the thread, we need to call wake_up_process on the returned task_struct pointer.
The kthread_run function combines these two steps: it creates and starts the thread in one call
Global variables for threads
*/ 
static struct task_struct *kthread1;
static struct task_struct *kthread2;
static int t1 = 1;
static int t2 = 2;

// Function executed by each thread
static int thread_function(void *thread_number)
{
    int counter = 0;
    int t_number = *(int *)thread_number;

    while (!kthread_should_stop()) {
        pr_info("kthread %d executed, counter = %d\n", t_number, counter++);
        msleep(t_number * 1000);   // sleep (1s for thread 1, 2s for thread 2)
    }

    pr_info("kthread %d finished execution\n", t_number);
    return 0;
}

// Module init
static int __init kfret_init(void)
{
    pr_info("Loading kfret module\n");

    // First way: kthread_create + wake_up_process
    kthread1 = kthread_create(thread_function, &t1, "kthread1");
    if (kthread1) {
        wake_up_process(kthread1);
        pr_info("kthread1 created and running with kthread create plus wakeup in 2 steps\n");
    } else {
        pr_err("kthread1 could not be created\n");
        return -1;
    }

    // Second way: kthread_run (create + run in one step)
    kthread2 = kthread_run(thread_function, &t2, "kthread2");
    if (kthread2) {
        pr_info("kthread2 created and running\ in one step \n");
    } else {
        pr_err("kthread2 could not be created\n");
        kthread_stop(kthread1); // cleanup thread1
        return -1;
    }

    pr_info("Both threads are running now\n");
    return 0;
}

// Module exit
static void __exit kfret_exit(void)
{
    if (kthread1)
        kthread_stop(kthread1);
    if (kthread2)
        kthread_stop(kthread2);

    pr_info("Stopped both threads, exiting module\n");
}

module_init(kfret_init);
module_exit(kfret_exit);
