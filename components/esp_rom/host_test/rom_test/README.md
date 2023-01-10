| Supported Targets | Linux |
| ----------------- | ----- |

# Simplest rom test on Linux target

This unit test tests only if some of the supplied Linux functions seem to work correctly. The test framework is CATCH.

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
test
===============================================================================
All tests passed (8 assertions in 6 test cases)
```
