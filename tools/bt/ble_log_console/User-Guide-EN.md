# BLE Log Console User Guide

## Introduction

BLE Log Console is a terminal-based tool for real-time capture and parsing of BLE Log frames from ESP chip firmware via UART DMA. It displays parsed frames in an interactive TUI and saves the raw binary data to a file for offline analysis.

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
Component config → Bluetooth → BT Logs → Enable BT Log Async Output (Dev Only)  [y]
```

UART DMA transport, 3 Mbps baud rate, and PORT 0 are all enabled by default -- no further configuration needed in most cases.

> **About UART PORT 0**: When configured for PORT 0, the firmware automatically wraps `ESP_LOG` output in BLE Log frames (`REDIR` source). The console decodes and displays these as regular log lines.

### 2. Hardware Connection

**PORT 0 (recommended):** Simply connect the development board's UART port via USB — no additional wiring needed.

**Other PORTs:** Connect the designated TX GPIO to an external USB-to-serial adapter:

```
ESP32 TX GPIO ──────── USB-Serial RX
ESP32 GND     ──────── USB-Serial GND
```

Ensure your USB-serial adapter supports the configured baud rate (3 Mbps by default). Adapters based on CP2102N, CH343, or FT232H are recommended.

## Getting Started

### Quick Start (Recommended)

Use the provided launcher script -- it automatically activates the ESP-IDF environment and installs dependencies. Can be run from any directory:

```bash
# Linux / macOS
<esp-idf-root>/tools/bt/ble_log_console/run.sh

# Windows
<esp-idf-root>\tools\bt\ble_log_console\run.bat
```

When launched without arguments, the tool opens a **Launch Screen** -- an interactive configuration interface where you can:

- **Select a serial port** from a dropdown of auto-detected devices (with a **Refresh** button to re-scan)
- **Choose a baud rate** from preset options (115200 to 3000000, default: 3000000)
- **Set the log directory** via text input or a **Browse** file picker
- Press **Connect** to start capture

To skip the Launch Screen and start capture directly, pass `--port`:

```bash
# Linux / macOS
./run.sh -p /dev/ttyUSB0
./run.sh -p /dev/ttyUSB0 -b 3000000 -d /tmp/my_captures

# Windows
run.bat -p COM3
```

All CLI options are forwarded to `console.py`.

### Manual Launch

If you prefer to manage the ESP-IDF environment yourself:

```bash
cd <esp-idf-root>
. ./export.sh
python -m pip install textual textual-fspicker      # Install extra dependencies
cd tools/bt/ble_log_console
python console.py                                  # Launch Screen
python console.py -p /dev/ttyUSB0                  # Direct capture
python console.py -p /dev/ttyUSB0 -b 3000000       # Custom baud rate
python console.py -p /dev/ttyUSB0 -d /tmp/captures  # Custom log directory
```

### CLI Options

| Option | Short | Default | Description |
|--------|-------|---------|-------------|
| `--port` | `-p` | (optional) | Serial port path, e.g., `/dev/ttyUSB0`, `COM3`. Omit to use Launch Screen. |
| `--baudrate` | `-b` | `3000000` | Baud rate -- must match `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE` |
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
Sync: SYNCED | Checksum: XOR / Header+Payload | Press h for help
RX: 1.2 MB  Speed: 293.0 KB/s  Max: 300.0 KB/s  Rate: 3421 fps  Lost: 12 frames, 480 B
```

- **Sync**: Sync state (SEARCHING -> CONFIRMING -> SYNCED -> CONFIRMING_LOSS)
- **Checksum**: Auto-detected checksum mode (algorithm / scope)
- **RX**: Total received bytes
- **Speed / Max**: Current / peak transport speed (in KB/s or MB/s)
- **Rate**: Current frame rate
- **Lost**: Cumulative lost frames and bytes since console start

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `q` | Quit |
| `Ctrl+C` | Quit |
| `c` | Clear the log view |
| `s` | Toggle auto-scroll (on by default) |
| `d` | Open per-source frame statistics (press `Escape` or `d` to close) |
| `m` | Show buffer utilization (press `Escape` or `m` to close) |
| `h` | Show keyboard shortcuts (press `Escape` to close) |
| `r` | Reset chip via DTR/RTS toggle |

