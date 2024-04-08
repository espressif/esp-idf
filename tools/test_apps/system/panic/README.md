| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Introduction

The panic test app checks the behavior of ESP-IDF Panic Handler.

This test app is relatively complex because it has to check many possible combinations of:
- Failure scenario: abort, assertion, interrupt watchdog, illegal instruction, ...
- Chip target: esp32, esp32c3, ...
- Configuration: default, GDB Stub, Core Dump to UART, ...

Failure scenarios are implemented in [test_panic.c](main/test_panic.c). The test application receives the name of the scenario from console (e.g. `test_illegal_instruction` ). The failure scenario is executed and the app panics. Once the panic output is printed, the pytest-based test case parses the output and verifies that the behavior of the panic handler was correct.

In [pytest_panic.py](pytest_panic.py), there typically is one test function for each failure scenario. Each test function is then parametrized by `config` parameter. This creates "copies" of the test case for each of the configurations (default, GDB Stub, etc.) Tests are also parametrized with target-specific markers. Most tests can run on every target, but there are a few exceptions, such as failure scenarios specific to the dual-core chips.

The test cases use a customized DUT class `PanicTestDut`, defined in [panic_dut.py](test_panic_util/panic_dut.py). This class is derived from [`IdfDut`](https://docs.espressif.com/projects/pytest-embedded/en/latest/references/pytest_embedded_idf/#pytest_embedded_idf.dut.IdfDut). It defines several helper functions to make the test cases easier to read.

# Building
Several configurations are provided as `sdkconfig.ci.XXX` and serve as a template.

For example, to build the test app with configuration `panic` for ESP32-C3, run:
```bash
idf.py set-target esp32c3
cat sdkconfig.defaults sdkconfig.ci.panic > sdkconfig
idf.py build
```

# Building multiple configurations side by side

If you need to work with multiple configurations at the same time it can be useful to keep each build in a separate directory. For example, to build the `panic` configuration for ESP32-C3 in a separate directory, run:
```bash
idf.py -DIDF_TARGET=esp32c3 -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.panic" -DSDKCONFIG=build_esp32c3_panic/sdkconfig -B build_esp32c3_panic build
```

This way, all the build products and the sdkconfig file are kept in the directory `build_esp32c3_gdbstub`. pytest-embedded will search for binaries in this directory if you run tests as shown in the section below.

This approach allows switching between different build configurations and targets without deleting the build directories.

# Running the app manually

```bash
idf.py flash monitor
```
(don't forget the -B argument if you have built the app in a directory other than `build`)

Once the app is running, input the name of the test (e.g. `test_abort`) and press Enter.

# Running tests

Suppose you have built the app for a specific target and with a certain `sdkconfig.ci.CONFIG` config. You need to run the tests just for this config and the target:
```bash
pytest --target TARGET -k '[CONFIG]'
```

For example, if you have built the `panic` config for ESP32-C3, run:
```bash
pytest --target esp32c3 -k '[panic]'
```

Or, to run a single test for the given config, e.g. `test_abort`:
```bash
pytest --target esp32c3 -k 'test_abort[panic]'
```
