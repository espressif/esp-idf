| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BAP Broadcast Sink Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example acts as a **BAP Broadcast Sink**. It scans for extended advertisements that carry the Broadcast Audio Announcement Service UUID and whose complete-name AD matches the hard-coded `"BAP Broadcast Source"` string; on a hit, it creates a periodic-advertising sync, builds a BAP broadcast sink for that PA handle and broadcast ID, decodes the BASE / BIGInfo from the PA channel, and then synchronizes to the BIG to receive BIS streams.

With `EXAMPLE_SCAN_OFFLOAD` enabled (off by default) it instead advertises connectable as a **Scan Delegator** and never picks a source itself, waiting for a [broadcast_assistant](../broadcast_assistant) to connect over BASS and tell it which broadcast to receive.

The build runs on top of the selected BLE host stack (Bluedroid by default; NimBLE via the `sdkconfig.defaults.nimble` overlay) and the ESP BLE Audio component set. Sink-side APIs used: `esp_ble_audio_common_init` / `_start`, `esp_ble_audio_pacs_register` + `esp_ble_audio_pacs_cap_register` (sink PAC and sink location enabled), `esp_ble_audio_bap_scan_delegator_register` (so a Broadcast Assistant can drive PA-sync, broadcast-code, and BIS-sync requests via BASS), `esp_ble_audio_bap_broadcast_sink_register_cb`, `esp_ble_audio_bap_broadcast_sink_create` / `_sync` / `_stop` / `_delete`, and `esp_ble_audio_bap_base_get_subgroup_count` / `_get_bis_indexes`. PAC capabilities are LC3 with sample rates 16 kHz + 24 kHz, frame duration 10 ms, 1 channel, 40–60 octets/frame, 1 frame/SDU. The fallback broadcast code is `"1234"`; if a Broadcast Assistant has supplied one through BASS it is used instead.

`scan_stop()` halts the extended scanner once PA sync is established — BASE/BIGInfo arrive over the PA channel. Self-scan builds re-arm it from `pa_sync_lost()`; offload builds arm it only for the duration of a requested sync, from `pa_sync_req_cb()`. The host-specific GAP/PA-sync plumbing lives in `main/bluedroid/scan.c` and `main/nimble/scan.c`; `main.c` only sees the host-agnostic interface in `scan.h`.

## Requirements

* A board with Bluetooth LE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* A peer running the [broadcast_source](../broadcast_source) example (or another BAP Broadcast Source advertising the matching name)
* Optionally, a [broadcast_assistant](../broadcast_assistant) peer to drive sync over BASS

## Configuration

Open menuconfig:

```bash
idf.py menuconfig
```

One build-time option, under **Example: Broadcast Sink**:

| Option | Default | Meaning |
| --- | --- | --- |
| `EXAMPLE_SCAN_OFFLOAD` | `n` | Advertise connectable (name `"BAP Broadcast Sink"`, BASS + PACS UUIDs) and let a Broadcast Assistant pick the source, instead of scanning for one. Enable it to pair with [broadcast_assistant](../broadcast_assistant). |
| `EXAMPLE_PAST` | `y` | Take up an Assistant's offer to transfer its own periodic sync, instead of establishing one locally. Needs `EXAMPLE_SCAN_OFFLOAD`; see the table below. |

The two modes are exclusive, because *source selection* is what gets offloaded. A self-found source is registered as a local receive state by `bt_bap_broadcast_sink_create()`, and the Assistant's Add Source for that same `{address type, SID, Broadcast ID}` would then be rejected as a duplicate (`0xFC`, BAP § 6.5.4). In offload builds without PAST the scanner still runs, but only between `pa_sync_req_cb()` and the sync being established: without a transfer the SyncInfo exists only in the source's extended advertising, so it has to be received on air. With `EXAMPLE_PAST=y` and an Assistant that offers it, the scanner never runs at all. The BASS scan delegator is registered in both modes; with `EXAMPLE_SCAN_OFFLOAD=n` nothing drives it.

`EXAMPLE_PAST` selects the host's PAST symbol (`BT_NIMBLE_PERIODIC_ADV_SYNC_TRANSFER` on NimBLE, `BT_BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER` on Bluedroid), which is what makes the library report PAST as available to an Assistant. Per BASS 3.1.1.4 a server may answer `PA_Sync = 0x01` either by requesting SyncInfo or by establishing the sync itself; `EXAMPLE_PAST` picks which. Either way an Assistant that offers PAST is served rather than rejected.

### PAST combinations

