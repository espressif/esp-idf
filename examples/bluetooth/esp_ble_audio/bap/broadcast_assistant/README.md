| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BAP Broadcast Assistant Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements the **BAP Broadcast Assistant** role on top of the selected BLE host stack (Bluedroid by default; NimBLE via the `sdkconfig.defaults.nimble` overlay) with ISO and LE Audio support. It never renders or transmits audio itself — it is the device that *tells someone else what to listen to*. In order: it scans for a connectable advertiser exposing the Broadcast Audio Scan Service (a Scan Delegator), connects and bonds with it, discovers BASS and reads back every Broadcast Receive State; it then scans for a Broadcast Audio Announcement, periodic-advertising-syncs to that source itself so it can decode the BASE, and finally writes a BASS **Add Source** naming the exact BIS indexes and metadata it found. If the sink takes up the offer to have that sync transferred (PAST) rather than scan for the source itself, the assistant hands it over. If the sink reports that the stream is encrypted and it has no key, the assistant pushes the Broadcast Code over BASS. The GAP device name is set to `BAP Broadcast Assistant`; the assistant is a scanner and initiator only and never advertises.

Design notes:

* Every stage advances from a GAP/GATT/BASS callback rather than blocking `app_main()`, matching the other examples in this directory.
* A sink is matched on BASS alone — BASS in the advertised UUID list is what identifies a Scan Delegator, and PACS is not required.
* An encrypted broadcast is handled end to end: a receive state reporting `BCODE_REQ` triggers `esp_ble_audio_bap_broadcast_assistant_set_broadcast_code()`, so the sink needs no pre-provisioned key.
* The sync used to read the BASE is reusable: a receive state reporting `INFO_REQ` triggers a Periodic Advertising Sync Transfer of that same handle, so the sink never has to find the source on air.

APIs used: `esp_ble_audio_common_init` / `_start`, `esp_ble_audio_gattc_disc_start`, `esp_ble_audio_bap_broadcast_assistant_register_cb` / `_discover` / `_read_recv_state` / `_scan_start` / `_scan_stop` / `_add_src` / `_set_broadcast_code`, and the BASE readers `esp_ble_audio_bap_base_get_base_from_ad` / `_get_subgroup_count` / `_foreach_subgroup` / `_subgroup_get_bis_indexes` / `_subgroup_codec_to_codec_cfg`.

Host-specific GAP plumbing (extended scan, ACL create, pairing, MTU, PA sync, PAST send) lives in `main/bluedroid/central.c` and `main/nimble/central.c`; `main.c` only sees the host-agnostic interface in `assistant.h`.

## Requirements

* A board with Bluetooth LE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* A **Broadcast Source** peer — the [broadcast_source](../broadcast_source) example
* A **Scan Delegator** peer — the [broadcast_sink](../broadcast_sink) example built with `EXAMPLE_SCAN_OFFLOAD=y` (it is off by default and the sink does not advertise without it), or [cap/acceptor](../../cap/acceptor), which always advertises but needs `TARGET_SINK_NAME` changed to match its name

> PAST is built in on both hosts (`CONFIG_BT_BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER` / `CONFIG_BT_NIMBLE_PERIODIC_ADV_SYNC_TRANSFER`). Because this example already holds a sync to the source — it syncs locally to read the BASE — the library offers `PA_Sync = "PAST available"` (0x01) in Add Source, and a delegator that takes the offer up gets that very sync handed over instead of scanning for the source itself. A delegator that would rather establish its own sync is free to ignore the offer; BASS § 3.1.1.4 allows either, and `bap/broadcast_sink` does exactly that.

## Configuration

The two peers and the Broadcast Code are `#define`s at the top of `main/main.c`:

| Macro | Default | Meaning |
| --- | --- | --- |
| `TARGET_SINK_NAME` | `BAP Broadcast Sink` | Only connect to a sink whose advertised name contains this substring. Set it to `CAP Acceptor` for [cap/acceptor](../../cap/acceptor), or empty to accept any BASS advertiser. |
| `TARGET_SOURCE_NAME` | `BAP Broadcast Source` | Only add a source whose advertised or broadcast name contains this substring. Empty accepts any Broadcast Audio Announcement. |
| `TARGET_BROADCAST_CODE` | `1234` | Sent with BASS Set Broadcast Code; must match what the source encrypts with. |

Matching is a case-insensitive substring test (`example_is_substring()`), so a shorter fragment works too. The defaults pair the example with this repo's [broadcast_sink](../broadcast_sink) and [broadcast_source](../broadcast_source) out of the box.

