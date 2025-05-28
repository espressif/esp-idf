| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BLE 50 throughput GATT CLIENT Test

This is the demo used to test the BLE 5.0 throughput, this demo should used with throughput server demo together.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

To configure the project, you can follow these steps:

1. We can only test notify or write throughput at the same time, this demo default to test the notify throughput, if want to test the write throughput,
please set: `idf.py menuconfig --> Component config --> Example 'GATT CLIENT THROUGHPUT' Config --->` then select the `test the gattc write throughput` option.
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

### Throughput on 1M PHY
```
I (458) GATTC_DEMO_PHY: Legacy adv, adv type 0x13 data len 31
I (458) GATTC_DEMO_PHY: Extend adv, adv type 0x1 data len 67
I (458) GATTC_DEMO_PHY: searched device THROUGHPUT_PHY_DEMO
I (458) GATTC_DEMO_PHY: Device found ec:da:3b:0f:2d:b6
I (468) Adv name: THROUGHPUT_PHY_D
I (468) Adv name: EMO
I (468) GATTC_DEMO_PHY: Stop extend scan and create aux open, primary_phy 1 secondary phy 1
I (488) GATTC_DEMO_PHY: Scanning stop successfully
I (768) GATTC_DEMO_PHY: Connected, conn_id 0, remote ec:da:3b:0f:2d:b6
I (768) GATTC_DEMO_PHY: Open successfully, MTU 23
I (778) GATTC_DEMO_PHY: Read PHY, status 0, TX_PHY 1, RX_PHY 1
I (4928) GATTC_DEMO_PHY: MTU exchange, status 0, MTU 517
I (4928) GATTC_DEMO_PHY: Service search result
I (4928) GATTC_DEMO_PHY: Service found
I (4928) GATTC_DEMO_PHY: UUID16: ff
I (4938) GATTC_DEMO_PHY: Service search complete
I (4938) GATTC_DEMO_PHY: Notification register successfully
I (5448) GATTC_DEMO_PHY: Descriptor write successfully
I (6438) GATTC_DEMO_PHY: Notify Bit rate = 97642 Byte/s, = 781136 bit/s, time = 0s
I (8438) GATTC_DEMO_PHY: Notify Bit rate = 97390 Byte/s, = 779120 bit/s, time = 2s
I (10438) GATTC_DEMO_PHY: Notify Bit rate = 97233 Byte/s, = 777864 bit/s, time = 4s
I (12438) GATTC_DEMO_PHY: Notify Bit rate = 97178 Byte/s, = 777424 bit/s, time = 6s
I (14438) GATTC_DEMO_PHY: Notify Bit rate = 97142 Byte/s, = 777136 bit/s, time = 8s
```

### Throughput on 2M PHY
```
I (648) GATTC_DEMO_PHY: Read PHY, status 0, TX_PHY 2, RX_PHY 2
I (4798) GATTC_DEMO_PHY: MTU exchange, status 0, MTU 517
I (4798) GATTC_DEMO_PHY: Service search result
I (4798) GATTC_DEMO_PHY: Service found
I (4798) GATTC_DEMO_PHY: UUID16: ff
I (4808) GATTC_DEMO_PHY: Service search complete
I (4808) GATTC_DEMO_PHY: Notification register successfully
I (5318) GATTC_DEMO_PHY: Descriptor write successfully
I (6448) GATTC_DEMO_PHY: Notify Bit rate = 175369 Byte/s, = 1402952 bit/s, time = 1s
I (8448) GATTC_DEMO_PHY: Notify Bit rate = 175270 Byte/s, = 1402160 bit/s, time = 3s
I (10448) GATTC_DEMO_PHY: Notify Bit rate = 175251 Byte/s, = 1402008 bit/s, time = 5s
I (12448) GATTC_DEMO_PHY: Notify Bit rate = 175242 Byte/s, = 1401936 bit/s, time = 7s
I (14448) GATTC_DEMO_PHY: Notify Bit rate = 175192 Byte/s, = 1401536 bit/s, time = 9s
```

### Throughput on 500K PHY (Coded S2)
```
I (908) GATTC_DEMO_PHY: Read PHY, status 0, TX_PHY 3, RX_PHY 3
I (5058) GATTC_DEMO_PHY: MTU exchange, status 0, MTU 517
I (5058) GATTC_DEMO_PHY: Service search result
I (5058) GATTC_DEMO_PHY: Service found
I (5068) GATTC_DEMO_PHY: UUID16: ff
I (5068) GATTC_DEMO_PHY: Service search complete
I (5068) GATTC_DEMO_PHY: Notification register successfully
I (5578) GATTC_DEMO_PHY: Descriptor write successfully
I (6468) GATTC_DEMO_PHY: Notify Bit rate = 44292 Byte/s, = 354336 bit/s, time = 0s
I (8468) GATTC_DEMO_PHY: Notify Bit rate = 43961 Byte/s, = 351688 bit/s, time = 2s
I (10468) GATTC_DEMO_PHY: Notify Bit rate = 43898 Byte/s, = 351184 bit/s, time = 4s
I (12468) GATTC_DEMO_PHY: Notify Bit rate = 43871 Byte/s, = 350968 bit/s, time = 6s
I (14468) GATTC_DEMO_PHY: Notify Bit rate = 43857 Byte/s, = 350856 bit/s, time = 8s
```

### Throughput on 125K PHY (Coded S8)
```
I (778) GATTC_DEMO_PHY: Read PHY, status 0, TX_PHY 3, RX_PHY 3
I (4928) GATTC_DEMO_PHY: MTU exchange, status 0, MTU 517
I (4938) GATTC_DEMO_PHY: Service search result
I (4938) GATTC_DEMO_PHY: Service found
I (4938) GATTC_DEMO_PHY: UUID16: ff
I (4938) GATTC_DEMO_PHY: Service search complete
I (4938) GATTC_DEMO_PHY: Notification register successfully
I (5448) GATTC_DEMO_PHY: Descriptor write successfully
I (6448) GATTC_DEMO_PHY: Notify Bit rate = 13459 Byte/s, = 107672 bit/s, time = 0s
I (8448) GATTC_DEMO_PHY: Notify Bit rate = 13395 Byte/s, = 107160 bit/s, time = 2s
I (10448) GATTC_DEMO_PHY: Notify Bit rate = 13383 Byte/s, = 107064 bit/s, time = 4s
I (12448) GATTC_DEMO_PHY: Notify Bit rate = 13378 Byte/s, = 107024 bit/s, time = 6s
I (14448) GATTC_DEMO_PHY: Notify Bit rate = 13375 Byte/s, = 107000 bit/s, time = 8s
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
