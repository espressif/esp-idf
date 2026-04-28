# BLE Log Console

A Textual-based TUI tool for real-time capture, parsing, and display of BLE Log frames from UART DMA output. Designed for both Espressif internal developers and ESP-IDF customers.

**User Guide**: [English](docs/User-Guide-EN.md) | [中文](docs/User-Guide-CN.md)

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
- [Firmware Configuration](#firmware-configuration)
- [How It Works](#how-it-works)
- [Offline Analysis](#offline-analysis)
- [Keyboard Shortcuts](#keyboard-shortcuts)
- [Building Executable](#building-executable)
- [Architecture](#architecture)
- [Development](#development)
- [Troubleshooting](#troubleshooting)

## Features

- **Real-time frame parsing** with automatic checksum mode detection (XOR/Sum × Full/Header-only)
- **Frame sync state machine** with loss detection and recovery (SEARCHING → CONFIRMING → SYNCED → CONFIRMING_LOSS)
- **Internal frame decoding**: INIT_DONE (firmware version), INFO, ENH_STAT (per-source write/loss counters), FLUSH
- **UART redirect display**: When firmware uses UART PORT 0, redirected `ESP_LOG` output is decoded from `REDIR` frames and displayed as ASCII log lines
- **Dimmed internal logs**: Console-generated messages (sync, warnings, errors) are dimmed to visually separate from user application logs
- **Live status panel**: Sync state, RX bytes, transport speed (current + max), frame rate
- **Per-source frame loss warnings**: Real-time `[WARN]` notifications when firmware reports new frame loss, with source name (e.g., `LL_TASK`)
- **Per-source statistics view**: Press `d` to open a modal overlay showing written/lost frame and byte counts per source
- **Raw binary capture**: All received bytes are saved to a `.bin` file for [offline analysis](#offline-analysis)
- **Scrollable log view** with auto-scroll toggle

## Prerequisites

### 1. ESP-IDF Environment

BLE Log Console runs within the ESP-IDF Python environment. You must source `export.sh` before use:

```bash
cd <esp-idf-root>
. ./export.sh
```

This sets up the Python virtual environment at `~/.espressif/python_env/` which includes all required dependencies (`textual`, `pyserial`, `click`, etc.).

### 2. Firmware Configuration

The target ESP32 device must have the BLE Log module enabled and configured for UART DMA output. Configure via `idf.py menuconfig`:

```
Component config → Bluetooth → BT Logs → Enable BLE Log Module (Experimental)  [y]
```

Then select the transport peripheral and UART settings:

```
Component config → Bluetooth → BT Logs → BLE Log Module
  → Peripheral Selection → UART DMA
  → UART DMA Configuration
      → UART Port Number        (default: 0)
      → Baud Rate               (default: 3000000)
      → TX GPIO Number          (set to match your hardware)
```

#### Quick Setup: Critical-Log-Only Mode

The simplest way to enable BLE Log with UART DMA output:

```
Component config → Bluetooth → BT Logs → Enable critical-log-only mode  [y]
```

This automatically enables the BLE Log Module, selects UART DMA as the default peripheral, and restricts each stack (Controller/Host/Mesh) to critical logs only.

#### Recommended Kconfig Options

| Kconfig Option | Recommended | Why |
|----------------|-------------|-----|
| `CONFIG_BT_LOG_CRITICAL_ONLY` | `y` | One-click setup — enables BLE Log + UART DMA + compression |
| `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE` | `3000000` | 3 Mbps — balances throughput and reliability |
| `CONFIG_BLE_LOG_LL_ENABLED` | `y` (auto) | Auto-enabled by ESP BLE Controller detection |

> **Note**: Payload checksum (XOR, full scope) and enhanced statistics are always enabled — no Kconfig options needed.

> **Note on UART PORT 0**: When `CONFIG_BLE_LOG_PRPH_UART_DMA_PORT=0`, the firmware automatically wraps `ESP_LOG` output in BLE Log frames (`BLE_LOG_SRC_REDIR`). The console decodes and displays these as regular ASCII log lines. See the [BLE Log module README](../../../components/bt/common/ble_log/README.md#uart-redirect-port-0) for details.

### 3. Hardware Connection

Connect the ESP32 UART TX pin to a USB-to-serial adapter:

```
ESP32 TX GPIO ──────── USB-Serial RX
ESP32 GND     ──────── USB-Serial GND
```

Ensure your USB-serial adapter supports the configured baud rate (3 Mbps by default). Adapters based on CP2102N, CH343, or FT232H are recommended.

## Installation

No separate installation is needed. The `textual` and `textual-fspicker` packages are included in ESP-IDF's core requirements (`tools/requirements/requirements.core.txt`) and installed automatically by `./install.sh`.

Verify the dependency is available:

```bash
. ./export.sh
python -c "import textual; print(textual.__version__)"
```

## Usage

### Interactive Mode (Launch Screen)

Run `python console.py` with no arguments to open the **Launch Screen** — an interactive TUI where you can select the serial port, baud rate, and log directory before starting capture:

```bash
cd <esp-idf-root>
. ./export.sh
cd tools/bt/ble_log_console

python console.py
```

The Launch Screen lets you browse available ports and configure options without memorising CLI flags.

### Capture Mode (CLI)

Pass `--port` directly to skip the Launch Screen and start capture immediately:

```bash
cd <esp-idf-root>
. ./export.sh
cd tools/bt/ble_log_console

# Basic usage (--port is now optional; omit to use Launch Screen)
python console.py --port /dev/ttyUSB0

# With custom baud rate
python console.py --port /dev/ttyUSB0 --baudrate 2000000

# With custom log directory
python console.py --port /dev/ttyUSB0 --log-dir /tmp/my_captures

# With custom output file (deprecated — prefer --log-dir)
python console.py --port /dev/ttyUSB0 --output /tmp/my_capture.bin

# Short form
python console.py -p /dev/ttyUSB0 -b 3000000 -d /tmp/my_captures
```

#### Options

| Option | Short | Default | Description |
|--------|-------|---------|-------------|
| `--port` | `-p` | (optional) | UART port device path (e.g., `/dev/ttyUSB0`, `COM3`). Omit to use Launch Screen. |
| `--baudrate` | `-b` | `3000000` | Baud rate — must match `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE` |
| `--log-dir` | `-d` | current working directory | Directory where capture `.bin` files are saved |
| `--output` | `-o` | auto-generated | *(Deprecated)* Explicit output file path — use `--log-dir` instead |

When `--log-dir` is not specified, capture files are saved to the **current working directory** with a timestamp-based filename:

```
<cwd>/ble_log_YYYYMMDD_HHMMSS.bin
```

### List Saved Captures (`ls`)

List all previously saved `.bin` capture files, sorted by most recent first:

```bash
python console.py ls
```

Example output:

```
Captures in /tmp/ble_log_console:

  2026-03-17 14:30:25     2.3 MB  ble_log_20260317_143025.bin
  2026-03-17 10:15:03   512.0 KB  ble_log_20260317_101503.bin
  2026-03-16 18:42:11     1.1 MB  ble_log_20260316_184211.bin
```

These `.bin` files contain raw binary data exactly as received from UART, suitable for [offline analysis](#offline-analysis).

## Firmware Configuration

### Checksum Mode Detection

The console automatically detects the firmware's checksum mode by probing all 4 combinations during the SEARCHING phase:

| Firmware Config | Console Detects |
|-----------------|-----------------|
| XOR checksum + Full scope | `XOR / Header+Payload` |
| XOR checksum + Header-only scope | `XOR / Header` |
| Sum checksum + Full scope | `Sum / Header+Payload` |
| Sum checksum + Header-only scope | `Sum / Header` |

The detected mode is logged in the log view after sync is achieved (3 consecutive valid frames).

### Enhanced Statistics (ENH_STAT)

The firmware periodically emits `INTERNAL` frames containing per-source write/loss counters (enhanced statistics is always enabled). The console decodes these and uses them as the authoritative source of frame and byte loss. Loss counters are baselined on the first ENH_STAT received per source, so the console only shows loss since it started.

When new frame loss is detected in an ENH_STAT report, a `[WARN]` notification is displayed in the log view with the source name and incremental loss count. Press `d` at any time to view a per-source breakdown of written and lost frames/bytes.

## How It Works

### Sync State Machine

```
                  frame valid
  ┌──────────┐  ──────────────▶  ┌────────────────┐
  │ SEARCHING │                  │ CONFIRMING_SYNC │ ──(N frames)──▶ SYNCED
  └──────────┘  ◀──────────────  └────────────────┘
                  frame invalid

                  frame invalid
  ┌────────┐  ──────────────▶  ┌─────────────────┐
  │ SYNCED │                   │ CONFIRMING_LOSS  │ ──(M+1 fails)──▶ SEARCHING
  └────────┘  ◀──────────────  └─────────────────┘
                  frame valid
```

- **N** = 3 (sync confirmation threshold)
- **M** = 3 (loss tolerance — consecutive failures before resync)

### Frame Format

The console parses the standard BLE Log frame format:

```
[payload_len: 2B LE][frame_meta: 4B LE][payload: variable][checksum: 4B LE]
 └── Header (6B) ──┘                                       └── Tail (4B) ──┘
```

- `frame_meta` = `source_code[7:0] | frame_sn[31:8]`
- For most sources, payload starts with 4-byte `os_ts` (OS timestamp in ms)
- For `REDIR` source (code 8), payload is raw ASCII (no `os_ts` prefix)

### REDIR Frame Decoding

When the firmware uses UART PORT 0, `ESP_LOG` output is wrapped in frames with source code `REDIR` (8). The console:

1. Extracts the raw ASCII payload from each REDIR frame
2. Buffers partial lines across frames (a single log line may span multiple frames due to batch sealing)
3. Emits complete lines to the log view on each `\n` boundary

## Offline Analysis

### Raw Binary Capture

Every byte received from UART is saved to the output `.bin` file **before** parsing. This ensures the capture is complete and unmodified, regardless of parser state or sync loss.

Use `python console.py ls` to find saved captures.

### Parsing with BLE Log Analyzer

The saved `.bin` files can be parsed offline using the **BLE Log Analyzer**'s `ble_log_parser_v2` module for detailed analysis:

- Frame-by-frame decoding with source filtering
- HCI log extraction and conversion to btsnoop format (for Wireshark)
- Timestamp reconstruction and event correlation
- Link Layer log decoding

The binary format is identical whether captured by BLE Log Console, a logic analyzer, or any other tool — the parser reads the same frame structure documented above.

> **Tip**: The `bt_hci_to_btsnoop` tool at `tools/bt/bt_hci_to_btsnoop/` can convert extracted HCI logs to btsnoop format for analysis in Wireshark.

## Keyboard Shortcuts

All shortcuts are **case-insensitive** (e.g., `Q` and `q` both quit).

| Key | Action |
|-----|--------|
| `q` | Quit the application |
| `Ctrl+C` | Quit the application |
| `c` | Clear the log view |
| `s` | Toggle auto-scroll (on by default) |
| `d` | Show per-source frame statistics (press `Escape` to return) |
| `h` | Show keyboard shortcuts (press `Escape` to return) |
| `r` | Reset chip via DTR/RTS toggle |

## Building Executable

To distribute BLE Log Console as a standalone single-file executable (no Python installation required on the target machine), use the provided `build_exe.py` script with [PyInstaller](https://pyinstaller.org/):

```bash
pip install pyinstaller
cd tools/bt/ble_log_console
python build_exe.py
```

The executable is written to `dist/ble_log_console` (Linux/macOS) or `dist\ble_log_console.exe` (Windows). Copy it to any machine and run it directly — no ESP-IDF environment needed.

> **Note**: Build the executable on the same OS/architecture as the target machine. PyInstaller does not cross-compile.

## Architecture

```
console.py (Click CLI)
    │
    ▼
BLELogApp (Textual App)
    │
    ├── Backend Worker (thread)
    │     │
    │     ├── UART Transport ── open_serial() ── raw .bin file
    │     │
    │     ├── FrameParser ── sync state machine + checksum auto-detection
    │     │     │
    │     │     └── ParsedFrame { source_code, frame_sn, payload, os_ts_ms }
    │     │
    │     ├── InternalDecoder ── decode INIT_DONE, INFO, ENH_STAT, FLUSH
    │     │
    │     └── StatsAccumulator ── RX bytes, BPS, FPS, firmware-reported loss
    │
    └── Frontend (Textual widgets)
          ├── LogView ── scrollable RichLog with styled output
          ├── StatusPanel ── fixed bottom bar with live stats
          ├── StatsScreen ── modal overlay for per-source statistics (d key)
          └── ShortcutScreen ── modal overlay for keyboard shortcuts (h key)
```

### Source Layout

```
src/
  __init__.py                  # Python 3.10 guard + textual dependency check
  app.py                       # Textual App — wires backend worker to frontend
  backend/
    models.py                  # Enums, dataclasses, Textual Message types
    checksum.py                # XOR and Sum checksum (matches firmware impl)
    frame_parser.py            # Sync state machine with checksum auto-detection
    internal_decoder.py        # INTERNAL frame payload decoder
    stats/                     # Statistics sub-package
      __init__.py              # Re-exports StatsAccumulator
      accumulator.py           # Thin composition of sub-modules
      transport.py             # RX bytes, BPS, FPS tracking
      firmware_loss.py         # ENH_STAT loss delta tracking
      firmware_written.py      # ENH_STAT write tracking
      sn_gap.py                # SN gap detection
      peak_burst.py            # 1ms window peak write burst
      traffic_spike.py         # Wire saturation detection
    uart_transport.py          # Serial port helpers, file I/O
  frontend/
    log_view.py                # RichLog wrapper with color-coded write methods
    shortcut_screen.py         # Modal screen for keyboard shortcuts
    stats_screen.py            # Modal screen for per-source statistics
    status_panel.py            # Reactive status bar (sync, speed, help hint)
tests/
  helpers.py                   # Synthetic frame builder for tests
  test_checksum.py             # Checksum algorithm tests
  test_frame_parser.py         # State machine + auto-detection tests
  test_internal_decoder.py     # Internal frame decoding tests
  test_stats.py                # Stats accumulator and firmware loss tests
```

## Development

### Running Tests

```bash
cd <esp-idf-root>
. ./export.sh
cd tools/bt/ble_log_console
python -m pytest tests/ -v
```

### Linting & Formatting

```bash
python -m ruff format src/ tests/
python -m ruff check --fix src/ tests/
```

### Type Checking

```bash
python -m mypy src/backend/
```

## Troubleshooting

### "UART port not found"

- Check the device is connected: `ls /dev/ttyUSB*` (Linux) or `ls /dev/tty.usb*` (macOS)
- Ensure you have permission: `sudo usermod -aG dialout $USER` (Linux, then re-login)
- On WSL, USB devices need [usbipd-win](https://github.com/dorssel/usbipd-win) to pass through

### Sync stays in SEARCHING

- **Baud rate mismatch**: Ensure `--baudrate` matches `CONFIG_BLE_LOG_PRPH_UART_DMA_BAUD_RATE`
- **Wrong port**: Verify you're connected to the correct UART TX pin
- **Firmware not running**: Check the device has booted and BLE Log is initialized
- **Signal integrity**: At 3 Mbps, use short cables and ensure solid GND connection

### No ESP_LOG output

When using UART PORT 0, `ESP_LOG` is redirected through BLE Log frames. If you don't see log lines:
- Ensure the firmware has `CONFIG_BLE_LOG_PRPH_UART_DMA_PORT=0`
- The console automatically decodes REDIR frames — no extra configuration needed
- Logs are flushed by a 1-second periodic timer, so there may be a short delay

### High frame loss

- Press `d` to view per-source loss counters (enhanced statistics is always enabled)
- Increase buffer sizes: `CONFIG_BLE_LOG_LBM_TRANS_SIZE`, `CONFIG_BLE_LOG_LBM_LL_TRANS_SIZE`
- Add more LBMs: `CONFIG_BLE_LOG_LBM_ATOMIC_LOCK_TASK_CNT`
- Increase baud rate if your adapter supports higher speeds

### Import errors

```
ModuleNotFoundError: No module named 'textual'
```

Re-run the ESP-IDF installer:

```bash
cd <esp-idf-root>
./install.sh
. ./export.sh
```
