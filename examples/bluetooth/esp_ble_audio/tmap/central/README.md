| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# TMAP Central Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example takes the **TMAP Call Gateway (CG)** and **Unicast Media Sender (UMS)** roles, registered together via `esp_ble_audio_tmap_register(ESP_BLE_AUDIO_TMAP_ROLE_CG | ESP_BLE_AUDIO_TMAP_ROLE_UMS)`. It scans for connectable extended advertising that carries TMAS service data with the **UMR** role bit set, connects, pairs, exchanges MTU, and then drives TMAP, CSIP and VCP discovery before bringing up unicast audio.

The example uses the ESP-BLE-AUDIO library pieces for: CAP initiator with the BAP Unicast Client (LC3 preset 48_2_1, MEDIA context), CSIP Set Coordinator, VCP Volume Controller, MCP server backed by the media proxy player, and CCP server registering a single GTBS bearer (`Generic TBS`, UCI `un000`, `tel,wechat` URI schemes, 5G technology). Each transmitting stream runs its own TX scheduler in the ISO task, feeding dummy ISO SDUs filled with that stream's sequence number. Device name is set to `TMAP Central`.

## Topology

The number of peripherals and the number of CIS is a build-time choice. The first CIS is always **bidirectional** — one sink and one source ASE of the same peripheral share it — and a second CIS, when present, is **transmit-only**:

| Topology | Peripherals | CIS #0 | CIS #1 |
| --- | --- | --- | --- |
| `1P_1CIS` | 1 | TX+RX — sink #0 + source #0 | — |
| `1P_2CIS` (default) | 1 | TX+RX — sink #0 + source #0 | TX — sink #1 of the same peer |
| `2P_2CIS` | 2 (CSIP coordinated set) | TX+RX — rank 1 member | TX — rank 2 member |

All CIS live in a single CIG. Which endpoint goes on which CIS is expressed by one table (`bindings[]` in `main/cap_initiator.c`); everything else in the state machine is shared between the three topologies.

> **The topology picked here must match how [tmap_peripheral](../peripheral/) is built.** The central does not negotiate the topology — it looks for a fixed number of peers and a fixed number of sink ASEs on each. A mismatch does not fall back gracefully: it stalls in discovery or refuses to build the group.

| Central **Unicast topology** | Peripheral boards | **Earbuds type** | **Earbud Location** |
| --- | --- | --- | --- |
| `1 peripheral, 1 CIS (TX+RX)` | 1 | `Single headset` | `Mono` |
| `1 peripheral, 2 CISes (TX+RX, TX)` **(default)** | 1 | `Single headset` **(default)** | `Stereo` **(default)** |
| `2 peripherals (coordinated set), 2 CISes (TX+RX, TX)` | 2 | `Duo headset` on both | `Left` on one, `Right` on the other |

What a mismatch looks like:

* central built for 2 peripherals, only one flashed → stops after `Peer 1/2 connected, scanning for the rest` and never starts audio
* central built for 2 CISes, peripheral left on `Mono` → `[SNK #1] Peer 0 has no sink endpoint 1` and `Failed to create unicast group`
* peripheral built with `CONFIG_BT_ASCS_MAX_ACTIVE_ASES` below 3 → the third codec configuration is rejected with `[SNK #1] Config response, rsp_code 13` (Insufficient Resources) and `Unicast start completed, err -77`
* two set members flashed with the same ear → same rank on both, `both report rank 1`, streams come up unlocked

The peripheral's Kconfig only offers the location options that suit its earbuds type — mono/stereo for a single headset, left/right for a duo — so the two peripheral settings on their own cannot express an unsupported combination. What still has to be kept in step is picking the *right* one of the three rows above.

### Channel allocation

The rendered channel comes from the peer's **PACS Sink Audio Locations**, not from its CSIP rank — CSIS only requires a rank to be unique and non-zero, it says nothing about which ear a device is. `sink_preset()` in `main/cap_initiator.c` resolves it as:

* peer carries **one** sink stream → its own advertised location (`FRONT_LEFT` / `FRONT_RIGHT`), or mono when it advertises an empty bitmap or both channels
* peer carries **two** sink streams → front left for the first, front right for the second

An acceptor that publishes no location therefore gets a mono stream, which is what the peripheral example's `Mono` location produces.

Rank still decides *which* member gets the bidirectional CIS in `2P_2CIS`, which is a separate question from which ear it is.

### Coordinated set (`2P_2CIS`)

The central resolves each advertised RSI against the SIRK hardcoded in `main/csip_set_coordinator.c`, connects only members of that set, orders them by CSIS rank, and runs the CSIP ordered access procedure — locking the set ascending by rank before stream setup and releasing it once `unicast_start_complete` fires.

CAP names the members by their CSIS instance (`ESP_BLE_AUDIO_CAP_SET_TYPE_CSIP`) as soon as every member exposes one, and falls back to naming them by connection (`..._AD_HOC`) otherwise — that fallback is what lets the same code path serve a peripheral without CSIS. Both forms resolve to the same connections; the set lock itself is driven by the CSIP layer, not by the CAP set type.

