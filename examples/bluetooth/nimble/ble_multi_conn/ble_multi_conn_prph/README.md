| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# BLE Multiple Connection Peripheral Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Please check the [tutorial](tutorial/Ble_Multiple_Connections_Peripheral_Example_Walkthrough.md) for more information about this example.

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
I (444) ESP_MULTI_CONN_EXT: BLE Host Task Started
I (1854) ESP_MULTI_CONN_EXT: Started adv, Device Address e0:e1:f5:6f:ec:9d
I (6254) ESP_MULTI_CONN_EXT: Connection established. Handle:1. Total:1
I (6254) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (6504) ESP_MULTI_CONN_EXT: Started adv, Device Address ee:16:69:80:72:d5
I (8414) ESP_MULTI_CONN_EXT: Connection established. Handle:2. Total:2
I (8414) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (8654) ESP_MULTI_CONN_EXT: Started adv, Device Address ef:1a:6e:d6:64:44
I (10124) ESP_MULTI_CONN_EXT: Connection established. Handle:3. Total:3
I (10124) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (10434) ESP_MULTI_CONN_EXT: Started adv, Device Address cb:f4:5d:b2:c8:1d
I (11264) ESP_MULTI_CONN_EXT: Connection established. Handle:4. Total:4
I (11264) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (11464) ESP_MULTI_CONN_EXT: Started adv, Device Address e8:08:e5:ad:61:f6
I (12414) ESP_MULTI_CONN_EXT: Connection established. Handle:5. Total:5
I (12414) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (12794) ESP_MULTI_CONN_EXT: Started adv, Device Address c1:53:a8:6f:2a:b4
I (14154) ESP_MULTI_CONN_EXT: Connection established. Handle:6. Total:6
I (14154) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (14294) ESP_MULTI_CONN_EXT: Started adv, Device Address dd:fb:5b:13:6a:20
I (14934) ESP_MULTI_CONN_EXT: Connection established. Handle:7. Total:7
I (14934) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (15324) ESP_MULTI_CONN_EXT: Started adv, Device Address d5:71:9c:fe:4f:6e
I (16594) ESP_MULTI_CONN_EXT: Connection established. Handle:8. Total:8
I (16594) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (16974) ESP_MULTI_CONN_EXT: Started adv, Device Address d9:56:91:21:d4:25
I (17904) ESP_MULTI_CONN_EXT: Connection established. Handle:9. Total:9
I (17914) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (18104) ESP_MULTI_CONN_EXT: Started adv, Device Address f7:f9:b1:73:38:13
I (18734) ESP_MULTI_CONN_EXT: Connection established. Handle:10. Total:10
I (18734) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (19004) ESP_MULTI_CONN_EXT: Started adv, Device Address e7:e5:94:d0:32:78
I (20374) ESP_MULTI_CONN_EXT: Connection established. Handle:11. Total:11
I (20374) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (20684) ESP_MULTI_CONN_EXT: Started adv, Device Address fb:c6:f9:46:11:dc
I (22374) ESP_MULTI_CONN_EXT: Connection established. Handle:12. Total:12
I (22374) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (22594) ESP_MULTI_CONN_EXT: Started adv, Device Address c0:e3:ef:80:e6:fd
I (24394) ESP_MULTI_CONN_EXT: Connection established. Handle:13. Total:13
I (24404) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (24674) ESP_MULTI_CONN_EXT: Started adv, Device Address d8:9d:6d:b8:c9:40
I (26134) ESP_MULTI_CONN_EXT: Connection established. Handle:14. Total:14
I (26134) ESP_MULTI_CONN_EXT: advertisement completed. Reason=0.
I (44654) BLE_PRPH_SVC: Characteristic write; conn_handle=1
I (44654) BLE_PRPH_SVC: 12 
I (44674) BLE_PRPH_SVC: Characteristic write; conn_handle=2
I (44674) BLE_PRPH_SVC: 12 
I (44684) BLE_PRPH_SVC: Characteristic write; conn_handle=3
I (44684) BLE_PRPH_SVC: 12 
I (44694) BLE_PRPH_SVC: Characteristic write; conn_handle=4
I (44694) BLE_PRPH_SVC: 12 
I (44724) BLE_PRPH_SVC: Characteristic write; conn_handle=6
I (44724) BLE_PRPH_SVC: 12 
I (44734) BLE_PRPH_SVC: Characteristic write; conn_handle=7
I (44734) BLE_PRPH_SVC: 12 
I (44754) BLE_PRPH_SVC: Characteristic write; conn_handle=8
I (44754) BLE_PRPH_SVC: 12 
I (44764) BLE_PRPH_SVC: Characteristic write; conn_handle=9
I (44764) BLE_PRPH_SVC: 12 
I (44784) BLE_PRPH_SVC: Characteristic write; conn_handle=10
I (44784) BLE_PRPH_SVC: 12 
I (44804) BLE_PRPH_SVC: Characteristic write; conn_handle=12
I (44804) BLE_PRPH_SVC: 12 
I (44814) BLE_PRPH_SVC: Characteristic write; conn_handle=13
I (44814) BLE_PRPH_SVC: 12 
I (44834) BLE_PRPH_SVC: Characteristic write; conn_handle=14
I (44834) BLE_PRPH_SVC: 12 
I (44914) BLE_PRPH_SVC: Characteristic write; conn_handle=5
I (44914) BLE_PRPH_SVC: 12 
I (45194) BLE_PRPH_SVC: Characteristic write; conn_handle=11
I (45194) BLE_PRPH_SVC: 12
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
