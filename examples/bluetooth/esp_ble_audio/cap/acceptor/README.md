| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# CAP Acceptor Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements the **Common Audio Profile (CAP) Acceptor** role on top of the NimBLE host stack with ISO and LE Audio support. It can be built with one or both roles selected at build time: a **CAP Acceptor / BAP Unicast Server**, and / or a **CAP Acceptor / BAP Broadcast Sink** (with the BAP Scan Delegator role enabled). Dual-role builds are supported (BAP spec C.2). PACS is registered in all configurations with LC3 sink and source capabilities (any sampling frequency, 7.5 ms or 10 ms frame, up to 2 channels, 30..155 octets per frame, up to 2 frames per SDU). Sink and source PAC location are set to `FRONT_LEFT | FRONT_RIGHT` (note in `main.c`: with `MONO_AUDIO`, Samsung S24 declines unicast).

In **unicast** mode (`cap_acceptor_unicast.c`) the acceptor registers BAP unicast-server callbacks (config / reconfig / qos / enable / start / metadata / disable / stop / release) and CAP stream ops, and on enable of a sink ASE automatically issues `bap_stream_start` (Receiver Start Ready). When the source ASE starts, an internal TX scheduler begins sending dummy SDUs. In **broadcast** mode (`cap_acceptor_broadcast.c`) the acceptor registers BAP scan-delegator and broadcast-sink callbacks, drives PA sync (preferring PAST when the Assistant supports it) on request, receives BASE / BIGInfo / broadcast code, then calls `esp_ble_audio_bap_broadcast_sink_sync` on the BIS bitmap selected by the Assistant via BASS Modify Source — up to `CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT` BIS streams. The Broadcast Sink object is created on PA sync and deleted on PA loss; Assistant pause / resume keep the same sink. See **Broadcast Mode Internals** below for sequence, gate flags and sink lifecycle. Optional **self-scan** lets the acceptor scan for a broadcast source named `CAP Broadcast Source` and use a hardcoded broadcast code `1234` instead of waiting for a Broadcast Assistant.

The acceptor advertises connectable extended advertising on handle 0 with flags, the ASCS+CAS UUID list, CAS service data with targeted-announcement byte, ASCS targeted-announcement and contexts (unicast build), BASS service data (broadcast build), and the complete device name `cap_acceptor`. The GAP/GATT device name is set to `CAP Acceptor`.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

Open menuconfig:

```bash
idf.py menuconfig
```

Under **Example: CAP Acceptor**:

* **Unicast** (`EXAMPLE_UNICAST`, default) — act as BAP Unicast Server; advertise CAS + ASCS for a CAP Initiator.
* **Broadcast** (`EXAMPLE_BROADCAST`) — act as BAP Broadcast Sink with Scan Delegator; advertise CAS + BASS.

The two options can be enabled together for a dual-role acceptor (BAP spec C.2). When **Broadcast** is selected and **Unicast** is disabled (dual-role + self-scan coexistence is not yet supported), an additional option appears:

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
8. **Broadcast**: on `pa_sync_req` the acceptor establishes PA sync — preferring HCI Periodic Advertising Sync Transfer (PAST) when the Assistant supports it, otherwise scanning for the source by address / SID. On PA sync the sink is created (inlined in `broadcast_pa_synced`) and lives until both PA and BIS are gone. BASE, BIGInfo and (if the BIG is encrypted) the broadcast code accumulate as gate flags. When the Assistant's BASS Modify Source pushes a non-zero `bis_sync` bitmap and all gates are open, `bap_broadcast_sink_sync` is called with a `streams[]` of size = popcount of the bitmap (≤ `CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT`). Assistant pause (`bis_sync = 0`) issues `_stop` but **does not delete** the sink; resume re-`_sync`s the same object so the cached BIGInfo / BASE / QoS stay valid. PA loss does **not** tear down the BIG — per BASS spec the two are independent. See **Broadcast Mode Internals** below for the full sequence and state machine.
9. On ACL disconnect the connection handle is reset and `ext_adv_start` re-arms advertising.

## Broadcast Mode Internals

The broadcast acceptor coordinates three roles — Scan Delegator (BASS server), Broadcast Sink (BAP), and PA / BIG sync at the LE controller — under a small atomic-flag state machine. `check_sync_broadcast()` is the single fan-in that calls `bap_broadcast_sink_sync` once all gates are open; each callback that sets a gate invokes it.

### End-to-end sequence (Assistant-driven)

