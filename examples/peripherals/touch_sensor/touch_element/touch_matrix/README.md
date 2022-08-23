| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Touch Element matrix example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the Touch Element library of capacitive touch sensor and set up touch matrix.

## How to use example

### Hardware Required

* A development board with ESP32-S2 or ESP32-S3 chip
* A touch extension board like [esp32-s2-touch-devkit-1](https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32s2/esp32-s2-touch-devkit-1/user_guide.html)

### Configure the project

* Set the target of the build by following command, where TARGET can be `esp32s2` or `esp32s3`.
```
idf.py set-target TARGET
```
* Run `idf.py menuconfig` to select a dispatch method for the example.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (331) Touch Matrix Example: Touch element library installed
I (331) Touch Matrix Example: Touch matrix installed
I (341) Touch Matrix Example: Touch matrix created
I (341) Touch Matrix Example: Touch element library start
I (1951) Touch Matrix Example: Matrix Press, axis: (0, 0) index: 0
I (2131) Touch Matrix Example: Matrix Release, axis: (0, 0) index: 0
I (3121) Touch Matrix Example: Matrix Press, axis: (1, 1) index: 4
I (3281) Touch Matrix Example: Matrix Release, axis: (1, 1) index: 4
I (4621) Touch Matrix Example: Matrix Press, axis: (2, 0) index: 6
I (4801) Touch Matrix Example: Matrix Release, axis: (2, 0) index: 6
I (5381) Touch Matrix Example: Matrix Press, axis: (2, 2) index: 8
I (5571) Touch Matrix Example: Matrix Release, axis: (2, 2) index: 8
I (6221) Touch Matrix Example: Matrix Press, axis: (0, 2) index: 2
I (6441) Touch Matrix Example: Matrix Release, axis: (0, 2) index: 2
I (7551) Touch Matrix Example: Matrix Press, axis: (1, 1) index: 4
I (8551) Touch Matrix Example: Matrix LongPress, axis: (1, 1) index: 4
I (9551) Touch Matrix Example: Matrix LongPress, axis: (1, 1) index: 4
I (10551) Touch Matrix Example: Matrix LongPress, axis: (1, 1) index: 4
I (11031) Touch Matrix Example: Matrix Release, axis: (1, 1) index: 4
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
