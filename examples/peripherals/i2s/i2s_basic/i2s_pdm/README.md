| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# I2S Basic PDM Mode Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example is going to show how to use the PDM TX and RX mode.

## How to Use Example

### Hardware Required

#### General

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* A USB cable for power supply and programming

#### PDM RX

* For non-ESP32-S3 SoC, a PDM microphone whose `sel` pin is supposed to be pulled down, and connecting its `clk` pin to `EXAMPLE_PDM_RX_CLK_IO`, `data` pin to `EXAMPLE_PDM_RX_DIN_IO`.

```
┌───────────────────────┐               ┌──────────────────┐
│          ESP          │               │   PDM microphone │
│                       │   PDM clock   │                  │
│ EXAMPLE_PDM_RX_CLK_IO ├──────────────►│ CLK              │
│                       │   PDM data    │                  │
│ EXAMPLE_PDM_RX_DIN_IO │◄──────────────┤ DATA             │
│                       │               │                  │
│                       │         ┌─────┤ SEL              │
│                       │         │     │                  │
│                   GND ├─────────┴─────┤ GND              │
│                       │               │                  │
│                   VCC ├───────────────┤ VCC              │
└───────────────────────┘               └──────────────────┘
```

* For ESP32-S3 SoC, this example shows how to support 8 PDM microphones, in this case, the connection can be:

```
┌──────────────────┐                        ┌─────────────────────────┐                        ┌──────────────────┐
│  PDM microphone3 │                        │            ESP          │                        │  PDM microphone1 │
│                  │      PDM clock         │                         │          PDM clock     │                  │
│             CLK  │◄───┬───────────────────┤  EXAMPLE_PDM_RX_CLK_IO  ├───────────────────┬───►│ CLK              │
│                  │    │                   │                         │                   │    │                  │
│             DATA ├────┼────┐              │  EXAMPLE_PDM_RX_DIN_IO  │◄─────────────┬────┼────┤ DATA             │
│                  │    │    │              │                         │              │    │    │                  │
│             SEL  ├────┼────┼─────────┐    │                         │    ┌─────────┼────┼────┤ SEL              │
│                  │    │    │         │    │                         │    │         │    │    │                  │
│             GND  ├────┼────┼────┐    │    │                         │    │    ┌────┼────┼────┤ GND              │
│                  │    │    │    │    │    │                         │    │    │    │    │    │                  │
│             VCC  ├────┼────┼────┼────┤    │                         │    ├────┼────┼────┼────┤ VCC              │
└──────────────────┘    │    │    │    │    │                         │    │    │    │    │    └──────────────────┘
                        │    │    │    │    │                         │    │    │    │    │
┌──────────────────┐    │    │    │    │    │                         │    │    │    │    │    ┌──────────────────┐
│  PDM microphone4 │    │    │    │    │    │                         │    │    │    │    │    │  PDM microphone2 │
│                  │    │    │    │    │    │                         │    │    │    │    │    │                  │
│             CLK  │◄───┤    │    │    │    │                         │    │    │    │    ├───►│ CLK              │
│                  │    │    │    │    │    │                         │    │    │    │    │    │                  │
│             DATA ├────┼────┴────┼────┼───►│ EXAMPLE_PDM_RX_DIN1_IO  │    │    │    └────┼────┤ DATA             │
│                  │    │         │    │    │                         │    │    │         │    │                  │
│             SEL  ├────┼─────────┤    │    │                         │    │    ├─────────┼────┤ SEL              │
│                  │    │         │    │    │                         │    │    │         │    │                  │
│             GND  ├────┼─────────┤    │    │                         │    │    ├─────────┼────┤ GND              │
│                  │    │         │    │    │                         │    │    │         │    │                  │
│             VCC  ├────┼─────────┼────┤    │                         │    ├────┼─────────┼────┤ VCC              │
└──────────────────┘    │         │    │    │                         │    │    │         │    └──────────────────┘
                        │         │    │    │                         │    │    │         │
┌──────────────────┐    │         │    │    │                         │    │    │         │    ┌──────────────────┐
│  PDM microphone7 │    │         │    │    │                         │    │    │         │    │  PDM microphone5 │
│                  │    │         │    │    │                         │    │    │         │    │                  │
│             CLK  │◄───┤         │    │    │                         │    │    │         ├───►│ CLK              │
│                  │    │         │    │    │                         │    │    │         │    │                  │
│             DATA ├────┼────┐    │    │    │ EXAMPLE_PDM_RX_DIN2_IO  │◄───┼────┼────┬────┼────┤ DATA             │
│                  │    │    │    │    │    │                         │    │    │    │    │    │                  │
│             SEL  ├────┼────┼────┼────┤    │                         │    ├────┼────┼────┼────┤ SEL              │
│                  │    │    │    │    │    │                         │    │    │    │    │    │                  │
│             GND  ├────┼────┼────┤    │    │                         │    │    ├────┼────┼────┤ GND              │
│                  │    │    │    │    │    │                         │    │    │    │    │    │                  │
│             VCC  ├────┼────┼────┼────┤    │                         │    ├────┼────┼────┼────┤ VCC              │
└──────────────────┘    │    │    │    │    │                         │    │    │    │    │    └──────────────────┘
                        │    │    │    │    │                         │    │    │    │    │
┌──────────────────┐    │    │    │    │    │                         │    │    │    │    │    ┌──────────────────┐
│  PDM microphone8 │    │    │    │    │    │                         │    │    │    │    │    │  PDM microphone6 │
│                  │    │    │    │    │    │                         │    │    │    │    │    │                  │
│             CLK  │◄───┘    │    │    │    │                         │    │    │    │    └───►│ CLK              │
│                  │         │    │    │    │                         │    │    │    │         │                  │
│             DATA ├─────────┴────┼────┼───►│ EXAMPLE_PDM_RX_DIN3_IO  │    │    │    └─────────┤ DATA             │
│                  │              │    │    │                         │    │    │              │                  │
│             SEL  ├──────────────┤    │    │                         │    │    ├──────────────┤ SEL              │
│                  │              │    │    │                         │    │    │              │                  │
│             GND  ├──────────────┴────┼────┤           GND           ├────┼────┴──────────────┤ GND              │
│                  │                   │    │                         │    │                   │                  │
│             VCC  ├───────────────────┴────┤           VCC           ├────┴───────────────────┤ VCC              │
└──────────────────┘                        └─────────────────────────┘                        └──────────────────┘
```

