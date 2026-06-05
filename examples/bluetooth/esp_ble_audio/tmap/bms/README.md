| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# TMAP Broadcast Media Sender (BMS) Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements the **Telephony and Media Audio Profile (TMAP) Broadcast Media Sender (BMS)** role. The device acts as a non-connectable BAP Broadcast Source: extended advertising carries the TMAS service UUID with the BMS role flag, the Broadcast Audio Announcement service with a 24-bit broadcast ID (`0x123456`), and the complete local name (`"TMAP Broadcast Source"`); periodic advertising carries the encoded BASE.

The implementation uses the ESP-BLE-AUDIO library for the TMAP role registration and the CAP initiator broadcast pieces. A single subgroup with one stream is configured from the LC3 `48_2_1` broadcast preset (front-left location, media context). After the broadcast source is created and the BIG is started, a periodic TX scheduler (`example_audio_tx_scheduler_*`) drives ISO SDU transmission at the stream's QoS interval, and a sent callback tracks packet counts and drift.

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

### Security & Pairing

Security is inherited from `../../common_components/example_init/ble_audio_example_init.c`: LE Secure Connections with bonding enabled, no MITM, **Just-Works** pairing (`BLE_SM_IO_CAP_NO_IO`). The BMS broadcast itself is non-connectable and unencrypted, so these settings only apply if a peer ever opens an ATT connection.

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

1. Initialize NVS, the Bluetooth controller/host, and the LE Audio common layer (no GAP/GATT app callbacks).
2. Register the TMAP role as `ESP_BLE_AUDIO_TMAP_ROLE_BMS` and initialize the CAP initiator (register stream ops, init TX scheduler).
3. Start the audio stack, then build the create params (1 subgroup × 1 stream, LC3 48_2_1 preset, sequential packing, no encryption) and create the broadcast source.
4. Configure and start non-connectable extended advertising (TMAS+BMS role, Broadcast Audio + broadcast ID `0x123456`, device name) and periodic advertising carrying the BASE.
5. Add the advertising handle to the BIG and start broadcast audio; on stream-started, allocate the SDU buffer and arm the TX scheduler at `qos->interval`.
6. The scheduler callback fills SDUs with the rolling sequence number and calls `esp_ble_audio_cap_stream_send`; the sent callback updates RX-side metrics via `example_audio_tx_scheduler_on_sent`.
7. `cap_initiator_update` and `cap_initiator_stop` are exposed for runtime metadata refresh and teardown.

## Expected Log

TAG is `TMAP_BMS`.

Init:
```
I (xxx) TMAP_BMS: CAP initiator initialized
```

Setup:
```
I (xxx) TMAP_BMS: Creating broadcast source
I (xxx) TMAP_BMS: Advertising started (handle 0)
I (xxx) TMAP_BMS: CAP initiator setup
```

Streaming:
```
I (xxx) TMAP_BMS: [SRC #0] Stream started
```
(Subsequent per-packet metrics are emitted by `example_audio_tx_scheduler_on_sent` under the `SRC #0` label.)

Stop / teardown:
```
I (xxx) TMAP_BMS: [SRC #0] Stream stopped, reason 0x..
I (xxx) TMAP_BMS: [SRC #0] ISO disconnected, reason 0x..
I (xxx) TMAP_BMS: Advertising stopped (handle 0)
```

Errors (representative):
```
E (xxx) TMAP_BMS: Failed to create broadcast source, err ..
E (xxx) TMAP_BMS: Failed to start broadcast source, err ..
E (xxx) TMAP_BMS: [SRC #0] Invalid stream qos
E (xxx) TMAP_BMS: [SRC #0] Failed to alloc TX buffer, SDU ..
```

## Peer Pairing

Run [bmr](../bmr/) on a second board.

1. Flash and boot the BMR peer; it starts scanning for TMAP BMS advertisers.
2. Boot this BMS; it advertises with TMAS BMS role and broadcast ID `0x123456` and starts periodic advertising carrying the BASE.
3. The BMR matches the TMAS BMS role plus Broadcast Audio UUID, creates a PA sync, and waits for the BASE.
4. After BASE is received the BMR creates a broadcast sink and syncs to the BIG using the BIS index bitfield.
5. The BMS stream-started callback fires; its TX scheduler begins emitting SDUs that the BMR receives in its stream `recv` callback.
6. Stopping or power-cycling the BMS triggers PA sync loss on the BMR, which deletes the sink and resumes scanning.
