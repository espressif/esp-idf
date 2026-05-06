| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BIG Receiver

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example demonstrates a raw BLE Isochronous Broadcast Receiver — it joins a Broadcast Isochronous Group (BIG) directly at the ISO transport layer over the NimBLE host, without any BLE Audio profile (BAP/CAP) on top.

The device acts as the **receiver (BIG sync sink)**: it scans for an extended advertiser by device name, synchronises to the peer's periodic advertising train, parses the BIGInfo report, and then issues `LE BIG Create Sync` to join two BIS sub-events. An HCI output data path is installed on each BIS and incoming SDUs are accounted for by a small RX-metrics helper.

The payloads received are **application-supplied dummy data** produced by `big_broadcaster`; this example does not decode LE-Audio / LC3 — the coding format on the data path is `ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT`.

## Requirements

* A board with BLE 5.2 and ISO support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

Notable hard-coded parameters in `main/main.c`:

* `TARGET_DEVICE_NAME` — `"BIG Broadcaster"` (matched against the AD complete-name field)
* `TARGET_BROADCAST_CODE` — `"1234"` (must match the broadcaster)
* `SCAN_INTERVAL` / `SCAN_WINDOW` — 100 ms / 100 ms (passive)
* `PA_SYNC_TIMEOUT` — 10 s
* `BIG_SYNC_TIMEOUT` — 1 s
* `BIS_ISO_CHAN_COUNT` — 2

### Security & Pairing

The shared init at `../common_components/example_init/ble_iso_example_init.c` configures Just-Works pairing (LE Secure Connections, no MITM, `BLE_SM_IO_CAP_NO_IO`) with bonding enabled, and leaves `gatts_register_cb = NULL` (no GATT services). These settings are not exercised by this example — the receiver passively syncs to PA + BIS without ATT or pairing.

## Build & Flash

```bash
idf.py set-target esp32h4
idf.py -p PORT flash monitor
```

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. NVS, NimBLE host, and the ISO common layer are initialised; a GAP application callback is registered for ISO-related GAP events.
2. Passive extended scanning is started via `ble_gap_disc()`.
3. On each `EXT_SCAN_RECV` the AD payload is parsed; if the complete-name field equals `"BIG Broadcaster"` and the report carries a non-zero periodic-advertising interval, `ble_gap_periodic_adv_sync_create()` is called.
4. On `PA_SYNC` success the extended scan is cancelled (BIGInfo arrives over the PA channel anyway).
5. On the first `BIGINFO_RECV` event the receiver fills `esp_ble_iso_big_sync_param_t` (both BIS in `bis_bitfield`, broadcast code `"1234"`, MSE = `nse` from BIGInfo) and calls `esp_ble_iso_big_sync()` — the HCI `LE BIG Create Sync` command.
6. When each BIS becomes ready, the connected callback resets that BIS's RX metrics and installs an HCI output data path (`ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT`, transparent coding).
7. Each `recv` callback updates the per-BIS counters; one log line is emitted every `LOG_INTERVAL_PACKETS` SDUs.
8. On BIS disconnect or PA-sync-lost the receiver clears its sync state and restarts extended scanning.

## Expected Log

Tag: `BIG_SNC`.

Discovery and PA sync:

```
I (xxx) BIG_SNC: Scanning for broadcaster...
I (xxx) BIG_SNC: PA synced: handle <h> sid <s> phy <p> peer xx:xx:xx:xx:xx:xx
```

BIS bring-up:

```
I (xxx) BIG_SNC: [BIS #0] Connected
I (xxx) BIG_SNC: [BIS #1] Connected
```

Steady-state (logged every `LOG_INTERVAL_PACKETS` SDUs by the shared RX helper):

```
I (xxx) BIG_SNC: [BIS #0] RX: <N> packets
I (xxx) BIG_SNC: [BIS #1] RX: <N> packets
```

Teardown / re-sync (broadcaster disappears or restarts; reason `0x08` is the typical timeout, `0x3D` is the rare MIC-fail race documented in source):

```
I (xxx) BIG_SNC: [BIS #0] Disconnected, reason 0x08
I (xxx) BIG_SNC: [BIS #1] Disconnected, reason 0x08
I (xxx) BIG_SNC: PA sync lost: sync_handle <h> reason 0x<rr>
I (xxx) BIG_SNC: Scanning for broadcaster...
```

## Peer Pairing

Run [big_broadcaster](../big_broadcaster/) on a second board.

1. Flash and run `big_broadcaster` on board A; it begins extended + periodic advertising and creates the BIG.
2. Flash and run `big_receiver` on board B; it logs `Scanning for broadcaster...` and waits.
3. Board B matches the AD name `"BIG Broadcaster"`, creates a PA sync, logs `PA synced`, and stops the extended scan.
4. On the first BIGInfo report, board B issues `LE BIG Create Sync` with broadcast code `"1234"` and both BIS selected; both sides log `[BIS #0/1] Connected`.
5. Per-BIS RX-packet milestones are logged on B at the same cadence that A logs TX milestones.
6. Resetting board A causes B to log a BIS disconnect and `PA sync lost`, then automatically restart scanning to re-pair.
