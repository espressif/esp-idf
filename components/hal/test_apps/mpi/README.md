| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

## MPI peripheral test

This application contains basic test cases for the MPI peripheral without using any OS functionality or higher abstraction layer.

This contains tests for the following features of MPI peripheral:

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
