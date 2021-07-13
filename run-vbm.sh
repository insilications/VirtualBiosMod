#!/bin/bash
export StartupDelay=0

# /insilications/build/custom-apps/VirtualBiosMod/tools/uefi-run --bios /insilications/build/custom-apps/VirtualBiosMod/tools/OVMF.fd --qemu qemu-system-x86_64 /insilications/build/custom-apps/VirtualBiosMod/VirtualBiosMod.efi

# /insilications/build/custom-apps/VirtualBiosMod/tools/uefi-run --bios /insilications/build/custom-apps/VirtualBiosMod/OVMF.fd --qemu qemu-system-x86_64 /insilications/build/custom-apps/VirtualBiosMod/VirtualBiosMod.efi

/insilications/build/custom-apps/VirtualBiosMod/tools/uefi-run --bios /usr/share/qemu/OVMF.fd --qemu qemu-system-x86_64 /insilications/build/custom-apps/VirtualBiosMod/VirtualBiosMod.efi
