# OTA Tool Example

This example demonstrates common operations the OTA tool [otatool.py](../../../components/app_update/otatool.py) allows the user to perform:

- reading, writing and erasing OTA partitions,
- switching boot partitions, and
- switching to factory partition.

Users taking a look at this example should focus on the contents of the python script [otatool_example.py](otatool_example.py). The script contains programmatic invocations of the tool [otatool.py](../../../components/app_update/otatool.py) in Python for the operations mentioned above; and can serve as a guide for users wanting to do the same in their applications.

The built application in this example outputs the currently running partition, whose output is used to verify if the tool switched OTA
partitions succesfully. The built application binary is written to all OTA partitions at the start of the example to be able to determine the running
partition for all switches performed.

## How to use example

### Build and Flash

Before running the example script [otatool_example.py](otatool_example.py), it is necessary to build and flash the firmware using the usual means:

```bash
# If using Make
make build flash

# If using CMake
idf.py build flash
```

### Running [otatool_example.py](otatool_example.py)

The example can be executed by running the script [otatool_example.py](otatool_example.py). Either run it directly using

```bash
./otatool_example.py
```

or run it using

```bash
python otatool_example.py
```

The script searches for valid target devices connected to the host and performs the operations on the first one it finds. This could present problems if there 
are multiple viable target devices attached to the host. To perform the operations on a specific device, specify the port it is attached to during script invocation:

```bash
# The target device is attached to /dev/ttyUSB2, for example
python otatool_example.py --port /dev/ttyUSB2
```

## Example output

Running the script produces the following output:

```
Writing factory firmware to ota_0
Writing factory firmware to ota_1
Checking written firmware to ota_0 and ota_1 match factory firmware
Switching to ota partition name factory
Switching to ota partition name factory
Switching to ota partition slot 0
Switching to ota partition name ota_1
Switching to ota partition slot 1
Switching to ota partition name ota_0
Switching to ota partition slot 0
Switching to ota partition name factory
Switching to ota partition slot 1

OTA tool operations executed successfully!
```
