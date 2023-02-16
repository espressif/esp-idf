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
I (4788) phy:  Tx continuous test!
phy> 
phy> tx_contin_en 0
I (7518) phy: Tx packet test!
phy> 
phy> cbw40m_en 1
I (10668) phy: cbw40m_en: 1
phy> 
phy> cbw40m_en 0
I (13238) phy: cbw40m_en: 0
phy> 
phy> cmdstop
I (8828) phy: Tx Over 0x0
phy> 
phy> esp_rx -n 1 -r 0
I (19348) phy: wifi rx start: channel is 1, rate is 0x0
phy> 
phy> cmdstop
I (142881) phy: Total: 535 Correct: 474 RSSI: -650 noise: -960 gain: 531 para1: 5759 para2: 3
phy>
phy> get_rx_result
I (145991) cmd_phy: Total: 535, Correct: 474, RSSI: -650, flag: 1
phy> 
phy> wifiscwout -e 1 -c 1 -p 0
I (98308) phy:  wifi single carrier tx out, single carrier is in 1 channel, power is about (2
phy> 
phy> esp_ble_tx -p 8 -n 1 -l 37 -t 2 -s 0x71764129 -r 0 -m 0
I (319138) phy: fcc_le_tx_syncw: txpwr=8,chan=1,len=37,data_type=2,syncw=0x71764129,rate=0,tx_
phy> 
phy> cmdstop
I (321208) phy: rw done! ce9
phy> 
phy> esp_ble_rx -n 1 -s 0x71764129 -r 0
I (2898) phy: RW LE V9 RX PER
phy> 
phy> cmdstop
I (381241) phy: 3 0 0 0 3 0 0 0 0 0 p -263 -86 -279 a8 -100 -77
phy>
phy> get_rx_result
I (383871) cmd_phy: Total: 3, Correct: 0, RSSI: 0, flag: 2
phy> 
phy> bt_tx_tone -e 1 -n 1 -p 0
I (56008) phy: BT TX TONE START!
phy>
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