Whether the SyncInfo is transferred over the ACL or found on air is a build-time
choice on **both** sides: `EXAMPLE_PAST` here (which needs `EXAMPLE_SCAN_OFFLOAD`) and `EXAMPLE_PAST` in
[broadcast_assistant](../broadcast_assistant).

`PA_Sync` in Add Source states what the **assistant** can do, not what happens:
`0x01` means "I hold this sync and can transfer it", and the sink still chooses
whether to ask for it.

| Assistant | Sink | `PA_Sync` written | Sink's answer | SyncInfo comes from |
| --- | --- | --- | --- | --- |
| on | on | `0x01` offered | `INFO_REQ` | the ACL — transferred, **no scanning** |
| off | on | `0x02` not offered | establishes its own | the air — sink scans |
| on | off | `0x01` offered | declines, establishes its own | the air — sink scans |
| off | off | `0x02` not offered | establishes its own | the air — sink scans |

Declining is legal: BASS § 3.1.1.4 lets the server answer either way for `0x01`
and `0x02` alike. Rows 2 and 4 are indistinguishable from the sink's side —
they differ only in whether the assistant has PAST built in. Row 3 exists at all
because neither side checks the peer's PAST feature bit (`config_past_check` is
off in the library), so the offer goes out regardless of what the sink accepts.

### Security & Pairing

The example inherits a Just-Works pairing model (LE Secure Connections, no MITM, no I/O capability) with bonding enabled from the shared init at `../common_components/example_init/ble_audio_example_init.c`; change pairing/IO-cap there if needed.

## Build & Flash

The base `sdkconfig.defaults` defaults to the **Bluedroid** host; idf.py automatically merges the per-target overlay (`sdkconfig.defaults.$IDF_TARGET`). To build with **NimBLE** host instead, layer `sdkconfig.defaults.nimble` on top via `-DSDKCONFIG_DEFAULTS`.

### Bluedroid host (default)

```bash
idf.py set-target esp32h4
idf.py -p PORT flash monitor
```

### NimBLE host

```bash
idf.py set-target esp32h4
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.esp32h4;sdkconfig.defaults.nimble" -p PORT flash monitor
```

For `esp32s31`, replace the chip overlay accordingly.

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. `app_main` initializes NVS, calls `bluetooth_init()`, and calls `esp_ble_audio_common_init(&info)` with `info.gap_cb = iso_gap_app_cb`.
2. PACS is registered (`snk_pac` + `snk_loc`), each stream's `ops` field is wired to `stream_ops`, and the LC3 sink capability is registered via `esp_ble_audio_pacs_cap_register(ESP_BLE_AUDIO_DIR_SINK, ...)`.
3. The scan delegator (`scan_delegator_cbs`: `recv_state_updated`, `pa_sync_req`, `pa_sync_term_req`, `broadcast_code`, `bis_sync_req`) and broadcast-sink callbacks (`base_recv`, `syncable`) are registered, then `esp_ble_audio_common_start(NULL)` runs.
4. `scan_init()` performs host-specific GAP setup (Bluedroid: registers the GAP callback for `*_COMPLETE_EVT` semaphore signalling + posts `EXT_ADV_REPORT` / `PERIODIC_ADV_SYNC_ESTAB` / `PERIODIC_ADV_REPORT` / `PERIODIC_ADV_SYNC_LOST` to the audio engine; NimBLE: no-op — the scan-instance callback is passed at `ble_gap_disc` / `ble_gap_periodic_adv_sync_create` time). With `EXAMPLE_SCAN_OFFLOAD=n`, `scan_start()` then runs passive extended discovery (`itvl=window=160`). For each `ESP_BLE_AUDIO_GAP_EVENT_EXT_SCAN_RECV`, `data_cb` matches the complete/short/broadcast name AD type against `"BAP Broadcast Source"` and records the Broadcast ID from the Broadcast Audio Service Data.
5. On match (and only when not already PA-syncing and no scan-delegator state is pinned), `pa_sync_create(addr_type, addr, sid)` invokes the host-specific PA-sync create routine (`ble_gap_periodic_adv_sync_create` / `esp_ble_gap_periodic_adv_create_sync`) with `skip=0`, `sync_timeout=10s`.
   With `EXAMPLE_SCAN_OFFLOAD` the same `pa_sync_create()` is instead reached from `pa_sync_req_cb()`, which arms the scanner first and then syncs using the address, SID and Broadcast_ID the Assistant wrote into the receive state. With `EXAMPLE_PAST` and `past_available`, that whole branch is skipped: `pa_sync_with_past()` arms the PAST receive, the receive state goes to `INFO_REQ`, and the sync arrives on `ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_PAST` instead. The address there is on-air (LSB-first) order and is byte-reversed for Bluedroid by `addr_from_le()`. A failure reports `ESP_BLE_AUDIO_BAP_PA_STATE_FAILED` back over BASS and releases `req_recv_state`, so the Assistant can retry or pick another source.
