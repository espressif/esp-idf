| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

## Security peripherals test

This is a combined security peripherals verification application using mostly HAL APIs. This application is intentionally kept simple and does not use any higher layer constructs. This application can help in the early verification of the new SoC.

This contains tests for the following features of the security peripherals:

- MPI peripheral
    - MPI Modular Multiplication
    - MPI Multiplication
    - MPI Modular Exponentiation

# Building

```bash
idf.py set-target <TARGET>
idf.py build
```

# Running the app manually

```bash
idf.py flash monitor
```

Enter the test that you want to run locally

# Running tests

```bash
pytest --target <TARGET>
```
