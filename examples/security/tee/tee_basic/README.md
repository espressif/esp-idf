| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# Basic TEE example

## Overview

- This example illustrates the ESP-TEE (Trusted Execution Environment) framework to encrypt/decrypt data using AES within a secure environment.
- The non-secure world i.e. the Rich Execution Environment (REE) raises a request for AES operation in TEE through the secure service call interface. The TEE performs encrypts/decrypts the given buffer with the AES-256-CBC mode using the key protected by TEE. If the operation is successful, the result of the AES operation is returned in the output buffer provided in the secure service call by the REE.
- This example also demonstrates how to add custom service calls to TEE. You can refer to `components/example_service` for more information - see the structure below.

```
└── example_secure_service       # Component parent directory
    ├── CMakeLists.txt
    ├── example_service.c        # Custom secure service APIs
    ├── sec_srv_tbl_example.yml  # Custom secure service table, which is parsed alongwith the default one provided by TEE
    ├── include
    │   └── example_service.h
    └── tee_project.cmake        # To be manually included in the project's top level CMakeLists.txt before project(...)
                                 # Processes the custom service table
```

## How to use the example

### Hardware Required

This example can be executed on any development board with a Espressif SOC chip supporting the TEE framework (see Supported Targets table above).

### Build and Flash

Before building the example, be sure to set the correct chip target using idf.py set-target <chip_name>.

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Example Output

```log
I (315) main_task: Calling app_main()
I (315) example_tee_basic: AES-256-CBC operations in TEE
TEE: Secure service call for AES-256-CBC operation
TEE: In PROTECTED M-mode
I (325) example_tee_basic: AES encryption successful!
I (325) example_tee_basic: Cipher text -
I (325) example_tee_basic: ee 04 9b ee 95 6f 25 04 1e 8c e4 4e 8e 4e 7a d3
TEE: Secure service call for AES-256-CBC operation
TEE: In PROTECTED M-mode
I (345) example_tee_basic: AES decryption successful!
I (345) main_task: Returned from app_main()
```
