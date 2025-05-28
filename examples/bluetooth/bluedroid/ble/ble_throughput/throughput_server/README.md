| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BLE throughput GATT SERVER Test

This is the demo used to test the BLE throughput, this demo should used with throughput client demo together.
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
4. Should change the CPU frequency to 160 MHz or 240 MHz in the `idf.py menuconfig`  and `Component config  ---> ESP32-specific  ---> CPU frequency (240 MHz or 160 MHz)`.
5. Should change the bluetooth controller and Bluedroid run in different Core in the `idf.py menuconfig` on `Component config  ---> Bluetooth  ---> The cpu core which bluetooth controller run (Core 0 (PRO CPU))` and `Bluedroid Enable  ---> The cpu core which Bluedroid run (Core 1 (APP CPU))`.
6. In order to maximize throughput, please test in a clean environment without many BLE devices working and both test devices are ESP32.

### Hardware Required

* A development board with supported SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (498) BTDM_INIT: BT controller compile version [d03a5d3]
I (498) system_api: Base MAC address is not set
I (498) system_api: read default base MAC address from EFUSE
I (508) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (918) GATTS_DEMO: REGISTER_APP_EVT, status 0, app_id 0

W (918) BT_BTM: BTM_BleWriteAdvData, Partial data write into ADV
W (918) BT_BTM: BTM_BleWriteScanRsp, Partial data write into ADV
I (928) GATTS_DEMO: CREATE_SERVICE_EVT, status 0,  service_handle 40

I (948) GATTS_DEMO: SERVICE_START_EVT, status 0, service_handle 40

I (948) GATTS_DEMO: ADD_CHAR_EVT, status 0,  attr_handle 42, service_handle 40

I (948) GATTS_DEMO: the gatts demo char length = 3

I (958) GATTS_DEMO: prf_char[0] =11

I (958) GATTS_DEMO: prf_char[1] =22

I (968) GATTS_DEMO: prf_char[2] =33

I (968) GATTS_DEMO: ADD_DESCR_EVT, status 0, attr_handle 43, service_handle 40

I (6258) GATTS_DEMO: ESP_GATTS_CONNECT_EVT, conn_id 0, remote ac:67:b2:6d:4e:22:
I (7538) GATTS_DEMO: ESP_GATTS_MTU_EVT, MTU 517
I (7698) GATTS_DEMO: GATT_WRITE_EVT, conn_id 0, trans_id 2, handle 43
I (7698) GATTS_DEMO: GATT_WRITE_EVT, value len 2, value :
I (7698) GATTS_DEMO: 01 00
I (7698) GATTS_DEMO: notify enable
I (7708) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7718) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7718) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7728) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7728) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7738) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7738) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7748) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7748) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
I (7758) GATTS_DEMO: ESP_GATTS_CONF_EVT, status 0
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
