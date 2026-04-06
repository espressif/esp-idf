| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# L2CAP COC Throughput Peripheral Example

`l2cap_coc_prph` demonstrates the peripheral side of an L2CAP Connection-Oriented Channel (COC) throughput test using NimBLE on ESP32. It advertises with UUID 0x1812, accepts an incoming GAP connection from `l2cap_coc_cent`, registers an L2CAP COC server on PSM 0x1002, and measures RX throughput as the central sends SDUs.

The peripheral tracks throughput per PHY — each time the central switches PHY, the peripheral prints a per-PHY throughput summary box and resets its counters. A background stats task also prints a live per-second RX rate while data is flowing. It must be used together with the `l2cap_coc_cent` example which acts as the sending side.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* Two development boards, one flashed with `l2cap_coc_prph` and the other with `l2cap_coc_cent`.
* A USB cable for power supply and programming.

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information.

### Configure the Project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `L2CAP COC Throughput Configuration` menu:

| Option | Default | Description |
|---------|---------|-------------|
| `EXAMPLE_L2CAP_COC_MTU` | `16384` | Peripheral L2CAP CoC SDU MTU size in bytes. |
| `EXAMPLE_EXTENDED_ADV` | `y` (BLE 5.0 chips) | Enable extended advertising for BLE 5.0 capable devices. Required for Coded PHY testing on ESP32-C6 and ESP32-H2. |

> **Note:** Throughput in the central → peripheral direction is primarily determined by the peripheral MTU. With the default configuration (`MTU=16384`, `MPS=247`), NimBLE grants approximately 67 initial credits to the central sender, allowing multiple packets to remain in flight and maximizing link throughput.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

On successful connection and data reception, the peripheral prints a live per-second RX rate while data flows:

```
I (xxx) l2cap_coc_prph: BLE Host Task started
I (xxx) l2cap_coc_prph: Device Address: xx:xx:xx:xx:xx:xx
I (xxx) l2cap_coc_prph: Extended advertising started
I (xxx) l2cap_coc_prph: Connected; handle=0
I (xxx) l2cap_coc_prph: L2CAP COC connected, chan=0xxxxxxxxx
I (xxx) l2cap_coc_prph: PHY updated: tx=2 rx=2 status=0
I (xxx) l2cap_coc_prph: | RX : xxxx   kbps |
I (xxx) l2cap_coc_prph: | RX : xxxx   kbps |
I (xxx) l2cap_coc_prph: | RX : xxxx   kbps |
I (xxx) l2cap_coc_prph: | RX : xxxx   kbps |
I (xxx) l2cap_coc_prph: | RX : xxxx   kbps |
I (xxx) l2cap_coc_prph: | RX : xxxx   kbps |
I (xxx) l2cap_coc_prph: | RX : xxxx   kbps |
I (xxx) l2cap_coc_prph: | RX : xxxx   kbps |
```

> **Note:** The peripheral prints one RX line per second. The central controls PHY selection and test duration; the peripheral tracks and displays throughput continuously as long as data is flowing.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
