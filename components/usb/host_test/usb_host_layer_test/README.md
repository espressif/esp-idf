| Supported Targets | Linux |
| ----------------- | ----- |

# Description

This directory contains test code for `USB Host layer` of USB Host stack. Namely:
* USB Host public API calls to install and uninstall the USB Host driver with partially mocked USB Host stack to test Linux build and Cmock run for this partial Mock
* Mocked are all layers of the USB Host stack below the USB Host layer, which is used as a real component

Tests are written using [Catch2](https://github.com/catchorg/Catch2) test framework, use CMock, so you must install Ruby on your machine to run them.

This test directory uses freertos as real component

# Build

Tests build regularly like an idf project. Currently only working on Linux machines.

```
idf.py --preview set-target linux
idf.py build
```

# Run

The build produces an executable in the build folder.

Just run:

```
idf.py monitor
```

or run the executable directly:

```
./build/host_test_usb_host_layer.elf
```