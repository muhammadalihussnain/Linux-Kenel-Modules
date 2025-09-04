# Hello Character Device Driver

This project demonstrates a simple Linux character device driver and a user-space test program.  
The driver registers itself with a dynamic **major number** and allows opening and closing the device file.  

When the device is opened/closed, kernel logs will show details such as:
- Major and Minor device numbers
- File position
- File flags
- File mode

---

## 📂 Project Structure

.
├── hello_cdev.c # Kernel module (character device driver)
├── test.c # User-space test program
├── Makefile # Build rules for the kernel module
└── README.md # Documentation


---

⚙️ Build Instructions

    Clone or copy this project into a folder.

    Run make to build the kernel module:

make

This will produce:

    hello_cdev.ko → kernel module (driver)

🚀 Loading the Driver

Insert the kernel module:

sudo insmod hello_cdev.ko

Check kernel logs for the assigned major number in another terminal:

dmesg | tail -n 10

You should see something like:

hello_cdev: Registered char device with major number 240

📂 Creating Device Files

Once you know the major number (replace 240 with your actual one), create device nodes:

sudo mknod /dev/hello0 c <major_number> 0
sudo mknod /dev/hello1 c <major_number> 1

Check that they exist:

ls -l /dev/hello*

🖥️ Running the User Program

Compile the user-space test program:

gcc test.c -o test

Run it by passing the device file:

./test /dev/hello0

If successful, check the kernel log:

dmesg | tail -n 20

You’ll see messages like:

hello_cdev: Device opened with major 240 and minor 0
hello_cdev: Device closed with major 240 and minor 0

❌ Unloading the Driver

Remove the module when done:

sudo rmmod hello_cdev

Confirm in the logs:

dmesg | tail -n 10

Expected message:

hello_cdev: Unregistered char device with major number 240

🧹 Cleaning Up

To clean build artifacts:

make clean

To remove device files:

sudo rm /dev/hello0 /dev/hello1

📖 Summary of Commands

# Build the module
make

# Insert module
sudo insmod hello_cdev.ko

# Check logs for major number
dmesg | tail -n 10

# Create device file (replace 240 with actual major number)
sudo mknod /dev/hello0 c 240 0
sudo mknod /dev/hello1 c 240 1

# Compile and run test
gcc test.c -o test
./test /dev/hello0

# Check kernel logs
dmesg | tail -n 20

# Remove module
sudo rmmod hello_cdev

# Clean
make clean

👨‍💻 Author

Muhammad Ali Hussnain

    Project: Simple Linux Character Device Driver


---

Would you like me to also add a **section with example log outputs** (`dmesg`) so your teamm