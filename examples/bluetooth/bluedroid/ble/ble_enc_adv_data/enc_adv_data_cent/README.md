| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H4 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# BLE Encrypted Advertising Data Central Example (Bluedroid)

This example demonstrates how to receive and decrypt BLE Encrypted Advertising Data (EAD) with Bluedroid stack.

## Overview

This central example works with the `enc_adv_data_prph` peripheral example to demonstrate:

1. Scanning for devices that advertise encrypted data
2. Connecting to read the Key Material characteristic
3. Decrypting advertising data using the obtained key

## Two Operation Modes

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        Mode Selection                                        │
├────────────────────────────────┬────────────────────────────────────────────┤
│   Mode 1: With Connection      │   Mode 2: No Connection                    │
│         (Default)              │      (Pre-shared Key)                      │
├────────────────────────────────┼────────────────────────────────────────────┤
│                                │                                            │
│  First scan:                   │  All scans:                                │
│  ┌─────────┐    ┌─────────┐   │  ┌─────────┐    ┌─────────┐               │
│  │ Central │══▶│ Periph  │   │  │ Central │──▶│ Periph  │               │
│  └─────────┘    └─────────┘   │  └─────────┘    └─────────┘               │
│       │              │         │       │                                   │
│       │ Connect      │         │       │ Scan only                         │
│       │ Read Key     │         │       ▼                                   │
│       │ Disconnect   │         │  Use pre-configured                       │
│       ▼              │         │  key to decrypt                           │
│  Store key           │         │       │                                   │
│       │              │         │       ▼                                   │
│       ▼              │         │  ✅ Decrypt immediately                   │
│  Later scans:        │         │                                            │
│  ┌─────────┐    ┌─────────┐   │                                            │
│  │ Central │──▶│ Periph  │   │                                            │
│  └─────────┘    └─────────┘   │                                            │
│       │                        │                                            │
│       │ No connection needed   │                                            │
│       ▼                        │                                            │
│  ✅ Decrypt using stored key   │                                            │
│                                │                                            │
├────────────────────────────────┼────────────────────────────────────────────┤
│ ✓ Secure key exchange          │ ✓ No connection latency                   │
│ ✓ Dynamic key support          │ ✓ Simpler implementation                  │
│ ✗ First-time connection needed │ ✗ Key must be pre-provisioned             │
└────────────────────────────────┴────────────────────────────────────────────┘
```

## System Flow Diagram

### Mode 1: With Connection (Default)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    Central Flow - With Connection Mode                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────┐                                         ┌─────────────┐       │
│  │ Central │                                         │ Peripheral  │       │
│  └────┬────┘                                         └──────┬──────┘       │
│       │                                                      │              │
│       │  ════════════════ First Encounter ════════════════  │              │
│       │                                                      │              │
│       │  1. Scan                                             │              │
│       │ ──────────────────────────────────────────────────▶ │              │
│       │                                                      │              │
│       │  2. Receive Adv (UUID=0x2C01, Encrypted Data)       │              │
│       │ ◀────────────────────────────────────────────────── │              │
│       │                                                      │              │
│       │     [No key yet - cannot decrypt]                   │              │
│       │                                                      │              │
│       │  3. Connect                                          │              │
│       │ ═══════════════════════════════════════════════════▶│              │
│       │                                                      │              │
│       │  4. Establish Encrypted Link (Pairing)              │              │
│       │ ◀═══════════════════════════════════════════════════│              │
│       │                                                      │              │
│       │  5. Read Key Material Characteristic (0x2B88)       │              │
│       │ ═══════════════════════════════════════════════════▶│              │
│       │                                                      │              │
│       │  6. Response: [Session Key (16B)] [IV (8B)]         │              │
│       │ ◀═══════════════════════════════════════════════════│              │
│       │                                                      │              │
│       │  7. Store key in memory                              │              │
│       │  8. Disconnect                                       │              │
│       │ ═══════════════════════════════════════════════════▶│              │
│       │                                                      │              │
│       │  ════════════════ Later Scans ════════════════════  │              │
│       │                                                      │              │
│       │  9. Scan                                             │              │
│       │ ──────────────────────────────────────────────────▶ │              │
│       │                                                      │              │
│       │  10. Receive Encrypted Adv Data                     │              │
│       │ ◀────────────────────────────────────────────────── │              │
│       │                                                      │              │
│       │  11. Decrypt using stored key (NO CONNECTION!)      │              │
│       │      ┌────────────────────────────────────────┐     │              │
│       │      │ ble_ead_decrypt(session_key, iv, ...)  │     │              │
│       │      │ Result: "prph" (decrypted name)        │     │              │
│       │      └────────────────────────────────────────┘     │              │
│       │                                                      │              │
│       ▼                                                      ▼              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Mode 2: No Connection (Pre-shared Key)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                 Central Flow - No Connection Mode                            │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────┐                               │
│  │  Pre-configured Key (same as Peripheral) │                               │
│  │  Session Key: 19 6a 0a d1 2a 61 20 1e    │                               │
│  │               13 6e 2e d1 12 da a9 57    │                               │
│  │  IV:          9E 7a 00 ef b1 7a e7 46    │                               │
│  └─────────────────────────────────────────┘                               │
│                      │                                                      │
│                      ▼                                                      │
│  ┌─────────┐                                         ┌─────────────┐       │
│  │ Central │                                         │ Peripheral  │       │
│  └────┬────┘                                         └──────┬──────┘       │
│       │                                                      │              │
│       │  1. Scan (passive)                                   │              │
│       │ ──────────────────────────────────────────────────▶ │              │
│       │                                                      │              │
│       │  2. Receive Encrypted Adv Data                      │              │
│       │ ◀────────────────────────────────────────────────── │              │
│       │                                                      │              │
│       │  3. Immediately decrypt (NO CONNECTION!)            │              │
│       │      ┌────────────────────────────────────────┐     │              │
│       │      │ ble_ead_decrypt(pre_shared_key, ...)   │     │              │
│       │      │ Result: "prph" (decrypted name)        │     │              │
│       │      └────────────────────────────────────────┘     │              │
│       │                                                      │              │
│       ▼                                                      ▼              │
│                                                                             │
│  ⚡ No connection overhead - instant decryption!                            │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Decryption Process

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         Decryption Process                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  Received Encrypted Advertising Data:                                       │
│  ┌───────────────────┬─────────────────────┬─────────────────┐             │
│  │    Randomizer     │     Ciphertext      │      MIC        │             │
│  │    (5 bytes)      │     (6 bytes)       │   (4 bytes)     │             │
│  └─────────┬─────────┴──────────┬──────────┴────────┬────────┘             │
│            │                    │                   │                       │
│            ▼                    │                   │                       │
│  1. Extract Randomizer          │                   │                       │
│     ┌─────────────────────┐    │                   │                       │
│     │ XX XX XX XX [D|XX]  │    │                   │                       │
│     └─────────────────────┘    │                   │                       │
│            │                    │                   │                       │
│            ▼                    │                   │                       │
│  2. Build Nonce                 │                   │                       │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │  Randomizer (5B)  +  IV from Key Material (8B)  =  Nonce (13B)  │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│            │                    │                   │                       │
│            ▼                    ▼                   ▼                       │
│  3. AES-CCM Decrypt + Verify                                               │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │  Input: Ciphertext, MIC, Nonce, Session Key, AAD (0xEA)         │    │
│     │  Algorithm: AES-CCM-128 Authenticated Decryption                │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│            │                                                                │
│            ▼                                                                │
│  4. Output: Decrypted Plaintext                                            │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │  [05] [09] [70 72 70 68]                                        │    │
│     │   Len  Type  'p' 'r' 'p' 'h'                                    │    │
│     │                                                                  │    │
│     │  → Complete Local Name: "prph"                                  │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## How to Use Example

### Hardware Required

* Two development boards with ESP32/ESP32-C2/ESP32-C3/ESP32-C5/ESP32-C6/ESP32-C61/ESP32-H2/ESP32-S3 SoC
* USB cables for power supply and programming

### Setup

1. Flash `enc_adv_data_prph` on one board (peripheral)
2. Flash `enc_adv_data_cent` on another board (central)

### Configure the project

```bash
idf.py set-target <chip_name>
idf.py menuconfig
```

In menuconfig, navigate to:
- `Example Configuration` → `Central Mode`
  - `With Connection` - Connect to read key (default)
  - `No Connection` - Use pre-shared key

### Build and Flash

```bash
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

