| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# ESP-IDF BLE ACL Latency Test - Central

This example demonstrates how to measure BLE ACL (Asynchronous Connection-Less) link latency. The central device scans for the peripheral, connects to it, and then performs a latency test by sending packets and measuring the round-trip time (RTT).

This demo should be used together with the `periph` (peripheral) example.

## How It Works

1. The central scans for a device named `ESP_ACL_LATENCY`
2. Upon finding the peripheral, it connects and performs MTU exchange
3. After enabling notifications, the central sends 100 test packets (242 bytes each)
4. For each packet, the central records the send time
5. When the echoed packet is received via notification, it calculates RTT
6. Finally, statistics (average, min, max latency, packet loss) are printed

## Test Flow Diagram

```
    ┌──────────────┐                              ┌──────────────┐
    │   Central    │                              │  Peripheral  │
    │  (GATT Client)│                              │ (GATT Server)│
    └──────┬───────┘                              └──────┬───────┘
           │                                             │
           │  ──────────── Connection Setup ───────────  │
           │                                             │
           │  1. Scan for "ESP_ACL_LATENCY"              │
           │ ─────────────────────────────────────────>  │ Advertising
           │                                             │
           │  2. Connect Request                         │
           │ ─────────────────────────────────────────>  │
           │                                             │
           │  3. Connection Established                  │
           │ <─────────────────────────────────────────> │
           │                                             │
           │  4. MTU Exchange (517 bytes)                │
           │ <─────────────────────────────────────────> │
           │                                             │
           │  5. Connection Parameter Update (7.5ms)     │
           │ <─────────────────────────────────────────> │
           │                                             │
           │  6. Service Discovery                       │
           │ ─────────────────────────────────────────>  │
           │                                             │
           │  7. Enable Notification (CCCD)              │
           │ ─────────────────────────────────────────>  │
           │                                             │
           │  ──────────── Latency Test ─────────────    │
           │                                             │
           │  8. Write Test Packet [SEQ=0]    ──┐        │
           │     Record send_time             t1│        │
           │ ─────────────────────────────────────────>  │
           │                                    │        │  Echo back
           │  9. Notification [SEQ=0]           │        │  via Notify
           │     Record recv_time             t2│        │
           │ <─────────────────────────────────────────  │
           │                                    │        │
           │     RTT = t2 - t1                ──┘        │
           │     Latency = RTT / 2                       │
           │                                             │
           │  ... Repeat for SEQ 1-99 ...                │
           │                                             │
           │  10. Print Statistics                       │
           │      - Average/Min/Max Latency              │
           │      - Packet Loss Rate                     │
           │                                             │
    ┌──────┴───────┐                              ┌──────┴───────┐
    │   Central    │                              │  Peripheral  │
    └──────────────┘                              └──────────────┘
```

## Test Configuration

The following parameters can be modified in `gattc_latency.h`:

| Parameter              | Default Value | Description                        |
|------------------------|---------------|------------------------------------|
| TEST_PACKET_COUNT      | 100           | Number of test packets to send     |
| TEST_PACKET_INTERVAL_MS| 20            | Interval between packets (ms)      |

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

1. First, flash the `periph` example to one board:

```bash
cd ../periph
idf.py -p PORT1 flash
```

2. Then, flash this `cent` example to another board:

```bash
cd ../cent
idf.py -p PORT2 flash
```

### Monitor

Run `idf.py -p PORT monitor` to view the serial output of the central device.

(To exit the serial monitor, type `Ctrl-]`.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (362) BLE_ACL_LATENCY_CENT: BLE ACL Latency Test - Central
I (372) phy_init: phy_version 310,dde1ba9,Jun  4 2024,16:38:11
I (432) BLE_ACL_LATENCY_CENT: REG_EVT
I (442) BLE_ACL_LATENCY_CENT: scan start success
I (442) BLE_ACL_LATENCY_CENT: Central initialized, scanning for peripheral...
I (622) BLE_ACL_LATENCY_CENT: searched device ESP_ACL_LATENCY
I (622) BLE_ACL_LATENCY_CENT: connect to the remote device.
I (622) BLE_ACL_LATENCY_CENT: stop scan successfully
I (892) BLE_ACL_LATENCY_CENT: ESP_GATTC_CONNECT_EVT conn_id 0, if 4
I (892) BLE_ACL_LATENCY_CENT: REMOTE BDA: 58:cf:79:ea:77:30
I (892) BLE_ACL_LATENCY_CENT: open success
I (1042) BLE_ACL_LATENCY_CENT: update connection params status = 0, min_int = 6, max_int = 6,conn_int = 6,latency = 0, timeout = 500
I (1112) BLE_ACL_LATENCY_CENT: discover service complete conn_id 0
I (1112) BLE_ACL_LATENCY_CENT: service found
I (1122) BLE_ACL_LATENCY_CENT: ESP_GATTC_CFG_MTU_EVT, Status 0, MTU 517, conn_id 0
I (1132) BLE_ACL_LATENCY_CENT: ESP_GATTC_REG_FOR_NOTIFY_EVT
I (1172) BLE_ACL_LATENCY_CENT: write descr success
I (1172) BLE_ACL_LATENCY_CENT: Initializing latency test...
I (1172) LATENCY_TEST: Latency test initialized, conn_id=0, handle=42
I (2182) BLE_ACL_LATENCY_CENT: Starting latency test...
I (2182) LATENCY_TEST: ====================================
I (2182) LATENCY_TEST: Starting latency test...
I (2182) LATENCY_TEST: Packets: 100, Interval: 20 ms
I (2182) LATENCY_TEST: ====================================
SEQ=  0, RTT= 20040 us, Latency= 10020 us (10.02 ms)
SEQ=  1, RTT= 18341 us, Latency=  9170 us (9.17 ms)
SEQ=  2, RTT= 13339 us, Latency=  6669 us (6.67 ms)
...
SEQ= 99, RTT= 15835 us, Latency=  7917 us (7.92 ms)
I (4192) LATENCY_TEST: All packets sent, waiting for responses...
I (6192) LATENCY_TEST:
I (6192) LATENCY_TEST: ========================================
I (6192) LATENCY_TEST:          Test Results Summary
I (6192) LATENCY_TEST: ========================================
I (6192) LATENCY_TEST: Total packets:     100
I (6202) LATENCY_TEST: Received:          100
I (6202) LATENCY_TEST: Lost:              0 (0.0%)
I (6202) LATENCY_TEST: ----------------------------------------
I (6212) LATENCY_TEST: Average latency:   7929 us (7.93 ms)
I (6222) LATENCY_TEST: Min latency:       6667 us (6.67 ms)
I (6222) LATENCY_TEST: Max latency:       10020 us (10.02 ms)
I (6232) LATENCY_TEST: ========================================
```

## Connection Parameters

The example uses optimized connection parameters for low latency:

| Parameter           | Value    | Description                    |
|---------------------|----------|--------------------------------|
| Connection Interval | 7.5 ms   | Minimum allowed by BLE spec    |
| Slave Latency       | 0        | No skipped connection events   |
| Supervision Timeout | 4000 ms  | Connection timeout             |
| MTU                 | 517      | Maximum ATT MTU                |

## Understanding the Results

- **RTT (Round-Trip Time)**: Time from sending a packet until receiving the echo
- **Latency**: Estimated one-way delay (RTT / 2)
- **Expected Results**: With 7.5ms connection interval, typical latency is 6-10ms

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
