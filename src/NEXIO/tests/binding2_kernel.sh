


#!/bin/bash


PCI_ALLOWED="0000:81:00.0"
SPDK_PATH="/root/downloads/spdk"
NVME_DEVICE="/dev/nvme0n1"
MOUNT_POINT=$(mount | grep $NVME_DEVICE | awk '{print $1}')



echo "尝试绑定 $NVME_DEVICE 到用户空间..."
$SPDK_PATH/scripts/setup.sh reset

echo "脚本执行完毕。"


#!/bin/bash

# Set the path to SPDK
SPDK_PATH="/root/downloads/spdk"

# Set the name of the NVMe device
NVME_DEVICE_NAME="nvme0n1"

# Check if NVMe device is managed by the kernel
if lsblk | grep -q "$NVME_DEVICE_NAME"; then
    echo "NVMe SSD ($NVME_DEVICE_NAME) is already in kernel."
else
    echo "NVMe SSD ($NVME_DEVICE_NAME) is managed by the usersapce. Attempting to unbind to kernel..."
    ./clear_data.sh
    # Check if SPDK setup.sh script exists
    if [ -f "$SPDK_PATH/scripts/setup.sh" ]; then
        # Set HUGEMEM and execute the SPDK setup script
        HUGEMEM=8192 PCI_ALLOWED="0000:81:00.0"  $SPDK_PATH/scripts/setup.sh
        echo "SPDK setup script executed."
    else
        echo "Error: SPDK setup script not found. Please check the SPDK path."
    fi
    
fi
