| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# Rotary Encoder Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The PCNT peripheral is designed to count the number of rising and/or falling edges of an input signal. Each PCNT unit has two channels, which makes it possible to extract more information from two input signals than only one signal.
This example shows how to make use of the HW features to decode the differential signals generated from a common rotary encoder -- [EC11](https://tech.alpsalpine.com/prod/e/html/encoder/incremental/ec11/ec11_list.html).

The signals a rotary encoder produces (and what can be handled by this example) are based on a 2-bit gray code available on 2 digital data signal lines. The typical encoders use 3 output pins: 2 for the signals and one for the common signal usually GND.

Typical signals:

```
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

```
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

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (181323) example: Encoder value: 0
I (182323) example: Encoder value: 0
I (183323) example: Encoder value: -12
I (184323) example: Encoder value: -18
I (185323) example: Encoder value: -24
I (188323) example: Encoder value: 4
I (189323) example: Encoder value: 8
I (190323) example: Encoder value: 8
I (191323) example: Encoder value: 8
```

This example enables the 4X mode to parse the rotary signals, which means, each complete rotary step will result PCNT counter to increase/decrease by 4, depending on the direction of rotation.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
