| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# TMAP Peripheral Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example takes the **TMAP Call Terminal (CT)** and **Unicast Media Receiver (UMR)** roles, registered together via `esp_ble_audio_tmap_register(ESP_BLE_AUDIO_TMAP_ROLE_CT | ESP_BLE_AUDIO_TMAP_ROLE_UMR)`. It runs connectable extended advertising at a 200 ms interval that includes the GAP Earbud appearance, the ASCS/CAS/TMAS UUIDs, an ASCS targeted unicast announcement carrying sink and source contexts (UNSPECIFIED | CONVERSATIONAL | MEDIA | GAME | INSTRUCTIONAL), a CAS targeted announcement, the TMAS UMR|CT role payload, and the device name `tmap_peripheral`. When the Duo earbuds option is selected, a CSIS RSI is added to the advertisement.

The example uses the ESP-BLE-AUDIO library pieces for: BAP Unicast Server with PACS (LC3 cap with 16/32/48 kHz, 7.5/10 ms duration, 30–155 octets), VCP Volume Renderer (initial volume 10, unmuted, with VOCS+AICS instances built from Kconfig counts), TBS client (CCP Call Terminal — discovers GTBS, reads URI list, originates/terminates calls), MCC controller (reads player name, track title/duration/position, playback/seeking speed, playing order, media state, opcodes, CCID, plus a `mcp_send_cmd` for PLAY/PAUSE), and optionally CSIP Set Member with SIRK and rank from Kconfig. Sink streams are auto-started from the `enabled` callback. Device name is set to `TMAP Peripheral`.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

> **This board must be built to match the topology [tmap_central](../central/) is built for.** Neither side negotiates it: the central looks for a fixed number of peers, each exposing a fixed number of sink ASEs with a specific channel. A mismatch stalls in discovery or fails to build the unicast group.

| Central **Unicast topology** | Boards of this example | **Earbuds type** | **Earbud Location** |
| --- | --- | --- | --- |
| `1 peripheral, 1 CIS (TX+RX)` | 1 | `Single headset` | `Mono` |
| `1 peripheral, 2 CISes (TX+RX, TX)` **(default)** | 1 | `Single headset` **(default)** | `Stereo` **(default)** |
| `2 peripherals (coordinated set), 2 CISes (TX+RX, TX)` | 2 | `Duo headset` on both | `Left` on one, `Right` on the other |

Under **Example: TMAP Peripheral (CT & UMR)**:

* **Earbuds type** — `Single headset` (standalone: no CSIS, no RSI) or `Duo headset` (selects `BT_CSIP_SET_MEMBER` and `BT_CAP_ACCEPTOR_SET_MEMBER`, enabling CSIS and adding RSI to the advertisement).
* **Earbud Location** — sets the PACS sink and source location bitmap, and **this is what the central uses to pick the channel it sends here**. The options offered follow the earbuds type, so the two choices together can only express a topology the central supports:
  * with `Single headset` — a whole device, so mono or stereo:
    * `Mono` — empty bitmap; the central sends one mono stream. Pair with the central's `1 peripheral, 1 CIS` topology.
    * `Stereo` (default) — `FRONT_LEFT | FRONT_RIGHT`, so one board renders both channels. Matches the central's default `1 peripheral, 2 CISes` topology.
  * with `Duo headset` — one of a pair, so left or right:
    * `Left` (default) — `FRONT_LEFT`, and rank 1.
    * `Right` — `FRONT_RIGHT`, and rank 2.
* **Device rank in set** — integer 1–2, only when Duo is selected; written into the CSIS register parameters. The CSIS instance is registered as lockable, which requires a non-zero rank that is **unique** within the set: duplicate ranks break the CSIP ordered access procedure the central locks the set with. The default follows Earbud Location (left → 1, right → 2), and a set member can only be left or right, so building the two boards for their respective ears already gives them distinct ranks. Rank orders the set lock and decides which member the central gives its bidirectional CIS to; it does **not** say which ear this board is.

