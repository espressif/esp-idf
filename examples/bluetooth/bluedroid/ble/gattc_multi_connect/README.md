| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# ESP-IDF Gatt Client Multi Connection Example

This example demonstrates how to use the BLE GATT client APIs to create a multi-connection client that can connect to multiple GATT servers simultaneously. By default, it supports connecting to up to 3 GATT servers at the same time.

## How It Works

The example creates three GATT client profiles (A, B, C), each corresponding to one connection. It scans for devices with specific names, connects to them, discovers services, registers for notifications, and exchanges data with each server.

### Target Devices

The example searches for three GATT servers with the following names:
- `ESP_GATTS_DEMO_a` → Profile A
- `ESP_GATTS_DEMO_b` → Profile B
- `ESP_GATTS_DEMO_c` → Profile C

You can use the [gatt_server](../gatt_server) example to create these server devices by modifying the device name.

## Flow Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        GATT Client Multi-Connection                         │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌──────────────────┐
    │   GATTC Client   │
    │ (Multi-Connect)  │
    └────────┬─────────┘
             │
             │  1. Start Scanning
             │
             ▼
    ┌────────────────────────────────────────────────────────────────────────┐
    │                         Scan for BLE Devices                           │
    │         Looking for: ESP_GATTS_DEMO_a, _b, _c                          │
    └────────────────────────────────────────────────────────────────────────┘
             │
             │  Found device matching name?
             │
             ├─────────────────────┬─────────────────────┐
             ▼                     ▼                     ▼
    ┌────────────────┐    ┌────────────────┐    ┌────────────────┐
    │ ESP_GATTS_DEMO_a│    │ ESP_GATTS_DEMO_b│    │ ESP_GATTS_DEMO_c│
    │   (Server A)   │    │   (Server B)   │    │   (Server C)   │
    └───────┬────────┘    └───────┬────────┘    └───────┬────────┘
            │                     │                     │
            ▼                     ▼                     ▼
    ┌───────────────────────────────────────────────────────────────────────┐
    │                    For Each Connected Device:                         │
    ├───────────────────────────────────────────────────────────────────────┤
    │  2. Stop Scanning                                                     │
    │  3. Connect (esp_ble_gattc_enh_open)                                  │
    │  4. MTU Exchange                                                      │
    │  5. Service Discovery (UUID: 0x00FF)                                  │
    │  6. Find Characteristic (UUID: 0xFF01)                                │
    │  7. Register for Notification                                         │
    │  8. Write CCCD (Enable Notification)                                  │
    │  9. Write Test Data (35 bytes)                                        │
    │ 10. Resume Scanning for remaining devices                             │
    └───────────────────────────────────────────────────────────────────────┘
             │
             │  All 3 devices connected?
             ▼
    ┌────────────────────────────────────────────────────────────────────────┐
    │                         Stop Scanning                                  │
    │              Continue receiving notifications from all devices          │
    └────────────────────────────────────────────────────────────────────────┘


                    ┌────────────────────────────────────┐
                    │     Connection Sequence Detail     │
                    └────────────────────────────────────┘

     Client                                              Server
       │                                                   │
       │  ─────────── Connection Phase ───────────         │
       │                                                   │
       │  Connect Request                                  │
       │ ────────────────────────────────────────────────> │
       │                                                   │
       │  Connection Established                           │
       │ <────────────────────────────────────────────────>│
       │                                                   │
       │  MTU Exchange Request                             │
       │ ────────────────────────────────────────────────> │
       │                                                   │
       │  MTU Exchange Response                            │
       │ <──────────────────────────────────────────────── │
       │                                                   │
       │  ─────────── Service Discovery ───────────        │
       │                                                   │
       │  Discover Services (UUID: 0x00FF)                 │
       │ ────────────────────────────────────────────────> │
       │                                                   │
       │  Service Found                                    │
       │ <──────────────────────────────────────────────── │
       │                                                   │
       │  Get Characteristic (UUID: 0xFF01)                │
       │ ────────────────────────────────────────────────> │
       │                                                   │
       │  Characteristic Found                             │
       │ <──────────────────────────────────────────────── │
       │                                                   │
       │  ─────────── Enable Notification ───────────      │
       │                                                   │
       │  Register for Notify                              │
       │ ────────────────────────────────────────────────> │
       │                                                   │
       │  Write CCCD (0x0001 = Enable Notify)              │
       │ ────────────────────────────────────────────────> │
       │                                                   │
       │  ─────────── Data Exchange ───────────            │
       │                                                   │
       │  Write Characteristic (35 bytes test data)        │
       │ ────────────────────────────────────────────────> │
       │                                                   │
       │  Notification (data from server)                  │
       │ <──────────────────────────────────────────────── │
       │                                                   │
