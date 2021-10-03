| Supported Targets | ESP32 |
| ----------------- | ----- |

# ESP-IDF BT/BLE HID Device Demo

This demo use APIs which esp_hid component provided to create a BT, BLE or Bluetooth dual mode hid device. Users can choose mode by setting `HID_DEV_MODE`.

The BT hid device plays as a mouse. When the connection is successfully established, users can follow the usage below to operate the 'mouse'.
```
########################################################################
BT hid mouse demo usage:
You can input these value to simulate mouse: 'q', 'w', 'e', 'a', 's', 'd', 'h'
q -- click the left key
w -- move up
e -- click the right key
a -- move left
s -- move down
d -- move right
h -- show the help
########################################################################
```

The BLE hid device plays as a remote control. When the connection is successfully established, the remote control will set volume up and down periodically.
