| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Non-Volatile Storage (NVS) Read and Write Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to read and write a single integer value using NVS.

In this example, value which is saved holds the number of ESP32 module restarts. Since it is written to NVS, the value is preserved between restarts.

Example also shows how to check if read / write operation was successful, or certain value is not initialized in NVS. Diagnostic is provided in plain text to help track program flow and capture any issues on the way.

Detailed functional description of NVS and API is provided in [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_flash.html).

Check another example *storage/nvs/nvs_rw_blob*, which shows how to read and write variable length binary data (blob).

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
...
I (296) nvs_example:
Opening Non-Volatile Storage (NVS) handle...
I (296) nvs_example:
Writing counter to NVS...
I (306) nvs_example:
Reading counter from NVS...
I (306) nvs_example: Read counter = 42
I (306) nvs_example:
Writing string to NVS...
I (306) nvs_example:
Reading string from NVS...
I (306) nvs_example: Read string: Hello from NVS!
I (316) nvs_example:
Finding keys in NVS...
I (316) nvs_example: Key: 'message', Type: str
I (316) nvs_example: Key: 'counter', Type: i32
I (326) nvs_example:
Deleting key from NVS...
I (336) nvs_example:
Committing updates in NVS...
I (336) nvs_example: NVS handle closed.
I (336) nvs_example: Returned to app_main
I (346) main_task: Returned from app_main()
...
```

To reset nvs data, erase the contents of flash memory using `idf.py erase-flash`, then upload the program again as described above.
