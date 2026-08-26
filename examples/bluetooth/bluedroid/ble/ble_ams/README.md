| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# ESP-IDF BLE AMS Example

The purpose of the Apple Media Service (AMS) is to give Bluetooth accessories (that connect to iOS devices through a Bluetooth low-energy link) a simple and convenient way to control media apps and access information about the media state of the connected iOS device.

## Flow Diagram

```
    ┌──────────────┐                                    ┌──────────────┐
    │    ESP32     │                                    │  iOS Device  │
    │ (AMS Client) │                                    │ (AMS Server) │
    └──────┬───────┘                                    └──────┬───────┘
           │                                                   │
           │  ─────────── Connection ───────────               │
           │                                                   │
           │  1. Start Advertising                             │
           │ ───────────────────────────────────────────────>  │
           │                                                   │
           │  2. iOS connects & pairs                          │
           │ <───────────────────────────────────────────────  │
           │                                                   │
           │  ─────────── Service Discovery ───────────        │
           │                                                   │
           │  3. Discover AMS Service                          │
           │     (UUID: 89D3502B-0F36-433A-8EF4-C502AD55F8DC)  │
           │ ───────────────────────────────────────────────>  │
           │                                                   │
           │  4. Subscribe to Remote Command                   │
           │     (UUID: 9B3C81D8-57B1-4A8A-B8DF-0E56F7CA51C2)  │
           │ ───────────────────────────────────────────────>  │
           │                                                   │
           │  5. Subscribe to Entity Update                    │
           │     (UUID: 2F7CABCE-808D-411F-9A0C-BB92BA96C102)  │
           │ ───────────────────────────────────────────────>  │
           │                                                   │
           │  6. Register entity/attribute pairs               │
           │     (Write to Entity Update)                      │
           │ ───────────────────────────────────────────────>  │
           │                                                   │
           │  ─────────── Media Update Flow ───────────        │
           │                                                   │
           │  7. Media state changes on iOS                    │
           │     (Track, Player, Queue updates)                │
           │ <───────────────────────────────────────────────  │
           │                                                   │
           │  8. Optional: read full truncated value           │
           │     (Write + Read Entity Attribute)               │
           │ ───────────────────────────────────────────────>  │
           │                                                   │
    ┌──────┴───────┐                                    ┌──────┴───────┐
    │    ESP32     │                                    │  iOS Device  │
    └──────────────┘                                    └──────────────┘
```

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

The Apple Media Service is a primary service whose service UUID is:

`89D3502B-0F36-433A-8EF4-C502AD55F8DC`

Only one instance of the AMS may be present on an MS. Due to the nature of iOS, the AMS is not guaranteed to always be present. As a result, the MR should look for and subscribe to the Service Changed characteristic of the GATT service in order to monitor for the potential publishing and unpublishing of the AMS at any time.

In its basic form, the AMS exposes three characteristics:

* Remote Command: UUID `9B3C81D8-57B1-4A8A-B8DF-0E56F7CA51C2` (writeable, notifiable)
* Entity Update: UUID `2F7CABCE-808D-411F-9A0C-BB92BA96C102` (writeable with response, notifiable)
* Entity Attribute: UUID `C6B2F38C-23AB-46D8-A6AB-A3A870BBD5D7` (readable, writeable)

After pairing, start playing music on the iOS device. The example subscribes to track, player, and queue entity updates and prints media metadata to the console.

The example uses legacy connectable advertising (`ESP_BLE_AMS`).

Advertising data matches `ble_ancs`: HID appearance, 16-bit service UUID `0x1812`, and device name in the scan response.

To send a remote command from application code, call `esp_send_remote_command()` with a value from `esp_remote_command_id_t`, for example `RemoteCommandIDTogglePlayPause`.

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-H2/ESP32-C2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (918) BLE_AMS: app_main init bluetooth
I (1018) BLE_AMS: REG_EVT
I (1058) BLE_AMS: advertising start success
I (5234) BLE_AMS: ESP_GATTC_OPEN_EVT
I (5456) BLE_AMS: ESP_GATTC_CFG_MTU_EVT, Status 0, MTU 517, conn_id 0
I (5678) BLE_AMS: Found Remote Command characteristic
I (5678) BLE_AMS: Found Entity Update characteristic
I (5680) BLE_AMS: Found Entity Attribute characteristic
I (5720) BLE_AMS: Entity Update notifications enabled
I (5721) BLE_AMS: Subscribe track entity updates
I (5800) BLE_AMS: Supported remote commands (6):
I (5801) BLE_AMS:   [0] Play
I (5802) BLE_AMS:   [1] Pause
I (5900) BLE_AMS: Entity update: Track / Title, flags=0x00, len=12
I (5901) BLE_AMS: Value: Example Song
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
