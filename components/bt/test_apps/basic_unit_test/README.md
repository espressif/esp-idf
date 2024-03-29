| Supported Targets | ESP32 | ESP32-C3 |
| ----------------- | ----- | -------- |

# `bt` component unit tests

When adding new test cases, check if the `depends_components` list in `.build-test-rules.yml` needs to be updated to include additional components. The test app will only be built and tested when these components are modified.

To build and run this test app, using esp32c3 target for example:

```bash
idf.py set-target esp32c3
idf.py build flash monitor
```

To run tests using pytest:

```bash
idf.py set-target esp32c3
idf.py build
pytest --target=esp32c3
```
