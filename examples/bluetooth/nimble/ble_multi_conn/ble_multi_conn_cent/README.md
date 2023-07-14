| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# BLE Multiple Connection Central Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Please check the [tutorial](tutorial/Ble_Multiple_Connections_Central_Example_Walkthrough.md) for more information about this example.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* At least two development board with ESP32-C6/ESP32-H2 SoC (e.g., ESP32-C6-DevKitC, ESP32-H2-DevKitC, etc.)
* USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
controller lib commit: [5cacafa]
I (564) ESP_MULTI_CONN_EXT: BLE Host Task Started
I (564) main_task: Returned from app_main()
I (604) ESP_MULTI_CONN_EXT: Create connection. -> peer addr e0:e1:f5:6f:ec:9d
I (1234) ESP_MULTI_CONN_EXT: Connection established. Handle:1, Total:1
I (2754) ESP_MULTI_CONN_EXT: Create connection. -> peer addr ee:16:69:80:72:d5
I (3394) ESP_MULTI_CONN_EXT: Connection established. Handle:2, Total:2
I (4454) ESP_MULTI_CONN_EXT: Create connection. -> peer addr ef:1a:6e:d6:64:44
I (5094) ESP_MULTI_CONN_EXT: Connection established. Handle:3, Total:3
I (6144) ESP_MULTI_CONN_EXT: Create connection. -> peer addr cb:f4:5d:b2:c8:1d
I (6244) ESP_MULTI_CONN_EXT: Connection established. Handle:4, Total:4
I (6444) ESP_MULTI_CONN_EXT: Create connection. -> peer addr e8:08:e5:ad:61:f6
I (7394) ESP_MULTI_CONN_EXT: Connection established. Handle:5, Total:5
I (8504) ESP_MULTI_CONN_EXT: Create connection. -> peer addr c1:53:a8:6f:2a:b4
I (9124) ESP_MULTI_CONN_EXT: Connection established. Handle:6, Total:6
I (9274) ESP_MULTI_CONN_EXT: Create connection. -> peer addr dd:fb:5b:13:6a:20
I (9904) ESP_MULTI_CONN_EXT: Connection established. Handle:7, Total:7
I (10934) ESP_MULTI_CONN_EXT: Create connection. -> peer addr d5:71:9c:fe:4f:6e
I (11574) ESP_MULTI_CONN_EXT: Connection established. Handle:8, Total:8
I (12264) ESP_MULTI_CONN_EXT: Create connection. -> peer addr d9:56:91:21:d4:25
I (12884) ESP_MULTI_CONN_EXT: Connection established. Handle:9, Total:9
I (13084) ESP_MULTI_CONN_EXT: Create connection. -> peer addr f7:f9:b1:73:38:13
I (13704) ESP_MULTI_CONN_EXT: Connection established. Handle:10, Total:10
I (14724) ESP_MULTI_CONN_EXT: Create connection. -> peer addr e7:e5:94:d0:32:78
I (15354) ESP_MULTI_CONN_EXT: Connection established. Handle:11, Total:11
I (16404) ESP_MULTI_CONN_EXT: Create connection. -> peer addr fb:c6:f9:46:11:dc
I (17344) ESP_MULTI_CONN_EXT: Connection established. Handle:12, Total:12
I (18434) ESP_MULTI_CONN_EXT: Create connection. -> peer addr c0:e3:ef:80:e6:fd
I (19374) ESP_MULTI_CONN_EXT: Connection established. Handle:13, Total:13
I (20484) ESP_MULTI_CONN_EXT: Create connection. -> peer addr d8:9d:6d:b8:c9:40
I (21104) ESP_MULTI_CONN_EXT: Connection established. Handle:14, Total:14
I (30814) ESP_MULTI_CONN_EXT: Connected to central. Handle:15
I (39624) BLE_CENT_SVC: Characteristic write; conn_handle=15
I (39624) BLE_CENT_SVC: 12
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
