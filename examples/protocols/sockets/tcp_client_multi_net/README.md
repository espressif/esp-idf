| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Multiple Ethernet Example
(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of Ethernet interface and WiFi station together. The workflow of the example could be as follow:

1. Connects to both WiFi and Ethernet using common-connect component
2. Starts two tasks, one for each interface to resolve configured host name and connect to it periodically
3. Connection to host endpoint is handled by:
    - creating a socket as TCP client
    - binding it to the related interface (Ethernet of WiFi)
    - send and receive a trivial HTTP request and response

If you have a new multiple interface application to go (for example, connect to IoT cloud via Ethernet and WiFi), try this as a basic template, then add your own code.

## How to use example

### Hardware Required

To run this example, you need to have one ESP32 development board integrated with an Ethernet interface, for example, ESP32-Ethernet-Kit, or just connect your ESP32-DevkitC board to a breakout board which features RMII Ethernet PHY.

### Configure the project

Enter project configuration by `idf.py menuconfig` and navigate into:

* `Example Connection Configuration` menu to choose the connection details:

    - Enter SSID and password for WiFi connection
    - Set Ethernet type and configuration for Ethernet connection
    - Note that the project is preconfigured to have both WiFi and Ethernet interface enabled by default
    - See the [README.md](../../README.md) for more details about common example connection component

* `Example Configuration` menu:

    - Set host name and port for the tcp_client to connect to

### Build and Flash

Run `idf.py -p PORT flash monitor` to build and flash the project..

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (695) example_connect: Connecting to DavidsAP...
I (795) phy: phy_version: 4180, cb3948e, Sep 12 2019, 16:39:13, 0, 0
I (795) wifi:mode : sta (30:ae:a4:c6:b4:f8)
W (795) event: handler already registered, overwriting
I (815) esp_eth.netif.glue: 30:ae:a4:c6:b4:fb
I (815) esp_eth.netif.glue: ethernet attached to netif
I (825) example_connect: Waiting for IP(s)
I (1525) wifi:new:<6,0>, old:<1,0>, ap:<255,255>, sta:<6,0>, prof:1
I (2295) wifi:state: init -> auth (b0)
I (2295) wifi:state: auth -> assoc (0)
I (2305) wifi:state: assoc -> run (10)
I (2315) wifi:connected with DavidsAP, aid = 2, channel 6, BW20, bssid = 16:f7:28:37:58:36
I (2315) wifi:security type: 3, phy: bgn, rssi: -35
I (2315) wifi:pm start, type: 1

I (2325) wifi:AP's beacon interval = 102400 us, DTIM period = 3
I (3125) esp_netif_handlers: example_connect: sta ip: 192.168.2.15, mask: 255.255.255.0, gw: 192.168.2.1
I (3125) example_connect: Interface description example_connect: sta
I (3135) example_connect: Interface "example_connect: sta" got IPv4 address: 192.168.2.15
I (3625) example_connect: Interface "example_connect: sta" got IPv6 address: fe80:0000:0000:0000:32ae:a4ff:fec6:b4f8, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (4825) example_connect: Ethernet Link Up
I (5625) esp_netif_handlers: example_connect: eth ip: 192.168.32.148, mask: 255.255.252.0, gw: 192.168.32.3
I (5625) example_connect: Interface description example_connect: eth
I (5635) example_connect: Interface "example_connect: eth" got IPv4 address: 192.168.32.148
I (6625) example_connect: Interface "example_connect: eth" got IPv6 address: fe80:0000:0000:0000:32ae:a4ff:fec6:b4fb, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (6625) example_connect: Connected to example_connect: eth
I (6635) example_connect: - IPv4 address: 192.168.32.148
I (6635) example_connect: - IPv6 address: fe80:0000:0000:0000:32ae:a4ff:fec6:b4fbtype: ESP_IP6_ADDR_IS_LINK_LOCAL
I (6645) example_connect: Connected to example_connect: sta
I (6655) example_connect: - IPv4 address: 192.168.2.15
I (6655) example_connect: - IPv6 address: fe80:0000:0000:0000:32ae:a4ff:fec6:b4f8type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (6675) example_connect: Connected to Ethernet
I (6675) tcp_client_multiple: netif described as "sta" corresponds to esp-netif ptr:0x3ffba3ac
I (6675) tcp_client_multiple: netif described as "eth" corresponds to esp-netif ptr:0x3ffc608c
I (6895) tcp_client_multiple: "example_connect: eth" Socket created
I (6895) tcp_client_multiple: "example_connect: sta" Socket created
I (6895) tcp_client_multiple: "example_connect: eth" Successfully connected
I (6905) tcp_client_multiple: "example_connect: sta" Successfully connected
I (6965) tcp_client_multiple: "example_connect: eth" Received Data 127 bytes
I (6965) tcp_client_multiple: HTTP/1.1 200 OK
Date: Thu, 23 Apr 2020 07:02:58 GMT
Expires: -1
Cache-Control: private, max-age=0
Content-Type: text/html;
I (6965) tcp_client_multiple: "example_connect: sta" Received Data 127 bytes
I (6985) tcp_client_multiple: HTTP/1.1 200 OK
Date: Thu, 23 Apr 2020 07:02:58 GMT
Expires: -1
Cache-Control: private, max-age=0
Content-Type: text/html;
I (7675) tcp_client_multiple: "example_connect: eth" Socket created
I (7675) tcp_client_multiple: "example_connect: eth" Successfully connected
I (7695) tcp_client_multiple: "example_connect: sta" Socket created
I (7705) tcp_client_multiple: "example_connect: sta" Successfully connected
I (7735) tcp_client_multiple: "example_connect: eth" Received Data 127 bytes
I (7735) tcp_client_multiple: HTTP/1.1 200 OK
Date: Thu, 23 Apr 2020 07:02:59 GMT
Expires: -1
Cache-Control: private, max-age=0
Content-Type: text/html;
I (7955) tcp_client_multiple: "example_connect: sta" Received Data 127 bytes
I (7955) tcp_client_multiple: HTTP/1.1 200 OK
Date: Thu, 23 Apr 2020 07:02:59 GMT
Expires: -1
Cache-Control: private, max-age=0
Content-Type: text/html;
I (8445) tcp_client_multiple: "example_connect: eth" Socket created
I (8445) tcp_client_multiple: "example_connect: eth" Successfully connected
I (8505) tcp_client_multiple: "example_connect: eth" Received Data 127 bytes
I (8505) tcp_client_multiple: HTTP/1.1 200 OK
Date: Thu, 23 Apr 2020 07:03:00 GMT
Expires: -1
Cache-Control: private, max-age=0
Content-Type: text/html;
I (8675) tcp_client_multiple: "example_connect: sta" Socket created
```

## Troubleshooting

* When connecting using Ethernet, please consult troubleshooting described in [Ethernet common readme](../../../ethernet/README.md)
or [Ethernet documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_eth.html).
If using Ethernet for the first time, it is recommended to start with the [Ethernet example readme](../../../ethernet/basic/README.md), which contains instructions for connecting and configuring the PHY.
Once Ethernet example obtains IP address successfully, proceed to this example.

* When connecting using Wi-Fi, please refer to the WiFi examples in [examples/wifi/getting_started/](../../../wifi/getting_started).
