| Supported Targets | ESP32 |
| ----------------- | ----- |

# ESP-IDF BT-INQUIRY demo

Demo of Classic Bluetooth Device and Service Discovery
This is the demo for user to use ESP_APIs to perform inquiry to search for a target device and then performs service search via SDP.

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Description

After the program started, the device will start inquiry to search for a device with Major device type "PHONE" in the Class of Device Field. Then it will cancel the inquiry and started to perform service discovering on this remote device.

## Trouble shooting

- Some old BT device doesn't place their device name in EIR, but new devices place their device name in EIR. Users can obtain the peer device name in `bt_app_gap_cb` `ESP_BT_GAP_DISC_RES_EVT` event handler or resolve it in EIR just like the function `get_name_from_eir` does.

- ESP32 places its device name in EIR by default.