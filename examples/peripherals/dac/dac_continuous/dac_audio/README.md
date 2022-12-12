| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# DAC Constant Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows how to play a piece of audio by DAC driver.

## How to use the Example

### Hardware Required

* A development board with ESP32 or ESP32-S2 SoC
    - Note that some ESP32-S2 DevKits have LED on it which is connected to GPIO18 (same pin as DAC channel2), so the output voltage of DAC channel 1 can't go down due the this LED.
* An Audio Power Amplifier like `NS4150`
* A speaker or earphone to play the audio

### Configure the Project

This example uses the audio that stored in a buffer, which is put in `audio_example_file.h`. You can also create your own audio buffer by the python script `generate_audio_file.py`.

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
I (277) dac audio: DAC audio example start
I (277) dac audio: --------------------------------------
I (287) dac audio: DAC initialized success, DAC DMA is ready
I (297) dac audio: Audio size 79512 bytes, played at frequency 16000 Hz
I (5137) dac audio: Audio size 79512 bytes, played at frequency 16000 Hz
I (9967) dac audio: Audio size 79512 bytes, played at frequency 16000 Hz
...
```

And meanwhile, you can hear the audio played every 1 second from the speaker or earphone.
