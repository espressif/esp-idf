# BLE Log Decompression Tool

`ble_log_decompress.py` converts BLE Log compressed binary frames into readable log messages.

It supports:

- **Real-time decoding** from a serial port
- **Offline decoding** from captured hex text (`stdin`)

## Why this tool exists

In BLE Log mode, firmware does not transmit full printf-style strings over the logging transport. Instead, it sends compact binary records (log IDs + encoded arguments). This reduces runtime overhead and transport bandwidth on target.

The decompression tool runs on the host side and reconstructs the original log message text by combining:

- incoming frame bytes, and
- a generated dictionary file (`*_logs.json`) that maps log IDs to format strings.

## What the tool does

Firmware sends compact BLE Log frames instead of full text strings. The tool:

1. Parses each frame (`header + payload + checksum tail`)
2. Validates frame integrity (checksum and size guards)
3. Resolves log IDs using a generated dictionary (`*_logs.json`)
4. Prints reconstructed text output

## End-to-end data flow

1. **Input acquisition**
  - Serial mode: reads one line at a time from UART.
  - Stdin mode: reads captured hex lines from a file/pipe.

2. **Hex line parsing**
  - Converts space-separated hex tokens into raw bytes.
  - Tolerates optional line labels like `RX:`.
  - Malformed tokens are skipped to improve robustness.

3. **Rolling frame assembly**
  - Bytes are appended to a rolling buffer.
  - Frames are extracted only when a complete frame is present.
  - This allows recovery when a frame spans multiple lines.

4. **Frame validation and resynchronization**
  - Validates checksum using selected mode (`off`, `head`, `head_payload`, `auto`).
  - Applies sanity checks (for example `--max-payload-len`) to avoid “ghost frames” caused by padding/noise.
  - On mismatch, parser shifts forward and attempts resync.

5. **Compressed payload decode**
  - Decodes compressed record types and argument encodings.
  - Rebuilds formatted messages using the dictionary.
  - Prints a normalized line with timestamp, sequence, source, log ID, and message.

## Current scope

The current decoder path accepts **host compressed logs only** (on-wire source id **0**), using the dictionary `BLE_HOST_logs.json` (Bluedroid or NimBLE — only one host is active per build).

If the payload source is not **host (0)** (for example mesh sources 1 or 2), that record path is ignored by this script.

## Requirements

- Python 3.8+
- `pyserial` (required only for serial mode)

Install:

```bash
pip install pyserial
```

## Quick start

### Real-time serial mode

```bash
python ble_log_decompress.py \
  --port /dev/ttyUSB0 \
  --baud 115200 \
  --dict path/to/BLE_HOST_logs.json
```

### Offline mode (from file)

```bash
cat captured_dump.txt | python ble_log_decompress.py \
  --stdin \
  --dict path/to/BLE_HOST_logs.json
```

If `--dict` is not provided, the tool attempts best-effort auto-discovery of a suitable JSON dictionary under `build/`.

## Input format

Each line should be space-separated hex bytes, for example:

```text
08 00 07 4F 01 00 CC CA 18 00 ...
```

An optional leading label token ending with `:` is accepted.

Example accepted lines:

```text
08 00 07 4F 01 00 CC CA 18 00 ...
RX: 08 00 07 4F 01 00 CC CA 18 00 ...
```

For best results, keep input as raw hex bytes from the BLE Log transport path.

## Command-line options

| Option | Default | Purpose |
|---|---|---|
| `--port` | `/dev/ttyUSB0` | Serial port path |
| `--baud` | `115200` | Serial baud rate |
| `--stdin` | off | Read from stdin instead of serial |
| `--dict` | auto-discover if omitted | Path to dictionary JSON |
| `--checksum` | `auto` | `off`, `head`, `head_payload`, `auto` |
| `--hex-buf` | `summary` | HEX buffer output: `skip`, `summary`, `full` |
| `--max-payload-len` | `1014` | Payload sanity guard |

Notes:

- `--checksum auto` learns checksum coverage mode from the stream and then locks to the dominant mode.
- Validation supports both checksum algorithms used by firmware:
  - additive sum
  - XOR (32-bit little-endian chunk XOR)
- If `--dict` points to a JSON file, sibling known dictionaries in the same directory are also checked.

## Output format

Example:

```text
[12.345s] [Seq 123] [Src 3] [Log 44] GAP procedure initiated: advertise;
```

Field meaning:

- `Timestamp`: target-side tick timestamp (ms) converted to seconds
- `Seq`: frame sequence number from frame metadata
- `Src`: compressed source ID found in payload
- `Log`: dictionary log ID used for format lookup
- trailing text: reconstructed message after argument decoding/formatting

Depending on record type and options, the tool may also print:

- unknown/no-format records,
- HEX buffer summaries (`--hex-buf summary`), or
- full HEX buffer dumps (`--hex-buf full`).

## Common issues

- **No decoded output**
  - Ensure the dictionary matches the running firmware build.
  - Ensure the input is BLE Log frame hex, not plain UART text logs.
  - Confirm logs are Host compressed logs for this decoder path.

- **Frequent drops/resync**
  - Use `--checksum auto`.
  - Confirm serial port/baud and wiring.
  - Adjust `--max-payload-len` only if your transport framing differs.

- **Messages decode but text is wrong**
  - Dictionary likely does not match the firmware binary.
  - Rebuild and use the dictionary generated from the same build output.

