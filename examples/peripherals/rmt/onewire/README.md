| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Advanced RMT Transmit & Receive Example -- Simulate 1-Wire Bus

(See the README.md file in the upper level 'examples' directory for more information about examples.)

RMT peripheral has independent transmit and receive channels. We can simulate the [1-Wire](https://www.analog.com/en/technical-articles/guide-to-1wire-communication.html) bus by attaching a pair of transmit and receive channel to the same GPIO, and turning on the open-drain mode of the GPIO pad.

We've made the 1-Wire protocol implementation into a component called `onewire_bus`, which has been uploaded to the [component registry](https://components.espressif.com/components/espressif/onewire_bus).

This example demonstrates how to use that `onewire_bus` library to read temperature from the [DS18B20](https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf) sensor. Likewise, the DS18B20 device is also made as a single component and pushed to the [component registry](https://components.espressif.com/components/espressif/ds18b20).

One of the amazing feature that offered by the `onewire_bus` driver is that, is can support enumerate the devices on the bus, thus you can connect multiple DS18B20 sensors to the same bus and read their temperature one by one.

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* One or more DS18B20 sensors connected to the same bus by a 4.7 KΩ pull-up resistor

Connection :

```plain
┌──────────────────────────┐
│                      3.3V├───────┬─────────────┬──────────────────────┐
│                          │      ┌┴┐            │VDD                   │VDD
│          ESP Board       │  4.7k│ │     ┌──────┴──────┐        ┌──────┴──────┐
│                          │      └┬┘   DQ│             │      DQ│             │
│          ONEWIRE_GPIO_PIN├───────┴──┬───┤   DS18B20   │    ┌───┤   DS18B20   │   ......
│                          │          └───│-------------│────┴───│-------------│──
│                          │              └──────┬──────┘        └──────┬──────┘
│                          │                     │GND                   │GND
│                       GND├─────────────────────┴──────────────────────┘
└──────────────────────────┘
```

The GPIO number used in this example can be changed according to your board, by the macro `EXAMPLE_ONEWIRE_BUS_GPIO` defined in [onewire_example_main.c](main/onewire_example_main.c).

> **Note**
> Parasite power mode is not supported, you have to connect VDD pin to make DS18B20 functional.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Console Output

```plain
I (340) main_task: Started on CPU0
I (350) main_task: Calling app_main()
I (350) gpio: GPIO[0]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (360) gpio: GPIO[0]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (370) example: 1-Wire bus installed on GPIO0
I (370) example: Device iterator created, start searching...
I (490) example: Found a DS18B20[0], address: 070822502019FC28
I (590) example: Found a DS18B20[1], address: FC0921C076034628
I (590) example: Searching done, 2 DS18B20 device(s) found
I (1620) example: temperature read from DS18B20[0]: 22.50C
I (2430) example: temperature read from DS18B20[1]: 22.81C
I (3440) example: temperature read from DS18B20[0]: 22.50C
I (4250) example: temperature read from DS18B20[1]: 22.81C
I (5260) example: temperature read from DS18B20[0]: 22.50C
I (6070) example: temperature read from DS18B20[1]: 22.81C
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
