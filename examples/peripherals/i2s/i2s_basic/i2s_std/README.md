| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# I2S Basic Standard Mode Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is going to show how to use the standard mode in simplex mode or full-duplex mode.

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* A USB cable for power supply and programming

### Configure the Project

There are simplex mode and duplex mode can be chosen in this example, you can choose the mode in the menuconfig or just setting `EXAMPLE_I2S_DUPLEX_MODE` directly, when `EXAMPLE_I2S_DUPLEX_MODE` is `0` the example will adopt the simplex mode, otherwise it will adopt the full-duplex mode.

Note that ESP32-S2 simplex mode is not available because this example requires both TX & RX channels, however, ESP32-S2 has only one I2S controller and the simplex TX & RX channels can't coexist in a same controller. By the way, the simplex TX & RX channels can't coexist on ESP32 as well, but ESP32 has two I2S controllers so the simplex TX & RX channels can be registered on the different I2S controllers.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

While `EXAMPLE_I2S_DUPLEX_MODE` is set to `1`, then you can see the following log:

```
Write Task: i2s write 2048 bytes
Read Task: i2s read 2048 bytes
-----------------------------------
[0] 12 [1] 34 [2] 56 [3] 78
[4] 9a [5] bc [6] de [7] f0

Write Task: i2s write 2048 bytes
Read Task: i2s read 2048 bytes
-----------------------------------
[0] 12 [1] 34 [2] 56 [3] 78
[4] 9a [5] bc [6] de [7] f0
...
```

While `EXAMPLE_I2S_DUPLEX_MODE` is set to `0`, you can see the receiving data is always `0` because no signal inputted:

```
Write Task: i2s write 2048 bytes
Read Task: i2s read 2048 bytes
-----------------------------------
[0] 0 [1] 0 [2] 0 [3] 0
[4] 0 [5] 0 [6] 0 [7] 0

Write Task: i2s write 2048 bytes
Read Task: i2s read 2048 bytes
-----------------------------------
[0] 0 [1] 0 [2] 0 [3] 0
[4] 0 [5] 0 [6] 0 [7] 0
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
