| Supported Targets | ESP32-C2 | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- | -------- |

## ECC peripheral test

This application contains basic test cases for the ECC peripheral without using any OS functionality or higher abstraction layer.

This contains tests for the following features of ECC peripheral:

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
