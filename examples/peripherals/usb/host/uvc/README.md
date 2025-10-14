| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# USB Camera Example

## Overview

This example shows basic usage of [USB UVC host driver](https://components.espressif.com/components/espressif/usb_host_uvc). The example waits for USB camera connection, it then configures the camera and starts streaming video frames until the device is removed.

More advanced examples can be found in [UVC host driver repository](https://github.com/espressif/esp-usb/tree/master/host/class/uvc/usb_host_uvc/examples).

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to Use Example

### Hardware Required

* Development board with USB-OTG support and external PSRAM
* Exposed USB host connector
* USB camera

Running this example on an **ESP module without external PSRAM will fail on initialization**. Please select your PSRAM configuration in menuconfig `Component config->ESP PSRAM`. If you manually disable PSRAM, the required frame buffers might not fit into DRAM (especially on ESP32-S2). If you absolutely must not use PSRAM, consider the following for minimizing RAM usage:
* Select smaller resolution (eg. 320x240)
* Select fewer frame buffers (eg. 2)
* Select smaller and fewer URBs (eg. 2x 2kB)

Follow instructions in [examples/usb/README.md](../../README.md) for specific hardware setup.

### Frame format

You can select frame format in [main.c](./main/main.c) file in `stream_config` variable. Default is set to very common 640x480@15FPS, MJPEG encoded.

This example will print supported formats on device connection - this can be disabled in menuconfig ` Component config → USB HOST UVC`. The frame format can also be changed in runtime by calling `uvc_host_stream_format_select()` function.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py set-target esp32xx` to set target chip.

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

## Known limitations

On targets with Full Speed USB peripheral the MPS (maximum packet size) is limited to 512 bytes, the ESP32-S2/S3 is capable of reading data at approximately 0.5 MB/s. When connected to Full Speed USB host, cameras usually provide resolution no larger than 640x480 pixels. The following two formats are the most commonly supported at Full Speed :
* 320x240  30 FPS  MJPEG
* 640x480  15 FPS  MJPEG

## Tested cameras
* Logitech C980
* Logitech C270
* Logitech C170
* Logitech Brio 100
* CANYON CNE-CWC2
* Trust WebCam
* Anker Powerconf C200
* Microsoft LifeCam HD-6000
* Asus Webcam C3

## Example Output

Output with USB camera already connected at program start:
```
...
I (1460) main_task: Calling app_main()
I (1460) UVC example: Installing USB Host
I (1500) UVC example: Installing UVC driver
I (1500) UVC example: Opening UVC device 0x0000:0x0000 640x480@15.0FPS...
I (1500) main_task: Returned from app_main()
*** Configuration descriptor ***

...
Here is rest of Configuration descriptor
...

I (3140) UVC example: UVC Device OPENED!
I (3240) UVC example: Stream start. Iteration 0
I (4410) UVC example: New frame! Len: 13392
I (4470) UVC example: New frame! Len: 13392
I (4530) UVC example: New frame! Len: 13392

...
More frames received
...

I (8310) UVC example: New frame! Len: 18512
I (8310) UVC example: Stream stop
I (10440) UVC example: Stream start. Iteration 1

...
Streaming continues here
```
