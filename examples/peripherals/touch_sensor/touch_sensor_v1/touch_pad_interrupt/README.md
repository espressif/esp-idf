| Supported Targets | ESP32 |
| ----------------- | ----- |

# Touch Pad Interrupt Example

## ESP32 platform

Demonstrates how to set up ESP32's capacitive touch pad peripheral to trigger interrupt when a pad is touched. It also shows how to detect the touch event by the software for sensor designs when greater touch detection sensitivity is required.

ESP32 supports touch detection by configuring hardware registers. The hardware periodically detects the pulse counts. If the number of pulse counts exceeds the set threshold, a hardware interrupt will be generated to notify the application layer that a certain touch sensor channel may be triggered.

For the sensor designs when the pad is covered a glass or plastic, the difference caused by a 'touch' action could be very small. In such a case we are using software pooling and algorithms to reduce noise to still be able to detect small changes of the pulse counts. In certain cases we may need to use additional routines to adjust the threshold level dynamically as it may change depending on environment conditions.

Comparison of the two modes:

- The hardware interrupt mode occupies less CPU resources, but only a single threshold can be set and cannot support various software algorithms.
- The continuous pooling is flexible and supports various software algorithms. However, it also costs CPU overhead

The application is cycling between the interrupt mode and the pooling mode with a filter, to compare performance of the touch sensor system in both scenarios:

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

Note: Sensing threshold is set up automatically at start up by performing simple calibration. Application is reading current value for each pad and assuming two thirds of this value as the sensing threshold. Do not touch pads on application start up, otherwise sensing may not work correctly.

## Reference Information

For a simpler example how to configure and read capacitive touch pads, please refer to [touch_pad_read](../touch_pad_read).

Design and implementation of the touch sensor system is a complex process. The [Touch Sensor Application Note](https://github.com/espressif/esp-iot-solution/blob/release/v1.0/documents/touch_pad_solution/touch_sensor_design_en.md) contains several ESP32 specific notes and comments to optimize the design and get the best out of the application with sensors controlled with the ESP32.  

See the README.md file in the upper level 'examples' directory for more information about examples.