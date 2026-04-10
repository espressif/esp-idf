| Supported Targets | ESP32-C5 | ESP32-P4 |
| ----------------- | -------- | -------- |

# Key Manager Signing Example

## Overview

This example demonstrates how to use the Key Manager peripheral for cryptographic signing operations on Key Manager supported chips. The Key Manager provides secure key deployment and management, allowing keys to be used for cryptographic operations without exposing the raw key material.

The example showcases two signing use cases:

1. **ECDSA Signing**: Deploy an ECDSA private key via Key Manager and use it for ECDSA-P256 signature generation through the PSA Crypto API.

2. **RSA-DS Signing**: Deploy a Digital Signature (DS) key via Key Manager and use it for RSA-3072 signature generation through the PSA Crypto API.

## Key Manager Concepts

The Key Manager peripheral provides:

- **Secure Key Deployment**: Keys can be deployed in different modes (AES, ECDH0, Random) without exposing the raw key material.
- **Key Recovery**: Once deployed, keys can be recovered using the key recovery information for subsequent operations.
- **Hardware Protection**: Keys are protected by hardware and cannot be read back in plaintext.

### Key Deployment Modes

This example uses the **AES deployment mode** with pre-generated key material:

- `sw_init_key`: Software initialization key (32 bytes)
- `k2_info`: K2 information for key derivation (64 bytes)
- `k1_encrypted`: Encrypted K1 key material (32 bytes)

These parameters are used to derive and deploy the actual cryptographic key inside the Key Manager.

## How to Use the Example

### Hardware Required

This example can be executed on any development board with a Espressif SOC chip supporting the Key Manager peripheral (see Supported Targets table above).

### Configure the Project

```bash
idf.py set-target <chip_name>
idf.py menuconfig
```

In the menuconfig, you can enable/disable the signing examples:

- `Key Manager Signing Example Configuration`
  - `Enable ECDSA signing example` (default: enabled)
  - `Enable RSA-DS signing example` (default: enabled)

### Build and Flash

```bash
idf.py build
idf.py -p PORT flash monitor
```

(Replace `PORT` with your serial port, e.g., `/dev/ttyUSB0` or `COM3`)

To exit the serial monitor, type `Ctrl-]`.

### Key Storage in NVS

The example stores **key recovery info** in the NVS namespace `key_mgr` so that keys persist across reboots:

- **First boot** (or after `idf.py erase-flash`): No key is found in NVS, so the example deploys a new key via Key Manager and saves the recovery info to NVS. You will see logs like `No ECDSA key in NVS, deploying new key via Key Manager...`.
- **Later boots** (reset or `idf.py flash monitor` without erase): The example finds the key in NVS and reuses it. You will see `Using existing ECDSA key from NVS` (or DS).

To force **new keys** to be deployed (e.g. for testing), run erase before flashing:

```bash
idf.py erase-flash
idf.py -p PORT flash monitor
```

After `erase-flash`, the first boot will always deploy and store new keys; subsequent runs without erase will use the stored keys.

### Verify the flow with idf.py

From the example directory:

```bash
cd path/to/esp-idf/examples/security/key_manager

# Build
idf.py build

idf.py -p PORT flash

# Erase the NVS partition to force fresh key deployment on first run
esptool -p PORT --after no-reset erase-region 0x9000 0x6000

idf.py -p PORT monitor
# Expect: "No ECDSA key in NVS, deploying new key..." and "No DS key in NVS, deploying new key..."
# Exit monitor with Ctrl+]

# Second run (reuse path): flash and monitor only (no erase)
idf.py -p PORT monitor
# Expect: "Using existing ECDSA key from NVS" and "Using existing DS key from NVS"
```

### Example Output

#### First Boot

