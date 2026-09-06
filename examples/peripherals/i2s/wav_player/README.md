| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# I2S WAV Player Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example plays an embedded WAV file through a codec DAC over I2S STD TX. It supports two codecs, selectable in menuconfig:

* **ES8311** — low-power mono ADC/DAC codec (for example ESP-BOX and ESP32-S3-Korvo-2).
* **ES8389** — stereo ADC/DAC codec (for example ESP32-S31-Korvo-1).

The firmware embeds `assets/music.wav` in flash and loops it on the TX channel. A standard PCM WAV is a short RIFF header plus little-endian signed PCM; the example finds the `data` chunk and hands that payload to I2S. The clip stays mapped in flash and is copied in DMA-sized pieces, not into a RAM buffer. There is no microphone path. For recording, see [mic_recorder](../mic_recorder).

| Audio Setting | Value |
|:---:|:---:|
| Sample Rate |16000 Hz|
| Bits per Sample |16 bits|
| Channels |Stereo|
| Source |Embedded `assets/music.wav`|

These settings are defined as macros at the top of `main/wav_player_example_main.c` (see `EXAMPLE_SAMPLE_RATE`, `EXAMPLE_BITS_PER_SAMPLE`, etc.). **A replacement WAV must be uncompressed signed 16-bit PCM with the same rate, width, and channel count.**

`music.wav` is an excerpt of *Canon in D Major (piano only)* by [Ava](https://freemusicarchive.org/music/ava-drumm/single/canon-in-d-major-piano-only), released on the Free Music Archive under [CC0 1.0 Universal](https://creativecommons.org/publicdomain/zero/1.0/). The clip was resampled for this example. See [assets/README.md](assets/README.md) for the full provenance.

## How to Use Example

### Hardware Required

* A development board with a SoC that supports I2S STD and I2C (see `Supported Targets` table above).
* A USB cable for power supply and programming.
* A board or module with an ES8311 or ES8389 codec and a headphone or speaker output.

### Connection
```
┌─────────────────┐           ┌──────────────────────────┐
│       ESP       │           │      ES8311 / ES8389     │
│                 │           │                          │
│       I2S_MCK_IO├──────────►│MCLK                      │
│                 │           │                          │           ┌─────────┐
│       I2S_BCK_IO├──────────►│BCLK            analog out├───────────┤         │
│                 │           │                          │           │ EARPHONE│
│        I2S_WS_IO├──────────►│LRCK / WS                 │           │ / SPK   │
│                 │           │                          │           └─────────┘
│      I2S_DOUT_IO├──────────►│SDIN                      │
│                 │           │                          │
│       I2C_SCL_IO├──────────►│SCL / CCLK                │
│                 │           │                          │
│       I2C_SDA_IO│◄─────────►│SDA / CDATA               │
│                 │           │                          │
│        PA_CTRL  ├──────────►│amp EN / CTRL (optional)  │
│                 │           │                          │
│          VCC 3.3├───────────┤VCC                       │
│              GND├───────────┤GND                       │
└─────────────────┘           └──────────────────────────┘
```

The codec chip has no amplifier pin. `PA_CTRL` is the MCU GPIO that enables the board's speaker amplifier (HT8691, NS4150, and similar). Check the schematic: the enable is often labeled `EN`, `CTRL`, `PA`, or even `INT`. Set `Power amplifier enable IO` in menuconfig to that GPIO so the codec driver raises it when the DAC starts. Set it to `-1` if the enable is already strapped high or the board has no amplifier. A disabled amplifier is a common reason for silent speakers while headphones still work.

Assign every signal in the diagram to your board's GPIOs under `Example Configuration` → `Example IO Configuration` in menuconfig. Some targets also ship extra defaults in `sdkconfig.defaults.<target>`; `idf.py set-target` applies those automatically.

### Dependency

This example is based on [esp_codec_dev](https://components.espressif.com/components/espressif/esp_codec_dev). The component is already listed in `main/idf_component.yml` and is fetched by the [IDF Component Manager](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html) on the first build.

To add the same dependency to another project:

```
idf.py add-dependency "espressif/esp_codec_dev^1.6.0"
```

### Configure the Project

```
idf.py menuconfig
```

In the `Example Configuration` menu:

* Use `DAC / codec type` to select ES8311 or ES8389.
* Use `Example IO Configuration` to assign the shared I2C, I2S, and amplifier-enable GPIOs.
* Use `Voice volume` to set the output volume (0 to 100).

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

## Example Output

```
wav player example start
--------------------------
I (...) wav_player: i2s driver init success
I (...) wav_player: ES8311 codec init success
I (...) wav_player: i2s music played, ... bytes are written.
I (...) wav_player: i2s music played, ... bytes are written.
......
```

An ES8389 run looks the same, except the codec init line names ES8389.

If you have a logic analyzer, probe the I2S signals on the GPIOs you set in `Example IO Configuration`.

## Troubleshooting

* No sound from the speaker or headphone

    * Confirm `DAC / codec type` and the I2C/I2S GPIOs match the board. Playback data goes to the codec **SDI** (SDIN) pin, not SDO.
    * If the board has no MCLK pin (ESP32-S31-Korvo-1 does not), set `I2S MCLK IO` to -1 so the codec derives its clock from BCLK. Driving an unconnected MCLK GPIO while the codec waits for external MCLK produces silence.
    * Check the schematic and set `Power amplifier enable IO` to the speaker-amplifier enable (`EN` / `CTRL` / `PA` / `INT`). Set the option to -1 if that pin is already strapped high.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
