#!/usr/bin/env bash
#
# Demonstrates command-line interface of OTA Partitions Tool, otatool.py
#
#
# $1 - serial port where target device to operate on is connnected to, by default the first found valid serial port
# $2 - path to this example's built binary file (parttool.bin), by default $PWD/build/otatool.bin

PORT=$1
OTATOOL_PY="python $IDF_PATH/components/app_update/otatool.py -q"

if [[ "$PORT" != "" ]]; then
    OTATOOL_PY="$OTATOOL_PY --port $PORT"
fi

BINARY=$2

if [[ "$BINARY" == "" ]]; then
    BINARY=build/otatool.bin
fi

function assert_file_same()
{
    sz_a=$(stat -c %s $1)
    sz_b=$(stat -c %s $2)
    sz=$((sz_a < sz_b ? sz_a : sz_b))
    res=$(cmp -s -n $sz $1 $2) ||
        (echo "!!!!!!!!!!!!!!!!!!!"
        echo "FAILURE: $3"
        echo "!!!!!!!!!!!!!!!!!!!")
}

function assert_running_partition()
{
    running=$(python get_running_partition.py)
    if [[ "$running" != "$1" ]]; then
        echo "!!!!!!!!!!!!!!!!!!!"
        echo "FAILURE: Running partition '$running' does not match expected '$1'"
        echo "!!!!!!!!!!!!!!!!!!!"
        exit 1
    fi
}

# Flash the example firmware to OTA partitions. The first write uses slot number to identify OTA
# partition, the second one uses the name.
echo "Writing factory firmware to ota_0"
$OTATOOL_PY write_ota_partition --slot 0 --input $BINARY

echo "Writing factory firmware to ota_1"
$OTATOOL_PY write_ota_partition --name ota_1 --input $BINARY

# Read back the written firmware
$OTATOOL_PY read_ota_partition --name ota_0 --output app0.bin
$OTATOOL_PY read_ota_partition --slot 1 --output app1.bin

assert_file_same $BINARY app0.bin "Slot 0 app does not match factory app"
assert_file_same $BINARY app1.bin "Slot 1 app does not match factory app"

# Switch to factory app
echo "Switching to factory app"
$OTATOOL_PY erase_otadata
assert_running_partition factory

# Switch to slot 0
echo "Switching to OTA slot 0"
$OTATOOL_PY switch_ota_partition --slot 0
assert_running_partition ota_0

# Switch to slot 1 twice in a row
echo "Switching to OTA slot 1 (twice in a row)"
$OTATOOL_PY switch_ota_partition --slot 1
assert_running_partition ota_1
$OTATOOL_PY switch_ota_partition --name ota_1
assert_running_partition ota_1

# Switch to slot 0 twice in a row
echo "Switching to OTA slot 0 (twice in a row)"
$OTATOOL_PY switch_ota_partition --slot 0
assert_running_partition ota_0
$OTATOOL_PY switch_ota_partition --name ota_0
assert_running_partition ota_0

# Switch to factory app
echo "Switching to factory app"
$OTATOOL_PY erase_otadata
assert_running_partition factory

# Switch to slot 1
echo "Switching to OTA slot 1"
$OTATOOL_PY switch_ota_partition --slot 1
assert_running_partition ota_1

# Example end and cleanup
printf "\nPartition tool operations performed successfully\n"
rm -rf app0.bin app1.bin