```

## How to Use Example

### Hardware Required

* Four development boards with supported SoC:
  - One board for the GATT Client (this example)
  - Three boards for GATT Servers (using [gatt_server](../gatt_server) example)
* Four USB cables for power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information.

### Setup GATT Servers

1. Flash the [gatt_server](../gatt_server) example to three separate boards
2. Modify each server's device name before flashing:
   - Server 1: Change `ESP_GATT_DEMO` to `ESP_GATTS_DEMO_a`
   - Server 2: Change `ESP_GATT_DEMO` to `ESP_GATTS_DEMO_b`
   - Server 3: Change `ESP_GATT_DEMO` to `ESP_GATTS_DEMO_c`

### Configure the Project

Set the correct chip target:

```bash
idf.py set-target <chip_name>
```

**Connecting More Than 4 Devices:**

The default maximum number of ACL connections is 4. To connect more devices:

```bash
idf.py menuconfig
```

Navigate to: `Component config` → `Bluetooth` → `Bluedroid Options` → `BT/BLE MAX ACL CONNECTIONS`

### Build and Flash

```bash
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (512) BTDM_INIT: BT controller compile version [1342a48]
I (522) system_api: Base MAC address is not set
I (522) system_api: read default base MAC address from EFUSE
I (522) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (932) GATTC_MULTIPLE_DEMO: REG_EVT
I (932) GATTC_MULTIPLE_DEMO: REG_EVT
I (932) GATTC_MULTIPLE_DEMO: REG_EVT
I (942) GATTC_MULTIPLE_DEMO: Scan start success
I (1072) GATTC_MULTIPLE_DEMO: 38 68 a4 69 bb 7c
I (1072) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 28, Scan Response Len 0
I (1072) GATTC_MULTIPLE_DEMO: Searched Device Name Len 0
I (1082) GATTC_MULTIPLE_DEMO:

I (1102) GATTC_MULTIPLE_DEMO: 08 ef 3b a7 04 41
I (1102) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 9, Scan Response Len 15
I (1102) GATTC_MULTIPLE_DEMO: Searched Device Name Len 13
I (1112) GATTC_MULTIPLE_DEMO: LG CM2760(41)
I (1112) GATTC_MULTIPLE_DEMO:

I (1222) GATTC_MULTIPLE_DEMO: 38 68 a4 69 bb 7c
I (1222) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 28, Scan Response Len 0
I (1222) GATTC_MULTIPLE_DEMO: Searched Device Name Len 0
I (1232) GATTC_MULTIPLE_DEMO:

I (1372) GATTC_MULTIPLE_DEMO: 38 68 a4 69 bb 7c
I (1372) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 28, Scan Response Len 0
I (1372) GATTC_MULTIPLE_DEMO: Searched Device Name Len 0
I (1382) GATTC_MULTIPLE_DEMO:

I (1412) GATTC_MULTIPLE_DEMO: 08 ef 3b a7 04 41
I (1412) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 9, Scan Response Len 15
I (1422) GATTC_MULTIPLE_DEMO: Searched Device Name Len 13
I (1422) GATTC_MULTIPLE_DEMO: LG CM2760(41)
I (1432) GATTC_MULTIPLE_DEMO:

I (1522) GATTC_MULTIPLE_DEMO: 38 68 a4 69 bb 7c
I (1522) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 28, Scan Response Len 0
I (1522) GATTC_MULTIPLE_DEMO: Searched Device Name Len 0
I (1532) GATTC_MULTIPLE_DEMO:
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
