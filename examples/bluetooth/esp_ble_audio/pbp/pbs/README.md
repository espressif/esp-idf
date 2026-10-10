| Supported Targets | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | --------- |

# PBP Public Broadcast Source (PBS) Example

(See the README.md file in the upper level `examples` directory for more information about examples.)

## Overview

This example implements the **Public Broadcast Profile (PBP) Public Broadcast Source (PBS)** role. The device acts as a non-connectable, unencrypted broadcast source: extended advertising carries the Broadcasting Device appearance, the Broadcast Name (`"PBP Broadcast Source"`), the Broadcast Audio Announcement service with a 24-bit broadcast ID (`0x123456`), and the Public Broadcast Announcement (PBA) with the high quality (HQ) feature and Program Info metadata; periodic advertising carries the encoded BASE.

The implementation uses the ESP-BLE-AUDIO library for the PBA encoding and the CAP initiator broadcast pieces. A single subgroup with one stream is configured from the LC3 `48_2_1` broadcast preset (front-left location, media context); the BIS inherits the subgroup codec configuration. After the broadcast source is created and the BIG is started, a periodic TX scheduler (`example_audio_tx_scheduler_*`) drives ISO SDU transmission at the stream's QoS interval, and a sent callback tracks packet counts and drift. The SDUs are synthetic rather than encoded LC3 audio, so a receiver that decodes them produces unpredictable output (noise or silence).

## Requirements

* A board with BLE 5.2, ISO, and LE Audio support (e.g. ESP32-H4, ESP32-S31)
* Peer device running the paired example

## Configuration

```bash
idf.py menuconfig
```

No build-time options — runtime defaults are baked into source.

**Synchronize multi-stream TX with timestamps** (`CONFIG_EXAMPLE_AUDIO_TX_TIMESTAMP`, under **Example: Common Audio Utilities**) is not supported by this example and has no effect when enabled: it gives the SDUs of all BIS streams of a BIG the same timestamps so that the SDUs of one frame (e.g. the left and right audio) go in one event, while this example sends a single stream, so there is nothing to align. See `bap/broadcast_source` or `cap/initiator` for a multi-stream source that uses it.

### Security & Pairing

Security is inherited from `../../common_components/example_init/ble_audio_example_init.c`: LE Secure Connections with bonding enabled, no MITM, **Just-Works** pairing (`BLE_SM_IO_CAP_NO_IO`). The PBS broadcast itself is non-connectable and unencrypted, so these settings only apply if a peer ever opens an ATT connection.

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

1. Initialize NVS, the Bluetooth controller/host, and the LE Audio common layer (no GAP/GATT app callbacks).
2. Register the CAP stream ops, start the audio stack, initialize the TX scheduler, and set the device name.
3. Create the broadcast source (1 subgroup × 1 stream, LC3 48_2_1 preset, sequential packing, no encryption).
4. Encode the PBA (HQ feature, Program Info metadata), then build the extended advertising data (appearance, Broadcast Name, Broadcast Audio + broadcast ID `0x123456`, PBA) and the periodic advertising data carrying the BASE.
5. Configure and start non-connectable extended advertising and periodic advertising, then add the advertising handle to the BIG and start broadcast audio. If a later step fails, the advertising handle is removed, advertising is stopped, and the broadcast source is deleted.
6. On stream-started, allocate the SDU buffer and arm the TX scheduler at `qos->interval`; the scheduler callback fills SDUs with the rolling sequence number and calls `esp_ble_audio_cap_stream_send`, and the sent callback updates TX metrics via `example_audio_tx_scheduler_on_sent`.

## Expected Log

TAG is `PBP_PBS`.

Init:
```
I (xxx) PBP_PBS: PBP broadcast source initialized
```

Setup:
```
I (xxx) PBP_PBS: Starting high quality announcement, Broadcast ID 0x123456
I (xxx) PBP_PBS: Advertising started (handle 0)
```

Streaming:
```
I (xxx) PBP_PBS: [SRC #0] Stream started
```
(Subsequent per-packet metrics are emitted by `example_audio_tx_scheduler_on_sent` under the `SRC #0` label.)

Errors (representative):
```
E (xxx) PBP_PBS: Failed to create broadcast source, err ..
E (xxx) PBP_PBS: Failed to start broadcast source, err ..
E (xxx) PBP_PBS: [SRC #0] Invalid QoS
E (xxx) PBP_PBS: [SRC #0] Failed to alloc TX buffer, SDU ..
```

## Peer Pairing

Run [pbk](../pbk/) on a second board.

1. Flash and boot the PBK peer; it starts scanning for public broadcasts.
2. Boot this PBS; it advertises the HQ PBA, the Broadcast Name, and broadcast ID `0x123456`, and starts periodic advertising carrying the BASE.
3. The PBK matches the Broadcast Audio and PBA service data, creates a PA sync, and creates a broadcast sink.
4. After BASE and BIGInfo are received the PBK syncs to the BIG.
5. The PBS stream-started callback fires; its TX scheduler begins emitting SDUs that the PBK receives in its stream `recv` callback.
6. Stopping or power-cycling the PBS ends the BIG on the PBK, which deletes its sink and resumes scanning.