Two sdkconfig values bound what a BASE may contain: `CONFIG_BT_BAP_BASS_MAX_SUBGROUPS` (default 2) bounds the subgroup array written into Add Source, and `CONFIG_BT_AUDIO_CODEC_CFG_MAX_METADATA_SIZE` (default 60) bounds the per-subgroup metadata copy. A BASE that exceeds either is truncated with a warning rather than rejected.

### PAST combinations

Whether the SyncInfo is transferred over the ACL or found on air is a build-time
choice on **both** sides: `EXAMPLE_PAST` here and `EXAMPLE_PAST` in
[broadcast_sink](../broadcast_sink) (which in turn needs `EXAMPLE_SCAN_OFFLOAD`).

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

Just-Works pairing (LE Secure Connections, no MITM, no I/O capability) with bonding, inherited from `../../common_components/example_init/ble_audio_example_init.c`. BASS characteristics require an encrypted link, so pairing is not optional here.

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

1. `app_main` initializes NVS, `bluetooth_init()`, `app_host_init()`, then `esp_ble_audio_common_init(&info)` with both `gap_cb` and `gatt_cb`. `esp_ble_audio_bap_broadcast_assistant_register_cb(&assistant_cbs)` registers the BASS client callbacks before `esp_ble_audio_common_start(NULL)`.
2. `scan_restart(SCAN_MODE_SINK)` starts a passive extended scan. `sink_data_cb` accepts a connectable advertiser whose 16-bit UUID list contains BASS and whose name matches `TARGET_SINK_NAME`.
3. On a hit the scanner is stopped first (NimBLE rejects `ble_gap_connect` during discovery) and `conn_create()` runs. `acl_connect` → `pairing_start()` → `security_change` → `exchange_mtu()` → `gatt_mtu_change` → `esp_ble_audio_gattc_disc_start()` → `gattc_disc_cmpl`. MTU-updated and discovery-complete can arrive in either order, so both set a flag and the second one calls `bass_discover()`.
4. `assistant_discover_cb` reports the receive state count; `read_next_recv_state()` walks every index through `esp_ble_audio_bap_broadcast_assistant_read_recv_state()`. A `NULL` state is an empty slot, not an error.
5. Once all states are read, `esp_ble_audio_bap_broadcast_assistant_scan_start(conn, false)` sends the BASS *Remote Scan Started* opcode. `start_scan` is `false` on purpose: the example drives its own extended scanner, and letting the library start one too would have the two fight over the controller.
6. `scan_restart(SCAN_MODE_SOURCE)` re-arms the scanner. `source_data_cb` matches the Broadcast Audio Announcement service data (24-bit Broadcast ID) plus `TARGET_SOURCE_NAME`. The advertiser address, SID, Broadcast ID and the **real** periodic interval are cached from the scan report.
7. `pa_sync_create()` syncs locally so the BASE can be read. `ESP_BLE_AUDIO_GAP_EVENT_PA_SYNC_RECV` feeds each periodic advertising report to `esp_ble_audio_data_parse()`; `base_store_cb` calls `esp_ble_audio_bap_base_get_base_from_ad()` and, on the first BASE, walks the subgroups.
8. Per subgroup, `_subgroup_get_bis_indexes()` supplies `bis_sync` and `_subgroup_codec_to_codec_cfg()` decodes the codec configuration and metadata into buffers the caller supplies through `codec_cfg.data` / `.meta` — see the invariant below, a zeroed `codec_cfg` faults.
9. `add_source()` writes BASS Add Source with `pa_sync = true`, the measured `pa_interval`, and the filled subgroup array. On success `assistant_add_src_cb` sends *Remote Scan Stopped*.
10. `assistant_recv_state_cb` then tracks the sink's progress. `pa_sync_state == INFO_REQ` means the sink took up the PAST offer, and `send_past()` transfers the local sync handle with the Source_ID in the high octet of the service data. `encrypt_state == BCODE_REQ` triggers `send_broadcast_code()`; `BAD_CODE` retries once (latched, so a permanently-wrong code does not loop).
11. Teardown: `acl_disconnect` terminates the local PA sync, clears state and goes back to scanning for a sink. `pa_sync_lost` clears only the source-side state and, while the delegator link is up, resumes scanning for a source.

## Assistant Internals

The assistant is a linear pipeline — find sink, find source, describe source to sink — but it runs entirely out of callbacks and has to time-share **one** extended scanner between the two search phases. Everything below is that arbitration plus the latches that keep each stage from re-firing.

### End-to-end sequence

