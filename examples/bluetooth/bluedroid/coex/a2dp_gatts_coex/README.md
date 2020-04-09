| Supported Targets | ESP32 |
| ----------------- | ----- |

ESP-IDF A2DP-GATTS_COEX demo
======================
This demo showcases APIs to create a GATT service and A2DP profile and demonstrates BLE and classic Bluetooth coexistence.

The BLE GATT SERVER part of demo creates a GATT service and then starts advertising, waiting to be connected by a GATT client. After the program is started, the GATT client can discover a device named "ESP_COEX_BLE_DEMO". Once a connection is established, GATT client can read or write data to the device. Or GATT client can receive notification or indication data. 

The classic bluetooth A2DP part of the demo implements Advanced Audio Distribution Profile to receive an audio stream. After the program is started, other bluetooth devices such as smart phones can discover a device named "ESP_COEX_A2DP_DEMO". Once a connection is established, audio data can be transmitted. This will be visible in the application log including a count of audio data packets.