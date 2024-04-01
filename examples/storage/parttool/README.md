| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Partitions Tool Example

This example demonstrates common operations the partitions tool [parttool.py](../../../components/partition_table/parttool.py) allows the user to perform:

- reading, writing and erasing partitions,
- retrieving info on a certain partition,
- dumping the entire partition table

Users taking a look at this example should focus on the contents of the Python script [parttool_example.py](parttool_example.py) or shell script [parttool_example.sh](parttool_example.sh). The scripts contain
programmatic invocation of the tool's functions via the Python API and command-line interface, respectively. Note
that on Windows, the shell script example requires a POSIX-compatible environment via MSYS2/Git Bash/WSL etc.

The example performs the operations mentioned above in a straightforward manner: it performs writes to partitions and then verifies correct content
by reading it back. For partitions, contents are compared to the originally written file. For the partition table, contents are verified against the partition table CSV
file. An erased partition's contents is compared to a generated blank file.

## How to use example

### Build and Flash

Before running either of the example scripts, it is necessary to build and flash the firmware using the usual means:

```bash
idf.py build flash
```

### Running [parttool_example.py](parttool_example.py)

The example can be executed by running the script [parttool_example.py](parttool_example.py) or [parttool_example.sh](parttool_example.sh).

Python script:
```bash
python parttool_example.py
```

Shell script:
```
./parttool_example.sh
```

The script searches for valid target devices connected to the host and performs the operations on the first one it finds. To perform the operations on a specific device, specify the port it is attached to during script invocation:

Python script:
```bash
python parttool_example.py --port /dev/ttyUSB2
```

Shell script:
```
./parttool_example.sh /dev/ttyUSB2
```

## Example output

Running the script produces the following output:

```
Checking if device app binary matches built binary
Found data partition at offset 0x110000 with size 0x10000
Writing to data partition
Reading data partition
Erasing data partition
Reading data partition

Partition tool operations performed successfully!
```

