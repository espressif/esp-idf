# I2S Built-in ADC/DAC Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

In this example, we configure I2S to work in I2S_ADC and I2S_DAC modes and then:
* recording sound from ADC,
* playing the recorded sound,
* playing an audio file in flash via DAC.

#### Note:
The `tools` directory contains `generate_audio_file.py` script for generating audio files:

    * The script provides an example of generating audio tables from `.wav` files.
    * In this example, the `wav` file must be in 16k/16bit mono format.
    * The script will bundle the `wav` files into a single table named `audio_example_file.h`.
    * Since the ADC can only play 8-bit data, the script will scale each 16-bit value to a 8-bit value.
    * The script will covert all signed values into unsigned values because only positive values will be output by the ADC.

## How to Use Example

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming
* A microphone (with amplifier) and one or two speaker(s) for testing.

The following is the hardware connection:

|hardware|module|GPIO|
|:---:|:---:|:---:|
|Microphone|ADC1_CH0|GPIO36|
|speaker(R)|DAC1|GPIO25|
|speaker(L)|DAC2|GPIO26|

### Configure the Project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options, the flash size should be set to 4 MB.
* Select "Custom partition table CSV" and rename "Custom partition CSV file" to "partitions_adc_dac_example.csv".

(Note that you can use `sdkconfig.defaults`)

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Reset your development board. The application it will first record the sound through the microphone. Then it will play the recorded sound, and finally a piece of audio stored in the flash. The following is the output log:

```
partiton addr: 0x00110000; size: 2097152; label: storage
Erasing flash
partiton addr: 0x00110000; size: 2097152; label: storage
Erase size: 323584 Bytes
I2S: PLL_D2: Req RATE: 16000, real rate: 1004.000, BITS: 16, CLKM: 83, BCK: 60, MCLK: 83.333, SCLK: 32128.000000, diva: 64, divb: 21
Sound recording 5%
Sound recording 10%
...
Sound recording 97%
Sound recording 102% 
playing: 0 %
playing: 1 %
playing: 2 %
...
playing: 96 %
playing: 97 %
playing: 98 %
Playing file example:
I2S: PLL_D2: Req RATE: 16000, real rate: 1004.000, BITS: 16, CLKM: 83, BCK: 60, MCLK: 83.333, SCLK: 32128.000000, diva: 64, divb: 21
```

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
