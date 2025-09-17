#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>

#define DEVICE "/dev/sigdev"
#define IOCTL_SET_PID _IOW('a', 'a', int32_t *)

static void handler(int signo, siginfo_t *info, void *context)
{
    if (signo == SIGUSR1) {
        printf("User: Received SIGUSR1 from kernel!\n");
        printf("User: Data received (si_int) = %d\n", info->si_int);
    }
}

int main()
{
    int fd;
    int pid = getpid();

    printf("User: My PID = %d\n", pid);

    // Open device file
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("User: Failed to open device");
        return 1;
    }

    // Register my PID with kernel via ioctl
    if (ioctl(fd, IOCTL_SET_PID, &pid) < 0) {
        perror("User: Failed to register PID with kernel");
        close(fd);
        return 1;
    }
    printf("User: Registered my PID (%d) with kernel\n", pid);

    // Setup signal handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;   // we want siginfo_t data
    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("User: sigaction failed");
        close(fd);
        return 1;
    }

    printf("User: Waiting for signal from kernel...\n");

    // Loop forever waiting for signals
    while (1) {
        pause();  // suspend until a signal is received
    }

    close(fd);
    return 0;
}
