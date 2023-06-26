| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 | Linux |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | ----- |

# Hello World Example Compatible with POSIX-port

This is a version of the "Hello World" example compatible with the linux target. Just by using `idf.py (--preview) set-target <target>`, it can be compiled for chip targets as well as for the [FreeRTOS POSIX/Linux simulator](https://www.freertos.org/FreeRTOS-simulator-for-Linux.html), i.e., for running it on Linux. The applications can then be run on the chosen target.

## Requirements

If you want to use this example on Linux, you need a Linux machine as host. The remaining requirements are the same requirements as for [Unit Testing on Linux (using cmock)](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/linux-host-testing.html#requirements), except you do not need Ruby.

## How to use example

### Configure the project

No special configuration is required, we also do not recommend changing configuration when compiled for the POSIX/Linux simulator as this is still in preview. If you have to configure something, use the usual IDF menuconfig:
```
idf.py menuconfig
```

### Build and Flash

You can compile this example for chip targets, e.g. ESP32 and then run it by using:
```
idf.py set-target esp32
idf.py build
idf.py -p <port> flash monitor
```

If you want to build this example for the linux target and run it, use the same commands except setting the linux target and omitting the flash command:
```
idf.py --preview set-target linux
idf.by build
idf.py monitor
```
The linux target is still in preview, hence the necessary `--preview` argument. Flashing can be omitted on Linux.


## Example folder contents

The files in this project have the same structure as the files in the [original Hello World application](../../../../examples/get-started/hello_world/).

## Example Output

The output is similar to the output of the [original Hello World application](../../../../examples/get-started/hello_world/), except that no chip information is printed and there won't be any bootloader output on the linux target.
