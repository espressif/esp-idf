| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# HTTP Request Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

Uses a POSIX socket to make a very simple HTTP request.

## How to use example
Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

### Configure the project

```
idf.py menuconfig
```
Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

The default configuration (`sdkconfig.defaults`) fully enables both IPv6 and IPv4: it enables SLAAC (State-Less Address Auto-Configuration) to get IPv6 address prefixes from Router Advertisement (RA) messages, and both IPv6 RA RDNSS (Recursive DNS Server) and DHCPv6 Stateless mode to get IPv6 DNS server details from whichever is available on the network.

The application is written to work on whatever network is available: dual-stack, IPv4-only, and IPv6-only networks.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Dual-stack network connecting to dual-stack server

Default configuration on a dual-stack network (Astral) connecting to a dual-stack server (`v4v6.ipv6-test.com`), starts the connection test as soon as any address, e.g. an IPv4 address (`192.168.1.146`, private range) is received. So the first connection does an IPv4 DNS lookup (`51.75.78.103`) and uses that address.

Note that the server responds with the NAT44 public address it sees for the client (`220.240.255.134`).

When an IPv6 prefix (`2407:8800:bc61:1340::`) is then receive via RA (router advertisement) a global IPv6 address is configured, so the second connection test does a DNS lookup for IPv6 first, and uses that address (`2001:41D0:701:1100::29C8`).

The server then responds with the IPv6 address seen: `2407:8800:bc61:1340:0a3a:f2ff:fe65:db28`

```
I (764) example_connect: Connecting to Astral...
I (764) example_connect: Waiting for IP(s)
I (3174) wifi:new:<11,0>, old:<1,0>, ap:<255,255>, sta:<11,0>, prof:1
I (3424) wifi:state: init -> auth (b0)
I (3434) wifi:state: auth -> assoc (0)
I (3444) wifi:state: assoc -> run (10)
I (3464) wifi:connected with Astral, aid = 8, channel 11, BW20, bssid = f4:92:bf:a8:db:d1
I (3464) wifi:security: WPA2-PSK, phy: bgn, rssi: -70
I (3464) wifi:pm start, type: 1

I (3464) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3494) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (3514) wifi:<ba-add>idx:0 (ifx:0, f4:92:bf:a8:db:d1), tid:6, ssn:1, winSize:64
I (4474) esp_netif_handlers: example_netif_sta ip: 192.168.1.146, mask: 255.255.255.0, gw: 192.168.1.1
I (4474) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.1.146
I (4484) example_common: Connected to example_netif_sta
I (4484) example_common: - IPv4 address: 192.168.1.146,
I (4494) example_common: - IPv6 address: fe80:0000:0000:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (4504) example: DNS lookup v4v6.ipv6-test.com (port 80)
I (4514) example_common: IPv4 DNS lookup
I (4514) main_task: Returned from app_main()
I (4544) wifi:<ba-add>idx:1 (ifx:0, f4:92:bf:a8:db:d1), tid:0, ssn:1, winSize:64
I (4554) example: DNS lookup succeeded. (2) IP=51.75.78.103
I (4554) example: ... allocated socket
I (4564) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fe80:0000:0000:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5044) example: ... connected
I (5044) example: ... socket send success
I (5054) example: ... set socket receiving timeout success
HTTP/1.1 200 OK
Date: Sat, 24 Feb 2024 05:33:08 GMT
Server: Apache/2.4.25 (Debian)
Vary: Accept-Encoding
Connection: close
Content-Type: text/html; charset=UTF-8

220.240.255.134
I (5464) example: ... done reading from socket. Last read return=0 errno=128.
I (5474) example: 10... 
I (6474) example: 9... 
I (7474) example: 8... 
I (7564) example_connect: Got IPv6 event: Interface "example_netif_sta" address: 2407:8800:bc61:1340:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_GLOBAL
I (7564) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fd7c:e25e:67e8:0040:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_UNIQUE_LOCAL
I (8474) example: 7... 
I (9474) example: 6... 
I (10474) example: 5... 
I (11474) example: 4... 
I (12474) example: 3... 
I (13474) example: 2... 
I (14474) example: 1... 
I (15474) example: 0... 
I (16474) example: Starting again!
I (16474) example: DNS lookup v4v6.ipv6-test.com (port 80)
I (16474) example_common: IPv6 DNS lookup
I (16514) example: DNS lookup succeeded. (10) IP=2001:41D0:701:1100::29C8
I (16514) example: ... allocated socket
I (16924) example: ... connected
I (16924) example: ... socket send success
I (16924) example: ... set socket receiving timeout success
HTTP/1.1 200 OK
Date: Sat, 24 Feb 2024 05:33:19 GMT
Server: Apache/2.4.25 (Debian)
Vary: Accept-Encoding
Connection: close
Content-Type: text/html; charset=UTF-8

2407:8800:bc61:1340:a3a:f2ff:fe65:db28
I (17334) example: ... done reading from socket. Last read return=0 errno=128.
I (17344) example: 10... 
```

