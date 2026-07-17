# Unsafe kernel module

This minimal module is only for a disposable VM. It requests 5 MiB with
`kmalloc()`. On the system used for this study, one `kmalloc()` block is limited
to 4 MiB, so the request returns `NULL`. The module deliberately ignores that
failure and writes through the invalid pointer.

This causes a kernel fault while the module is loading, so Linux kills the
`insmod` process. The VM normally remains running. The module does not call
`panic()` or change `panic_on_oops`.

Take a VM snapshot before running it.

## Build and trigger the fault

```bash
cd ~/kernel-study/kernel_module_unsafe
make clean
make
sudo insmod ./unsafe_driver.ko
```

The terminal normally prints `Killed`. Inspect the kernel report with:

```bash
sudo dmesg | tail -n 40
```

The module should not be loaded after its initialization failed:

```bash
lsmod | grep unsafe_driver || echo "unsafe_driver is not loaded"
make clean
```

Never load this module on a physical machine.
