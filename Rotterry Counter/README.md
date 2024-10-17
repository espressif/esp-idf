| Supported Targets | ESP32 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Rotary Encoder Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The PCNT peripheral is designed to count the number of rising and/or falling edges of an input signal. Each PCNT unit has two channels, which makes it possible to extract more information from two input signals than only one signal.

This example shows how to make use of the HW features to decode the differential signals generated from a common rotary encoder -- [EC11](https://tech.alpsalpine.com/e/products/category/encorder/sub/01/series/ec11e/).

The signals a rotary encoder produces (and what can be handled by this example) are based on a 2-bit gray code available on 2 digital data signal lines. The typical encoders use 3 output pins: 2 for the signals and one for the common signal usually GND.

Typical signals:

```text
A      +-----+     +-----+     +-----+
             |     |     |     |
             |     |     |     |
             +-----+     +-----+
B         +-----+     +-----+     +-----+
                |     |     |     |
                |     |     |     |
                +-----+     +-----+

 +--------------------------------------->
                CW direction
```

## How to Use Example

### Hardware Required

* An ESP development board
* EC11 rotary encoder (or other encoders which can produce quadrature waveforms)

Connection :

```text
      +--------+              +---------------------------------+
      |        |              |                                 |
      |      A +--------------+ GPIO_A (internal pull up)       |
      |        |              |                                 |
+-------+      |              |                                 |
|     | |  GND +--------------+ GND                             |
+-------+      |              |                                 |
      |        |              |                                 |
      |      B +--------------+ GPIO_B (internal pull up)       |
      |        |              |                                 |
      +--------+              +---------------------------------+
```

The GPIO used by the example can be changed according to your board by `EXAMPLE_EC11_GPIO_A` and `EXAMPLE_EC11_GPIO_B` in [source file](main/rotary_encoder_example_main.c).

### Build and Flash

By configuring one of the EC11 GPIO (e.g. `EXAMPLE_EC11_GPIO_A`) as a wake up source, you can make the rotary encoder wake the system from light sleep. This example can illustrate this feature if you enable the `EXAMPLE_WAKE_UP_LIGHT_SLEEP` from the menuconfig.

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
I (0) cpu_start: Starting scheduler on APP CPU.
I (325) example: install pcnt unit
I (335) example: set glitch filter
I (345) example: install pcnt channels
I (395) example: set edge and level actions for pcnt channels
I (405) example: add watch points and register callbacks
I (405) example: clear pcnt unit
I (415) example: start pcnt unit
I (1415) example: Pulse count: 0
I (2415) example: Pulse count: 8
I (3415) example: Pulse count: 27
I (4415) example: Pulse count: 40
I (4705) example: Watch point event, count: 50
I (5705) example: Pulse count: 72
I (6705) example: Pulse count: 96
I (6785) example: Watch point event, count: 100
I (6785) example: Watch point event, count: 0
I (7785) example: Pulse count: 8
I (8785) example: Pulse count: 8
I (9225) example: Watch point event, count: 0
I (10225) example: Pulse count: -20
I (11225) example: Pulse count: -28
I (12225) example: Pulse count: -48
I (12995) example: Watch point event, count: -50
I (13995) example: Pulse count: -68
I (14995) example: Pulse count: -82
I (15995) example: Pulse count: -92
I (16875) example: Watch point event, count: -100
I (16875) example: Watch point event, count: 0
I (17875) example: Pulse count: -12
I (18875) example: Pulse count: -12
I (19875) example: Pulse count: -12
```

This example enables the 4X mode to parse the rotary signals, which means, each complete rotary step will result in PCNT counter increasing or decreasing by 4, depending on the direction of rotation.
The example adds five watch points, events will be triggered when counter reaches to any watch point.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
