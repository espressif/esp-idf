| Supported Targets | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# USB Camera Example

## Overview

This example demonstrates how to:

- Capture video from a USB camera using the `libuvc` library.
- Stream the video over WiFi by hosting a TCP server.

The example enumerates a connected USB camera, negotiates a selected resolution along with an associated `FPS`, then starts capturing video. The `frame_callback` function is then invoked after receiving each frame. Users can process the received frames according to their needs.

Optionally, the captured video can be visualized on a PC with help of the `player.py` script provided in this example. After setting the `Example Configuration->Enable streaming` option in menuconfig, the example will create a TCP server upon startup, and waits until `player.py` connects to the server. Once a connection is established, the example streams each received frame to the PC for visualization. The network connection can be configured in menuconfig via `Example Connection Configuration`.

**Notice** that `libuvc` selects highest possible `dwMaxPayloadTransferSize` by default. As a result, this example will manually overwrite this value to 512 bytes (the maximum packet size supported by ESP32-S2/S3).

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to Use Example

### Hardware Required

* ESP with USB peripheral and external PSRAM
* Exposed USB host connector
* USB camera

Running this example on an **ESP module without external PSRAM will fail on initialization**. Please select your PSRAM configuration in menuconfig `Component config->ESP PSRAM`. If you manually disable PSRAM, the required framebuffers might not fit into DRAM (especially on ESP32-S2).

### Configure the project

Following configuration is needed for streaming video:

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Connection Configuration` menu:

* Set the Wi-Fi configuration.
    * Set `WiFi SSID`.
    * Set `WiFi Password`.

In the `Example Configuration` menu:

* Set the Example configuration
    * `Enable streaming`

* Select one of UVC Protocol Mode
    * `Auto`
    * `Custom`

Optional: If you need, change the other options according to your requirements.

Additionally, the `player.py` python script makes use of the `opencv-python` and `numpy` packages which are not included in the `idf-env` environment by default. Run following command to install those packages:

```bash
pip install opencv-python numpy
```

#### UVC Protocol Mode: Auto

When the protocol mode is set to Auto, the example will make three attempts to negotiate the protocol with following parameters:

1. Attempt: 640x480, 15 FPS, MJPEG
2. Attempt: 320x240, 30 FPS, MJPEG
3. Attempt: 320x240, first available FPS, MJPEG

If all three attempts result in an error, the example displays the error message and suggests to try another USB UVC Device.

#### UVC Protocol Mode: Custom

When the protocol mode set to Custom, the example tries to negotiate the protocol using user provided values for the following parameters: Attempts, Width, Height, FPS, and Frame Coding format. After all attempts result in an error, the example displays the error message and suggests to try another USB UVC device.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py set-target esp32xx` to set target chip.

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

## Known limitations

Having only a Full Speed USB peripheral and hardware limited MPS (maximum packet size) to 512 bytes, the ESP32-S2/S3 is capable of reading data at approximately 0.5 MB/s. When connected to Full Speed USB host, cameras normally provide resolution no larger than 640x480 pixels. The following two formats are the most commonly supported at Full Speed (both encoded in MJPEG):
* 320x240  30 FPS
* 640x480  15 FPS

## Tested cameras
* Logitech C980
* Logitech C270
* CANYON CNE-CWC2

## Example Output

```
...
I (1186) example: Waiting for USB UVC device connection ...
I (1606) example: Device found
DEVICE CONFIGURATION (0c45:6340/	S) ---
Status: idle
VideoControl:
    bcdUVC: 0x0100
VideoStreaming(1):
    bEndpointAddress: 129
    Formats:
    MJPEGFormat(1)
          bits per pixel: 0
          GUID: 4d4a5047000000000000000000000000 (MJPG)
          default frame: 1
          aspect ratio: 0x0
          interlace flags: 00
          copy protect: 00
            FrameDescriptor(1)
              capabilities: 00
              size: 640x480
              bit rate: 24576000-147456000
              max frame size: 614400
              default interval: 1/30
              interval[0]: 1/30
              interval[1]: 1/25
              interval[2]: 1/20
              interval[3]: 1/15
              interval[4]: 1/10
              interval[5]: 1/5
            FrameDescriptor(2)
              capabilities: 00
              size: 352x288
              bit rate: 8110080-48660480
              max frame size: 202752
              default interval: 1/30
              interval[0]: 1/30
              interval[1]: 1/25
              interval[2]: 1/20
              interval[3]: 1/15
              interval[4]: 1/10
              interval[5]: 1/5
            FrameDescriptor(3)
              capabilities: 00
              size: 320x240
              bit rate: 6144000-36864000
              max frame size: 153600
              default interval: 1/30
              interval[0]: 1/30
              interval[1]: 1/25
              interval[2]: 1/20
              interval[3]: 1/15
              interval[4]: 1/10
              interval[5]: 1/5
            FrameDescriptor(4)
              capabilities: 00
              size: 176x144
              bit rate: 2027520-12165120
              max frame size: 50688
              default interval: 1/30
              interval[0]: 1/30
              interval[1]: 1/25
              interval[2]: 1/20
              interval[3]: 1/15
              interval[4]: 1/10
              interval[5]: 1/5
            FrameDescriptor(5)
              capabilities: 00
              size: 160x120
              bit rate: 1536000-9216000
              max frame size: 38400
              default interval: 1/30
              interval[0]: 1/30
              interval[1]: 1/25
              interval[2]: 1/20
              interval[3]: 1/15
              interval[4]: 1/10
              interval[5]: 1/5
            StillFrameDescriptor
              bEndPointAddress: 00
              wWidth(1) = 640
              wHeight(1) = 480
              wWidth(2) = 352
              wHeight(2) = 288
              wWidth(3) = 320
              wHeight(3) = 240
              wWidth(4) = 176
              wHeight(4) = 144
              wWidth(5) = 160
              wHeight(5) = 120
END DEVICE CONFIGURATION
I (1796) example: Negotiate streaming profile 640x480, fps 15 ...
I (1816) example: Negotiation complete.
bmHint: 0001
bFormatIndex: 1
bFrameIndex: 1
dwFrameInterval: 666666
wKeyFrameRate: 0
wPFrameRate: 0
wCompQuality: 0
wCompWindowSize: 0
wDelay: 23469
dwMaxVideoFrameSize: 614400
dwMaxPayloadTransferSize: 512
bInterfaceNumber: 1
I (1836) example: Streaming...
I (4016) example: fps: 10, bytes per second: 69920

...

I (44916) example: fps: 9, bytes per second: 62928
E (45626) USBH: Device 1 gone
I (45636) example: Done streaming.
```
