# Linux kernel temperature monitor

This educational module monitors one real temperature sensor through the
Linux thermal framework. It reads the temperature every few seconds, detects
threshold crossings, writes warnings to the kernel log, and publishes its
current status in `/proc/temp_monitor`.

The project demonstrates:

- how Linux identifies thermal zones;
- periodic work with a delayed workqueue;
- configurable kernel-module parameters;
- kernel logging without repeating the same warning continuously;
- a small read-only procfs interface between kernel space and user space.

## Important design choice

Linux exposes thermal information to applications below `/sys/class/thermal`.
Kernel code should not open and parse those sysfs files. This module instead
uses the thermal framework directly and asks for a zone by its `type` name.

Temperature policy is normally better implemented with the existing Linux
thermal framework or a user-space service. This module is a learning example,
not a replacement for the system's thermal protection.

## 1. Install the build tools

On Ubuntu or Debian:

```bash
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r)
```

## 2. Find a sensor

```bash
chmod +x scripts/list_thermal_zones.sh
./scripts/list_thermal_zones.sh
```

Example:

```text
ZONE               TYPE                     TEMPERATURE
thermal_zone2      x86_pkg_temp             58.000 C
```

Use the value in the `TYPE` column, such as `x86_pkg_temp`. A virtual machine
often has no thermal zones because VirtualBox does not normally pass the
host's CPU sensors to the guest. Use the physical Linux host for a real sensor
test; this module only reads and reports data and does not control the CPU.

## 3. Build the module

```bash
make
```

This creates `temp_monitor.ko`.

## 4. Load it

Replace `x86_pkg_temp` with the sensor type found in step 2:

```bash
sudo insmod ./temp_monitor.ko zone=x86_pkg_temp warning_c=75 critical_c=85 interval_ms=2000
```

`insmod` asks the kernel to load the module. The monitoring code itself then
runs inside the kernel. It uses delayed work to take another measurement every
two seconds; `insmod` does not remain running.

## 5. Read the result

```bash
cat /proc/temp_monitor
sudo dmesg | grep temp_monitor
```

The proc file contains the current and maximum temperature, the current state,
the number of samples, and the number of threshold transitions. The log shows
when the state changes between `normal`, `warning`, and `critical`.

The thresholds can be changed while the module is loaded:

```bash
echo 65 | sudo tee /sys/module/temp_monitor/parameters/warning_c
echo 75 | sudo tee /sys/module/temp_monitor/parameters/critical_c
```

Keep the warning value lower than the critical value.

## 6. Stop and clean

```bash
sudo rmmod temp_monitor
make clean
```

`rmmod` first waits for the scheduled work to stop, removes the proc entry, and
then unloads the module.

## Expected behavior

| Temperature | State | Kernel message |
|---|---|---|
| Below 75 C | normal | Only when returning from a higher state |
| 75 to 84.999 C | warning | One warning when entering this state |
| 85 C or higher | critical | One error when entering this state |

The module does not shut down the computer, change fan speed, or override the
machine's existing thermal protection.

The module source is GPL-2.0-only because it integrates with GPL kernel APIs.
The repository's other material remains covered by its top-level license.

## Project files

```text
temperature_monitor/
├── Makefile
├── README.md
├── scripts/
│   └── list_thermal_zones.sh
└── temp_monitor.c
```
