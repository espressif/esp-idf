| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# USB Host CDC-ACM driver test project

Main purpose of this application is to test the USB Host CDC-ACM driver.
It tests basic functionality of the driver like open/close/read/write operations,
advanced features like CDC control request, multi-threaded or multi-device access,
as well as reaction to sudden disconnection and other error states.

## Hardware Required

This test expects that TinyUSB dual CDC device with VID = 0x303A and PID = 0x4002
is connected to the USB host.
