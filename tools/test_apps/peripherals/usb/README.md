| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# USB Host Class driver test project
Main purpose of this application is to test the USB Host Class drivers.

## CDC-ACM driver

It tests basic functionality of the driver like open/close/read/write operations,
advanced features like CDC control request, multi-threaded or multi-device access,
as well as reaction to sudden disconnection and other error states.

### Hardware Required

This test expects that TinyUSB dual CDC device with VID = 0x303A and PID = 0x4002
is connected to the USB host.

## MSC driver

Basic functionality such as MSC device install/uninstall, file operatons, 
raw access to MSC device and sudden disconnect is tested.

### Hardware Required

This test requires two ESP32-S2/S3 boards with a interconnected USB perpherals,
one acting as host running MSC host driver and another MSC device driver (tinyusb).
