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

The default output is idle. If I2S is used, it can be configured through menuconfig. Currently, DAC is not supported.

### Configure the project

```
idf.py menuconfig
```

* Choose external I2S codec or internal DAC for audio output, and configure the output PINs under A2DP Sink Internal Codec Example Configuration.

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


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
