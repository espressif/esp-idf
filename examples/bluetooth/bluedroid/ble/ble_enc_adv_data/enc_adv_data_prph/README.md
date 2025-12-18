| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# BLE Encrypted Advertising Data Peripheral Example (Bluedroid)

This example demonstrates how to use BLE Encrypted Advertising Data (EAD) feature with Bluedroid stack.

## Overview

The Encrypted Advertising Data feature (introduced in Bluetooth Core Specification 5.4) allows devices to encrypt portions of their advertising data using AES-CCM. This enables:

- Privacy protection for sensitive advertising data
- Selective disclosure of advertising data to authorized devices
- Enhanced security for BLE advertising

## System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         PERIPHERAL (This Example)                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────┐    ┌──────────────────┐    ┌─────────────────────┐    │
│  │  Original Data  │───▶│  AES-CCM Encrypt │───▶│  Encrypted Adv Data │    │
│  │  "prph" (name)  │    │  (Session Key+IV)│    │  (Randomizer+Cipher │    │
│  └─────────────────┘    └──────────────────┘    │   +MIC)             │    │
│                                                  └──────────┬──────────┘    │
│                                                             │               │
│                                                             ▼               │
│  ┌──────────────────────────────────────────────────────────────────────┐  │
│  │                     BLE Advertising Packet                            │  │
│  ├──────────┬─────────────┬────────────────┬────────────────────────────┤  │
│  │  Flags   │  Name "key" │  UUID 0x2C01   │  Encrypted Data (AD 0x31) │  │
│  │  (3B)    │    (5B)     │     (4B)       │         (16B)              │  │
│  └──────────┴─────────────┴────────────────┴────────────────────────────┘  │
│                                                                             │
│  ┌──────────────────────────────────────────────────────────────────────┐  │
│  │                     GATT Server                                       │  │
│  ├──────────────────────────────────────────────────────────────────────┤  │
│  │  GAP Service (0x1800)                                                 │  │
│  │    └── Key Material Characteristic (0x2B88)                          │  │
│  │          └── Value: [Session Key (16B)] [IV (8B)]                    │  │
│  │          └── Permission: Read (Encrypted Link Required)              │  │
│  └──────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Encryption Flow

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         Encryption Process                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  1. Generate Random Randomizer (5 bytes)                                    │
│     ┌─────────────────────────────────────────┐                            │
│     │ XX XX XX XX [D|XX]                      │  D = Direction Bit = 1     │
│     └─────────────────────────────────────────┘                            │
│                          │                                                  │
│                          ▼                                                  │
│  2. Build Nonce (13 bytes)                                                  │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │     Randomizer (5B)      │           IV (8B)                    │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                          │                                                  │
│                          ▼                                                  │
│  3. AES-CCM Encryption                                                      │
│     ┌─────────────────┐                                                    │
│     │  Plaintext      │  +  Session Key  +  Nonce  +  AAD (0xEA)           │
│     │  [05 09 p r p h]│                                                    │
│     └─────────────────┘                                                    │
│                          │                                                  │
│                          ▼                                                  │
│  4. Output: Encrypted Payload                                               │
│     ┌─────────────────────────────────────────────────────────────────┐    │
│     │  Randomizer (5B)  │  Ciphertext (6B)  │  MIC (4B)               │    │
│     └─────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Advertising Data Format

```
Complete Advertising Packet (29 bytes):

Offset  Length  Type  Data                    Description
──────  ──────  ────  ────                    ───────────
0       2       0x01  0x06                    Flags: LE General Discoverable
3       4       0x09  'k' 'e' 'y'             Complete Local Name
8       3       0x03  0x01 0x2C               16-bit Service UUID: 0x2C01
12      16      0x31  [Encrypted Payload]     Encrypted Advertising Data

Encrypted Payload Detail:
┌───────────────────┬─────────────────────┬─────────────────┐
│    Randomizer     │     Ciphertext      │      MIC        │
│    (5 bytes)      │     (6 bytes)       │   (4 bytes)     │
│  Random + Dir=1   │  AES-CCM output     │  Auth Tag       │
└───────────────────┴─────────────────────┴─────────────────┘
```

## How to Use Example

### Hardware Required

* A development board with ESP32/ESP32-C2/ESP32-C3/ESP32-C5/ESP32-C6/ESP32-C61/ESP32-H2/ESP32-S3 SoC
* A USB cable for power supply and programming

### Configure the project

```bash
idf.py set-target <chip_name>
idf.py menuconfig
```

### Build and Flash

```bash
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

### Example Output

```
I (XXX) ENC_ADV_PRPH: Encrypted Advertising Data Peripheral started
I (XXX) ENC_ADV_PRPH: Key Material (Session Key + IV):
I (XXX) ENC_ADV_PRPH: 19 6a 0a d1 2a 61 20 1e 13 6e 2e d1 12 da a9 57 9e 7a 00 ef b1 7a e7 46
I (XXX) ENC_ADV_PRPH: Data before encryption:
I (XXX) ENC_ADV_PRPH: 05 09 70 72 70 68
I (XXX) ENC_ADV_PRPH: Encryption of adv data done successfully
I (XXX) ENC_ADV_PRPH: Raw advertising data set complete
I (XXX) ENC_ADV_PRPH: Advertising start successfully
```

## Testing with Central

Use the `enc_adv_data_cent` example as the central device:

```
┌──────────────────┐                    ┌──────────────────┐
│    PERIPHERAL    │                    │     CENTRAL      │
│  (This Example)  │                    │ (enc_adv_data_   │
│                  │                    │      cent)       │
└────────┬─────────┘                    └────────┬─────────┘
         │                                       │
         │  1. Broadcast Encrypted Adv           │
         │ ─────────────────────────────────────▶│
         │                                       │
         │  2. Connect (first time only)         │
         │ ◀═══════════════════════════════════ │
         │                                       │
         │  3. Read Key Material (0x2B88)        │
         │ ═══════════════════════════════════▶ │
         │                                       │
         │  4. Return Session Key + IV           │
         │ ◀═══════════════════════════════════ │
         │                                       │
         │  5. Disconnect                        │
         │ ◀═══════════════════════════════════ │
         │                                       │
         │  6. Future: Decrypt without connect   │
         │ ─────────────────────────────────────▶│
         │                                       │
         ▼                                       ▼
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub.
