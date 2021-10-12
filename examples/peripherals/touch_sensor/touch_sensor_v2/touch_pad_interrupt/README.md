| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Touch Pad Interrupt Example

## ESP32-S2, ESP32-S3 platform

Demonstrates how to set up ESP32-S2's capacitive touch pad peripheral to trigger interrupt when a pad is touched. It also shows how to detect the touch event by the software for sensor designs when greater touch detection sensitivity is required.

ESP32-S2 supports touch detection by configuring hardware registers. The hardware periodically detects the pulse counts. If the number of pulse counts exceeds the set threshold, a hardware interrupt will be generated to notify the application layer that a certain touch sensor channel may be triggered.

The application is cycling between the interrupt mode and the pooling mode with a filter, to compare performance of the touch sensor system in both scenarios:

```
I (304) Touch pad: Initializing touch pad
I (304) Touch pad: Denoise function init
I (304) Touch pad: touch pad waterproof init
I (304) Touch pad: touch pad filter init 2
I (414) Touch pad: test init: touch pad [7] base 7382, thresh 1476
I (414) Touch pad: test init: touch pad [9] base 7349, thresh 1469
I (414) Touch pad: test init: touch pad [11] base 8047, thresh 1609
I (414) Touch pad: test init: touch pad [13] base 8104, thresh 810
I (5954) Touch pad: TouchSensor [9] be actived, status mask 0x200
W (6034) Touch pad: TouchSensor [13] be actived, enter guard mode
W (6034) Touch pad: In guard mode. No response
W (6174) Touch pad: TouchSensor [13] be actived, exit guard mode
I (6194) Touch pad: TouchSensor [9] be inactived, status mask 0x0
```

## Reference Information

For a simpler example how to configure and read capacitive touch pads, please refer to [touch_pad_read](../touch_pad_read).

Design and implementation of the touch sensor system is a complex process. The [Touch Sensor Application Note](https://github.com/espressif/esp-iot-solution/blob/release/v1.0/documents/touch_pad_solution/touch_sensor_design_en.md) contains several ESP32 specific notes and comments to optimize the design and get the best out of the application with sensors controlled with the ESP32.  

See the README.md file in the upper level 'examples' directory for more information about examples.