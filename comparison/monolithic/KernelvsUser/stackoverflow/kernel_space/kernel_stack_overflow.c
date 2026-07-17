// SPDX-License-Identifier: MIT OR GPL-2.0-only
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <asm/cpu.h>

#define COMMAND_SIZE 16
#define STACK_FRAME_SIZE 1024

static struct dentry *demo_directory;

static noinline void exhaust_stack(unsigned long depth)
{
    volatile unsigned char frame[STACK_FRAME_SIZE];

    /* Every call keeps another 1 KiB array on the kernel stack. */
    memset((void *)frame, (unsigned char)depth, sizeof(frame));

    /* A theoretical end stops the compiler reporting infinite recursion. */
    if (depth < ~0UL)
        exhaust_stack(depth + 1);

    /* Keep the local array alive so recursion is not tail-optimized. */
    barrier_data(frame);
}

static ssize_t trigger_write(struct file *file, const char __user *user_buffer,
                             size_t count, loff_t *position)
{
    char command[COMMAND_SIZE];
    size_t length = min(count, (size_t)COMMAND_SIZE - 1);

    if (copy_from_user(command, user_buffer, length))
        return -EFAULT;

    command[length] = '\0';
    if (!sysfs_streq(command, "CRASH"))
        return -EINVAL;

    pr_err("kernel_stack_overflow: starting recursive stack exhaustion\n");
    exhaust_stack(0);
    return count;
}

static const struct file_operations trigger_operations = {
    .owner = THIS_MODULE,
    .open = simple_open,
    .write = trigger_write,
};

static int __init stack_demo_start(void)
{
    struct dentry *trigger;

    /* The destructive module refuses to load on a physical machine. */
    if (!boot_cpu_has(X86_FEATURE_HYPERVISOR)) {
        pr_err("kernel_stack_overflow: refused outside a virtual machine\n");
        return -EPERM;
    }

    demo_directory = debugfs_create_dir("stack_overflow_demo", NULL);
    if (IS_ERR(demo_directory))
        return PTR_ERR(demo_directory);

    trigger = debugfs_create_file("trigger", 0200, demo_directory, NULL,
                                  &trigger_operations);
    if (IS_ERR(trigger)) {
        debugfs_remove_recursive(demo_directory);
        return PTR_ERR(trigger);
    }

    pr_info("kernel_stack_overflow: module loaded; trigger is ready\n");
    return 0;
}

static void __exit stack_demo_stop(void)
{
    debugfs_remove_recursive(demo_directory);
    pr_info("kernel_stack_overflow: module removed\n");
}

module_init(stack_demo_start);
module_exit(stack_demo_stop);
MODULE_LICENSE("Dual MIT/GPL");
MODULE_DESCRIPTION("VM-only kernel stack-overflow isolation demonstration");
