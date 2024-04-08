| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Example: C++ exception handling

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of C++ exceptions in ESP-IDF.

By default, C++ exceptions support is disabled in ESP-IDF. It can be enabled using `CONFIG_COMPILER_CXX_EXCEPTIONS` configuration option.

In this example, the `sdkconfig.defaults` file sets the `CONFIG_COMPILER_CXX_EXCEPTIONS` option. This enables both compile time support (`-fexceptions` compiler flag) and run-time support for C++ exception handling.

The example source code declares a class which can throw exception from the constructor if the argument provided is equal to `0`. This is used to demonstrate that exceptions can be thrown and caught using standard C++ facilities.

**Note: Due to the use of the C++ exceptions, this example is written in C++ instead of C.**

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

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
app_main starting
In constructor, arg=42
In constructor, arg=0
In destructor, m_arg=42
Exception caught: Exception in constructor
app_main done
```

