# Linux Kernel Module: Kernel Threads Example (`kfret`)

This project demonstrates how to **create and manage kernel threads** in a Linux kernel module.  
It shows two different ways to start kernel threads using:
1. `kthread_create()` + `wake_up_process()`  
2. `kthread_run()` (shortcut that does both in one step)  

Each thread prints log messages at different intervals, with independent counters.

---

## 📂 Project Structure
```
.
├── kfret.c      # Kernel module source code
├── Makefile     # Makefile to build the kernel module
└── README.md    # Documentation
```

---

## ⚙️ Prerequisites

- Linux system with kernel headers installed  
- Basic knowledge of `insmod`, `rmmod`, and `dmesg`  

On Ubuntu/Debian, install build essentials and kernel headers:
```bash
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r)
```

---

## 🛠️ Build Instructions

Compile the module:
```bash
make
```

This will produce a kernel object file:
```
kfret.ko
```

Clean build artifacts:
```bash
make clean
```

---

## 🚀 Usage

### 1. Load the module
```bash
sudo insmod kfret.ko
```

Check if it’s loaded:
```bash
lsmod | grep kfret
```

### 2. View kernel logs
Since the module uses `pr_info()` for logging, check messages with:
```bash
dmesg | tail -n 20
```

You should see messages like:
```
[1234.567890] Loading kfret module
[1234.567892] kthread1 created and running with kthread create plus wakeup in 2 steps
[1234.567894] kthread2 created and running in one step
[1234.567896] Both threads are running now
[1235.567900] kthread 1 executed, counter = 0
[1236.567902] kthread 1 executed, counter = 1
[1236.567905] kthread 2 executed, counter = 0
[1237.567907] kthread 1 executed, counter = 2
...
```

Explanation:
- **Thread1** runs every 1 second (counter increments each run).  
- **Thread2** runs every 2 seconds.  
- Both counters are independent.  

### 3. Remove the module
```bash
sudo rmmod kfret
```

Logs will show:
```
[1240.123456] Stopped both threads, exiting module
[1240.123458] kthread 1 finished execution
[1240.123460] kthread 2 finished execution
```

---

## 🧠 How It Works

- **`thread_function`** is the entry point for each thread.  
- Each thread runs in a loop until `kthread_stop()` is called (when the module is removed).  
- Threads use `msleep()` to sleep between iterations:
  - Thread1 sleeps 1 second
  - Thread2 sleeps 2 seconds  

---

## 📖 Key Functions Used

- `kthread_create(func, data, name)`: Creates a thread but does **not** start it.  
- `wake_up_process(task)`: Starts a thread created by `kthread_create`.  
- `kthread_run(func, data, name)`: Shortcut to create **and** start a thread.  
- `kthread_should_stop()`: Loop condition to check if the thread should exit.  
- `kthread_stop(task)`: Stops the thread safely.  
- `msleep(ms)`: Makes the thread sleep for the given milliseconds.  

---

## 🔍 Notes

- This module is for **educational purposes only**.  
- Kernel threads run in kernel space, so misuse may cause crashes.  
- Always unload (`rmmod`) the module to stop running threads before modifying code.  

---

## ✅ Summary

This project demonstrates:
- How to create kernel threads in Linux  
- Difference between `kthread_create` and `kthread_run`  
- Running multiple threads with independent states  
- Logging output via `dmesg`

---

👨‍💻 Author: *Tutorial Example (modified for explanation clarity)*  
License: GPL
