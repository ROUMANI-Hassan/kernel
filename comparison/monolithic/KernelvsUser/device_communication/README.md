# Communication between user space and kernel space

This safe example shows a user application requesting data from a kernel
module. The module creates the read-only device `/dev/kernel_message`. The
application opens that device and reads a message from it.

```text
User application
      |
      | open() and read()
      v
Linux system-call boundary
      |
      v
Kernel module: /dev/kernel_message
```

The application cannot call the module's C function directly. It uses normal
system calls, and Linux transfers the request across the boundary.

## Build and load the kernel module

Run these commands inside the prepared VM:

```bash
cd ~/kernel-study/KernelvsUser/device_communication/kernel_space
make clean
make
sudo insmod ./kernel_message.ko
ls -l /dev/kernel_message
sudo dmesg | tail -n 5
```

`insmod` asks Linux to load the module. The module then registers the device.
The `insmod` process exits, but `/dev/kernel_message` remains available because
the module is running as part of the kernel.

## Build and run the user application

```bash
cd ~/kernel-study/KernelvsUser/device_communication/user_space
make clean
make
./kernel_message_client
./kernel_message_client
```

Expected output:

```text
User application received: Hello from kernel space. Read number: 1
User application received: Hello from kernel space. Read number: 2
```

The counter belongs to the kernel module, so it remains available after the
first user process exits. Each application has its own user-space memory, while
the loaded module keeps its state in kernel memory.

## Unload and clean

```bash
sudo rmmod kernel_message
ls /dev/kernel_message
sudo dmesg | tail -n 5

cd ~/kernel-study/KernelvsUser/device_communication/kernel_space
make clean
cd ../user_space
make clean
```

After `rmmod`, `ls` should report that `/dev/kernel_message` no longer exists.
