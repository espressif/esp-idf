| Supported Targets | Linux |
| ----------------- | ----- |

# Simple log test on Linux target

These Catch2 tests run the log component on Linux with a FreeRTOS mock. They cover text formatting and message handlers in V1 and V2, including binary messages, wrapper chains, recursion, and concurrent replacement. Early and DRAM calls, and constrained calls in V2, are checked for bypassing the handler; their target-specific output is not validated on Linux.

## Requirements

* A Linux system
* The usual IDF requirements for Linux system, as described in the [Getting Started Guides](../../../../docs/en/get-started/index.rst).
* The host's gcc/g++

This application has been tested on Ubuntu 20.04 with `gcc` version *9.3.0*.

## Build

First, make sure that the target is set to Linux. Run `idf.py --preview set-target linux` if you are not sure. Then do a normal IDF build: `idf.py build`.

## Run

```bash
idf.py monitor
```

## Example Output

Ideally, all tests pass, which is indicated by "All tests passed" in the last line:

```bash
$ idf.py monitor
===============================================================================
All tests passed (8 assertions in 6 test cases)
```

To run only the message handler tests after building:

```bash
./build/test_log_host.elf "[handler]"
```

`pytest_log_linux.py` runs these tests with the existing log configurations, including V1, V2, binary output, and different timestamp and tag-level settings.
