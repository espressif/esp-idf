| Supported Targets | ESP32 | ESP32-S31 |
| ----------------- | ----- | --------- |

A2DP-SOURCE-AAC EXAMPLE
========================

Example of A2DP audio **source** role with **external codec** and **software AAC encoding**.

This example uses the Bluedroid **external codec** path: the application generates PCM, encodes it to MPEG-2/4 AAC (M24) with [esp_audio_codec](https://components.espressif.com/components/espressif/esp_audio_codec), and sends the encoded stream.

## Required components

- [espressif/esp_audio_codec](https://components.espressif.com/components/espressif/esp_audio_codec)

## How to use this example

### Hardware Required

This example runs on any commonly available ESP32 or ESP32-S31 development board.

It is intended to connect to an A2DP sink that supports AAC, such as [A2DP sink stream AAC example](../a2dp_sink_stream_aac).

### Configure the project

```
idf.py menuconfig
```

* Enable Classic Bluetooth and A2DP under Component config --> Bluetooth --> Bluedroid Enable

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output.

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

For the first step, this example performs device discovery to search for a target device (A2DP sink) whose device name is "ESP_SPEAKER_AAC" and whose "Rendering" bit of its Service Class field is set in its Class of Device (COD). If a candidate target is found, the local device will initiate connection with it.

After connection with A2DP sink is established, the example performs the following running loop 
1-2-3-4-1:
1. audio transmission starts and lasts for a while
2. audio transmission stops
3. disconnect with target device
4. reconnect to target device

The example implements an event loop triggered by a periodic "heart beat" timer and events from Bluetooth protocol stack callback functions.

After the local device discovers the target device and initiates connection, there will be logging message like this:

```
I (4090) BT_AV: Found a target device, address xx:xx:xx:xx:xx:xx, name ESP_SPEAKER_AAC
I (4090) BT_AV: Cancel device discovery ...
I (4100) BT_AV: Device discovery stopped.
I (4100) BT_AV: a2dp connecting to peer: ESP_SPEAKER_AAC
```

If connection is set up successfully, there will be such message:

```
I (5100) BT_AV: a2dp connected
```

Starting of audio transmission has the following notification message:

```
I (10880) BT_AV: a2dp media ready checking ...
...
I (10880) BT_AV: a2dp media ready, starting ...
...
I (11400) BT_AV: a2dp media start successfully.
I (11410) BT_AV_AAC: AAC enc: 44100 Hz, ch 2, bitrate 160000 (vbr=0)
I (11420) BT_AV_AAC: stream: 23219 us/frame, 44100 Hz, 1024 samples
```

Stop of audio transmission, and disconnection with remote device generate the following notification message:

```
I (110880) BT_AV: a2dp media suspending...
I (110920) BT_AV: a2dp media suspend successfully, disconnecting...
I (111040) BT_AV: a2dp disconnected
```

### Audio content

PCM is a **440 Hz sine tone** (stereo by default, 16-bit). Each frame is encoded to AAC and paced to match the negotiated sample rate. On the sink side you should hear a steady tone, not random noise.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
