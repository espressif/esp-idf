| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# BIG Broadcaster

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example demonstrates a raw BLE Isochronous Broadcaster — it creates a Broadcast Isochronous Group (BIG) directly at the ISO transport layer over either the NimBLE or Bluedroid host (selected at build time via Kconfig), without any BLE Audio profile (BAP/CAP) on top.

The device acts as the **broadcaster**: it starts extended + periodic advertising, creates a BIG carrying two BIS streams, sets up an HCI input data path on each BIS, and then transmits SDUs on a fixed 10 ms cadence using a software TX scheduler. The peer (`big_receiver`) discovers the broadcaster by name, syncs to the periodic advertising train, decodes BIGInfo, and joins the BIS sub-events.

The audio data on the wire is **application-supplied dummy payload** (each SDU is filled with the byte value of its sequence number). The coding format is `ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT`, so no LC3 / LE-Audio framing is involved.

## Requirements

* A board with BLE 5.2 and ISO support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

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

1. NVS, the selected BLE host (NimBLE or Bluedroid), and the ISO common layer are initialised.
2. A TX scheduler is initialised for each of the two BIS channels.
3. Extended advertising parameters / data and periodic advertising parameters / data are configured for handle 0 (non-connectable, non-scannable, primary 1M / secondary 2M).
4. Periodic advertising and extended advertising are started.
5. `esp_ble_iso_big_ext_adv_add()` ties the adv handle to the BIG, then `esp_ble_iso_big_create()` issues the HCI `LE Create BIG` with two BIS, encryption on, and broadcast code `"1234"`.
6. As each BIS becomes ready, the connected callback installs an HCI input data path (`ESP_BLE_ISO_DATA_PATH_DIR_INPUT`, transparent coding).
7. Once both BIS are connected, the per-channel TX schedulers start firing every 10 ms, calling `esp_ble_iso_chan_send()` with a 120-byte dummy SDU and an incrementing sequence number.
8. On disconnect of all BIS the schedulers are stopped and the BIG handle is cleared.

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
I (xxx) BIG_BRD: All 2 BIS channels connected, starting TX
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

## Peer Pairing

Run [big_receiver](../big_receiver/) on a second board.

1. Flash and run `big_broadcaster` on board A; it begins extended + periodic advertising and creates the BIG immediately.
2. Flash and run `big_receiver` on board B; it starts passive extended scanning.
3. Board B matches on the device name `"BIG Broadcaster"` and creates a periodic advertising sync to the broadcaster's PA train.
4. Once PA-synced, board B receives the BIGInfo report and calls `LE BIG Create Sync` with broadcast code `"1234"` and both BIS in the bitfield.
5. Board A logs `[BIS #0/1] Connected` as the controller reports the BIS as established; board B logs `PA synced` followed by per-BIS `Connected` and starts counting received SDUs.
6. Resetting either side causes the receiver to drop and (via `pa_sync_lost`) restart its scan, re-establishing the link automatically.
