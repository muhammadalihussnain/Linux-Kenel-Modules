# Hello Character Device Driver

This project demonstrates a simple Linux character device driver that registers a character device, assigns it a major number, and implements a basic `read` operation.

---

## Files
- `hello_cdev.c` → The kernel module source code (driver).
- `Makefile` → Used to build the kernel module.

---

## 1. Build the Module
Run the following command in the project directory:
```bash
make
This will compile the code and generate a loadable kernel module file
-  'hello_cdev.ko'

Insert the module into the kernel by the following command:
sudo insmod hello_cdev.ko


Open the other terminal and run the command 
-   sudo dmesg -W
the above command will show us the logs in the other terminal and print the major device number which have been assigned by the kerenl

now we have to make the Device file by the command given below 
-  sudo mknode /dev/Hello0(name what you want to file) c (here c for the charater device) 235(major number ) 0 (minor number)

then run the command given below 
- sudo cat /dev/Hello0

its output will show us the read function has successfully executed

