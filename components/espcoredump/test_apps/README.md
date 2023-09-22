| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 |
| ----------------- | ----- | -------- | -------- |

# Coredump unit tests

The unit tests are currently run only on the chips listed above just to save CI resources. If you are adding some tests which need to run on a different chip, update [.build-test-rules.yml](../.build-test-rules.yml), adding the chip you need.

When adding new test cases, check if the `depends_components` list in `.build-test-rules.yml` needs to be updated to include additional components. The test app will only be built and tested when these components are modified.

See also the [panic test app](../../../tools/test_apps/system/panic) which serves as an integration test for espcoredump and is run on all supported chips.

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
