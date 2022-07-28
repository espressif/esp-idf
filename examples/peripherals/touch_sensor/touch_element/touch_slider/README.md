| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Touch Element slider example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the Touch Element library of capacitive touch sensor and set up touch slider.

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
I (331) Touch Slider Example: Touch element library installed
I (331) Touch Slider Example: Touch slider installed
I (341) Touch Slider Example: Touch slider created
I (341) Touch Slider Example: Touch element library start
I (1911) Touch Slider Example: Slider Press, position: 0
I (1921) Touch Slider Example: Slider Calculate, position: 0
I (1931) Touch Slider Example: Slider Calculate, position: 0
I (1941) Touch Slider Example: Slider Calculate, position: 0
I (1951) Touch Slider Example: Slider Calculate, position: 0
I (1961) Touch Slider Example: Slider Calculate, position: 0
I (1971) Touch Slider Example: Slider Calculate, position: 0
I (1981) Touch Slider Example: Slider Calculate, position: 0
I (1991) Touch Slider Example: Slider Calculate, position: 0
I (2001) Touch Slider Example: Slider Calculate, position: 0
I (2011) Touch Slider Example: Slider Calculate, position: 0
I (2021) Touch Slider Example: Slider Calculate, position: 1
I (2031) Touch Slider Example: Slider Calculate, position: 1
I (2041) Touch Slider Example: Slider Calculate, position: 2
I (2051) Touch Slider Example: Slider Calculate, position: 2
I (2061) Touch Slider Example: Slider Calculate, position: 4
I (2071) Touch Slider Example: Slider Calculate, position: 5
I (2081) Touch Slider Example: Slider Calculate, position: 6
I (2091) Touch Slider Example: Slider Calculate, position: 8
I (2101) Touch Slider Example: Slider Calculate, position: 10
I (2111) Touch Slider Example: Slider Calculate, position: 12
I (2121) Touch Slider Example: Slider Calculate, position: 15
I (2131) Touch Slider Example: Slider Calculate, position: 17
I (2141) Touch Slider Example: Slider Calculate, position: 19
I (2151) Touch Slider Example: Slider Calculate, position: 22
I (2161) Touch Slider Example: Slider Calculate, position: 24
I (2171) Touch Slider Example: Slider Calculate, position: 26
I (2181) Touch Slider Example: Slider Calculate, position: 29
I (2191) Touch Slider Example: Slider Calculate, position: 31
I (2201) Touch Slider Example: Slider Calculate, position: 33
I (2211) Touch Slider Example: Slider Calculate, position: 35
I (2221) Touch Slider Example: Slider Calculate, position: 37
I (2231) Touch Slider Example: Slider Calculate, position: 40
I (2241) Touch Slider Example: Slider Calculate, position: 42
I (2251) Touch Slider Example: Slider Calculate, position: 44
I (2261) Touch Slider Example: Slider Calculate, position: 46
I (2271) Touch Slider Example: Slider Calculate, position: 48
I (2281) Touch Slider Example: Slider Calculate, position: 50
I (2291) Touch Slider Example: Slider Calculate, position: 52
I (2301) Touch Slider Example: Slider Calculate, position: 54
I (2311) Touch Slider Example: Slider Calculate, position: 56
I (2321) Touch Slider Example: Slider Calculate, position: 57
I (2331) Touch Slider Example: Slider Calculate, position: 59
I (2341) Touch Slider Example: Slider Calculate, position: 60
I (2351) Touch Slider Example: Slider Calculate, position: 61
I (2361) Touch Slider Example: Slider Release, position: 61
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
