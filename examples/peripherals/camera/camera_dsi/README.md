| Supported Targets | ESP32-P4 |
| ----------------- | -------- |


# Camera display via DSI example

## Overview

This example demonstrates how to use the esp_driver_cam component to capture camera sensor signals and display it via DSI interface. This example will auto-detect camera sensors via [ESP camera sensor driver](https://components.espressif.com/components/espressif/esp_cam_sensor/versions/0.5.3) and capture camera sensor signals via CSI interface and display it via DSI interface.

## Usage

The subsections below give only absolutely necessary information. For full steps to configure ESP-IDF and use it to build and run projects, see [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started).


### Hardware Required

- OV5647 or SC2336 camera sensor, or other camera sensors
- EK79007 or ILI9881C LCD screen
- ESP32P4 devkit

**Note:** OV5647 has its own ISP functions, whereas SC2336 is a camera sensor without ISP functions. You can use the ESP on-chip ISP functions to tune the image together with the sensor ISP functions, if the image quality is not as expected.

**Note:** For EK79007 you will need to connect following pins:
- 5V - 5V
- GND - GND
- RST_LCD - 3V3

You can also connect camera sensors and LCD screens from other vendors to the ESP chip, you can find corresponding camera or LCD drivers from [ESP Component Registry](https://components.espressif.com), or design your own customized drivers.


                                   GND                                                                   GND
                ┌────────────────────────────────────────────────┐             ┌─────────────────────────────────────────────────────────┐
                │                                                │             │                                                         │
                │                                                │             │                                                         │
                │                                                │             │                                                         │
                │                                                │             │                                                         │
                │                                                │             │                                                         │
                │                                                │             │                                                         │
                │                                                │             │                                                         │
                │                                                │             │                                                         │
                │                                ┌───────────────┴─────────────┴──────────────────┐                                      │
                │                                │                                                │                           ┌──────────┴───────────┐
                │                                │                                                │      DSI DATA 1P          │                      │
                │                                │                                                ├───────────────────────────┤                      │
    ┌───────────┴─────────┐ CSI DATA 1P          │                                                │                           │                      │
    │                     ├──────────────────────┤                                                │      DSI DATA 1N          │                      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │ CSI DATA 1N          │                  ESP32-P4                      │                           │                      │
    │       Camera        ├──────────────────────┤                                                │      DSI CLK N            │      LCD Screen      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │ CSI CLK N            │                                                │                           │                      │
    │                     ├──────────────────────┤                                                │      DSI CLK P            │                      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │ CSI CLK P            │                                                │                           │                      │
    │                     ├──────────────────────┤                                                │      DSI DATA 0P          │                      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │ CSI DATA 0P          │                                                │                           │                      │
    │                     ├──────────────────────┤                                                │      DSI DATA 0N          │                      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │ CSI DATA 0N          │                                                │                           │                      │
    │                     ├──────────────────────┤                                                │                           └──────────────────────┘
    │                     │                      │                                                │
    └───────┬──┬──────────┘                      │                                                │
            │  │           I2C SCL               │                                                │
            │  └─────────────────────────────────┤                                                │
            │              I2C SDA               │                                                │
            └────────────────────────────────────┤                                                │
                                                 └────────────────────────────────────────────────┘


### Set Chip Target

First of all, your target must be supported by both:

- **By your ESP-IDF version**: For the full list of supported targets, run:
  ```
  idf.py --list-targets
  ```
- **By this example**: For the full list of supported targets,  refer to the supported targets table at the top of this README.

After you make sure that your target is supported, go to your example project directory and [set the chip target](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/tools/idf-py.html#select-the-target-chip-set-target):

```
idf.py set-target <target>
```

For example, to set esp32-P4 as the chip target, run:

```
idf.py set-target esp32p4
```


### Configure the Project

For information about Kconfig options, see [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html) > _Name of relevant section(s)_.

To conveniently check or modify Kconfig options for this example in a project configuration menu, run:

```
idf.py menuconfig
```

```
Set CONFIG_CAMERA_OV5647 to y
Set CONFIG_CAMERA_SC2336 to y
```


### Build and Flash

Execute the following command to build the project, flash it to your development board, and run the monitor tool to view the serial output:

```
idf.py build flash monitor
```

This command can be reduced to `idf.py flash monitor`.

If the above command fails, check the log on the serial monitor which usually provides information on the possible cause of the issue.

To exit the serial monitor, use `Ctrl` + `]`.


## Example Output

If you see the following console output, your example should be running correctly:

```
I (1085) main_task: Calling app_main()
I (1095) ili9881c: ID1: 0x98, ID2: 0x81, ID3: 0x5c
I (1125) gpio: GPIO[31]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (1125) gpio: GPIO[34]| InputEn: 1| OutputEn: 1| OpenDrain: 1| Pullup: 1| Pulldown: 0| Intr:0
I (1295) ov5647: Detected Camera sensor PID=0x5647 with index 0
I (1305) cam_dsi: fmt[0].name:MIPI_2lane_24Minput_RAW8_800x1280_50fps
I (1305) cam_dsi: fmt[1].name:MIPI_2lane_24Minput_RAW8_800x640_50fps
I (1315) cam_dsi: fmt[2].name:MIPI_2lane_24Minput_RAW8_800x800_50fps
I (1355) cam_dsi: Format in use:MIPI_2lane_24Minput_RAW8_800x640_50fps
```


## Reference

- Link to the ESP-IDF feature's API reference, for example [ESP-IDF: Camera Controller Driver](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/camera_driver.html)
- [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started)
- [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html) (Kconfig Options)
