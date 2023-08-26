| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

ESP BLE Mesh Node demo
==========================

This demo shows how BLE Mesh device can be set up as a node with the following features:

- One element
- Two SIG models
	- **Configuration Server model**: The role of this model is mainly to configure Provisioner device’s AppKey and set up its relay function, TTL size, subscription, etc.
   - **OnOff Server model**: This model implements the most basic function of turning the lights on and off.

The default purpose of this demo is to enable the advertising function with 20-ms non-connectable interval in BLE 5.0. You can disable this function through menuconfig: `idf.py menuconfig --> Example Configuration --> This option facilitates sending with 20ms non-connectable interval...`

For a better demonstration effect, an RGB LED can be soldered onto the ESP32-DevKitC board, by connecting their corresponding GPIO pins are GPIO\_NUM\_25, GPIO\_NUM\_26, GPIO\_NUM\_27. Then you need to select the following option in menuconfig:
   `idf.py menuconfig --> Example Configuration --> Board selection for BLE Mesh --> ESP-WROOM-32`

Please check the [tutorial](tutorial/BLE_Mesh_Node_OnOff_Server_Example_Walkthrough.md) for more information about this example.
