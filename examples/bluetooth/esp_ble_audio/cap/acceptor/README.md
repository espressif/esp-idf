| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# CAP Acceptor Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements the **Common Audio Profile (CAP) Acceptor** role on top of the NimBLE host stack with ISO and LE Audio support. It is built in one of two mutually exclusive sub-modes selected at build time: a **CAP Acceptor / BAP Unicast Server**, or a **CAP Acceptor / BAP Broadcast Sink** (with the BAP Scan Delegator role enabled). PACS is registered in both modes with LC3 sink and source capabilities (any sampling frequency, 7.5 ms or 10 ms frame, up to 2 channels, 30..155 octets per frame, up to 2 frames per SDU). Sink and source PAC location are set to `FRONT_LEFT | FRONT_RIGHT` (note in `main.c`: with `MONO_AUDIO`, Samsung S24 declines unicast).

In **unicast** mode (`cap_acceptor_unicast.c`) the acceptor registers BAP unicast-server callbacks (config / reconfig / qos / enable / start / metadata / disable / stop / release) and CAP stream ops, and on enable of a sink ASE automatically issues `bap_stream_start` (Receiver Start Ready). When the source ASE starts, an internal TX scheduler begins sending dummy SDUs. In **broadcast** mode (`cap_acceptor_broadcast.c`) the acceptor registers BAP scan-delegator and broadcast-sink callbacks, drives PA sync (without PAST) on request, receives BASE and BIGInfo, then calls `esp_ble_audio_bap_broadcast_sink_sync` on the first BIS index. Optional **self-scan** lets the acceptor scan for a broadcast source named `CAP Broadcast Source` and use a hardcoded broadcast code `1234` instead of waiting for a Broadcast Assistant.

The acceptor advertises connectable extended advertising on handle 0 with flags, the ASCS+CAS UUID list, CAS service data with targeted-announcement byte, ASCS targeted-announcement and contexts (unicast build), BASS service data (broadcast build), and the complete device name `cap_acceptor`. The GAP/GATT device name is set to `CAP Acceptor`.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

Open menuconfig:

```bash
idf.py menuconfig
```

Under **Example: CAP Acceptor** -> **CAP Acceptor mode**:

* **Unicast** (`EXAMPLE_UNICAST`, default) — act as BAP Unicast Server; advertise CAS + ASCS for a CAP Initiator. Mutually exclusive with Broadcast.
* **Broadcast** (`EXAMPLE_BROADCAST`) — act as BAP Broadcast Sink with Scan Delegator; advertise CAS + BASS. Mutually exclusive with Unicast.

When **Broadcast** is selected, an additional option appears:

* **Scan for Broadcast Sources without Broadcast Assistant** (`EXAMPLE_SCAN_SELF`) — start scanning at boot for a source advertising the name `CAP Broadcast Source`; on match, create the PA sync directly. The hardcoded broadcast code `1234` is used if the BIG is encrypted. Without this option, a separate Broadcast Assistant must drive PA / BIS sync over BASS.

### Security & Pairing

Just-Works pairing (LE Secure Connections, no MITM, `BLE_SM_IO_CAP_NO_IO`) with bonding enabled, inherited from `../../common_components/example_init/ble_audio_example_init.c`. Change pairing or IO-cap there.

## Build & Flash

```bash
idf.py set-target esp32h4
idf.py -p PORT flash monitor
```

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. `app_main` initializes NVS, the Bluetooth controller (`bluetooth_init`) and the LE Audio common layer (`esp_ble_audio_common_init`) with GAP and GATT callbacks.
2. `init_cap_acceptor` registers PACS, registers sink+source PAC capabilities, sets PAC location (`FRONT_LEFT | FRONT_RIGHT`) and supported/available contexts.
3. The selected sub-mode initializes itself: `cap_acceptor_unicast_init` registers BAP unicast-server callbacks, CAP stream ops on the sink and source streams, and a TX scheduler; or `cap_acceptor_broadcast_init` registers BAP scan-delegator and broadcast-sink callbacks plus broadcast stream ops.
4. `esp_ble_audio_common_start` starts the audio stack and the device name is set to `CAP Acceptor`.
5. If `EXAMPLE_SCAN_SELF` is set, `check_start_scan` starts extended scanning for the broadcast source.
6. `ext_adv_start` configures and starts connectable extended advertising on handle 0 with the CAS / ASCS / BASS service data appropriate to the build.
7. **Unicast**: on ACL connect the connection handle is stored. On MTU change the acceptor starts service discovery (acting as GATT client). The unicast server then handles config -> reconfig -> qos -> enable -> start (auto Receiver Start Ready for sink ASEs) -> metadata -> disable -> stop -> release per ASE; when the source ASE starts, the TX scheduler begins sending dummy SDUs.
8. **Broadcast**: on `pa_sync_req` from a Broadcast Assistant (or on a scan match in self-scan mode) the acceptor creates a PA sync without PAST. On PA sync, it creates a broadcast sink for the broadcast ID, receives BASE and BIGInfo, then syncs to the first BIS. Stream `started` enters the synced state; `stopped` and `pa_sync_lost` clear flags and (in self-scan mode) restart scanning.
9. On ACL disconnect the connection handle is reset and `ext_adv_start` re-arms advertising.

## Expected Log

TAG: `CAP_ACC`.

### Unicast mode

