# Safe kernel module

This minimal module allocates one memory page, checks whether the allocation
succeeded, and releases the page when unloaded.

## Build and load

```bash
cd ~/kernel-study/kernel_module_safe
make clean
make
sudo insmod ./safe_driver.ko
```

## Verify and unload

```bash
lsmod | grep safe_driver
sudo dmesg | tail -n 5
sudo rmmod safe_driver
sudo dmesg | tail -n 5
make clean
```

Expected messages include `one page allocated` and `memory released`. The
module should no longer appear in `lsmod` after `rmmod`.
