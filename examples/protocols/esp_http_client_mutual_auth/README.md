| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

# ESP HTTP Client Mutual TLS Authentication Example

This example demonstrates mutual TLS (mTLS) authentication using `esp_http_client`. The server verifies the client's identity via a client certificate, and the client verifies the server using a CA certificate.

Two client key modes are supported, selectable via Kconfig:

| Mode | Kconfig | Client key source |
|------|---------|-------------------|
| **Software keys** (default) | `EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS=n` | PEM files embedded in firmware |
| **DS peripheral** | `EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS=y` | Hardware Digital Signature peripheral via `esp_secure_cert` partition |

## How it works

1. Device connects to Wi-Fi or Ethernet (via `protocol_examples_common`)
2. Prompts for HTTPS server URL on serial console
3. Performs HTTPS GET with mutual TLS — sends client certificate to server
4. Logs the HTTP status code

## Quick start (software key mode)

```bash
idf.py set-target esp32c3
idf.py build flash monitor
```

When prompted, enter the URL of an HTTPS server that requires client certificates. The pre-generated certificates in `main/certs/` are used automatically.

## Certificates

Pre-generated certificates with 10-year lifetime are in `main/certs/`. A single CA signs both client and server certificates. See `main/certs/README.md` for OpenSSL commands to regenerate.

| File | Purpose | Embedded in firmware? |
|------|---------|----------------------|
| `ca_cert.pem` | CA certificate (verifies server) | Yes (both modes) |
| `client_cert.pem` | Client certificate | Yes (software mode only) |
| `client_key.pem` | Client private key | Yes (software mode only) |
| `server_cert.pem` | Server certificate (pytest) | No |
| `server_key.pem` | Server private key (pytest) | No |

## DS peripheral mode

The DS (Digital Signature) peripheral holds the client private key in hardware. The client certificate and DS context are stored in the `esp_secure_cert` partition.

### Prerequisites

- ESP32-C3 or other chip with `SOC_DIG_SIGN_SUPPORTED`
- `esp-secure-cert-tool` (`pip install esp-secure-cert-tool`)

### Provisioning

1. **Generate DS partition and HMAC key** (from the example directory):

```bash
configure_esp_secure_cert.py \
  --device-cert main/certs/client_cert.pem \
  --target_chip esp32c3 \
  --configure_ds --priv_key_algo RSA 2048 \
  --skip_flash --keep_ds_data_on_host \
  --private-key main/certs/client_key.pem \
  --efuse_key_id 1
```

Copy the output files to `main/certs/esp_secure_cert_data/`:
- `esp_secure_cert.bin`
- `hmac_key.bin`

2. **Burn HMAC key to eFuse:**

```bash
# QEMU
idf.py qemu efuse-burn-key --do-not-confirm BLOCK_KEY1 \
  main/certs/esp_secure_cert_data/hmac_key.bin HMAC_DOWN_DIGITAL_SIGNATURE

# Hardware (irreversible!)
idf.py efuse-burn-key --do-not-confirm BLOCK_KEY1 \
  main/certs/esp_secure_cert_data/hmac_key.bin HMAC_DOWN_DIGITAL_SIGNATURE
```

3. **Build and flash:**

```bash
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.qemu_ds" build flash monitor
```

## QEMU testing

Three QEMU tests are included:

- `test_mutual_auth_software_keys` — software key mTLS, expects HTTP 200
- `test_mutual_auth_ds` — DS peripheral mTLS, expects HTTP 200
- `test_mutual_auth_ds_fails_wrong_credentials` — DS with wrong server CA, expects failure

The tests start a local HTTPS server with mutual TLS, inject the URL via serial, and verify device output.

## Troubleshooting

**TLS handshake failed: BAD_SIGNATURE**
The client certificate is not signed by the CA the server expects. Regenerate certs using the commands in `main/certs/README.md`.

**ESP_ERR_HW_CRYPTO_DS_HMAC_FAIL**
The HMAC key in eFuse does not match the key used to create the DS partition. For QEMU, delete the eFuse image and regenerate it from the current `hmac_key.bin`.
