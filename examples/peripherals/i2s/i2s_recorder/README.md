| Supported Targets | ESP32 | ESP32-P4 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# I2S Digital Microphone Recording Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

In this example, we record a sample audio file captured from the digital MEMS microphone on the I2S peripheral using PDM data format.

The audio is recorded into the SDCard using WAVE file format.

| Audio Setting | Value |
|:---:|:---:|
| Sample Rate |44100 Hz|
| Bits per Sample |16 bits|

## How to Use Example

### Hardware Required

* A development board with ESP32 or ESP32S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming
* A digital microphone (SPK0838HT4H PDM output was used in this example)

The digital PDM microphone is connected on the I2S interface `I2S_NUM_0`.

The default GPIO configuration is the following:

|Mic        | GPIO   |
|:---------:|:------:|
| PDM Clock | GPIO4 |
| PDM Data  | GPIO5 |

The SDCard is connected using SPI peripheral.

| SPI  | SDCard |  GPIO  |
|:----:|:------:|:------:|
| MISO | DAT0   | GPIO17 |
| MOSI | CMD    | GPIO16 |
| SCLK | CLK    | GPIO18 |
| CS   | CD     | GPIO19 |

To change the GPIO configuration, see the `Example Configuration` from the menuconfig.

### Configure the Project

```
idf.py menuconfig
```

In the `Example Configuration` menu:

* Use `SDCard Configuration` to assign the SPI peripheral GPIOs.
* Use `I2S MEMS MIC Configuration` to assign the I2S peripheral GPIOs and audio settings.

Optional: If you need, change the other options according to your requirements.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

## Example Output

Running this example, you will see the Bits per Sample changes every 5 seconds after you have run this example. You can use `i2s_set_clk` to change the Bits per Sample and the Sample Rate. The output log can be seen below:

```
I (361) pdm_rec_example: PDM microphone recording Example start
I (371) I2S: DMA Malloc info, datalen=blocksize=2048, dma_desc_num=64
I (401) I2S: APLL: Req RATE: 44100, real rate: 88199.977, BITS: 16, CLKM: 1, BCK_M: 8, MCLK: 22579194.000, SCLK: 2822399.250000, diva: 1, divb: 0
I (431) I2S: APLL: Req RATE: 44100, real rate: 88199.977, BITS: 16, CLKM: 1, BCK_M: 8, MCLK: 22579194.000, SCLK: 2822399.250000, diva: 1, divb: 0
I (431) pdm_rec_example: Initializing SD card
I (431) pdm_rec_example: Using SDMMC peripheral
I (441) gpio: GPIO[13]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
Name: USD
Type: SDHC/SDXC
Speed: 20 MHz
Size: 3813MB
I (481) pdm_rec_example: Starting recording for 60 seconds!
I (481) pdm_rec_example: Opening file
I (60451) pdm_rec_example: Recording done!
I (60471) pdm_rec_example: File written on SDCard
I (60471) pdm_rec_example: Card unmounted
```

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
