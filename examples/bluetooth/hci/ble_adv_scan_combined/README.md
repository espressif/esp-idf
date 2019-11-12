| Supported Targets | ESP32 |
| ----------------- | ----- |

ESP-IDF Combined Bluetooth advertising and scanning
===================================================

This is a Bluetooth advertising and scanning demo with virtual HCI interface. Send Reset, ADV_PARAM, ADV_DATA and HCI_ADV_ENABLE command for BLE advertising. And SET_EVENT_MASK, SCAN_PARAMS and SCAN_START commands for BLE scanning. Scanned advertising reports from other devices are also displayed.

In this example no host is used. But some of the functionalities of a host are implemented.
