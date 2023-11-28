#!/bin/bash
echo fb0-=0-= | sudo -S bash -c 'echo 800000 > /proc/sys/fs/file-max'
ulimit -n 800000

BASE_VALUE_SIZE=128
billion=1000000000
range_dividers=(1)

# Script path
SCRIPT_PATH="./binding_devices.sh"

# Check if the script exists
if [ -f "$SCRIPT_PATH" ]; then
    # Check if the script is executable
    if [ ! -x "$SCRIPT_PATH" ]; then
        echo "The script does not have execute permissions, attempting to grant them..."
        # Grant execute permission
        chmod +x "$SCRIPT_PATH"
    fi

    # Execute the script
    "$SCRIPT_PATH"
else
    echo "Error: The script does not exist at the specified path: $SCRIPT_PATH"
fi



convert_to_billion_format() {
    local num=$1
    local integer_part=$((num / billion))
    local decimal_part=$((num % billion))

    if ((decimal_part == 0)); then
        echo "${integer_part}B"
    else
        # Convert decimal part to the format of "x billionths"
        local formatted_decimal=$(echo "scale=9; $decimal_part / $billion" | bc | sed 's/0*$//' | sed 's/\.$//')
        # Extract only the fractional part after the decimal point
        formatted_decimal=$(echo $formatted_decimal | cut -d'.' -f2)
        echo "${integer_part}.${formatted_decimal}B"
    fi
}

for i in {2..2}; do
    base_num=$(($billion * $i))
    for divider in ${range_dividers[@]}; do
        for value_size in 128; do
            num_entries=$(($base_num * $BASE_VALUE_SIZE / $value_size))
            current_range=$(($num_entries / $divider))

            echo "base_num: $base_num"
            echo "num_entries: $num_entries"
            echo "current_range: $divider"
            echo "value_size:$value_size"
            cgexec -g memory:kv256 gdb --args ./TestNEXIO \
            --num=$num_entries \
            --value_size=$value_size \
            --range=$current_range \
            --benchmarks=fillrandom \
            --pci_address=0000:81:00.0 
        done
    done
done