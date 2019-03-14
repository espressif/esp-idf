#!/usr/bin/env python
#
# Demonstrates the use of parttool.py, a tool for performing partition level
# operations.
#
# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import os
import sys
import subprocess
import argparse

IDF_PATH = os.path.expandvars("$IDF_PATH")

PARTTOOL_PY = os.path.join(IDF_PATH, "components", "partition_table", "parttool.py")

PARTITION_TABLE_OFFSET = 0x8000

INVOKE_ARGS = [sys.executable, PARTTOOL_PY, "-q", "--partition-table-offset", str(PARTITION_TABLE_OFFSET)]


def sized_file_compare(file1, file2):
    with open(file1, "rb") as f1:
        with open(file2, "rb") as f2:
            f1 = f1.read()
            f2 = f2.read()

            if len(f1) < len(f2):
                f2 = f2[:len(f1)]
            else:
                f1 = f1[:len(f2)]

            return f1 == f2


def check(condition, message):
    if not condition:
        print("Error: " + message)
        sys.exit(1)


def write_data_partition(size):
    print("Writing to data partition")
    with open("write.bin", "wb") as f:
        # Create a file to write to the data partition with randomly generated content
        f.write(os.urandom(int(size, 16)))

        # Invokes the command
        #
        # parttool.py --partition-table-offset 0x8000 -q --partition-name storage write_partition --input write.bin
        #
        # to write the contents of a file to a partition in the device.
        invoke_args = INVOKE_ARGS + ["--partition-name", "storage", "write_partition", "--input", f.name]
        subprocess.check_call(invoke_args)
        return f.name


def read_data_partition():
    print("Reading data partition")
    # Invokes the command
    #
    # parttool.py --partition-table-offset 0x8000 -q --partition-name storage read_partition --output read.bin
    #
    # to read the contents of a partition in the device, which is then written to a file.
    f = "read.bin"
    invoke_args = INVOKE_ARGS + ["--partition-name", "storage", "read_partition", "--output", f]
    subprocess.check_call(invoke_args)
    return f


def get_data_partition_info():
    print("Retrieving data partition offset and size")
    # Invokes the command
    #
    # parttool.py --partition-table-offset 0x8000 -q --partition-name storage get_partition_info --info offset size
    #
    # to get the offset and size of a partition named 'storage'.
    invoke_args = INVOKE_ARGS + ["--partition-name", "storage", "get_partition_info", "--info", "offset", "size"]

    (offset, size) = subprocess.check_output(invoke_args).strip().split(b" ")
    return (offset, size)


def check_app(args):
    print("Checking if device app binary matches built binary")
    # Invokes the command
    #
    # parttool.py --partition-table-offset 0x8000 --partition-type app --partition-subtype factory read_partition --output app.bin"
    #
    # to read the app binary and write it to a file. The read app binary is compared to the built binary in the build folder.
    invoke_args = INVOKE_ARGS + ["--partition-type", "app", "--partition-subtype", "factory", "read_partition", "--output", "app.bin"]
    subprocess.check_call(invoke_args)

    app_same = sized_file_compare("app.bin", args.binary)
    check(app_same, "Device app binary does not match built binary")


def check_partition_table():
    sys.path.append(os.path.join(IDF_PATH, "components", "partition_table"))
    import gen_esp32part as gen

    print("Checking if device partition table matches partition table csv")
    # Invokes the command
    #
    # parttool.py --partition-table-offset 0x8000 get_partition_info --table table.bin
    #
    # to read the device partition table and write it to a file. The read partition table is compared to
    # the partition table csv.
    invoke_args = INVOKE_ARGS + ["get_partition_info", "--table", "table.bin"]
    subprocess.check_call(invoke_args)

    with open("table.bin", "rb") as read:
        partition_table_csv = os.path.join(IDF_PATH, "examples", "storage", "parttool", "partitions_example.csv")
        with open(partition_table_csv, "r") as csv:
            read = gen.PartitionTable.from_binary(read.read())
            csv = gen.PartitionTable.from_csv(csv.read())
            check(read == csv, "Device partition table does not match csv partition table")


def erase_data_partition():
    print("Erasing data partition")
    # Invokes the command
    #
    # parttool.py --partition-table-offset 0x8000 --partition-name storage erase_partition
    #
    # to erase the 'storage' partition.
    invoke_args = INVOKE_ARGS + ["--partition-name", "storage", "erase_partition"]
    subprocess.check_call(invoke_args)


def generate_blank_data_file():
    print("Generating blank data partition file")

    # Invokes the command
    #
    # parttool.py --partition-table-offset 0x8000 --partition-name storage generate_blank_partition_file --output blank.bin
    #
    # to generate a blank partition file and write it to a file. The blank partition file has the same size as the
    # 'storage' partition.
    f = "blank.bin"
    invoke_args = INVOKE_ARGS + ["--partition-name", "storage", "generate_blank_partition_file", "--output", f]
    subprocess.check_call(invoke_args)
    return f


def main():
    global INVOKE_ARGS

    parser = argparse.ArgumentParser("ESP-IDF Partitions Tool Example")

    parser.add_argument("--port", "-p", help="port where the device to perform operations on is connected")
    parser.add_argument("--binary", "-b", help="path to built example binary", default=os.path.join("build", "parttool.bin"))

    args = parser.parse_args()

    if args.port:
        INVOKE_ARGS += ["--port", args.port]

    # Before proceeding, do checks to verify whether the app and partition table in the device matches the built binary and
    # the generated partition table during build
    check_app(args)
    check_partition_table()

    # Get the offset and size of the data partition
    (offset, size) = get_data_partition_info()

    print("Found data partition at offset %s with size %s" % (offset, size))

    # Write a generated file of random bytes to the found data partition
    written = write_data_partition(size)

    # Read back the contents of the data partition
    read = read_data_partition()

    # Compare the written and read back data
    data_same = sized_file_compare(read, written)
    check(data_same, "Read contents of the data partition does not match written data")

    # Erase the data partition
    erase_data_partition()

    # Read back the erase data partition, which should be all 0xFF's after erasure
    read = read_data_partition()

    # Generate blank partition file (all 0xFF's)
    blank = generate_blank_data_file()

    # Verify that the partition has been erased by comparing the contents to the generated blank file
    data_same = sized_file_compare(read, blank)
    check(data_same, "Erased data partition contents does not match blank partition file")

    print("\nPartition tool operations performed successfully!")


if __name__ == '__main__':
    main()
