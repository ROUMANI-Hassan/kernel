// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/cpu.h>

static unsigned int memory_mb = 1;
module_param(memory_mb, uint, 0400);
MODULE_PARM_DESC(memory_mb, "Contiguous kernel memory to request in MiB");
static bool unsafe_on_failure;
module_param(unsafe_on_failure, bool, 0400);
MODULE_PARM_DESC(unsafe_on_failure, "Deliberately access NULL after allocation failure (VM only)");
static void *buffer;

static int __init memory_driver_init(void)
{
    size_t bytes;

    if (memory_mb == 0 || memory_mb > 4096)
        return -EINVAL;

    bytes = (size_t)memory_mb * 1024 * 1024;
    pr_info("memory_driver: kernel kmalloc ceiling is %lu MiB\n",
            (unsigned long)(KMALLOC_MAX_SIZE / (1024 * 1024)));
    pr_info("memory_driver: requesting %u MiB\n", memory_mb);
    buffer = kmalloc(bytes, GFP_KERNEL | __GFP_NORETRY | __GFP_NOWARN);

    if (buffer == NULL) {
        pr_err("memory_driver: allocation of %u MiB failed\n", memory_mb);

        /* Correct driver behaviour: stop and report the allocation error. */
        if (!unsafe_on_failure) {
            pr_info("memory_driver: failure handled safely with -ENOMEM\n");
            return -ENOMEM;
        }

        /* The teaching fault is allowed only when a hypervisor is detected. */
        if (!boot_cpu_has(X86_FEATURE_HYPERVISOR)) {
            pr_err("memory_driver: unsafe mode refused; no hypervisor detected\n");
            return -EPERM;
        }

        pr_warn("memory_driver: hypervisor detected; VM-only fault enabled\n");
        pr_emerg("memory_driver: intentionally writing through NULL\n");
        *(volatile unsigned char *)buffer = 0xA5;
        return -EFAULT;
    }

    memset(buffer, 0xA5, bytes);
    pr_info("memory_driver: allocated and initialized %u MiB\n", memory_mb);
    return 0;
}
static void __exit memory_driver_exit(void) { kfree(buffer); pr_info("memory_driver: buffer released\n"); }
module_init(memory_driver_init);
module_exit(memory_driver_exit);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("VM-only monolithic kernel memory failure demonstration");