### Example Output

**Mode 1 - With Connection (first scan):**
```
I (XXX) ENC_ADV_CENT: Encrypted Advertising Data Central started
I (XXX) ENC_ADV_CENT: Scanning started
I (XXX) ENC_ADV_CENT: Found target device: xx:xx:xx:xx:xx:xx
I (XXX) ENC_ADV_CENT: Connecting to get key material...
I (XXX) ENC_ADV_CENT: Connected, conn_id 0
I (XXX) ENC_ADV_CENT: Authentication success
I (XXX) ENC_ADV_CENT: Key material received:
I (XXX) ENC_ADV_CENT: 19 6a 0a d1 2a 61 20 1e 13 6e 2e d1 12 da a9 57 9e 7a 00 ef b1 7a e7 46
I (XXX) ENC_ADV_CENT: Disconnected
```

**Mode 1 - With Connection (subsequent scans):**
```
I (XXX) ENC_ADV_CENT: Found target device: xx:xx:xx:xx:xx:xx
I (XXX) ENC_ADV_CENT: Have key material, decrypting...
I (XXX) ENC_ADV_CENT: Decryption successful!
I (XXX) ENC_ADV_CENT: Decrypted data:
I (XXX) ENC_ADV_CENT: 05 09 70 72 70 68
I (XXX) ENC_ADV_CENT: Decrypted device name: prph
```

**Mode 2 - No Connection:**
```
I (XXX) ENC_ADV_CENT_SIMPLE: ========================================
I (XXX) ENC_ADV_CENT_SIMPLE:   EAD Central - No Connection Mode
I (XXX) ENC_ADV_CENT_SIMPLE: ========================================
I (XXX) ENC_ADV_CENT_SIMPLE: ⚡ This example decrypts WITHOUT connecting!
I (XXX) ENC_ADV_CENT_SIMPLE: 🔍 Scanning started (no connection mode)
...
I (XXX) ENC_ADV_CENT_SIMPLE: ✅ Decryption successful (no connection needed!)
I (XXX) ENC_ADV_CENT_SIMPLE: 📛 Decrypted device name: "prph"
```

## Troubleshooting

### Authentication Failed
- Ensure both devices have matching security parameters
- Try erasing NVS on both devices: `idf.py erase_flash`

### Decryption Failed
- Verify the peripheral and central use matching session key and IV
- Check that the encrypted advertising data format is correct

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub.
