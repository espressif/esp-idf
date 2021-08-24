| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |

# HC-SR04 Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The capture module in MCPWM peripheral is designed to accurately log the time stamp on the hardware side when an event happens (compared to GPIO ISR which requires a software-based logging method). Each capture unit has three channels, which can be used together to capture IO events parallelly.
This example shows how to make use of the HW features to decode the pulse width signals generated from a common HC-SR04 sonar range finder -- [HC-SR04](https://www.sparkfun.com/products/15569).

The signal that HC-SR04 produces (and what can be handled by this example) is a simple pulse whose width indicates the measured distance. A pulse is required to send to HC-SR04 on `Trig` pin to begin a new measurement. Then the pulse described above will be sent back on `Echo` pin for decoding.

Typical signals:

```
Trig       +-----+
           |     |
           |     |
      -----+     +-----------------------
Echo                   +-----+
                       |     |
                       |     |
      -----------------+     +-----------

 +--------------------------------------->
                Timeline
```

## How to Use Example

### Hardware Required

* An ESP development board
* HC-SR04 module

Connection :

```
        +------+              +---------------------------------+
+-------+      |              |                                 |
|       |  VCC +--------------+ 5V                              |
+-------+      |              |                                 |
        + Echo +----=====>----+ GPIO18 (internal pull up)       |
        |      |              |                                 |
        + Trig +----<=====----+ GPIO19                          |
+-------|      |              |                                 |
|       |  GND +--------------+ GND                             |
+-------|      |              |                                 |
        +------+              +---------------------------------+
```

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (314) hc-sr04: HC-SR04 example based on capture function from MCPWM
I (324) hc-sr04: Echo pin configured
I (324) gpio: GPIO[19]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0 
I (334) hc-sr04: Trig pin configured
I (344) hc-sr04: trig task started
I (444) hc-sr04: Pulse width: 419us, Measured distance: 7.22cm
I (544) hc-sr04: Pulse width: 419us, Measured distance: 7.22cm
I (644) hc-sr04: Pulse width: 416us, Measured distance: 7.17cm
I (744) hc-sr04: Pulse width: 415us, Measured distance: 7.16cm
I (844) hc-sr04: Pulse width: 415us, Measured distance: 7.16cm
I (944) hc-sr04: Pulse width: 416us, Measured distance: 7.17cm
I (1044) hc-sr04: Pulse width: 391us, Measured distance: 6.74cm
```

This example runs at 10Hz sampling rate. out of range data is dropped and only valid measurement is printed.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