```
Source (BSRC)      Assistant (BA)          Delegator (BSNK)
   |                      |                        |
   |                      |<--- connectable adv ---|  scan_mode = SINK
   |                      |                        |  sink_data_cb: BASS in UUID list
   |                      |--- ACL connect+pair -->|
   |                      |--- MTU + GATT disc --->|
   |                      |--- BASS discover ----->|  assistant_discover_cb
   |                      |<-- Receive State x N --|  read_next_recv_state
   |                      |--- Remote Scan Start ->|  scan_start(conn, false)
   |                      |                        |
   |--- Broadcast Audio ->|                        |  scan_mode = SOURCE
   |    Announcement      |                        |  source_data_cb
   |<===== PA sync =======|                        |  pa_sync_create
   |===== BASE report ===>|                        |  base_store_cb -> subgroups[]
   |                      |--- BASS Add Source --->|  add_source()
   |                      |    addr/sid/id/        |
   |                      |    pa_interval/        |
   |                      |    bis_sync+metadata   |
   |                      |<-- Add Source rsp -----|  assistant_add_src_cb
   |                      |--- Remote Scan Stop -->|
   |                      |                        |
   |   ------- delegator took PAST offer -------   |
   |                      |<-- Receive State ------|  pa = INFO_REQ
   |                      |--- PAST (over ACL) --->|  send_past()
   |                      |                        |
   |    ------------ BIG encrypted ------------    |
   |                      |<-- Receive State ------|  enc = BCODE_REQ
   |                      |--- BASS Set BCode ---->|  send_broadcast_code
   |                      |                        |
   |<================== BIG sync ==================|  delegator's own — we
   |                      |                        |  only supplied the code
   |                      |<-- Receive State ------|  enc = DECRYPTING, bis_sync != 0
```

### Scanner arbitration

`scan_mode` decides which report handler `ext_scan_recv()` dispatches to, and `SCAN_MODE_IDLE` is what keeps a stale report from starting a second connect or a second PA sync while the first is still in flight.

| `scan_mode` | Scanner | Handler | Leaves on |
| --- | --- | --- | --- |
| `SCAN_MODE_IDLE` | stopped | — | `scan_restart()` from a failure or teardown path |
| `SCAN_MODE_SINK` | running | `ext_scan_recv_sink` | connectable + BASS + name match → stop scan, `IDLE`, `conn_create()` |
| `SCAN_MODE_SOURCE` | running | `ext_scan_recv_source` | Broadcast ID + name match + `per_adv_itvl != 0` → stop scan, `IDLE`, `pa_sync_create()` |

Three things keep the single scanner from being contended:

- The BASS *Remote Scan Started* write passes `start_scan = false`, so the library never opens a scanner of its own.
- Both handlers call `ext_scan_stop()` **before** the operation they start, because NimBLE refuses `ble_gap_connect` while a discovery procedure is running.
- `ext_scan_recv_source` drops reports with `per_adv_itvl == 0` outright: without a periodic train there is no BASE to read and nothing for the delegator to sync to.

### Progress gates

| Variable | Set by | Cleared by |
| --- | --- | --- |
| `conn_handle` | `acl_connect` | `acl_disconnect` |
| `mtu_exchanged` | `gatt_mtu_change` (MTU ≥ `ESP_BLE_AUDIO_ATT_MTU_MIN`) | `acl_disconnect` |
| `disc_completed` | `gattc_disc_cmpl` | `acl_disconnect` |
| `recv_states_synced` | `read_next_recv_state` once the last index is read | `assistant_discover_cb`; `acl_disconnect` |
| `pa_syncing` | `ext_scan_recv_source` after `pa_sync_create()` | `pa_sync` (success and failure); `reset_source_state` |
| `sync_handle` | `pa_sync` (success) | `reset_source_state` |
| `base_received` | `base_store_cb`, only once `add_source()` returned 0 | `assistant_add_src_cb` on failure; `reset_source_state` |
| `subgroup_count` | `base_subgroup_cb`, one per subgroup | `base_store_cb` before each walk; `reset_source_state` |
| `code_attempts` | `send_broadcast_code()` | `assistant_recv_state_removed_cb`; `reset_source_state` |

`reset_source_state()` is the source-side half of teardown and runs from both `acl_disconnect` and `pa_sync_lost`; the delegator-side gates above it are cleared only by `acl_disconnect`.

### Key invariants

