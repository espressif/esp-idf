| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Non-Volatile Storage (NVS) Read and Write Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to read and write a single integer value and a blob (binary large object) using NVS to preserve them between ESP module restarts.

  * value - tracks number of soft and hard restarts.
  * blob - contains a table with module run times. The table is read from NVS to dynamically allocated RAM. New run time is added to the table on each manually triggered soft restart and written back to NVS. Triggering is done by pulling down the boot mode pin (GPIO0 on ESP32 and ESP32-S2, GPIO9 on ESP32-C3).

Example also shows how to implement diagnostics if read / write operation was successful.

Detailed functional description of NVS and API is provided in [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_flash.html).

If not done already, consider checking simpler example *storage/nvs_rw_value*, that has been used as a starting point for preparing this one.

## How to use example

### Hardware required

This example can be run on most common development boards which have an active button connected to boot mode pin. On most boards, this button is labeled as "Boot". When pressed, the button connects boot mode pin to ground.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

First run:
```
Restart counter = 0
Run time:
Nothing saved yet!
```

At this point, press "Boot" button and hold it for a second. The board will perform software restart, printing:

```
Restarting...
```

After booting again, restart counter and run time array will be printed:

```
Restart counter = 1
Run time:
1: 5110
```

After pressing "Boot" once more:

```
Restart counter = 2
Run time:
1: 5110
2: 5860
```

To reset the counter and run time array, erase the contents of flash memory using `idf.py erase-flash`, then upload the program again as described above.
