# Kernel-space stack overflow

This module contains the same recursive 1 KiB stack use as the user program,
but it runs on the much smaller kernel stack. The module loads first and creates
an explicit test control in `debugfs`. The dangerous recursion starts only when
`CRASH` is written to that control.

Use only a disposable VM and take a snapshot first.

```bash
cd ~/kernel-study/KernelvsUser/stackoverflow/kernel_space
make clean
make
sudo insmod ./kernel_stack_overflow.ko
lsmod | grep kernel_stack_overflow
pgrep -a insmod || echo "insmod has exited; the module remains loaded"
sudo dmesg | tail -n 5
```

At this point, `insmod` has exited while the module remains loaded in the
kernel. Ensure `debugfs` is mounted:

```bash
grep CONFIG_VMAP_STACK /boot/config-$(uname -r)
mountpoint -q /sys/kernel/debug || sudo mount -t debugfs debugfs /sys/kernel/debug
sudo ls -l /sys/kernel/debug/stack_overflow_demo/trigger
```

Trigger the intentional kernel stack overflow:

```bash
printf 'CRASH\n' | sudo tee /sys/kernel/debug/stack_overflow_demo/trigger
```

On an x86-64 kernel with guarded kernel stacks (`CONFIG_VMAP_STACK=y`), reaching
the guard page normally causes a fatal stack-overflow exception. The VM may
panic, freeze, or restart. The exact result depends on the guest kernel.

This module does not call `panic()` and does not change `panic_on_oops`.
