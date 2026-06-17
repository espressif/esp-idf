| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | --------- |

# NAN Crypto Test Application

This test application validates the NAN ND-PMK derivation implementation against
official Wi-Fi Aware Specification v4.0 test vectors (Appendix M.1).

## Building

```bash
cd components/esp_wifi/test_apps/nan_crypto_test
idf.py build
```

## Running

```bash
idf.py -p PORT flash monitor
```

## Test Vectors

The application tests three official test vectors:
1. Passphrase: "NAN"
2. Passphrase: "NAN2"
3. Passphrase: "NAN-Testvector-Phrase"

All use:
- Cipher ID: 0x01 (NCS-SK-128)
- Service ID: 2b9c450f6671
- NMI: 02904c12d001
