| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BIG Broadcaster

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example demonstrates a raw BLE Isochronous Broadcaster — it creates a Broadcast Isochronous Group (BIG) directly at the ISO transport layer over either the NimBLE or Bluedroid host (selected at build time via Kconfig), without any BLE Audio profile (BAP/CAP) on top.

The device acts as the **broadcaster**: it starts extended + periodic advertising, creates a BIG carrying two BIS streams, sets up an HCI input data path on each BIS, and then transmits one SDU on each BIS on a fixed 10 ms cadence using one software TX scheduler for the BIG. The peer (`big_receiver`) discovers the broadcaster by name, syncs to the periodic advertising train, decodes BIGInfo, and joins the BIS sub-events.

The audio data on the wire is **application-supplied dummy payload** (each SDU is filled with the byte value of its sequence number). The coding format is `ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT`, so no LC3 / LE-Audio framing is involved.

## Requirements

* A board with BLE 5.2 and ISO support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

Under **Example: Common ISO Utilities**, enable **Synchronize multi-channel TX with
timestamps** (`CONFIG_EXAMPLE_ISO_TX_TIMESTAMP`, disabled by default) to give
the SDUs of both BIS the same timestamps, based on the controller clock:

- Once both BIS are connected, one BIS sends SDUs without timestamps, one per
  SDU interval, which the controller places by sequence number. After each,
  **LE Read ISO TX Sync** (`esp_ble_iso_chan_get_tx_sync()`) gives the BIG
  anchor point of the last SDU sent; reading after sending keeps this blocking
  HCI command (about 2.5 ms) from delaying the SDU. The first of as many SDUs at
  one anchor point as fit in an ISO interval starts its event, so its reference
  time is that anchor point; the timestamps go on from it.
- From then on each tick of the BIG's TX scheduler sends one frame: one SDU on
  each BIS with `esp_ble_iso_chan_send_ts()` and the same timestamp, one SDU
  interval after the last frame's. All BIS of a BIG share one anchor, so equal
  timestamps put their SDUs in the same event. Each BIS has its own sequence
  numbers; the other BIS starts from 0. The best lead (how long before its
  reference time an SDU is handled, from the sent callback) of every 50 SDUs
  follows the clock difference between the timer and the controller: below
  200 us the scheduler adds one frame, above two SDU intervals more it skips one.
  A stall lowers only some SDUs, and the ticks then catch up, so it changes
  nothing; a best lead below minus one SDU interval is a stall or a backlog,
  and no frame is added for it.

In this mode the first 4 bytes of each SDU carry its timestamp (0 without one),
so the receiver can tell which SDUs of the BIS belong to one frame, e.g. the left
and right audio.

This mode requires unframed data. TX stops when the BIG ends. The timestamp
aligns the SDUs, but each BIS still uses its own sub-events on air.

Notable hard-coded parameters in `main/main.c`:

* `LOCAL_DEVICE_NAME` — `"BIG Broadcaster"` (advertised in extended adv)
* `LOCAL_BROADCAST_CODE` — `"1234"` (BIG is encrypted)
* `BIG_SDU_INTERVAL_US` — 10 000 us
* `BIG_LATENCY_MS` — 10 ms
* `BIG_PHY` — 2M
* `BIG_RTN` — 2 (retransmissions)
* `BIS_ISO_CHAN_COUNT` — 2
* `BIS_SDU_SIZE` — 120 bytes

### Security & Pairing

On the NimBLE path, the shared init at `../common_components/example_init/ble_iso_example_init.c` configures Just-Works pairing (LE Secure Connections, no MITM, `BLE_SM_IO_CAP_NO_IO`) with bonding enabled, and leaves `gatts_register_cb = NULL` (no GATT services). On the Bluedroid path the same init just brings up the controller and host with no SMP configuration. Either way these settings are not exercised by this example — BIG broadcast traffic is non-connectable and BIS payload encryption is driven by the broadcast code, independent of host SMP.

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

