| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Touch Pad Interrupt Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to set up ESP32-S2/S3's capacitive touch pad peripheral to trigger interrupt when a pad is touched. It also shows how to detect the touch event by the software for sensor designs when greater touch detection sensitivity is required.

- The hardware interrupt mode occupies less CPU resources, but we can only apply fixed threshold and software algorithms are also impossibile.
- The polling mode is flexible and supports various software algorithms. However, it comsumes more CPU.

ESP32-S2/S3 supports touch detection by configuring hardware registers. The hardware periodically detects the pulse counts. If the number of pulse counts exceeds the set threshold, a hardware interrupt will be generated to notify the application layer that a certain touch sensor channel may be triggered.

For a simpler example how to configure and read capacitive touch pads, please refer to [touch_pad_read](../touch_pad_read).

## How to use example

### Hardware Required

* A development board with ESP32-S2 or ESP32-S3 chip
* A touch extension board like [esp32-s2-touch-devkit-1](https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/esp32s2/esp32-s2-touch-devkit-1/user_guide.html)

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

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