```
Source (SRC)        Assistant (ASS)            Acceptor (ACC)
   |                     |                          |
   |                     |---- ACL connect+pair --->|
   |                     |---- BASS Add Source ---->|  pa_sync_req_cb
   |                     |     (id, addr, sid)      |
   |                     |                          |
   |                                                |
   |    ----- PAST supported (peer + local) ----    |
   |                     |<--- PA state=INFO_REQ ---|  set after subscribe
   |                     |- HCI PA Sync Transfer -->|
   |                                                |
   |    ----------- else (no PAST) -------------    |
   |<------- periodic_adv_sync_create (scan) -------|
   |                                                |
   |============== PA sync established ============>|  broadcast_pa_synced
   |                                                |  -> create_broadcast_sink
   |============== PA report (BASE) ===============>|  base_recv_cb
   |                                                |  -> FLAG_BASE_RECEIVED
   |============== PA report (BIGInfo) ============>|  syncable_cb
   |                                                |  -> FLAG_BROADCAST_SYNCABLE
   |                                                |
   |    ------------ BIG encrypted -------------    |
   |                     |---- BASS Set BCode ----->|  broadcast_code_cb
   |                     |                          |  -> FLAG_..._CODE_RECEIVED
   |                                                |
   |                     |---- BASS Modify Source ->|  bis_sync_req_cb
   |                     |     (bis_sync bitmap)    |  -> FLAG_..._SYNC_REQUESTED
   |                                                |  check_sync_broadcast -> _sync
   |<========= HCI LE BIG Create Sync ==============|
   |                                                |
   |========== BIG Sync Established ===============>|  stream_started_cb
   |                                                |  -> FLAG_BROADCAST_SYNCED
   |============== ISO data (BIS) =================>|
```

### Sync-gate flags

`check_sync_broadcast()` short-circuits on the first unset gate and logs which one it waited on. All gates use atomic bit ops so callbacks may run on any thread without ordering tricks.

| Flag                              | Set by                                                              | Cleared by                                                                                        |
| --------------------------------- | ------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------- |
| `FLAG_PA_SYNCED`                  | `broadcast_pa_synced`                                               | `broadcast_pa_lost`                                                                               |
| `FLAG_BROADCAST_SYNCABLE`         | `syncable_cb` (BIGInfo)                                             | `broadcast_pa_lost` (BIS idle); `broadcast_sink_reset`                                            |
| `FLAG_BASE_RECEIVED`              | `base_recv_cb` (first BASE)                                         | `broadcast_pa_lost` (BIS idle); `broadcast_sink_reset`                                            |
| `FLAG_BROADCAST_CODE_REQUIRED`    | `syncable_cb` (`biginfo->encryption=1`)                             | `syncable_cb` (encryption=0); `broadcast_pa_lost` (BIS idle); `broadcast_sink_reset`              |
| `FLAG_BROADCAST_CODE_RECEIVED`    | `broadcast_code_cb` (BASS Set Broadcast Code); self-scan local code | `broadcast_sink_reset`                                                                            |
| `FLAG_BROADCAST_SYNC_REQUESTED`   | `bis_sync_req_cb` (bitmap ≠ 0); self-scan PA match                  | `bis_sync_req_cb` (bitmap = 0); `broadcast_sink_reset`                                            |
| `FLAG_BROADCAST_RESYNC_PENDING`             | `bis_sync_req_cb` before `_stop` (bitmap change while streaming)    | `stream_stopped_cb` after driving the re-sync; `_stop` failure; `broadcast_sink_reset`            |
| `FLAG_BROADCAST_SYNCING`          | `check_sync_broadcast` after `_sync` returns OK                     | `stream_started_cb`; `stream_stopped_cb`                                                          |
| `FLAG_BROADCAST_SYNCED`           | `stream_started_cb`                                                 | `stream_stopped_cb`                                                                               |

`check_sync_broadcast()` runs `_sync` only when:

```
BASE_RECEIVED && BROADCAST_SYNCABLE
&& (!CODE_REQUIRED || CODE_RECEIVED)
&& BROADCAST_SYNC_REQUESTED
&& PA_SYNCED
&& !(SYNCED || SYNCING)
```

### Sink object lifecycle

```
              +---------+
   [start] -->| Created |
              +----+----+
                   |
                   | check_sync_broadcast -> _sync
                   v
              +---------+   Modify Source: new   +---------+
              | Syncing |<------------------------| Stopped |
              +----+----+         bis_sync       +----^----+
                   |                                  |
                   | stream_started_cb                | Assistant bis_sync = 0
                   v                                  | (source pause) OR
              +-----------+                           | bitmap change -> _stop
              | Streaming |---------------------------+
              +-----+-----+                           |
                    |                                 | BIG drops while PA gone
                    |                                 v
                    +---------> stream_stopped_cb + !PA_SYNCED
                                     |
                                     | _delete + broadcast_sink_reset
                                     v
                                   [end]
```

Key invariants:

