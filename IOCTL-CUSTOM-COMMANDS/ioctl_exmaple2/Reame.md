# ioctl_example

This project demonstrates how to use **IOCTL (Input/Output Control)** commands to exchange data between **user space** and **kernel space** in Linux.  
It consists of:

- A **kernel module (`ioctl_example.ko`)** implementing a character device.
- A **user-space test program (`test_ioctl.c`)** that interacts with the device using IOCTL commands.
- A **Makefile** for building the kernel module.

---

## Features

The driver supports three IOCTL commands:

1. **WR_VALUE** – Write an integer from user space to kernel space.  
2. **RD_VALUE** – Read the integer back from kernel space to user space.  
3. **GREETER** – Send a struct (`repeat`, `name`) from user space to kernel space and log the greeting.

---

## File Structure

```
├── ioctl_example.c   # Kernel module source
├── test_ioctl.c      # User space test program
├── Makefile          # Build rules for the kernel module
```

---

## Build Instructions

### 1. Build the kernel module
```bash
make
```

This produces `ioctl_example.ko`.

### 2. Insert the kernel module
```bash
sudo insmod ioctl_example.ko
```

Check kernel logs:
```bash in the other terminal
dmesg | tail
```

You will see output like:
```
ioctl_example: module loaded, major = 240
mknod /dev/ioctl_example c 240 0
```

### 3. Create the device node
Copy the `mknod` command from `dmesg` output, for example:
```bash
sudo mknod /dev/ioctl_example c 240 0
sudo chmod 666 /dev/ioctl_example
```

---

## Running the User Program

1. Compile the test program:
```bash
gcc test_ioctl.c -o test_ioctl
```

2. Run it:
```bash
./test_ioctl
```

Expected output:
```
Read value from kernel: 23
```

Kernel logs (`dmesg`) will show:
```
ioctl_example: device opened
ioctl_example: Updated answer to 23
ioctl_example: The answer was copied to user
ioctl_example: 3 greets to Muhammad
ioctl_example: device closed
```

---

## Cleanup

Remove the module and device node:
```bash
sudo rmmod ioctl_example
sudo rm /dev/ioctl_example
make clean
```

---

## Notes

- IOCTL is used when **read/write syscalls are not sufficient** for controlling a device.
- The driver demonstrates safe data exchange using:
  - `copy_from_user()` (user → kernel)
  - `copy_to_user()` (kernel → user)
- The `GREETER` command shows how to pass a **struct** from user space to kernel space.

---

## License

This project is licensed under the **GPL v2** license.

Author: *Muhammad Ali*  
Description: *IOCTL Example with int and struct exchange*