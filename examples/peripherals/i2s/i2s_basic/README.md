| Supported Targets | ESP32 | ESP32-S2 | ESP32-C3 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# I2S Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

In this example, we generate a 100Hz triangle and sine wave and send it out from left and right channels at a sample rate of 36kHz through the I2S bus.

## How to Use Example

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

### Configure the Project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Running this example, you will see I2S start to read and write data, and only the first 4 elements in the receive buffer will be displayed. The output log can be seen below:

```
[i2s write] 1440 bytes are written successfully

[i2s read] 8192 bytes are read successfully
----------------------------------------------
[0]    0 [1]    0 [2]    0 [3]    0

[i2s write] 1440 bytes are written successfully
[i2s write] 1440 bytes are written successfully
[i2s write] 1440 bytes are written successfully

[i2s read] 8192 bytes are read successfully
----------------------------------------------
[0] a7d468d9 [1] a88a6a1d [2] a9406b58 [3] a9f66c8b

[i2s write] 1440 bytes are written successfully
[i2s write] 1440 bytes are written successfully

[i2s read] 8192 bytes are read successfully
----------------------------------------------
[0] 8b622120 [1] 8c182347 [2] 8cce256c [3] 8d84278d
```

There is a abnormal case that printing `Data dropped`, it is caused by a long polling time of `i2s_channel_read`, please refer to the `Application Notes` section in I2S API reference.

```
[i2s read] 8192 bytes are read successfully
----------------------------------------------
[0] a7d468d9 [1] a88a6a1d [2] a9406b58 [3] a9f66c8b


[i2s monitor] Data dropped


[i2s monitor] Data dropped


[i2s monitor] Data dropped

[i2s write] 1440 bytes are written successfully

[i2s monitor] Data dropped
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
