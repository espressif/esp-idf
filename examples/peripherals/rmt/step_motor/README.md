| Supported Targets | ESP32-S2 | ESP32-C3 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# RMT Transmit Loop Example -- Step Motor controller

(See the README.md file in the upper level 'examples' directory for more information about examples.)

RMT peripheral can send customized RMT items in a loop, which means we can use it to generate a configurable length of periodic signal, with accurate number of pulses.

This example will show how to control an A4988 based step motor driver to step accurately with simple APIs, based on the RMT loop feature. The example also implements a [Smoothstep](https://en.wikipedia.org/wiki/Smoothstep) feature which works out of the box.

## How to Use Example

### Hardware Required

* Recommend running this example on development board with SOC chip that support loop auto-stop feature by hardware (e.g. ESP32-S3)
* A USB cable for Power supply and programming
* A 4-wire (A+, A-, B+, B-) step motor
* An A4988 module

Connection :

```
+----------------+             +--------------------+      +--------------+
|                |             |       A4988        |      |    4-wire    |
|            GND +-------------+ GND                |      |     Step     |
|                |             |                    |      |     Motor    |
|             5V +-------------+ VDD             1B +------+ A2           |
|                |             |                    |      |              |
|         GPIO18 +------------>+ DIRECTION       1A +------+ A1           |
|                |             |                    |      |              |
| ESP     GPIO17 +------------>+ STEP            2A +------+ B1           |
|                |             |                    |      |              |
|         GPIO16 +------------>+ SLEEP           2B +------+ B2           |
|                |             |                    |      +--------------+
|         GPIO15 +------------>+ RESET         VMOT +-------------------+
|                |             |                    |                   |
|          GPIO7 +------------>+ MS3            GND +----------+        |
|                |             |                    |          |        |
|          GPIO6 +------------>+ MS2                |          |        |
|                |             |                    |          |        |
|          GPIO5 +------------>+ MS1                |      +---+--------+-----+
|                |             |                    |      |  GND     +12V    |
|          GPIO4 +------------>+ ENABLE             |      |   POWER SUPPLY   |
+----------------+             +--------------------+      +------------------+

```

IO mapping on ESP side can be changed in `step_motor_main.c`:

```c
// GPIO configuration
#define STEP_MOTOR_DIRECTION_PIN GPIO_NUM_18
#define STEP_MOTOR_STEP_PIN GPIO_NUM_17
#define STEP_MOTOR_SLEEP_PIN GPIO_NUM_16
#define STEP_MOTOR_RESET_PIN GPIO_NUM_15
#define STEP_MOTOR_MS3_PIN GPIO_NUM_7
#define STEP_MOTOR_MS2_PIN GPIO_NUM_6
#define STEP_MOTOR_MS1_PIN GPIO_NUM_5
#define STEP_MOTOR_ENABLE_PIN GPIO_NUM_4
```

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example Output

```
I (344) step_motor: init
I (344) step_motor: set_step
I (1354) step_motor: step 10 @ 1000/s
I (2364) step_motor: step 100 @ 1000/s
I (3464) step_motor: step 1000 @ 1200/s
I (5294) step_motor: step 5000 @ 1400/s
I (9864) step_motor: smoothstep start 5000 steps @ 500~1400/s
I (14454) step_motor: smoothstep finish
I (15454) step_motor: continuous running for 5s
I (20454) step_motor: stop
I (21504) step_motor: deinit
```

Motor should move as output indicates.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
