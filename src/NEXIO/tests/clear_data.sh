#!/bin/bash


# Load SPDK path from the config file
source ./spdk_config.sh
NVME_DEVICE="/dev/nvme0n1"

# Find the mount point of the NVMe device
MOUNT_POINT=$(mount | grep $NVME_DEVICE | awk '{print $1}')

# Unmount the device if it is mounted
if [ ! -z "$MOUNT_POINT" ]; then
    echo "Unmounting $MOUNT_POINT ..."
    umount $MOUNT_POINT
    echo "Unmounted successfully."
fi

# Clear data on the NVMe device
echo "Wiping data on $NVME_DEVICE..."
wipefs -a /dev/nvme0n1

echo "Script execution complete."

