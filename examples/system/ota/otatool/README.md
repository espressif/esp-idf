| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# OTA Tool Example

This example demonstrates common operations the OTA tool [otatool.py](../../../../components/app_update/otatool.py) allows the user to perform:

- reading, writing and erasing OTA partitions,
- switching boot partitions, and
- switching to factory partition.

Users taking a look at this example should focus on the contents of the Python script [otatool_example.py](otatool_example.py) or shell script [otatool_example.sh](otatool_example.sh). The scripts contain
programmatic invocation of the tool's functions via the Python API and command-line interface, respectively. Note
that on Windows, the shell script example requires a POSIX-compatible environment via MSYS2/Git Bash/WSL etc.

The built application in this example outputs the currently running partition, whose output is used to verify if the tool switched OTA
partitions successfully. The built application binary is written to all OTA partitions at the start of the example to be able to determine the running
partition for all switches performed.

## How to use example

### Build and Flash

Before running either of the example scripts, it is necessary to build and flash the firmware using the usual means:

```bash
idf.py build flash
```

### Running [otatool_example.py](otatool_example.py)

The example can be executed by running the script [otatool_example.py](otatool_example.py) or [otatool_example.sh](otatool_example.sh).

Python script:
```bash
python otatool_example.py
```

Shell script:
```
./otatool_example.sh
```

The script searches for valid target devices connected to the host and performs the operations on the first one it finds. This could present problems if there
are multiple viable target devices attached to the host. To perform the operations on a specific device, specify the port it is attached to during script invocation ("/dev/ttyUSB2" for example):

Python script:
```bash
python otatool_example.py --port /dev/ttyUSB2
```


Shell script:
```
./otatool_example.sh /dev/ttyUSB2
```

## Example output

Running the script produces the following output:

```
Writing factory firmware to ota_0
Writing factory firmware to ota_1
Switching to factory app
Switching to OTA slot 0
Switching to OTA slot 1 (twice in a row)
Switching to OTA slot 0 (twice in a row)
Switching to factory app
Switching to OTA slot 1

Partition tool operations performed successfully

```
