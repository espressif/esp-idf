| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

## Crypto peripherals test

This is a combined crypto peripherals verification application using mostly HAL APIs. This application is intentionally kept simple and does not use any higher layer constructs. This application can help in the early verification of the new SoC.

This contains tests for the following features of the crypto peripherals:

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

- HMAC periperhal
    - HMAC 'downstream' JTAG Enable mode
    - HMAC 'downstream' JTAG Disable
    - HMAC 'upstream' MAC generation with zeroes
    - HMAC 'upstream' MAC generation from data

- DS peripheral
    - Digital Signature Parameter Encryption
    - Digital Signature wrong HMAC key purpose
    - Digital Signature Blocking wrong HMAC key purpose
    - Digital Signature Operation
    - Digital Signature Blocking Operation
    - Digital Signature Invalid Data

- ECDSA peripheral
    - ECDSA P192 signature generation
    - ECDSA P256 signature generation
    - ECDSA P192 signature verification
    - ECDSA P256 signature verification
    - ECDSA P192 export public key
    - ECDSA P256 export public key
    - ECDSA P192 deterministic signature generation
    - ECDSA P256 deterministic signature generation

- AES peripheral
    - Block Mode
        - CBC AES-256
        - CTR AES-256

- SHA peripheral
    - Block Mode
        - SHA-1
        - SHA-224
        - SHA-256
        - SHA-384
        - SHA-512
        - SHA-512/224
        - SHA-512/256
        - SHA-512/t

> **_NOTE:_** The verification tests for the HMAC and Digital Signature peripherals would get exercised only by enabling the example config in an FPGA environment.

# Burning the HMAC key

The HMAC tests need an HMAC key to be burned in the `BLOCK_KEY3` and `BLOCK_KEY4` of the efuses. As this verification application is independent of the efuse component, the user needs to manually burn the keys and their key purposes using `espefuse.py`.

```bash
espefuse.py -p $ESPPORT burn_key BLOCK_KEY3 main/hmac/hmac_key.bin HMAC_DOWN_JTAG

espefuse.py -p $ESPPORT burn_key BLOCK_KEY4 main/hmac/hmac_key.bin HMAC_UP
```

# Burning the HMAC keys for Digital Signature tests

The tests needs some HMAC keys to be burned in the `BLOCK_KEY0`, `BLOCK_KEY1` and `BLOCK_KEY2` of the efuses. As this verification application is independent of the efuse component, the user needs to manually burn the keys and their key purposes using `espefuse.py`.

If SOC_DS_SIGNATURE_MAX_BIT_LEN == 3072:
```bash
espefuse.py -p $ESPPORT burn_key BLOCK_KEY0 main/ds/keys/3072/ds_key1.bin HMAC_DOWN_DIGITAL_SIGNATURE

espefuse.py -p $ESPPORT burn_key BLOCK_KEY1 main/ds/keys/3072/ds_key2.bin HMAC_DOWN_DIGITAL_SIGNATURE

espefuse.py -p $ESPPORT burn_key BLOCK_KEY2 main/ds/keys/3072/ds_key3.bin HMAC_DOWN_DIGITAL_SIGNATURE
```

If SOC_DS_SIGNATURE_MAX_BIT_LEN == 4096:
```bash
espefuse.py -p $ESPPORT burn_key BLOCK_KEY0 main/ds/keys/4096/ds_key1.bin HMAC_DOWN_DIGITAL_SIGNATURE

espefuse.py -p $ESPPORT burn_key BLOCK_KEY1 main/ds/keys/4096/ds_key2.bin HMAC_DOWN_DIGITAL_SIGNATURE

espefuse.py -p $ESPPORT burn_key BLOCK_KEY2 main/ds/keys/4096/ds_key3.bin HMAC_DOWN_DIGITAL_SIGNATURE
```

# Burning the ECDSA keys

By default, ECDSA tests are disabled. You can enable it after disabling HMAC tests using `idf.py menuconfig -> Test App Configuration -> Enable ECDSA Peripheral test cases`

The ECDSA tests need some ECDSA keys to be burned in the `BLOCK_KEY4` and `BLOCK_KEY5` of the efuses. As this verification application is independent of the efuse component, the user needs to manually burn the keys and their key purposes using `espefuse.py`.

```bash
espefuse.py -p $ESPPORT burn_key BLOCK_KEY4 main/ecdsa/ecdsa192_priv_key.pem ECDSA_KEY

espefuse.py -p $ESPPORT burn_key BLOCK_KEY5 main/ecdsa/ecdsa256_priv_key.pem ECDSA_KEY
```

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
