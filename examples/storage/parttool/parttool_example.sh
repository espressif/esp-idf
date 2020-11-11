#!/usr/bin/env bash
#
# Demonstrates command-line interface of Partition Tool, parttool.py
#
#
# $1 - serial port where target device to operate on is connnected to, by default the first found valid serial port
# $2 - path to this example's built binary file (parttool.bin), by default $PWD/build/parttool.bin
PORT=$1
PARTTOOL_PY="python $IDF_PATH/components/partition_table/parttool.py -q"

if [[ "$PORT" != "" ]]; then
    PARTTOOL_PY="$PARTTOOL_PY --port $PORT"
fi

GEN_EMPTY_PARTITION_PY="python $IDF_PATH/components/partition_table/gen_empty_partition.py"

BINARY=$2

if [[ "$BINARY" == "" ]]; then
    BINARY=build/parttool.bin
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

# Read app partition and save the contents to a file. The app partition is identified
# using type-subtype combination
echo "Checking if device app binary matches built binary"
$PARTTOOL_PY read_partition --partition-type=app --partition-subtype=factory --output=app.bin
assert_file_same app.bin $BINARY "Device app binary does not match built binary"

# Retrieve info on data storage partition, this time identifying it by name.
offset=$($PARTTOOL_PY get_partition_info --partition-name=storage --info offset)
size=$($PARTTOOL_PY get_partition_info --partition-name=storage  --info size)
echo "Found data partition at offset $offset with size $size"

# Create a file whose contents will be written to the storage partition
head -c $(($size)) /dev/urandom > write.bin

# Write the contents of the created file to storage partition
echo "Writing to data partition"
$PARTTOOL_PY write_partition --partition-name=storage --input write.bin

# Read back the contents of the storage partition
echo "Reading data partition"
$PARTTOOL_PY read_partition --partition-name=storage --output read.bin

assert_file_same write.bin read.bin "Read contents of storage partition does not match source file contents"

# Erase contents of the storage partition
echo "Erasing data partition"
$PARTTOOL_PY erase_partition --partition-name=storage

# Read back the erased data partition
echo "Reading data partition"
$PARTTOOL_PY read_partition --partition-name=storage --output read.bin

# Generate a file of all 0xFF
$GEN_EMPTY_PARTITION_PY $(($size)) blank.bin

assert_file_same read.bin blank.bin "Contents of storage partition not fully erased"

# Example end and cleanup
printf "\nPartition tool operations performed successfully\n"
rm -rf app.bin read.bin blank.bin write.bin
