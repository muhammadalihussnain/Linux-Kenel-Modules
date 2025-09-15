# Makefile for ioctl_example kernel module

# KERNELDIR points to the currently running kernel headers
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Target module name (without .ko)
obj-m := ioctl_example.o

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean