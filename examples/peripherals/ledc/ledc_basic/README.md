| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# _LEDC Basic Example_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the LEDC to generate a PWM signal using the `LOW SPEED` mode.
To use `HIGH SPEED` mode check if the selected SoC supports this mode.

## How to use example

### Hardware Required

* A development board with any Espressif SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

Connect the GPIO to an oscilloscope to see the generated signal:

|ledc channel| GPIO  |
|:----------:|:-----:|
| Channel 0  | GPIO5 |

### Configure the project

The example uses fixed PWM frequency of 4 kHz, duty cycle in 50%, and output GPIO pin. To change them, adjust `LEDC_FREQUENCY`, `LEDC_DUTY`, `LEDC_OUTPUT_IO` macros at the top of ledc_basic_example_main.c.

Depending on the selected `LEDC_FREQUENCY`, you will need to change the `LEDC_DUTY_RES`.

To dynamically set the duty and frequency, you can use the following functions:

To set the frequency to 2.5 kHZ i.e:

```c
ledc_set_freq(LEDC_MODE, LEDC_TIMER, 2500);
```

Now set the duty to 100% i.e:

```c
ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 8192);
ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
```

To change the duty cycle you need to calculate the duty range according to the duty resolution.

If duty resolution is 13 bits:

Duty range: `0 to (2 ** 13) = 8191` where 0 is 0% and 8192 is 100%.

### Build and Flash

* [ESP-IDF Getting Started Guide](https://idf.espressif.com/)

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Running this example, you will see the PWM signal with a duty cycle of 50%.

![PWM](image/ledc_pwm_signal.png)

## Troubleshooting

* Duty Resolution

    * If you get the following error log `ledc: requested frequency and duty resolution can not be achieved, try reducing freq_hz or duty_resolution.` you need to change the `LEDC_DUTY_RES` to a lower resolution and change the range of the duty.

* Programming fail

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
