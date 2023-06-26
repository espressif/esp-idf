| Supported Targets | ESP32 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# HC-SR04 Example based on MCPWM Capture

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The capture module in MCPWM peripheral is designed to accurately log the time stamp on the hardware side when an event happens (compared to GPIO ISR which requires a software-based logging method). Each capture unit has three channels, which can be used together to capture IO events in parallel.

This example shows how to make use of the hardware features to decode the pulse width signals generated from a common HC-SR04 sonar sensor -- [HC-SR04](https://www.sparkfun.com/products/15569).

The signal that HC-SR04 produces (and what can be handled by this example) is a simple pulse whose width indicates the measured distance. An excitation pulse is required to send to HC-SR04 on `Trig` pin to begin a new measurement. Then the pulse described above will appear on the `Echo` pin after a while.

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

* An ESP development board that features the MCPWM peripheral
* An HC-SR04 sensor module

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
I (0) cpu_start: Starting scheduler on APP CPU.
I (304) example: Create capture queue
I (304) example: Install capture timer
I (304) example: Install capture channel
I (314) gpio: GPIO[2]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (324) example: Register capture callback
I (324) example: Create a timer to trig HC_SR04 periodically
I (334) example: Configure Trig pin
I (334) gpio: GPIO[0]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (344) example: Enable and start capture timer
I (434) example: Pulse width: 189.02us, Measured distance: 3.26cm
I (534) example: Pulse width: 189.02us, Measured distance: 3.26cm
I (634) example: Pulse width: 189.01us, Measured distance: 3.26cm
I (734) example: Pulse width: 188.98us, Measured distance: 3.26cm
I (834) example: Pulse width: 188.99us, Measured distance: 3.26cm
```

This example runs at 10Hz sampling rate. Measure data that out of the range is dropped and only valid measurement is printed out.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
