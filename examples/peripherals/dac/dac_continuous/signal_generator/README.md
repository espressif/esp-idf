| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# DAC Continuous Signal Generator Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates how to continuously output waveforms using the DAC. There are two general approaches: DMA transfer and timer interrupts.

### DMA Transfer

Waveform samples are filled into a DMA buffer and continuously transferred to the DAC by DMA. Therefore, the sample (or DAC code) update rate is equal to the DMA transfer rate. This approach reduces CPU load and supports relatively high update rates. However, due to DMA clock limitations, very low update rates may not be achievable. In addition, due to the analog performance limitations of the DAC, excessively high frequencies may result in waveform distortion.

### Timer Interrupts

This approach essentially calls the DAC `oneshot` API from a timer interrupt callback. Therefore, the sample (or DAC code) update rate is determined by the timer interrupt frequency. Note that frequent interrupts can impose significant CPU overhead, so this approach is only suitable for relatively low update rates.

In general, the DMA-based approach is recommended for most applications, unless the DMA peripheral is already occupied or the required sample update rate is too low.

## How to use the Example

### Hardware Required

* A development board with ESP32 or ESP32-S2 SoC
* DAC channel to GPIO mapping: see [GPIO Summary](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html#gpio-summary) (switch the chip target on the documentation page if needed)
* Note that some ESP32-S2 DevKits have an LED on the DAC1 pin, so that channel's output may not go fully low
* (Optional) An oscilloscope to monitor the output wave

### Configure the Project

You can switch the output method in menuconfig under ``Example Configuration`` → ``Select DAC continuous example mode`` (DMA or Timer).

Four waveform types are supported: sine, triangle, sawtooth, and square waves. The waveform data is generated at runtime and stored in the corresponding buffers. Each waveform buffer contains one complete period with 400 samples by default, which can be changed via `EXAMPLE_ARRAY_LEN`. The following relationship applies:

Output waveform frequency = DAC sample update rate / Number of samples

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

This example outputs sine, triangle, sawtooth and square waves on DAC channel 0, switching every 2 seconds. Connect an oscilloscope to the DAC channel 0 GPIO to observe the waveforms.

### DMA Output

You can see sine wave, triangle wave, saw tooth wave and square wave at 2 KHz on the oscilloscope.

```
I (266) signal_generator: --------------------------------------------------
I (266) signal_generator: DAC continuous output by DMA
I (266) signal_generator: DAC channel 0 io: GPIO_NUM_25
I (276) signal_generator: Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE
I (276) signal_generator: DAC sample update rate (Hz): 800000
I (286) signal_generator: Waveform frequency (Hz): 2000
I (286) signal_generator: --------------------------------------------------
I (296) signal_generator: sine wave start
I (2296) signal_generator: triangle wave start
I (4296) signal_generator: sawtooth wave start
I (6296) signal_generator: square wave start
...
```
### Timer Triggered Output

You can see sine wave, triangle wave, saw tooth wave and square wave at 50 Hz on the oscilloscope.

```
I (265) signal_generator: --------------------------------------------------
I (265) signal_generator: DAC continuous output by timer
I (265) signal_generator: DAC channel 0 io: GPIO_NUM_25
I (275) signal_generator: Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE
I (275) signal_generator: DAC sample update rate (Hz): 20000
I (285) signal_generator: Waveform frequency (Hz): 50
I (285) signal_generator: --------------------------------------------------
I (295) signal_generator: sine wave start
I (2295) signal_generator: triangle wave start
I (4295) signal_generator: sawtooth wave start
I (6295) signal_generator: square wave start
...
```
