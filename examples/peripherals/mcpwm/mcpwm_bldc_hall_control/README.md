| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |

# MCPWM BLDC Hall motor control Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example will illustrate how to use MCPWM driver to control BLDC motor with hall sensor feedback. In the example, a timer is running at the background to update the motor speed periodically.

With the hardware fault detection feature of MCPWM, the example will shut down the MOSFETs when over current happens.

## How to Use Example

### Hardware Required

1. The BLDC motor used in this example has a hall sensor capture sequence of `6-->4-->5-->1-->3-->2-->6-->4-->` and so on.
2. A three-phase gate driver, this example uses [IR2136](https://www.infineon.com/cms/en/product/power/gate-driver-ics/ir2136s/).
3. Six N-MOSFETs, this example uses [IRF540NS](https://www.infineon.com/cms/en/product/power/mosfet/12v-300v-n-channel-power-mosfet/irf540ns/).
4. A development board with any Espressif SoC which features MCPWM peripheral (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
5. A USB cable for Power supply and programming.

Connection :

```
          ┌─────────────────────────────────────────────┐
          │                                             │
          │         ┌───────────────────────────┐       │
          │         │                           │       │
┌─────────┴─────────┴───────────┐      ┌────────┴───────┴──────────┐
│      GPIO19      GPIO18       │      │        EN    FAULT        │
│                         GPIO21├──────┤PWM_UH                     │        ┌────────────┐
│                         GPIO22├──────┤PWM_UL                    U├────────┤            │
│                               │      │                           │        │            │
│                         GPIO23├──────┤PWM_VH                    V├────────┤    BLDC    │
│         ESP Board       GPIO25├──────┤PWM_VL   3-Phase Bridge    │        │            │
│                               │      │               +          W├────────┤            │
│                         GPIO26├──────┤PWM_WH      MOSFET         │        └─┬───┬───┬──┘
│                         GPIO27├──────┤PWM_WL                     │          │   │   │
│    GPIO5  GPIO4  GPIO2        │      │                           │          │   │   │
└─────┬──────┬──────┬───────────┘      └───────────────────────────┘          │   │   │
      │      │      │   Hall U                                                │   │   │
      │      │      └─────────────────────────────────────────────────────────┘   │   │
      │      │          Hall V                                                    │   │
      │      └────────────────────────────────────────────────────────────────────┘   │
      │                 Hall W                                                        │
      └───────────────────────────────────────────────────────────────────────────────┘
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
I (327) example: Disable gate driver
I (327) gpio: GPIO[18]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (337) example: Setup PWM and Hall GPIO (pull up internally)
I (347) example: Initialize PWM (default to turn off all MOSFET)
I (357) example: Initialize over current fault action
I (357) example: Initialize Hall sensor capture
I (367) example: Please turn on the motor power
I (5367) example: Enable gate driver
I (5367) example: Changing speed at background
...
```

## Dive into the example

1. How to change the rotation direction?

      The rotation direction is controlled by how the hall sensor value is parsed. If you pass `false` to `bldc_get_hall_sensor_value`, the BLDC should rotate in clock wise. Likewise, passing `true` to that function will make tha BLDC rotate in counter clock wise.
