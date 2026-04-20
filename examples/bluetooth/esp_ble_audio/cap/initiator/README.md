| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# CAP Initiator Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements the **Common Audio Profile (CAP) Initiator** role on top of the NimBLE host stack with ISO and LE Audio support. It is built in one of two mutually exclusive sub-modes selected at build time: a **CAP Unicast Initiator / BAP Unicast Client**, or a **CAP Broadcast Source**. A small TX module (`cap_initiator_tx.c`) drives audio packets onto every started stream using the LE Audio example TX scheduler.

In **unicast** mode, the initiator scans for connectable extended advertising that carries CAS service data, connects, initiates pairing, exchanges MTU, performs GATT service discovery, then discovers CAS, sink ASEs and source ASEs, configures the codec with the LC3 16_2_1 unicast preset, creates an ad-hoc unicast group, and starts the streams. In **broadcast** mode, the initiator creates a CAP broadcast source with the LC3 16_2_1 broadcast preset, starts non-connectable extended advertising plus periodic advertising carrying the Broadcast Audio Announcement UUID, a hardcoded broadcast ID (`0x123456`), the local device name (`CAP Broadcast Source`), and the BASE, then starts the broadcast stream.

The GAP/GATT device name follows the build-time mode: `CAP Initiator` in unicast mode, `CAP Broadcast Source` in broadcast mode (matching the local-name field embedded in extended advertising, which is what the acceptor's self-scan path matches against). Stream location and context are hardcoded to `MONO_AUDIO` / `UNSPECIFIED` in the LC3 preset.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

Open menuconfig:

```bash
idf.py menuconfig
```

Under **Example: CAP Initiator** -> **CAP Initiator mode**:

* **Unicast** (`EXAMPLE_UNICAST`, default) — scan for a CAP Acceptor and set up unicast streams as BAP Unicast Client. Mutually exclusive with Broadcast.
* **Broadcast** (`EXAMPLE_BROADCAST`) — act as a CAP Broadcast Source; start extended + periodic advertising and a BIG so Broadcast Sinks can sync. Mutually exclusive with Unicast.

### Security & Pairing

Just-Works pairing (LE Secure Connections, no MITM, `BLE_SM_IO_CAP_NO_IO`) with bonding enabled, inherited from `../../common_components/example_init/ble_audio_example_init.c`. Change pairing or IO-cap there.

## Build & Flash

```bash
idf.py set-target esp32h4
idf.py -p PORT flash monitor
```

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. `app_main` initializes NVS, the Bluetooth controller (`bluetooth_init`) and the LE Audio common layer (`esp_ble_audio_common_init`); GAP/GATT callbacks are registered only in unicast mode.
2. The selected sub-mode initializes itself: `cap_initiator_unicast_init` registers CAP and BAP unicast-client callbacks, or `cap_initiator_broadcast_init` registers the broadcast stream ops.
3. `cap_initiator_tx_init` initializes the TX scheduler slot(s); `esp_ble_audio_common_start` starts the audio stack and the GAP device name is set per build-time mode (`CAP Initiator` for unicast, `CAP Broadcast Source` for broadcast).
4. **Unicast**: `cap_initiator_unicast_start` begins extended scanning. On a connectable advertising report containing CAS service data, it cancels scanning and calls `ble_gap_connect`.
5. **Unicast**: After ACL connect the initiator starts pairing, then on `SECURITY_CHANGE` exchanges MTU; on MTU change it starts GATT service discovery. Once both MTU and discovery are done, it calls `esp_ble_audio_cap_initiator_unicast_discover` (CAS), then BAP `discover` for SINK and SOURCE.
6. **Unicast**: It creates an ad-hoc unicast group with the LC3 16_2_1 QoS, then starts unicast audio. Stream callbacks log configured / qos_set / enabled / started; on start the sink (TX) stream is registered with the TX module.
7. **Broadcast**: `cap_initiator_broadcast_start` creates the broadcast source, configures and starts extended + periodic advertising (handle 0), adds the adv to the broadcast source, then starts the broadcast audio. On stream started the source stream is registered with the TX module.
8. On stream stopped/disconnected the TX module is unregistered; in unicast mode an ACL disconnect deletes the unicast group and re-arms scanning.

## Expected Log

TAG: `CAP_INI`.

### Unicast mode

```
I (xxx) CAP_INI: CAP initiator unicast initialized
I (xxx) CAP_INI: Scanning for CAP Acceptor...
I (xxx) CAP_INI: Found CAS in peer adv data!
I (xxx) CAP_INI: Connected: handle ... role ... peer ...
I (xxx) CAP_INI: Security: handle ... level ... bonded ...
I (xxx) CAP_INI: MTU updated: handle ... mtu ...
I (xxx) CAP_INI: Service discovery started: handle ...
I (xxx) CAP_INI: Service discovery complete: handle ...
I (xxx) CAP_INI: Discovering CAS
I (xxx) CAP_INI: Found CAS
I (xxx) CAP_INI: Discovering sinks
I (xxx) CAP_INI: [SNK] Endpoint discovered
I (xxx) CAP_INI: Discover sinks complete
I (xxx) CAP_INI: Discovering source ASEs
I (xxx) CAP_INI: [SRC] Endpoint discovered
I (xxx) CAP_INI: Discover sources complete
I (xxx) CAP_INI: Created unicast group
I (xxx) CAP_INI: Starting unicast streams
I (xxx) CAP_INI: [SNK #0] Stream configured, QoS preference:
I (xxx) CAP_INI: [SRC #0] Stream configured, QoS preference:
I (xxx) CAP_INI: [SNK #0] QoS set
I (xxx) CAP_INI: [SRC #0] QoS set
I (xxx) CAP_INI: [SNK #0] Stream enabled
I (xxx) CAP_INI: [SRC #0] Stream enabled
I (xxx) CAP_INI: Unicast start completed
I (xxx) CAP_INI: [SNK #0] Stream started
I (xxx) CAP_INI: [SNK #0] Started (SDU ... interval ... us)
I (xxx) CAP_INI: [SRC #0] Stream started
```

On disconnect:

```
I (xxx) CAP_INI: Disconnected: handle ... reason 0x...
I (xxx) CAP_INI: Deleted unicast group
I (xxx) CAP_INI: Scanning for CAP Acceptor...
```

### Broadcast mode

```
I (xxx) CAP_INI: CAP initiator broadcast initialized
I (xxx) CAP_INI: Creating broadcast source
I (xxx) CAP_INI: Advertising started (handle 0)
I (xxx) CAP_INI: [SRC #0] Stream started
I (xxx) CAP_INI: [SRC #0] Started (SDU ... interval ... us)
```

## Peer Pairing

Run the [acceptor](../acceptor/) on a second board. The initiator and acceptor must be configured for the **same sub-mode** — both `EXAMPLE_UNICAST`, or both `EXAMPLE_BROADCAST` — otherwise they will not pair.

### Unicast

1. Flash the acceptor with `EXAMPLE_UNICAST`; it advertises with CAS + ASCS service data.
2. Flash this initiator with `EXAMPLE_UNICAST`; it scans, finds CAS in the peer's adv data and connects.
3. Pairing, MTU exchange and GATT service discovery complete.
4. CAS, sink and source discovery complete; the initiator creates the unicast group with the LC3 16_2_1 preset.
5. Streams are configured, QoS-set, enabled and started; bidirectional audio begins.
6. On ACL disconnect the initiator deletes the unicast group and resumes scanning.

### Broadcast

1. Flash this initiator with `EXAMPLE_BROADCAST`; it advertises as `CAP Broadcast Source` (broadcast ID `0x123456`) and starts the BIG.
2. Flash the acceptor with `EXAMPLE_BROADCAST`. With `EXAMPLE_SCAN_SELF` set the acceptor scans for the broadcast source by name and broadcast ID; without it, a separate Broadcast Assistant must drive PA sync via BASS.
3. The acceptor PA-syncs, receives the BASE/BIGInfo and syncs the BIG.
4. The acceptor's `[SNK #0] Stream started` and the initiator's TX flow are now active.
