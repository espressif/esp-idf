| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |
# MCPWM RC Servo Control Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example illustrates how to drive a typical [RC Servo](https://en.wikipedia.org/wiki/Servo_%28radio_control%29) by sending a PWM signal using the MCPWM driver. The PWM pulse has a frequency of 50Hz (period of 20ms), and the active-high time (which controls the rotation) ranges from 1ms to 2ms with 1.5ms always being center of range.

## How to Use Example

### Hardware Required

* A development board with any Espressif SoC which features MCPWM peripheral (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* A RC servo motor, e.g. [SG90](http://www.ee.ic.ac.uk/pcheung/teaching/DE1_EE/stores/sg90_datasheet.pdf)

Connection :

```
+-------+                                     +-----------------+
|       |                                     |                 |
|       +-+ GPIO18++  PWM(Orange)  +----------+                 |
|  ESP  |---5V------+ Vcc(Red) +--------------|   Servo Motor   |
|       +---------+   GND(Brown)   +----------+                 |
|       |                                     |                 |
+-------+                                     +-----------------+
```

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example Output

Run the example, you will see the following output log:

```
...
I (0) cpu_start: Starting scheduler on APP CPU.
I (349) example: Angle of rotation: -90
I (449) example: Angle of rotation: -89
I (549) example: Angle of rotation: -88
I (649) example: Angle of rotation: -87
I (749) example: Angle of rotation: -86
...
```

The servo will rotate from -90 degree to 90 degree, and then turn back again.

## Troubleshooting

Note that, some kind of servo might need a higher current supply than the development board usually can provide. It's recommended to power the servo separately.

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
