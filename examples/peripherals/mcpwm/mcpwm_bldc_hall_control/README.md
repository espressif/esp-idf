| Supported Targets | ESP32 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# MCPWM BLDC Motor Control with HALL Sensor Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The MCPWM peripheral can generate three pairs of complementary PWMs by the internal dead time submodule, which is suitable for a BLDC motor application. This example demonstrates how to use the MCPWM peripheral to control a BLDC motor in a six-step commutation scheme.
We will change the on/off state of the six MOSFETs in a predefined order when the Hall sensor detects a change of the motor phase, so that the motor can spin in a predefined direction.

## How to Use Example

### Hardware Required

1. A ESP board with MCPWM peripheral supported (e.g. ESP32-S3-Motor-DevKit)
2. A BLDC motor whose commutation table is `6-->4-->5-->1-->3-->2-->6`
3. A three-phase gate driver, for example, the [DRV8302](https://www.ti.com.cn/product/zh-cn/DRV8302)
4. Six N-MOSFETs, for example, the [IRF540NS](https://www.infineon.com/cms/en/product/power/mosfet/12v-300v-n-channel-power-mosfet/irf540ns/)
5. A USB cable for Power supply and programming

Connection :

```
              +---------------------------------------------------------------------------------+
              |                                                                                 |
              |                             +---------------------------------------------+     |  VM
              |                             |                                             |     |   ^
              |                             |         +---------------------------+       |     |   |
              |                             |         |                           |       |     |   |
+-------------+-----------------------------+---------+-----------+      +--------+-------+-----+---++
|            GND           BLDC_DRV_FAULT_GPIO   BLDC_DRV_EN_GPIO |      |        EN    FAULT  GND   |
|                                                BLDC_PWM_UH_GPIO +------+PWM_UH                     |        +------------+
|                                                BLDC_PWM_UL_GPIO +------+PWM_UL                    U+--------+            |
|                                                                 |      |                           |        |            |
|                     ESP Board                  BLDC_PWM_VH_GPIO +------+PWM_VH                    V+--------+    BLDC    |
|                                                BLDC_PWM_VL_GPIO +------+PWM_VL   3-Phase Bridge    |        |            |
|                                                                 |      |               +          W+--------+            |
|                                                BLDC_PWM_WH_GPIO +------+PWM_WH      MOSFET         |        +-+---+---+--+
|                                                BLDC_PWM_WL_GPIO +------+PWM_WL                     |          |   |   |
|   HALL_CAP_W_GPIO    HALL_CAP_V_GPIO    HALL_CAP_U_GPIO         |      |                           |          |   |   |
+-----------+------------------+------------------+---------------+      +---------------------------+          |   |   |
            |                  |                  |       Hall U                                                |   |   |
            |                  |                  +-------------------------------------------------------------+   |   |
            |                  |                          Hall V                                                    |   |
            |                  +------------------------------------------------------------------------------------+   |
            |                                             Hall W                                                        |
            +-----------------------------------------------------------------------------------------------------------+
```

You can change the GPIO number in the [example code](main/mcpwm_bldc_hall_control_example_main.c) according to your board. You can define the spin direction in the code as well by the `BLDC_SPIN_DIRECTION_CCW` macro.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example Output

Run the example, you will see the following output log:

```
...
I (0) cpu_start: Starting scheduler on APP CPU.
I (307) example: Disable MOSFET gate
I (307) gpio: GPIO[46]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (317) example: Create MCPWM timer
I (317) example: Create MCPWM operator
I (327) example: Connect operators to the same timer
I (327) example: Create comparators
I (337) example: Create over current fault detector
I (337) gpio: GPIO[10]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (347) example: Set brake mode on the fault event
I (357) example: Create PWM generators
I (357) gpio: GPIO[47]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (367) gpio: GPIO[21]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (377) gpio: GPIO[14]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (387) gpio: GPIO[13]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (397) gpio: GPIO[12]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (407) gpio: GPIO[11]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (417) example: Set generator actions
I (417) example: Setup deadtime
I (427) example: Turn off all the gates by default
I (427) example: Create Hall sensor capture channels
I (437) gpio: GPIO[4]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (447) gpio: GPIO[5]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (457) gpio: GPIO[6]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (457) example: Register event callback for capture channels
I (467) example: Start a timer to adjust motor speed periodically
I (477) example: Enable MOSFET gate
I (477) example: Start the MCPWM timer
...
```

The BLDC motor will update the speed periodically.

## Troubleshooting

* Make sure your ESP board and H-bridge module have been connected to the same GND panel.
* Check the fault signal polarity, otherwise the motor will not spin if the MCPWM detector treats the normal level as a fault one.
* Don't use the PC USB as the power source of the BLDC motor (see the `VM` in the above connection diagram), it might damage your UAB port.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
