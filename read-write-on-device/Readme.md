hello_cdev – Simple Linux Character Device Driver 
📌 Overview 

This project implements a basic Linux character device driver named hello_cdev. It demonstrates how to: Register a character device Handle open, release, read, and write operations Exchange data between kernel space and user space using copy_to_user and copy_from_user The driver maintains an internal 64-byte buffer that can be read/written by user applications. 📂 Project Structure
hello_cdev.c      → Kernel module source code
Makefile          → Build instructions


⚙️ Features

    Registers a character device dynamically with a major number assigned by the kernel.

    Provides the following file operations:

        open: Logs when the device is opened.

        release: Logs when the device is closed.

        read: Reads data from the kernel buffer into user space.

        write: Writes user data into the kernel buffer.

    Uses printk for kernel log messages (viewable with dmesg).

    Supports multiple minor numbers.



🛠️ Build & Install


1. Compile the module

make

This produces hello_cdev.ko.


2. Insert the module

sudo insmod hello_cdev.ko

Check kernel logs:

dmesg | tail

You should see a message like:

hello_cdev: registered successfully with major number <X>

(X will be the major number assigned by the kernel).




📑 Device File Creation

To interact with the driver, create device nodes in /dev.

Example (replace <X> with the actual major number from dmesg):

sudo mknod /dev/hello_cdev c <X> 0
sudo mknod /dev/hello_cdev10 c <X> 10

Check:

ls -l /dev/hello_cdev*





🧪 Testing the Driver


Write data

echo "hello kernel" | sudo tee /dev/hello_cdev

Read data back

cat /dev/hello_cdev

Write longer messages

echo "this is a longer test string" | sudo tee /dev/hello_cdev
cat /dev/hello_cdev

Multiple device nodes

cat /dev/hello_cdev10

(Will show buffer contents for minor=10, which maps to the same driver).





🧹 Cleanup
Remove device nodes

sudo rm -f /dev/hello_cdev /dev/hello_cdev10

Unload the module

sudo rmmod hello_cdev

Clean build files

make clean




🔎 Debugging

    Kernel logs:

dmesg | grep hello_cdev

    Example log entries:

hello_cdev: device opened (major=235, minor=0)
hello_cdev: wrote 13 bytes, new offset=13
hello_cdev: read 13 bytes, new offset=13
hello_cdev: device closed (major=235, minor=0)



📜 License

This project is licensed under the GPL v2 license (required for Linux kernel modules).




👤 Author

Muhammad Ali Hussnain
Simple example to demonstrate Linux kernel character device drivers.