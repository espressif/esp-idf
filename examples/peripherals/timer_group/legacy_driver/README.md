| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Example: General Purpose Timer

This example uses the **legacy timer group driver** to generate timer interrupts with and without auto-reload. But we highly recommend you to use the new [GPTimer Driver](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gptimer.html) in your new projects.

## How to Use Example

### Hardware Required

* A development board with ESP SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [ESP-IDF Getting Started Guide](https://idf.espressif.com/) for all the steps to configure and use the ESP-IDF to build projects.

## Example Output

```text
I (0) cpu_start: Starting scheduler on APP CPU.
I (325) example: Init timer with auto-reload
I (835) example: Timer auto reloaded, count value in ISR: 3
I (1335) example: Timer auto reloaded, count value in ISR: 3
I (1835) example: Timer auto reloaded, count value in ISR: 3
I (2335) example: Timer auto reloaded, count value in ISR: 3
I (2335) example: Init timer without auto-reload
I (2835) example: Timer alarmed at 500003
I (3335) example: Timer alarmed at 1000003
I (3835) example: Timer alarmed at 1500003
I (4335) example: Timer alarmed at 2000003
```

## Functionality Overview

* Configure one timer with auto-reload enabled, alarm period set to 0.5s
* On reaching the interval value the timer will generate an alarm
* The timer will reload with initial count value on alarm, by hardware
* Reconfigure the timer with auto-reload disabled, initial alarm value set to 0.5s
* The timer keeps incrementing and in the alarm callback, the software reconfigures its alarm value by increasing 0.5s
* The main task will print the count value that captured in the alarm callback

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
