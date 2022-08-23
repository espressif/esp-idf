| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# RMT Transmit & Receive Example -- 1-Wire bus

(See the README.md file in the upper level 'examples' directory for more information about examples.)

RMT peripheral has both transmit and receive channels. Connecting one transmit channel and one receive channel to the same GPIO and put the GPIO in open-drain mode can simulate bi-directional single wire protocols, such as [1-Wire protocol](https://www.maximintegrated.com/en/design/technical-documents/tutorials/1/1796.html).

This example demonstrates how to use RMT to simulate 1-Wire bus and read temperatrue from [DS18B20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf).

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* Several DS18B20 temperature sensors and a 4.7kohm pullup resistor

Connection :

```
┌──────────────────────────┐
│                      3.3V├───────┬─────────────┬──────────────────────┐
│                          │      ┌┴┐            │VDD                   │VDD
│        ESP32 Board       │  4.7k│ │     ┌──────┴──────┐        ┌──────┴──────┐
│                          │      └┬┘   DQ│             │      DQ│             │
│          ONEWIRE_GPIO_PIN├───────┴──┬───┤   DS18B20   │    ┌───┤   DS18B20   │   ......
│                          │          └───│-------------│────┴───│-------------│──
│                          │              └──────┬──────┘        └──────┬──────┘
│                          │                     │GND                   │GND
│                       GND├─────────────────────┴──────────────────────┘
└──────────────────────────┘
```

The GPIO number used in this example can be changed according to your board, by the macro `EXAMPLE_ONEWIRE_GPIO_PIN` defined in [onewire_ds18b20_example_main.c](main/onewire_ds18b20_example_main.c).

*Note*: Parasite power mode is not supported currently by this example, you have to connect VDD pin to make DS18B20 functional.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Console Output

If there are some DS18B20s on the bus:

```
I (327) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (338) gpio: GPIO[5]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (348) onewire_rmt: RMT Tx channel created for 1-wire bus
I (358) gpio: GPIO[5]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (358) onewire_rmt: RMT Rx channel created for 1-wire bus
I (368) example: 1-wire bus installed
I (418) example: found device with rom id 28FF30BE21170317
I (458) example: found device with rom id 28FF297E211703A1
I (498) example: found device with rom id 28FF6F7921170352
I (508) example: 3 devices found on 1-wire bus
I (2518) example: temperature of device 28FF30BE21170317: 27.00C
I (2528) example: temperature of device 28FF297E211703A1: 26.81C
I (2538) example: temperature of device 28FF6F7921170352: 26.50C
I (3548) example: temperature of device 28FF30BE21170317: 26.94C
I (3558) example: temperature of device 28FF297E211703A1: 26.75C
I (3568) example: temperature of device 28FF6F7921170352: 26.44C
```

If there is no DS18B20 on the bus:

```
I (327) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (337) gpio: GPIO[5]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (347) onewire_rmt: RMT Tx channel created for 1-wire bus
I (357) gpio: GPIO[5]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (357) onewire_rmt: RMT Rx channel created for 1-wire bus
I (367) example: 1-wire bus installed
E (377) onewire_rmt: no device present on 1-wire bus
I (377) example: 0 device found on 1-wire bus
I (387) gpio: GPIO[5]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (397) gpio: GPIO[5]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (397) example: 1-wire bus deleted
```

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
