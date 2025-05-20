| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF iBeacon demo

From welcoming people as they arrive at a sporting event to providing information about a nearby museum exhibit, iBeacon opens a new world of possibilities for location awareness, and countless opportunities for interactivity between iOS devices and iBeacon hardware.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

**Note:** *iBeacon is a trademark of Apple Inc.*

Before building devices which use iBeacon technology, visit [Apple iBeacon](https://developer.apple.com/ibeacon/) to obtain a license.

### iBeacon Mode

This example demonstrates iBeacon-compatible BLE advertising, and scanning of iBeacons:

- **IBEACON_SENDER**: demo to send iBeacon-compatible advertising data.

- **IBEACON_RECEIVER**: demo to receive and resolve iBeacon advertising data.

Which demo will be run depends on the menuconfig, developers can set it in `iBeacon Example Configuration`.

The default mode is iBeacon Sender.

### Configure the project

Open the project configuration menu:

```bash
idf.py menuconfig
```

And then enter `Component config --> Bluetooth --> Bluedroid Enable`

Because the number of peripherals may be very large, developers can enable the **BLE Scan Duplicate Options**, the maximum number of devices in scan duplicate filter depends on the free heap size, when the cache is full, it is cleared.

### Event Processing
In the iBeacon receiver demo, the scan result will be posted to `ESP_GAP_SEARCH_INQ_RES_EVT` event:

```c
switch (scan_result->scan_rst.search_evt) {
    case ESP_GAP_SEARCH_INQ_RES_EVT:
    /* Search for BLE iBeacon Packet */
    ......
    break;
    default:
    break;
}
```
### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The iBeacon sender will broadcast iBeacon packet after initializing the Bluetooth protocol stack, and the iBeacon receiver will scan the iBeacon packet.

### iBeacon Sender

```
I (384) boot: Loaded app from partition at offset 0x10000
I (384) boot: Disabling RNG early entropy source...
I (386) cpu_start: Pro cpu up.
I (389) cpu_start: Starting app cpu, entry point is 0x40081010
I (0) cpu_start: App cpu up.
I (400) heap_init: Initializing. RAM available for dynamic allocation:
I (406) heap_init: At 3FFAFF10 len 000000F0 (0 KiB): DRAM
I (413) heap_init: At 3FFCCCA8 len 00013358 (76 KiB): DRAM
I (419) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (425) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (431) heap_init: At 40090E58 len 0000F1A8 (60 KiB): IRAM
I (438) cpu_start: Pro cpu start user code
I (120) cpu_start: Starting scheduler on PRO CPU
I (0) cpu_start: Starting scheduler on APP CPU
I (244) BTDM_INIT: BT controller compile version [44d04c1]

I (244) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (624) phy: phy_version: 3910, c0c45a3, May 21 2018, 18:07:06, 0, 0
I (654) IBEACON_DEMO: register callback
```

### iBeacon Receiver

```
I (384) boot: Loaded app from partition at offset 0x10000
I (384) boot: Disabling RNG early entropy source...
I (385) cpu_start: Pro cpu up.\0x1b[0m
I (389) cpu_start: Starting app cpu, entry point is 0x40081010
I (0) cpu_start: App cpu up.
I (400) heap_init: Initializing. RAM available for dynamic allocation:
I (406) heap_init: At 3FFAFF10 len 000000F0 (0 KiB): DRAM
I (412) heap_init: At 3FFCCC88 len 00013378 (76 KiB): DRAM
I (418) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (425) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (431) heap_init: At 40090E58 len 0000F1A8 (60 KiB): IRAM
I (437) cpu_start: Pro cpu start user code\0x1b[0m
I (120) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (243) BTDM_INIT: BT controller compile version [44d04c1]

I (243) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (633) phy: phy_version: 3910, c0c45a3, May 21 2018, 18:07:06, 0, 0
I (663) IBEACON_DEMO: register callback
I (329203) IBEACON_DEMO: ----------iBeacon Found----------
I (329203) IBEACON_DEMO: Device address:: 30 ae a4 00 42 82
I (329203) IBEACON_DEMO: Proximity UUID:: fd a5 06 93 a4 e2 4f b1 af cf c6 eb 07 64 78 25
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
