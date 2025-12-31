| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Example: General Purpose Timer

This example illustrates how to use gptimer APIs to generate periodic alarm events and how different alarm actions behave on events.

## How to Use Example

### Hardware Required

* A development board with ESP SOC chip
* A USB cable for Power supply and programming

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [ESP-IDF Getting Started Guide](https://idf.espressif.com/) for all the steps to configure and use the ESP-IDF to build projects.
## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (326) example: Create timer handle
I (326) example: Enable timer
I (336) example: Start timer, stop it at alarm event
I (1336) example: Timer stopped, count=1000002
I (1336) example: Set count value
I (1336) example: Get count value
I (1346) example: Timer count value=100
I (1346) example: Disable timer
I (1346) example: Enable timer
I (1356) example: Start timer, auto-reload at alarm event
I (2356) example: Timer reloaded, count=2
I (3356) example: Timer reloaded, count=2
I (4356) example: Timer reloaded, count=2
I (5356) example: Timer reloaded, count=2
I (5356) example: Stop timer
I (5356) example: Disable timer
I (5356) example: Enable timer
I (5366) example: Start timer, update alarm value dynamically
I (6366) example: Timer alarmed, count=1000002
I (7366) example: Timer alarmed, count=2000002
I (8366) example: Timer alarmed, count=3000002
I (9366) example: Timer alarmed, count=4000002
I (9376) example: Stop timer
I (9376) example: Disable timer
I (9376) example: Delete timer
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
