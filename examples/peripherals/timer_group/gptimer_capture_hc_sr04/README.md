| Supported Targets | ESP32-C6 | ESP32-H2 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |

# HC-SR04 Example based on GPTimer Capture and ETM

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The general purpose timer can capture internal timer count value into dedicated registers. This feature can be used to measure the time between two events. With the help of ETM (Event Task Matrix) peripheral, we can route different events to trigger the GPTimer's capture task. In this example, we configure one GPIO to detect any edge signals and then notify the GPTimer to capture the timestamp at once, all done by hardware.

This example also shows how to decode the pulse width signals generated from a common HC-SR04 sonar sensor -- [HC-SR04](https://www.sparkfun.com/products/15569).

The signal that HC-SR04 produces (and what can be handled by this example) is a simple pulse whose width indicates the measured distance. An excitation pulse is required to send to HC-SR04 on `Trig` pin to begin a new measurement. Then the pulse described above will appear on the `Echo` pin after a while.

Typical signals:

```text
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

* A development board with ESP SOC chip that supports ETM peripheral
* An HC-SR04 sensor module

Connection :

```text
        +------+              +--------------------------------------+
+-------+      |              |                                      |
|       |  VCC +--------------+ 5V                                   |
+-------+      |              |                                      |
        + Echo +----=====>----+ HC_SR04_ECHO_GPIO (internal pull up) |
        |      |              |                                      |
        + Trig +----<=====----+ HC_SR04_TRIG_GPIO                    |
+-------|      |              |                                      |
|       |  GND +--------------+ GND                                  |
+-------|      |              |                                      |
        +------+              +--------------------------------------+
```

You can change the GPIO number according to your board, by `HC_SR04_TRIG_GPIO` and `HC_SR04_ECHO_GPIO` in the [source code](main/gptimer_capture_hc_sr04.c).

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```text
I (339) cpu_start: Starting scheduler.
I (344) example: Configure trig gpio
I (344) gpio: GPIO[0]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (354) example: Configure echo gpio
I (354) gpio: GPIO[1]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:3
I (364) example: Create etm event handle for echo gpio
I (374) example: Create gptimer handle
I (384) example: Get gptimer etm task handle (capture)
I (384) example: Create ETM channel then connect gpio event and gptimer task
I (394) example: Install GPIO edge interrupt
I (404) example: Enable etm channel and gptimer
I (404) example: Start gptimer
I (424) example: Measured distance: 93.40cm
I (934) example: Measured distance: 103.93cm
I (1444) example: Measured distance: 102.09cm
I (1954) example: Measured distance: 122.47cm
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
