| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BLE CIS Central Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This is a raw BLE Connected Isochronous Stream (CIS) example operating directly at the ISO transport layer over either the NimBLE or Bluedroid host (selected at build time via Kconfig). It is **not** a BAP/CAP (BLE Audio profile) example — it does not implement Unicast Server/Client, ASCS, PACS, or any LC3 codec; it only exercises the underlying CIG/CIS plumbing.

The central scans for a peer advertising the name `CIS Peripheral`, opens an ACL link, optionally pairs (security level `ESP_BLE_ISO_SECURITY_NO_MITM`), creates a single-CIS CIG (10 ms SDU interval, 2M PHY, RTN 2, 120-byte SDU, sequential/unframed), connects the CIS, configures the input data path to the HCI in transparent format, and then drives a software TX scheduler that submits one SDU every 10 ms.

The transmitted payload is a dummy buffer filled with the current sequence number byte — there is no real audio data, the example just demonstrates the ISO transport mechanics via the `esp_ble_iso_*` APIs.

> **Bluedroid host status (phase 1):** scanning and AUTH_CMPL forwarding work; ACL connection initiation and pairing kick-off (`conn_create` / `pairing_start`) currently return `ESP_ERR_NOT_SUPPORTED` and will be wired up against the public Bluedroid APIs in a follow-up. Use the NimBLE overlay for end-to-end runtime testing.

## Requirements

* A board with BLE 5.2 and ISO support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

### Security & Pairing

Just-Works pairing (LE Secure Connections, no MITM, IO capability = None) with bonding enabled. On NimBLE the configuration is inherited from the shared host init in `../common_components/example_init/ble_iso_example_init.c`; on Bluedroid the equivalent SMP setup will live in `main/bluedroid/scan.c` once `pairing_start` is implemented. ISO examples do not register any custom GATT services.

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

1. Initialize NVS, the selected BLE host, and the ISO common layer with a GAP callback.
2. Start passive extended scanning for a device whose Complete Local Name is `CIS Peripheral`.
3. Cancel scan and create an ACL connection (interval 80 ms, supervision 5 s) once the target is matched.
4. On ACL connect, initiate pairing because the configured security level is `ESP_BLE_ISO_SECURITY_NO_MITM`.
5. After the security change, call `esp_ble_iso_cig_create` (one CIS, 10 ms latencies and SDU interval, sequential/unframed, SCA unknown) and `esp_ble_iso_chan_connect` over the ACL handle.
6. On CIS connect, set up the input data path (HCI / transparent) and start the periodic TX scheduler.
7. The scheduler invokes `esp_ble_iso_chan_send` every 10 ms with an incrementing sequence number on a 120-byte dummy SDU.

## Expected Log

TAG: `CIS_CEN`

Scan and connection phase:

```
I CIS_CEN: Scanning for peripheral...
I CIS_CEN: Connected: handle <h> role <r> peer XX:XX:XX:XX:XX:XX
I CIS_CEN: Security: handle <h> level <l> bonded <b>
```

CIS setup and streaming phase (TX log emitted every `LOG_INTERVAL_PACKETS` SDUs by the shared utility):

```
I CIS_CEN: [CIS #0] Connected
I CIS_CEN: [CIS #0] TX: <count> packets
```

Disconnect path:

```
I CIS_CEN: [CIS #0] Disconnected, reason 0x<rr>
I CIS_CEN: Disconnected: handle <h> reason 0x<rr>
```

## Peer Pairing

Run [cis_peripheral](../cis_peripheral/) on a second board.

1. Flash and run `cis_peripheral` first; it begins extended advertising as `CIS Peripheral`.
2. Flash and run `cis_central` on the second board; it scans and matches that name.
3. The central creates the ACL connection and initiates pairing.
4. After the security change, the central creates the CIG and connects the CIS.
5. Both sides set up their data paths (input on central, output on peripheral).
6. The central streams 120-byte SDUs every 10 ms; the peripheral reports `RX: <count> packets` periodically.
