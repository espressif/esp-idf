| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# TEE: Secure Storage example

## Overview

- This example demonstrates the ESP-TEE framework's Secure Storage with two workflows:
  - Signing and verification
    - Create and securely store an ECDSA `secp256r1` keypair in a protected memory space i.e. the secure storage partition
    - Sign a message in the TEE (given its digest) using the ECDSA keypair stored with the given ID
    - Retrieve the ECDSA public key associated with the private key with the given ID
    - Verify the generated signature in the REE
  - Encryption and decryption
    - Generate and securely store an AES-256 key in the secure storage partition
    - Encrypt a message in the TEE using the AES key stored with the given ID with the `aes-256-gcm` algorithm and generate an authentication tag
    - Decrypt the ciphertext using the same AES key and validate the authentication tag
    - Verify that the decrypted message matches the original

## How to use the example

### Hardware Required

This example can be executed on any development board with a Espressif SOC chip supporting the TEE framework (see Supported Targets table above).

### Configure the project

Before the project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

Open the project configuration menu (`idf.py menuconfig`).

- Configure the secure storage example key ID at `Example Configuration → TEE: Secure Storage Key ID`.

TEE Secure Storage follows the NVS partition format and uses an AES-XTS encryption scheme derived via the HMAC peripheral. It supports two key derivation modes, configurable via `CONFIG_SECURE_TEE_SEC_STG_MODE`:

  - **Development** Mode: Encryption keys are embedded in the ESP-TEE firmware (identical across all instances).
  - **Release** Mode: Encryption keys are derived via the HMAC peripheral using a key stored in eFuse, specified by `CONFIG_SECURE_TEE_SEC_STG_EFUSE_HMAC_KEY_ID`.

#### Configure the eFuse key ID storing the HMAC key

- Navigate to `ESP-TEE (Trusted Execution Environment) → Secure Services → Secure Storage: Mode` and enable the `Release` mode configuration.
- Set the eFuse key ID storing the HMAC key at `ESP-TEE (Trusted Execution Environment) → Secure Services → Secure Storage: eFuse HMAC key ID for storage encryption keys`.

**Note:** Before running the example, users must program the HMAC key into the configured eFuse block - refer to the snippet below. The TEE checks whether the specified eFuse block is empty or already programmed with a key. If the block is empty, an error will be returned; otherwise, the pre-programmed key will be used.

```shell
# Generate a random 32-byte HMAC key
openssl rand -out hmac_key_file.bin 32
# Programming the HMAC key (256-bit) in eFuse
# Here, BLOCK_KEYx is a free eFuse key-block between BLOCK_KEY0 and BLOCK_KEY5
espefuse.py -p PORT burn_key BLOCK_KEYx hmac_key_file.bin HMAC_UP
```

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Example Output

```log
I (321) main_task: Calling app_main()
I (321) example_tee_sec_stg: TEE Secure Storage
I (331) example_tee_sec_stg: Message-to-be-signed: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
I (961) Signature: 21 68 98 44 68 04 f2 39 70 f6 dc 79 73 5e 88 80
I (961) Signature: 39 e8 88 4b 41 ea 6f 3c 63 6f 89 08 bf 8d b7 29
I (961) Signature: 97 2b 21 0e 6a 35 a9 86 ee b9 e5 97 e4 b5 3f 76
I (971) Signature: d8 b6 bc d0 7a a6 f9 76 ad 9f e4 6b 3c 50 be 5b
I (1001) example_tee_sec_stg: Signature verified successfully!
I (1001) Plaintext: 4c 6f 72 65 6d 20 69 70 73 75 6d 20 64 6f 6c 6f
I (1001) Plaintext: 72 20 73 69 74 20 61 6d 65 74 2c 20 63 6f 6e 73
I (1011) Plaintext: 65 63 74 65 74 75 72 20 61 64 69 70 69 73 63 69
I (1021) Plaintext: 6e 67 20 65 6c 69 74 2e
I (1111) Encrypted data: 8a c5 1b cf 9f 5a 7c 73 a6 e7 5a bf 2a 25 bf e2
I (1111) Encrypted data: 80 8a 2e 3e 7a 26 9a 14 d0 c3 1b c3 f9 48 40 cc
I (1111) Encrypted data: ea 0b 61 c6 45 8c 39 b6 66 8c 28 5a 10 f3 66 b6
I (1121) Encrypted data: ce 7d 94 7a d8 ba 18 2a
I (1131) IV: b9 3f 51 52 f4 0b 32 71 7e 45 56 33
I (1131) Tag: e8 78 6f 56 0a 14 95 cb b2 c3 ae 14 3c 7c 26 5d
I (1131) example_tee_sec_stg: Done with encryption/decryption!
I (1141) main_task: Returned from app_main()
```