### Frame Statistics Detail (`d` key)

Pressing `d` opens a modal overlay with two tables, refreshed every second:

**Firmware Counters (since chip init)** -- per-source write and buffer loss counts as reported by the firmware:

| Source | Written Frames | Written Bytes | Buffer Loss Frames | Buffer Loss Bytes |
|--------|---------------|---------------|-------------------|-------------------|
| LL_TASK | 12345 | 56.7 KB | 5 | 200 B |
| LL_HCI | 890 | 34.2 KB | - | - |
| HOST | 456 | 12.1 KB | - | - |
| HCI | 234 | 8.5 KB | 2 | 80 B |

**Console Measurements (since console start)** -- per-source receive rates and peak bursts measured by the console:

| Source | Received Frames | Received Bytes | Avg Frames/s | Avg Bytes/s | Peak Frames/10ms | Peak Bytes/s |
|--------|----------------|---------------|-------------|------------|-----------------|-------------|
| LL_TASK | 12340 | 56.5 KB | 412 | 18.8 KB/s | 8 | 24.0 KB/s |
| LL_HCI | 890 | 34.2 KB | 30 | 1.1 KB/s | 3 | 3.6 KB/s |

Sources with buffer loss are highlighted in red in the firmware table.

### Buffer Utilization (`m` key)

Pressing `m` opens a modal overlay showing per-LBM (Log Buffer Manager) buffer utilization as reported by the firmware:

| Pool | Idx | Name | Peak | Total | Util% |
|------|-----|------|------|-------|-------|
| COMMON_TASK | 0 | spin | 3 | 4 | 75% |
| COMMON_TASK | 1 | atomic[0] | 4 | 4 | 100% |
| COMMON_ISR | 0 | spin | 2 | 4 | 50% |
| LL | 0 | ll_task | 4 | 4 | 100% |
| LL | 1 | ll_hci | 2 | 4 | 50% |

- **Pool**: Buffer pool category (COMMON_TASK, COMMON_ISR, LL, REDIR)
- **Idx**: LBM index within the pool
- **Name**: Human-readable LBM name (spin, atomic[N], ll_task, ll_hci, redir)
- **Peak**: Maximum number of transport buffers in flight simultaneously
- **Total**: Total transport buffers available for this LBM
- **Util%**: Peak / Total as percentage; 100% (highlighted in red) means all buffers were in use simultaneously, indicating potential frame loss

This helps diagnose which buffer pool is under-provisioned when experiencing frame loss.

## Building Standalone Executable

Use the provided build script to package BLE Log Console as a single-file executable that requires no Python or ESP-IDF environment on the target machine:

```bash
# Linux / macOS
<esp-idf-root>/tools/bt/ble_log_console/build.sh

# Windows
<esp-idf-root>\tools\bt\ble_log_console\build.bat
```

The script automatically activates the ESP-IDF environment, installs PyInstaller, builds the executable, places it in the current working directory, and cleans up intermediate artifacts.

Output: `./ble_log_console` (Linux/macOS) or `.\ble_log_console.exe` (Windows).

> **Note**: Build on the same OS/architecture as the target machine. PyInstaller does not cross-compile.

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
- Press `m` to check buffer utilization -- pools at 100% need more buffers
- Increase firmware buffers: `CONFIG_BLE_LOG_LBM_TRANS_BUF_SIZE`, `CONFIG_BLE_LOG_LBM_LL_TRANS_BUF_SIZE`
- Add more LBMs: `CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT`
- Increase baud rate (if your adapter supports it)

### No ESP_LOG output

- Confirm the firmware has `CONFIG_BLE_LOG_PRPH_UART_DMA_PORT=0`
- The console decodes REDIR frames automatically — no extra configuration needed
- Logs are flushed by a 1-second periodic timer, so there may be a short delay


