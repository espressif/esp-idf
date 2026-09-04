| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# DAC Continuous Audio Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows how to play a piece of audio by DAC driver.

## How to use the Example

### Hardware Required

* A development board with ESP32 or ESP32-S2 SoC
* DAC channel to GPIO mapping: see [GPIO Summary](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html#gpio-summary) (switch the chip target on the documentation page if needed)
* Note that some ESP32-S2 DevKits have an LED on the DAC1 pin, so that channel's output may not go fully low
* An Audio Power Amplifier like `NS4150`
* A speaker or earphone to play the audio

### Configure the Project

The audio table and sample rate are extracted at build time from a WAV file under `tools/` and written into a generated header in the build directory. In menuconfig, under ``Example Configuration``:

* ``WAV file name``: file under `tools/` (default `hi_idf_audio.wav`)
* ``DAC audio table bit width``: Please select based on the DAC channel configuration

You can also run the converter by hand to inspect the table:

```
python tools/generate_audio_file.py --bitwidth 8 -o audio_example_file.h tools/hi_idf_audio.wav
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

You can see the following logs on the monitor:

```
I (277) dac_audio: DAC audio example start
I (277) dac_audio: --------------------------------------
I (287) dac_audio: DAC initialized success, DAC DMA is ready
I (297) dac_audio: Audio size 95824 bytes, played at frequency 48000 Hz synchronously
Play count: 1
Play count: 2
...
```

And meanwhile, you can hear the audio played every 1 second from the speaker or earphone.
