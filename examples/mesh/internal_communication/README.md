| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# Mesh Internal Communication Example

This example demonstrates how to use the mesh APIs to set up a mesh network, send and receive messages over the mesh network and etc.

Features Demonstrated

- mesh initialization

- mesh configuration

- mesh start

- mesh event handler

- root send and receive

- other nodes receive

Open project configuration menu (`idf.py menuconfig`) to configure the mesh network channel, router SSID, router password and mesh softAP settings.

When the mesh network is established and if you happen to run this example on ESP-WROVER-KIT boards, the RGB light indicator will show you on which layer devices are.
The pink reprents root; the yellow reprents layer 2; the red reprents layer 3; the blue reprents layer 4; the green reprents layer 5; the white reprents layer greater than 5.
Root continuously sends an On / Off control message to all devices in its routing table. Devices including root itself receive this message and do the On / Off.
