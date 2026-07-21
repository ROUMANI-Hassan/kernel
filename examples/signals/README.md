# Linux signals and signal handlers

This user-space example demonstrates the kernel's role in signal delivery.
It installs handlers with `sigaction()`, waits without busy-spinning, reacts to
`SIGUSR1`, and exits cleanly after `SIGINT` or `SIGTERM`.

The handler only updates `volatile sig_atomic_t` flags. The normal program loop
performs output and shutdown work because functions such as `printf()` are not
async-signal-safe.

## Build and run

```bash
make
./signal_demo
```

The program prints its PID. From another terminal, send an event:

```bash
kill -USR1 <PID>
```

Request a clean shutdown:

```bash
kill -TERM <PID>
```

You can also press `Ctrl-C`, which normally causes the terminal to send
`SIGINT` to the foreground process group.

## Observe the system calls

```bash
strace -e trace=rt_sigaction,rt_sigreturn,pause,kill ./signal_demo
```

Send `SIGUSR1` from a second terminal while `strace` is running. The trace
shows the handler registration, interruption of `pause()`, signal delivery and
return to the main program.

## Kernel relationship

```text
kill(pid, SIGUSR1)
        |
        v
kernel validates permission and marks SIGUSR1 pending
        |
        v
kernel checks the target thread's mask and disposition
        |
        v
kernel creates a signal frame and redirects user execution
        |
        v
signal_handler(SIGUSR1) runs in user mode
        |
        v
rt_sigreturn restores the interrupted execution context
```

The kernel manages signal generation, pending state, masks and delivery. The
registered handler itself is application code and executes in user mode.

## Why `SIGKILL` is not handled

`SIGKILL` and `SIGSTOP` cannot be caught, blocked or ignored, so this example
does not attempt to install handlers for them.
