| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BLE throughput GATT CLIENT Test

This is the demo used to test the BLE throughput, this demo should used with throughput server demo together.
The throughput of BLE can up to 720-767 Kbps between to ESP32 board.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

To configure the project, you can follow these steps:

1. In order to maximize throughput, we need to set the uart print baud rate at `921600` or more:
Go to: `idf.py menuconfig --> Component config --> ESP32-specific --> UART console baud rate` and set to `921600` or `1500000` and don't print too much log.
2. We can only test notify or write throughput at the same time, this demo default to test the notify throughput, if want to test the write throughput,
please set: `idf.py menuconfig --> Component config --> Example 'GATT CLIENT THROUGHPUT' Config  --->` then select the `test the gattc write throughput` option.
3. This demo only test unidirectional throughput, if you want to test the bidirectional throughput please change the demo by yourself.
4. Should change the CPU frequency to 160 MHZ or 240 MHz in the `idf.py menuconfig`  and `Component config  ---> ESP32-specific  ---> CPU frequency (240 MHz or 160 MHz)`.
5. Should change the bluetooth controller and Bluedroid run in different Core in the `idf.py menuconfig` on `Component config  ---> Bluetooth  ---> The cpu core which bluetooth controller run (Core 0 (PRO CPU))` and `Bluedroid Enable  ---> The cpu core which Bluedroid run (Core 1 (APP CPU))`.
6. In order to maximize throughput, please test in a clean environment without many BLE devices working and esure both test devices are ESP32.

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-C2/ESP32-H2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (6061) GATTC_DEMO: fc f5 c4 3c 5e ca
I (6061) GATTC_DEMO: searched Adv Data Len 29, Scan Response Len 29
I (6061) GATTC_DEMO: searched Device Name Len 15
I (6061) GATTC_DEMO: THROUGHPUT_DEMO
I (6071) GATTC_DEMO:

I (6071) GATTC_DEMO: searched device THROUGHPUT_DEMO

I (6081) GATTC_DEMO: connect to the remote device.
E (6081) BT_BTM: BTM_BleConfigConnParams

I (6091) GATTC_DEMO: stop scan successfully
I (6361) GATTC_DEMO: ESP_GATTC_CONNECT_EVT conn_id 0, if 1
I (6361) GATTC_DEMO: REMOTE BDA:
I (6361) GATTC_DEMO: fc f5 c4 3c 5e ca
I (6361) GATTC_DEMO: open success
I (6921) GATTC_DEMO: Notify Bit rate = 0 Byte/s, = 0 bit/s
I (7671) GATTC_DEMO: ESP_GATTC_CFG_MTU_EVT, Status 0, MTU 517, conn_id 0
I (7671) GATTC_DEMO: ESP_GATTC_SEARCH_RES_EVT
I (7671) GATTC_DEMO: service found
I (7681) GATTC_DEMO: UUID16: ff
I (7681) GATTC_DEMO: ESP_GATTC_SEARCH_CMPL_EVT
I (7691) GATTC_DEMO: ESP_GATTC_REG_FOR_NOTIFY_EVT
I (7831) GATTC_DEMO: write descr success
I (8921) GATTC_DEMO: Notify Bit rate = 87534 Byte/s, = 700272 bit/s, time = 1s
I (10921) GATTC_DEMO: Notify Bit rate = 82174 Byte/s, = 657392 bit/s, time = 3s
I (12921) GATTC_DEMO: Notify Bit rate = 81480 Byte/s, = 651840 bit/s, time = 5s
I (14921) GATTC_DEMO: Notify Bit rate = 77054 Byte/s, = 616432 bit/s, time = 7s
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
