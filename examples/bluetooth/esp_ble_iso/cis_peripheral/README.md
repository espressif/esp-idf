| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BLE CIS Peripheral Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This is a raw BLE Connected Isochronous Stream (CIS) example operating directly at the ISO transport layer. It is **not** a BAP/CAP (BLE Audio profile) example — it does not implement Unicast Server, ASCS, PACS, or any LC3 codec; it only exercises the underlying CIS accept/receive plumbing.

The peripheral starts connectable extended advertising under the name `CIS Peripheral` (1M primary / 2M secondary PHY, 200 ms interval), registers an ISO server with security level `ESP_BLE_ISO_SECURITY_NO_MITM` and an accept callback that hands out the single CIS channel slot, sets up the output data path to the HCI in transparent format when the CIS connects, and tallies received SDUs through the shared RX-metrics helper.

There is no audio decoding — incoming SDUs are simply counted (valid / error / lost / null), so any 120-byte dummy payload from the central is consumed as opaque data by the NimBLE host with ISO support and the `esp_ble_iso_*` APIs.

## Requirements

* A board with BLE 5.2 and ISO support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

### Security & Pairing

Just-Works pairing (LE Secure Connections, no MITM, `BLE_SM_IO_CAP_NO_IO`) with bonding enabled, inherited from the shared host init in `../common_components/example_init/ble_iso_example_init.c`. ISO examples do not register any GATT services (`gatts_register_cb = NULL`).

## Build & Flash

```bash
idf.py set-target esp32h4
idf.py -p PORT flash monitor
```

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. Initialize NVS, NimBLE host, and the ISO common layer with a GAP callback.
2. Register an ISO server (`esp_ble_iso_server_register`) whose accept callback returns the single static CIS channel.
3. Build flags + complete-local-name AD payload and start connectable extended advertising on handle 0 at a 200 ms interval.
4. On ACL connect, log the peer; the central drives subsequent pairing and CIG/CIS creation.
5. When the CIS request arrives, the accept callback supplies the channel; on CIS connect, set up the output data path (HCI / transparent) and reset RX metrics.
6. Each received SDU updates valid/error/lost counters; periodic RX summaries are logged by the shared utility.
7. On ACL disconnect, restart extended advertising.

## Expected Log

TAG: `CIS_PER`

Advertising and connection phase:

```
I CIS_PER: Advertising started (handle 0)
I CIS_PER: Connected: handle <h> role <r> peer XX:XX:XX:XX:XX:XX
I CIS_PER: Security: handle <h> level <l> bonded <b>
```

CIS accept and streaming phase (RX log emitted every `LOG_INTERVAL_PACKETS` SDUs by the shared utility):

```
I CIS_PER: Incoming CIS request from handle <h>
I CIS_PER: [CIS #0] Connected
I CIS_PER: [CIS #0] RX: <count> packets
```

Disconnect path:

```
I CIS_PER: [CIS #0] Disconnected, reason 0x<rr>
I CIS_PER: Disconnected: handle <h> reason 0x<rr>
```

## Peer Pairing

Run [cis_central](../cis_central/) on a second board.

1. Flash and run `cis_peripheral` first; it advertises as `CIS Peripheral` on extended-advertising handle 0.
2. Flash and run `cis_central` on the second board; it scans and matches the name.
3. The central opens the ACL connection and initiates pairing; this peripheral handles the security change passively.
4. The central creates the CIG and CIS; the peripheral's accept callback returns the local channel.
5. The peripheral configures its output data path on CIS connection.
6. Incoming 120-byte SDUs are counted and `RX: <count> packets` is logged periodically.
