| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# DAC Constant Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows the basic usage of outputting continuous voltage by the DAC driver. There are two ways to realize continuous output, one is outputting by DMA transmission and another is by timer interrupt.

### Timer Interrupt

While using timer interrupt to output the waves, it actually sets the voltage by `oneshot` API in every timer interrupt callback. Which means the conversion frequency is equal to the timer interrupt frequency. Obviously, the conversion frequency is limited by the interrupt, which relies on the CPU scheduling, thus it can't reach a high frequency in this mode. But it can be used as a supplementary way while the conversion frequency is too low to use DMA mode.

### DMA transmission

While using DMA to transmit the wave buffers, the digital values are put into a DMA buffer waiting for transmission and conversion, that means the conversion frequency is equal to the frequency that DMA transmitting the data. We can set the DMA frequency directly, and the digital data in the buffer will be sent automatically when the buffer has been loaded into the DMA. So the conversion frequency can reach even several MHz while using DMA mode. But the wave can be distorted if the frequency is too high.

## How to use the Example

### Hardware Required

* A development board with ESP32 or ESP32-S2 SoC
    - Note that some ESP32-S2 DevKits have LED on it which is connected to GPIO18 (same pin as DAC channel1), so the output voltage of DAC channel 1 can't go down due the this LED.
* (Optional) An oscilloscope to monitor the output wave

### Configure the Project

You can switch the output method by setting the macro `EXAMPLE_DAC_CONTINUOUS_MODE` to `EXAMPLE_DAC_CONTINUOUS_BY_TIMER` or `EXAMPLE_DAC_CONTINUOUS_BY_DMA`.

There are four waves: sine, triangle, saw tooth and square. These waves are stored in corresponding buffers, and each wave has 400 points as default, which can be modified by `EXAMPLE_ARRAY_LEN`, reduce the point number can increase the wave frequency.

### Build and Flash

Note that as we use the ADC to monitor the output data, we need to set false to `CONFIG_ADC_DISABLE_DAC_OUTPUT` in the menuconfig, otherwise the ADC will shutdown the DAC power to guarantee it won't be affect by DAC.

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

This example can output sine wave, triangle wave, saw tooth wave and square wave periodically, each wave will last for 3 seconds.

The DAC channels can be read by ADC channels internally. The ADC read period is 500 ms, the following log is the raw ADC value read from the DAC channels. But since the ADC sample-rate is lower than the DAC output-rate, the sampling value can only indicate that the voltage is changing.

### Timer Triggered Output

You can see sine wave, triangle wave, saw tooth wave and square wave at 50 Hz on the oscilloscope.

```
I (333) dac continuous: --------------------------------------------------
I (343) dac continuous: DAC continuous output by Timer
I (343) dac continuous: DAC channel 0 io: GPIO_NUM_25
I (353) dac continuous: DAC channel 1 io: GPIO_NUM_26
I (353) dac continuous: Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE
I (363) dac continuous: DAC conversion frequency (Hz): 20000
I (373) dac continuous: DAC wave frequency (Hz): 50
I (373) dac continuous: --------------------------------------------------
DAC channel 0 value: 2291       DAC channel 1 value: 2331
DAC channel 0 value:   43       DAC channel 1 value:    3
DAC channel 0 value:   55       DAC channel 1 value:   32
DAC channel 0 value:   57       DAC channel 1 value:   33
DAC channel 0 value:   56       DAC channel 1 value:   34
DAC channel 0 value:   59       DAC channel 1 value:   34
DAC channel 0 value:   56       DAC channel 1 value:   33
I (3393) dac continuous(timer): triangle wave start
DAC channel 0 value: 2258       DAC channel 1 value: 2243
DAC channel 0 value: 2257       DAC channel 1 value: 2242
DAC channel 0 value: 2259       DAC channel 1 value: 2242
DAC channel 0 value: 2257       DAC channel 1 value: 2245
DAC channel 0 value: 2257       DAC channel 1 value: 2243
DAC channel 0 value: 2258       DAC channel 1 value: 2240
I (6393) dac continuous(timer): sawtooth wave start
DAC channel 0 value: 2704       DAC channel 1 value: 2735
DAC channel 0 value: 2704       DAC channel 1 value: 2735
DAC channel 0 value: 2704       DAC channel 1 value: 2736
DAC channel 0 value: 2704       DAC channel 1 value: 2717
DAC channel 0 value: 2704       DAC channel 1 value: 2734
DAC channel 0 value: 2704       DAC channel 1 value: 2736
I (9393) dac continuous(timer): square wave start
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
I (12393) dac continuous(timer): sine wave start
DAC channel 0 value:   82       DAC channel 1 value:   62
DAC channel 0 value:   83       DAC channel 1 value:   62
DAC channel 0 value:   82       DAC channel 1 value:   62
DAC channel 0 value:   87       DAC channel 1 value:   62
DAC channel 0 value:   84       DAC channel 1 value:   63
DAC channel 0 value:   83       DAC channel 1 value:   64
...
```

### DMA Output

You can see sine wave, triangle wave, saw tooth wave and square wave at 2 KHz on the oscilloscope.

```
I (335) dac continuous: --------------------------------------------------
I (345) dac continuous: DAC continuous output by DMA
I (345) dac continuous: DAC channel 0 io: GPIO_NUM_25
I (355) dac continuous: DAC channel 1 io: GPIO_NUM_26
I (355) dac continuous: Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE
I (365) dac continuous: DAC conversion frequency (Hz): 800000
I (375) dac continuous: DAC wave frequency (Hz): 2000
I (375) dac continuous: --------------------------------------------------
DAC channel 0 value: 3131       DAC channel 1 value: 1634
DAC channel 0 value: 1712       DAC channel 1 value: 2531
DAC channel 0 value: 1716       DAC channel 1 value: 2535
DAC channel 0 value: 1715       DAC channel 1 value: 2544
DAC channel 0 value: 1715       DAC channel 1 value: 2533
DAC channel 0 value: 1712       DAC channel 1 value: 2539
I (3395) dac continuous(DMA): triangle wave start
DAC channel 0 value:  592       DAC channel 1 value: 1190
DAC channel 0 value: 4095       DAC channel 1 value: 3518
DAC channel 0 value: 4095       DAC channel 1 value: 3515
DAC channel 0 value: 4095       DAC channel 1 value: 3516
DAC channel 0 value: 4095       DAC channel 1 value: 3514
DAC channel 0 value: 4095       DAC channel 1 value: 3515
I (6395) dac continuous(DMA): sawtooth wave start
DAC channel 0 value:  294       DAC channel 1 value:  560
DAC channel 0 value: 2861       DAC channel 1 value: 3227
DAC channel 0 value: 2860       DAC channel 1 value: 3216
DAC channel 0 value: 2861       DAC channel 1 value: 3227
DAC channel 0 value: 2861       DAC channel 1 value: 3216
DAC channel 0 value: 2859       DAC channel 1 value: 3183
I (9395) dac continuous(DMA): square wave start
DAC channel 0 value: 4095       DAC channel 1 value: 4095
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
DAC channel 0 value:    0       DAC channel 1 value:    0
I (12395) dac continuous(DMA): sine wave start
DAC channel 0 value: 2864       DAC channel 1 value: 3691
DAC channel 0 value:    0       DAC channel 1 value:  204
DAC channel 0 value:    0       DAC channel 1 value:  202
DAC channel 0 value:    0       DAC channel 1 value:  193
DAC channel 0 value:    0       DAC channel 1 value:  181
DAC channel 0 value:    0       DAC channel 1 value:  194
...
```
