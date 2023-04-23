| Supported Targets | ESP32-C3 | ESP32-S3 |
| ----------------- | -------- | -------- |

# Certification Test Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the Certification test APIS.

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32-C3/ESP32-S3/ SoC.
* A USB cable for Power supply and programming

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Enable RF certification test functions.
* Disable Interrupt Watchdog.
* Disable Task Watchdog Timer.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html)

## Example Output

The command and output logs for each test are as follows:

```
 ==================================================
 |            RF certification test               |
 |                                                |
 |  1. Print 'help' to gain overview of commands  |
 |  2. Wi-Fi certification test                   |
 |  3. Bluetooth certification test               |
 |                                                |
 =================================================


Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
I (598) main_task: Returned from app_main()
phy> 
phy> tx_contin_en 1
I (11870) phy:  Tx continuous test!
phy>
phy> tx_contin_en 0
I (19860) phy:  Tx packet test!
phy>
phy> cbw40m_en 1
I (25660) phy: cbw40m_en: 1
phy>
phy> cbw40m_en 0
I (33490) phy: cbw40m_en: 0
phy>
phy> esp_tx -n 1 -r 0x0 -p 0 -l 1000 -d 1000 -c 0
I (39640) phy: Wifi tx out: chan=1, rate=0x0, BK=0, len=1000, delay=1000, tx_num=0
phy>
phy> cmdstop
I (45460) phy: Tx Over 0x0
phy>
phy> esp_rx -n 1 -r 0
I (51130) phy: wifi rx start: channel is 1, rate is 0x0
phy>
phy> cmdstop
I (58800) phy: rx_num: 166 rx_rssi: -527
phy>
phy> get_rx_result
I (66350) cmd_phy: Desired: 167, Correct: 166, RSSI: -527, flag: 1
phy>
phy> wifiscwout -e 1 -c 1 -p 0
I (80310) phy: wifi single carrier tx start: chan=1, pwr=20-0.0 
phy>
phy> wifiscwout -e 0 -c 1 -p 0
I (88260) phy: wifi single carrier tx stop
phy>
phy> esp_ble_tx -p 8 -n 1 -l 37 -t 2 -s 0x71764129 -r 0 -m 0
I (91470) phy: ble_tx:pwr=8,chan=1,len=37,type=2,syncw=0x71764129,rate=0,txnum=0
phy>
phy> cmdstop
I (98650) phy: rw done! e22e
phy>
phy> esp_ble_rx -n 1 -s 0x71764129 -r 0
I (104990) phy: RW LE V9 RX PER
phy>
phy> cmdstop
I (130260) phy: rx_num: 0 rx_rssi: 0
phy>
phy> get_rx_result
I (139550) cmd_phy: Desired: 0, Correct: 0, RSSI: 0, flag: 2
phy>
phy> bt_tx_tone -e 1 -n 1 -p 0
I (151900) phy: BT TX TONE START!
phy>
phy> bt_tx_tone -e 0 -n 1 -p 0
I (191970) phy: BT TX TONE STOP!
phy>
```

## PHY Commands Format

For BLE test, if you want to use `fcc_le_tx` and `rw_le_rx_per` legacy commands for tx/rx test, you need to enable `ESP_PHY_LEGACY_COMMANDS` in menuconfig, otherwise, the new format commands `esp_ble_tx` and `esp_ble_rx` are supported.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.