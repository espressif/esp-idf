| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Touch Pad Read Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Read and display raw values from capacitive touch pad sensors.

Once configured, ESP32S2/S3 will continuously measure capacitance of touch pad sensors. Measurement is reflected as numeric value inversely related to sensor's capacitance. With a finger touched on a pad, its capacitance will get larger meanwhile the measured value gets smaller, and vice versa.

To detect if a sensor is touched or not, each particular design should be calibrated by obtaining both measurements for each individual sensor. Then a threshold between both values can be established. With specific threshold, API is then able to distinguish whether specific sensor is touched or released. For ESP32-S2/S3, the hardware integrates the edge detection algorithm, which can achieve the purpose of detecting touch actions by configuring appropriate parameters. There is another similar example that demonstrates how to perform simple calibration and trigger an interrupt when a pad is touched - see [touch_pad_interrupt](../touch_pad_interrupt).

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

ESP32-S2/S3 supports up to 14 capacitive touch pads, T1 - T14, which are connected to the specific GPIO pins. For the information of available pins please refer to ESP32-S2/S3 Technical Reference Manual. Note that T0 is an internal channel with no corresponding GPIO, it is mainly used for de-noise. This example will initialize all the 14 touch pads. The raw values of each pad can be monitored in the terminal:

```
Touch Sensor read, the output format is:
Touchpad num:[raw data]

T1: [6473] T2: [6507] T3: [6638] T4: [8917] T5: [9053] T6: [7190] T7: [7176] T8: [7416] T9: [7145] T10: [7387] T11: [7973] T12: [7776] T13: [8151] T14: [8190]
T1: [6463] T2: [6512] T3: [6643] T4: [8920] T5: [9050] T6: [7191] T7: [7176] T8: [7416] T9: [7143] T10: [7387] T11: [7974] T12: [7778] T13: [8152] T14: [8192]
T1: [6476] T2: [6508] T3: [6641] T4: [8919] T5: [9053] T6: [7190] T7: [7177] T8: [7416] T9: [7143] T10: [7386] T11: [7974] T12: [7776] T13: [8153] T14: [8193]
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
