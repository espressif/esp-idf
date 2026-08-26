| Supported Targets | Linux |
| ----------------- | ----- |

# Simple log test on Linux target

This unit test tests basic functionality of the log component. The test does not use mocks. Instead, it runs the whole implementation of the component on the Linux host. The test framework is CATCH. For early log, we only perform a compile time test since there's nothing to test on Linux except for the log macros themselves (all the implementation will be in chip ROM).

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
