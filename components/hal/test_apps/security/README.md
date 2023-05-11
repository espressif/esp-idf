| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

## Security peripherals test

This is a combined security peripherals verification application using mostly HAL APIs. This application is intentionally kept simple and does not use any higher layer constructs. This application can help in the early verification of the new SoC.

This contains tests for the following features of the security peripherals:

- MPI peripheral
    - MPI Modular Multiplication
    - MPI Multiplication
    - MPI Modular Exponentiation

- ECC peripheral
    - ECC Point multiplication for P192 and P256 curve
    - ECC Point verification for P192 and P256 curve
    - ECC Point verify and multiply for P192 and P256 curve
    - ECC Inverse multiplication for P192 and P256

    If the hardware supports extended work modes then it also tests:
    - ECC Jacobian multiplication for P192 and P256 curve
    - ECC Jacobian verification for P192 and P256 curve
    - ECC Point verification and Jacobian multiplication for P192 and P256 curve
    - ECC Point addition for P192 and P256 curve
    - Mod addition
    - Mod subtraction
    - Mod multiplication

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
