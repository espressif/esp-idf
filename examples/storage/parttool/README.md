# Partitions Tool Example

This example demonstrates common operations the partitions tool [parttool.py](../../../components/partition_table/parttool.py) allows the user to perform:

- reading, writing and erasing partitions,
- retrieving info on a certain partition,
- dumping the entire partition table, and 
- generating a blank partition file.

Users taking a look at this example should focus on the contents of the python script [parttool_example.py](parttool_example.py). The script contains programmatic invocations of [parttool.py](../../../components/partition_table/parttool.py) in Python for the operations mentioned above; and can serve as a guide for users wanting to do the same in their applications.

The example performs the operations mentioned above in a straightforward manner: it performs writes to partitions and then verifies correct content
by reading it back. For partitions, contents are compared to the originally written file. For the partition table, contents are verified against the partition table CSV
file. An erased partition's contents is compared to a generated blank file.

## How to use example

### Build and Flash

Before running the example script [parttool_example.py](parttool_example.py), it is necessary to build and flash the firmware using the usual means:

```bash
# If using Make
make build flash

# If using CMake
idf.py build flash
```

### Running [parttool_example.py](parttool_example.py)

The example can be executed by running the script [parttool_example.py](parttool_example.py). Either run it directly using

```bash
./parttool_example.py
```

or run it using

```bash
python parttool_example.py
```

The script searches for valid target devices connected to the host and performs the operations on the first one it finds. To perform the operations on a specific device, specify the port it is attached to during script invocation:

```bash
# The target device is attached to /dev/ttyUSB2, for example
python parttool_example.py --port /dev/ttyUSB2
```

## Example output

Running the script produces the following output:

```
Checking if device app binary matches built binary
Checking if device partition table matches partition table csv
Retrieving data partition offset and size
Found data partition at offset 0x110000 with size 0x10000
Writing to data partition
Reading data partition
Erasing data partition
Generating blank data partition file
Reading data partition

Partition tool operations performed successfully!
```

