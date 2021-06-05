# Example: GPIO

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This test code shows how to configure GPIO and how to use it with interruption.

## GPIO functions:

| GPIO     | Direction | Configuration                                          |
| -------- | --------- | ------------------------------------------------------ |
| GPIO18   | output    |                                                        |
| GPIO19   | output    |                                                        |
| GPIO4    | input     | pulled up, interrupt from rising edge and falling edge |
| GPIO5    | input     | pulled up, interrupt from rising edge                  |

## Test:
 1. Connect GPIO18 with GPIO4
 2. Connect GPIO19 with GPIO5
 3. Generate pulses on GPIO18/19, that triggers interrupt on GPIO4/5

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* Some jumper wires to connect GPIOs.

### Configure the project

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)

## Example Output

As you run the example, you will see the following log:

```
I (317) gpio: GPIO[18]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (327) gpio: GPIO[19]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (337) gpio: GPIO[4]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:1 
I (347) gpio: GPIO[5]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:1 
Minimum free heap size: 289892 bytes
cnt: 0
cnt: 1
cnt: 2
cnt: 3
cnt: 4
cnt: 5
cnt: 6
cnt: 7
cnt: 8
cnt: 9
cnt: 10
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
