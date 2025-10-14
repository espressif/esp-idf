| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# Parallel IO TX Example: Advancde RGB LED Matrix

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The common used LED Matrix board has a so-called HUB75 interface, which is a 16-bit parallel interface. This example shows how to use the Parallel IO TX unit to drive a RGB LED Matrix board. We take use of the Parallel IO TX unit to send the RGB data and control signals and  line address to the LED Matrix board.

In the other [basic RGB LED Matrix](../simple_rgb_led_matrix) example, we use the CPU to continuously send refresh transactions for each row. In this advanced example, we let the hardware automatically refresh the entire screen content continuously, without CPU intervention.

The example uses the [LVGL](https://lvgl.io/) library to draw some simple UI elements on the LED Matrix board.

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* A USB cable for programming
* A RGB LED Matrix board, with HUB75 interface (this example will use the [`64*32` resolution version](https://www.waveshare.net/wiki/RGB-Matrix-P4-64x32))
* A 5V-2A power supply for the LED Matrix board

Connection :

```plain_text
             +-----------------------+
             |      HUB75            |
             | +-------------+       |
             | |             |       |
PIN_NUM_R1---+-+---R1     G1 +-------+---PIN_NUM_G1
             | |             |       |
PIN_NUM_B1---+-+---B1  +-GND |       |
             | |       |     |       |
PIN_NUM_R2---+-+-+-R2  |  G2 +-------+---PIN_NUM_G2
             |   |     |     |       |
PIN_NUM_B2---+---+-B2  +-GND-+-------+---->GND
             |   |     |     |       |
PIN_NUM_A----+---+-A   |   B +-------+---PIN_NUM_B
             |   |     |     |       |
PIN_NUM_C----+-+-+-C   |   D +-------+---PIN_NUM_D
             | |       |     |       |
PIN_NUM_CLK--+-+---CLK | LAT +-------+---PIN_NUM_LAT
             | |       |     |       |
PIN_NUM_OE---+-+---OE  +-GND |       |
             | |             |  PWR--+---->5V
             | +-------------+       |
             |    RGB LED Matrix     |
             +-----------------------+
```

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Console Output

```plain_text
I (348) main_task: Started on CPU0
I (348) main_task: Calling app_main()
I (358) example: Install parallel IO TX unit
I (458) example: Initialize LVGL library
I (468) example: Install LVGL tick timer
I (468) example: Display LVGL UI
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
