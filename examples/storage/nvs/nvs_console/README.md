| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

## Overview

This example demonstrates how to use Non-Volatile Storage (NVS) through an interactive console interface. It provides a set of commands to read, write, and manage data in NVS.

## Hardware Required

This example can run on any ESP32 family development board.

## Configuration

The example can be configured through `menuconfig`:
1. Enable/disable command history storage (`CONFIG_CONSOLE_STORE_HISTORY`)
2. Configure UART parameters
3. Configure console prompt color settings

## How to Use

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

### Console Commands

The following commands are available:

1. NVS Operations:
   - `nvs_namespace  <namespace>` - Set current namespace
     - Example: `nvs_namespace storage`

   - `nvs_set <key> <type> -v <value>` - Set a value in NVS
     - type can be: i8, u8, i16, u16 i32, u32 i64, u64, str, blob
     - Example: `nvs_set counter i32 -v 42`
     - Example: `nvs_set name str -v "esp"`
     - Example: `nvs_set blob_val blob -v "657370"` - To set a blob value, provide the hex representation of the data you want to store. For example, 65 (e), 73 (s), 70 (p).

   - `nvs_get <key>` - Get a value from NVS
     - Example: `nvs_get counter`

   - `nvs_erase <key>` - Erase a key from NVS
     - Example: `nvs_erase counter`

   - `nvs_list  <partition> [-n <namespace>] [-t <type>]` - List stored key-value pairs stored in NVS. Use default partition name 'nvs' for listing the stored data.
     - Example: `nvs_list nvs` - This command lists all namespaces and their stored key-value pairs in the 'nvs' partition.
     - Example: `nvs_list nvs -n storage -t i8`

   - `nvs_erase_namespace <namespace>` -  Erases specified namespace
     - Example: `nvs_erase_namespace storage`

2. System Commands:
   - `help` - List all commands
   - `free` - Get the current size of free heap memory
   - `restart` - Software reset of the chip
   - `version` - Get the chip info together with ESP-IDF version used in the application
   - `heap` - Get minimum size of free heap memory that was available during program execution

### Example Output

``` bash
...
NVS Console Example
-------------------
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
Press Ctrl+C to exit the console.

nvs> version
IDF Version:v5.5-dev-2627-g2cbfce97768-dirt
Chip info:
	model:ESP32
	cores:2
	feature:/802.11bgn/BLE/BT/External-Flash:2 MB
	revision number:0
nvs> free
298172
nvs> heap
min heap size: 298156
nvs> nvs_list nvs
namespace 'storage_1', key 'u8_key', type 'u8'
namespace 'storage_1', key 'i8_key', type 'i8'
namespace 'storage_1', key 'u16_key', type 'u16'
namespace 'storage_2', key 'u32_key', type 'u32'
namespace 'storage_2', key 'i32_key', type 'i32'
namespace 'storage_2', key 'str_key', type 'str'
nvs> nvs_namespace storage_1
I (85497) cmd_nvs: Namespace set to 'storage_1'
nvs> nvs_get i8_key i8
-128
nvs> nvs_set i8_key i8 -v -50
I (233297) cmd_nvs: Value stored under key 'i8_key'
nvs> nvs_get i8_key i8
-50
nvs>
...
```
