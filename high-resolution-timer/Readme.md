# ⏱️ High-Resolution Timer Kernel Module

## 📌 Overview
This project demonstrates the use of **High-Resolution Timers (hrtimer)** in the Linux kernel.  
The module initializes a timer that fires after a specified interval (100 ms in this example) and logs the elapsed time since the timer was started.

It serves as a simple example for learning:
- How to create and start high-resolution timers in kernel space
- How to implement and register a timer callback
- How to cancel timers when the module is unloaded

---

## 📂 Project Structure

    high-resolution-timer/
    │── my_timer.c # Main kernel module source code
    │── Makefile # Build rules for kernel module
    │── README.md # Documentation


---

## ⚙️ Build Instructions

    1. Ensure you have kernel headers installed:
    ```bash
    sudo apt install build-essential linux-headers-$(uname -r)

        Compile the module:

    make

    You should now have the kernel object file:

        my_timer.ko

🚀 Usage

    Load the module

    sudo insmod my_timer.ko

    Check kernel logs

    dmesg | tail -n 20

    Example output:

    [ 1234.5678 ] Hello, Kernel
    [ 1234.6789 ] start_t -> now_t = 100

    List loaded modules

    lsmod | grep my_timer

    Unload the module

    sudo rmmod my_timer

    Check logs again:

    dmesg | tail -n 10

    Example output:

        [ 1234.9876 ] Goodbye, Kernel

🛠️ How It Works

    When loaded, the module:

        Initializes a high-resolution timer (hrtimer)

        Starts it with a delay of 100 ms

    When the timer expires:

        The callback function logs elapsed time (jiffies_to_msecs(now - start))

    When unloaded:

        The timer is cancelled

        A farewell message is logged

📚 References

    Linux Kernel Docs – hrtimer

Linux Device Drivers, 3rd Edition
⚠️ Notes

    This module is for educational purposes only.

    Requires root privileges to load/unload kernel modules.

    Do not run in production systems without proper testing.