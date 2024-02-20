| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# Parallel IO TX Example: Simple RGB LED Matrix

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The common used LED Matrix board has a so-called HUB75 interface, which is a 16-bit parallel interface. This example shows how to use the Parallel IO TX unit to drive a RGB LED Matrix board. We take use of the Parallel IO TX unit to send the RGB data and control signals to the LED Matrix board, and use the fast GPIO bundle to control the line address.

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

The GPIOs used in this example are defined in the [source file](main/rgb_led_matrix_example_main.c). You can change them according to your hardware.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Console Output

```plain_text
I (348) main_task: Started on CPU0
I (348) main_task: Calling app_main()
I (348) example: Install fast GPIO bundle for line address control
I (358) example: Install parallel IO TX unit
I (368) gpio: GPIO[0]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (368) gpio: GPIO[1]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (378) gpio: GPIO[2]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (388) gpio: GPIO[3]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (398) gpio: GPIO[4]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (408) gpio: GPIO[5]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (418) gpio: GPIO[6]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (428) gpio: GPIO[7]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (438) gpio: GPIO[10]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (458) example: Initialize LVGL library
I (458) example: Register display driver to LVGL
I (468) example: Install LVGL tick timer
I (468) example: Display LVGL UI
```

## Limitations

:warning: Because of the hardware limitation in ESP32-C6 and ESP32-H2, the transaction length can't be controlled by DMA, thus we can't flush the LED screen continuously within a hardware loop. Instead, this example uses a task to split the screen flush into multiple line transactions. This obviously will increase the CPU load and may also lead to a task watchdog timeout. So this example **disables** the task watchdog by default.

Please note that, the example is only for illustrating the usage of parallel IO TX APIs, it's far from ideal for use in a real product.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
