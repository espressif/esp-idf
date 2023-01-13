| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

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

## Run test
Open two terminals (1) and (2)
1) Start the test server which would pass packets from TTCN3 test suite into ESP32 board in `$IDF_PATH/components/lwip/weekend_test`
```
python net_suite_test.py
```

2) Start test suite in TTCN3 environment in `src` subdir of the cloned repository `net-test-suites.git`
```
ttcn3_start test_suite esp32_netsuite.cfg
```

## Internal connection

Purpose of this test is to execute standard network suite on a ESP32 network stack.

DUT, Device (Network stack in this case) under test, runs normally on target, but a specific interface with configured esp-netif for passing arbitrary data to
and from the network stack. Embedded code `net_suite.c` implements an application which serves stdin/stdout and propagates the data to/from this test interface.

Standard Intel net suite executed by TTCN3 engine uses udp ports for input/ouput of network packets. Python script `net_suite.py` translates this communication
from/to those udp ports to stdin/stdout, where after propagating over USB/UART to the ESP32 board are processed in the network stack (on the target).

Actual test execution, progress, evaluation and test reporting is done using standard net-test-suite scripts running on PC.

```
  PC
+---------------------------------------------------------+       ESP32 board
|                                                         |    +----------------------------------------+
|  TTCN3 engine                                           |    |  +----------------------------------+  |
|                                                         |    |  | net_suite.c                      |  |
| +-----------------+            +--------------+         |    |  |         +------------------------+  |
| | net-test-suite  |--7777/udp--| net_suite.py |--stdout---------| ----->  | esp_netif / lwip       |  |
| |                 |--7771/udp--|              |--stdin----------| <-----  |                        |  |
| +-----------------+            +--------------+         |    |  +---------+------------------------+  |
+---------------------------------------------------------+    +----------------------------------------+
```
