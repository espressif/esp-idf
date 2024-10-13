| Supported Targets | ESP32-P4 |
| ----------------- | -------- |


# PPA DSI example

## Overview

This example demonstrates how to use the esp_driver_ppa component to process a given image and display it via DSI interface.

The example used image will be first scaled up, rotated at counter-clockwise direction and rotated back, mirrored and mirror back, and scaled down. Then the image will be blended with a whole red image with less transparency. Next the `ESP32` word will be color-keyed out. Lastly a frame will be filled around the `ESP32`.

## Usage

The subsections below give only absolutely necessary information. For full steps to configure ESP-IDF and use it to build and run projects, see [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started).


### Hardware Required

This example requires:

- EK79007 or ILI9881C LCD screen
- ESP32P4 devkit

**Note:** For EK79007 you will need to connect following pins:
- 5V - 5V
- GND - GND
- RST_LCD - 3V3


                                                                  GND
                                        ┌─────────────────────────────────────────────────────────┐
                                        │                                                         │
                                        │                                                         │
                                        │                                                         │
                                        │                                                         │
                                        │                                                         │
                                        │                                                         │
                                        │                                                         │
                                        │                                                         │
          ┌───────────────-─────────────┴──────────────────┐                                      │
          │                                                │                           ┌──────────┴───────────┐
          │                                                │      DSI DATA 1P          │                      │
          │                                                ├───────────────────────────┤                      │
          │                                                │                           │                      │
          │                                                │      DSI DATA 1N          │                      │
          │                                                ├───────────────────────────┤                      │
          │                  ESP32-P4                      │                           │                      │
          │                                                │      DSI CLK N            │      LCD Screen      │
          │                                                ├───────────────────────────┤                      │
          │                                                │                           │                      │
          │                                                │      DSI CLK P            │                      │
          │                                                ├───────────────────────────┤                      │
          │                                                │                           │                      │
          │                                                │      DSI DATA 0P          │                      │
          │                                                ├───────────────────────────┤                      │
          │                                                │                           │                      │
          │                                                │      DSI DATA 0N          │                      │
          │                                                ├───────────────────────────┤                      │
          │                                                │                           │                      │
          │                                                │                           └──────────────────────┘
          │                                                │
          │                                                │
          │                                                │
          │                                                │
          │                                                │
          │                                                │
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
I (1555) main_task: Calling app_main()
I (1555) ili9881c: ID1: 0x98, ID2: 0x81, ID3: 0x5c
I (1795) ppa_dsi: JPEG image decoded! Size of the decoded image is: 320px x 240px
start srm operations
start blend operations
start fill operations
I (10085) main_task: Returned from app_main()
```


## Reference

- Link to the ESP-IDF feature's API reference, for example [ESP-IDF: Pixel-Processing Accelerator](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/peripherals/ppa.html)
- [ESP-IDF Getting Started](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started)
- [Project Configuration](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/kconfig.html) (Kconfig Options)
