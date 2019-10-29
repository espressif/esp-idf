# I2S Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

In this example, we generate a 100Hz triangle and sine wave and send it out from left and right channels at a sample rate of 36kHz through the I2S bus.

## How to Use Example

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

### Configure the Project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Running this example, you will see the Bits per Sample changes every 5 seconds after you have run this example. You can use `i2s_set_clk` to change the Bits per Sample and the Sample Rate. The output log can be seen below:

```
Test bits=24 free mem=293780, written data=2880
I2S: DMA Malloc info, datalen=blocksize=480, dma_buf_count=6
I2S: PLL_D2: Req RATE: 36000, real rate: 37878.000, BITS: 24, CLKM: 11, BCK: 8, MCLK: 13837837.838, SCLK: 1818144.000000, diva: 64, divb: 36

Test bits=32 free mem=292336, written data=2880
I2S: DMA Malloc info, datalen=blocksize=480, dma_buf_count=6
I2S: PLL_D2: Req RATE: 36000, real rate: 36764.000, BITS: 32, CLKM: 17, BCK: 4, MCLK: 9216921.692, SCLK: 2352896.000000, diva: 64, divb: 23

Test bits=16 free mem=293772, written data=1440
I2S: DMA Malloc info, datalen=blocksize=240, dma_buf_count=6
I2S: PLL_D2: Req RATE: 36000, real rate: 36764.000, BITS: 16, CLKM: 17, BCK: 8, MCLK: 9216921.692, SCLK: 1176448.000000, diva: 64, divb: 23
```

If you have a logic analyzer, you can use a logic analyzer to grab online data. The following table describes the pins we use by default (Note that you can also use other pins for the same purpose).

| pin name| function | gpio_num |
|:---:|:---:|:---:|
| WS  |word select| GPIO_NUM_15 |
| SCK |continuous serial clock| GPIO_NUM_13 |
| SD  |serial data| GPIO_NUM_21 |

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
