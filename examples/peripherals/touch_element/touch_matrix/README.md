| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# Touch Element basic example (EVENT)

This example demonstrates how to use the Touch Element library of capacitive touch sensor and set up touch matrix.

## How to use example

### Configure the project

* Set the target of the build (where `{IDF_TARGET}` stands for the target chip such as `esp32` or `esp32s2`).
* Run `menuconfig` to select a dispatch method for the example.

### Build and Flash

Build the project and flash it to the target board, then run monitor tool to view serial output:

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

See the README.md file in the upper level 'examples' directory for more information about examples.
