# Memory driver test

Use this real Linux module only in the disposable `Kernel-Monolithic-Lab` VM.
`memory_mb=1` succeeds. A large request normally returns `-ENOMEM`. With
`unsafe_on_failure=1`, the driver writes through NULL after that failure. In
the observed VM, Linux killed the current `insmod` task and continued running
after recording an oops. The unsafe branch first checks the CPU hypervisor flag
and refuses to run when no virtual machine is detected. Never use unsafe mode
on the physical host.
