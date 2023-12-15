#!/bin/bash

# Load SPDK path from the config file
source ./spdk_config.sh

# Set the name of the NVMe device
NVME_DEVICE_NAME="nvme0n1"

# Check if NVMe device is managed by the kernel
if lsblk | grep -q "$NVME_DEVICE_NAME"; then
    echo "NVMe SSD ($NVME_DEVICE_NAME) is already in kernel."
else
    echo "NVMe SSD ($NVME_DEVICE_NAME) is managed by the usersapce. Attempting to unbind to kernel..."
    # Check if SPDK setup.sh script exists
    if [ -f "$SPDK_PATH/scripts/setup.sh" ]; then
        # Set HUGEMEM and execute the SPDK setup script
          $SPDK_PATH/scripts/setup.sh  reset
        echo "SPDK setup script executed."
    else
        echo "Error: SPDK setup script not found. Please check the SPDK path."
    fi
    
fi
