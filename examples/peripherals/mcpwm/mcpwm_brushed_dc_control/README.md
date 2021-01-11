| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |
# MCPWM Brushed DC Motor Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example mainly illustrates how to drive a brushed DC motor by generating two specific PWM signals. We used [L298N](https://www.st.com/content/st_com/en/products/motor-drivers/brushed-dc-motor-drivers/l298.html) as the H-bridge driver to provide the needed voltage and current for brushed DC motor.

## How to Use Example

### Hardware Required

* A development board with any Espressif SoC which features MCPWM peripheral (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* A separate 12V power for brushed DC (the voltage depends on the motor model used in the example)
* A brushed DC motor, e.g. [25GA370](http://www.tronsunmotor.com/data/upload/file/201807/e03b98802b5c5390d6570939def525ba.pdf)

Connection :
```
                                                       Power (12V)
                                                            ^
                                                            |
+----------------+                             +------------+--------------+                      +-------------+
|                |                             |                           |                      |             |
|          GPIO15+------ PWM0A +-+ IN_A +------+                           +-------+ OUT_A +------+   Brushed   |
|   ESP          |                             |          H-Bridge         |                      |     DC      |
|          GPIO16+------ PWM0B +-+ IN_B +------+                           +-------+ OUT_B +------+    Motor    |
|                |                             |                           |                      |             |
+--------+-------+                             +------------+--------------+                      +-------------+
         |                                                  |
         +------------------------------------------------->+
                                                            |
                                                            v
                                                           GND
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
I (350) example: running forward
I (2350) example: running backward
I (4350) example: stop
I (6350) example: running forward
I (8350) example: running backward
I (10350) example: stop
...
```

Motor first moves forward, then backward and then stops for 2 seconds each, continuously.

## Troubleshooting

* Make sure your ESP board and H-bridge module have been connected to the same GND panel.

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
