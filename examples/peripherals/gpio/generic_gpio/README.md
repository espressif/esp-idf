| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Example: GPIO

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This test code shows how to configure GPIO and how to use it with interruption.

## GPIO functions:

| GPIO                         | Direction | Configuration                                          |
| ---------------------------- | --------- | ------------------------------------------------------ |
| CONFIG_GPIO_OUTPUT_0         | output    |                                                        |
| CONFIG_GPIO_OUTPUT_1         | output    |                                                        |
| CONFIG_GPIO_INPUT_0          | input     | pulled up, interrupt from rising edge and falling edge |
| CONFIG_GPIO_INPUT_1          | input     | pulled up, interrupt from rising edge                  |

## Test:
 1. Connect CONFIG_GPIO_OUTPUT_0 with CONFIG_GPIO_INPUT_0
 2. Connect CONFIG_GPIO_OUTPUT_1 with CONFIG_GPIO_INPUT_1
 3. Generate pulses on CONFIG_GPIO_OUTPUT_0/1, that triggers interrupt on CONFIG_GPIO_INPUT_0/1

 **Note:** The following pin assignments are used by default, you can change them by `idf.py menuconfig` > `Example Configuration`.

|                        | CONFIG_GPIO_OUTPUT_0 | CONFIG_GPIO_OUTPUT_1 | CONFIG_GPIO_INPUT_0 | CONFIG_GPIO_INPUT_1 |
| ---------------------- | -------------------- | -------------------- | ------------------- | ------------------- |
| ESP32C2/ESP32H2        | 8                    | 9                    | 4                   | 5                   |
| All other chips        | 18                   | 19                   | 4                   | 5                   |

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with any Espressif SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* Some jumper wires to connect GPIOs.

### Configure the project

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

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
GPIO[4] intr, val: 1
GPIO[5] intr, val: 1
cnt: 2
GPIO[4] intr, val: 0
cnt: 3
GPIO[4] intr, val: 1
GPIO[5] intr, val: 1
cnt: 4
GPIO[4] intr, val: 0
cnt: 5
GPIO[4] intr, val: 1
GPIO[5] intr, val: 1
cnt: 6
GPIO[4] intr, val: 0
cnt: 7
GPIO[4] intr, val: 1
GPIO[5] intr, val: 1
cnt: 8
GPIO[4] intr, val: 0
cnt: 9
GPIO[4] intr, val: 1
GPIO[5] intr, val: 1
cnt: 10
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
