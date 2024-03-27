| Supported Targets | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- |

# I2S TDM Example -- ES7210 4-Ch ADC Codec

(See the README.md file in the upper level 'examples' directory for more information about examples.)

I2S on `ESP32S3` and `ESP32C3` supports [TDM mode](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/i2s.html#tdm-mode), in which multiple slots can be transmitted by standard I2S connection.

This example demonstrates how to use I2S TDM mode to record 4 MICs connected to [ES7210](http://www.everest-semi.com/pdf/ES7210%20PB.pdf) codec. ES7210 has 4 TDM modes, which are `ES7210_I2S_FMT_I2S` `ES7210_I2S_FMT_LJ` `ES7210_I2S_FMT_DSP_A` and `ES7210_I2S_FMT_DSP_B`, and they are all supported by I2S TDM driver. Relation between ES7210 TDM modes and I2S Driver TDM modes is shown in the following table.

|  Mode of ES7210 TDM  | Mode of I2S Driver TDM |
| :------------------: | :--------------------: |
|  ES7210_I2S_FMT_I2S  |     Philips format     |
|  ES7210_I2S_FMT_LJ   |       MSB format       |
| ES7210_I2S_FMT_DSP_A |    PCM short format    |
| ES7210_I2S_FMT_DSP_B |    PCM long format     |

Recorded voice will be saved to SD card in `wav` format, and can be played or processed on PC.

## How to Use Example

### Hardware Required

* A development board with any supported Espressif SOC chip (see `Supported Targets` table above)
* An ES7210 module with 4 MICs connected
* A SPI SD card module
* A SD card reader and a PC (if you want to play recorded voice)

All the GPIO used in this example can be changed according to your board, by macros `EXAMPLE_xxx_IO` defined at the beginning of [i2s_es7210_record_example.c](main/i2s_es7210_record_example.c).

### Dependency

This example is based on [es7210 component](https://components.espressif.com/component/espressif/es7210)

The component can be installed by esp component manager. Since this example already installed it, no need to re-installed it again, but if you want to install this component in your own project, you can input the following command:
```
idf.py add-dependency espressif/es7210^1.0.0
```

If the dependency is added, you can check `idf_component.yml` for more detail. When building this example or other projects with managed components, the component manager will search for the required components online and download them into the `managed_components` folder.

### Configure the project

* Set the target of the build by following command, where TARGET can be `esp32s3` or `esp32c3`.
```
idf.py set-target TARGET
```
* Change value of `EXAMPLE_I2S_FORMAT` to check I2S driver's functionality on different I2S formats.
* Change `EXAMPLE_ES7210_MIC_GAIN` and `EXAMPLE_ES7210_MIC_BIAS` according your MIC's specs if needed.
* Change `EXAMPLE_ES7210_ADC_VOLUME` if recorded voice is too loud or too quite.

Note: it's better to adjust `EXAMPLE_ES7210_MIC_GAIN` first. If adjusting MIC gain doesn't meet your demand, you can then adjust `EXAMPLE_ES7210_ADC_VOLUME`. That is to say, it's better to adjust analog gain than digital gain.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Console Output

```
I (277) cpu_start: Starting scheduler.
I (281) example: Create I2S receive channel
I (281) example: Configure I2S receive channel to TDM mode
I (291) example: Init I2C used to configure ES7210
I (291) example: Configure ES7210 codec parameters
I (301) ES7210: format: standard i2s, bit width: 16, tdm mode enabled
I (311) ES7210: sample rate: 48000Hz, mclk frequency: 12288000Hz
I (311) example: Initializing SPI bus for SD card
I (321) example: Mounting SD card
I (321) gpio: GPIO[4]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (371) sdspi_transaction: cmd=5, R1 response: command not supported
I (381) example: Card size: 14772MB, speed: 20MHz
I (381) example: Opening file /RECORD.WAV
I (391) example: Recording: 1/10s
I (1401) example: Recording: 2/10s
I (2411) example: Recording: 3/10s
I (3401) example: Recording: 4/10s
I (4411) example: Recording: 5/10s
I (5411) example: Recording: 6/10s
I (6421) example: Recording: 7/10s
I (7411) example: Recording: 8/10s
I (8411) example: Recording: 9/10s
I (9401) example: Recording: 10/10s
I (10401) example: Recording done! Flushing file buffer
I (10431) gpio: GPIO[4]| InputEn: 1| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (10431) example: You can now safely remove the card, recorded file is /RECORD.WAV
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
