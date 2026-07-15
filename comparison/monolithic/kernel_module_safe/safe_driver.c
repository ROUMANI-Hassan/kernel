// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

/* Keep the address so the memory can be released when the module is removed. */
static void *buffer;

/* __init marks this function as code used only while loading the module. */
static int __init safe_driver_start(void)
{
    /* Request one page of kernel memory. GFP_KERNEL allows the call to wait. */
    buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);

    /* kmalloc returns NULL when it cannot provide the requested memory. */
    if (!buffer)
        return -ENOMEM;

    /* pr_info writes this message to the kernel log read by dmesg. */
    pr_info("safe_driver: one page allocated\n");
    return 0;
}

/* __exit marks the function called when rmmod removes the module. */
static void __exit safe_driver_stop(void)
{
    /* Every successful allocation must have a matching release. */
    kfree(buffer);
    pr_info("safe_driver: memory released\n");
}

/* Connect the load and unload commands to the two functions above. */
module_init(safe_driver_start);
module_exit(safe_driver_stop);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Minimal safe kernel allocation example");
