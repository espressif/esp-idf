#  _RMT Transmit Example_

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example will shows how to configure and operate the remote control (RMT) peripheral to transmit a sample message in the [Morse code](https://en.wikipedia.org/wiki/Morse_code), it also shows how to transmit custom format of data.

## How to Use Example

### Hardware Required

To run this example, you need to prepare a development board with esp32 SoC(e.g., ESP32_Core_board, ESP-WORVER_KIT etc), a LED, a speaker or an earphone and a USB cable for power supply and programming.

### Configure the Project

```
make menuconfig
```

* Set serial port under Serial Flasher Options.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

To be able to see and hear the message output by the RMT, connect a LED and a speaker or an earphone (be careful - it may be loud) to the GPIO18(the pin can be changed by modify the definition of `RMT_TX_GPIO` in `main/rmt_tx_main.c`).


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

Run this example, you will see the following output log:
```
RMT Tx: Transmission complete
RMT Tx: Sample transmission complete
```

## Troubleshooting

* Programming fail

    * To check whether the hardware connection is correct, you can run `make monitor` and reboot your board to see if there are any output logs.
    * Your download baud-rate is too high. please lower your download baud-rate in menuconfig and try again.

If any problems that can't be resolved, please open an issue in GitHub(https://github.com/espressif/esp-idf/issues), we will reply to you soon.