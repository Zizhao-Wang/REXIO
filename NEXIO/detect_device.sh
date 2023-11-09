#!/bin/bash

# Check devices bound to uio_pci_generic driver
uio_devices=$(ls /sys/bus/pci/drivers/uio_pci_generic/ | grep -E "^[0-9a-f]{4}:")

# If uio_devices has content, a device has been setup by setup.sh
if [[ ! -z "$uio_devices" ]]; then
    echo "1"   # Device is setup by setup.sh
else
    echo "0"   # No device has been setup by setup.sh
fi
