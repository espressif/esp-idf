| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BLE CIS Central Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This is a raw BLE Connected Isochronous Stream (CIS) example operating directly at the ISO transport layer over either the NimBLE or Bluedroid host (selected at build time via Kconfig). It is **not** a BAP/CAP (BLE Audio profile) example — it does not implement Unicast Server/Client, ASCS, PACS, or any LC3 codec; it only exercises the underlying CIG/CIS plumbing.

The central scans for a peer advertising the name `CIS Peripheral`, opens an ACL link, optionally pairs (security level `ESP_BLE_ISO_SECURITY_NO_MITM`), creates a **two-CIS** CIG (10 ms SDU interval, 2M PHY, RTN 2, 120-byte SDU, sequential/unframed), connects both CIS, configures each input data path to the HCI in transparent format, and then drives one software TX scheduler for the CIG that submits one SDU on each established CIS every 10 ms.

Both CIS ride the same ACL connection to the same peer, which is what `esp_ble_iso_chan_connect()` expresses: it takes one ACL handle plus a channel count. Each stream has its own sequence numbering and TX counters; the timer is shared by the CIG. The optional timestamp mode starts as soon as the first input data path is ready; the other CIS joins it once established.

The transmitted payload is a dummy buffer filled with the current sequence number byte — there is no real audio data, the example just demonstrates the ISO transport mechanics via the `esp_ble_iso_*` APIs.

> **Bluedroid host status (phase 1):** scanning and AUTH_CMPL forwarding work; ACL connection initiation and pairing kick-off (`conn_create` / `pairing_start`) currently return `ESP_ERR_NOT_SUPPORTED` and will be wired up against the public Bluedroid APIs in a follow-up. Use the NimBLE overlay for end-to-end runtime testing.

## Requirements

* A board with BLE 5.2 and ISO support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

Under **Example: Common ISO Utilities**, enable **Synchronize multi-channel TX with
timestamps** (`CONFIG_EXAMPLE_ISO_TX_TIMESTAMP`, disabled by default) to give
the SDUs of every established CIS the same timestamps, based on the controller
clock:

- As soon as the first CIS input data path is ready, that CIS sends SDUs
  without timestamps, one per SDU interval, which the controller places by
  sequence number. After each, **LE Read ISO TX Sync**
  (`esp_ble_iso_chan_get_tx_sync()`) gives the CIG reference point of the last
  SDU sent; reading after sending keeps this blocking HCI command (about 2.5 ms)
  from delaying the SDU. The first of as many SDUs at one reference point as fit
  in an ISO interval starts its event, so its reference time is that reference
  point; the timestamps go on from it.
- From then on each tick of the CIG's TX scheduler sends one frame: one SDU on
  each established CIS with `esp_ble_iso_chan_send_ts()` and the same timestamp,
  one SDU interval after the last frame's. All CIS of a CIG share one reference
  point, so equal timestamps put their SDUs in the same event. A CIS established
  later joins at the next frame, with its own sequence numbers from 0. The best
  lead (how long before its reference time an SDU is handled, from the sent
  callback) of every 50 SDUs follows the clock difference between the timer and
  the controller: below 200 us the scheduler adds one frame, above two SDU
  intervals more it skips one. A stall lowers only some SDUs, and the ticks then
  catch up, so it changes nothing; a best lead below minus one SDU interval is a
  stall or a backlog, and no frame is added for it.

In this mode the first 4 bytes of each SDU carry its timestamp (0 without one),
so the receiver can tell which SDUs of the CIS belong to one frame, e.g. the left
and right audio.

This mode requires unframed data. A CIS that disconnects does not affect the
others, and the example does not reconnect it while the ACL is up. The timestamp
aligns the SDUs, but each CIS still uses its own sub-events on air.

The number of CIS is the `CIS_COUNT` macro in `main/main.c`; raising it also requires `CONFIG_BT_ISO_MAX_CHAN` in `sdkconfig.defaults` to be at least that value (the host default is 1, so a second CIS is rejected without it).

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
5. After the security change, call `esp_ble_iso_cig_create` (two CIS, 10 ms latencies and SDU interval, sequential/unframed, SCA unknown) and `esp_ble_iso_chan_connect` once for both channels over the same ACL handle.
6. The controller establishes the CIS one at a time. On each CIS connect, set up that channel's input data path (HCI / transparent). The first one starts the CIG's periodic TX scheduler; it stops when the last one disconnects.
7. The scheduler invokes `esp_ble_iso_chan_send` every 10 ms on each established CIS, with a per-CIS incrementing sequence number on a 120-byte dummy SDU. In timestamp mode, SDUs without timestamps on the first CIS and LE Read ISO TX Sync start the timing instead; each tick then sends `esp_ble_iso_chan_send_ts()` with the same timestamp on every CIS.

## Expected Log

TAG: `CIS_CEN`

Scan and connection phase:

```
I CIS_CEN: Scanning for peripheral...
I CIS_CEN: Connected: handle <h> role <r> peer XX:XX:XX:XX:XX:XX
I CIS_CEN: Security: handle <h> level <l> bonded <b>
```

CIS setup and streaming phase. The two CIS come up a couple of hundred milliseconds apart, and each reports its own count (TX log emitted every `LOG_INTERVAL_PACKETS` SDUs by the shared utility):

```
I CIS_CEN: [CIS #0] Connected
I CIS_CEN: [CIS #0] Starting TX without timestamps
I CIS_CEN: [CIS #1] Connected
I CIS_CEN: [CIS #1] Starting TX without timestamps
I CIS_CEN: [CIS #0] TX: <count> packets
I CIS_CEN: [CIS #1] TX: <count> packets
```

There should be no `IsoSendChanNotConn` errors in the gap between the two `Connected` lines: each CIS is only sent on once it is established, so nothing is submitted to a stream that is still connecting.

Disconnect path (per CIS, then the ACL):

```
I CIS_CEN: [CIS #0] Disconnected, reason 0x<rr>
I CIS_CEN: [CIS #1] Disconnected, reason 0x<rr>
I CIS_CEN: Disconnected: handle <h> reason 0x<rr>
```

In both modes, a TX log of a CIS that had late SDUs since its last one:

```
I CIS_CEN: [CIS #<n>] TX: <count> packets, <k> late
```

A late SDU missed its event and was dropped, e.g. after a stall of the ISO task.

Timestamp mode, while running (TX goes on):

```
W CIS_CEN: [CIS #<n>] Send failed, err <e>
```

That SDU is dropped.

Timestamp mode, at start:

```
W ISO_TX_SYNC: Framed data is not supported yet
E CIS_CEN: [CIS #<n>] Timestamp TX start failed, err <e>
```

The first keeps timestamp TX from starting on every CIS; the second keeps that CIS from sending.

## Peer Pairing

Run [cis_peripheral](../cis_peripheral/) on a second board.

1. Flash and run `cis_peripheral` first; it begins extended advertising as `CIS Peripheral`.
2. Flash and run `cis_central` on the second board; it scans and matches that name.
3. The central creates the ACL connection and initiates pairing.
4. After the security change, the central creates the CIG and connects both CIS over that one ACL.
5. Both sides set up a data path per CIS (input on central, output on peripheral).
6. The central streams 120-byte SDUs every 10 ms on each CIS; the peripheral reports `RX: <count> packets` per CIS periodically.
