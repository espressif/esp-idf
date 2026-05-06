| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# TMAP Peripheral Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example takes the **TMAP Call Terminal (CT)** and **Unicast Media Receiver (UMR)** roles, registered together via `esp_ble_audio_tmap_register(ESP_BLE_AUDIO_TMAP_ROLE_CT | ESP_BLE_AUDIO_TMAP_ROLE_UMR)`. It runs connectable extended advertising at a 200 ms interval that includes the GAP Earbud appearance, the ASCS/CAS/TMAS UUIDs, an ASCS targeted unicast announcement carrying sink and source contexts (UNSPECIFIED | CONVERSATIONAL | MEDIA | GAME | INSTRUCTIONAL), a CAS targeted announcement, the TMAS UMR|CT role payload, and the device name `tmap_peripheral`. When the Duo earbuds option is selected, a CSIS RSI is added to the advertisement.

The host stack is NimBLE. The example uses the ESP-BLE-AUDIO library pieces for: BAP Unicast Server with PACS (LC3 cap with 16/32/48 kHz, 7.5/10 ms duration, 30–155 octets), VCP Volume Renderer (initial volume 10, unmuted, with VOCS+AICS instances built from Kconfig counts), TBS client (CCP Call Terminal — discovers GTBS, reads URI list, originates/terminates calls), MCC controller (reads player name, track title/duration/position, playback/seeking speed, playing order, media state, opcodes, CCID, plus a `mcp_send_cmd` for PLAY/PAUSE), and optionally CSIP Set Member with SIRK and rank from Kconfig. Sink streams are auto-started from the `enabled` callback. Device name is set to `TMAP Peripheral`.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

Under **Example: TMAP Peripheral (CT & UMR)**:

* **Earbuds type** — `Single ear headset` or `Duo headset`. Duo selects `BT_CSIP_SET_MEMBER` and `BT_CAP_ACCEPTOR_SET_MEMBER`, enabling CSIS and adding RSI to the advertisement.
* **Device rank in set** — integer 1–2, only when Duo is selected; written into the CSIS register parameters.
* **Earbud Location** — `Left Ear` or `Right Ear`. Adds `FRONT_LEFT` / `FRONT_RIGHT` to the PACS sink/source location bitmap.

### Security & Pairing

Just-Works pairing (LE Secure Connections, no MITM, no I/O capability) with bonding enabled, inherited from `../../common_components/example_init/ble_audio_example_init.c`.

## Build & Flash

```bash
idf.py set-target esp32h4
idf.py -p PORT flash monitor
```

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
9. ASCS callbacks accept config/QoS/enable/start; the `enabled` stream op auto-starts sink streams (receiver-start-ready); RX metrics are reset on start and updated per received SDU.
10. Helpers `initiate_call`/`terminate_call` (gated on peer CG) and `play_media`/`pause_media` (gated on peer UMS) issue TBS and MCC commands.
11. On disconnect, the connection handle is cleared and advertising restarts.

## Expected Log

Initialization phase:

```
TMAP_PER: TMAP CT UMR initialized
TMAP_PER: CSIP set member initialized
TMAP_PER: PRSI: 0x<hex>
TMAP_PER: vcp vol renderer, vocs_cnt <n> aics_cnt <n>
TMAP_PER: VCP volume renderer initialized
TMAP_PER: BAP unicast server initialized
TMAP_PER: CCP call controller initialized
TMAP_PER: MCP controller initialized
TMAP_PER: Advertising started (handle 0)
```

Connection / discovery:

```
TMAP_PER: Connected: handle <h> role <r> peer <addr>
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

ASCS / unicast streaming:

```
TMAP_PER: [SNK] Config request:
TMAP_PER: [SNK #0] QoS request:
TMAP_PER: [SNK #0] Enable request (meta_len <n>)
TMAP_PER: [SNK #0] Stream enabled
TMAP_PER: [SNK #0] Start request
TMAP_PER: [SNK #0] Stream started
TMAP_PER: [SNK #0] Stop request
TMAP_PER: [SNK #0] Stream stopped, reason 0x<rr>
TMAP_PER: [SNK #0] Disable request
TMAP_PER: [SNK #0] Release request
```

VCP / call control / disconnect:

```
TMAP_PER: VCS volume <v>, mute <m>
TMAP_PER: VCS flags 0x<ff>
TMAP_PER: Call <i> originated
TMAP_PER: Call <i> terminated
TMAP_PER: Disconnected: handle <h> reason 0x<rr>
```

Tag is `TMAP_PER`.

## Peer Pairing

Run [tmap_central](../central/) on a second board.

1. Flash this peripheral and start it; advertising begins on handle 0.
2. Flash and start the central; it scans for TMAS+UMR and connects to this device.
3. After pairing and MTU exchange, both sides complete TMAP discovery; this peripheral then discovers GTBS and reads the URI list.
4. The MCC chain reads the central's media proxy state; the central's CAP initiator configures and starts the sink stream.
5. The peripheral auto-starts the sink stream from the `enabled` callback and logs received SDU metrics.
6. Use `initiate_call`/`terminate_call` to drive the central's TBS, and `play_media`/`pause_media` to send PLAY/PAUSE via MCC.
