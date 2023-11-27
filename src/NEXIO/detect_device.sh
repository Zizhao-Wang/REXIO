#!/bin/bash

# Check for NVMe devices in the system using lspci command
nvme_devices=$(lspci | grep -i "Non-Volatile memory controller")

# If nvme_devices variable is not empty, it means NVMe devices are present
if [[ ! -z "$nvme_devices" ]]; then
    echo "1"   # NVMe device(s) found in the system
else
    echo "0"   # No NVMe device found in the system
fi
