| Supported Targets | ESP32 | ESP32-S3 |
| ----------------- | ----- | -------- |
# MCPWM Brushed DC Motor Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example mainly illustrates how to drive a brushed DC motor by generating two specific PWM signals. This example assumes an [L298N](https://www.st.com/content/st_com/en/products/motor-drivers/brushed-dc-motor-drivers/l298.html) H-bridge driver is used to provide the needed voltage and current for brushed DC motor. This example also implements a motor control command console such that users can configure and control the motors at run time using console commands.

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
|            GND +------------>|                    |      +--------------+
|                |             |                    |      |              |
|         GPIO15 +----PWM0A--->| IN_A         OUT_A +----->|   Brushed    |
|                |             |                    |      |     DC       |
|         GPIO16 +----PWM0B--->| IN_A         OUT_B +----->|    Motor     |
|                |             |                    |      |              |
| ESP            |             +--------------------+      |              |
|                |                                         +------+-------+
|                |                                                |
|                |             +--------------------+             |
|         VCC3.3 +------------>|      Encoder       |             |
|                |             |                    |             |
|            GND +------------>|                    |<------------+
|                |             |                    |
|         GPIO18 |<---PhaseA---+ C1                 |
|                |             |                    |
|         GPIO19 |<---PhaseB---+ C2                 |
|                |             |                    |
+----------------+             +--------------------+
```
NOTE: If some other GPIO pins (e.g., 13/14) are chosen as the PCNT encoder pins, flashing might fail while the wires are connected. If this occurs, please try disconnecting the power supply of the encoder while flashing.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.


## Example Output

Run the example, you will see the following output log:

```bash
...
Testing brushed motor with PID...
initializing mcpwm gpio...
Configuring Initial Parameters of mcpwm...

Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
 =================================================================
 |             Example of Motor Control                          |
 |                                                               |
 |  1. Try 'help', check all supported commands                  |
 |  2. Try 'config' to set control period or pwm frequency       |
 |  3. Try 'pid' to configure pid paremeters                     |
 |  4. Try 'expt' to set expectation value and mode              |
 |  5. Try 'motor' to start motor in several seconds or stop it  |
 |                                                               |
 =================================================================

Default configuration are shown as follows.
You can input 'config -s' to check current status.
 -----------------------------------------------------------------
                  Current Configuration Status

 Configuration
       Period = 10 ms	PID = enabled

 PID - Increment
       Kp = 0.800	Ki = 0.000	Kd = 0.100

 Expectation - Triangle
       init = 30.000	max = 50.000	min = -50.000	pace = 1.000

 MCPWM
       Frequency = 1000 Hz

 Motor
       Running seconds = 10
 -----------------------------------------------------------------

 mcpwm-motor>
```

### Check all supported commands and their usages
* Command: `help`

```bash
mcpwm-motor> help
help
  Print the list of registered commands

config  config -s
  Enable or disable PID and set motor control period
  --pid=<y|n>  Enable or disable PID algorithm
  -T, --period=<ms>  Set motor control period
  -s, --show  Show current configurations

expt  expt -i <duty> -m <fixed/tri/rect> -p <double> --max <duty> --min -50<duty>
  Set initial value, limitation and wave mode of expectation. Both dynamic and
  static mode are available
  --max=<duty>  Max limitation for dynamic expectation
  --min=<duty>  Min limitation for dynamic expectation
  -p, --pace=<double>  The increasing pace of expectation every 50ms
  -i, --init=<duty>  Initial expectation. Usually between -100~100
  -m, --mode=<fixed/tri/rect>  Select static or dynamic expectation wave mode. 'fixed' for static, 'tri' for triangle, 'rect' for rectangle

pid  pid -p <double> -i <double> -d <double> -t <loc/inc>
  Set parameters and type for PID algorithm
  -p, --kp=<double>  Set Kp value for PID
  -i, --ki=<double>  Set Ki value for PID
  -d, --kd=<double>  Set Kd value for PID
  -t, --type=<loc/inc>  Select locational PID or incremental PID

motor  motor -u 10
  Start or stop the motor
  -u, --start=<seconds>  Set running seconds for motor, set '0' to keep motor running
  -d, --stop  Stop the motor
```

### Check status

* Command: `config -s`

```bash
 mcpwm-motor> config -s

 -----------------------------------------------------------------
                  Current Configuration Status

 Configuration
       Period = 10 ms	PID = enabled

 PID - Increment
       Kp = 0.800	Ki = 0.000	Kd = 0.100

 Expectation - Triangle
       init = 30.000	max = 50.000	min = -50.000	pace = -1.000

 MCPWM
       Frequency = 1000 Hz

 Motor
       Running seconds = 10
 -----------------------------------------------------------------
```

### Enable or disable PID

* Command: `config --pid <y/n>`
* 'y' - enable PID
* 'n' - disable PID

```bash
mcpwm-motor> config --pid n
config: pid disabled
mcpwm-motor> config --pid y
config: pid enabled
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

### Set expectation parameters

* Command: `expt -i <duty> -m <fixed/tri/rect> -p <double> --max <duty> --min <duty>`
* 'i' - initial duty if you set mode 'fixed'
* 'm' - expectation mode. 'fixed' means the expectation will never change, 'tri' means the expectation will changes with trigonometric wave, 'rect' means the expectation will changes with rectangular wave
* 'p' - the setp size of expectation changed in every 50ms, it can adjust the expectation changing speed
* 'max' - the maximum limitation of expectation
* 'min' - the minimum limitation of expectation

```bash
mcpwm-motor> expt -i 40 -m rect -p 1.5 --max 80 --min -60
expt: init = 40.000
expt: max = 80.000
expt: min = -60.000
expt: pace = 1.500
expt: mode = rectangle
```

### Start or stop motor

* Command: `motor -u <sec>`
* Command: `motor -d`
* 'u' - start the motor in <sec> seconds, if <sec> is 0, the motor won't stop until 'motor -d' is inputed
* 'd' - stop the motor right now

```bash
mcpwm-motor> motor -u 10
motor: motor starts to run in 10 seconds
mcpwm-motor> 1
2
3
4
5
6
7
8
9
10

Time up: motor stoped
```

## Troubleshooting

* Make sure your ESP board and H-bridge module have been connected to the same GND panel.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
