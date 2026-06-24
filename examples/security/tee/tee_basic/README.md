| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# Basic TEE example

## Overview

- This example illustrates the ESP-TEE (Trusted Execution Environment) framework to encrypt/decrypt data using AES within a secure environment.
- The non-secure world i.e. the Rich Execution Environment (REE) raises a request for AES operation in TEE through the secure service call interface. The TEE performs encrypts/decrypts the given buffer with the AES-256-GCM mode using the key protected by TEE. If the operation is successful, the result of the AES operation is returned in the output buffer provided in the secure service call by the REE.
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
I (353) main_task: Calling app_main()
I (353) example_tee_basic: AES-256-GCM operations in TEE
I (364) example_tee_service: Secure service call: PROTECTED M-mode
I (370) example_tee_service: AES-256-GCM encryption
I (373) ciphertext: 23 32 a6 1f ff 27 15 d7 35 70 db f5 e3 0c 13 41
I (373) ciphertext: eb 80 d7 2c 9c f5 68 5a b4 2c 43 b0 64 68 13 36
I (383) tag: f8 85 ab 3b 47 a6 65 0d 0a 42 bd 3d
I (391) example_tee_service: Secure service call: PROTECTED M-mode
I (397) example_tee_service: AES-256-GCM decryption
I (393) example_tee_basic: AES-GCM decryption successful!
I (403) main_task: Returned from app_main()
```
