// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <asm/cpu.h>

#define REQUEST_SIZE (5 * 1024 * 1024)

static int __init unsafe_driver_start(void)
{
    char *memory;

    if (!boot_cpu_has(X86_FEATURE_HYPERVISOR)) {
        pr_err("unsafe_driver: refused outside a virtual machine\n");
        return -EPERM;
    }

    pr_info("unsafe_driver: requesting 5 MiB with kmalloc\n");
    memory = kmalloc(REQUEST_SIZE, GFP_KERNEL);

    if (!memory)
        pr_err("unsafe_driver: allocation failed, but the driver will continue unsafely\n");

    /* Intentionally wrong: memory can be NULL after the failed allocation. */
    memset(memory, 0, REQUEST_SIZE);
    kfree(memory);

    return 0;
}

module_init(unsafe_driver_start);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("VM-only example of mishandling a failed memory allocation");
