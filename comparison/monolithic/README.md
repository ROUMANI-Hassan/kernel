# Monolithic kernel studies

This folder contains the Linux examples for the monolithic-kernel study. The
examples use loadable modules because they can be built and tested without
compiling a complete Linux kernel.

Once loaded, module code executes in kernel space. `insmod` is only the
user-space tool that requests the load; the module can remain active after that
command exits.

## Examples

| Folder | Question tested |
|---|---|
| `kernel_module_safe/` | How does a module allocate and release memory correctly? |
| `kernel_module_unsafe/` | What happens when module initialization uses a failed allocation? |
| `KernelvsUser/device_communication/` | How does a user application request data from a kernel module? |
| `KernelvsUser/stackoverflow/` | How does the same stack error differ between user space and kernel space? |

Every example has its own README with its build, run, verification, and cleanup
commands.

## Safety

Run destructive examples only in a disposable VM with a snapshot. Do not load
unsafe modules on the host operating system. The hypervisor checks in the code
are an additional precaution, not a replacement for careful testing.

## Create the Lubuntu VM

Download the **64-bit PC (AMD64) desktop image** from the
[official Lubuntu 24.04 LTS page](https://download.cdimage.ubuntu.com/lubuntu/releases/24.04/release/).

From the `comparison` folder on the Linux host:

```bash
./scripts/create_lubuntu_vm.sh ~/Downloads/lubuntu-24.04.4-desktop-amd64.iso
```

Install Lubuntu in the VirtualBox window. The script registers the current
`comparison` folder as the read-only share `kernel-comparison`.

If the VM already exists, shut it down completely. Open a host terminal in the
repository's `comparison` folder, then recreate the share:

```bash
VBoxManage list vms
VBoxManage sharedfolder remove "Kernel-Monolithic-Lab" --name kernel-comparison 2>/dev/null || true
VBoxManage sharedfolder add "Kernel-Monolithic-Lab" --name kernel-comparison --hostpath "$PWD" --readonly --automount
```

Replace `Kernel-Monolithic-Lab` if `VBoxManage list vms` shows another name.

## Prepare the guest

Inside Lubuntu:

```bash
sudo apt update
sudo apt install -y virtualbox-guest-utils virtualbox-guest-x11 build-essential linux-headers-$(uname -r)
sudo usermod -aG vboxsf "$USER"
sudo reboot
```

After reboot, verify the automatic mount:

```bash
ls /media/sf_kernel-comparison/monolithic
```

If it is missing, mount it manually:

```bash
sudo mkdir -p /mnt/kernel-comparison
sudo mount -t vboxsf kernel-comparison /mnt/kernel-comparison
ls /mnt/kernel-comparison/monolithic
```

## Create a writable study copy

The shared folder is read-only, while compilation must create `.o` and `.ko`
files. Copy the complete monolithic folder into the guest home directory:

```bash
rm -rf ~/kernel-study
cp -r /media/sf_kernel-comparison/monolithic ~/kernel-study
```

If you used the manual mount, copy from `/mnt/kernel-comparison/monolithic`
instead. All example READMEs use `~/kernel-study` as their starting path.

## Current structure

```text
monolithic/
├── README.md
├── KernelvsUser/
│   ├── device_communication/
│   └── stackoverflow/
├── kernel_module_safe/
└── kernel_module_unsafe/
```