6. `ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC` clears `pa_syncing`, cancels discovery, stores `sync_handle`, and calls `esp_ble_audio_bap_broadcast_sink_create()`.
7. `base_recv_cb` extracts the subgroup count and BIS index bitfield (masked by `bis_index_mask`); when no Broadcast Assistant is connected, `requested_bis_sync` defaults to `ESP_BLE_AUDIO_BAP_BIS_SYNC_NO_PREF`.
8. `syncable_cb` AND-masks the BASE bitfield with the requested mask, copies `TARGET_BROADCAST_CODE` if the BIG is encrypted (unless BASS already supplied one), and calls `esp_ble_audio_bap_broadcast_sink_sync()` with the chosen mask and `streams_p`.
9. `stream_started_cb` resets per-stream RX metrics and increments `stream_count_started`; `stream_recv_cb` updates metrics via `example_audio_rx_metrics_on_recv()`. When all streams have stopped, `stream_stopped_cb` clears `stream_started`; the sink itself is deleted from `broadcast_sink_stopped_cb` (the `stopped` sink callback), which runs once BASS has cleared `bis_sync` — deleting from `stream_stopped_cb` would race `rem_src` while `bis_sync` is still non-zero. `pa_sync_lost()` clears the cached `req_recv_state`, deletes any sink, and — in self-scan builds — restarts the scanner.

## Expected Log

```
I (xxx) BAP_BSNK: Scanning for broadcast source...
I (xxx) BAP_BSNK: Broadcast source PA synced, creating Broadcast Sink
I (xxx) BAP_BSNK: BASE received (... subgroup(s))
I (xxx) BAP_BSNK: bis_index_bitfield = 0x...
I (xxx) BAP_BSNK: Broadcast sink syncable, BIG encrypted
I (xxx) BAP_BSNK: Syncing to broadcast: BIS mask 0x... (... stream(s))
I (xxx) BAP_BSNK: [SNK #0] Stream started (.../...)
```

Per-stream RX metrics are then emitted under the `BAP_BSNK` tag, name `SNK #<idx>`. If a Broadcast Assistant connects via BASS, additional log lines appear:

```
I (xxx) BAP_BSNK: Receive state updated, pa_sync 0x... encrypt 0x...
I (xxx) BAP_BSNK: Assistant requests PA sync to 0x... (PAST {not }available)
I (xxx) BAP_BSNK: Broadcast code received
I (xxx) BAP_BSNK: BIS sync req: broadcast_id 0x... BIS mask 0x... subgroup mask 0x... (...)
I (xxx) BAP_BSNK: Received request to terminate PA sync
```

On teardown / sync loss:

```
I (xxx) BAP_BSNK: [SNK #0] Stream stopped, reason 0x... (.../...)
I (xxx) BAP_BSNK: Broadcast sink stopped, reason 0x...
I (xxx) BAP_BSNK: PA sync lost: sync_handle ... reason 0x...
I (xxx) BAP_BSNK: PA sync terminated
```

## Peer Pairing

Run [broadcast_source](../broadcast_source/) on a second board. Expected interaction sequence:

1. Source advertises extended PDU containing the Broadcast Audio Announcement Service UUID, Broadcast ID `0x123456`, and complete name `"BAP Broadcast Source"`; sink scans and matches by name + UUID.
2. Source's periodic advertising carries the encoded BASE; sink establishes PA sync and parses the BASE to recover subgroup count and BIS index bitfield.
3. Source's BIGInfo advertises the BIG as encrypted (broadcast code `"1234"`); sink reports `BIG encrypted`.
4. Source starts the BIG and the two BIS streams (`FRONT_LEFT`, `FRONT_RIGHT`); sink calls `esp_ble_audio_bap_broadcast_sink_sync()` with the chosen BIS bitfield and the matching broadcast code.
5. Source's TX scheduler keeps pushing SDUs at `preset_active.qos.interval`; sink stream `recv` callbacks deliver the data and update RX metrics.
6. Stopping the source (or losing PA sync) tears down the BIG; `broadcast_sink_stopped_cb` then deletes the sink (after BASS clears `bis_sync`) and, in self-scan builds, scanning resumes.
