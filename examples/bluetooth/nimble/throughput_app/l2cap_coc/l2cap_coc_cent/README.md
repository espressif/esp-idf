| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# L2CAP COC Throughput Central Example

`l2cap_coc_cent` demonstrates the central (initiator) side of an L2CAP Connection-Oriented Channel (COC) throughput test using NimBLE on ESP32. It passively scans for a peripheral advertising UUID 0x1812, establishes a GAP connection, enables Data Length Extension (DLE), then opens an L2CAP COC channel over PSM 0x1002 and continuously sends SDUs to measure TX throughput.

The central automatically cycles through all enabled PHYs (1M, 2M, Coded S2, Coded S8) in sequence, printing a throughput summary box after each test interval. It must be used together with the `l2cap_coc_prph` example which acts as the receiving side.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* Two development boards, one flashed with `l2cap_coc_cent` and the other with `l2cap_coc_prph`.
* A USB cable for power supply and programming.

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information.

### Configure the Project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `L2CAP COC Throughput Configuration` menu:

| Option | Default | Description |
|--------|---------|-------------|
| `EXAMPLE_L2CAP_COC_MTU` | 2048 | L2CAP CoC SDU MTU size in bytes (central receive buffer). Data flows cent → prph in this test, so throughput is governed by the peripheral's MTU. This value only limits how much the peripheral can send back and does not affect TX throughput. |
| `EXAMPLE_EXTENDED_ADV` | y (BLE 5.0 chips) | Enable extended scanning to find peripherals using extended advertising. Required for Coded PHY testing on ESP32-C6/H2. |
| `EXAMPLE_TEST_PHY_1M` | n | Enable throughput test on 1M PHY. |
| `EXAMPLE_TEST_PHY_2M` | y | Enable throughput test on 2M PHY (BLE 5.0 chips only). |
| `EXAMPLE_TEST_PHY_CODED_S2` | n | Enable throughput test on Coded PHY S2 (500 kbps, BLE 5.0 chips only). |
| `EXAMPLE_TEST_PHY_CODED_S8` | n | Enable throughput test on Coded PHY S8 (125 kbps, BLE 5.0 chips only). |
| `EXAMPLE_TEST_DURATION_1M` | 8 | Test duration in seconds for 1M PHY. |
| `EXAMPLE_TEST_DURATION_2M` | 8 | Test duration in seconds for 2M PHY. |
| `EXAMPLE_TEST_DURATION_CODED_S2` | 8 | Test duration in seconds for Coded S2 PHY. |
| `EXAMPLE_TEST_DURATION_CODED_S8` | 8 | Test duration in seconds for Coded S8 PHY. |

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

On successful connection and throughput test, the central prints a per-PHY summary box after each test interval, then loops back to the first enabled PHY continuously:

```
I (xxx) l2cap_coc_cent: BLE Host Task started
I (xxx) l2cap_coc_cent: Device Address: xx:xx:xx:xx:xx:xx
I (xxx) l2cap_coc_cent: Connecting to xx:xx:xx:xx:xx:xx (addr_type=0)
I (xxx) l2cap_coc_cent: Connected; handle=0 peer=xx:xx:xx:xx:xx:xx
I (xxx) l2cap_coc_cent: L2CAP COC connected, chan=0xxxxxxxxx
I (xxx) l2cap_coc_cent: L2CAP COC Throughput — TX side (central sends to peripheral)
I (xxx) l2cap_coc_cent: Number of enabled PHYs: x
I (xxx) l2cap_coc_cent: PHY updated: tx=2 rx=2 status=0
I (xxx) l2cap_coc_cent: [2M PHY] Sending for 8 s
I (xxx) l2cap_coc_cent: +-------------------------------------------------+
I (xxx) l2cap_coc_cent: | PHY    : 2M                                    |
I (xxx) l2cap_coc_cent: | TX     : xxxx   kbps                            |
I (xxx) l2cap_coc_cent: | Bytes  : xxxxxxx                                  |
I (xxx) l2cap_coc_cent: | Time   : 8     s                                |
I (xxx) l2cap_coc_cent: +-------------------------------------------------+
I (xxx) l2cap_coc_cent: Cycle complete. Looping back to first PHY...
I (xxx) l2cap_coc_cent: PHY updated: tx=2 rx=2 status=0
I (xxx) l2cap_coc_cent: [2M PHY] Sending for 8 s
I (xxx) l2cap_coc_cent: +-------------------------------------------------+
I (xxx) l2cap_coc_cent: | PHY    : 2M                                    |
I (xxx) l2cap_coc_cent: | TX     : xxxx   kbps                            |
I (xxx) l2cap_coc_cent: | Bytes  : xxxxxxx                                  |
I (xxx) l2cap_coc_cent: | Time   : 8     s                                |
I (xxx) l2cap_coc_cent: +-------------------------------------------------+
I (xxx) l2cap_coc_cent: Cycle complete. Looping back to first PHY...
```

> **Note:** The above output was captured on ESP32-H2 with only 2M PHY enabled. With additional PHYs enabled (1M, Coded S2, Coded S8), the central cycles through each in sequence before looping back.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