- **The local PA sync reads the BASE and is then reusable.** The assistant never creates a BIG sync and never renders audio. Reading the BASE is what lets Add Source name exact BIS indexes instead of `BIS_SYNC_NO_PREF`, and holding that sync is also what makes the library offer PAST — `past_available()` requires a sync to this `{address, SID}` plus PAST_SEND built in.
- **The PA sync outlives Add Source.** Nothing terminates it on success — `acl_disconnect` does, and `pa_sync_lost` reports it going away on its own. Holding it is what allows a retry from the next periodic advertising report.
- **Byte-order conversion touches the BASS parameter only.** GAP hands out addresses in the host's own order (Bluedroid MSB-first, NimBLE on-air/LSB-first) and `pa_sync_create()` wants those bytes unchanged; `addr_host_to_le()` / `addr_type_host_to_le()` in `main.c` convert only what goes into the Add Source PDU, which is always on-air order. Per BASS § 3.1.1.4 the type collapses to two values — `0x00` public (device *or* identity), `0x01` random (device *or* static identity).
- **`codec_cfg.data` and `.meta` are the caller's buffers.** In this port they are pointers, not arrays inside `esp_ble_audio_codec_cfg_t`, and `_subgroup_codec_to_codec_cfg()` memcpy's into both unconditionally — passing a zeroed struct stores to address 0. It also bounds-checks the decoded lengths against `CONFIG_BT_AUDIO_CODEC_CFG_MAX_DATA_SIZE` / `_MAX_METADATA_SIZE` rather than against the buffers it was given, so a smaller buffer would overflow instead of being rejected. `base_subgroup_cb()` therefore points `.meta` straight at this subgroup's `subgroup_meta[]` slot (which is exactly the Kconfig size, and is where Add Source wants the bytes anyway) and `.data` at a shared scratch buffer, since the codec configuration LTVs are decoded but not carried in Add Source.
- **The BASE is acted on once per source.** `pa_sync_recv` returns early unless the report's `sync_handle` matches and `base_received` is clear, so the periodic train's repetition does not re-issue Add Source.
- **A failed Add Source un-latches instead of giving up.** `assistant_add_src_cb` clears `base_received`, so the next periodic advertising report rebuilds the subgroups and writes again.
- **The Broadcast Code is latched at `BROADCAST_CODE_MAX_ATTEMPTS` (2).** The sink republishes its receive state on every change, so a permanently wrong code would otherwise loop forever: one send on `BCODE_REQ`, one retry on `BAD_CODE`, then silence. `recv_state_removed` resets the counter for the next source.

### Event handling

| Event | Action |
| --- | --- |
| `EXT_SCAN_RECV` | Dispatched by `scan_mode`; ignored in `SCAN_MODE_IDLE`. |
| `ACL_CONNECT` with `status != 0` | `scan_restart(SCAN_MODE_SINK)`. `ACL_DISCONNECT` never fires for a link that was never established, so the retry has to happen here. |
| `ACL_DISCONNECT` | Clear every delegator-side gate, terminate the local PA sync if one is held, `reset_source_state()`, back to `SCAN_MODE_SINK`. |
| `SECURITY_CHANGE` with `status != 0` | `security_failed_recover()` — the peer cleared its side of an existing bond, so encrypt-with-cached-key times out; drop the local bond and disconnect, and the next connection pairs fresh. |
| `PA_SYNC` failure | `scan_restart(SCAN_MODE_SOURCE)`; the source may still be there on the next pass. |
| `PA_SYNC_RECV` | Ignored unless `sync_handle` matches and `base_received` is clear. |
| Receive State with `pa_sync_state == INFO_REQ` | `send_past()` transfers the local sync handle over the ACL. Warns instead if the sync has since been lost. |
| `PA_SYNC_LOST` | Source-side state only. Re-scans for a source if the delegator link is still up; otherwise `acl_disconnect` has already re-armed the sink scan. |
| `GATT_MTU_CHANGE` / `GATTC_DISC_CMPL` | Either order. Each sets its flag and the second one calls `bass_discover()`. An MTU below `ESP_BLE_AUDIO_ATT_MTU_MIN` warns and stops the chain. |

## Expected Log

TAG: `BAP_BA`.

