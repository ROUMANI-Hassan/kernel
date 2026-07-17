// SPDX-License-Identifier: MIT
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>

#define STACK_FRAME_SIZE 1024

static __attribute__((noinline)) void exhaust_stack(unsigned long depth)
{
    volatile unsigned char frame[STACK_FRAME_SIZE];

    /* Every call keeps another 1 KiB array on this process's stack. */
    memset((void *)frame, (unsigned char)depth, sizeof(frame));

    /* A theoretical end stops the compiler reporting infinite recursion. */
    if (depth < ~0UL)
        exhaust_stack(depth + 1);

    /* This use after recursion prevents tail-call optimization. */
    frame[depth % STACK_FRAME_SIZE] ^= (unsigned char)depth;
}

int main(void)
{
    struct rlimit limit;

    if (getrlimit(RLIMIT_STACK, &limit) == 0)
        printf("User stack limit: %lu KiB\n",
               (unsigned long)(limit.rlim_cur / 1024));

    puts("Starting recursion in user space...");
    fflush(stdout);
    exhaust_stack(0);
    return 0;
}
