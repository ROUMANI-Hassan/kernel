# My kernel studies

This repository documents my personal study of operating-system kernels. I am
using it to record what I learn over time, test ideas with small practical
examples, correct my understanding when results are different from what I
expected, and show my progress.

This is a learning project, not a finished reference guide. The explanations
and examples will develop as I study more kernel concepts and implement new
experiments.

The main subjects are:

- monolithic, microkernel, and hybrid kernel architectures;
- kernel modules and device drivers;
- memory allocation and failure handling in kernel space;
- isolation boundaries and the effect of driver faults;
- reproducible tests in virtual machines;
- future Linux, MINIX, and Windows driver examples.

## Current work

The first study is in [`comparison/`](comparison/). It contains small Linux
kernel modules and documentation comparing the three architectures. The first
example studies memory handling. The next runs the same stack-overflow error in
user space and kernel space to compare their isolation boundaries.

Future studies will be added as separate folders. Each folder will document
the question I studied, the code I wrote, the results I observed, and what I
learned from the experiment.

## Safety

Kernel experiments can stop or damage a running operating system. Unsafe tests
in this repository must be run only in disposable virtual machines with a
snapshot. Do not run VM-only fault demonstrations on a physical host.

## License

This repository is available under the [MIT License](LICENSE). You may use,
copy, modify, and distribute it, but copies or substantial portions must retain
the copyright and license notice for Hassan Roumani.
