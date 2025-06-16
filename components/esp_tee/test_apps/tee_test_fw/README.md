| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

## ESP-TEE: Test Suite

- ESP-TEE utilizes the `pytest` framework in ESP-IDF for executing the dedicated unit tests on the target. The test application includes cases spanning the following modules -
    - Secure service call interface
    - Interrupts and exception handling
    - Privilege violation
    - Cryptographic operations
    - TEE OTA updates
    - Secure storage
    - Attestation

- For executing the test locally, ESP-IDF needs to be installed with the additional Python requirements.

```bash
cd $IDF_PATH
bash install.sh --enable-ci --enable-pytest
. ./export.sh
```

- For example, to execute the TEE test suite for ESP32-C6 with all the available `sdkconfig` files, run the following steps. The required test applications will be built and flashed automatically onto the DUT by the `pytest` framework.

```bash
python $IDF_PATH/tools/ci/ci_build_apps.py . --target esp32c6 -v --pytest-apps
pytest --target esp32c6
```
