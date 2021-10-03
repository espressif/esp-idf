# Example: General Purpose Timer

This example uses the timer group driver to generate timer interrupts at two specified alarm intervals.

## How to Use Example

### Hardware Required

* A development board with ESP SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [ESP-IDF Getting Started Guide](https://idf.espressif.com/) for all the steps to configure and use the ESP-IDF to build projects.
## Example Output

```
Timer Group with auto reload
Group[0], timer[0] alarm event
------- EVENT TIME --------
Counter: 0x0000000000000008
Time   : 0.00000160 s
-------- TASK TIME --------
Counter: 0x0000000000004ed8
Time   : 0.00403680 s
Timer Group without auto reload
Group[1], timer[0] alarm event
------- EVENT TIME --------
Counter: 0x00000000017d7848
Time   : 5.00000160 s
-------- TASK TIME --------
Counter: 0x00000000017dcb32
Time   : 5.00424680 s
Timer Group with auto reload
Group[0], timer[0] alarm event
------- EVENT TIME --------
Counter: 0x0000000000000008
Time   : 0.00000160 s
-------- TASK TIME --------
Counter: 0x0000000000004dd4
Time   : 0.00398480 s
```

## Functionality Overview

* Two timers are configured
* Each timer is set with some sample alarm interval
* On reaching the interval value each timer will generate an alarm
* One of the timers is configured to automatically reload it's counter value on the alarm
* The other timer is configured to keep incrementing and is reloaded by the application each time the alarm happens
* Alarms trigger subsequent interrupts, that is tracked with messages printed on the terminal:

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
