#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>

#define DEVICE "/dev/ioctl_example"

#define MY_IOCTL_MAGIC 'M'
#define WR_VALUE   _IOW(MY_IOCTL_MAGIC, 0, int32_t)
#define RD_VALUE   _IOR(MY_IOCTL_MAGIC, 1, int32_t)
#define GREETER    _IOW(MY_IOCTL_MAGIC, 2, struct mystruct)

struct mystruct {
    int repeat;
    char name[32];
};

int main() {
    int fd;
    int32_t value, read_value;
    struct mystruct greet;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Write a value
    value = 23;
    ioctl(fd, WR_VALUE, &value);

    // Read it back
    ioctl(fd, RD_VALUE, &read_value);
    printf("Read value from kernel: %d\n", read_value);

    // Send a struct
    greet.repeat = 3;
    strcpy(greet.name, "Muhammad");
    ioctl(fd, GREETER, &greet);

    close(fd);
    return 0;
}
