| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Captive Portal Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates two methods of a captive portal, used to direct users to an authentication page or other necessary starting point before browsing.

One approach response to all DNS queries with the address of the softAP, and redirects all HTTP requests to the captive portal root page. This "funnelling" of DNS and traffic triggers the captive portal (sign in) to appear on Android, iOS, and Windows. Note that the example will not redirect HTTPS requests.

The other approach is a more modern method which includes a field in the DHCP offer (AKA DHCP Option 114), provided when the client is assigned an IP address, which specifies to the client where the captive portal is. This is advantageous because it doesn't require the overhead of DNS redirects and can work more reliably around HTTPS, HTST, and other security systems, as well as being more standards compliant. This feature is toggleable in the `Example Configuration`, but does not conflict with the DNS methodology -- these two methods work towards the same goal and can complement each other.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming
* WiFi interface

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Set the Wi-Fi configuration.
    * Set `SoftAP SSID`
    * Set `SoftAP Password`
    * Set `Maximal STA connections`
    * Set `DHCP Captive portal` to enable or disable DHCP Option 114

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output


### ESP32 Output for DNS Redirect

```
I (733) example: Set up softAP with IP: 192.168.4.1
I (743) example: wifi_init_softap finished. SSID:'esp32_ssid' password:'esp32_pwd'
I (753) example: Starting server on port: '80'
I (753) example: Registering URI handlers
I (763) example_dns_redirect_server: Socket created
I (763) example_dns_redirect_server: Socket bound, port 53
I (773) example_dns_redirect_server: Waiting for data
I (1873) wifi:new:<1,1>, old:<1,1>, ap:<1,1>, sta:<255,255>, prof:1
I (1873) wifi:station: e8:84:a5:18:8f:80 join, AID=1, bgn, 40U
I (2203) example: station e8:84:a5:18:8f:80 join, AID=1
I (2833) example_dns_redirect_server: Received 50 bytes from 192.168.4.2 | DNS reply with len: 66
I (2843) example_dns_redirect_server: Waiting for data
I (3043) example_dns_redirect_server: Received 39 bytes from 192.168.4.2 | DNS reply with len: 55
I (3043) example_dns_redirect_server: Waiting for data
I (3043) example_dns_redirect_server: Received 42 bytes from 192.168.4.2 | DNS reply with len: 58
I (3053) example_dns_redirect_server: Waiting for data
W (3203) wifi:<ba-add>idx:4 (ifx:1, e8:84:a5:18:8f:80), tid:0, ssn:9, winSize:64
I (3533) example: Redirecting to root
I (5693) example_dns_redirect_server: Received 37 bytes from 192.168.4.2 | DNS reply with len: 53
I (5693) example_dns_redirect_server: Waiting for data
I (5783) example_dns_redirect_server: Received 46 bytes from 192.168.4.2 | DNS reply with len: 62
I (5783) example_dns_redirect_server: Waiting for data
I (6303) example_dns_redirect_server: Received 41 bytes from 192.168.4.2 | DNS reply with len: 57
I (6303) example_dns_redirect_server: Waiting for data
I (6303) example_dns_redirect_server: Received 41 bytes from 192.168.4.2 | DNS reply with len: 57
I (6313) example_dns_redirect_server: Waiting for data
I (6593) example: Redirecting to root
I (9623) example: Redirecting to root
I (12913) example: Redirecting to root
I (13263) example_dns_redirect_server: Received 34 bytes from 192.168.4.2 | DNS reply with len: 50
I (13273) example_dns_redirect_server: Waiting for data
I (13273) example_dns_redirect_server: Received 34 bytes from 192.168.4.2 | DNS reply with len: 50
I (13283) example_dns_redirect_server: Waiting for data
I (16303) example_dns_redirect_server: Received 32 bytes from 192.168.4.2 | DNS reply with len: 48
I (16303) example_dns_redirect_server: Waiting for data
I (18073) example: Redirecting to root
I (18273) example_dns_redirect_server: Received 34 bytes from 192.168.4.2 | DNS reply with len: 50
I (18273) example_dns_redirect_server: Waiting for data
I (18273) example_dns_redirect_server: Received 34 bytes from 192.168.4.2 | DNS reply with len: 50
I (18283) example_dns_redirect_server: Waiting for data
I (20683) example_dns_redirect_server: Received 42 bytes from 192.168.4.2 | DNS reply with len: 58
I (20683) example_dns_redirect_server: Waiting for data
I (20753) example: Redirecting to root
I (21323) example: Redirecting to root
I (22683) example_dns_redirect_server: Received 48 bytes from 192.168.4.2 | DNS reply with len: 64
I (22693) example_dns_redirect_server: Waiting for data
I (23443) example_dns_redirect_server: Received 48 bytes from 192.168.4.2 | DNS reply with len: 64
I (23453) example_dns_redirect_server: Waiting for data
I (23473) example: Serve root
I (23503) example_dns_redirect_server: Received 48 bytes from 192.168.4.2 | DNS reply with len: 64
I (23513) example_dns_redirect_server: Waiting for data
```

### `tcpdump` Output for DHCP Option 114

Note `URL (114)` with the AP address.

```
19:14:20.522698 c8:yy:yy:yy:yy:yy > 74:xx:xx:xx:xx:xx, ethertype IPv4 (0x0800), length 590: (tos 0x0, ttl 64, id 243, offset 0, flags [none], proto UDP (17), length 576)
    192.168.4.1.67 > 192.168.4.2.68: [udp sum ok] BOOTP/DHCP, Reply, length 548, xid 0x76a26648, Flags [none] (0x0000)
	  Your-IP 192.168.4.2
	  Client-Ethernet-Address 74:xx:xx:xx:xx:xx
	  Vendor-rfc1048 Extensions
	    Magic Cookie 0x63825363
	    DHCP-Message (53), length 1: Offer
	    Subnet-Mask (1), length 4: 255.255.255.0
	    Lease-Time (51), length 4: 7200
	    Server-ID (54), length 4: 192.168.4.1
	    Default-Gateway (3), length 4: 192.168.4.1
	    Domain-Name-Server (6), length 4: 192.168.4.1
	    BR (28), length 4: 192.168.4.255
	    MTU (26), length 2: 1500
	    URL (114), length 18: "http://192.168.4.1"
	    Router-Discovery (31), length 1: N
	    Vendor-Option (43), length 6: 1.4.0.0.0.2
```
