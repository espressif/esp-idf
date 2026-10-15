| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BAP Unicast Client Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements a **BAP Unicast Client** on top of the selected BLE host stack (Bluedroid by default; NimBLE via the `sdkconfig.defaults.nimble` overlay). It scans for a peer advertising the ASCS service UUID with a complete local name of `BAP Unicast Server`, opens an ACL connection, initiates pairing, exchanges MTU, and starts GATT service discovery.

Once the peer is reachable, the client uses the BAP unicast client API (`esp_ble_audio_bap_unicast_client_discover`, `esp_ble_audio_bap_stream_config`, `esp_ble_audio_bap_unicast_group_create`, `esp_ble_audio_bap_stream_qos`, `esp_ble_audio_bap_stream_enable`, `esp_ble_audio_bap_stream_connect`, `esp_ble_audio_bap_stream_start`) to discover sink and source ASEs, configure each with the LC3 `16_2_1` unicast preset, build a unicast group, set QoS, enable, and ISO-connect the streams. Source (TX-from-server) streams are started by this client; sink (TX-to-server) streams are started by the server.

The TX path in `stream_tx.c` registers each newly started sink stream with one `example_audio_tx_scheduler` for the unicast group, which sends one SDU-sized buffer on every registered stream per SDU interval via `esp_ble_audio_bap_stream_send` (or `esp_ble_audio_bap_stream_send_ts` in timestamp mode). The PACS callbacks (`pac_record`, `endpoint`, `location`, `available_contexts`) print the discovered peer capabilities.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

Open menuconfig:

```bash
idf.py menuconfig
```

Apart from the option below, runtime defaults are baked into source.

Under **Example: Common Audio Utilities**, enable **Synchronize multi-stream TX with
timestamps** (`CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP`, disabled by default) to give the
SDUs of all sink streams of the unicast group the same timestamps, so that the SDUs of
one frame (e.g. the left and right audio) go in one event:

- One TX scheduler for the unicast group sends one frame per SDU interval: one SDU on
  each started stream, all with the same timestamp.
- At the start only the first stream sends, without timestamps, and reads
  **LE Read ISO TX Sync** (`esp_ble_audio_bap_stream_get_tx_sync()`) after each
  send until it finds the SDU at the start of an event; the timestamps go on from
  that event's anchor. The read is a blocking HCI command of about 2.5 ms, so it
  comes after the send.
- A stream started later joins at the next frame, with the same timestamp.
- The best lead (how long before its reference time an SDU is handled) of every
  50 SDUs follows the clock difference between the timer and the controller:
  below 200 us the scheduler adds one frame, above two SDU intervals more it
  skips one. A stall lowers only some SDUs and changes nothing; a best lead
  below minus one SDU interval is a stall or a backlog, and no frame is added.

In this mode the first 4 bytes of each SDU carry its timestamp (0 without one),
and unframed data is required. With the option disabled, the same scheduler sends
one SDU per stream per interval without timestamps. Logs on anomalies:

```
I BAP_UCL: [SNK #<n>] TX: <N> packets, <k> late
W BAP_UCL: [SNK #<n>] Send failed, err <e>
E BAP_UCL: [SNK #<n>] Timestamp TX start failed, err <e>
W AUDIO_TX_SYNC: Framed data is not supported yet
```

### Security & Pairing

This example inherits a Just-Works pairing model (LE Secure Connections, no MITM) with bonding enabled from the shared init at `../common_components/example_init/ble_audio_example_init.c`.

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

