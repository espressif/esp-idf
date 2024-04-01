| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# eFuse Example

## Overview

This example shows how to use the eFuse API. It demonstrates read and write operations with fields from the common and custom eFuse tables. For more information about eFuse, see the “eFuse Manager” chapter in the documentation and the “eFuse Controller” chapter in ESP32 TRM.

The eFuse is a single bit of non-volatile memory with the restriction that once an eFuse bit is programmed to 1, it can not be reverted to 0.
The eFuse fields can be useful to store: device types, serial numbers, some system variables, etc.

Note that the bits already written cannot be reset to the original state. For debugging purposes, the `CONFIG_EFUSE_VIRTUAL` option is provided. This option will block physical burning. All work happens with an array in RAM. Use `CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH` to keep eFuse changes after reboots.
In this example, all write operations are wrapped in `#ifdef CONFIG_EFUSE_VIRTUAL ... # endif` to prevent accidental burn while testing the features.

## How to use example

This example first reads the eFuse field, then writes a value, then reads it again. The program can be seen in the logs.

The program uses two eFuse tables:

 * The common table (for IDF purposes).
 * The custom table (for custom purposes).

The custom table has five fields:

```
MODULE_VERSION,                   EFUSE_BLK3,    56,  8,    Module version
DEVICE_ROLE,                      EFUSE_BLK3,    64,  3,    Device role
SETTING_1,                        EFUSE_BLK3,    67,  6,    Setting 1
SETTING_2,                        EFUSE_BLK3,    73,  5,    Setting 2
CUSTOM_SECURE_VERSION,            EFUSE_BLK3,    78, 16,    Custom secure version
```

### Hardware Required

To run this example, you need to use the ESP32 developer board. If your ESP32 has the `None` coding scheme of eFuse, then write and read operations have no limitations. But if it has the `3/4` or `RS` coding scheme, the writing may lead to an error due to the fact that some coded value was written earlier. A simple way to find out what coding scheme your chip has, you can see in the log while esptool is working or the log of the program:

```
esptool.py v2.7-dev
Serial port /dev/ttyUSB0
Connecting........_____.....__
Detecting chip type... ESP32
Chip is ESP32-PICO-D4 (revision 1)
Features: WiFi, BT, Dual Core, Embedded Flash, Coding Scheme None
MAC: d8:a0:1d:40:ac:90
```

The esptool also has a command to get general information about the chip `esptool.py chip_id`:

```
esptool.py v2.8-dev
Found 1 serial ports
Serial port /dev/ttyUSB0
Connecting........_
Detecting chip type... ESP32
Chip is ESP32-PICO-D4 (revision 1)
Features: WiFi, BT, Dual Core, Embedded Flash, Coding Scheme None
Crystal is 40MHz
MAC: d8:a0:1d:40:ac:90
Uploading stub...
Running stub...
Stub running...
Warning: ESP32 has no Chip ID. Reading MAC instead.
MAC: d8:a0:1d:40:ac:90
Hard resetting via RTS pin...
```

### Configure the project

```
idf.py menuconfig
```

In this example, there is `sdkconfig.default`, which already includes all the necessary settings. No need to configure.

 * `CONFIG_EFUSE_CUSTOM_TABLE=y` - It allows using the custom table.
 * `CONFIG_EFUSE_VIRTUAL=y` - All read and writes operations are redirected to RAM instead of eFuse registers. Not really burning eFuses.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.


## Example Output

For ``None`` coding scheme:


```
I (0) cpu_start: Starting scheduler on APP CPU.
I (323) example: Coding Scheme NONE
I (323) example: Uses common and custom tables
I (333) example: read efuse fields
I (333) example: 1. read MAC address: d8:a0:1d:40:ac:90
I (343) example: 2. read secure_version: 0
I (343) example: 3. read custom fields
I (353) example: module_version = 0
I (353) example: device_role = None
I (363) example: setting_1 = 0
I (363) example: setting_2 = 0
I (363) example: custom_secure_version = 0
W (373) example: This example does not burn any efuse in reality only virtually
W (383) example: Write operations in efuse fields are performed virtually
I (383) example: write custom efuse fields
W (393) efuse: Virtual efuses enabled: Not really burning eFuses
W (403) efuse: Virtual efuses enabled: Not really burning eFuses
W (403) efuse: Virtual efuses enabled: Not really burning eFuses
W (413) efuse: Virtual efuses enabled: Not really burning eFuses
W (423) efuse: Virtual efuses enabled: Not really burning eFuses
I (423) example: module_version = 1
I (433) example: device_role = Slave
I (433) example: setting_1 = 3
I (433) example: setting_2 = 4
I (443) example: custom_secure_version = 5
I (443) example: Done
```

And for ``3/4`` coding scheme:


```
I (0) cpu_start: Starting scheduler on APP CPU.
I (327) example: Coding Scheme 3/4
I (327) example: Uses common and custom tables
I (337) example: read efuse fields
I (337) example: 1. read MAC address: 84:0d:8e:18:8e:44
I (347) example: 2. read secure_version: 0
I (347) example: 3. read custom fields
I (357) example: module_version = 0
I (357) example: device_role = None
I (367) example: setting_1 = 0
I (367) example: setting_2 = 0
I (367) example: custom_secure_version = 0
W (377) example: This example does not burn any efuse in reality only virtually
W (387) example: Write operations in efuse fields are performed virtually
I (387) example: In the case of 3/4 coding scheme, you cannot write efuse fields separately
I (397) example: You should use the batch mode of writing fields for this
I (407) efuse: Batch mode of writing fields is enabled
W (417) efuse: Virtual efuses enabled: Not really burning eFuses
I (417) efuse: Batch mode of writing fields is disabled
I (427) example: module_version = 1
I (427) example: device_role = Slave
I (437) example: setting_1 = 3
I (437) example: setting_2 = 4
I (437) example: custom_secure_version = 5
I (447) example: Done
```


### How to create the new custom table

In the main folder of the example there is a custom eFuse table - `esp_efuse_custom_table.csv`. You can add there some fields and run `idf.py efuse-custom-table`, it will generate all the necessary source files. If all is ok then you will see:

```
...
Parsing efuse CSV input file /home/kostia/esp/esp-idf/components/efuse/esp32/esp_efuse_table.csv ...
Verifying efuse table...
Parsing efuse CSV input file /home/kostia/esp/esp-idf/examples/system/efuse/main/esp_efuse_custom_table.csv ...
Verifying efuse table...
Creating efuse *.h file /home/kostia/esp/esp-idf/examples/system/efuse/main/include/esp_efuse_custom_table.h ...
Creating efuse *.c file /home/kostia/esp/esp-idf/examples/system/efuse/main/esp_efuse_custom_table.c ...
Max number of bits in BLK 192
```

If the following fields were not changed in the CSV file: field_name, efuse_block, bit_start or bit_count, then a new generation will not occur, and you will see this message `Source files do not require updating correspond to csv file`.

This command will create into main folder two files: `esp_efuse_custom_table.c` and `include/esp_efuse_custom_table.h`.

Do not forget to add these files for the Cmake build system:

```
idf_component_register(SRCS "efuse_main.c" "esp_efuse_custom_table.c"
                       INCLUDE_DIRS "." "include")
```

If you are not sure which eFuse bits are free you can run `idf.py show-efuse-table`. It prints a sorted list of the common and custom tables.
