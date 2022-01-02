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
I (323) example: Create timer handle
I (323) example: Start timer, stop it at alarm event
I (1333) example: Timer stopped, count=1000002
I (1333) example: Set count value
I (1333) example: Get count value
I (1333) example: Timer count value=100
I (1343) example: Start timer, auto-reload at alarm event
I (2343) example: Timer reloaded, count=2
I (3343) example: Timer reloaded, count=2
I (4343) example: Timer reloaded, count=2
I (5343) example: Timer reloaded, count=2
I (5343) example: Stop timer
I (5343) example: Update alarm value dynamically
I (6353) example: Timer alarmed, count=1000002
I (7353) example: Timer alarmed, count=2000002
I (8353) example: Timer alarmed, count=3000002
I (9353) example: Timer alarmed, count=4000002
I (9353) example: Stop timer
I (9353) example: Delete timer
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
