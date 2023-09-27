| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# wpa_supplicant unit test

To build and run this test app, using esp32s3 target for example:

```bash
idf.py set-target esp32s3
idf.py build flash monitor
```

To run tests using pytest:

```bash
idf.py set-target esp32s3
idf.py build
pytest --target=esp32s3
```