```
I (283) key_mgr_sign: ECDSA Signing with Key Manager
I (283) key_mgr_sign: Step 1: No ECDSA key in NVS, deploying new key via Key Manager...
I (293) key_mgr_sign: Key deployed successfully in Key Manager
I (303) key_mgr_sign: Stored key_recovery_info in NVS for key_type "ecdsa"
I (303) key_mgr_sign: Step 2: Creating PSA opaque key reference...
I (313) key_mgr_sign: Step 3: Signing hash using Key Manager key...
I (333) key_mgr_sign: Signature generated successfully (64 bytes)
I (333) key_mgr_sign: Step 4: Verifying signature with known public key...
I (353) key_mgr_sign: ECDSA signature verification PASSED!
I (353) key_mgr_sign: ECDSA Signing Example Complete

I (353) key_mgr_sign: RSA-DS Signing with Key Manager
I (353) key_mgr_sign: Step 1: No DS key in NVS, deploying new key via Key Manager...
I (373) key_mgr_sign: Key deployed successfully in Key Manager
I (383) key_mgr_sign: Stored key_recovery_info in NVS for key_type "ds"
I (383) key_mgr_sign: Step 2: Preparing DS data context...
I (393) key_mgr_sign: Step 3: Creating PSA opaque key reference for RSA-DS...
I (393) key_mgr_sign: Step 4: Signing hash using RSA-DS with Key Manager key...
I (1563) key_mgr_sign: RSA-DS signature generated successfully (384 bytes)
I (1563) key_mgr_sign: Step 5: Verifying RSA-DS signature with known public key...
I (1573) key_mgr_sign: RSA-DS signature verification PASSED!
I (1573) key_mgr_sign: RSA-DS Signing Example Complete

I (1573) key_mgr_sign: Key Manager Signing Example finished.
```

#### Second Boot

```
I (273) key_mgr_sign: Key Manager Signing Example
I (283) key_mgr_sign: ECDSA Signing with Key Manager
I (283) key_mgr_sign: Step 1: Using existing ECDSA key from NVS
I (293) key_mgr_sign: Step 2: Creating PSA opaque key reference...
I (293) key_mgr_sign: Step 3: Signing hash using Key Manager key...
I (313) key_mgr_sign: Signature generated successfully (64 bytes)
I (313) key_mgr_sign: Step 4: Verifying signature with known public key...
I (333) key_mgr_sign: ECDSA signature verification PASSED!
I (333) key_mgr_sign: ECDSA Signing Example Complete

I (343) key_mgr_sign: RSA-DS Signing with Key Manager
I (353) key_mgr_sign: Step 1: Using existing DS key from NVS
I (353) key_mgr_sign: Step 2: Preparing DS data context...
I (363) key_mgr_sign: Step 3: Creating PSA opaque key reference for RSA-DS...
I (363) key_mgr_sign: Step 4: Signing hash using RSA-DS with Key Manager key...
I (1543) key_mgr_sign: RSA-DS signature generated successfully (384 bytes)
I (1543) key_mgr_sign: Step 5: Verifying RSA-DS signature with known public key...
I (1553) key_mgr_sign: RSA-DS signature verification PASSED!
I (1553) key_mgr_sign: RSA-DS Signing Example Complete

I (1553) key_mgr_sign: Key Manager Signing Example finished.
```

## Code Walkthrough

### ECDSA Signing Flow

1. **Deploy Key**: Use `esp_key_mgr_deploy_key_in_aes_mode()` to deploy an ECDSA key.
2. **Create Opaque Key**: Create an `esp_ecdsa_opaque_key_t` structure with the key recovery info.
3. **Import to PSA**: Use `psa_import_key()` with `PSA_KEY_LIFETIME_ESP_ECDSA_VOLATILE` lifetime.
4. **Sign**: Use `psa_sign_hash()` to generate the signature.
5. **Verify**: Import the known ECDSA public key (from `test_data.h`) and use `psa_verify_hash()` to verify the signature.

### RSA-DS Signing Flow

1. **Deploy Key**: Use `esp_key_mgr_deploy_key_in_aes_mode()` to deploy a DS (HMAC) key.
2. **Prepare DS Context**: Create `esp_ds_data_t` with IV and encrypted parameters.
3. **Create Opaque Key**: Create an `esp_rsa_ds_opaque_key_t` structure.
4. **Import to PSA**: Use `psa_import_key()` with `PSA_KEY_LIFETIME_ESP_RSA_DS` lifetime.
5. **Sign**: Use `psa_sign_hash()` to generate the RSA signature.
6. **Verify**: Import the known RSA public key (from `test_data.h`) and use `psa_verify_hash()` to verify the signature.

## Test Data

The example uses test data generated by `generate_test_data.py` and stored in `main/test_data.h`.

The included test data was generated with **seed 12345** for ESP32-C5 (default target):

```bash
python generate_test_data.py --seed 12345 --output-header main/test_data.h
```

### Target-Specific RSA Key Sizes

Different ESP32 chips support different maximum RSA key sizes for the Digital Signature (DS) peripheral:

| Target | SOC_RSA_MAX_BIT_LEN | RSA Key Size |
|--------|---------------------|--------------|
| ESP32-C5 | 3072 | 3072-bit |
| ESP32-P4 | 4096 | 4096-bit |

The `--target` option allows generating test data with the correct RSA key size for your target chip.

### Generating Custom Test Data

