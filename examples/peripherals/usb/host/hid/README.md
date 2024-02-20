| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# USB HID Class example
This example implements a basic USB Host HID Class Driver, and demonstrates how to use the driver to communicate with USB HID devices (such as Keyboard and Mouse or both) on the ESP32-S2/S3. Currently, the example only supports the HID boot protocol which should be present on most USB Mouse and Keyboards. The example will continuously scan for the connection of any HID Mouse or Keyboard, and attempt to fetch HID reports from those devices once connected. To quit the example (and shut down the HID driver), users can GPIO0 to low (i.e., pressing the "Boot" button on most ESP dev kits).


### Hardware Required
* Development board with USB capable ESP SoC (ESP32-S2/ESP32-S3)
* A USB cable for Power supply and programming
* USB OTG Cable

### Common Pin Assignments

If your board doesn't have a USB A connector connected to the dedicated GPIOs, 
you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

```
ESP BOARD    USB CONNECTOR (type A)
                   --
                  | || VCC
[GPIO19]  ------> | || D-
[GPIO20]  ------> | || D+
                  | || GND
                   --
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

The example serial output will be the following:

```
I (198) example: HID HOST example
I (598) example: Interface number 0, protocol Mouse
I (598) example: Interface number 1, protocol Keyboard

Mouse
X: 000883       Y: 000058       |o| |
Keyboard
qwertyuiop[]\asdfghjkl;'zxcvbnm,./
Mouse
X: 000883       Y: 000058       | |o|
```

Where every keyboard key printed as char symbol if it is possible and a Hex value for any other key. 

#### Keyboard input data
Keyboard input data starts with the word "Keyboard" and every pressed key is printed to the serial debug.
Left or right Shift modifier is also supported. 

```
Keyboard
Hello, ESP32 USB HID Keyboard is here!
```

#### Mouse input data 
Mouse input data starts with the word "Mouse" and has the following structure. 
```
Mouse
X: -00343   Y: 000183   | |o|
     |            |      | |
     |            |      | +- Right mouse button pressed status ("o" - pressed, " " - not pressed)
     |            |      +--- Left mouse button pressed status ("o" - pressed, " " - not pressed)
     |            +---------- Y relative coordinate of the cursor 
     +----------------------- X relative coordinate of the cursor 
```
