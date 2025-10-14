# BLE Log Console

## Introduction

BLE Log Console is a Python utility for capturing and displaying BLE logs from UART DMA output, which is able to 

- Output normal ASCII logs to console in runtime
- Capture binary BLE logs from UART DMA output in background
- Live status panel showing data transfer statistics
- Automatic detection of frame loss

It would provide users and developers with convenience using this tool when `CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED` is enabled for the purpose of capturing BLE logs.

## Usage

```bash
cd <esp-idf-path>
. ./export.sh
cd tools/bt/ble_log_console
python ble_log_console.py --port <UART_PORT> [--output <OUTPUT_FILE>] [--baudrate <BAUD_RATE>]
```

### Notes

Please check the baud rate of the UART port in menuconfig `CONFIG_BT_BLE_LOG_UHCI_OUT_ENABLED` is enabled. It's recommended to set the baud rate to 921600 or higher to avoid BLE log frame loss.

### Arguments

- `--port` (mandatory): The UART port to connect to (e.g., `/dev/ttyUSB0` on Linux or `COM3` on Windows)
- `--output` (optional): Output filename for binary capture (default: `uart_data.bin`)
- `--baudrate` (optional): Baud rate for UART communication (default: `115200`)
