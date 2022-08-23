| Supported Targets | ESP32 |
| ----------------- | ----- |

# Touch Pad Read Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Read and print raw values or IIR filtered values from capacitive touch sensors.

Once configured, ESP32 will continuously measure capacitance of touch pad sensors. Measurement is reflected as numeric value inversely related to sensor's capacitance. With a finger touched on a pad, its capacitance will get larger meanwhile the measured value gets smaller, and vice versa.

To detect if a sensor is touched or not, each particular design should be calibrated by obtaining both measurements for each individual sensor. Then a threshold between both values can be established. With specific threshold, API is then able to distinguish whether specific sensor is touched or released.


There is another similar example that demonstrates how to perform simple calibration and trigger an interrupt when a pad is touched - see [touch_pad_interrupt](../touch_pad_interrupt).

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

ESP32 supports up to ten capacitive touch pad sensors, T0 - T9, which are connected to specific GPIO pins. For the information about touch sensor capable pins, please refer to [Technical Reference Manual](https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf). This example will initialize all the 10 touch pads, and print the measured values to serial terminal:

```
Touch Sensor filter mode read, the output format is:
Touchpad num:[raw data, filtered data]

T0:[1072,1071] T1:[ 475, 475] T2:[1004,1003] T3:[1232,1231] T4:[1675,1676] T5:[1146,1146] T6:[1607,1607] T7:[1118,1118] T8:[1695,1695] T9:[1223,1222]
T0:[1072,1071] T1:[ 475, 475] T2:[1003,1003] T3:[1231,1231] T4:[1676,1676] T5:[1146,1146] T6:[1607,1607] T7:[1118,1118] T8:[1694,1694] T9:[1222,1221]
T0:[1071,1071] T1:[ 475, 475] T2:[1004,1004] T3:[1231,1231] T4:[1678,1677] T5:[1147,1146] T6:[1607,1607] T7:[1118,1118] T8:[1694,1694] T9:[1222,1221]
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