1. `app_main` initializes NVS, the BLE host, runs `app_host_init` for host-specific setup (GAP callback, plus a Bluedroid-only GATTC app registration so we have a `gattc_if` for outgoing aux-open), brings up the LE Audio common layer, registers stream ops on every sink/source slot, registers the BAP unicast client callbacks, calls `set_device_name()` (uses `LOCAL_DEVICE_NAME` from `central.h`), then calls `ext_scan_start`.
2. Passive extended scanning runs forever; `scan_data_cb` matches the complete local name `BAP Unicast Server` and parses the ASCS service-data AD for announcement type plus a 32-bit available-contexts field.
3. On a connectable match `ext_scan_recv` calls `ext_scan_stop` and then `conn_create` (NimBLE: `ble_gap_connect`; Bluedroid: `esp_ble_gattc_aux_open`); the resulting `ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT` saves the handle and calls `pairing_start` (NimBLE: `ble_gap_security_initiate`; Bluedroid: `esp_ble_set_encryption`).
4. After the security-change event the client triggers `exchange_mtu`. On NimBLE this issues `ble_gattc_exchange_mtu`; on Bluedroid the GATTC adapter auto-configures MTU inside `BTA_GATTC_Enh_Open`, so `exchange_mtu` drives `esp_ble_audio_gattc_disc_start` directly. `gatt_mtu_change` then calls `esp_ble_audio_gattc_disc_start` (the retry is idempotent — `-EALREADY` is treated as success), and once both MTU and discovery are reported `discover_sinks` runs.
5. `discover_cb` chains sink discovery into `discover_sources`; `endpoint_cb` records each EP into the next free `sinks[]`/`sources[]` slot, and source completion kicks `configure_stream`, which configures every EP with `unicast_preset.codec_cfg`.
6. `config_cb` configures each EP per Config CP ACK until all are done (`all_config_acked`); `create_group` (building `pair_params`, sinks first then sources, with `ESP_BLE_ISO_PACKING_SEQUENTIAL`) and `set_stream_qos` then run from `try_create_group_and_qos`, which waits for every EP to reach codec-configured (`stream_configured_cb` / `ops.configured`) — the CP ACK can arrive before the ASE state change, so driving QoS off the ACK alone races the last EP still in idle.
7. `enable_stream` runs once every EP reports qos-configured (`stream_qos_set_cb` / `ops.qos_set`), not off the QoS CP ACK; `connect_stream` ISO-connects each pair (sink first) once every EP reports enabling (`stream_enabled_cb` / `ops.enabled`), not off the Enable CP ACK. (The ACK callbacks `qos_cb` / `enable_cb` only handle rejects — an ACK can outrun the ASE state change.) When all are up `start_stream` starts only source streams. Sink streams are started by the server, and `stream_started_cb` calls `stream_tx_register` to begin TX.

## Expected Log

TAG: `BAP_UCL`.

Startup and scanning:

```
I (xxx) BAP_UCL: Scanning for unicast server...
I (xxx) BAP_UCL: Unicast server found: type ... contexts 0x...
```

ACL, security, MTU, discovery:

```
I (xxx) BAP_UCL: Connected: handle ... role ... peer ...
I (xxx) BAP_UCL: Security: handle ... level ... bonded ...
I (xxx) BAP_UCL: MTU updated: handle ... mtu ...
I (xxx) BAP_UCL: Service discovery started: handle ...
I (xxx) BAP_UCL: Service discovery complete: handle ...
```

ASE discovery:

```
I (xxx) BAP_UCL: [SNK] Discovering...
I (xxx) BAP_UCL: [SNK #...] Endpoint discovered
I (xxx) BAP_UCL: [SNK] Discovery complete
I (xxx) BAP_UCL: [SRC] Discovering...
I (xxx) BAP_UCL: [SRC #...] Endpoint discovered
I (xxx) BAP_UCL: [SRC] Discovery complete
```

Config / group / QoS / enable / connect / start:

```
I (xxx) BAP_UCL: [SNK #...] Config ACK'd
I (xxx) BAP_UCL: [SNK #...] Stream configured, QoS preference:
I (xxx) BAP_UCL: [SNK #...] Configured
I (xxx) BAP_UCL: Pair[...]: SNK=... SRC=...
I (xxx) BAP_UCL: Unicast group created (... pair(s))
I (xxx) BAP_UCL: [SNK #...] QoS ACK'd
I (xxx) BAP_UCL: [SNK #...] QoS set
I (xxx) BAP_UCL: [SNK #...] Enable ACK'd
I (xxx) BAP_UCL: [SNK #...] Enabled
I (xxx) BAP_UCL: Pair[...] connecting (SNK first)
I (xxx) BAP_UCL: [SNK #...] ISO connected (pair[...])
I (xxx) BAP_UCL: [SNK #...] Connected
I (xxx) BAP_UCL: [SRC] Starting all source streams
I (xxx) BAP_UCL: [SRC #...] Stream started
I (xxx) BAP_UCL: [SNK #...] Stream started
I (xxx) BAP_UCL: [SNK #...] Started (SDU ..., interval ... us)
```

Disconnect:

```
I (xxx) BAP_UCL: [SNK #...] ISO disconnected, reason 0x...
I (xxx) BAP_UCL: Disconnected: handle ... reason 0x...
```

## Peer Pairing

Run [unicast_server](../unicast_server/) on a second board.

1. Server boots and starts connectable extended advertising with the ASCS UUID and `BAP Unicast Server` name.
2. Client boots and scans; on matching the name and ASCS service data it cancels scanning and issues `conn_create` (NimBLE: `ble_gap_connect`; Bluedroid: `esp_ble_gattc_aux_open`).
3. After ACL is up the client initiates pairing/security; both sides log `Connected:` and `Security:`.
4. Client exchanges MTU and runs GATT discovery, then discovers sink ASEs followed by source ASEs on the server.
5. Client drives ASCS Config -> QoS -> Enable -> ISO Connect on every ASE; the server returns its QoS preference and accepts each operation.
6. Client starts source streams; server auto-starts each sink stream from `stream_enabled_cb`, after which the client's TX scheduler feeds SDUs and the server's `stream_recv_cb` accumulates RX metrics.
