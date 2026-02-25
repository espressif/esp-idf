| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# Async CRC Console Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates how to use the Async CRC driver (`esp_async_crc.h`) through an interactive console CLI. The Async CRC driver provides hardware-accelerated CRC calculation using the General DMA (GDMA) peripheral.

The example provides a `crc` command that allows users to:
- Calculate CRC-8, CRC-16, and CRC-32 checksums
- Customize CRC parameters (polynomial, initial value, final XOR, bit reversal)
- Test various CRC algorithms interactively

## Hardware Required

Any board with a supported ESP target that mentioned in the above table can be used.

## Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Check help

```bash
crc> help
crc  [--width=<width>] [--poly=<hex>] [--init=<hex>] [--xor=<hex>] [--reverse-input=<0|1>] [--reverse-output=<0|1>] <data>
  Calculate CRC checksum using hardware async CRC driver
  --width=<width>  CRC width (8, 16, or 32). Default: 8
  --poly=<hex>  CRC polynomial in hex. Default: 0x07
  --init=<hex>  Initial CRC value in hex. Default: 0x00
  --xor=<hex>  Final XOR value in hex. Default: 0x00
  --reverse-input=<0|1>  Reverse input bits (0 or 1). Default: 0
  --reverse-output=<0|1>  Reverse output bits (0 or 1). Default: 0
  <data>  Input data string
```

### Calculate CRC-8 (default)

```bash
crc> crc "test"
CRC result: 0xB9
```

### Calculate CRC-16/CCITT

```bash
crc> crc --width 16 --poly 0x1021 "test"
CRC result: 0x9B06
```

### Calculate CRC-32

```bash
crc> crc --width 32 --poly 0x04C11DB7 --init 0xFFFFFFFF --xor 0xFFFFFFFF --reverse-input 1 --reverse-output 1 "test"
CRC result: 0xD87F7E0C
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
