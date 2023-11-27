#!/bin/bash
echo fb0-=0-= | sudo -S bash -c 'echo 800000 > /proc/sys/fs/file-max'
ulimit -n 800000

BASE_VALUE_SIZE=128
billion=20000000
range_dividers=(1 4 8)

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

for i in {1..1}; do
    base_num=$(($billion * $i))
    dir1="${i}B"
    if [ ! -d "$dir1" ]; then
        mkdir $dir1
    fi
    for divider in ${range_dividers[@]}; do
        dir2="${dir1}/divider_$divider"
        if [ ! -d "$dir2" ]; then
            mkdir $dir2
        fi
        for value_size in 128 256 512 1024 2048; do
            dir3="${dir2}/value_size_$value_size"
            if [ ! -d "$dir3" ]; then
                mkdir $dir3
            fi
            num_entries=$(($base_num * $BASE_VALUE_SIZE / $value_size))
            current_range=$(($num_entries / $divider))

            num_format=$(convert_to_billion_format $num_entries)

            log_file="NEXIO_${num_format}_ran_${divider}_val_${value_size}.log"
            cd $dir3
            if [ -f "$log_file" ]; then
                echo "Log file $log_file already exists. Skipping this iteration."
                cd ../../..
                continue
            fi
            echo "base_num: $base_num"
            echo "num_entries: $num_entries"
            echo "current_range: $divider"
            echo "value_size:$value_size"
            echo "$num_format"
            cgexec -g memory:kv256 ../../../TestNEXIO \
            --num=$num_entries \
            --value_size=$value_size \
            --range=$current_range \
            --benchmarks=fillrandom\
            # | tee $log_file  \
            # | awk -v n="$num_format" -v r="$divider" -v v="$value_size" '{print n, r, v, $0}' >> ../../../../write_amplification.txt
            cd ../../..
        done
    done
done