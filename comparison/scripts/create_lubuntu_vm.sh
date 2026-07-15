#!/usr/bin/env bash
set -euo pipefail
VM_NAME=Kernel-Monolithic-Lab
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ISO="${1:-}"
BASE="${HOME}/VirtualBox VMs"
DISK="$BASE/$VM_NAME/$VM_NAME.vdi"

if [[ -z "$ISO" || ! -f "$ISO" ]]; then
  echo "Usage: $0 /absolute/path/to/lubuntu.iso" >&2
  exit 1
fi

ISO="$(realpath "$ISO")"
VBoxManage showvminfo "$VM_NAME" >/dev/null 2>&1 && { echo "VM already exists"; exit 1; }
VBoxManage createvm --name "$VM_NAME" --ostype Ubuntu_64 --basefolder "$BASE" --register
VBoxManage modifyvm "$VM_NAME" --memory 4096 --cpus 2 --vram 128 --graphicscontroller vmsvga --boot1 dvd --boot2 disk --nic1 nat
VBoxManage createmedium disk --filename "$DISK" --size 25600 --format VDI --variant Standard
VBoxManage storagectl "$VM_NAME" --name SATA --add sata --controller IntelAhci
VBoxManage storageattach "$VM_NAME" --storagectl SATA --port 0 --device 0 --type hdd --medium "$DISK"
VBoxManage storageattach "$VM_NAME" --storagectl SATA --port 1 --device 0 --type dvddrive --medium "$ISO"
VBoxManage sharedfolder add "$VM_NAME" --name kernel-comparison --hostpath "$ROOT" --readonly --automount
VBoxManage startvm "$VM_NAME" --type gui
echo "The VM is running. Install Lubuntu in the VirtualBox window."
