# Safe kernel module

This minimal module allocates one memory page, checks whether the allocation
succeeded, and releases the page when unloaded.

```bash
make
sudo insmod ./safe_driver.ko
sudo dmesg | tail -n 5
sudo rmmod safe_driver
```