On a dual-stack network when connecting to an IPv4-only server, then (with an IPv6 global address) an IPv6 lookup is still attempted first. If DNS64/NAT64 is not available, then the IPv6 DNS lookup will fail, and the code will fall back to an IPv4 DNS lookup.

```
I (17034) example: DNS lookup v4.ipv6-test.com (port 80)
I (17034) example_common: IPv6 DNS lookup
I (17074) example_common: - IPv6 DNS lookup failed, trying IPv4 lookup
I (17074) example_common: IPv4 DNS lookup
I (17074) example: DNS lookup succeeded. (2) IP=51.75.78.103
I (17084) example: ... allocated socket
I (17694) example: ... connected
I (17694) example: ... socket send success
I (17694) example: ... set socket receiving timeout success
HTTP/1.1 200 OK
Date: Sat, 24 Feb 2024 06:10:00 GMT
Server: Apache/2.4.25 (Debian)
Vary: Accept-Encoding
Connection: close
Content-Type: text/html; charset=UTF-8

220.240.255.134
I (18314) example: ... done reading from socket. Last read return=0 errno=128.
I (18314) example: 10...
```

### IPv4-only network connecting to dual-stack server

This shows an IPv4 only network (Shadow) connecting to a dual-stack server (`v4v6.ipv6-test.com`).

The connection attempt starts as soon as the IPv4 address is received, and as the device has no global IPv6 address, the DNS lookup and connection will only be IPv4 (`51.75.78.103`).

Note that, although the device has an IPv6 link-local address, it is ignored because it cannot be used to connect to a public address returned from DNS.

Although we only get a private IPv4 address, the assumption is that IPv4 DNS lookups are still usable, either for private DNS or via NAT44, e.g. we see the server responds with the NAT44 address `220.240.255.134`. (A similar assumption is made for IPv6 Unique Local Addresses)

