| Supported Targets | Linux |
| ----------------- | ----- |

# Simple test application for [SMP Linux](TODO) port

Note that the IDF version of the FreeRTOS POSIX/Linux simulator is not tested here, that one is merely the upstream version.

## Build

```
idf.py --preview set-target linux
```

Amazon FReeRTOS SMP configuration is already set via `sdkconfig.defaults`, no need to configure.

```
idf.py build
```

## Run

```
idf.py monitor
```

After the test output, input: `![ignore]` to not run the ignored test
