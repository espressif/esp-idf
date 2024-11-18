| Supported Targets | ESP32 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | --------- | -------- | -------- |

ESP BLE Mesh Client Model Demo
========================

This demo shows how to use the Generic OnOff Client Model to get/set the generic on/off state. The basic procedures are as follows:

1. Download and run this demo.
2. Use any app for BLE Mesh to provision this device as well as the device running the Generic OnOff Server demo.
3. After both onoff client and server devices are provisioned, use UART1 to input the unicast address of the element within the server device.
4. The Generic OnOff Client will start to get and set Generic OnOff states periodically.

>**Notes:**
>
>1. The NetKey index and AppKey index are fixed to 0x0000 in this demo.
>2. If the client device is re-provisioned, but the server device is not, the first few get/set messages from the client will be treated as replay attacks. To avoid this, both devices should be re-provisioned prior to transmitting messages.