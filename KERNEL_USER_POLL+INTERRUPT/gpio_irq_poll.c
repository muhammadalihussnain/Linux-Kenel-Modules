#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/poll.h>
#include <linux/wait.h>

#define GPIO_BUTTON 17        // Example: GPIO17 on Raspberry Pi
#define DEVICE_NAME "irqpoll"
#define DEVICE_MAJOR 64



/*
To bind a button’s hardware interrupt to our driver, we first configure the GPIO pin by requesting it from
the kernel and setting it as an input (so the button’s press/release can be detected). Then we use gpio_to_irq()
to translate that GPIO pin into a kernel IRQ number (irq_number). This IRQ number uniquely identifies the
hardware event inside the kernel. Next, we pass it to request_irq(), which links that IRQ line with our 
interrupt handler function. Whenever the button state changes, the kernel triggers our handler, allowing
us to update flags or wake waiting processes. Finally, we release both the IRQ using free_irq() and the 
GPIO pin using gpio_free() when unloading the module.*/
static int irq_number;

/*
irq_ready is a flag variable to indicate whether an interrupt has occurred and data is ready to be read.
When an interrupt occurs, the interrupt handler sets irq_ready to 1, indicating that data is available.
When a process calls the poll function to check for data availability, it checks the value of irq_ready.
If irq_ready is 1, the poll function resets it to 0 (indicating that the data has been consumed) and returns POLLIN to 
signal that data is ready to be read.
If irq_ready is 0, the poll function returns 0, indicating that no data is currently available.
This mechanism allows the poll function to communicate the availability of data to user-space processes
that are waiting for events on the device.
*/
static int irq_ready = 0;
static wait_queue_head_t waitqueue;

static irq_handler_t gpio_irq_poll_handler(unsigned int irq, void *dev_id,
                                           struct pt_regs *regs)
{
    printk(KERN_INFO "gpio_irq_poll: Button interrupt detected!\n");
    irq_ready = 1;
    wake_up(&waitqueue);  // wake processes in poll()
    return (irq_handler_t)IRQ_HANDLED;
}

static unsigned int my_poll(struct file *file, poll_table *wait)
{
    poll_wait(file, &waitqueue, wait);

    if (irq_ready == 1) {
        irq_ready = 0;
        return POLLIN;   // Data ready
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .poll  = my_poll,
};

static int __init ModuleInit(void)
{
    int result;

    printk(KERN_INFO "gpio_irq_poll: Initializing module...\n");

    /*
 * When this module is loaded, the kernel first runs the ModuleInit() function.
 *
 * Inside ModuleInit():
 *  - Sets up a waitqueue.
 *  - Validates and requests control of GPIO pin 17.
 *  - Configures GPIO 17 as input.
 *  - Exports it for visibility in sysfs.
 *
 * Why waitqueue?
 *  - Processes calling poll() may need to sleep until an event occurs.
 *  - The waitqueue provides the mechanism for the kernel to block
 *    those processes efficiently and wake them later.
 *
 * Implementation detail:
 *  - A waitqueue is set up by declaring a wait_queue_head_t, e.g.:
 *        DECLARE_WAIT_QUEUE_HEAD(my_queue);
 *    or:
 *        init_waitqueue_head(&my_queue);
 *  - This object acts as the anchor point for all sleeping processes.
 */

    init_waitqueue_head(&waitqueue);

    /*  
 * Validate that GPIO_BUTTON (GPIO 17) is a valid GPIO number supported 
 * by the platform. If the number is invalid, print an error message 
 * and return -ENODEV to indicate that the device is not available.  
 */
    if (!gpio_is_valid(GPIO_BUTTON)) {
        printk(KERN_ERR "Invalid GPIO %d\n", GPIO_BUTTON);
        return -ENODEV;
    }

    gpio_request(GPIO_BUTTON, "sysfs");
    gpio_direction_input(GPIO_BUTTON);
    gpio_export(GPIO_BUTTON, false);

    /* 
     * Get the Linux kernel's IRQ number for our GPIO pin.
     * The kernel translates the physical GPIO pin (e.g., 17) into its
     * corresponding internal interrupt request number (irq_number).
     * This number is needed to register our interrupt handler.
     */
    irq_number = gpio_to_irq(GPIO_BUTTON);
    printk(KERN_INFO "gpio_irq_poll: GPIO %d mapped to IRQ %d\n",
           GPIO_BUTTON, irq_number);
    
    /*
     * Register our interrupt handler function with the kernel.
     * We tell the kernel: "When the interrupt identified by 'irq_number'
     * occurs (e.g., on the rising/falling edge of the signal), please call
     * our function 'gpio_irq_poll_handler'."
     */
    result = request_irq(irq_number,
                         (irq_handler_t)gpio_irq_poll_handler,
                         IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                         "gpio_irq_poll",
                         NULL);
    if (result) {
        printk(KERN_ERR "gpio_irq_poll: Cannot request IRQ\n");
        gpio_unexport(GPIO_BUTTON);
        gpio_free(GPIO_BUTTON);
        return result;
    }

    // Register character device
    result = register_chrdev(DEVICE_MAJOR, DEVICE_NAME, &fops);
    if (result < 0) {
        printk(KERN_ERR "gpio_irq_poll: Failed to register device\n");
        free_irq(irq_number, NULL);
        gpio_unexport(GPIO_BUTTON);
        gpio_free(GPIO_BUTTON);
        return result;
    }

    printk(KERN_INFO "gpio_irq_poll: Module loaded. Device: /dev/%s (major=%d)\n",
           DEVICE_NAME, DEVICE_MAJOR);
    return 0;
}

static void __exit ModuleExit(void)
{
    free_irq(irq_number, NULL);
    gpio_unexport(GPIO_BUTTON);
    gpio_free(GPIO_BUTTON);
    unregister_chrdev(DEVICE_MAJOR, DEVICE_NAME);

    printk(KERN_INFO "gpio_irq_poll: Module unloaded\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Based on Johannes tutorial");
MODULE_DESCRIPTION("GPIO Poll Example with IRQ + waitqueue");
