| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |
# MCPWM Brushed DC Motor Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example mainly illustrates how to drive a brushed DC motor by generating two specific PWM signals. However the PWM signals from ESP32 can't drive motors directly as the motor usually consumes high current. So an H-bridge like [DRV8848](https://www.ti.com/product/DRV8848) should be used to provide the needed voltage and current for brushed DC motor. To measure the speed of motor, a photoelectric encoder is used to generate the "speed feedback" signals (e.g. a pair of quadrature signal). The example uses a simple PID control approach to keep the motor speed in a constant speed. The example provides a console command line interface for user to update the PID parameters according to actual situation.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with any Espressif SoC which features MCPWM and PCNT peripheral (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* A separate 12V power supply for brushed DC motor and H-bridge (the voltage depends on the motor model used in the example)
* A motor driving board to transfer pwm signal into driving signal
* A brushed DC motor, e.g. [25GA370](http://www.tronsunmotor.com/data/upload/file/201807/e03b98802b5c5390d6570939def525ba.pdf)
* A quadrature encoder to detect speed

Connection :
```
                                     Power(12V)
                                         |
                                         v
+----------------+             +--------------------+
|                |             |      H-Bridge      |
|            GND +<----------->| GND                |      +--------------+
|                |             |                    |      |              |
|  GENA_GPIO_NUM +----PWM0A--->| IN_A         OUT_A +----->|   Brushed    |
|                |             |                    |      |     DC       |
|  GENB_GPIO_NUM +----PWM0B--->| IN_B         OUT_B +----->|    Motor     |
|                |             |                    |      |              |
| ESP            |             +--------------------+      |              |
|                |                                         +------+-------+
|                |                                                |
|                |             +--------------------+             |
|         VCC3.3 +------------>| VCC    Encoder     |             |
|                |             |                    |             |
|            GND +<----------->|                    |<------------+
|                |             |                    |
|PHASEA_GPIO_NUM |<---PhaseA---+ C1                 |
|                |             |                    |
|PHASEB_GPIO_NUM |<---PhaseB---+ C2                 |
|                |             |                    |
+----------------+             +--------------------+
```

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.


## Example Output

Run the example, you will see the following output log:

```
I (0) cpu_start: Starting scheduler on APP CPU.
configure mcpwm gpio
init mcpwm driver
init and start rotary encoder
init PID control block
init motor control timer
D (561) gptimer: new group (0) @0x3fce0a24
D (561) gptimer: new gptimer (0,0) at 0x3fce0964, resolution=1000000Hz
create motor control task
start motor control timer
D (571) gptimer: install interrupt service for timer (0,0)
install console command line

Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
dc-motor>
dc-motor> help
help
  Print the list of registered commands

pid  [-p <kp>] [-i <ki>] [-d <kd>]
  Set PID parameters
       -p <kp>  Set Kp value of PID
       -i <ki>  Set Ki value of PID
       -d <kd>  Set Kd value of PID
```

### Set PID parameters

* Command: `pid -p <double> -i <double> -d <double> -t <loc/inc>`
* 'p' - proportion value
* 'i' - integral value
* 'd' - differential value
* 't' - PID calculation type (locational or incremental).

```bash
mcpwm-motor> pid -p 0.8 -i 0.02 -d 0.1 -t inc
pid: kp = 0.800
pid: ki = 0.020
pid: kd = 0.100
pid: type = increment
```

## Troubleshooting

* Make sure your ESP board and H-bridge module have been connected to the same GND panel.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
