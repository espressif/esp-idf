| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# Deep Sleep Wake Stub Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The [Deep-sleep wake stub](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/deep-sleep-stub.html) is used to RTC fast boot mode that avoid the SPI flash booting, thus speeding up the wakeup process. This example demonstrates how to implement the wake stub.

In this example, the `CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` Kconfig option is used, which allows you to reduce the boot time of the bootloader during waking up from deep sleep. The bootloader stores in rtc memory the address of a running partition and uses it when it wakes up. This example allows you to skip all image checks and speed up the boot.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32/ESP32-S2/ESP32-S3/ESP32-C3 development board without any extra hardware if only **Timer** wake up sources are used.

### Configure the project


```
idf.py menuconfig
```

* **Wake up time** can be configured via `Example configuration > Wake up interval in seconds`
Wake up sources that are unused or unconnected should be disabled in configuration to prevent inadvertent triggering of wake up as a result of floating pins.


### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

On initial startup, this example will detect that this is the first boot and output the following log:

```
...
I (309) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
Not a deep sleep reset
Enabling timer wakeup, 10s
Entering deep sleep
```

The ESP chips will then enter deep sleep. When a timer wake up occurs, if deep sleep wake stub enabled, the ESP chips will boot from RTC memory and execute stub code. The output log such as the following:

```
...
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x5 (DSLEEP),boot:0x8 (SPI_FAST_FLASH_BOOT)
wake stub: wakeup count is 1, wakeup cause is 8
wake stub: going to deep sleep
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x5 (DSLEEP),boot:0x8 (SPI_FAST_FLASH_BOOT)
wake stub: wakeup count is 2, wakeup cause is 8
wake stub: going to deep sleep
ESP-ROM:esp32s3-20210327
Build:Mar 27 2021
rst:0x5 (DSLEEP),boot:0x8 (SPI_FAST_FLASH_BOOT)
wake stub: wakeup count is 3, wakeup cause is 8
wake stub: going to deep sleep
```