The ASE counts come from `sdkconfig.defaults`: two sink ASEs and one source ASE, which is what the central's topologies need (two sinks on one device, or one sink per device, plus a source for the bidirectional CIS). Each ASE gets its own stream slot, RX metrics and — for source ASEs — its own TX scheduler, buffer and sequence number.

`CONFIG_BT_ASCS_MAX_ACTIVE_ASES` is pinned to 3 there as well, and it is **not** the same thing as the CIS count. It bounds how many ASEs may be non-idle at once, and a bidirectional CIS carries two ASEs on one CIS — so `1 peripheral, 2 CISes` needs 3 active ASEs (two sinks plus one source) over only 2 CIS. Its Kconfig default is `BT_ISO_MAX_CHAN`, i.e. 2, which is one short; leaving it at the default makes the third codec configuration fail with ASCS response `0x0D` (Insufficient Resources) and `AscsAseNewFail` in the library log, and the central reports `Unicast start completed, err -77`.

### Security & Pairing

Just-Works pairing (LE Secure Connections, no MITM, no I/O capability) with bonding enabled, inherited from `../../common_components/example_init/ble_audio_example_init.c`.

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

A coordinated set needs this example on two boards, one built for the `Left`
ear and the other for `Right`.

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. `app_main` initializes NVS, Bluetooth, and the audio common layer, then registers TMAP CT+UMR.
2. If `BT_CSIP_SET_MEMBER` is on, the CSIP set member registers via the CAP acceptor, generates an RSI, and is plumbed into `start_info.csis_insts[0]`.
3. VCP volume renderer, BAP unicast server (PACS, ASCS callbacks, stream ops), CCP call control (TBS client), and MCP controller (MCC) are initialized.
4. The audio stack starts, device name is set, and extended advertising starts on handle 0.
5. On ACL connect the connection handle is stored; on MTU change GATT service discovery starts; on discovery complete `tmap_discover_tmas` runs.
6. TMAP discovery records whether the peer is CG and/or UMS, then `ccp_discover_tbs` triggers TBS discovery.
7. After GTBS is discovered the TBS URI list is read; `read_uri_schemes_string_cb` saves the first URI and chains into `mcp_discover_mcs`.
8. MCS discovery walks player name → track title → duration → position → playback speed → seeking speed → playing order → orders supported → media state → opcodes → CCID.
9. ASCS callbacks accept config/QoS/enable/start; the `enabled` stream op auto-starts sink streams (receiver-start-ready); RX metrics are reset on start and updated per received SDU, per sink ASE.
10. A source ASE that reaches "started" arms its own TX scheduler at the QoS interval and transmits dummy SDUs back to the central — this is the uplink half of the central's bidirectional CIS. The scheduler and buffer are released when the stream stops, when its CIS disconnects, or when the ASE is released.
11. Helpers `initiate_call`/`terminate_call` (gated on peer CG) and `play_media`/`pause_media` (gated on peer UMS) issue TBS and MCC commands.
12. On disconnect, the connection handle is cleared and advertising restarts.

## Expected Log

Initialization phase:

```
TMAP_PER: TMAP CT UMR initialized
TMAP_PER: CSIP set member initialized
TMAP_PER: PRSI: 0x<hex>
TMAP_PER: vcp vol renderer, vocs_cnt <n> aics_cnt <n>
TMAP_PER: VCP volume renderer initialized
TMAP_PER: BAP unicast server initialized: 2 sink / 1 source ASE
TMAP_PER: CCP call controller initialized
TMAP_PER: MCP controller initialized
TMAP_PER: Advertising started (handle 0)
```

Connection / discovery:

```
TMAP_PER: Connected: handle <h> role <r> peer <addr>
TMAP_PER: Security: handle <h> level <l> bonded <b>
TMAP_PER: MTU updated: handle <h> mtu <m>
TMAP_PER: Service discovery started: handle <h>
TMAP_PER: Service discovery complete: handle <h> status 0
TMAP_PER: TMAP discovery done
TMAP_PER: Discovered GTBS
TMAP_PER: Discovered remote URI <uri>
TMAP_PER: Discover MCS succeeded
TMAP_PER: Read player name succeeded, name <name>
TMAP_PER: Read track title succeeded, title <title>
TMAP_PER: Read media state succeeded, state <s>
TMAP_PER: Read content control id succeeded, ccid <c>
```

