| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |

# ULP ADC Example

This example demonstrates how to use the ULP coprocessor to poll ADC in deep sleep.

ULP program periodically measures the input voltage on ADC_CHANNEL_6 (GPIO34 on ESP32, GPIO7 on ESP32-S3). The voltage is compared to two thresholds. If the voltage is less than the low threshold, or higher than the high threshold, ULP wakes up the system.

By default, thresholds are set to 1.35V and 1.75V, approximately.

GPIO15 is connected to ground to supress output from ROM bootloader.

Average current drawn by the ESP32 in this example (with the default 4x averaging) depending on the measurement frequency is as follows:

Measurement frequency, Hz | Average current, uA
--------------------------|---------------------
  10                      | 8.5
  50                      | 20
 100                      | 37

In this example, the `CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` Kconfig option is used, which allows you to reduce the boot time of the bootloader during waking up from deep sleep. The bootloader stores in rtc memory the address of a running partition and uses it when it wakes up. This example allows you to skip all image checks and speed up the boot.

## Example output

Below is the output from this example.

```
Not ULP wakeup
Entering deep sleep

Deep sleep wakeup
ULP did 1531 measurements
Thresholds:  low=1500  high=2000
Value=973 was below threshold
Entering deep sleep

Deep sleep wakeup
ULP did 2 measurements
Thresholds:  low=1500  high=2000
Value=0 was below threshold
Entering deep sleep

Deep sleep wakeup
ULP did 2 measurements
Thresholds:  low=1500  high=2000
Value=0 was below threshold
Entering deep sleep
```
