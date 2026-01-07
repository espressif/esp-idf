| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# ESP-IDF BLE ACL Latency Test - Peripheral

This example demonstrates how to measure BLE ACL (Asynchronous Connection-Less) link latency. The peripheral acts as a GATT server that echoes back any data written to it, allowing the central device to measure round-trip time (RTT) and calculate one-way latency.

This demo should be used together with the `cent` (central) example.

## How It Works

1. The peripheral advertises with the device name `ESP_ACL_LATENCY`
2. When a central device connects, the peripheral accepts the connection
3. The peripheral provides a GATT service with a characteristic that supports both write and notify
4. When data is written to the characteristic, the peripheral immediately echoes it back via notification
5. The central measures the time between sending and receiving to calculate RTT and latency

## Test Flow Diagram

```
    ┌──────────────┐                              ┌──────────────┐
    │   Central    │                              │  Peripheral  │
    │ (GATT Client)│                              │ (GATT Server)│
    └──────┬───────┘                              └──────┬───────┘
           │                                             │
           │                                             │  Advertising
           │                                             │  "ESP_ACL_LATENCY"
           │  Scan & Connect                             │
           │ ─────────────────────────────────────────>  │
           │                                             │
           │  Connection Established                     │
           │ <─────────────────────────────────────────> │
           │                                             │
           │  MTU Exchange, Service Discovery            │
           │ <─────────────────────────────────────────> │
           │                                             │
           │  ──────────── Latency Test ─────────────    │
           │                                             │
           │  Write [SEQ=N, 242 bytes]                   │
           │ ─────────────────────────────────────────>  │
           │                                     ┌───────┴───────┐
           │                                     │ Echo the same │
           │                                     │ data back via │
           │                                     │ Notification  │
           │                                     └───────┬───────┘
           │  Notification [SEQ=N, 242 bytes]            │
           │ <─────────────────────────────────────────  │
           │                                             │
           │  ... Repeat for 100 packets ...             │
           │                                             │
    ┌──────┴───────┐                              ┌──────┴───────┐
    │   Central    │                              │  Peripheral  │
    └──────────────┘                              └──────────────┘

                    ┌─────────────────────────────────┐
                    │     Latency Calculation         │
                    ├─────────────────────────────────┤
                    │  RTT = recv_time - send_time    │
                    │  One-way Latency = RTT / 2      │
                    │  Typical Result: 6-10 ms        │
                    └─────────────────────────────────┘
```

## How to Use Example

### Hardware Required

* Two development boards with supported SoC (e.g., ESP32-C3-DevKitC, ESP32-DevKitC, etc.)
* Two USB cables for power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information.

### Configure the Project

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Build and Flash

Build the project and flash it to the board:

```bash
idf.py -p PORT flash
```

(Replace PORT with the serial port name, e.g., `/dev/ttyUSB0` on Linux or `COM3` on Windows)

### Monitor

Run `idf.py -p PORT monitor` to view the serial output.

(To exit the serial monitor, type `Ctrl-]`.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (362) BLE_ACL_LATENCY_PERIPH: BLE ACL Latency Test - Peripheral
I (372) phy_init: phy_version 310,dde1ba9,Jun  4 2024,16:38:11
I (432) BLE_ACL_LATENCY_PERIPH: GATT server register, status=0, app_id=0
I (442) BLE_ACL_LATENCY_PERIPH: Create attribute table successfully, the number handle=4
I (442) BLE_ACL_LATENCY_PERIPH: SERVICE_START_EVT, status=0, service_handle=40
I (452) BLE_ACL_LATENCY_PERIPH: Advertising started
I (462) BLE_ACL_LATENCY_PERIPH: Peripheral ready, waiting for connection...
I (5532) BLE_ACL_LATENCY_PERIPH: ESP_GATTS_CONNECT_EVT, conn_id=0, remote 7c:df:a1:66:a6:00
I (5892) BLE_ACL_LATENCY_PERIPH: MTU exchange, MTU=517
I (5912) BLE_ACL_LATENCY_PERIPH: Connection params updated: status=0, conn_int=6, latency=0, timeout=500
```

## GATT Service Structure

| Attribute | UUID   | Properties        | Description                     |
|-----------|--------|-------------------|---------------------------------|
| Service   | 0x1234 | -                 | Latency test service            |
| Char      | 0x5678 | Write, Notify     | Write data, receive echo back   |
| CCCD      | 0x2902 | Read, Write       | Client Characteristic Config    |

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
