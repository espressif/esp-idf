# _RMT Transmit Example_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example will shows how to configure and operate the remote control (RMT) peripheral to transmit a sample message in the [Morse code](https://en.wikipedia.org/wiki/Morse_code), it also shows how to transmit custom format of data.

## How to Use Example

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming
* A LED, a speaker or an earphone

Connection :

```
             330R            LED     
GPIO18 +----/\/\/\----+------|>|-----+ GND
                      |    
                      | /|
                     +-+ |   Speaker
                     | | |     or
                     +-+ |  earphone
                      | \|
                      |
                      +--------------+ GND
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

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

To be able to see and hear the message output by the RMT, connect a LED and a speaker or an earphone (be careful - it may be loud) to the GPIO18(the pin can be changed by modify the definition of `RMT_TX_GPIO` in `main/rmt_tx_main.c`).

Run this example, you will see the following output log:
```
RMT Tx: Transmission complete
RMT Tx: Sample transmission complete
```

## Troubleshooting

* Programming fail

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there is any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
