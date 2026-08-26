| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# I2S Microphone Recording Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example captures 16-bit stereo PCM audio from a microphone and streams it over the console back to the host PC, where it can be reconstructed as a ``.wav`` file. It supports two kinds of microphones, selectable in menuconfig:

* **Digital microphone (PDM)** — one or two PDM digital MEMS microphones sampled through the I2S peripheral's hardware PDM-to-PCM conversion.
* **Analog microphone with ES8389 codec** — the ES8389 ADC samples the analog microphone, then transmits PCM data to the SoC over I2S STD mode. The example configures the codec through the [esp_codec_dev](https://components.espressif.com/components/espressif/esp_codec_dev) component.

The device records the PCM data in RAM, stops the I2S/codec, and prints the audio as chunked Base64 over the console. No SD card or filesystem is used. The accompanying ``pytest`` test reconstructs the WAVE file on the host PC.

| Audio Setting | Value |
|:---:|:---:|
| Sample Rate |16000 Hz|
| Bits per Sample |16 bits|
| Channels |Stereo|
| Recording Time |2 seconds|

These settings are defined as macros at the top of `main/mic_recorder_example_main.c` (see `EXAMPLE_SAMPLE_RATE`, `EXAMPLE_RECORD_TIME_SECONDS`, etc.). The sample rate is shared by both the digital and the analog path.

## How to Use Example

### Hardware Required

* A development board with a SoC that supports the chosen microphone type:
  * Digital microphone: hardware PDM-to-PCM conversion (ESP32, ESP32-S3, ESP32-P4, ESP32-S31, etc.).
  * Analog microphone: I2S STD mode plus I2C to drive the ES8389 codec (for example the ESP32-S31-Korvo-1 board).
* A USB cable for power supply and programming.
* The microphone(s) matching the selected type.

For stereo PDM capture, two microphones share one data line. Connect the left microphone's `select` pin to GND and the right microphone's `select` pin to VDD. Configure every GPIO according to your board schematic.

### Configure the Project

```
idf.py menuconfig
```

In the `Example Configuration` menu:

* Use `Microphone type` to select between the digital (PDM) and the analog (ES8389) microphone.
* Use `Startup data to discard (ms)` to drop the first N milliseconds of PCM after the microphone path is running. The default is 20 ms. Set it to 0 to keep every sample. Refer to the microphone datasheet for the startup delay (also called turn-on time).
* The configuration menu shown below the mic type is updated automatically:
  * `PDM MIC Configuration` — assign the PDM clock and data GPIOs (digital microphone).
  * `ES8389 Codec Configuration` — assign the I2C and I2S GPIOs, plus the mic gain (analog microphone).

The recording duration is defined by `EXAMPLE_RECORD_TIME_SECONDS` in `main/mic_recorder_example_main.c`. The default 2-second stereo recording occupies 128 KB of RAM. Increasing the recording duration or sample rate increases RAM use proportionally.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)

## Console Output and WAVE Artifact

After recording completes, the device emits an `AUDIO_META` line followed by Base64 chunks bracketed by `AUDIO_BASE64_BEGIN` and `AUDIO_BASE64_END`. The chunks are intentionally short so that the serial stream can be parsed reliably.

A digital-microphone run looks like:

```
PDM MIC recording example start
--------------------------------------
I (...) mic_rec_example: Starting PDM recording for 2 seconds!
I (...) mic_rec_example: Recording done, sending PCM data over console
AUDIO_META sample_rate=16000 bits_per_sample=16 channels=2 data_size=128000 encoding=base64
AUDIO_BASE64_BEGIN
AUDIO_BASE64 <Base64-encoded PCM chunk>
...
AUDIO_BASE64_END
mic recorder example done
```

An analog-microphone run looks the same, except the start line reads `ES8389 MIC recording example start`.

At the default 115200 baud console speed, sending the complete Base64 payload takes roughly 15--20 seconds for the default recording. This happens only after the I2S/codec has stopped, so console throughput cannot cause microphone DMA overflows.

### Getting the Recorded Audio on the Host PC

The Base64 payload printed over the console carries the full PCM recording.

### Run pytest and Save a WAVE File

With the board connected, run the test for the microphone type from the example directory:

Digital PDM microphone:

```
pytest --target esp32s3 --port /dev/ttyUSB0 pytest_mic_record.py::test_mic_recorder_dmic_example
```

Analog ES8389 microphone on ESP32-S31-Korvo-1:

```
pytest --target esp32s31 --port /dev/ttyUSB0 pytest_mic_record.py::test_mic_recorder_amic_example
```

Replace `/dev/ttyUSB0` with your board's serial port. Each test selects its matching `sdkconfig.ci.*` configuration, decodes the serial output, and writes the recording to:

```
<logdir>/mic_recording.wav
```

(`<logdir>` is printed at the start of the pytest run; it is also reported by the ``Saved microphone recording artifact to ...`` log line.) The test both validates the console protocol and saves the captured microphone data as a WAVE artifact.

The resulting `mic_recording.wav` is 16-bit PCM: 16000 Hz sample rate, 2 channels, ~2 seconds long.
Open it with [Audacity](https://www.audacityteam.org/) to listen to the recording or inspect its waveform.

## Troubleshooting

* Analog microphone produces silence or a very low signal

    * Check the I2C wiring (SDA/SCL) and that `ES8389 Codec Configuration` matches your board.
    * Increase `Mic gain (dB)` in the `Example Configuration` menu.

* Recording starts with a pop, click, or a short stretch of invalid audio

    * Increase `Startup data to discard (ms)` in the `Example Configuration` menu. The stored recording length stays the same.
    * Refer to the microphone datasheet for the startup delay (also called turn-on time).

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
