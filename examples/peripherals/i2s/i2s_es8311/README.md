# I2S ES8311 Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

In this example, you can choose music mode or echo mode in menuconfig. In music mode, the example will play a piece of music in 'canon.pcm', and in echo mode, you can hear what you said in earphone.

## ES8311 brief

ES8311 low power mono audio codec features:

- High performance and low power multi-bit delta-sigma audio ADC and DAC
- I2S/PCM master or slave serial data port
- I2C interface for configuration
- ADC: 24-bit, 8 to 96 kHz sampling frequency
- ADC: 100 dB signal to noise ratio, -93 dB THD+N
- DAC: 24-bit, 8 to 96 kHz sampling frequency
- DAC: 110 dB signal to noise ratio, -80 dB THD+N

For more details, see [ES8311 datasheet](http://www.everest-semi.com/pdf/ES8311%20PB.pdf)

## How to Use Example

### Hardware Required

* An ESP development board that support I2S.
* A USB cable for power supply and programming.
* A board with ES8311 codec, mic and earphone interface(e.g. ESP-LyraT-8311A extension board).

### Connection
```
┌─────────────────┐           ┌──────────────────────────┐
│       ESP       │           │          ES8311          │
│                 │           │                          │
│     MCLK-GPIO 0 ├──────────►│PIN2-MCLK                 │
│                 │           │                          │           ┌─────────┐
│     BCLK-GPIO 4 ├──────────►│PIN6-BCLK       PIN12-OUTP├───────────┤         │
│                 │           │                          │           │ EARPHONE│
│       WS-GPIO 5 ├──────────►│PIN8-LRCK       PIN13-OUTN├───────────┤         │
│                 │           │                          │           └─────────┘
│    SDOUT-GPIO 18├──────────►│PIN9-SDIN                 │
│                 │           │                          │
│     SDIN-GPIO 19│◄──────────┤PIN7-SDOUT                │
│                 │           │                          │           ┌─────────┐
│                 │           │               PIN18-MIC1P├───────────┤         │
│      SCL-GPIO 16├──────────►│PIN1 -CCLK                │           │  MIC    │
│         (GPIO 7)│           │               PIN17-MIC1N├───────────┤         │
│      SDA-GPIO 17│◄─────────►│PIN19-CDATA               │           └─────────┘
│         (GPIO 8)│           │                          │
│          VCC 3.3├───────────┤VCC                       │
│                 │           │                          │
│              GND├───────────┤GND                       │
└─────────────────┘           └──────────────────────────┘
```
Note: Since ESP32-C3 board does not have GPIO 16/17, you can use other available GPIOs instead. In this example, we set GPIO 7/8 as I2C pins for ESP32-C3 and GPIO 16/17 for other chips.

### Dependency

This example is based on [es8311 component](https://components.espressif.com/component/espressif/es8311)

The component can be installed by esp component manager. Since this example already installed it, no need to re-installed it again, but if you want to install this component in your own project, you can input the following command:
```
idf.py add-dependency espressif/es8311==0.0.2-alpha
```

If dependency is added, you can check `idf_component.yml` for more detail. When building this example or other project with managed-component in it, the component manager will search the componet online and download it under `managed_componets` folder.

### Configure the Project

```
idf.py menuconfig
```
You can find configurations for this example in 'Example Configutation' tag.

* In 'Example mode' subtag, you can set the example mode to 'music' or 'echo'. You can hear a piece of music in 'music' mode and echo the sound sampled by mic in 'echo' mode. You can also customize you own music to play as shown below.

* In 'Set MIC gain' subtag, you can set the mic gain for echo mode.

* In 'Voice volume', you can set the volum between 0 to 100.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Running this example in music mode, you can hear a piece of music (canon), the log is shown as follow:

```
I (348) I2S: DMA Malloc info, datalen=blocksize=1200, dma_buf_count=6
I (348) I2S: DMA Malloc info, datalen=blocksize=1200, dma_buf_count=6
I (358) I2S: I2S0, MCLK output by GPIO0
I (368) DRV8311: ES8311 in Slave mode
I (378) gpio: GPIO[10]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (3718) i2s_es8311: I2S music played, 213996 bytes are written.
I (7948) i2s_es8311: I2S music played, 213996 bytes are written.
......
```

Running this example in echo mode, you can hear the sound in earphone that collected by mic.
```
I (312) I2S: DMA Malloc info, datalen=blocksize=1200, dma_buf_count=6
I (312) I2S: DMA Malloc info, datalen=blocksize=1200, dma_buf_count=6
I (322) I2S: I2S0, MCLK output by GPIO0
I (332) DRV8311: ES8311 in Slave mode
I (342) gpio: GPIO[10]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
```

If you have a logic analyzer, you can use a logic analyzer to grab GPIO signal directly. The following table describes the pins we use by default (Note that you can also use other pins for the same purpose).

| pin name| function | gpio_num |
|:---:|:---:|:---:|
| MCLK  |module clock   | GPIO_NUM_0|
| BCLK  |bit clock      | GPIO_NUM_4 |
| WS    |word select    | GPIO_NUM_5 |
| SDOUT |serial data out| GPIO_NUM_18 |
| SDIN  |serial data in | GPIO_NUM_19 |

### Customize your own music

The example have contained a piece of music in canon.pcm, if you want to play your own music, you can follow these steps:

1. Choose the music in any format you want to play (e.g. a.mp3)
2. Install 'ffmpeg' tool
3. Check your music format using ```ffprobe a.mp3```, you can get the stream format (e.g. Stream #0.0: Audio: mp3, 44100Hz, stereo, s16p, 64kb/s)
4. Cut your music since there is no enough space for the whole piece of music. ```ffmpeg -i  a.mp3 -ss 00:00:00  -t  00:00:20  a_cut.mp3```
5. Transfer the music format into .pcm. ```ffmpeg -i a_cut.mp3 -f s16ls -ar 16000 -ac -1 -acodec pcm_s16le a.pcm```
6. Move 'a.pcm' under 'main' directory
7. Replace 'canon.pcm' with 'a.pcm' in 'CMakeLists.txt' under 'main' directory
8. Replace '_binary_canon_pcm_start' and '_binary_canon_pcm_end' with '_binary_a_pcm_start' and '_binary_a_pcm_end' in `i2s_es9311_example.c`(line 46/47)
9. Download the example and enjoy your own music

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
