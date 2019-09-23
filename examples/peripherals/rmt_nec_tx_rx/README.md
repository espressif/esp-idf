# _RMT NEC_TX_RX Example_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example uses the remote control (RMT) peripheral to transmit and receive codes for the NEC infrared remote protocol.

## How to Use Example

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

By default, this example runs a self test which assumes the TX pin (GPIO18) and RX pin (GPIO19) are _connected together_. 

To disable self-test mode, comment out RMT_RX_SELF_TEST in infrared_nec_main.c, after which, you need to connect a IR transmitter and a receiver to GPIO18 and GPIO19. 

The TX pin and RX pin can be modified in top of the main/infrared_nec_main.c file.

```
#define RMT_TX_GPIO_NUM  18     /*!< GPIO number for transmitter signal */
#define RMT_RX_GPIO_NUM  19     /*!< GPIO number for receiver */
```

### Configure the Project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Run this example, you will see the following output log:
```
NEC: RMT TX DATA
NEC: RMT RCV --- addr: 0xee11 cmd: 0xff00
NEC: RMT RCV --- addr: 0xed12 cmd: 0xfe01
NEC: RMT RCV --- addr: 0xec13 cmd: 0xfd02
NEC: RMT RCV --- addr: 0xeb14 cmd: 0xfc03
NEC: RMT RCV --- addr: 0xea15 cmd: 0xfb04
NEC: RMT RCV --- addr: 0xe916 cmd: 0xfa05
NEC: RMT RCV --- addr: 0xe817 cmd: 0xf906
NEC: RMT RCV --- addr: 0xe718 cmd: 0xf807
NEC: RMT RCV --- addr: 0xe619 cmd: 0xf708
NEC: RMT RCV --- addr: 0xe51a cmd: 0xf609
NEC: RMT RCV --- addr: 0xe41b cmd: 0xf50a
NEC: RMT RCV --- addr: 0xe31c cmd: 0xf40b
NEC: RMT RCV --- addr: 0xe21d cmd: 0xf30c
NEC: RMT RCV --- addr: 0xe11e cmd: 0xf20d
NEC: RMT RCV --- addr: 0xe01f cmd: 0xf10e
NEC: RMT RCV --- addr: 0xdf20 cmd: 0xf00f
NEC: RMT RCV --- addr: 0xde21 cmd: 0xef10
NEC: RMT RCV --- addr: 0xdd22 cmd: 0xee11
NEC: RMT RCV --- addr: 0xdc23 cmd: 0xed12
NEC: RMT RCV --- addr: 0xdb24 cmd: 0xec13
NEC: RMT RCV --- addr: 0xda25 cmd: 0xeb14
```

## Troubleshooting

* Programming fail

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there is any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
