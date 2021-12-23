| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# USB CDC-ACM Host Driver BG96 Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up ESP chip to interface with CDC-like device by using the CDC-ACM Host Driver. CDC-like devices implement a Vendor-specific class, and support a subset of the functions of a fully compliant CDC-ACM device.

## How to use example

### Hardware Required

Any ESP board with USB-OTG supported and a Quectel BG96 LTE/GPS modem.

Connect USB_D+, USB_D-, GND and +5V signals of ESP board to BG96.

_Note:_ Quectel BG96 modem must be started after power-up by applying low pulse on PWRKEY (pin 15).

#### Pin Assignment

See common pin assignments for USB Device examples from [upper level](../../../README.md#common-pin-assignments).

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see the output at idf monitor:

```
I (276) BG96: USB Host installed
I (24446) AT: ATE0
I (24446) AT: 
OK

I (24526) AT: 
+QIND: SMS DONE

I (24646) AT: 
APP RDY

I (25446) BG96: Sending AT
I (25446) AT: 
OK

I (26446) BG96: Enabling GNSS
I (26446) AT: 
OK

GPVTG Sentence:
  Track [deg]:   0.00
  Speed [kmph]:  0.00
  Speed [knots]: 0.00
GPGSA Sentence:
  Mode: A
  Fix:  1
  PDOP: 0.00
  HDOP: 0.00
  VDOP: 0.00
GPGGA sentence
Number of satellites: 0
Altitude: 0.000000
GPRMC sentence
Longitude:
  Degrees: 0
  Minutes: 0.000000
  Cardinal:
Latitude:
  Degrees: 0
  Minutes: 0.000000
  Cardinal: 
Date & Time: 00 Jan 00:00:00 1900
Speed, in Knots: 0.000000
Track, in degrees: 0.000000
Magnetic Variation:
  Degrees: 0.000000
  Cardinal: 
Invalid Magnetic Variation Direction!
Adjusted Track (heading): 0.000000
I (27446) BG96: Sending AT+GSN
I (27446) AT: 
860517045660414

OK
...

```
