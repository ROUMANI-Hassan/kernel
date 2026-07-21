# Kernel examples

This folder contains independent practical examples created while studying
Linux kernel concepts. Each project has its own explanation and commands.

## Current examples

- [`temperature_monitor/`](temperature_monitor/) monitors a real Linux thermal
  zone with a kernel module and exposes its status through procfs.
- [`signals/`](signals/) demonstrates how the kernel delivers `SIGUSR1` to a
  user-space signal handler and how to request a clean shutdown with `SIGTERM`.
