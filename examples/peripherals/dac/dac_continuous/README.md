| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# DAC Constant Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows the basic usage of outputting continuous voltage by the DAC driver. There are two ways to realize continuous output, one is outputting by DMA transmission and another is by timer interrupt.

### Timer Interrupt

While using timer interrupt to output the waves, the digital value will be set in every timer interrupt callback, it means the conversion frequency is equal to the timer interrupt frequency. Obviously, the conversion frequency is limited by the interrupt, which relies on the CPU scheduling, thus it can't reach a high frequency in this mode. But it can be used as a supplementary way while the conversion frequency is too low to use DMA mode.

### DMA transmission

While using DMA to transmit the wave buffers, the digital values are put into a DMA buffer wait for transmitting and converting, it means the conversion frequency is equal to the frequency that DMA transmitting the data. We can set the DMA frequency directly, and the digital data int the buffer will be sent automatically when the buffer has been loaded onto the DMA. So the conversion frequency can reach even several MHz while using DMA mode. But the wave can be distorted if the frequency is too high.

## How to use the Example

### Hardware Required

* A development board with ESP32 or ESP32-S2 SoC
    - Note that some ESP32-S2 DevKits have LED on it which is connected to GPIO18 (same pin as DAC channel2), so the output voltage of DAC channel 1 can't go down due the this LED.
* (Optional) An oscilloscope to monitor the output wave

### Configure the Project

You can switch the output method by setting the macro `EXAMPLE_DAC_CONTINUOUS_MODE` to `EXAMPLE_DAC_CONTINUOUS_BY_TIMER` or `EXAMPLE_DAC_CONTINUOUS_BY_DMA`.

There are four waves: sine, triangle, saw tooth and square. These waves are stored in corresponding buffers, and each wave has 400 points as default, which can be modified by `EXAMPLE_ARRAY_LEN`, reduce the point number can increase the wave frequency.

### Build and Flash

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
DAC channel 0 vaule: 2291       DAC channel 1 vaule: 2331
DAC channel 0 vaule:   43       DAC channel 1 vaule:    3
DAC channel 0 vaule:   55       DAC channel 1 vaule:   32
DAC channel 0 vaule:   57       DAC channel 1 vaule:   33
DAC channel 0 vaule:   56       DAC channel 1 vaule:   34
DAC channel 0 vaule:   59       DAC channel 1 vaule:   34
DAC channel 0 vaule:   56       DAC channel 1 vaule:   33
I (3393) dac continuous: triangle wave start
DAC channel 0 vaule: 2258       DAC channel 1 vaule: 2243
DAC channel 0 vaule: 2257       DAC channel 1 vaule: 2242
DAC channel 0 vaule: 2259       DAC channel 1 vaule: 2242
DAC channel 0 vaule: 2257       DAC channel 1 vaule: 2245
DAC channel 0 vaule: 2257       DAC channel 1 vaule: 2243
DAC channel 0 vaule: 2258       DAC channel 1 vaule: 2240
I (6393) dac continuous: sawtooth wave start
DAC channel 0 vaule: 2704       DAC channel 1 vaule: 2735
DAC channel 0 vaule: 2704       DAC channel 1 vaule: 2735
DAC channel 0 vaule: 2704       DAC channel 1 vaule: 2736
DAC channel 0 vaule: 2704       DAC channel 1 vaule: 2717
DAC channel 0 vaule: 2704       DAC channel 1 vaule: 2734
DAC channel 0 vaule: 2704       DAC channel 1 vaule: 2736
I (9393) dac continuous: square wave start
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
I (12393) dac continuous: sine wave start
DAC channel 0 vaule:   82       DAC channel 1 vaule:   62
DAC channel 0 vaule:   83       DAC channel 1 vaule:   62
DAC channel 0 vaule:   82       DAC channel 1 vaule:   62
DAC channel 0 vaule:   87       DAC channel 1 vaule:   62
DAC channel 0 vaule:   84       DAC channel 1 vaule:   63
DAC channel 0 vaule:   83       DAC channel 1 vaule:   64
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
DAC channel 0 vaule: 3131       DAC channel 1 vaule: 1634
DAC channel 0 vaule: 1712       DAC channel 1 vaule: 2531
DAC channel 0 vaule: 1716       DAC channel 1 vaule: 2535
DAC channel 0 vaule: 1715       DAC channel 1 vaule: 2544
DAC channel 0 vaule: 1715       DAC channel 1 vaule: 2533
DAC channel 0 vaule: 1712       DAC channel 1 vaule: 2539
I (3395) dac continuous(DMA): triangle wave start
DAC channel 0 vaule:  592       DAC channel 1 vaule: 1190
DAC channel 0 vaule: 4095       DAC channel 1 vaule: 3518
DAC channel 0 vaule: 4095       DAC channel 1 vaule: 3515
DAC channel 0 vaule: 4095       DAC channel 1 vaule: 3516
DAC channel 0 vaule: 4095       DAC channel 1 vaule: 3514
DAC channel 0 vaule: 4095       DAC channel 1 vaule: 3515
I (6395) dac continuous(DMA): sawtooth wave start
DAC channel 0 vaule:  294       DAC channel 1 vaule:  560
DAC channel 0 vaule: 2861       DAC channel 1 vaule: 3227
DAC channel 0 vaule: 2860       DAC channel 1 vaule: 3216
DAC channel 0 vaule: 2861       DAC channel 1 vaule: 3227
DAC channel 0 vaule: 2861       DAC channel 1 vaule: 3216
DAC channel 0 vaule: 2859       DAC channel 1 vaule: 3183
I (9395) dac continuous(DMA): square wave start
DAC channel 0 vaule: 4095       DAC channel 1 vaule: 4095
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
DAC channel 0 vaule:    0       DAC channel 1 vaule:    0
I (12395) dac continuous(DMA): sine wave start
DAC channel 0 vaule: 2864       DAC channel 1 vaule: 3691
DAC channel 0 vaule:    0       DAC channel 1 vaule:  204
DAC channel 0 vaule:    0       DAC channel 1 vaule:  202
DAC channel 0 vaule:    0       DAC channel 1 vaule:  193
DAC channel 0 vaule:    0       DAC channel 1 vaule:  181
DAC channel 0 vaule:    0       DAC channel 1 vaule:  194
...
```
