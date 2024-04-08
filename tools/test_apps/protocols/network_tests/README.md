| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Intel net test suite for LwIP network stack

This project provides a test interface to esp32 network stack in order to execute standard set of
Intel network test suite defined in TTCN3 framework.

## Important notice
*This is an internal ESP-IDF test and not a user project example*

##  Execute  net test suite

These network tests could be executed in both manual or automated mode in CI.

Note: TTCN3 engine works reliably only on Linux and Windows.

## Setup TTCN3

* Clone a repository https://github.com/intel/net-test-suites.git and install titan core as described in the README.md
* Copy files `esp32_netsuite.cfg` and `esp32_netsuite.ttcn` (located in `$IDF_PATH/components/lwip/weekend_test`) to `src` subdir of the cloned repository `net-test-suites`
* Rebuild the netsuite tests (according to README.md in net-test-suite) by executing `source make.sh` in `src` subdir


## Build application

```
cd $IDF_PATH/examples/system/network_tests
idf.py build
idf.py -p PORT flash
```
