| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

This test app runs a few FATFS test cases in a FAT-formatted SD card.

These tests require a development board with an SD card slot:

* ESP32-WROVER-KIT
* ESP32-S2 USB_OTG
* ESP32-C3-DevKit-C with an SD card breakout board

The test cases are split between `[sdmmc]` and `[sdspi]`. Only a few tests are executed for sdspi, though. The app could be refactored to ensure that a similar set of tests runs for sdmmc and sdspi.

See [../README.md](../README.md) for more information about FATFS test apps.
