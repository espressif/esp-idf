| Supported Targets | ESP32 |
| ----------------- | ----- |

# ESP-IDF BT/BLE HID Host Demo

This demo use APIs which esp_hid component provided to create a Bluetooth dual mode hid host. After the program is started, the HID host will scan the surrounding Bluetooth HID device, and try to connect to the last device which has been scanned. When the connection is successfully established, the HID host will dump the HID device information, and can receive the data sent by the HID device.
