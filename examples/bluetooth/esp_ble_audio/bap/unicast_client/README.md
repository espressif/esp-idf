| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BAP Unicast Client Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements a **BAP Unicast Client** on top of the NimBLE host stack. It scans for a peer advertising the ASCS service UUID with a complete local name of `BAP Unicast Server`, opens an ACL connection, initiates pairing, exchanges MTU, and starts GATT service discovery.

Once the peer is reachable, the client uses the BAP unicast client API (`esp_ble_audio_bap_unicast_client_discover`, `esp_ble_audio_bap_stream_config`, `esp_ble_audio_bap_unicast_group_create`, `esp_ble_audio_bap_stream_qos`, `esp_ble_audio_bap_stream_enable`, `esp_ble_audio_bap_stream_connect`, `esp_ble_audio_bap_stream_start`) to discover sink and source ASEs, configure each with the LC3 `16_2_1` unicast preset, build a unicast group, set QoS, enable, and ISO-connect the streams. Source (TX-from-server) streams are started by this client; sink (TX-to-server) streams are started by the server.

The TX path in `stream_tx.c` registers each newly started sink stream with `example_audio_tx_scheduler` and feeds it SDU-sized buffers driven by `esp_ble_audio_bap_stream_send`. The PACS callbacks (`pac_record`, `endpoint`, `location`, `available_contexts`) print the discovered peer capabilities.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

Open menuconfig:

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

### Security & Pairing

This example inherits a Just-Works pairing model (LE Secure Connections, no MITM) with bonding enabled from the shared init at `../common_components/example_init/ble_audio_example_init.c`.

## Build & Flash

```bash
idf.py set-target esp32h4
idf.py -p PORT flash monitor
```

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. `app_main` initializes NVS, NimBLE, and the LE Audio common layer, registers stream ops on every sink/source slot, registers the BAP unicast client callbacks, sets the device name `BAP Unicast Client`, and calls `ext_scan_start`.
2. Passive extended scanning runs forever; `scan_data_cb` matches the complete local name `BAP Unicast Server` and parses the ASCS service-data AD for announcement type plus a 32-bit available-contexts field.
3. On a connectable match `conn_create` cancels scanning and issues `ble_gap_connect`; the resulting `ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT` saves the handle and calls `ble_gap_security_initiate`.
4. After the security-change event the client triggers `ble_gattc_exchange_mtu`; `gatt_mtu_change` then calls `esp_ble_audio_gattc_disc_start`, and once both MTU and discovery are reported `discover_sinks` runs.
5. `discover_cb` chains sink discovery into `discover_sources`; `endpoint_cb` records each EP into the next free `sinks[]`/`sources[]` slot, and source completion kicks `configure_stream`, which configures every EP with `unicast_preset.codec_cfg`.
6. After the last `config_cb`, `create_group` builds `pair_params` (sinks first, then sources) with `ESP_BLE_ISO_PACKING_SEQUENTIAL` and calls `esp_ble_audio_bap_unicast_group_create`, then `set_stream_qos`.
7. When `is_all_stream_qos_set` returns true `enable_stream` walks every stream; on the final `enable_cb`, `connect_stream` ISO-connects each pair (sink first if available); when all are up `start_stream` starts only source streams. Sink streams are started by the server, and `stream_started_cb` calls `stream_tx_register` to begin TX.

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
2. Client boots and scans; on matching the name and ASCS service data it cancels scanning and issues `ble_gap_connect`.
3. After ACL is up the client initiates pairing/security; both sides log `Connected:` and the client logs `Security:`.
4. Client exchanges MTU and runs GATT discovery, then discovers sink ASEs followed by source ASEs on the server.
5. Client drives ASCS Config -> QoS -> Enable -> ISO Connect on every ASE; the server returns its QoS preference and accepts each operation.
6. Client starts source streams; server auto-starts each sink stream from `stream_enabled_cb`, after which the client's TX scheduler feeds SDUs and the server's `stream_recv_cb` accumulates RX metrics.
