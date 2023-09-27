| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# app_trace test

To build and run this test app for app_trace related tests:
```bash
IDF_TARGET=esp32 idf.py @app_trace build flash monitor
```

To build and run this test app for SystemView related tests:
```bash
IDF_TARGET=esp32 idf.py @sysview build flash monitor
```

`@app_trace` and `@sysview` arguments apply additional `idf.py` options, from [app_trace](app_trace) and [sysview](sysview) files.
