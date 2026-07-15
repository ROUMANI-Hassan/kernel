// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <asm/cpu.h>

/* This request is larger than the 4 MiB kmalloc ceiling in the tested VM. */
#define REQUEST_SIZE (5 * 1024 * 1024)

/* The kernel runs this function while insmod is loading the module. */
static int __init unsafe_driver_start(void)
{
    char *memory;

    /* Refuse the destructive test when no virtual-machine flag is present. */
    if (!boot_cpu_has(X86_FEATURE_HYPERVISOR)) {
        pr_err("unsafe_driver: refused outside a virtual machine\n");
        return -EPERM;
    }

    pr_info("unsafe_driver: requesting 5 MiB with kmalloc\n");
    memory = kmalloc(REQUEST_SIZE, GFP_KERNEL);

    /* This message reports the failure but does not stop the function. */
    if (!memory)
        pr_err("unsafe_driver: allocation failed, but the driver will continue unsafely\n");

    /*
     * Intentionally wrong: memory is NULL after the failed allocation.
     * Writing through it causes the kernel fault that kills insmod.
     */
    memset(memory, 0, REQUEST_SIZE);

    /* This line is reached only if the allocation and memset both succeeded. */
    kfree(memory);

    return 0;
}

/* Tell Linux which function to call when this module is loaded. */
module_init(unsafe_driver_start);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("VM-only example of mishandling a failed memory allocation");
