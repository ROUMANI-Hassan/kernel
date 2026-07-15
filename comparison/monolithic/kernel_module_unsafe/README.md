# Unsafe kernel module

This minimal module is only for a disposable VM. It requests 5 MiB with
`kmalloc()`. On the system used for this study, one `kmalloc()` block is limited
to 4 MiB, so the request returns `NULL`. The module deliberately ignores that
failure and writes through the invalid pointer.

This causes a kernel fault while the module is loading, so Linux kills the
`insmod` process. The VM normally remains running. The module does not call
`panic()` or change `panic_on_oops`.

Take a VM snapshot before running it.

```bash
make
sudo insmod ./unsafe_driver.ko
```

Never load this module on a physical machine.
