| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# ULP-RISC-V ADC Example

This example demonstrates how to use the ULP-RISC-V coprocessor to poll the ADC in deep sleep.

The ULP program periodically measures the input voltage on two ADC channels, EXAMPLE_ADC_CHANNEL_0 and EXAMPLE_ADC_CHANNEL_1 (by default ADC1 channels 0 and 1, GPIO1 and GPIO2 on both ESP32-S2 and ESP32-S3). Each voltage is compared to an upper threshold. If the voltage on either channel is higher than the threshold, the ULP wakes up the system.

By default, the threshold is set to 1.75V, approximately.

In this example, you need to connect a voltage source (e.g. a DC power supply) to the GPIO pins corresponding to the ADC channels specified in `ulp/example_config.h` (see the macros defined on the top of the header file). Feel free to modify the channel settings.

### Hardware Required

* A development board with a SOC which has a RISC-V ULP coprocessor (e.g., ESP32-S2 Saola)
* A USB cable for power supply and programming

## Example output

Below is the output from this example.

```
Not a ULP-RISC-V wakeup (cause = 0), initializing it!
Entering in deep sleep

ULP-RISC-V woke up the main CPU
Threshold: high = 2000
Channel 0 value = 4095 was above threshold
Channel 1 value = 1024
Entering in deep sleep
```