#### PDM TX

* An earphone or a speaker
* An audio power amplifier that can input PDM signal. If the power amplifier can only receive the analog signal without PDM clock (i.e. DAC line mode, otherwise codec line mode), a low-pass passive or active filter is required to restore the PDM data wave into analog signal, before it is transmitted to the power amplifier.

**MAX98358 (codec case)**

Please refer to the [Datasheet of MAX98358](https://datasheets.maximintegrated.com/en/ds/MAX98358.pdf) for more details.

```
┌────────────────────────┐               ┌───────────────┐
│          ESP           │               │   MAX 98358   │
│                        │   PDM clock   │               │
│  EXAMPLE_PDM_TX_CLK_IO ├──────────────►│ CLK           │   ┌─────────┐
│                        │   PDM data    │               │   │ Speaker │
│ EXAMPLE_PDM_TX_DOUT_IO ├──────────────►│ DATA     OUTP ├───┤         │
│                        │               │               │   │         │
│                        │         ┌─────┤ SD_MODE  OUTN ├───┤         │
│                        │         │     │               │   │         │
│                    VCC ├─────────┴─────┤ VCC           │   └─────────┘
│                        │               │               │
│                    GND ├───────────────┤ GND           │
└────────────────────────┘               └───────────────┘
```

**NS4150 (dac case)**

Please refer to the NS4150 datasheet for more details.

```
┌────────────────────────┐                              ┌───────────────┐
│          ESP           │                              │    NS 4150    │
│                        │                              │               │
│  EXAMPLE_PDM_TX_CLK_IO │(No need to connect)          │ INN           │   ┌─────────┐
│                        │PDM data┌────────────────┐    │               │   │ Speaker │
│ EXAMPLE_PDM_TX_DOUT_IO ├────────┤ Low-pass Filter├───►│ INP       VoP ├───┤         │
│                        │        └────────────────┘    │               │   │         │
│                        │                          ┌───┤ CTRL      VoN ├───┤         │
│                        │                          │   │               │   │         │
│                    VCC ├──────────────────────────┴───┤ VCC           │   └─────────┘
│                        │                              │               │
│                    GND ├──────────────────────────────┤ GND           │
└────────────────────────┘                              └───────────────┘
```

### Configure the Project

PDM can only works in simplex mode, you can select the PDM direction in the menu config, or just setting the macro `EXAMPLE_PDM_DIR` directly. Setting it to `EXAMPLE_PDM_TX` or `EXAMPLE_PDM_RX` can choose the PDM direction of this example. But currently ESP32-C3 does not support PDM RX mode.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

### PDM TX

While `EXAMPLE_PDM_DIR` is set to `EXAMPLE_PDM_TX`, then you can see the following log:

```
I2S PDM TX example start
---------------------------
D (284) i2s_common: tx channel is registered on I2S0 successfully
D (294) i2s_common: DMA malloc info: dma_desc_num = 6, dma_desc_buf_size = dma_frame_num * slot_num * data_bit_width = 500
D (304) i2s_pdm: Clock division info: [sclk] 160000000 Hz [mdiv] 3 [mclk] 49152000 Hz [bdiv] 8 [bclk] 6144000 Hz
D (314) i2s_pdm: The tx channel on I2S0 has been initialized to PDM TX mode successfully
D (324) i2s_common: i2s tx channel enabled
Playing bass `twinkle twinkle little star`
Playing alto `twinkle twinkle little star`
Playing treble `twinkle twinkle little star`
...
```

You can hear the audio 'twinkle twinkle little star' in three tones if you connect a speaker to it.

### PDM RX

While `EXAMPLE_PDM_DIR` is set to `EXAMPLE_PDM_RX`, but without connecting a PDM microphone, then you can see the following log:

```
I2S PDM RX example start
---------------------------
D (10) i2s_common: rx channel is registered on I2S0 successfully
D (10) i2s_common: DMA malloc info: dma_desc_num = 6, dma_desc_buf_size = dma_frame_num * slot_num * data_bit_width = 500
D (20) i2s_common: i2s rx channel enabled
Read Task: i2s read 2048 bytes
-----------------------------------
[0] -6595 [1] 0 [2] -29199 [3] -32768
[4] -30203 [5] -32156 [6] -30704 [7] -31348

Read Task: i2s read 2048 bytes
-----------------------------------
[0] -30935 [1] -30935 [2] -30935 [3] -30935
[4] -30935 [5] -30935 [6] -30935 [7] -30935

Read Task: i2s read 2048 bytes
-----------------------------------
[0] -30935 [1] -30935 [2] -30935 [3] -30935
[4] -30935 [5] -30935 [6] -30935 [7] -30935
```

And only if you connect a PDM microphone, you can see the data changes:

```
I2S PDM RX example start
---------------------------
D (10) i2s_common: rx channel is registered on I2S0 successfully
D (10) i2s_common: DMA malloc info: dma_desc_num = 6, dma_desc_buf_size = dma_frame_num * slot_num * data_bit_width = 500
D (20) i2s_common: i2s rx channel enabled
Read Task: i2s read 2048 bytes
-----------------------------------
[0] -3181 [1] 0 [2] -7194 [3] -24288
[4] -777 [5] 3650 [6] 109 [7] 571

Read Task: i2s read 2048 bytes
-----------------------------------
[0] 2391 [1] 2378 [2] 2397 [3] 2385
[4] 2399 [5] 2375 [6] 2401 [7] 2389
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
