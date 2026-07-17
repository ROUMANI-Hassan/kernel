# Kernel architecture comparison

This study compares monolithic, microkernel, and hybrid operating-system
designs. It focuses on where system components run, how they communicate, and
what may happen when a driver fails.

| Design | Driver location | Communication | Failure isolation |
|---|---|---|---|
| Monolithic | Usually kernel space | Direct function calls | A serious driver fault can affect the kernel |
| Microkernel | Usually a user-space process | IPC messages | A failed driver can often be restarted separately |
| Hybrid | Kernel space and user space | Direct calls and IPC | The result depends on the component location |

## Monolithic architecture

```mermaid
flowchart TB
  A[Application] -->|system call| K[Monolithic kernel]
  K -->|direct call| D[Kernel driver]
  D --> H[Hardware]
  classDef app fill:#dbeafe,stroke:#2563eb,color:#172554
  classDef kernel fill:#fee2e2,stroke:#dc2626,color:#450a0a
  classDef driver fill:#fef3c7,stroke:#d97706,color:#451a03
  classDef hardware fill:#dcfce7,stroke:#16a34a,color:#052e16
  class A app
  class K kernel
  class D driver
  class H hardware
```

A monolithic driver shares the kernel execution environment. Linux can recover
from some driver errors, but severe faults can leave the kernel damaged or stop
the complete operating system.

## Microkernel architecture

```mermaid
flowchart TB
  A[Application] -->|system call| K[Small microkernel]
  K -->|IPC| D[User-space driver]
  K -->|IPC| S[User-space service]
  D --> H[Hardware]
  classDef app fill:#dbeafe,stroke:#2563eb,color:#172554
  classDef kernel fill:#fee2e2,stroke:#dc2626,color:#450a0a
  classDef service fill:#f3e8ff,stroke:#9333ea,color:#3b0764
  classDef hardware fill:#dcfce7,stroke:#16a34a,color:#052e16
  class A app
  class K kernel
  class D,S service
  class H hardware
```

A microkernel keeps many services in separate user-space processes. The system
can often stop and restart a failed service without stopping the microkernel.
This isolation requires additional IPC communication.

## Hybrid architecture

```mermaid
flowchart TB
  A[Application] -->|system call| K[Kernel core]
  K -->|direct call| D[Kernel driver]
  K -->|IPC| S[User-space service]
  S -->|request| D
  D --> H[Hardware]
  classDef app fill:#dbeafe,stroke:#2563eb,color:#172554
  classDef kernel fill:#fee2e2,stroke:#dc2626,color:#450a0a
  classDef driver fill:#fef3c7,stroke:#d97706,color:#451a03
  classDef service fill:#f3e8ff,stroke:#9333ea,color:#3b0764
  classDef hardware fill:#dcfce7,stroke:#16a34a,color:#052e16
  class A app
  class K kernel
  class D driver
  class S service
  class H hardware
```

A hybrid design keeps some components in the kernel and others in user space.
Its performance and failure isolation depend on where each component runs.

## Implemented studies

Only the Linux monolithic section currently contains executable examples. See
[`monolithic/README.md`](monolithic/README.md) for the environment and the list
of experiments.

Microkernel and hybrid implementations will be added as the study develops.

## License

This project is available under the repository [MIT License](../LICENSE).
