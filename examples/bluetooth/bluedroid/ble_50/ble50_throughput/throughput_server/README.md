| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BLE 50 throughput GATT SERVER Test

This is the demo used to test the BLE throughput, this demo should used with throughput client demo together.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```
To configure the project, you can follow these steps:

1. We can only test notify or write throughput at the same time, this demo default to test the notify throughput, if want to test the write throughput,
please set: `idf.py menuconfig --> Component config --> Example 'GATT SERVER THROUGHPUT' Config --->` then select the `test the gattc write throughput` option.
2. This demo only test unidirectional throughput, if you want to test the bidirectional throughput please change the demo by yourself.
3. Should change the CPU frequency to 160 MHZ in the `idf.py menuconfig` and `Component config ---> ESP System Settings ---> CPU frequency (160 MHz)`.
4. In order to maximize throughput, please test in a clean environment without many BLE devices working and esure both test devices are ESP32 series.

### Hardware Required

* A development board with supported SoC (e.g., ESP32-C3-DevKitM-1, ESP32-C6-DevKitC-1, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (377) main_task: Started on CPU0
I (377) main_task: Calling app_main()
I (377) BLE_INIT: BT controller compile version [d752dea]
I (377) BLE_INIT: Bluetooth MAC: ec:da:3b:0f:2d:b6
I (377) phy_init: phy_version 1180,01f2a49,Jun  4 2024,16:34:25
I (437) GATTS_DEMO_PHY: GATT server register, status 0, app_id 0
I (437) GATTS_DEMO_PHY: Extended advertising params set, status 0
I (437) GATTS_DEMO_PHY: Service create, status 0, service_handle 40
I (447) GATTS_DEMO_PHY: Extended advertising data set, status 0
I (447) GATTS_DEMO_PHY: Service start, status 0, service_handle 40
I (457) GATTS_DEMO_PHY: Characteristic add, status 0, attr_handle 42, service_handle 40
I (467) GATTS_DEMO_PHY: the gatts demo char length = 3
I (467) GATTS_DEMO_PHY: prf_char[0] =11
I (477) GATTS_DEMO_PHY: prf_char[1] =22
I (477) GATTS_DEMO_PHY: prf_char[2] =33
I (477) GATTS_DEMO_PHY: Extended advertising start, status 0
I (487) GATTS_DEMO_PHY: Descriptor add, status 0, attr_handle 43, service_handle 40
I (497) main_task: Returned from app_main()
I (8957) GATTS_DEMO_PHY: Extended advertising terminated, status 0
I (8957) GATTS_DEMO_PHY: Advertising successfully ended with a connection being created
I (9087) GATTS_DEMO_PHY: Connected, conn_id 0, remote 68:67:25:4d:11:fa
I (13117) GATTS_DEMO_PHY: MTU exchange, MTU 517
I (13637) GATTS_DEMO_PHY: Notification enable
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
