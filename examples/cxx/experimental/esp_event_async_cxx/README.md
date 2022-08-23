| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# ESP-Event asynchronous example

(See the README.md file in the upper level 'examples' directory for more information about examples.)


## How to use example

### Configure the project

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

The object is created twice, hence the started Eventloop and finished destruction lines appear twice.
```
NORMAL TESTING...
received event: test/0; data: 47
received event: test/1

TIMEOUT TESTING...
received event: test/0

TIMEOUT for event: test/0
I (10419) ESP Event C++ Async: Finished example
```
