# RMT Transmit Example -- LED Strip

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Although RMT peripheral is mainly designed for infrared remote applications, it can also support other generic protocols thanks to its flexible data format. [WS2812](http://www.world-semi.com/Certifications/WS2812B.html) is a digital RGB LED which integrates a driver circuit and a single control wire. The protocol data format defined in WS2812 is compatible to that in RMT peripheral. This example will illustrate how to drive an WS2812 LED strip based on the RMT driver.

## How to Use Example

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* A WS2812 LED strip

Connection :

```
                             --- 5V
                              |
                              +
GPIO18 +-----------------+---|>| (WS2812)
                        DI    +
                              |
                             --- GND
```

### Configure the Project

Open the project configuration menu (`idf.py menuconfig`). 

In the `Example Configuration` menu:

* Set the GPIO number used for transmitting the IR signal under `RMT TX GPIO` option.
* Set the number of LEDs in a strip under `Number of LEDS in a strip` option.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

Connect the `DI` signal of WS2812 LED strip to the GPIO you set in menuconfig.

Run the example, you will see a rainbow chasing demonstration effect. To change the chasing speed, you can update the `EXAMPLE_CHASE_SPEED_MS` value in `led_strip_main.c` file.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
