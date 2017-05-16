# ULP ADC Example

This example demonstrates how to use the ULP coprocessor to poll ADC in deep sleep.

ULP program periodically measures the input voltage on GPIO34. The voltage is compared to two thresholds. If the voltage is less than the low threshold, or higher than the high threshold, ULP wakes up the system.

By default, thresholds are set to 1.35V and 1.75V, approximately.

Average current drawn by the ESP32 in this example with the default configuration (10Hz measurement period, 4x averaging) is 80 uA.

## Example output

Below is the output from this example. GPIO15 is pulled down to ground to supress output from ROM bootloader.

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
