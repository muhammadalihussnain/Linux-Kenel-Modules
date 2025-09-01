#include<linux/init.h>
#include<linux/module.h>

int hello_init(void)
{
    printk(KERN_ALERT "Hello, world\n");
    return 0;
}

void hello_exit(void)
{
    printk(KERN_ALERT "Goodbye, world\n");
}


module_init(hello_init);
module_exit(hello_exit);


MODULE_LICENSE("GPL");