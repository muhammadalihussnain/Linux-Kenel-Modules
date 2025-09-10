#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

/* Variable contains pin number of interrupt controller to which GPIO 17 is mapped */
static unsigned int irq_number;

/* Interrupt Service Routine — called when interrupt is triggered */
static irq_handler_t gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
    printk(KERN_INFO "gpio_irq: Interrupt was triggered and ISR was called!\n");
    return (irq_handler_t) IRQ_HANDLED;
}

/* Module init function */
static int __init ModuleInit(void)
{
    printk(KERN_INFO "gpio_irq: Loading module...\n");

    /* Setup GPIO */
    if (gpio_request(17, "rpi-gpio-17")) {
        printk(KERN_ERR "Error: Cannot allocate GPIO 17\n");
        return -1;
    }

    /* Set GPIO 17 direction to input */
    if (gpio_direction_input(17)) {
        printk(KERN_ERR "Error: Cannot set GPIO 17 to input\n");
        gpio_free(17);
        return -1;
    }

    /* Setup the interrupt */
    irq_number = gpio_to_irq(17);

    if (request_irq(irq_number,
                    (irq_handler_t) gpio_irq_handler,
                    IRQF_TRIGGER_RISING,
                    "my_gpio_irq",
                    NULL)) {
        printk(KERN_ERR "Error: Cannot request interrupt nr. %d\n", irq_number);
        gpio_free(17);
        return -1;
    }

    printk(KERN_INFO "Done!\n");
    printk(KERN_INFO "GPIO 17 is mapped to IRQ Nr.: %d\n", irq_number);

    return 0;
}

/* Module exit function */
static void __exit ModuleExit(void)
{
    free_irq(irq_number, NULL);
    gpio_free(17);
    printk(KERN_INFO "gpio_irq: Module unloaded.\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Example from tutorial");
MODULE_DESCRIPTION("Simple Linux Kernel Module for GPIO Interrupt");
MODULE_VERSION("1.0");  
