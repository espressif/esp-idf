| Supported Targets | ESP32-H21 | ESP32-H4 |
| ----------------- | --------- | -------- |

# VDDBAT Under-voltage Protection Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

A lithium cell loses capacity irreversibly when it is discharged too far. This example installs
VDDBAT under-voltage detection and, from the callback, shuts the chip down into deep sleep once
VDDBAT drops below ~3.0 V. The chip wakes again once the battery is charged back above that level.
Deep sleep is the application's choice; the driver only reports the under-voltage.

## How to use example

### Hardware Required

An ESP32-H4 or ESP32-H21 board powered from a single lithium cell on VDDBAT.

### Configure the project

```
idf.py set-target <chip_name>
idf.py menuconfig
```

The feature is selected under `Component config → Hardware Settings → Power Supplier →
Under-voltage protection → VDDBAT under-voltage interrupt`. It is already enabled by
this example's `sdkconfig.defaults`. Selecting it disables the hardware brownout reset.

### Build and Flash

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use. To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

On a normal boot with a healthy battery:

```
I (300) main_task: Calling app_main()
I (310) vddbat_example: VDDBAT protection enabled, waiting for under voltage
```

When the battery drops below the threshold:

```
W (85320) vddbat_example: VDDBAT under voltage, shutting down
```

The chip then enters deep sleep and stays there until VDDBAT rises above the threshold again,
after which it boots and reports the wakeup cause:

```
ESP-ROM:esp32h4-eco2-20260611
Build:Jun 11 2026
rst:0x5 (SLEEP_WAKEUP),boot:0xe (SPI_FAST_FLASH_BOOT)
...
```
