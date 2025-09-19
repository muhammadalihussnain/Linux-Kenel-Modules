#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <string.h>

#define DEVICE_PATH "/dev/irqpoll"

int main(void)
{
    int fd;
    struct pollfd pfd;
    int ret;

    // Open the device file created by the kernel module
    fd = open(DEVICE_PATH, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    pfd.fd = fd;
    pfd.events = POLLIN;   // We are only interested in "data ready" events

    printf("Waiting for button press/release events on %s...\n", DEVICE_PATH);

    while (1) {
        ret = poll(&pfd, 1, -1);  // Wait indefinitely (-1) until an event occurs
        if (ret < 0) {
            perror("poll failed");
            close(fd);
            return EXIT_FAILURE;
        }

        if (pfd.revents & POLLIN) {
            printf("Button event detected!\n");
        }
    }

    close(fd);
    return EXIT_SUCCESS;
}
