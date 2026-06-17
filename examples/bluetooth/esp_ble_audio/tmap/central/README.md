| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# TMAP Central Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example takes the **TMAP Call Gateway (CG)** and **Unicast Media Sender (UMS)** roles, registered together via `esp_ble_audio_tmap_register(ESP_BLE_AUDIO_TMAP_ROLE_CG | ESP_BLE_AUDIO_TMAP_ROLE_UMS)`. It scans for connectable extended advertising that carries TMAS service data with the **UMR** role bit set, connects to the first match, pairs, exchanges MTU, and then drives TMAP and VCP discovery before bringing up unicast audio.

The example uses the ESP-BLE-AUDIO library pieces for: CAP initiator with the BAP Unicast Client (LC3 preset 48_2_1, sink direction, FRONT_LEFT, MEDIA context), VCP Volume Controller, MCP server backed by the media proxy player, and CCP server registering a single GTBS bearer (`Generic TBS`, UCI `un000`, `tel,wechat` URI schemes, 5G technology). A periodic TX scheduler in the ISO task feeds dummy ISO SDUs filled with the sequence number. Device name is set to `TMAP Central`.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

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
2. Sub-modules initialize in order: CAP initiator (with TX scheduler), VCP volume controller, MCP server (media proxy), CCP server (GTBS bearer).
3. The audio stack starts, the device name is set, and extended scanning begins.
4. Each connectable scan result is parsed for TMAS service data; if the peer's TMAP role contains UMR, scan is cancelled and a connection is created.
5. On ACL connect, security is initiated; on security change, an MTU exchange is requested.
6. After the MTU exchange and GATT discovery both complete, TMAP and VCP discovery start.
7. TMAP discovery completion triggers `cap_initiator_setup` → CAS discover → sink and source ASE discovery → unicast group create → unicast audio start.
8. When a sink stream reaches "started", the TX scheduler is started at the QoS interval and `unicast_audio_tx` sends ISO SDUs filled with the sequence number.
9. After VCP discovery, the volume state is read to sync the change counter, then a mute command is issued from the state callback.
10. On disconnect, the unicast group is deleted and scanning restarts.

## Expected Log

Initialization phase:

```
TMAP_CEN: CAP initiator initialized
TMAP_CEN: VCP volume controller initialized
TMAP_CEN: MCP server initialized
TMAP_CEN: Registered gtbs bearer <n>
TMAP_CEN: Scanning for peripheral...
```

Scan and connect:

```
TMAP_CEN: Found TMAS in peer adv data!
TMAP_CEN: Connected: handle <h> role <r> peer <addr>
TMAP_CEN: Security: handle <h> level <l> bonded <b>
TMAP_CEN: MTU updated: handle <h> mtu <m>
TMAP_CEN: Service discovery started: handle <h>
TMAP_CEN: Service discovery complete: handle <h> status 0
```

TMAP / VCP / CAP setup:

```
TMAP_CEN: TMAP discovery done
TMAP_CEN: CAP initiator setup
TMAP_CEN: Found CAS
TMAP_CEN: [SNK #0] Endpoint discovered
TMAP_CEN: Sink discover complete
TMAP_CEN: Source discover complete
TMAP_CEN: Created unicast group
TMAP_CEN: Started unicast audio
TMAP_CEN: VCP volume controller discovering
TMAP_CEN: VCP discovery done
TMAP_CEN: VCP state cb done, volume <v> mute <m>
```

Unicast streaming phase:

```
TMAP_CEN: [SNK #0] Stream configured, QoS preference:
TMAP_CEN: [SNK #0] QoS set
TMAP_CEN: [SNK #0] Stream enabled
TMAP_CEN: [SNK #0] Stream connected
TMAP_CEN: [SNK #0] Stream started
TMAP_CEN: Streaming, interval <i>, length <l>
TMAP_CEN: Unicast start completed
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

Run [tmap_peripheral](../peripheral/) on a second board.

1. Flash the peripheral and let it start extended advertising.
2. Flash and start this central; it scans for TMAS+UMR.
3. The central connects, pairs, and exchanges MTU with the peripheral.
4. TMAP discovery completes on both sides; the central runs CAP unicast setup against the peripheral's ASCS.
5. The central starts the sink stream and feeds ISO SDUs to the peripheral at the QoS interval.
6. The central can drive the peripheral's volume via VCP and accept TBS originate/terminate from the peripheral.
