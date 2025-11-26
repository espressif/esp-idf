| Supported Targets | ESP32 | ESP32-P4 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

This is a test app for sdmmc component. This app is for internal use.

# Building

```bash
idf.py build
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
