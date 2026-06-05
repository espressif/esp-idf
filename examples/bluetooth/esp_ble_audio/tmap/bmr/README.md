| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# TMAP Broadcast Media Receiver (BMR) Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements the **Telephony and Media Audio Profile (TMAP) Broadcast Media Receiver (BMR)** role. The device scans for non-connectable extended advertisers that carry both the Broadcast Audio Announcement service (with a 24-bit broadcast ID) and the TMAS service with the **BMS** role bit set, then synchronizes to periodic advertising and the BIG to receive broadcast audio.

The implementation uses ESP-BLE-AUDIO for TMAP role registration, the BAP broadcast sink, the BAP scan delegator, the PACS sink (LC3 capability: 48 kHz, 10 ms, 1 channel, 100 octets/frame, media context — sized for the 48_2_1 preset that the paired BMS sends), and a VCP **Volume Renderer** (initial volume 10, unmuted, step 1). Up to `CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT` streams are pre-allocated and reused for every BIG sync; received SDUs are accounted for via `example_audio_rx_metrics_*`.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

### Security & Pairing

Security is inherited from `../../common_components/example_init/ble_audio_example_init.c`: LE Secure Connections with bonding enabled, no MITM, **Just-Works** pairing (`BLE_SM_IO_CAP_NO_IO`). The BMR receives broadcast (unencrypted) audio, so these settings only apply if a peer (e.g. a Broadcast Assistant) opens an ATT connection to the scan delegator / VCP renderer.

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

1. Initialize NVS, the Bluetooth controller/host, and the LE Audio common layer with a GAP app callback that handles `EXT_SCAN_RECV`, `PA_SYNC`, and `PA_SYNC_LOST`.
2. Register the TMAP role as `ESP_BLE_AUDIO_TMAP_ROLE_BMR`, then initialize the VCP Volume Renderer (`vcp_vol_renderer_init`) and the BAP broadcast sink (`bap_broadcast_sink_init`: PACS sink + sink location, broadcast sink callbacks, LC3 sink capability, scan delegator, stream ops on each pre-allocated stream).
3. Start the audio stack and begin passive extended scanning at 100 ms / 100 ms with `BLE_HS_FOREVER` duration.
4. On each scan result, parse service-data: capture broadcast ID from Broadcast Audio Announcement, set `tmap_bms_found` when TMAS carries the BMS role; if both are present and no PA sync is in progress, create the periodic-advertising sync.
5. On `PA_SYNC` success, cancel scanning, log the sync handle, and create the broadcast sink for the sync handle and broadcast ID.
6. The `base_recv` callback extracts the BIS index bitfield masked by the available stream count; the `syncable` callback then calls `esp_ble_audio_bap_broadcast_sink_sync` with the stream pointer array.
7. Per-stream `started` callback resets RX metrics; `recv` callback feeds each SDU into `example_audio_rx_metrics_on_recv` (tracking valid/error/lost/zero-length counts); `stopped` logs the reason.
8. On `PA_SYNC_LOST` matching the active sync handle, delete the broadcast sink and re-enter scanning.

## Expected Log

TAG is `TMAP_BMR`.

Init:
```
I (xxx) TMAP_BMR: VCP volume renderer initialized
I (xxx) TMAP_BMR: BAP broadcast sink initialized
I (xxx) TMAP_BMR: Scanning for broadcaster...
```

Scan and PA sync:
```
I (xxx) TMAP_BMR: Found TMAP BMS, starting PA sync (broadcast ID 0x......)
I (xxx) TMAP_BMR: PA synced: handle .. sid .. phy .. peer xx:xx:xx:xx:xx:xx
I (xxx) TMAP_BMR: PA sync .. synced with broadcast ID 0x......
I (xxx) TMAP_BMR: Broadcast source PA synced, waiting for BASE
I (xxx) TMAP_BMR: BASE received, creating broadcast sink
```

Streaming:
```
I (xxx) TMAP_BMR: [SNK #0] Stream started
```
(Subsequent per-packet metrics are emitted by `example_audio_rx_metrics_on_recv` under the `SNK #N` label.)

Stop / sync loss:
```
I (xxx) TMAP_BMR: [SNK #0] Stream stopped, reason 0x..
I (xxx) TMAP_BMR: PA sync lost: sync_handle .. reason 0x..
I (xxx) TMAP_BMR: PA sync .. lost with reason ..
```

VCP (when remote queries volume/flags):
```
I (xxx) TMAP_BMR: VCS volume .., mute ..
I (xxx) TMAP_BMR: VCS flags 0x..
```

Warnings (malformed adv):
```
W (xxx) TMAP_BMR: Invalid ad size .. (uuid)
W (xxx) TMAP_BMR: Invalid ad size .. (Broadcast ID)
W (xxx) TMAP_BMR: Invalid ad size .. (tmap role)
```

## Peer Pairing

Run [bms](../bms/) on a second board.

1. Flash and boot this BMR; it initializes VCP and PACS, then enters extended scanning.
2. Boot the BMS peer; it advertises with TMAS BMS role and broadcast ID `0x123456` plus periodic advertising carrying the BASE.
3. This BMR matches both service-data entries on a single scan result and creates the PA sync; on success it cancels scanning and creates the broadcast sink.
4. `base_recv` derives the BIS index bitfield, then `syncable` triggers `esp_ble_audio_bap_broadcast_sink_sync` with the pre-allocated stream pointers.
5. Stream `started` fires, RX metrics reset, and `recv` accumulates valid/error/lost/zero-length SDU counters as the BMS sends.
6. If the BMS stops or moves out of range, PA sync loss tears down the sink and the BMR resumes scanning automatically.
