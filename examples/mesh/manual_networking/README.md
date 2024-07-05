| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Mesh Manual Networking Example

ESP-MESH provides the function of self-organized networking, but if users disable this function on one node, users must designate a parent for this node.

This example demonstrates how to scan a list of parent candidates, choose an appropriate parent and set as the parent of this node.

Open project configuration menu (`idf.py menuconfig`) to configure the mesh network channel, router SSID, router password and mesh softAP settings.

When the mesh network is established and if you happen to run this example on ESP-WROVER-KIT boards, the RGB light indicator will show you on which layer devices are. The pink represents root; the yellow represents layer 2; the red represents layer 3; the blue represents layer 4; the green represents layer 5; the white represents layer greater than 5.


