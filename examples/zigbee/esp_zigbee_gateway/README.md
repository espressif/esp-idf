| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- |

# Gateway Example

This example demonstrates how to build a Zigbee Gateway device.

The ESP Zigbee SDK provides more examples and tools for productization:
* [ESP Zigbee SDK Docs](https://docs.espressif.com/projects/esp-zigbee-sdk)
* [ESP Zigbee SDK Repo](https://github.com/espressif/esp-zigbee-sdk)

## Hardware Required

By default, two SoCs are required to run this example:
* An ESP32 series Wi-Fi SoC (ESP32, ESP32-C, ESP32-S, etc) loaded with this esp_zigbee_gateway example.
* An ESP32-H2 802.15.4 SoC loaded with [ot_rcp](../../openthread/ot_rcp/) example

Connect the two SoCs via UART, below is an example setup with ESP32-DevKitC and ESP32-H2-DevKitC:
![Zigbee_gateway](../../openthread/ot_br/image/thread-border-router-esp32-esp32h2.jpg)

ESP32 pin     | ESP32-H2 pin
------------- |-------------
   GND        |    G
   GPIO4 (RX) |    TX
   GPIO5 (TX) |    RX

* TX, RX pin can be also configured by user in esp_zigbee_gateway.h

The example could also run on a single SoC which supports both Wi-Fi and Zigbee (e.g., ESP32-C6), but since there is only one RF path in ESP32-C6, which means Wi-Fi and Zigbee can't receive simultaneously, it has a significant impact on performance. Hence the two SoCs solution is recommended.

## Configure the RCP

The `OPENTHREAD_NCP_VENDOR_HOOK` of `ot_rcp` should be selected via menuconfig when the [ot_rcp](../../openthread/ot_rcp/) example is built. Then use `idf.py -p PORT erase-flash` to flash the RCP firmware to ESP32-H2-DevKitC.

## Configure the project

Before project configuration and build, make sure to set the correct chip target using `idf.py set-target <chip_name>`.

In order to run the example on single SoC which supports both Wi-Fi and Thread, the option `CONFIG_ESP_COEX_SW_COEXIST_ENABLE` and option `CONFIG_ZB_RADIO_NATIVE` should be enabled. The two options are enabled by default for ESP32-C6 target.

## Erase the NVRAM 

Before flash it to the board, it is recommended to erase NVRAM if user doesn't want to keep the previous examples or other projects stored info using `idf.py -p PORT erase-flash`

## Build and Flash

Build the project, flash it to the board, and start the monitor tool to view the serial output by running `idf.py -p PORT flash monitor`

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

As you run the example, you will see the following log:

esp_zigbee_gateway:
```
I (499) main_task: Calling app_main()
I (519) ESP_RADIO_SPINEL: spinel UART interface initialization completed
I (519) ESP_RADIO_SPINEL: Spinel UART interface has been successfully enabled
I (519) ZB_ESP_SPINEL: Spinel UART interface enable successfully
I (529) main_task: Returned from app_main()
I(529) OPENTHREAD:[I] P-RadioSpinel-: RCP reset: RESET_POWER_ON
I(539) OPENTHREAD:[I] P-RadioSpinel-: Software reset RCP successfully
I (569) ZB_ESP_SPINEL: Radio spinel workflow register successfully
I (769) ESP_ZB_GATEWAY: Production configuration is ready
W (769) ESP_ZB_GATEWAY: Production configuration is not present
I (769) example_connect: Start example_connect.
I (779) pp: pp rom version: e7ae62f
I (779) net80211: net80211 rom version: e7ae62f
I (799) wifi:wifi driver task: 3fca80d8, prio:23, stack:6656, core=0
I (799) wifi:wifi firmware version: 3ce09e5
I (799) wifi:wifi certification version: v7.0
I (799) wifi:config NVS flash: enabled
I (799) wifi:config nano formatting: disabled
I (809) wifi:Init data frame dynamic rx buffer num: 32
I (809) wifi:Init static rx mgmt buffer num: 5
I (819) wifi:Init management short buffer num: 32
I (819) wifi:Init dynamic tx buffer num: 32
I (819) wifi:Init static tx FG buffer num: 2
I (829) wifi:Init static rx buffer size: 1600
I (829) wifi:Init static rx buffer num: 10
I (839) wifi:Init dynamic rx buffer num: 32
I (839) wifi_init: rx ba win: 6
I (839) wifi_init: tcpip mbox: 32
I (849) wifi_init: udp mbox: 6
I (849) wifi_init: tcp mbox: 6
I (849) wifi_init: tcp tx win: 5760
I (859) wifi_init: tcp rx win: 5760
I (859) wifi_init: tcp mss: 1440
I (869) wifi_init: WiFi IRAM OP enabled
I (869) wifi_init: WiFi RX IRAM OP enabled
I (879) phy_init: phy_version 670,b7bc9b9,Apr 30 2024,10:54:13
W (879) phy_init: failed to load RF calibration data (0x1102), falling back to full calibration
I (989) wifi:mode : sta (f4:12:fa:41:a7:f4)
I (989) wifi:enable tsf
I (999) example_connect: Connecting to esp-office-2.4G...
I (999) example_connect: Waiting for IP(s)
I (3409) wifi:new:<13,0>, old:<1,0>, ap:<255,255>, sta:<13,0>, prof:1
I (3649) wifi:state: init -> auth (b0)
I (3719) wifi:state: auth -> assoc (0)
I (3759) wifi:state: assoc -> run (10)
I (3769) wifi:connected with esp-office-2.4G, aid = 1, channel 13, BW20, bssid = 9c:3a:9a:04:18:92
I (3769) wifi:security: WPA2-PSK, phy: bgn, rssi: -42
I (3769) wifi:pm start, type: 1

I (3779) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3789) wifi:set rx beacon pti, rx_bcn_pti: 0, bcn_timeout: 25000, mt_pti: 0, mt_time: 10000
I (3819) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (3849) wifi:<ba-add>idx:0 (ifx:0, 9c:3a:9a:04:18:92), tid:0, ssn:0, winSize:64
I (4799) esp_netif_handlers: example_netif_sta ip: 192.168.200.133, mask: 255.255.252.0, gw: 192.168.200.1
I (4799) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.200.133
I (5509) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fe80:0000:0000:0000:f612:faff:fe41:a7f4, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5509) example_common: Connected to example_netif_sta
I (5519) example_common: - IPv4 address: 192.168.200.133,
I (5519) example_common: - IPv6 address: fe80:0000:0000:0000:f612:faff:fe41:a7f4, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5529) wifi:Set ps type: 0, coexist: 0

I (5539) ESP_ZB_GATEWAY: Initialize Zigbee stack
I (5549) ESP_ZB_GATEWAY: Device started up in  factory-reset mode
I (5549) ESP_ZB_GATEWAY: Start network formation
W (5729) ESP_ZB_GATEWAY: Network(0xb8e9) closed, devices joining not allowed.
I (5729) ESP_ZB_GATEWAY: Formed network successfully (Extended PAN ID: 60:55:f9:ff:fe:f7:73:e8, PAN ID: 0xb8e9, Channel:13, Short Address: 0x0000)
I (6339) ESP_ZB_GATEWAY: Network(0xb8e9) is open for 180 seconds
I (6339) ESP_ZB_GATEWAY: Network steering started
```

## Gateway Functions

 * When the device starts up, it will attempt to connect to a Wi-Fi network and then interface with the OT-RCP via UART to form a Zigbee network.
 * For more Gateway functionalities, please refer to [matter zigbee bridge](https://github.com/espressif/esp-matter/tree/main/examples/bridge_apps/zigbee_bridge/) and [Rainmaker Zigbee Gateway](https://github.com/espressif/esp-rainmaker/tree/master/examples/zigbee_gateway) examples.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon
