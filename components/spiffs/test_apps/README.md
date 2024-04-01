| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

This is a test app for spiffs component.

# Building
Several configurations are provided as `sdkconfig.ci.XXX` and serve as a template.

## Example with configuration "release" for target ESP32

```bash
rm -rf sdkconfig build
idf.py -DIDF_TARGET=esp32 -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.release" build
```

# Running

To run locally:

```bash
idf.py flash monitor
```

The tests will be executed and the summary will be printed:

```
-----------------------
21 Tests 0 Failures 0 Ignored
OK
```

Note, when the Python test script is executed in internal CI, it will test each configuration one by one. When executing this script locally, it will use whichever binary is already built and available in `build` directory.
