# Example: ESPTimer C++ class

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of the ESPTimer c++ class in ESP-IDF.

In this example, the `sdkconfig.defaults` file sets the `CONFIG_COMPILER_CXX_EXCEPTIONS` option. 
This enables both compile time support (`-fexceptions` compiler flag) and run-time support for C++ exception handling.
This is necessary for the C++ APIs.

## How to use example

### Hardware Required

Any ESP32 family development board.

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
Setting up timer to trigger in 500ms
timeout
Setting up timer to periodically every 200ms
periodic timeout
periodic timeout
periodic timeout
periodic timeout
periodic timeout

```