```
I (xxx) CAP_ACC: Advertising started (handle 0)
I (xxx) CAP_ACC: Connected: handle ... role ... peer ...
I (xxx) CAP_ACC: MTU updated: handle ... mtu ...
I (xxx) CAP_ACC: Service discovery started: handle ...
I (xxx) CAP_ACC: Service discovery complete: handle ...
I (xxx) CAP_ACC: [SNK] Config request, codec cfg:
I (xxx) CAP_ACC: [SNK #0] Stream configured, QoS preference:
I (xxx) CAP_ACC: [SRC] Config request, codec cfg:
I (xxx) CAP_ACC: [SRC #0] Stream configured, QoS preference:
I (xxx) CAP_ACC: [SNK #0] QoS request:
I (xxx) CAP_ACC: [SNK #0] QoS set
I (xxx) CAP_ACC: [SRC #0] QoS request:
I (xxx) CAP_ACC: [SRC #0] QoS set
I (xxx) CAP_ACC: [SNK #0] Enable request (meta_len ...)
I (xxx) CAP_ACC: [SNK #0] Stream enabled
I (xxx) CAP_ACC: [SRC #0] Enable request (meta_len ...)
I (xxx) CAP_ACC: [SRC #0] Stream enabled
I (xxx) CAP_ACC: [SNK #0] Stream started
I (xxx) CAP_ACC: [SRC #0] Start request
I (xxx) CAP_ACC: [SRC #0] Stream started
```

On teardown / disconnect:

```
I (xxx) CAP_ACC: [SNK #0] Stop request
I (xxx) CAP_ACC: [SNK #0] Stream stopped, reason 0x...
I (xxx) CAP_ACC: [SNK #0] Release request
I (xxx) CAP_ACC: [SNK #0] Stream released
I (xxx) CAP_ACC: Sink stream released
I (xxx) CAP_ACC: Disconnected: handle ... reason 0x...
I (xxx) CAP_ACC: Advertising started (handle 0)
```

### Broadcast mode (Broadcast Assistant)

```
I (xxx) CAP_ACC: Advertising started (handle 0)
I (xxx) CAP_ACC: Connected: handle ... role ... peer ...
I (xxx) CAP_ACC: Receive state updated, pa_sync 0x... encrypt 0x...
I (xxx) CAP_ACC: Received request to sync to PA (PAST not available): ...
I (xxx) CAP_ACC: Syncing without PAST
I (xxx) CAP_ACC: PA sync ... synced for broadcast sink
I (xxx) CAP_ACC: Creating broadcast sink for broadcast ID 0x......
I (xxx) CAP_ACC: BASE received
I (xxx) CAP_ACC: BIGInfo received
I (xxx) CAP_ACC: BIS sync request received (src_id ...): 0x........
I (xxx) CAP_ACC: Syncing to broadcast with bitfield 0x........
I (xxx) CAP_ACC: [SNK #0] Stream started
```

### Broadcast mode (self-scan)

```
I (xxx) CAP_ACC: Advertising started (handle 0)
I (xxx) CAP_ACC: Scanning for broadcast source...
I (xxx) CAP_ACC: Syncing without PAST from scan
I (xxx) CAP_ACC: PA sync ... synced for broadcast sink
I (xxx) CAP_ACC: Creating broadcast sink for broadcast ID 0x......
I (xxx) CAP_ACC: BASE received
I (xxx) CAP_ACC: BIGInfo received
I (xxx) CAP_ACC: Syncing to broadcast with bitfield 0x........
I (xxx) CAP_ACC: [SNK #0] Stream started
```

On PA sync loss:

```
I (xxx) CAP_ACC: [SNK #0] Stream stopped, reason 0x...
I (xxx) CAP_ACC: PA sync lost: sync_handle ... reason 0x...
I (xxx) CAP_ACC: Scanning for broadcast source...
```

## Peer Pairing

Run the [initiator](../initiator/) on a second board. The initiator and acceptor must be configured for the **same sub-mode** — both `EXAMPLE_UNICAST`, or both `EXAMPLE_BROADCAST` — otherwise they will not pair.

### Unicast

1. Flash this acceptor with `EXAMPLE_UNICAST`; it advertises connectable with CAS + ASCS service data and PACS contexts.
2. Flash the initiator with `EXAMPLE_UNICAST`; it scans, finds CAS, connects and pairs.
3. The acceptor exchanges MTU, runs service discovery, then handles ASCS config / qos / enable for both sink and source ASEs.
4. The sink ASE auto-starts on enable (Receiver Start Ready); the source ASE starts on the initiator's `start` and the acceptor's TX scheduler begins.
5. Bidirectional audio runs until either side disconnects.

### Broadcast

1. Flash the initiator with `EXAMPLE_BROADCAST`; it advertises as `CAP Broadcast Source` (broadcast ID `0x123456`) and starts the BIG.
2. Flash this acceptor with `EXAMPLE_BROADCAST`. Either enable `EXAMPLE_SCAN_SELF` to self-scan for the source by name (broadcast code `1234`), or leave it disabled and use a separate Broadcast Assistant that connects via BASS to drive PA / BIS sync.
3. The acceptor PA-syncs, receives BASE and BIGInfo, syncs the first BIS, and `[SNK #0] Stream started`.
4. On PA sync loss the acceptor cleans up; in self-scan mode it restarts scanning.