A Python script is provided to generate new test data. When keys are generated (not loaded from files), they are automatically saved to PEM files in the same directory as the output header:
- `ecdsa_private_key.pem`: ECDSA P-256 private key
- `rsa_private_key.pem`: RSA private key (size depends on target)

#### Basic Usage (Random Keys)

```bash
# Print test data to stdout (default: ESP32-C5, 3072-bit RSA)
python generate_test_data.py

# Generate for ESP32-C5 (3072-bit RSA, default)
python generate_test_data.py --seed 12345 --output-header main/test_data.h

# Generate for ESP32-P4 (4096-bit RSA)
python generate_test_data.py --target esp32p4 --seed 12345 --output-header main/test_data.h

# Use a different seed for new random keys
python generate_test_data.py --seed 54321 --output-header main/test_data.h
```

#### Using Your Own Keys from PEM Files

You can provide your own ECDSA and/or RSA private keys in PEM format. When a key is loaded from a file, it won't be saved again:

```bash
# Use your own ECDSA key (only RSA key will be generated and saved)
python generate_test_data.py --ecdsa-key my_ecdsa_key.pem --output-header main/test_data.h

# Use your own RSA key (only ECDSA key will be generated and saved)
python generate_test_data.py --rsa-key my_rsa_key.pem --output-header main/test_data.h

# Use both your own keys (no keys will be generated or saved)
python generate_test_data.py --ecdsa-key ecdsa.pem --rsa-key rsa.pem --output-header main/test_data.h

# Combine with seed for reproducible Key Manager parameters
python generate_test_data.py --seed 12345 --ecdsa-key test/ecdsa_private_key.pem --rsa-key test/rsa_private_key.pem --output-header main/test_data.h
```

**Key Requirements:**
- **ECDSA key**: Must be a P-256 (secp256r1) curve private key
- **RSA key**: Must match the target's RSA key size (3072-bit for ESP32-C5, 4096-bit for ESP32-P4)

**Generating PEM Keys Manually (using OpenSSL):**

```bash
# Generate ECDSA P-256 key
openssl ecparam -name prime256v1 -genkey -noout -out ecdsa_key.pem

# Generate RSA 3072-bit key (for ESP32-C5)
openssl genrsa -out rsa_key.pem 3072

# Generate RSA 4096-bit key (for ESP32-P4)
openssl genrsa -out rsa_key.pem 4096
```

The script generates:
- **Key Manager parameters**: `init_key`, `k2_info` for AES mode deployment
- **ECDSA key material**: `k1_ecdsa256_encrypt` (encrypted ECDSA private key)
- **DS key material**: `k1_ds_aes_key_encrypt`, `ds_iv`, `ds_c` (encrypted DS parameters)

> **Note:** The example verifies signatures against known public keys stored in `test_data.h`. When generating custom test data with new keys, you must also export the corresponding public keys and update `main/test_data.h`:
> ```bash
> # Export ECDSA public key (uncompressed point format for ecdsa_public_key[])
> openssl ec -in ecdsa_private_key.pem -pubout -outform DER -conv_form uncompressed | tail -c 65 | xxd -i
>
> # Export RSA public key (PKCS#1 DER format for rsa_public_key_der[])
> openssl rsa -in rsa_private_key.pem -RSAPublicKey_out -outform DER | xxd -i
> ```

#### How Key Manager Test Data is Generated

1. **Base Keys**: Random `init_key`, `k2`, and `rand_num` are generated. When `--seed` is provided, all three values are derived from the seeded RNG, making the entire output deterministic and reproducible across runs. Without `--seed`, `os.urandom` is used and every run produces different data.
2. **K2 Info**: Computed as `AES_ECB(init_key, AES_ECB(rand_num, k2) || rand_num)`
3. **K1 Encrypted**: The actual key (ECDSA or DS) is reversed and encrypted with K2 using AES-ECB
4. **DS Parameters**: RSA key parameters are encrypted using AES-CBC with the DS AES key derived from HMAC

> **Note:** The test data included in the example (`main/test_data.h`) was generated using `--seed 12345` together with the PEM keys stored in the `test/` directory (`test/ecdsa_private_key.pem` and `test/rsa_private_key.pem`). Using the same seed and key files will always produce identical output.

## Running the Test

This example includes a pytest that can be run with the ESP-IDF pytest framework:

```bash
# Run the test on the target
pytest --target <chip_name>
```

The test verifies:
- ECDSA key deployment and signing flow
- ECDSA signature verification using the known public key
- RSA-DS key deployment and signing flow
- RSA-DS signature verification using the known public key
