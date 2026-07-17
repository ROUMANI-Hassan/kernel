# User-space stack overflow

This program repeatedly calls the same function. Every call keeps a 1 KiB
array on the process stack. Eventually the stack reaches its protected boundary
and Linux sends `SIGSEGV` to this process.

```bash
cd ~/kernel-study/KernelvsUser/stackoverflow/user_space
make clean
make
ulimit -c 0
./user_stack_overflow
echo "The operating system is still running"
make clean
```

Expected ending:

```text
Segmentation fault (core dumped)
```

The program stops, but the terminal, kernel, and other applications continue.
