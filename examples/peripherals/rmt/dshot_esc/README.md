| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |
# RMT Infinite Loop Transmit Example -- Dshot ESC (Electronic Speed Controller)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

RMT TX channel can transmit symbols in an infinite loop, where the loop is totally controlled by the hardware. This feature is useful for scenarios where a device needs continuous stimulus.

The [DShot](https://github.com/betaflight/betaflight/wiki/Dshot) is a digital protocol between flight controller (FC) and ESC, which is more resistant to electrical noise than traditional analog protocols. The DShot protocol requires the FC to encode throttle information into pulses with various durations and send out the pulses periodically. This is what an RMT TX channel can perfectly do.

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* A USB cable for Power supply and programming
* An ESC that supports DShot protocol (this example will take the **DShot300** as an example)

Connection :

```
   BLDC           DShot ESC        12V GND
+--------+     +---------------+    |   |           ESP
|        |     |               |    |   | +----------------------+
|      U +-----+ U          P+ +----+   | |                      |
|        |     |               |        | |                      |
|      V +-----+ V          P- +--------+ |                      |
|        |     |               |          |                      |
|      W +-----+ W         SIG +----------+ DSHOT_ESC_GPIO_NUM   |
|        |     |           GND +----------+ GND                  |
+--------+     +---------------+          +----------------------+
```

The GPIO number used in this example can be changed according to your board, by the macro `DSHOT_ESC_GPIO_NUM` defined in the [source file](main/dshot_esc_example_main.c).

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Console Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (182) example: Create RMT TX channel
I (182) gpio: GPIO[43]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (182) example: Install Dshot ESC encoder
I (182) example: Start RMT TX channel
I (182) example: Start ESC by sending zero throttle for a while...
I (3182) example: Set throttle to 1000, no telemetry
```

The BLDC motor will beep when the ESC receives a burst of initialization pulses. And then starts high-speed rotation at the throttle set in the code.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
