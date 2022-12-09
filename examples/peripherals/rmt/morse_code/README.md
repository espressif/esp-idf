# RMT Transmit Example -- Morse Code

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example mainly illustrates how to transmit the [Morse code](https://en.wikipedia.org/wiki/Morse_code) using the RMT driver.

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

Open the project configuration menu (`idf.py menuconfig`). 

In the `Example Connection Configuration` menu:

* Set the GPIO number used for transmitting the IR signal under `RMT TX GPIO` optin.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.


## Example Output

To be able to see and hear the message output by the RMT, connect an LED and a speaker or an earphone (be careful it might make a large noise) to the GPIO you set in the menuconfig.

Run the example, you will see the following output log:

``` bash
...
I (304) example: Configuring transmitter
I (2814) example: Transmission complete
...
```

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