1. NVS, the selected BLE host (NimBLE or Bluedroid), and the ISO common layer are initialised.
2. One TX scheduler is initialised for the BIG.
3. Extended advertising parameters / data and periodic advertising parameters / data are configured for handle 0 (non-connectable, non-scannable, primary 1M / secondary 2M).
4. Periodic advertising and extended advertising are started.
5. `esp_ble_iso_big_ext_adv_add()` ties the adv handle to the BIG, then `esp_ble_iso_big_create()` issues the HCI `LE Create BIG` with two BIS, encryption on, and broadcast code `"1234"`.
6. As each BIS becomes ready, the connected callback installs an HCI input data path (`ESP_BLE_ISO_DATA_PATH_DIR_INPUT`, transparent coding).
7. Once both BIS are connected, the TX scheduler starts firing every 10 ms, calling `esp_ble_iso_chan_send()` on each BIS with a 120-byte dummy SDU and its incrementing sequence number. In timestamp mode, SDUs without timestamps on one BIS and LE Read ISO TX Sync start the timing instead; each tick then sends `esp_ble_iso_chan_send_ts()` with the same timestamp on both BIS.
8. On disconnect of all BIS the scheduler is stopped and the BIG handle is cleared.

## Expected Log

Tag: `BIG_BRD`.

Startup / advertising:

```
I (xxx) BIG_BRD: Advertising started (handle 0)
```

BIS bring-up (first BIS, then second, then TX start):

```
I (xxx) BIG_BRD: [BIS #0] Connected
I (xxx) BIG_BRD: Waiting for remaining BIS channels (1/2)
I (xxx) BIG_BRD: [BIS #1] Connected
I (xxx) BIG_BRD: All 2 BIS channels connected, starting TX without timestamps
```

Steady-state (logged every `LOG_INTERVAL_PACKETS` SDUs by the shared TX helper):

```
I (xxx) BIG_BRD: [BIS #0] TX: <N> packets
I (xxx) BIG_BRD: [BIS #1] TX: <N> packets
```

Teardown:

```
I (xxx) BIG_BRD: [BIS #0] Disconnected, reason 0x<rr>
I (xxx) BIG_BRD: [BIS #1] Disconnected, reason 0x<rr>
I (xxx) BIG_BRD: All BIS channels disconnected, TX stopped
```

In both modes, a TX log of a BIS that had late SDUs since its last one:

```
I (xxx) BIG_BRD: [BIS #<n>] TX: <N> packets, <k> late
```

A late SDU missed its event and was dropped, e.g. after a stall of the ISO task.

Timestamp mode, while running (TX goes on):

```
W (xxx) BIG_BRD: [BIS #<n>] Send failed, err <e>
```

That SDU is dropped.

Timestamp mode, at start:

```
W (xxx) ISO_TX_SYNC: Framed data is not supported yet
E (xxx) BIG_BRD: [BIS #<n>] Timestamp TX start failed, err <e>
```

The first keeps timestamp TX from starting on every BIS; the second keeps that BIS from sending.

## Peer Pairing

Run [big_receiver](../big_receiver/) on a second board.

1. Flash and run `big_broadcaster` on board A; it begins extended + periodic advertising and creates the BIG immediately.
2. Flash and run `big_receiver` on board B; it starts passive extended scanning.
3. Board B matches on the device name `"BIG Broadcaster"` and creates a periodic advertising sync to the broadcaster's PA train.
4. Once PA-synced, board B receives the BIGInfo report and calls `LE BIG Create Sync` with broadcast code `"1234"` and both BIS in the bitfield.
5. Board A logs `[BIS #0/1] Connected` as the controller reports the BIS as established; board B logs `PA synced` followed by per-BIS `Connected` and starts counting received SDUs.
6. Resetting either side causes the receiver to drop and (via `pa_sync_lost`) restart its scan, re-establishing the link automatically.
