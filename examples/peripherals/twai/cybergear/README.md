| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# CyberGear Motor Control Example

This example demonstrates how to control [XiaoMi CyberGear motors](https://www.mi.com/cyber-gear) using ESP32's TWAI interface. It provides a complete console interface for motor control with support for enable/disable, mode setting, speed and position control, and real-time status monitoring.

> [!NOTE]
> For simplicity, this console example can only control one motor.

## How to use example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* XiaoMi CyberGear motor
* Transceiver (e.g., SN65HVD230)
* Connecting wires

### Hardware Connection

The TWAI bus connection requires a transceiver to convert between the ESP32's TWAI signals and the TWAI bus differential signals.

```
ESP32 Pin      Transceiver     CyberGear Motor
---------      -----------    ---------------
GPIO TX   --->  CTX  
GPIO RX   <---  CRX 
GND       --->  GND             GND       <--- Separate power supply 
VCC(3.3V) --->  VCC(3.3V)       VCC (24V) <--- Separate power supply 
                CAN_H     <-->  CAN_H
                CAN_L     <-->  CAN_L  
---------     ------------     ---------------
```

### Configure the project

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

```bash
idf.py set-target esp32
idf.py menuconfig
```

In the `Example Configuration` menu:

* Set `TWAI TX GPIO number` - GPIO pin for TWAI TX
* Set `TWAI RX GPIO number` - GPIO pin for TWAI RX
* Set `ESP Node ID`  - TWAI node identifier
* Set `Motor ID` - Motor identifier
* Set `Motor bitrate` - Motor bitrate

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py build
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (330) Cybergear: CyberGear node started, TX: GPIO0, RX: GPIO2
I (340) Cybergear: CyberGear console started

Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
I (350) main_task: Returned from app_main()
cybergear>
```

## Console Commands

The example provides an interactive console with the following commands:

### Control Commands

```bash
# Enable motor
enable

# Disable motor and clear fault
disable

# Set control mode (0:Motion, 1:Location, 2:Speed, 3:Current)
mode <mode>

# Motion control
motion -t <torque> -l <location> -s <speed> -p <kp> -d <kd>

# Set motor speed (rad/s)
speed -s <speed>

# Set motor location (rad)  
loc -s <location>

# Set motor current (A)
current -s <current>

# Set current location as zero
zero

# Set motor ID
setid <id>

# Get motor ID
getid

# Show motor status
status

```

### Usage Examples

```bash
cybergear> enable                    # Enable motor
cybergear> mode 1                    # Set location control mode
cybergear> loc -s 1.57               # Set location to π/2 rad
cybergear> status                    # Check motor status
cybergear> mode 2                    # Set speed control mode
cybergear> speed -s 5.0              # Set speed to 5 rad/s
cybergear> disable                   # Disable motor
```

### Note

Command **setid**: The motor ID can not be changed when the motor is running, please disable the motor and set motor id.  
Command **getid**: If you forget the motor ID you set previously, you can run getid cmd first. And then power off the motor and power it on again. Then it will broadcast its motor ID.

### Technical Specifications

- **TWAI Bitrate**: 125 Kbps, 250 Kbps, 500 Kbps or 1 Mbps (default 1 Mbps)
- **Position Range**: ±12.5 rad
- **Speed Range**: ±30.0 rad/s
- **Torque Range**: ±12.0 Nm  
- **Current Range**: 0-23.0 A


### Motor Status Display

The `status` command shows detailed motor information:

```
===== Motor Status =====
Motor ID: 127
Fault Status: 0
State: 2
Location: 0.125000
Speed: 2.000000
Torque: 0.300000
Temperature: 36.500000
========================
```

## Troubleshooting

**If Motor not responding**

   - Check transceiver connections
   - Verify motor ID settings are correct
   - Confirm TWAI bus bitrate matches (1Mbps)
   - Check motor power supply

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
