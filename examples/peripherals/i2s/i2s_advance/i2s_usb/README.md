| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# I2S USB Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use I2S and USB to make ESP chip as a USB speaker and microphone. The USB on ESP chip will be initialized as a UAC(USB Audio Class) device to receive and transmit the audio data.

This example also demonstrates how to use I2S rate tuning feature to adjust the audio rate dynamically to match the UAC speed. The example only shows a possible approach to match UAC speed, you can specify your own strategy in you actual project.

NOTE: Please take care that the dynamic rate tuning might cause plosive sounds while changing the rate. Suggest to stop the audio first before tuning the rate if possible.

## How to Use Example

### Hardware Required

* ESP chip that has USB support
* A USB cable for connection
* A board with ES8311 codec and power amplifier
* A speaker

### Connection

The pin assignment can be specified in the menuconfig, or the target specific default sdkconfig in `sdkconfig.defaults.<target>`.

The example hardware connection can be referred as follow:

```
                          ┌──────────────┐         ┌─────────────────┐     ┌───────────┐
┌──────────────┐          │      ESP     │         │    ES8311       │     │    MIC    │
│              │ USB Cable│              │ I2C Bus │                 │     │           │
│     PC       ├──────────┤ USB_D+   SCL ├────────►│ CCLK     MIC_1P │◄────┤ OUT+      │
│              ├──────────┤ USB_D-   SDA │◄───────►│ CDATA    MIC_1N │◄────┤ OUT-      │
│              │          │              │         │                 │     │           │
└──────────────┘          │              │         │                 │     └───────────┘
                          │              │ I2S Bus │                 │     ┌───────────┐     ┌─────────┐
                          │         MCLK ├────────►│ MCLK            │     │    PA     │     │ SPEAKER │
                          │         BCLK ├────────►│ SCLK            │     │           │     │         │
                          │           WS ├────────►│ LRCK      OUT_P ├────►│ IN+  OUT+ ├────►│         │
                          │         SDIN │◄────────┤ ASDOUT    OUT_N ├────►│ IN-  OUT- ├────►│         │
                          │        SDOUT ├────────►│ DSDIN           │     │           │     │         │
                          │              │         └─────────────────┘     │           │     │         │
                          │              │ PA Enable IO                    │           │     └─────────┘
                          │      PA_CTRL ├────────────────────────────────►│ PA_EN     │
                          └──────────────┘                                 └───────────┘
```

### Dependency

This example is based on [es8311 component](https://components.espressif.com/component/espressif/es8311) and [usb_device_uac component](https://components.espressif.com/component/espressif/usb_device_uac).

The component can be installed by esp component manager. Since this example already installed it, no need to re-installed it again, but if you want to install this component in your own project, you can input the following command:
```
idf.py add-dependency espressif/esp_codec_dev: ^1.3.4 espressif/usb_device_uac^1.1.0
```

If the dependency is added, you can check `idf_component.yml` for more detail. When building this example or other projects with managed components, the component manager will search for the required components online and download them into the `managed_components` folder.

### Configure The Project

1. Set the target of the build by following command, where TARGET can be the supported targets listed at the top of this file.
```
idf.py set-target TARGET
```
2. Following configurations can be specified in menuconfig:
```
idf.py menuconfig
```
* `Example Configuration` menu:
    - I2C, I2S and PA_CTRL GPIOs
    - Audio data bit width
    - Microphone gain
    - Initial voice volume
    - Whether to enable the dynamic rate tuning
* `Component config > USB Device UAC Configuration > USB Device UAC` menu:
    - UAC sample rate
    - Speaker and microphone channel number
    - Speaker and microphone intervals
    - Speaker and microphone tasks

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Console Output

If you enabled the dynamic rate tuning, the example will force to deviate the MCLK initially, so that to show what will happen if the I2S speed is slower than UAC.

You might see the following output, the buffer water mark increases (the data pending to be sent was accumulated), then the MCLK will be increased if the water mark keeps above the threshold for a while:

```
initial MCLK: 4096000 Hz
I (399) ES8311: ES8311 in Slave mode and I2S format
I (407) gpio: GPIO[6]| InputEn: 0| OutputEn: 1| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (408) usbd_uac: UAC Device Start, Version: 1.1.0
I (409) i2s_usb: MCLK: 4095200, delta: -800, buffer water mark: 0%
I (915) i2s_usb: MCLK: 4095200, delta: -800, buffer water mark: 0%
...
I (6915) i2s_usb: MCLK: 4095200, delta: -800, buffer water mark: 20%
...
I (15415) i2s_usb: MCLK: 4095200, delta: -800, buffer water mark: 32%
...
I (143415) i2s_usb: MCLK: 4095200, delta: -800, buffer water mark: 95%
I (143915) i2s_usb: MCLK: 4095200, delta: -800, buffer water mark: 95%
I (144415) i2s_usb: MCLK: 4095200, delta: -800, buffer water mark: 95%
D (144730) i2s_common: i2s tx channel disabled
D (144730) i2s_common: i2s tx channel enabled
overflow detected, current MCLK: 4095238 Hz, -762 Hz changed
E (144733) i2s_usb: i2s write failed
I (144915) i2s_usb: MCLK: 4095238, delta: -762, buffer water mark: 97%
I (145415) i2s_usb: MCLK: 4095238, delta: -762, buffer water mark: 97%
I (145915) i2s_usb: MCLK: 4095238, delta: -762, buffer water mark: 97%
I (146415) i2s_usb: MCLK: 4095238, delta: -762, buffer water mark: 97%
D (146773) i2s_common: i2s tx channel disabled
D (146773) i2s_common: i2s tx channel enabled
overflow detected, current MCLK: 4095277 Hz, -723 Hz changed
E (146776) i2s_usb: i2s write failed
I (146915) i2s_usb: MCLK: 4095277, delta: -723, buffer water mark: 90%
I (147415) i2s_usb: MCLK: 4095277, delta: -723, buffer water mark: 90%
I (147915) i2s_usb: MCLK: 4095277, delta: -723, buffer water mark: 90%
I (148415) i2s_usb: MCLK: 4095277, delta: -723, buffer water mark: 90%
D (148912) i2s_common: i2s tx channel disabled
D (148912) i2s_common: i2s tx channel enabled
overflow detected, current MCLK: 4095322 Hz, -678 Hz changed
I (148915) i2s_usb: MCLK: 4095322, delta: -678, buffer water mark: 87%
E (148915) i2s_usb: i2s write failed
I (149424) i2s_usb: MCLK: 4095322, delta: -678, buffer water mark: 72%
```

(Once failed to write, it means the buffer is overflowed, you will see the overflow error log about the write failure.)

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
