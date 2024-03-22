| Supported Targets | ESP32 | ESP32-C3 | Linux |
| ----------------- | ----- | -------- | ----- |

Note: Most of the test cases shouldn't be run manually, but [pytest](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/contribute/esp-idf-tests-with-pytest.html) should be used instead. E.g., to run all test cases on ESP32 using pytest, use:

```
pytest --target esp32 -m generic
```