```
I (764) example_connect: Connecting to Shadow...
I (764) example_connect: Waiting for IP(s)
I (3174) wifi:new:<1,0>, old:<1,0>, ap:<255,255>, sta:<1,0>, prof:1
I (3424) wifi:state: init -> auth (b0)
I (3524) wifi:state: auth -> assoc (0)
I (3554) wifi:state: assoc -> run (10)
I (3584) wifi:connected with Shadow, aid = 1, channel 1, BW20, bssid = 06:25:9c:13:92:ab
I (3584) wifi:security: WPA2-PSK, phy: bgn, rssi: -56
I (3584) wifi:pm start, type: 1

I (3594) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3634) wifi:dp: 2, bi: 102400, li: 4, scale listen interval from 307200 us to 409600 us
I (3634) wifi:AP's beacon interval = 102400 us, DTIM period = 2
I (5104) esp_netif_handlers: example_netif_sta ip: 192.168.5.146, mask: 255.255.255.0, gw: 192.168.5.1
I (5104) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.5.146
I (5114) example_common: Connected to example_netif_sta
I (5114) example_common: - IPv4 address: 192.168.5.146,
I (5124) example_common: - IPv6 address: fe80:0000:0000:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5134) example: DNS lookup v4v6.ipv6-test.com (port 80)
I (5144) example_common: IPv4 DNS lookup
I (5144) main_task: Returned from app_main()
I (5564) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fe80:0000:0000:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5584) example: DNS lookup succeeded. (2) IP=51.75.78.103
I (5584) example: ... allocated socket
I (5914) example: ... connected
I (5914) example: ... socket send success
I (5914) example: ... set socket receiving timeout success
HTTP/1.1 200 OK
Date: Sat, 24 Feb 2024 05:54:34 GMT
Server: Apache/2.4.25 (Debian)
Vary: Accept-Encoding
Connection: close
Content-Type: text/html; charset=UTF-8

220.240.255.134
I (6414) example: ... done reading from socket. Last read return=0 errno=128.
I (6424) example: 10... 
```

### IPv6-only network, with NAT64, connecting to an IPv4 server

This connects to an IPv6-only network (Wildspace), and waits to receive an IPv6 global scope address before starting the connection test. The default configurations waits for the first public usable address, either IPv4 or IPv6, so works in any network.

The DNS lookup for an IPv4-only server (`v4.ipv6-test.com`) returns a DNS64 address (`64:FF9B::334B:4E67`), which is then used for the HTTP connection.

Note that the server responds with the NAT64 public IPv4 address it sees for the client (`220.240.255.134`).

```
I (764) example_connect: Connecting to Wildspace...
I (764) example_connect: Waiting for IP(s)
I (3174) wifi:new:<11,0>, old:<1,0>, ap:<255,255>, sta:<11,0>, prof:1
I (3424) wifi:state: init -> auth (b0)
I (3444) wifi:state: auth -> assoc (0)
I (3464) wifi:state: assoc -> run (10)
I (3504) wifi:connected with Wildspace, aid = 1, channel 11, BW20, bssid = ea:63:da:bd:5a:09
I (3514) wifi:security: WPA2-PSK, phy: bgn, rssi: -80
I (3514) wifi:pm start, type: 1

I (3514) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3634) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (4564) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fe80:0000:0000:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5584) wifi:<ba-add>idx:0 (ifx:0, ea:63:da:bd:5a:09), tid:0, ssn:0, winSize:64
I (7564) example_connect: Got IPv6 event: Interface "example_netif_sta" address: 2407:8800:bc61:1300:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_GLOBAL
I (7564) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fd7c:e25e:67e8:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_UNIQUE_LOCAL
I (7574) example_common: Connected to example_netif_sta
I (7584) example_common: - IPv4 address: 0.0.0.0,
I (7594) example_common: - IPv6 address: fe80:0000:0000:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (7604) example_common: - IPv6 address: 2407:8800:bc61:1300:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_GLOBAL
I (7614) example_common: - IPv6 address: fd7c:e25e:67e8:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_UNIQUE_LOCAL
I (7624) example: DNS lookup v4.ipv6-test.com (port 80)
I (7624) example_common: IPv6 DNS lookup
I (7634) main_task: Returned from app_main()
I (8584) example: DNS lookup succeeded. (10) IP=64:FF9B::334B:4E67
I (8584) example: ... allocated socket
I (9064) example: ... connected
I (9064) example: ... socket send success
I (9064) example: ... set socket receiving timeout success
HTTP/1.1 200 OK
Date: Sat, 24 Feb 2024 05:45:33 GMT
Server: Apache/2.4.25 (Debian)
Vary: Accept-Encoding
Connection: close
Content-Type: text/html; charset=UTF-8

220.240.255.134
I (9474) example: ... done reading from socket. Last read return=0 errno=128.
I (9484) example: 10... 
```
