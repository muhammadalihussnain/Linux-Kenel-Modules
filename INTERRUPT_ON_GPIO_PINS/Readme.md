


This program is a simple Linux kernel module that sets up a GPIO pin (GPIO17) as an input and attaches an interrupt handler to it. 

the pin detects a rising edge (like pressing a button connected to it), the kernel calls the ISR, which logs a message to the system 

log. On unloading the module, it cleans up by freeing the interrupt and releasing the GPIO pin.



To use GPIO pin 17 as an input, we first need to reserve the pin, and for that we call the kernel API gpio_request(). This function 

takes two arguments: the pin number and a label name for that pin. If the request is successful, it means the pin has been allocated 

for our use; otherwise, it is already in use by another driver. After that, we set the pin direction to input using the 

gpio_direction_input() function, which simply takes the pin number we want to configure. This completes the GPIO setup. The next step 

is to configure an interrupt for this pin so that an action can be triggered when its state changes.



next we want to setup the interrupt . first we will map the pin number to the interrupt number with the following API

"request_irq()".  this api takes 5 arguments. 1 interrupt number, 2 interrupt handler pointer, flag for which input works, name and 

the NULL