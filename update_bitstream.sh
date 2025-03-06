#!/bin/bash

# Usage: update_bitstream.sh /path/to/bitstream.bin

# Check if the bitstream file was provided as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 /path/to/bitstream.bin"
    exit 1
fi

bitstream="$1"

# Check if the bitstream file exists
if [ ! -f "$bitstream" ]; then
    echo "Bitstream file does not exist."
    exit 1
fi

# The actual commands to program the FPGA
echo 0 > /sys/class/fpga_manager/fpga0/flags
mkdir -p /lib/firmware
cp "$bitstream" /lib/firmware/
echo "$(basename "$bitstream")" > /sys/class/fpga_manager/fpga0/firmware

echo "Bitstream programming complete."
