#!/bin/bash

# Set the path to SPDK
SPDK_PATH="/root/downloads/spdk"

# Set the name of the NVMe device
NVME_DEVICE_NAME="nvme0n1"

# Check if NVMe device is managed by the kernel
if lsblk | grep -q "$NVME_DEVICE_NAME"; then
    echo "NVMe SSD ($NVME_DEVICE_NAME) is managed by the kernel. Attempting to bind to userspace..."

    # Check if SPDK setup.sh script exists
    if [ -f "$SPDK_PATH/scripts/setup.sh" ]; then
        # Set HUGEMEM and execute the SPDK setup script
        HUGEMEM=8192 PCI_ALLOWED="0000:00:05.0"  $SPDK_PATH/scripts/setup.sh
        echo "SPDK setup script executed."
    else
        echo "Error: SPDK setup script not found. Please check the SPDK path."
    fi
else
    echo "NVMe SSD ($NVME_DEVICE_NAME) is already in userspace."
fi
