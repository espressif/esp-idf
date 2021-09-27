| Supported Targets | Linux |
| ----------------- | ----- |

# C++ ESPTimer test on Linux target

This unit test tests basic functionality of the `ESPTimer` class. The test does not use mocks. Instead, it runs the whole implementation of the component on the Linux host. The test framework is CATCH.

## Requirements

* A Linux system
* The usual IDF requirements for Linux system, as described in the [Getting Started Guides](../../../../../../docs/en/get-started/index.rst).
* The host's gcc/g++

This application has been tested on Ubuntu 20.04 with `gcc` version *9.3.0*.

## Build

First, make sure that the target is set to Linux. Run `idf.py --preview set-target linux` if you are not sure. Then do a normal IDF build: `idf.py build`.

## Run

IDF monitor doesn't work yet for Linux. You have to run the app manually:

```bash
build/test_esp_timer_cxx_host.elf
```

## Example Output

Ideally, all tests pass, which is indicated by "All tests passed" in the last line:

```bash
$ build/test_esp_timer_cxx_host.elf
===============================================================================
All tests passed (9 assertions in 11 test cases)
```
