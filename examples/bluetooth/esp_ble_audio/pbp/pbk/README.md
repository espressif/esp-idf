| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# PBP Public Broadcast Sink (PBK) Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements the **Public Broadcast Profile (PBP) Public Broadcast Sink (PBK)** role. The device passively scans for non-connectable periodic advertisers that carry both the Broadcast Audio Announcement service (with a 24-bit broadcast ID) and a Public Broadcast Announcement (PBA) with the high quality (HQ) feature bit set and the encryption bit clear, then synchronizes to periodic advertising and the BIG to receive one BIS. SQ-only and encrypted public broadcasts are ignored; the example does not receive a Broadcast Code from a Broadcast Assistant.

The implementation uses ESP-BLE-AUDIO for the PBA parsing, the BAP broadcast sink, and the PACS sink (LC3 capability: 48 kHz, 10 ms, 1 channel, 100 octets/frame, media context — sized for the 48_2_1 preset that the paired PBS sends). The BAP broadcast sink depends on the scan delegator receive-state storage, so `CONFIG_BT_BAP_SCAN_DELEGATOR` is enabled, but no Assistant callbacks are registered and no connectable advertising is started. Received SDUs are accounted for via `example_audio_rx_metrics_*`; there is no LC3 decoder or audio output.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

### Security & Pairing

Security is inherited from `../../common_components/example_init/ble_audio_example_init.c`: LE Secure Connections with bonding enabled, no MITM, **Just-Works** pairing (`BLE_SM_IO_CAP_NO_IO`). The PBK only scans and receives an unencrypted broadcast; it neither advertises connectably nor starts pairing, so these settings are not used.

## Build & Flash

The base `sdkconfig.defaults` defaults to the **Bluedroid** host; idf.py automatically merges the per-target overlay (`sdkconfig.defaults.$IDF_TARGET`). To build with **NimBLE** host instead, layer `sdkconfig.defaults.nimble` on top via `-DSDKCONFIG_DEFAULTS` in a separate build directory with its own `sdkconfig`, because an existing `sdkconfig` takes precedence over the defaults.

### Bluedroid host (default)

```bash
idf.py --preview set-target esp32h4
idf.py -p PORT flash monitor
```

### NimBLE host

```bash
idf.py --preview -B build_nimble_esp32h4 -DSDKCONFIG=build_nimble_esp32h4/sdkconfig -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.nimble" set-target esp32h4
idf.py -B build_nimble_esp32h4 -DSDKCONFIG=build_nimble_esp32h4/sdkconfig -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.nimble" -p PORT flash monitor
```

For `esp32s31`, replace `esp32h4` with `esp32s31` in the commands above.

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. Initialize NVS, the Bluetooth controller/host, and the LE Audio common layer with a GAP app callback that handles `EXT_SCAN_RECV`, `PA_SYNC`, and `PA_SYNC_LOST`.
2. Register the PACS sink (mono location, media contexts, LC3 sink capability), the stream ops, and the broadcast sink callbacks, then start the audio stack and begin passive extended scanning at 100 ms / 100 ms.
3. On each scan result, parse the service data: capture the broadcast ID from Broadcast Audio Announcement and the features and metadata from the PBA (other service data UUIDs are skipped). If both are present, the HQ bit is set, the encryption bit is clear, and no PA sync is in progress, create the periodic-advertising sync.
4. On `PA_SYNC` success, stop scanning and create the broadcast sink for the sync handle and broadcast ID; a failed sync keeps scanning so a later report can retry, and a failed sink creation terminates the PA sync so that scanning resumes.
5. The `base_recv` callback selects the lowest BIS index of the first 48 kHz (High Quality) subgroup in the BASE, since a public broadcast may also carry Standard Quality subgroups; the `syncable` callback then calls `esp_ble_audio_bap_broadcast_sink_sync`. If BIGInfo arrives before BASE or the sync request fails, the next BIGInfo report retries; encrypted BIGInfo is ignored.
6. Stream `started` resets RX metrics and prints the codec configuration; `recv` feeds each SDU into `example_audio_rx_metrics_on_recv` (tracking valid/error/lost/zero-length counts); `stopped` logs the reason.
7. When the BIG stops, the sink is deleted and a PA sync that is still active is terminated; scanning resumes once the PA sync loss is reported. When the PA sync is lost first, a running BIG keeps receiving until it stops, while a sink that never reached BIG sync is deleted directly and scanning resumes.

## Expected Log

TAG is `PBP_PBK`.

Init:
```
I (xxx) PBP_PBK: PBP broadcast sink initialized
I (xxx) PBP_PBK: Scanning for public broadcasts...
```

Scan and PA sync:
```
I (xxx) PBP_PBK: Public source 'PBP Broadcast Source': id 0x123456, features 0x04, peer xx:xx:xx:xx:xx:xx
I (xxx) PBP_PBK: PA synced: handle .., waiting for BASE and BIGInfo
I (xxx) PBP_PBK: BASE received: selected BIS bitmap 0x00000001
I (xxx) PBP_PBK: BIG sync requested: BIS bitmap 0x00000001
```

Streaming:
```
I (xxx) PBP_PBK: [SNK #0] Stream started
```
(Subsequent per-packet metrics are emitted by `example_audio_rx_metrics_on_recv` under the `SNK #0` label.)

Stop / sync loss:
```
I (xxx) PBP_PBK: [SNK #0] Stream stopped, reason 0x..
I (xxx) PBP_PBK: Broadcast sink stopped, reason 0x..
I (xxx) PBP_PBK: PA sync lost: sync_handle .. reason 0x..
```

## Peer Pairing

Run [pbs](../pbs/) on a second board.

1. Flash and boot this PBK; it registers PACS and enters extended scanning.
2. Boot the PBS peer; it advertises the HQ PBA, the Broadcast Name, and broadcast ID `0x123456` plus periodic advertising carrying the BASE.
3. This PBK matches the Broadcast Audio and PBA service data on a single scan result and creates the PA sync; on success it stops scanning and creates the broadcast sink.
4. `base_recv` selects the BIS index of the 48 kHz subgroup, then `syncable` triggers `esp_ble_audio_bap_broadcast_sink_sync`.
5. Stream `started` fires, RX metrics reset, and `recv` accumulates valid/error/lost/zero-length SDU counters as the PBS sends.
6. If the PBS stops or moves out of range, the BIG stops, the sink is deleted, and the PBK resumes scanning automatically.
