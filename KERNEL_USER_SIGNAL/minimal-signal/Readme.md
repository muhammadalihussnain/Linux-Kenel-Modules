# Kernel to User Space Signal Communication

This repository demonstrates **communication from a Linux kernel module to a user-space program using signals**.

## Components

1. **Kernel Module (`sender_signal.c`)**

   * Registers a character device `/dev/sigdev`.
   * Accepts **PID registration** from user-space via `ioctl()`.
   * Sends `SIGUSR1` signal with custom data to the registered process.

2. **User Space Program (`receiver_user.c`)**

   * Opens `/dev/sigdev`.
   * Registers its PID with the kernel module via `ioctl()`.
   * Sets up a **signal handler** to receive `SIGUSR1`.
   * Prints the received signal data.

## Building the Project

A Makefile is provided to build both the kernel module and the user-space program.

```bash
make            # Build kernel module and user program
sudo insmod sender_signal.ko  # Load kernel module
./receiver_user  # Run user-space program
```

To remove the kernel module and clean the build:

```bash
sudo rmmod sender_signal
make clean
```

## Usage

1. **Start the user program**:

   ```bash
   ./receiver_user
   ```

   This prints the PID and registers it with the kernel.

2. **Kernel module sends a signal** (after 5 seconds by default) using the PID registered by the user program.

3. **User program receives the signal** and prints the attached custom data (`si_int = 1234`).

## How It Works

* The user program registers its PID using an **ioctl** call.
* Kernel stores this PID and uses **delayed work** to simulate an event.
* The kernel sends a **signal** (`SIGUSR1`) to the user-space program using `send_sig_info()`.
* The user program catches the signal via a **signal handler** and prints the data.

## Visual Flow

```
User Space                  Kernel Space
-------------               -------------
main()                       module_init()
  |                            |
  | open("/dev/sigdev")        |
  |--------------------------->|
  |                            |
  | ioctl(fd, IOCTL_SET_PID)   |
  |--------------------------->| saves PID
  |                            |
  |        schedule_delayed_work() after 5s
  |                            |
  |<---------------------------|
  |                            |
  |    send_sig_info(SIGUSR1)  |
  |<---------------------------| signal sent
  |                            |
  | signal handler executes    |
  | prints data (si_int = 1234)|
  |                            |
pause() <----------------------|
(wait for next signal)
```

## Key Points

* Communication is **Kernel → User** via signals.
* **PID registration** is required for kernel to know the target process.
* **Custom data** is sent using `si_int`.
* **Delayed work** simulates a kernel event.
* **Character device** acts as a communication channel.

## License

This project is licensed under **GPL**.
