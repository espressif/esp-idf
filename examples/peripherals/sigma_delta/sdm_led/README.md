| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Sigma Delta Modulation LED Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example uses the sigma-delta driver to generate modulated output on a GPIO. If you connect a LED or LCD backlight control to the output GPIO, you will see it slowly brightening and dimming.

## How to use example

### Hardware Required

* A development board with any supported Espressif SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* An LED and a resistor to limit the LED current. Connect them as below:

```
                                   330R            LED
EXAMPLE_SIGMA_DELTA_GPIO_NUM +----/\/\/\----+------|>|-----+ GND
```

### Configure the project

You can change the GPIO number by [EXAMPLE_SIGMA_DELTA_GPIO_NUM](main/sdm_led_example_main.c).

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Once the upload is complete and the board is reset, the program should start running. This is reported on the monitor as below:

```
...
I (245) cpu_start: Starting scheduler.
I (249) example: Install sigma delta channel
I (249) gpio: GPIO[0]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (259) example: Enable sigma delta channel
I (259) example: Change duty cycle continuously
```

Immediately after that the LED should start brightening and dimming.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
