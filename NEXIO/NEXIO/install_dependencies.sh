#!/bin/bash

# Function to check if a package is installed
is_package_installed() {
    local package_name=$1
    dpkg -s "$package_name" &> /dev/null
    return $?
}

# Function to install a package if it is not already installed
install_package() {
    local package_name=$1
    if is_package_installed "$package_name"; then
        echo "$package_name is already installed."
    else
        echo "Installing $package_name..."
        sudo apt-get install -y "$package_name"
    fi
}

# List of dependencies
dependencies=("jq" "g++" "make" "libgflags-dev" "cgroup-tools" "cpufrequtils")

# Check if all dependencies are installed
all_installed=true
for dep in "${dependencies[@]}"; do
    if ! is_package_installed "$dep"; then
        all_installed=false
        break
    fi
done

# Update the package list if needed
if [ "$all_installed" = false ]; then
    echo "Updating package list..."
    sudo apt-get update
fi

# Install dependencies
for dep in "${dependencies[@]}"; do
    install_package "$dep"
done

echo "All dependencies are installed."
