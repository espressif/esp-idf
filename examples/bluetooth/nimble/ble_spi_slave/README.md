| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# NimBLE SPI Slave Receiver Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example configures the device as an **SPI slave receiver** and prints received data to UART in hexadecimal format.

It is intended to work with a peer device acting as an SPI master (for example, a NimBLE BLE Log sender). Data is received on SPI and projected to the serial monitor, which makes this example useful for bring-up and transport validation.

### What this example does

- Initializes SPI slave mode (`SPI2_HOST`)
- Uses MOSI/SCLK/CS lines to receive data in half-duplex style
- Keeps MISO disabled (`-1`)
- Receives transactions in 1024-byte DMA buffers
- Prints received bytes to UART in 32-byte lines (`%02X` format)

## Hardware behavior

The receive pins are configured from Kconfig symbols used by this example:

- `CONFIG_BT_BLE_LOG_SPI_OUT_MOSI_IO_NUM`
- `CONFIG_BT_BLE_LOG_SPI_OUT_SCLK_IO_NUM`
- `CONFIG_BT_BLE_LOG_SPI_OUT_CS_IO_NUM`

Defined in [main/ble_spi_slave.h](main/ble_spi_slave.h).

The example enables pull-ups on MOSI/SCLK/CS to reduce false activity when no master is connected.

## Hardware connection

Connect your SPI master device to the example target:

- Master MOSI -> Slave MOSI
- Master SCLK -> Slave SCLK
- Master CS   -> Slave CS
- GND <-> GND

Notes:

- MISO is not used by this example.
- SPI mode is fixed to mode 0 in current code.
- Ensure both devices share a common ground.

## How to Use Example

Before project configuration and build, set the target:

```bash
idf.py set-target <chip_name>
```

### Configure the project

Open configuration menu:

```bash
idf.py menuconfig
```

Configure SPI pin assignments used by this example via Bluetooth/BLE Log SPI output configuration (MOSI/SCLK/CS), so they match your wiring.

### Build and flash

```bash
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type `Ctrl-]`.)

## Expected output

When the SPI master sends data, this example prints the received bytes as hexadecimal text, for example:

```text
08 00 07 4F 01 00 CC CA 18 00 ...
```

Output is line-broken every 32 bytes for readability in the monitor.

## Typical use with BLE Log pipelines

This receiver can be used as the SPI-side capture endpoint in BLE Log transport testing:

1. Master device sends BLE Log frame bytes over SPI.
2. This example receives and prints them on UART.
3. Host tools can capture UART hex and decode with BLE log decompression scripts.

## Limitations

- This example does not decode BLE payloads; it only receives and prints raw bytes.
- Transaction buffer size is fixed at 1024 bytes in current source.

## Troubleshooting

- **No data printed**
	- Verify wiring and shared ground.
	- Verify pin configuration in menuconfig matches your board wiring.
	- Verify SPI master is actively transmitting and uses mode 0.

- **Corrupted or unstable output**
	- Check signal integrity and clock speed.
	- Confirm pull-ups and board-level electrical compatibility.

- **Build errors related to drivers**
	- Ensure SPI/GPIO driver components are enabled (already declared in this example component dependencies).

