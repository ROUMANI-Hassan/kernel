/* These headers provide normal user-space C and POSIX functions. */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    /* This buffer belongs only to this user-space process. */
    char message[128];
    ssize_t bytes_read;
    int device;

    /*
     * open() asks Linux for access to the device. On success, Linux returns a
     * file descriptor: a small number representing the open device.
     */
    device = open("/dev/kernel_message", O_RDONLY);
    if (device == -1) {
        fprintf(stderr, "Cannot open /dev/kernel_message: %s\n",
                strerror(errno));
        return 1;
    }

    /*
     * read() is the important boundary crossing. The process enters the
     * kernel, and Linux runs kernel_message_read() from the module.
     */
    bytes_read = read(device, message, sizeof(message) - 1);
    if (bytes_read == -1) {
        fprintf(stderr, "Cannot read the device: %s\n", strerror(errno));
        close(device);
        return 1;
    }

    /* read() returns bytes, so add '\0' before treating them as a C string. */
    message[bytes_read] = '\0';

    /* printf() is a user-space library function and prints to the terminal. */
    printf("User application received: %s", message);

    /* Release this process's file descriptor; the module remains loaded. */
    close(device);
    return 0;
}
