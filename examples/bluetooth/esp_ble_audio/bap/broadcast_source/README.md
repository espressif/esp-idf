| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BAP Broadcast Source Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example acts as a **BAP Broadcast Source**. It creates a BAP broadcast source from the LC3 `16_2_1` broadcast preset, encodes the BASE into the periodic advertising payload, places the Broadcast Audio Announcement Service UUID and a fixed 24-bit Broadcast ID (`0x123456`) in the extended advertising payload, and then starts the BIG so that BIS streams transmit synthetic SDU data on a fixed cadence.

The build runs on top of the NimBLE host stack and the ESP BLE Audio component set. Source-side APIs used: `esp_ble_audio_common_init` / `esp_ble_audio_common_start` (common layer), `esp_ble_audio_bap_broadcast_source_create` / `_get_base` / `_start` (BAP), `esp_ble_audio_bap_broadcast_adv_add` (BAP/ISO glue), and the BAP stream callbacks (`started`, `stopped`, `sent`, `disconnected`). PACS, GAP scanner, and the scan delegator are **not** used on this side. Encryption is enabled because the broadcast code `"1234"` is non-empty; packing is `ESP_BLE_ISO_PACKING_SEQUENTIAL`. Channel allocation per stream is hard-coded as `FRONT_LEFT` for stream 0 and `FRONT_RIGHT` for stream 1.

The TX scheduler is built on `example_audio_tx_scheduler_*` helpers; the source comment notes that ESP timer resolution is not accurate enough for the SDU interval, so a `k_work_delayable`-based scheduler is used instead.

## Requirements

* A board with Bluetooth LE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* A peer running the [broadcast_sink](../broadcast_sink) example to receive and decode the BIS streams

## Configuration

Open menuconfig:

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source. The device name (`"BAP Broadcast Source"`), broadcast ID (`0x123456`), broadcast code (`"1234"`), advertising/periodic intervals, stream and subgroup counts (via `CONFIG_BT_BAP_BROADCAST_SRC_STREAM_COUNT` / `_SUBGROUP_COUNT` from the BAP component), and LC3 preset (`16_2_1`) are compile-time constants in `main.c`.

### Security & Pairing

The example inherits a Just-Works pairing model (LE Secure Connections, no MITM, no I/O capability) with bonding enabled from the shared init at `../common_components/example_init/ble_audio_example_init.c`; change pairing/IO-cap there if needed.

## Build & Flash

```bash
idf.py set-target esp32h4   # or esp32s31
idf.py -p PORT flash monitor
```

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. `app_main` initializes NVS, calls `bluetooth_init()`, and calls `esp_ble_audio_common_init(NULL)` (no GAP callback for this role).
2. `broadcast_source_setup()` registers `source_started` / `source_stopped` callbacks, populates per-stream channel-allocation BIS metadata (left/right), registers `stream_ops` on each stream, and calls `esp_ble_audio_bap_broadcast_source_create()` with the encrypted preset.
3. `esp_ble_audio_common_start(NULL)` starts the audio stack; each stream's `example_audio_tx_scheduler_init()` is wired with `tx_scheduler_cb`.
4. `ext_adv_start()` configures non-connectable extended adv (1M primary / 2M secondary, 200 ms interval), writes Broadcast Audio Service Data + Broadcast ID + complete name, configures periodic adv (100 ms), writes the BASE returned by `esp_ble_audio_bap_broadcast_source_get_base()`, then starts periodic and extended advertising.
5. `broadcast_start()` calls `esp_ble_audio_bap_broadcast_adv_add()` and `esp_ble_audio_bap_broadcast_source_start()` on the same `ADV_HANDLE`, which kicks off BIGInfo + BIS creation.
6. When each BIS stream goes streaming, `stream_started_cb` allocates an SDU-sized buffer and calls `example_audio_tx_scheduler_start()` at `preset_active.qos.interval`; the scheduler invokes `broadcast_source_tx()`, which fills the buffer with the low byte of `seq_num` and calls `esp_ble_audio_bap_stream_send()`.
7. `stream_sent_cb` forwards completions to `example_audio_tx_scheduler_on_sent()` for drift accounting; `stream_stopped_cb` and `stream_disconnected_cb` stop the scheduler; `source_stopped_cb` frees all per-stream buffers.

## Expected Log

```
I (xxx) BAP_BSRC: Creating broadcast source: ... subgroup(s), ... stream(s)/subgroup
I (xxx) BAP_BSRC: Advertising started (handle 0)
I (xxx) BAP_BSRC: Broadcast source started
I (xxx) BAP_BSRC: [SRC #0] Stream started
I (xxx) BAP_BSRC: [SRC #1] Stream started
```

Periodic TX accounting (emitted by `example_audio_tx_scheduler_on_sent` under the `BAP_BSRC` tag, name `SRC #<idx>`) follows. On teardown:

```
I (xxx) BAP_BSRC: [SRC #0] Stream stopped, reason 0x...
I (xxx) BAP_BSRC: [SRC #0] ISO disconnected, reason 0x...
I (xxx) BAP_BSRC: Broadcast source stopped, reason 0x...
```

## Peer Pairing

Run [broadcast_sink](../broadcast_sink/) on a second board. Expected interaction sequence:

1. Source advertises extended PDU containing the Broadcast Audio Announcement Service UUID, Broadcast ID `0x123456`, and complete name `"BAP Broadcast Source"`; sink scans and matches by name + UUID.
2. Source's periodic advertising carries the encoded BASE; sink establishes PA sync and parses the BASE to recover subgroup count and BIS index bitfield.
3. Source's BIGInfo advertises the BIG as encrypted (broadcast code `"1234"`); sink reports `BIG encrypted`.
4. Source starts the BIG and the two BIS streams (`FRONT_LEFT`, `FRONT_RIGHT`); sink calls `esp_ble_audio_bap_broadcast_sink_sync()` with the chosen BIS bitfield and the matching broadcast code.
5. Source's TX scheduler keeps pushing SDUs at `preset_active.qos.interval`; sink stream `recv` callbacks deliver the data and update RX metrics.
6. Stopping the source (or losing PA sync) triggers `stream_stopped_cb` on the sink, which deletes the broadcast sink and resumes scanning.
