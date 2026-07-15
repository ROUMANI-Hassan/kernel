# Kernel studies

This repository is a practical study of operating-system kernels. Its purpose
is to move from architecture concepts to small experiments that can be built,
run, observed, and explained.

The main subjects are:

- monolithic, microkernel, and hybrid kernel architectures;
- kernel modules and device drivers;
- memory allocation and failure handling in kernel space;
- isolation boundaries and the effect of driver faults;
- reproducible tests in virtual machines;
- future Linux, MINIX, and Windows driver examples.

## Current work

The first study is in [`comparison/`](comparison/). It contains a real Linux
memory module and documentation comparing the three kernel architectures. The
current experiment shows normal allocation, safe allocation failure, and a
VM-only faulty-driver path.

Future studies will be added as separate folders under this repository.

## Safety

Kernel experiments can stop or damage a running operating system. Unsafe tests
in this repository must be run only in disposable virtual machines with a
snapshot. Do not run VM-only fault demonstrations on a physical host.

## License

This repository is available under the [MIT License](LICENSE). You may use,
copy, modify, and distribute it, but copies or substantial portions must retain
the copyright and license notice for Hassan Roumani.
