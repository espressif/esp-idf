| Supported Targets | ESP32 | ESP32-C3 |
| ----------------- | ----- | -------- |

This is a test app for wear_levelling component. This app is for internal use.
In CI, it is sufficient to run this test for one chip of each architecture.

# Building
Several configurations are provided as `sdkconfig.ci.XXX` and serve as a template.

## Example with configuration "4k" for target ESP32

```bash
rm -rf sdkconfig build
idf.py -DIDF_TARGET=esp32 -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.4k" build
```

# Running

To run locally:

```bash
idf.py flash monitor
```

The tests will be executed and the summary will be printed:

```
-----------------------
4 Tests 0 Failures 0 Ignored 
OK
```

Note, when the Python test script is executed in internal CI, it will test each configuration one by one. When executing this script locally, it will use whichever binary is already built and available in `build` directory.
