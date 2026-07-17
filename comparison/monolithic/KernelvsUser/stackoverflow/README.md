# Stack overflow: user space compared with kernel space

Both projects deliberately run the same kind of recursive function. Each call
keeps a 1 KiB local array on its current stack.

| Test | Stack belongs to | Expected result |
|---|---|---|
| `user_space/` | One user process | Linux sends `SIGSEGV`; only the program stops |
| `kernel_space/` | The Linux kernel | A fatal kernel stack fault may stop the VM |

The experiment demonstrates isolation. A user process has its own virtual
address space and stack boundary. Linux can terminate that process without
terminating other applications. A kernel module shares the kernel execution
environment, so a severe stack fault may prevent the kernel from recovering.

Run the user-space test first. Take a VM snapshot before running the
kernel-space trigger.

## Run the user-space project

```bash
cd ~/kernel-study/KernelvsUser/stackoverflow/user_space
make clean
make
ulimit -c 0
./user_stack_overflow
echo "The shell still works"
make clean
```

The program should finish with `Segmentation fault`. Continue with the detailed
[`user_space/README.md`](user_space/README.md) explanation if needed.

## Run the kernel-space project

Take a VirtualBox snapshot first. Then:

```bash
cd ~/kernel-study/KernelvsUser/stackoverflow/kernel_space
make clean
make
sudo insmod ./kernel_stack_overflow.ko
lsmod | grep kernel_stack_overflow
pgrep -a insmod || echo "insmod has exited; the module remains loaded"
sudo dmesg | tail -n 5
```

Check for guarded kernel stacks and prepare `debugfs`:

```bash
grep CONFIG_VMAP_STACK /boot/config-$(uname -r)
mountpoint -q /sys/kernel/debug || sudo mount -t debugfs debugfs /sys/kernel/debug
ls -l /sys/kernel/debug/stack_overflow_demo/trigger
```

Trigger the fault only when the VM snapshot is ready:

```bash
printf 'CRASH\n' | sudo tee /sys/kernel/debug/stack_overflow_demo/trigger
```

The VM may panic, freeze, or restart. Restore the snapshot before repeating the
test.
