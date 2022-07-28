| Supported Targets | ESP32 |
| ----------------- | ----- |

# Touch Pad Interrupt Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to set up ESP32's capacitive touch pad peripheral to trigger interrupt when a pad is touched. It also shows how to detect the touch event by software for sensor designs where greater touch detection sensitivity is required.

- The hardware interrupt mode occupies less CPU resources, but we can only apply fixed threshold and software algorithms are also impossibile.
- The polling mode is flexible and supports various software algorithms. However, it comsumes more CPU.

ESP32 supports touch detection by configuring hardware registers. The hardware periodically detects the pulse counts. If the number of pulse counts exceeds the set threshold, a hardware interrupt will be generated to notify the application layer that a certain touch sensor channel may be triggered.

For the sensors covered with glass or plastic, the capacitance difference caused by a touch action could be very small. In such cases, software polling is used so software algorithms can be applied to reduce noise and catch small changes of the pulse counts. In certain cases, we may need to add additional routines to adjust the threshold level dynamically as it may change depending on environment conditions.

For a simpler example how to configure and read capacitive touch pads, please refer to [touch_pad_read](../touch_pad_read).

## How to use example

### Hardware Required

* A development board with ESP32 chip
* Proper touch sensor system

For hardware and firmware design guidelines on ESP32 touch sensor system, please refer to [Touch Sensor Application Note](https://github.com/espressif/esp-iot-solution/blob/release/v1.1/documents/touch_pad_solution/touch_sensor_design_en.md), where you may find comprehensive information on how to design and implement touch sensing applications, such as linear slider, wheel slider, matrix buttons and spring buttons.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The application cycles between the interrupt mode and the pooling mode with a filter, to compare performance of the touch sensor system between both scenarios:

```
I (6303) Touch pad: Waiting for any pad being touched...
I (6733) Touch pad: T6 activated!
I (7333) Touch pad: T5 activated!
I (7723) Touch pad: T3 activated!
I (8043) Touch pad: T2 activated!
I (8883) Touch pad: T4 activated!
I (9523) Touch pad: T7 activated!
I (12503) Touch pad: Waiting for any pad being touched...
I (15483) Touch pad: T6 activated!
I (16253) Touch pad: T5 activated!
I (17903) Touch pad: Waiting for any pad being touched...
I (22903) Touch pad: Waiting for any pad being touched...
```

## Troubleshooting

Sensing threshold is set up automatically at start up by performing simple calibration. Application is reading current value for each pad and assuming two thirds of this value as the sensing threshold. Do not touch pads on application start up, otherwise sensing may not work correctly.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
