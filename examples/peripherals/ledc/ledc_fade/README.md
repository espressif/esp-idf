| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# _LEDC Fade Example_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to control intensity of LEDs using selected SoC's on-board hardware LED PWM Controller module.

## How to use example

### Hardware Required

* A development board with any Espressif SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

Connect four LEDs to the following LEDC channels / individual GPIOs:

|                 | Channel 0 | Channel 1 | Channel 2 | Channel 3 |
| --------------- | --------- | --------- | --------- | --------- |
|     ESP32       | GPIO18    | GPIO19    | GPIO4     | GPIO5     |
| All other chips | GPIO8     | GPIO9     | GPIO4     | GPIO5     |

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Running this example, you will see the four LEDs' brightness change repeatedly in the following orders

* 1: Fade up / increase intensity on the LEDs connecting to Channel 0/1; Fade down / decrease intensity on the LEDs connecting to Channel 2/3
* 2: Fade down / decrease intensity on the LEDs connecting to Channel 0/1;Fade up / increase intensity on the LEDs connecting to Channel 2/3
* 3: All LEDs keep a stable intensity
* 4: All LEDs are off

you can also see the following output log on the serial monitor:

```
1. LEDC fade up to duty = 4000
2. LEDC fade down to duty = 0
3. LEDC set duty = 4000 without fade
4. LEDC set duty = 0 without fade
...
```

## Troubleshooting

* Programming fail

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
