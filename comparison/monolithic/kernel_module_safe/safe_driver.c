// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

static void *buffer;

static int __init safe_driver_start(void)
{
    buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);
    if (!buffer)
        return -ENOMEM;

    pr_info("safe_driver: one page allocated\n");
    return 0;
}

static void __exit safe_driver_stop(void)
{
    kfree(buffer);
    pr_info("safe_driver: memory released\n");
}

module_init(safe_driver_start);
module_exit(safe_driver_stop);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("Minimal safe kernel allocation example");