- **PA loss does NOT tear down a running BIS.** Per BASS § 3.2.1.6 / § 3.2.1.9, `PA_Sync_State` and `BIS_Sync_State` are independent. While BIS is streaming/syncing, `broadcast_pa_lost` only notifies the assistant (`PA_Sync_State = 0x00`) and clears PA-only local state (`sync_handle`, `FLAG_PA_SYNCED`); the BIG keeps running and audio continues to flow.
- **PA loss with BIS idle tears down the sink.** The sink is bound to the now-dead sync handle and its cached BASE / BIGInfo are stale. `broadcast_pa_lost` calls `_delete` and clears `FLAG_BASE_RECEIVED` / `FLAG_BROADCAST_SYNCABLE` / `FLAG_BROADCAST_CODE_REQUIRED`. The assistant's subscription (`requested_bis_sync`, `FLAG_BROADCAST_SYNC_REQUESTED`, `FLAG_BROADCAST_CODE_RECEIVED`) is preserved so the next PA sync re-creates a fresh sink and resumes streaming.
- **Sink deletion happens in `stream_stopped_cb` when both PA and BIS are gone.** Triggers: assistant unsubscribes via `Modify Source bis_sync = 0`, or the broadcaster stops the BIG while PA is already gone.
- `bis_sync_req_cb` going `X → 0` (Assistant pause) only issues `_stop`, never `_delete`. Going `X → Y` (BIS bitmap switch) likewise only `_stop`s; the next `check_sync_broadcast` (called from `stream_stopped_cb` when PA still synced) re-`_sync`s the same object.
- `pa_sync_term_req_cb` issues the HCI Periodic Advertising Terminate Sync but does **not** clear `broadcast_sink.sync_handle`. Cleanup runs from `BLE_GAP_EVENT_PERIODIC_SYNC_LOST` → `broadcast_pa_lost`. Resetting the handle early would make that gate miss.

### Event handling

| Event                                                        | Action                                                                                                       |
| ------------------------------------------------------------ | ------------------------------------------------------------------------------------------------------------ |
| **PA lost while BIS active** (broadcaster moved mid-stream)  | Set `PA_Sync_State = 0x00`; clear PA-only state (`sync_handle`, `FLAG_PA_SYNCED`). Sink + BIS untouched; BIG keeps running. |
| **PA lost while BIS idle** (Assistant `PA_Sync = 0`, or PA dropped after Assistant paused BIS) | Set `PA_Sync_State = 0x00` (skipped if BASS already updated it in-place); clear PA-only state; `_delete` the sink and clear `FLAG_BASE_RECEIVED` / `FLAG_BROADCAST_SYNCABLE` / `FLAG_BROADCAST_CODE_REQUIRED`. The next PA sync starts from a clean sink and lets the lib redeliver BASE / BIGInfo. |
| **Modify Source `bis_sync = 0`** (PA still synced)           | `bis_sync_req_cb` clears `FLAG_BROADCAST_SYNC_REQUESTED` then `_stop`s the BIG. Sink retained.               |
| **Modify Source bitmap change** (PA still synced, streaming) | Update `requested_bis_sync` + `FLAG_BROADCAST_SYNC_REQUESTED` + set `FLAG_BROADCAST_RESYNC_PENDING`, then `_stop`. `stream_stopped_cb` clears the flag and re-`_sync`s with the new bitmap. |
| **BIG drops while PA still synced** (e.g. broadcaster pause) | `stream_stopped_cb` clears SYNCED/SYNCING and exposes the loss via `BIS_Sync_State`. `FLAG_BROADCAST_RESYNC_PENDING` is not set, so no auto-retry — per BASS § 3.2.1.9 the assistant drives recovery via Modify Source. |
| **BIG drops after PA lost** (broadcaster turned off)         | `stream_stopped_cb` of the last active stream sees `!PA_SYNCED` → `_delete` + `broadcast_sink_reset`. Multi-BIS: earlier callbacks just decrement `active_streams` so `_delete` is not called while the sink is still in use. |
| **Assistant Remove Source**                                  | Spec allows only when BIS not synced; lib handles, app sees no special event.                                |

Two recurring patterns that drive the above behavior:

- **Update local state before calling `_stop`/`_delete`.** The lib may fire `stream_stopped_cb` synchronously from within `_stop`, so the callback must see the post-stop state. Applies in `bis_sync_req_cb` (updates `requested_bis_sync` + flag before `_stop`) and `broadcast_pa_lost` (no longer calls `_stop`).
- **Sink lifetime is BIS-driven, not PA-driven.** Sink is created on first PA sync and deleted only when the BIG itself stops and PA is also gone. This matches BASS spec's independent PA/BIS state model.

### Multi-BIS (stereo) configuration

`CONFIG_BT_BAP_BROADCAST_SNK_STREAM_COUNT` selects how many BIS streams the acceptor can sync to simultaneously. The `broadcast_sink.cap_streams[N]` array is sized to this value; `bis_sync_req_cb` rejects bitmaps whose popcount exceeds it (`-ENOMEM` back to BASS), and `check_sync_broadcast` builds the `streams[]` argument from the requested bitmap. `broadcast_sink.active_streams` tracks how many of those BIS are currently streaming so that `_delete` / re-sync only run after the *last* stream's `stopped_cb` — calling them while another BIS is still active hits the lib's `BapBsnkNotIdle` path.

For Auracast stereo against a Samsung Galaxy (typical bitmap `0x3` = BIS 1 + BIS 2), set this to `2` in the sdkconfig.

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
