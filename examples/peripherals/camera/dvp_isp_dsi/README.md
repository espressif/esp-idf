| Supported Targets | ESP32-P4 |
| ----------------- | -------- |


# DVP Camera display via DSI example

## Overview

This example demonstrates how to use the esp_driver_cam component to capture DVP camera sensor signals and display it via DSI interface. This example will auto-detect camera sensors via [ESP camera sensor driver](https://components.espressif.com/components/espressif/esp_cam_sensor) and capture camera sensor signals via DVP interface and display it via DSI interface.

## Usage

The subsections below give only absolutely necessary information. For full steps to configure ESP-IDF and use it to build and run projects, see [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started).


### Hardware Required

- OV2640 / SC030IOT camera sensor, or other camera sensors with DVP port that can output raw format color data
- EK79007 or ILI9881C LCD screen
- ESP32P4 devkit

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
    ┌───────────┴─────────┐                      │                                                │                           │                      │
    │                     │                      │                                                │      DSI DATA 1N          │                      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │       XCLK           │                  ESP32-P4                      │                           │                      │
    │     DVP Camera      ├──────────────────────┤                                                │      DSI CLK N            │      LCD Screen      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │       D0~7           │                                                │                           │                      │
    │                     ├──────────────────────┤                                                │      DSI CLK P            │                      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │       PCLK           │                                                │                           │                      │
    │                     ├──────────────────────┤                                                │      DSI DATA 0P          │                      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │       VSYNC          │                                                │                           │                      │
    │                     ├──────────────────────┤                                                │      DSI DATA 0N          │                      │
    │                     │                      │                                                ├───────────────────────────┤                      │
    │                     │      DE (HREF)       │                                                │                           │                      │
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
Set CONFIG_CAMERA_OV2640 to y
```

Remember to select the LCD screen model and set corresponding correct horizontal/vertical resolution in ``menuconfig`` > ``Example DSI Configuration``.

Available options for the camera sensor output horizontal/vertical resolution can be seen in ``menuconfig`` > ``Example Configuration``.


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
I (1509) main_task: Calling app_main()
I (1509) ek79007: version: 1.0.1
I (1549) ov2640: Detected Camera sensor PID=0x26
I (1549) sensor_init: fmt[0].name:DVP_8bit_20Minput_RGB565_640x480_6fps
I (1549) sensor_init: fmt[1].name:DVP_8bit_20Minput_YUV422_640x480_6fps
I (1549) sensor_init: fmt[2].name:DVP_8bit_20Minput_JPEG_640x480_25fps
I (1559) sensor_init: fmt[3].name:DVP_8bit_20Minput_RGB565_240x240_25fps
I (1569) sensor_init: fmt[4].name:DVP_8bit_20Minput_YUV422_240x240_25fps
I (1569) sensor_init: fmt[5].name:DVP_8bit_20Minput_JPEG_320x240_50fps
I (1579) sensor_init: fmt[6].name:DVP_8bit_20Minput_JPEG_1280x720_12fps
I (1589) sensor_init: fmt[7].name:DVP_8bit_20Minput_JPEG_1600x1200_12fps
I (1589) sensor_init: fmt[8].name:DVP_8bit_20Minput_RAW8_800x640_15fps
I (1599) sensor_init: fmt[9].name:DVP_8bit_20Minput_RAW8_800x800_15fps
I (1609) sensor_init: fmt[10].name:DVP_8bit_20Minput_RAW8_1024x600_15fps
I (2609) sensor_init: Format in use:DVP_8bit_20Minput_RAW8_1024x600_15fps
```


## Reference

- Link to the ESP-IDF feature's API reference, for example [ESP-IDF: Camera Controller Driver](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/camera_driver.html)
- [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started)
- [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html) (Kconfig Options)
