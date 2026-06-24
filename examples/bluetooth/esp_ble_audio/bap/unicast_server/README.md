| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BAP Unicast Server Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements a **BAP Unicast Server** on top of the selected BLE host stack (Bluedroid by default; NimBLE via the `sdkconfig.defaults.nimble` overlay). It registers PACS sink and source capabilities (LC3, any frequency, 10 ms frame duration, 2-channel support, 40-120 octets per frame, up to 2 frames per SDU) and a hard-coded location/context set (front-left + front-right; UNSPECIFIED | CONVERSATIONAL | MEDIA), then starts connectable extended advertising on 1M/2M PHY whose payload carries the Flags AD, the ASCS UUID, the targeted unicast announcement service data with sink/source contexts, and the device name `BAP Unicast Server`.

The unicast server callbacks (`config`, `reconfig`, `qos`, `enable`, `start`, `metadata`, `disable`, `stop`, `release`) react to ASCS PDUs from a peer client. `config_cb` allocates a free `sink_streams[]`/`source_streams[]` slot and returns a fixed `qos_pref` (unframed PDUs, 2M PHY preference, RTN 2, 10 ms max latency, 20-40 ms presentation delay). `enable_cb` decodes the codec config to sample rate, frame duration, and frame-blocks-per-SDU before accepting the request. Reconfig is rejected unconditionally.

The stream ops handle ISO data: when the client enables a sink ASE, `stream_enabled_cb` calls `esp_ble_audio_bap_stream_start` so the server starts sink streams locally (the client starts source streams). Inbound ISO SDUs flow through `stream_recv_cb` into `example_audio_rx_metrics_on_recv`, which logs RX metrics periodically.

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

1. `app_main` initializes NVS, NimBLE, and the LE Audio common layer, then calls `esp_ble_audio_pacs_register` with `snk_pac/snk_loc/src_pac/src_loc` all true.
2. It registers the unicast server (`esp_ble_audio_bap_unicast_server_register` with `CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT` / `CONFIG_BT_ASCS_MAX_ASE_SRC_COUNT`) and its callback table, registers sink and source PACS capabilities, and attaches `stream_ops` to every preallocated stream slot.
3. PACS location is set per direction (`SINK_LOCATION` / `SOURCE_LOCATION`) and supported plus available contexts are pushed for sink and source.
4. `esp_ble_audio_common_start` runs, then `adv_init("BAP Unicast Server")` performs host-specific setup (Bluedroid: create completion sem + register GAP cb + `esp_ble_gap_set_device_name`; NimBLE: `ble_svc_gap_device_name_set`). `ext_adv_start` then builds `ext_adv_data` with Flags, ASCS UUID, ASCS service data (targeted announcement + contexts), and the device name in host-agnostic bytes, and calls `adv_start(ext_adv_data, sizeof(ext_adv_data))`. The host-specific implementation in `main/bluedroid/adv.c` or `main/nimble/adv.c` configures and starts the connectable extended advertising set on `ADV_HANDLE` 0.
5. On `ESP_BLE_AUDIO_GAP_EVENT_ACL_CONNECT` the connection is logged; after the peer client triggers MTU exchange the server logs the new MTU and calls `esp_ble_audio_gattc_disc_start`.
6. ASCS callbacks fire as the client drives the state machine: `config_cb` allocates a stream and returns the QoS preference, `qos_cb` records `max_sdu`, `enable_cb` validates the codec, `metadata_cb` parses metadata, and `disable`/`stop`/`release` log their requests.
7. `stream_enabled_cb` auto-starts sink streams server-side; `stream_started_cb` resets TX or RX counters depending on direction; `stream_recv_cb` updates per-stream RX metrics on every ISO SDU.
8. On disconnect `acl_disconnect` logs the reason and restarts `ext_adv_start`.

## Expected Log

TAG: `BAP_USR`.

Startup and advertising:

```
I (xxx) BAP_USR: PACS: locations and contexts set
I (xxx) BAP_USR: Advertising started (handle 0)
```

Client connects:

```
I (xxx) BAP_USR: Connected: handle ... role ... peer ...
I (xxx) BAP_USR: Security: handle ... level ... bonded ...
I (xxx) BAP_USR: MTU updated: handle ... mtu ...
I (xxx) BAP_USR: Service discovery started: handle ...
I (xxx) BAP_USR: Service discovery complete: handle ... status ...
```

ASCS operations:

```
I (xxx) BAP_USR: [SNK #...] Config request
I (xxx) BAP_USR: [SNK #...] QoS request:
I (xxx) BAP_USR: [SNK #...] Enable request: ... Hz, ... us, ... block(s)/SDU
I (xxx) BAP_USR: [SNK #...] Stream enabled
I (xxx) BAP_USR: [SNK #...] Stream started
I (xxx) BAP_USR: [SRC #...] Config request
I (xxx) BAP_USR: [SRC #...] QoS request:
I (xxx) BAP_USR: [SRC #...] Enable request: ... Hz, ... us, ... block(s)/SDU
I (xxx) BAP_USR: [SRC #...] Stream enabled
I (xxx) BAP_USR: [SRC #...] Stream started
```

Teardown:

```
I (xxx) BAP_USR: [SNK #...] Disable request
I (xxx) BAP_USR: [SNK #...] Stop request
I (xxx) BAP_USR: [SNK #...] Stream stopped, reason 0x...
I (xxx) BAP_USR: [SNK #...] Release request
I (xxx) BAP_USR: Disconnected: handle ... reason 0x...
```

**Note — disconnect race.** On peer-initiated disconnect (e.g. supervision timeout, peer drops the link) with active streams, an additional Bluedroid error may interleave:

```
W (xxx) BT_APPL: gattc_conn_cb: if=4 st=0 id=4 rsn=0x8
W (xxx) BT_HCI: hcif disc complete: hdl 0x0, rsn 0x8 dev_find 1
I (xxx) BAP_USR: [SNK #0] ISO disconnected, reason 0x08
I (xxx) BAP_USR: [SNK #0] Stream disabled
E (xxx) BT_APPL: Unknown connection ID: 3 fail sending notification
I (xxx) BAP_USR: [SNK #0] Stream stopped, reason 0x08
```

`Unknown connection ID` is harmless. GATT notifications for the ASE state changes are queued for Bluedroid to send. If Bluedroid clears the BTA connection on the ACL disconnect before those queued sends drain, they fail with this error. The peer has already disconnected, so the missed notifications have no effect on either side.

## Peer Pairing

Run [unicast_client](../unicast_client/) on a second board.

1. Server boots and starts connectable extended advertising with the ASCS UUID and `BAP Unicast Server` name.
2. Client boots and scans; on matching the name and ASCS service data it cancels scanning and issues `ble_gap_connect`.
3. After ACL is up the client initiates pairing/security; both sides log `Connected:` and `Security:`.
4. Client exchanges MTU and runs GATT discovery, then discovers sink ASEs followed by source ASEs on the server.
5. Client drives ASCS Config -> QoS -> Enable -> ISO Connect on every ASE; the server returns its QoS preference and accepts each operation.
6. Client starts source streams; server auto-starts each sink stream from `stream_enabled_cb`, after which the client's TX scheduler feeds SDUs and the server's `stream_recv_cb` accumulates RX metrics.
