# BLE Log Console User Guide

## Introduction

BLE Log Console is a terminal-based tool for real-time capture and parsing of BLE Log frames from ESP32 firmware via UART DMA. It displays parsed frames in an interactive TUI and saves the raw binary data to a file for offline analysis.

## Prerequisites

### 1. Firmware Configuration

Enable the BLE Log module in `idf.py menuconfig`:

**Quick setup (recommended):**

```
Component config → Bluetooth → BT Logs → Enable critical-log-only mode  [y]
```

This enables the BLE Log module, selects UART DMA as the transport, and restricts each stack to critical logs only — all in one toggle.

**Manual configuration:**

```
Component config → Bluetooth → BT Logs → Enable BLE Log Module (Experimental)  [y]
Component config → Bluetooth → BT Logs → BLE Log Module
  → Peripheral Selection → UART DMA
  → UART DMA Configuration
      → UART Port Number        (default: 0)
      → Baud Rate               (default: 3000000)
      → TX GPIO Number          (set to match your hardware)
```

| Config Option | Recommended | Description |
|---------------|-------------|-------------|
| `CONFIG_BT_LOG_CRITICAL_ONLY` | `y` | One-click setup — enables BLE Log + UART DMA |
| `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE` | `3000000` | 3 Mbps — balances throughput and reliability |

> **About UART PORT 0**: When configured for PORT 0, the firmware automatically wraps `ESP_LOG` output in BLE Log frames (`REDIR` source). The console decodes and displays these as regular log lines.

### 2. Hardware Connection

**PORT 0 (recommended):** Simply connect the development board's UART port via USB — no additional wiring needed.

**Other PORTs:** Connect the designated TX GPIO to an external USB-to-serial adapter:

```
ESP32 TX GPIO ──────── USB-Serial RX
ESP32 GND     ──────── USB-Serial GND
```

Ensure your USB-serial adapter supports the configured baud rate (3 Mbps by default). Adapters based on CP2102N, CH343, or FT232H are recommended.

### 3. ESP-IDF Environment

Source the ESP-IDF environment before use:

```bash
cd <esp-idf-root>
. ./export.sh
```

No additional installation is needed — `textual` is included in ESP-IDF's core dependencies.

## Getting Started

```bash
cd <esp-idf-root>
. ./export.sh
cd tools/bt/ble_log_console
```

### Interactive Mode (Launch Screen)

Run with no arguments to open the Launch Screen — a TUI where you can select the serial port, baud rate, and log directory before starting capture:

```bash
python console.py
```

### Capture Mode (CLI)

Pass `--port` to skip the Launch Screen and start capture immediately:

```bash
# Basic usage
python console.py -p /dev/ttyUSB0

# With custom baud rate (must match firmware config)
python console.py -p /dev/ttyUSB0 -b 3000000

# With custom log directory
python console.py -p /dev/ttyUSB0 -d /tmp/my_captures
```

| Option | Short | Default | Description |
|--------|-------|---------|-------------|
| `--port` | `-p` | (optional) | Serial port path, e.g., `/dev/ttyUSB0`, `COM3`. Omit to use Launch Screen. |
| `--baudrate` | `-b` | `3000000` | Baud rate — must match `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE` |
| `--log-dir` | `-d` | current working directory | Directory where capture `.bin` files are saved |

Capture files are saved to the current working directory (or `--log-dir` if specified) with a timestamp-based filename:

```
ble_log_YYYYMMDD_HHMMSS.bin
```

## UI Overview

The interface has two areas:

### Log View (upper area)

A scrollable area displaying real-time logs:

- **`[INFO]`** (green): System information, e.g., firmware version
- **`[WARN]`** (yellow): Frame loss warnings, e.g., `Frame loss [LL_TASK]: 5 frames, 200 bytes`, indicating new frame loss on that source
- **`[SYNC]`** (cyan): Sync state transitions
- Plain text: `ESP_LOG` output via UART redirect (PORT 0 only)

### Status Panel (bottom bar)

Fixed at the bottom, updated in real time:

```
Sync: SYNCED  Checksum: XOR | Header+Payload
RX: 1.2 MB  Speed: 2.8 Mbps  Max: 2.9 Mbps  Rate: 3421 fps
Frame Loss: 12 frames, 480 bytes
```

- **Sync**: Sync state (SEARCHING → CONFIRMING → SYNCED → CONFIRMING_LOSS)
- **Checksum**: Auto-detected checksum mode
- **RX**: Total received bytes
- **Speed / Max**: Current / peak transport speed
- **Rate**: Current frame rate
- **Frame Loss**: Cumulative lost frames and bytes since console start

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `q` | Quit |
| `Ctrl+C` | Quit |
| `c` | Clear the log view |
| `s` | Toggle auto-scroll (on by default) |
| `d` | Open per-source frame statistics (press `Escape` or `d` to close) |
| `h` | Show keyboard shortcuts (press `Escape` to close) |
| `r` | Reset chip via DTR/RTS toggle |

### Frame Statistics Detail (`d` key)

Pressing `d` opens a modal overlay showing per-source write and loss statistics since the console started:

```
┌─────────── Per-Source Frame Statistics (since console start) ───────────┐
│ Source    │ Written Frames │ Written Bytes │ Lost Frames │ Lost Bytes   │
│ LL_TASK   │          12345 │       56.7 KB │           5 │        200 B │
│ LL_HCI    │            890 │       34.2 KB │           0 │          0 B │
│ HOST      │            456 │       12.1 KB │           0 │          0 B │
│ HCI       │            234 │        8.5 KB │           2 │         80 B │
└─────────────────────────────────────────────────────────────────────────┘
```

Rows with frame loss are highlighted in red.

## Viewing Saved Captures

```bash
python console.py ls
```

Example output:

```
Captures in /tmp/ble_log_console:

  2026-03-17 14:30:25     2.3 MB  ble_log_20260317_143025.bin
  2026-03-17 10:15:03   512.0 KB  ble_log_20260317_101503.bin
```

These `.bin` files contain raw binary data as received from UART. They can be parsed offline using the BLE Log Analyzer's `ble_log_parser_v2` module for HCI log extraction, btsnoop conversion, and more.

## Troubleshooting

### Sync stays in SEARCHING

- **Baud rate mismatch**: Ensure `--baudrate` matches `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE`
- **Wrong port**: Verify you are connected to the correct UART TX pin
- **Firmware not running**: Confirm the device has booted and BLE Log is initialized
- **Signal quality**: At 3 Mbps, use short cables and ensure a solid GND connection

### Buffer overflow warning

This means the parser's internal buffer exceeded 8 KB without successfully parsing any frames. Common causes:

- Device is still booting and no valid BLE Log frames have been sent yet
- Baud rate mismatch causing all received data to be garbage

If it only appears once at startup, this is normal. If persistent, check the baud rate and hardware connection.

### High frame loss

- Press `d` to view per-source loss details
- Increase firmware buffers: `CONFIG_BLE_LOG_LBM_TRANS_SIZE`, `CONFIG_BLE_LOG_LBM_LL_TRANS_SIZE`
- Add more LBMs: `CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT`
- Increase baud rate (if your adapter supports it)

### No ESP_LOG output

- Confirm the firmware has `CONFIG_BLE_LOG_PRPH_UART_DMA_PORT=0`
- The console decodes REDIR frames automatically — no extra configuration needed
- Logs are flushed by a 1-second periodic timer, so there may be a short delay

### `ModuleNotFoundError: No module named 'textual'`

Re-run the ESP-IDF installer:

```bash
cd <esp-idf-root>
./install.sh
. ./export.sh
```
