# Example: Blink C++ example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of the `GPIO_Output` C++ class in ESP-IDF.

In this example, the `sdkconfig.defaults` file sets the `CONFIG_COMPILER_CXX_EXCEPTIONS` option. 
This enables both compile time support (`-fexceptions` compiler flag) and run-time support for C++ exception handling.
This is necessary for the C++ APIs.

## How to use example

### Hardware Required

Any ESP32 family development board.

Connect an LED to the corresponding pin (default is pin 4). If the board has a normal LED already, you can use the pin number to which that one is connected.

Development boards with an RGB LED that only has one data line like the ESP32-C3-DevKitC-02 and ESP32-C3-DevKitM-1 will not work. In this case, please connect an external normal LED to the chosen pin.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
...
I (339) cpu_start: Starting scheduler.
I (343) gpio: GPIO[4]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
LED ON
LED OFF
LED ON
LED OFF
LED ON
LED OFF
LED ON
LED OFF
LED ON
LED OFF

```

