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
import argparse
import os
import sys

PARTITION_TABLE_DIR = os.path.join('components', 'partition_table', '')


def assert_file_same(file1, file2, err):
    with open(file1, 'rb') as f1:
        with open(file2, 'rb') as f2:
            f1 = f1.read()
            f2 = f2.read()

            if len(f1) < len(f2):
                f2 = f2[:len(f1)]
            else:
                f1 = f1[:len(f2)]

            if not f1 == f2:
                raise Exception(err)


def main():
    COMPONENTS_PATH = os.path.expandvars(os.path.join('$IDF_PATH', 'components'))
    PARTTOOL_DIR = os.path.join(COMPONENTS_PATH, 'partition_table')

    sys.path.append(PARTTOOL_DIR)
    from gen_empty_partition import generate_blanked_file
    from parttool import PartitionName, PartitionType, ParttoolTarget

    parser = argparse.ArgumentParser('ESP-IDF Partitions Tool Example')

    parser.add_argument('--port', '-p', help='port where the device to perform operations on is connected')
    parser.add_argument('--binary', '-b', help='path to built example binary', default=os.path.join('build', 'parttool.bin'))

    args = parser.parse_args()

    target = ParttoolTarget(args.port)

    # Read app partition and save the contents to a file. The app partition is identified
    # using type-subtype combination
    print('Checking if device app binary matches built binary')
    factory = PartitionType('app', 'factory')
    target.read_partition(factory, 'app.bin')
    assert_file_same(args.binary, 'app.bin', 'Device app binary does not match built binary')

    # Retrieve info on data storage partition, this time identifying it by name.
    storage = PartitionName('storage')
    storage_info = target.get_partition_info(storage)
    print('Found data partition at offset 0x{:x} with size 0x{:x}'.format(storage_info.offset, storage_info.size))

    # Create a file whose contents will be written to the storage partition
    with open('write.bin', 'wb') as f:
        # Create a file to write to the data partition with randomly generated content
        f.write(os.urandom(storage_info.size))

    # Write the contents of the created file to storage partition
    print('Writing to data partition')
    target.write_partition(storage, 'write.bin')

    # Read back the contents of the storage partition
    print('Reading data partition')
    target.read_partition(storage, 'read.bin')

    assert_file_same('write.bin', 'read.bin', 'Read contents of storage partition does not match source file contents')

    # Erase contents of the storage partition
    print('Erasing data partition')
    target.erase_partition(storage)

    # Read back the erased data partition
    print('Reading data partition')
    target.read_partition(storage, 'read.bin')

    # Generate a file of all 0xFF
    generate_blanked_file(storage_info.size, 'blank.bin')

    assert_file_same('blank.bin', 'read.bin', 'Contents of storage partition not fully erased')

    # Example end and cleanup
    print('\nPartition tool operations performed successfully!')
    clean_files = ['app.bin', 'read.bin', 'blank.bin', 'write.bin']
    for clean_file in clean_files:
        os.unlink(clean_file)


if __name__ == '__main__':
    main()
