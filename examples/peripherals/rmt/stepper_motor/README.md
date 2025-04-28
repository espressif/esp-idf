| Supported Targets | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- |

# RMT Based Stepper Motor Smooth Controller

(See the README.md file in the upper level 'examples' directory for more information about examples.)

One RMT TX channel can use different encoders in sequence, which is useful to generate waveforms that have obvious multiple stages.

This example shows how to drive a stepper motor with a **STEP/DIR** interfaced controller (e.g. [DRV8825](https://www.ti.com/lit/ds/symlink/drv8825.pdf)) in a [smooth](https://en.wikipedia.org/wiki/Smoothstep) way. To smoothly drive a stepper motor, there're three phases: **Acceleration**, **Uniform** and **Deceleration**. Accordingly, this example implements two encoders so that RMT channel can generate the waveforms with different characteristics:

* `curve_encoder` is to encode the **Acceleration** and **Deceleration** phase
* `uniform_encoder` is to encode the ***Uniform** phase

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* A USB cable for Power supply and programming
* A two-phase four-wire stepper motor
* A DRV8825 stepper motor controller

Connection :

```
+---------------------------+             +--------------------+      +--------------+
|          ESP Board        |             |       DRV8825      |      |    4-wire    |
|                       GND +-------------+ GND                |      |     Step     |
|                           |             |                    |      |     Motor    |
|                       3V3 +-------------+ VDD             A+ +------+ A+           |
|                           |             |                    |      |              |
|       STEP_MOTOR_GPIO_DIR +------------>+ DIR             A- +------+ A-           |
|                           |             |                    |      |              |
|      STEP_MOTOR_GPIO_STEP +------------>+ STEP            B- +------+ B-           |
|                           |             |                    |      |              |
|                           |      3V3----+ nSLEEP          B+ +------+ B+           |
|                           |             |                    |      +--------------+
|                           |      3V3----+ nRST            VM +-------------------+
|                           |             |                    |                   |
|                           |  3V3|GND----+ M2             GND +----------+        |
|                           |             |                    |          |        |
|                           |  3V3|GND----+ M1                 |          |        |
|                           |             |                    |          |        |
|                           |  3V3|GND----+ M0                 |      +---+--------+-----+
|                           |             |                    |      |  GND     +12V    |
|        STEP_MOTOR_GPIO_EN +------------>+ nEN                |      |   POWER SUPPLY   |
+---------------------------+             +--------------------+      +------------------+
```

The GPIO number used in this example can be changed according to your board, by the macro `STEP_MOTOR_GPIO_EN`, `STEP_MOTOR_GPIO_DIR` and `STEP_MOTOR_GPIO_STEP` defined in the [source file](main/stepper_motor_example_main.c).

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (325) example: Initialize EN + DIR GPIO
I (325) gpio: GPIO[16]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (335) gpio: GPIO[17]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (345) example: Create RMT TX channel
I (365) example: Set spin direction
I (365) example: Enable step motor
I (375) example: Create motor encoders
I (405) example: Start RMT channel
I (405) example: Spin motor for 6000 steps: 500 accel + 5000 uniform + 500 decel
```

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
