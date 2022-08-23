| Supported Targets | Linux |
| ----------------- | ----- |

# Description

This directory contains test code for the mqtt client that runs on host.

Tests are written using [Catch2](https://github.com/catchorg/Catch2) test framework 

# Build

Tests build regularly like an idf project. 

```
idf.py build
```

# Run

The build produces an executable in the build folder. 

Just run:

```
./build/host_mqtt_client_test.elf
```

The test executable have some options provided by the test framework. 


