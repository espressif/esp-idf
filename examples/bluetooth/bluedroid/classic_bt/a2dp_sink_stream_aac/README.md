| Supported Targets | ESP32 | ESP32-S31 |
| ----------------- | ----- | --------- |

A2DP-SINK-STREAM-AAC EXAMPLE
======================

Example of A2DP audio sink role with **external codec** and **software decoding** (SBC and AAC).

This example uses the Bluedroid **external codec** path: encoded media (SBC or MPEG-2/4 AAC) is delivered to the application through `esp_a2d_sink_register_audio_data_callback()`. The application decodes the stream with [esp_audio_codec](https://components.espressif.com/components/espressif/esp_audio_codec).

## Required components

- [bt_app_core_utils](../common/bt_app_core_utils)
- [bredr_app_common_utils](../common/bredr_app_common_utils)
- [a2dp_sink_common_utils](../common/a2dp_utils/a2dp_sink_common_utils)
- [a2dp_sink_int_codec_utils](../common/a2dp_utils/a2dp_sink_int_codec_utils) (I2S / DAC output)
- [a2dp_sink_ext_codec_utils](../common/a2dp_utils/a2dp_sink_ext_codec_utils)
- [espressif/esp_audio_codec](https://components.espressif.com/components/espressif/esp_audio_codec)

Detailed information can be viewed through the [../common/README.md](../common/README.md).

## How to use this example

### Hardware Required

#### ESP32

To play the sound, there is a need of loudspeaker and possibly an external I2S codec. Otherwise the example will only show a count of audio data packets received silently. Internal DAC can be selected and in this case external I2S codec may not be needed.

For the I2S codec, pick whatever chip or board works for you; this code was written using a PCM5102 chip, but other I2S boards and chips will probably work as well. The default I2S connections are shown below, but these can be changed in menuconfig:

| ESP pin   | I2S signal   |
| :-------- | :----------- |
| GPIO27    | LRCK         |
| GPIO25    | DATA         |
| GPIO26    | BCK          |

If the internal DAC is selected, analog audio will be available on GPIO25 and GPIO26. The output resolution on these pins will always be limited to 8 bit because of the internal structure of the DACs.

#### ESP32-S31

The default output is I2S. If I2S is used, it can be configured through menuconfig. The default I2S connections are shown below:

| ESP pin   | I2S signal   |
| :-------- | :----------- |
| GPIO3     | LRCK         |
| GPIO4     | DATA         |
| GPIO2     | BCK          |

Currently, DAC is not supported.

### Configure the project

```
idf.py menuconfig
```

* Choose external I2S codec or internal DAC for audio output, and configure the output PINs under A2DP Sink Internal Codec Example Configuration.

### Stream Endpoint (SEP) registration (one source device)

This example registers codec capabilities with `esp_a2d_sink_register_stream_endpoint()` after A2DP sink init and before connecting. A2DP requires **at least one SBC SEP**; using AAC therefore needs `ESP_A2D_MAX_SEPS >= 2`. Capabilities that do not meet A2DP Profile (SNK) fail with `ESP_A2D_SEP_REG_UNSUPPORTED` in `ESP_A2D_SEP_REG_STATE_EVT`. Overwriting the last SBC SEP fails with `ESP_A2D_SEP_REG_SBC_REQUIRED`.

**SBC (SNK)**

| Field | Mandatory |
| :---- | :-------- |
| Sampling frequency | 44.1 kHz and 48 kHz |
| Channel mode | Mono, Dual Channel, Stereo, and Joint Stereo |
| Block length | 4, 8, 12, and 16 |
| Subbands | 4 and 8 |
| Allocation method | SNR and Loudness |
| Bitpool | min = 2; max ≥ 53 (High Quality Joint Stereo 44.1 kHz, A2DP Table 4.7); max ≤ 250 |

**AAC (SNK)**

| Field | Mandatory |
| :---- | :-------- |
| Object type | MPEG-2 AAC LC |
| MPEG-D DRC | Must be 0 if only MPEG-2 AAC LC is advertised |
| Sampling frequency | 44.1 kHz and 48 kHz |
| Channels | 1 and 2 |
| VBR | Supported |

### Dual A2DP sink (two source devices)

This example can keep two A2DP sources connected at the same time, with the same last-started-wins speaker behavior as `a2dp_sink_stream`. The stack discards media from the inactive source; the application keeps **one** decoder and **one** I2S/DAC path and reconfigures them when `ESP_A2D_SNK_ACTIVE_DEVICE_EVT` reports a new active device. AAC and SBC may be used on different phones; takeover closes and reopens the shared decoder and updates the I2S clock.

Enable dual connection in menuconfig:

* `Component config` → `Bluetooth` → `Bluedroid Options` → `Maximum number of simultaneous A2DP sink connections` = 2 (`CONFIG_BT_A2DP_SINK_MAX_CONN=2`)
* Host ACL connections (`CONFIG_BT_ACL_CONNECTIONS`) and controller BR/EDR ACL max connections must be at least 2 (the ESP32 / ESP32-S31 defaults already are)

This example already sets `CONFIG_BT_A2DP_SEP_NUM_MAX=2` (AAC + SBC). The stack sizes the AVDTP SEP pool as `SEP_NUM_MAX * SINK_MAX_CONN`, so two connections with both codecs are covered without raising `SEP_NUM_MAX`.

Leave `CONFIG_BT_A2DP_SINK_MAX_CONN` at 1 (the default) for single-phone AAC/SBC playback.

With `MAX_CONN=2`:

* Phone B can connect while phone A is playing. B's `ESP_A2D_AUDIO_CFG_EVT` is cached and does not interrupt A's decoder or I2S clock.
* Starting playback on B switches rendering to B. A is suspended by the stack and stays connected; pause/play on A later takes over without reconnecting.
* Disconnecting one phone while the other is still connected leaves the audio path running.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output.

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

After the program is started, the example starts inquiry scan and page scan, awaiting being discovered and connected. Other bluetooth devices such as smart phones can discover a device named "ESP_SPEAKER_AAC". A smartphone or another ESP-IDF example of A2DP source can be used to connect to the local device.

Once A2DP connection is set up, there will be a notification message with the remote device's bluetooth MAC address like the following:

```
I (106427) BT_AV: A2DP connection state: Connected, [64:a2:f9:69:57:a4]
```

If a smartphone is used to connect to local device, starting to play music with an APP will result in the transmission of audio stream. The transmitting of audio stream will be visible in the application log including a count of audio data packets, like this:

```
I (120627) BT_AV: A2DP audio state: Started
I (122697) BT_AV: Undecoded audio package count: 100
I (124697) BT_AV: Undecoded audio package count: 200
I (126697) BT_AV: Undecoded audio package count: 300
I (128697) BT_AV: Undecoded audio package count: 400
```

With `CONFIG_BT_A2DP_SINK_MAX_CONN=2`, a second phone can connect while the first is playing. Starting music on the second phone logs which source is rendered:

```
I (140000) BT_AV: A2DP rendering [aa:bb:cc:dd:ee:ff], other sources suspended
```


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