Volume is treated as a property of the whole set: every member gets its own VCP volume controller, and volume / mute go through the **CAP commander** (`esp_ble_audio_cap_commander_change_volume*`) so both earbuds change together. The commander refuses to run until each member has a discovered volume controller, so the example reads each member's VCS state first — that also syncs the VCS change counter.

To make that visible, the example keeps driving the set once the streams are up: every 5 minutes it issues one commander procedure, walking the volume through 32 / 96 / 160 / 224 and then muting and unmuting. Each step is a single procedure covering every member, so both earbuds report the same value in the same tick rather than being written one at a time. It runs on its own application task — not the audio task and not the esp_timer one, which the stack's own timers share — and the audio APIs take the host lock themselves, so calling them from there is safe. A tick is skipped rather than queued if the previous procedure has not reported back, and the cycle pauses as soon as the set is no longer whole.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* One or two boards running [tmap_peripheral](../peripheral/), depending on the topology

## Configuration

```bash
idf.py menuconfig
```

Under **Example: TMAP Central (CG & UMS)** → **Unicast topology**, pick one — and build the peripheral to match, per the table in [Topology](#topology):

* **1 peripheral, 1 CIS (TX+RX)** (`EXAMPLE_TMAP_CEN_TOPO_1P_1CIS`) — peripheral: `Single headset` + `Mono`.
* **1 peripheral, 2 CISes (TX+RX, TX)** (`EXAMPLE_TMAP_CEN_TOPO_1P_2CIS`, **default**) — peripheral: `Single headset` + `Stereo`, which is also its default. It must expose two sink ASEs (`CONFIG_BT_ASCS_MAX_ASE_SNK_COUNT`, 2 in its `sdkconfig.defaults`).
* **2 peripherals (coordinated set), 2 CISes (TX+RX, TX)** (`EXAMPLE_TMAP_CEN_TOPO_2P_2CIS`) — selects `BT_CSIP_SET_COORDINATOR`. Peripheral: `Duo headset` on both boards, one `Left` and one `Right`; their ranks default from that and must stay distinct. Rank picks which member gets the bidirectional CIS, the location picks the channel.

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

(Exit serial monitor with `Ctrl-]`.)

## Example Flow

1. `app_main` initializes NVS, Bluetooth, the audio common layer, and registers TMAP CG+UMS.
2. Sub-modules initialize in order: CAP initiator (one TX scheduler per sink stream), CSIP set coordinator, VCP volume controller plus CAP commander callbacks, MCP server (media proxy), CCP server (GTBS bearer).
3. The audio stack starts, the device name is set, and extended scanning begins.
4. Each connectable scan result is parsed for TMAS service data and, in the two-peripheral topology, for the CSIS RSI. A peer is taken when its TMAP role contains UMR and — for a coordinated set build — its RSI resolves against our SIRK. Peers already connected are skipped by address.
5. On ACL connect, security is initiated; on security change, an MTU exchange is requested.
6. After the MTU exchange and GATT discovery both complete, TMAP, VCP and CSIP discovery start for that peer, and scanning resumes if more peers are still wanted. VCP discovery runs on **every** member, not just the first.
7. Once every peer has been through set discovery, the members are ordered by rank, the set is locked (two-peripheral topology only) and `cap_initiator_setup` runs CAS discover → sink and source ASE discovery on every member. The CAS discovery also records each peer's CSIS instance, and PACS reports its Sink Audio Locations.
8. When the last member reports back, the unicast group is created from `bindings[]` — each sink's channel resolved from that peer's advertised location — and unicast audio starts. The set lock is released when the start procedure completes.
9. Each sink stream that reaches "started" arms its own TX scheduler at the QoS interval and sends ISO SDUs filled with its own sequence number; the source stream's incoming SDUs are counted as RX metrics.
10. Once the streams are up and every member's VCS state has been read, a periodic demo starts driving the set through the CAP commander — one procedure every 5 minutes, cycling volume and then mute, each applying to the whole set at once.
11. Losing one member costs the others nothing: the CIG stays up, their CIS keep streaming, and the departed member's slot in the group is held for whatever comes back to fill it. Only that member is scanned for and rediscovered, and starting it again reuses its slot — releasing a stream leaves it in the group, and configuring it again rebinds the new endpoint to the CIS the slot already owns. The group is deleted only when the last member goes.

## Expected Log

Initialization phase:

```
TMAP_CEN: CAP initiator initialized: 2 peer(s), 2 CIS
TMAP_CEN: CSIP set coordinator initialized
TMAP_CEN: VCP volume controller initialized
TMAP_CEN: MCP server initialized
TMAP_CEN: Registered gtbs bearer <n>
```

Scan and connect (the `2/2` count and the `set member` note only appear in a coordinated-set build):

```
TMAP_CEN: Found peer 1/2 (TMAS UMR, set member)
TMAP_CEN: Connected: handle <h> role <r> peer <addr>
TMAP_CEN: Security: handle <h> level <l> bonded <b>
TMAP_CEN: MTU updated: handle <h> mtu <m>
TMAP_CEN: Service discovery started: handle <h>
TMAP_CEN: Service discovery complete: handle <h> status 0
TMAP_CEN: TMAP discovery done: handle <h>
TMAP_CEN: Peer 1/2 connected, scanning for the rest
TMAP_CEN: CSIP discovered: handle <h> set_size 2 rank 1 lockable 1
```

Set lock and CAP setup:

```
TMAP_CEN: All 2 peer(s) ready
TMAP_CEN: Set locked
TMAP_CEN: CAP initiator setup for 2 member(s)
TMAP_CEN: Found CAS with CSIS: handle <h>
TMAP_CEN: [SNK #0] Endpoint discovered on handle <h>
TMAP_CEN: [SRC #0] Endpoint discovered on handle <h>
TMAP_CEN: Sink discover complete: handle <h>
TMAP_CEN: Source discover complete: handle <h>
TMAP_CEN: [SNK #0] Peer 0 (loc 0x00000001) renders front left
TMAP_CEN: [SNK #1] Peer 1 (loc 0x00000002) renders front right
TMAP_CEN: Created unicast group: 2 CIS
TMAP_CEN: Started 3 unicast stream(s) as a coordinated set
TMAP_CEN: Unicast start completed
TMAP_CEN: Set released
```

Volume, applied to the whole set through the CAP commander:

```
TMAP_CEN: VCP discovery done, vocs <n> aics <n>
TMAP_CEN: VCP state cb done, volume <v> mute <m>
TMAP_CEN: Driving the volume of 2 member(s) every 300 s
TMAP_CEN: Changing volume of 2 member(s) to 32
TMAP_CEN: Set volume changed
TMAP_CEN: Changing volume of 2 member(s) to 96
TMAP_CEN: Set volume changed
TMAP_CEN: Changing volume mute state of 2 member(s) to 1
TMAP_CEN: Set volume mute changed
```

Streaming phase. Each stream reports its own count, so the two sinks advance independently and the source counts what the rank 1 peripheral sends back:

```
TMAP_CEN: [SNK #0] Stream started
TMAP_CEN: [SNK #0] Streaming, interval <i>, length <l>
TMAP_CEN: [SNK #1] Stream started
TMAP_CEN: [SNK #1] Streaming, interval <i>, length <l>
TMAP_CEN: [SNK #0] TX: <count> packets
TMAP_CEN: [SNK #1] TX: <count> packets
TMAP_CEN: [SRC #0] RX: <count> packets
```

Call control / disconnect:

```
TMAP_CEN: CCP: Placing call to remote with id <i> to <caller>
TMAP_CEN: CCP: Call terminated for id <i> with reason <r>
TMAP_CEN: [SNK #0] Stream stopped, reason 0x<rr>
TMAP_CEN: [SNK #0] ISO disconnected, reason 0x<rr>
TMAP_CEN: Disconnected: handle <h> reason 0x<rr>
TMAP_CEN: Deleted unicast group
```

Tag is `TMAP_CEN`.

## Peer Pairing

Run [tmap_peripheral](../peripheral/) on one or two boards, matching the topology.

### 1 peripheral (`1P_1CIS` / `1P_2CIS`)

1. Flash the peripheral and let it start extended advertising. `1P_2CIS` works with its defaults (`Single headset` + `Stereo`); for `1P_1CIS` switch its location to **Mono**.
2. Flash and start this central; it scans for TMAS+UMR.
3. The central connects, pairs, and exchanges MTU with the peripheral.
4. TMAP discovery completes on both sides; the central runs CAP unicast setup against the peripheral's ASCS.
5. The central starts the sink stream(s) and feeds ISO SDUs at the QoS interval; the peripheral transmits back on the bidirectional CIS.
6. The central can drive the peripheral's volume via VCP and accept TBS originate/terminate from the peripheral.

### 2 peripherals (`2P_2CIS`)

1. Build the peripheral twice as **Duo headset**, once with **Earbud Location** = **Left** and once with **Right**. The rank default follows the location (left → 1, right → 2), so the two boards get distinct ranks without touching **Device rank in set**. Flash one image per board.
2. Flash and start this central. It connects the first member it finds, resumes scanning, then connects the second.
3. After both members finish CSIP discovery the central locks the set by rank, sets up one CIG with two CIS, and releases the lock when the procedure completes.
4. The rank 1 board gets the bidirectional CIS and transmits back; the rank 2 board gets a transmit-only CIS. Which channel each renders comes from its **Earbud Location**, so a rank/ear mismatch shows up in the `renders front left` / `renders front right` log rather than silently sending the wrong channel.
5. Once both streams are up and both members' VCS state is read, the volume demo starts: every 5 minutes one CAP commander procedure moves both boards to the same volume, and the mute steps at the end of the cycle mute and unmute them together. Both boards log the same value within one tick — that is the point of routing it through the commander instead of writing each VCS separately.
6. Losing either board leaves the other one streaming: its CIS is untouched, and the central just rescans for the missing one and re-enters it into the slot it left. The surviving earbud logs nothing at all — no `Disable request`, no `ISO disconnected` — while the central logs `Lost a member; 1 still streaming` and later `[SNK #n] Already streaming, left alone`.
