# Overview

This project implements a Linux character device driver with custom IOCTL (Input/Output Control) commands. The driver maintains a simple counter variable that can be manipulated through user-space applications using specific IOCTL commands. The implementation demonstrates fundamental Linux kernel module programming concepts including device registration, file operations, and user-kernel communication.

## Key Components

- **Kernel Module (`mychardev.c`)**: The core device driver that:
  - Registers a character device with the kernel
  - Implements file operations (open, release, ioctl)
  - Maintains a counter variable in kernel space
  - Processes custom IOCTL commands to manipulate the counter

- **Header File (`mychardev.h`)**: Defines the IOCTL command macros and function prototypes used by the kernel module.

- **IOCTL Header (`mychardev_ioctl.h`)**: Shared header between kernel and user space that defines the IOCTL command macros with consistent values.

- **Test Application (`test_ioctl.c`)**: User-space program that demonstrates how to interact with the device driver using the custom IOCTL commands.

## Function Descriptions

### Kernel Module Functions (`mychardev.c`)

- **my_open()**: Called when a user-space process opens the device file. Logs the major and minor numbers of the accessed device.
- **my_release()**: Called when a user-space process closes the device file. Logs the major and minor numbers of the released device.
- **my_ioctl()**: Handles custom IOCTL commands from user space:
  - `IOCTL_CMD_INCREMENT`: Increases the internal counter by 1
  - `IOCTL_CMD_DECREMENT`: Decreases the internal counter by 1
  - `IOCTL_CMD_RESET`: Sets the internal counter to 0
- **test_init()**: Module initialization function that registers the character device with the kernel.
- **test_exit()**: Module cleanup function that unregisters the character device.

### Test Application Functions (`test_ioctl.c`)

- **main()**: Demonstrates usage of the device driver by:
  - Opening the device file
  - Sending increment, decrement, and reset commands via IOCTL
  - Closing the device file

## Prerequisites

- Linux kernel headers for your current kernel version
- GCC compiler
- Make utility
- Root/sudo privileges for loading kernel modules and creating device nodes

## Installation and Setup

1. Compile the kernel module:
   ```bash
   make
   ```

2. Load the kernel module:
   ```bash
   sudo insmod mychardev.ko
   ```

3. Create device nodes (replace X with actual major number from kernel logs):
   ```bash
   sudo mknod /dev/mychardev0 c X 0
   sudo mknod /dev/mychardev1 c X 1
   ```
   The major number is printed when loading the module (check with `dmesg`).

## Usage

1. Compile the test application:
   ```bash
   gcc -o test_ioctl test_ioctl.c
   ```

2. Run the test application:
   ```bash
   sudo ./test_ioctl
   ```

3. Check kernel logs to see the driver's operation:
   ```bash
   dmesg
   ```

## IOCTL Commands

The driver supports three custom IOCTL commands:

- **IOCTL_CMD_INCREMENT**: Increases the internal counter
- **IOCTL_CMD_DECREMENT**: Decreases the internal counter
- **IOCTL_CMD_RESET**: Resets the counter to zero

## Cleaning Up

1. Remove device nodes:
   ```bash
   sudo rm /dev/mychardev0
   sudo rm /dev/mychardev1
   ```

2. Unload the kernel module:
   ```bash
   sudo rmmod mychardev
   ```

3. Clean build files:
   ```bash
   make clean
   ```

## File Structure

```
.
├── mychardev.c          # Kernel module implementation
├── mychardev.h          # Kernel module header
├── mychardev_ioctl.h    # Shared IOCTL definitions
├── test_ioctl.c         # Test application
├── Makefile             # Build configuration
└── README.md            # This file
```

## Debugging

- Use `dmesg` to view kernel log messages
- Check device registration with `cat /proc/devices | grep mychardev`
- Verify device node creation with `ls -l /dev/mychardev*`

## Security Notes

- The device nodes are created with root privileges
- The test application requires sudo privileges to access the device
- In a production environment, consider proper permission management

## Limitations

- The counter is global across all device instances
- No synchronization mechanisms for concurrent access
- Simple implementation without advanced error handling

## Extending the Driver

This implementation can be extended to:

- Add more IOCTL commands
- Implement read/write operations
- Add per-device instance data
- Include synchronization for multi-threaded access
- Add device-specific functionality beyond a simple counter

## License

This project is licensed under the GPL license as specified in the module code.