ASCS / unicast streaming. Which streams appear depends on the topology the central was built for — a second `SNK #1` when it drives two CISes onto this device, and `SRC #0` whenever it pairs a source into the bidirectional CIS:

```
TMAP_PER: [SNK] Config request:
TMAP_PER: [SNK #0] QoS request:
TMAP_PER: [SNK #0] Enable request (meta_len <n>)
TMAP_PER: [SNK #0] Stream enabled
TMAP_PER: [SNK #0] Start request
TMAP_PER: [SNK #0] Stream started
TMAP_PER: [SNK #0] RX: <count> packets
TMAP_PER: [SRC #0] Stream started
TMAP_PER: [SRC #0] TX: <count> packets
TMAP_PER: [SNK #0] Stop request
TMAP_PER: [SNK #0] Stream stopped, reason 0x<rr>
TMAP_PER: [SNK #0] Disable request
TMAP_PER: [SNK #0] Release request
```

Each ASE keeps its own counters, so `[SNK #0]` and `[SNK #1]` advance independently rather than sharing one total.

VCP / call control / disconnect:

```
TMAP_PER: VCS volume <v>, mute <m>
TMAP_PER: VCS flags 0x<ff>
TMAP_PER: Call <i> originated
TMAP_PER: Call <i> terminated
TMAP_PER: Disconnected: handle <h> reason 0x<rr>
```

**Note — disconnect race.** On peer-initiated disconnect (e.g. supervision timeout, peer drops the link) with active streams, an additional Bluedroid error may interleave:

```
W BT_APPL: gattc_conn_cb: if=4 st=0 id=4 rsn=0x8
W BT_HCI: hcif disc complete: hdl 0x0, rsn 0x8 dev_find 1
TMAP_PER: [SNK #0] ISO disconnected, reason 0x08
TMAP_PER: [SNK #0] Stream disabled
E BT_APPL: Unknown connection ID: 3 fail sending notification
TMAP_PER: [SNK #0] Stream stopped, reason 0x08
```

`Unknown connection ID` is harmless. GATT notifications for the ASE state changes are queued for Bluedroid to send. If Bluedroid clears the BTA connection on the ACL disconnect before those queued sends drain, they fail with this error. The peer has already disconnected, so the missed notifications have no effect on either side.

Tag is `TMAP_PER`.

## Peer Pairing

Run [tmap_central](../central/) on another board. How many boards of this example are needed, and how they must be configured, follows the topology the central is built for:

| Central topology | Boards of this example | Configuration |
| --- | --- | --- |
| `1P_1CIS` | 1 | `Single headset` with the `Mono` location, so the central sends one mono stream |
| `1P_2CIS` | 1 | `Stereo` location, so PACS advertises both channels |
| `2P_2CIS` | 2 | `Duo headset` on both, `Left` on one and `Right` on the other — the rank default follows, giving them 1 and 2 |

1. Flash this peripheral and start it; advertising begins on handle 0.
2. Flash and start the central; it scans for TMAS+UMR (plus a matching CSIS RSI in a coordinated-set build) and connects to this device.
3. After pairing and MTU exchange, both sides complete TMAP discovery; this peripheral then discovers GTBS and reads the URI list.
4. The MCC chain reads the central's media proxy state; the central's CAP initiator configures and starts the streams.
5. The peripheral auto-starts each sink stream from the `enabled` callback and logs its received SDU metrics; a configured source stream transmits back on the bidirectional CIS.
6. Use `initiate_call`/`terminate_call` to drive the central's TBS, and `play_media`/`pause_media` to send PLAY/PAUSE via MCC.

For the two-board set, the CSIS instance is lockable, so the central takes the set lock by rank before configuring the streams and releases it afterwards. Each board logs `Client <p> locked the lock` / `... released the lock`.