```
I (xxx) BAP_BA: Scanning for broadcast sink...
I (xxx) BAP_BA: Broadcast sink found: xx:xx:xx:xx:xx:xx
I (xxx) BAP_BA: Connected: handle 0 peer xx:xx:xx:xx:xx:xx
I (xxx) BAP_BA: Security: handle 0 level 2 bonded 1
I (xxx) BAP_BA: MTU updated: handle 0 mtu 517
I (xxx) BAP_BA: Service discovery complete: handle 0
I (xxx) BAP_BA: BASS discovered, 2 receive state(s)
I (xxx) BAP_BA: Receive state empty
I (xxx) BAP_BA: Remote Scan Started: ok
I (xxx) BAP_BA: Scanning for broadcast source...
I (xxx) BAP_BA: Broadcast source found: id 0x123456 sid 0 pa_interval 600
I (xxx) BAP_BA: PA synced: sync_handle 0, waiting for BASE
I (xxx) BAP_BA: BASE received (1 subgroup(s))
I (xxx) BAP_BA: Subgroup 0: bis_sync 0x00000006 meta_len 4
I (xxx) BAP_BA: Add Source sent: id 0x123456 sid 0 pa_interval 600 subgroups 1
I (xxx) BAP_BA: Add Source OK
I (xxx) BAP_BA: Remote Scan Stopped: ok
I (xxx) BAP_BA: Receive state: src_id 1 id 0x123456 pa 0x01 enc 0x00
I (xxx) BAP_BA: SyncInfo transferred for src_id 1
I (xxx) BAP_BA: Receive state: src_id 1 id 0x123456 pa 0x02 enc 0x01
I (xxx) BAP_BA: Broadcast code sent for src_id 1
I (xxx) BAP_BA: Set Broadcast Code: ok
I (xxx) BAP_BA: Receive state: src_id 1 id 0x123456 pa 0x02 enc 0x02
I (xxx) BAP_BA:   subgroup 0 bis_sync 0x00000006
```

`enc` values are `0x01` = code required, `0x02` = decrypting, `0x03` = bad code. `pa 0x01` is *SyncInfo Request* (the sink wants PAST), `0x02` is PA synced. A sink that establishes its own sync goes straight to `0x02` and the `SyncInfo transferred` line does not appear.

On PA sync loss with the delegator still connected:

```
I (xxx) BAP_BA: PA sync lost: sync_handle 0 reason 0x...
I (xxx) BAP_BA: Scanning for broadcast source...
```

## Peer Pairing

Three boards:

| Board | Example |
| --- | --- |
| A | [broadcast_source](../broadcast_source) |
| B | [broadcast_sink](../broadcast_sink) with `EXAMPLE_SCAN_OFFLOAD=y` (or [cap/acceptor](../../cap/acceptor)) |
| C | this example |

Expected interaction:

1. A advertises the Broadcast Audio Announcement (Broadcast ID `0x123456`, name `"BAP Broadcast Source"`) and a periodic train carrying the BASE; its BIG is encrypted with `"1234"`.
2. C finds B by its advertised BASS UUID, connects, bonds, and discovers BASS.
3. C finds A, PA-syncs, decodes the BASE, and writes Add Source to B with the exact BIS bitfield.
4. B PA-syncs to A on its own, reports `BCODE_REQ`; C answers with the Broadcast Code.
5. B synchronizes to the BIG and starts receiving — on B you should see its BIS-sync request, `Broadcast code received`, and stream-started logs.

### Troubleshooting

* **`Add Source failed`** — most often the announced `pa_interval` or the advertiser address. This example takes the interval live from the scan report; sending `ESP_BLE_AUDIO_BAP_PA_INTERVAL_UNKNOWN` (`0xFFFF`) instead makes delegators that validate it reject the write. The address written into BASS is always on-air (LSB-first) order, which is a byte reversal of what a Bluedroid GAP event hands out — see `addr_host_to_le()` in `main.c`.
* **Sink stays at `enc 0x03`** — the Broadcast Code does not match the source. It is left-aligned and zero-padded to 16 octets over BASS; note this is the opposite convention to the HCI Create BIG / Create BIG Sync path, which takes the code byte-reversed. Only two sends are attempted per source, so fix the code and reconnect rather than waiting for another retry.
* **Nothing found in `SCAN_MODE_SINK`** — either the peer is not advertising (check that `bap/broadcast_sink` was built with `EXAMPLE_SCAN_OFFLOAD` enabled) or its name does not contain `TARGET_SINK_NAME`.
* **Nothing found in `SCAN_MODE_SOURCE`** — the source must be running a periodic advertising train; reports with `per_adv_itvl == 0` are dropped before the name and Broadcast ID are even looked at.
* **Add Source accepted, sink reports `pa 0x01` and stops** — `0x01` is *SyncInfo Request*: it took up the PAST offer and is waiting for the transfer. Look for `SyncInfo transferred for src_id N` right after; if it says the PA sync is gone instead, the local sync was lost before the sink asked.
