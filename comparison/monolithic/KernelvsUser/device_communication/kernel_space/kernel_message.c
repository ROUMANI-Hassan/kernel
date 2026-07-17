/* These are Linux kernel headers, not the normal C library headers. */
#include <linux/atomic.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

/*
 * This variable lives in kernel memory. It remains available after the user
 * application exits and must be safe when several processes read at once.
 */
static atomic_t read_count = ATOMIC_INIT(0);

/*
 * Linux calls this function in kernel space when a process calls read() on
 * /dev/kernel_message. The user application does not call it directly.
 *
 * The __user annotation says that user_buffer belongs to user space. Kernel
 * code must not use it like a normal kernel pointer.
 */
static ssize_t kernel_message_read(struct file *file, char __user *user_buffer,
                                   size_t count, loff_t *offset)
{
    /* Local variables in this function are stored on the kernel stack. */
    char message[80];
    int length;

    /* A second read returns end-of-file instead of repeating the message. */
    if (*offset != 0)
        return 0;

    /* scnprintf() is a kernel function; kernel code cannot use libc printf(). */
    length = scnprintf(message, sizeof(message),
                       "Hello from kernel space. Read number: %d\n",
                       atomic_inc_return(&read_count));

    /*
     * Copy the message safely from kernel memory to the process's buffer.
     * Returning the length tells read() how many bytes the process received.
     */
    return simple_read_from_buffer(user_buffer, count, offset, message, length);
}

/* Connect the device's read operation to our kernel callback. */
static const struct file_operations kernel_message_operations = {
    .owner = THIS_MODULE,
    .read = kernel_message_read,
};

/* Describe the device that Linux will expose as /dev/kernel_message. */
static struct miscdevice kernel_message_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "kernel_message",
    .fops = &kernel_message_operations,
    .mode = 0444,
};

/* This function runs in kernel space when insmod loads the module. */
static int __init kernel_message_init(void)
{
    int result = misc_register(&kernel_message_device);

    if (result)
        return result;

    /* Kernel messages go to the kernel log and can be viewed with dmesg. */
    pr_info("kernel_message: /dev/kernel_message is ready\n");
    return 0;
}

/* This function runs when rmmod unloads the module. */
static void __exit kernel_message_exit(void)
{
    misc_deregister(&kernel_message_device);
    pr_info("kernel_message: device removed\n");
}

/* Tell Linux which functions load and unload this module. */
module_init(kernel_message_init);
module_exit(kernel_message_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hassan Roumani");
MODULE_DESCRIPTION("Minimal device for comparing kernel space and user space");
