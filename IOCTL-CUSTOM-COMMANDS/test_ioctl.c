#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "mychardev_ioctl.h"  // Include the same header as the kernel module

int main() {
    int fd = open("/dev/mychardev0", O_RDWR);  // open minor 0 device
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    printf("Device opened successfully\n");

    // Increment counter
    if (ioctl(fd, IOCTL_CMD_INCREMENT) == -1) {
        perror("IOCTL increment failed");
    } else {
        printf("IOCTL increment executed\n");
    }

    // Decrement counter
    if (ioctl(fd, IOCTL_CMD_DECREMENT) == -1) {
        perror("IOCTL decrement failed");
    } else {
        printf("IOCTL decrement executed\n");
    }

    // Reset counter
    if (ioctl(fd, IOCTL_CMD_RESET) == -1) {
        perror("IOCTL reset failed");
    } else {
        printf("IOCTL reset executed\n");
    }

    close(fd);
    printf("Device closed\n");

    return 0;
}
