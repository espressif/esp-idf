| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# HTTPS Request Example

Uses APIs from `esp-tls` component to make a very simple HTTPS request over a secure connection, including verifying the server TLS certificate.

(See the README.md file in the upper level 'examples' directory for more information about examples.)

### Session Tickets

Session Tickets, specified in [RFC 5077](https://datatracker.ietf.org/doc/html/rfc5077) are a mechanism to distribute encrypted
session-state information to the client in the form of a ticket and a mechanism to present the ticket back to the server.  The ticket is created by a TLS server and sent to a TLS client.  The TLS client presents the ticket to the TLS server to resume a session. In TLS 1.2, this speeds up handshakes from two to one round-trip.

In ESP-IDF, this feature is supported (for both server and client) when mbedTLS is used as the SSL library.

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

For additional logging of TLS and DNS, in project configuration:

* `Component config` -> `Log output` -> `Maximum log verbosity`, select `Debug`
* `Component config` -> `LWIP` -> `Enable LWIP Debug` -> `Enable DNS debug messages`, enable

#### Configuring Client Session Tickets

Note: This example has client session tickets enabled by default.

* Open the project configuration menu (`idf.py menuconfig`)
* In the `Component Config` -> `ESP-TLS` submenu, select the `Enable client session tickets` option.

Ensure that the server has the session tickets feature enabled.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Run on a dual-stack network with DNS64/NAT64 available, and with DNS logging turned on.

At the time of the first check the device has an IPv4 global address, but only link-local IPv6, so even though DNS64 returns both IPv4 and IPv6 addresses, the IPv4 address is used as it has a matching scope.

By the time of the second check, the device has received IPv6 RA (router advertisement) prefixes and configured IPv6 addresses. Both addresses have matching scopes and labels, so DNS precedence rules use the IPv6 NAT64 address.

```
I (843) example_connect: Connecting to Astral...
I (843) example_connect: Waiting for IP(s)
I (3263) wifi:new:<1,0>, old:<1,0>, ap:<255,255>, sta:<1,0>, prof:1
I (3513) wifi:state: init -> auth (b0)
I (3523) wifi:state: auth -> assoc (0)
I (3543) wifi:state: assoc -> run (10)
I (3553) wifi:connected with Astral, aid = 7, channel 1, BW20, bssid = c0:56:27:73:4b:14
I (3553) wifi:security: WPA2-PSK, phy: bgn, rssi: -58
I (3553) wifi:pm start, type: 1

I (3553) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
I (3583) wifi:dp: 2, bi: 102400, li: 4, scale listen interval from 307200 us to 409600 us
I (3583) wifi:AP's beacon interval = 102400 us, DTIM period = 2
I (4633) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fe80:0000:0000:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5063) esp_netif_handlers: example_netif_sta ip: 192.168.1.146, mask: 255.255.255.0, gw: 192.168.1.1
I (5063) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.1.146
I (5073) example_common: Connected to example_netif_sta
I (5073) example_common: - IPv4 address: 192.168.1.146,
I (5083) example_common: - IPv6 address: fe80:0000:0000:0000:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5093) example: Updating time from NVS
I (5103) example: Start https_request example
I (5103) example: https_request using crt bundle
D (5103) esp-tls: host:www.howsmyssl.com: strlen 17
dns_enqueue: "www.howsmyssl.com": use DNS entry 0
dns_enqueue: "www.howsmyssl.com": use DNS pcb 0
I (5123) main_task: Returned from app_main()
dns_send: dns_servers[0] "www.howsmyssl.com": request
sending DNS request ID 22389 for name "www.howsmyssl.com" to server 0
dns_recv: "www.howsmyssl.com": response = 64:ff9b:0:0:0:0:2247:2dc8
dns_enqueue: "www.howsmyssl.com": use DNS entry 1
dns_enqueue: "www.howsmyssl.com": use DNS pcb 0
dns_send: dns_servers[0] "www.howsmyssl.com": request
sending DNS request ID 50477 for name "www.howsmyssl.com" to server 0
dns_recv: "www.howsmyssl.com": response = 34.71.45.200
dns_select: selecting from 2 candidates
dns_select: precedence labels flags 0x0013, ipv6 scopes flags 0x0004, ipv4 scopes flags 0x4004
dns_select: rule 2, cand_0 scope (14) match 0, cand_1 scope (14) match 1
D (5233) esp-tls: [sock=54] Resolved IPv4 address: 34.71.45.200
D (5243) esp-tls: [sock=54] Connecting to server. HOST: www.howsmyssl.com/a/check, Port: 443
D (5643) esp-tls: handshake in progress...
I (6103) esp-x509-crt-bundle: Certificate validated
dns_tmr: dns_check_entries
dns_tmr: dns_check_entries
I (7383) example: Connection established...
I (7393) example: 106 bytes written
I (7393) example: Reading HTTP response...
I (7633) example_connect: Got IPv6 event: Interface "example_netif_sta" address: 2407:8800:bc61:1340:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_GLOBAL
I (7633) example_connect: Got IPv6 event: Interface "example_netif_sta" address: fd7c:e25e:67e8:0040:0a3a:f2ff:fe65:db28, type: ESP_IP6_ADDR_IS_UNIQUE_LOCAL
dns_tmr: dns_check_entries
HTTP/1.1 200 OK
Access-Control-Allow-Origin: *
Connection: close
Content-Length: 2545
Content-Type: application/json
Strict-Transport-Security: max-age=631138519; includeSubdomains; preload
Vary: Accept-Encoding
Date: Tue, 27 Feb 2024 22:11:22 GMT

{"given_cipher_suites":["TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384","TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384","TLS_ECDHE_ECDSA_WITH_AES_256_CCM","TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384","TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384","TLS_ECDHE_ECDSA_WITH_AES_256_
CBC_SHA","TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA","TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8","TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384","TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384","TLS_ECDHE_ECDSA_WITH_ARIA_256_CBC_SHA384","TLS_ECDHE_RSA_WITH_ARIA_256_CBC_SHA384","TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256","TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256","TLS_ECDHE_ECDSA_WITH_AES_128_CCM","TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256","TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256","TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA","TLS_ECDHE_RSA_WITH_AE
S_128_CBC_SHA","TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8","TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256","TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256","TLS_ECDHE_ECDSA_WITH
dns_tmr: dns_check_entries
_ARIA_128_CBC_SHA256","TLS_ECDHE_RSA_WITH_ARIA_128_CBC_SHA256","TLS_RSA_WITH_AES_256_GCM_SHA384","TLS_RSA_WITH_AES_256_CCM","TLS_RSA_WITH_AES_256_CBC_SHA256","TLS_RSA_WITH_AES_256_CBC_SHA","TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384","TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384","TLS_ECDH_RSA_WITH_AES_256_CBC_SHA","TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384","TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384","TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA","TLS_RSA_WITH_AES_256_CCM_8","TLS_ECDH_ECDSA_WITH_ARIA_256_GCM_SHA384","TLS_ECDH_RSA_WIT
H_ARIA_256_GCM_SHA384","TLS_RSA_WITH_ARIA_256_GCM_SHA384","TLS_ECDH_ECDSA_WITH_ARIA_256_CBC_SHA384","TLS_ECDH_RSA_WITH_ARIA_256_CBC_SHA384","TLS_RSA_WITH_ARIA_256_CBC_SHA384","TLS_RSA_WITH_AES_128_GCM_SHA256","TLS_RSA_WITH_AES_128_CCM","TLS_RSA_WITH_AES_128_CBC_SHA256","TLS_RSA_WITH_AES_128_CBC_SHA","TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256","TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256","TLS_ECDH_RSA_WITH_AES_128_CBC_SHA","TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256","TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256","TLS_ECDH_ECDS
A_WITH_AES_128_CBC_SHA","TLS_RSA_WITH_AES_128_CCM_8","TLS_ECDH_ECDSA_WITH_ARIA_128_GCM_SHA256","TLS_ECDH_RSA_WITH_ARIA_128_GCM_SHA256","TLS_RSA_WITH_ARIA_128_GCM_SHA256","TLS_ECDH_ECDSA_WITH_ARIA_128_CBC_SHA256","TLS_ECDH_RSA_WITH_ARIA_128_CBC_SHA256","TLS_RSA_WITH_ARIA_128_CBC_SHA256","TLS_EMPTY_RENEGOTIATION_INFO_SCSV"],"ephemeral_keys_supported":true,"session_ticket_supported":true,"tls_compression_supported":false,"unknown_cipher_suite_supported":false,"beast_vuln":false,"able_to_detect_n_minus_one_spl
itting":false,"insecure_cipher_suites":{},"tls_version":"TLS 1.2","rating":"Probably Okay"}
I (9463) example: connection closed
I (9473) example: 10...
dns_tmr: dns_check_entries
I (10473) example: 9...
dns_tmr: dns_check_entries
I (11473) example: 8...
dns_tmr: dns_check_entries
I (12473) example: 7...
dns_tmr: dns_check_entries
I (13473) example: 6...
dns_tmr: dns_check_entries
I (14473) example: 5...
dns_tmr: dns_check_entries
I (15473) example: 4...
dns_tmr: dns_check_entries
I (16473) example: 3...
dns_tmr: dns_check_entries
I (17473) example: 2...
dns_tmr: dns_check_entries
I (18473) example: 1...
dns_tmr: dns_check_entries
I (19473) example: 0...
dns_tmr: dns_check_entries
I (20473) example: Minimum free heap size: 181364 bytes
I (20473) example: https_request using cacert_buf
D (20473) esp-tls: host:www.howsmyssl.com: strlen 17
dns_lookup: "www.howsmyssl.com": found = 64:ff9b:0:0:0:0:2247:2dc8
dns_lookup: "www.howsmyssl.com": found = 34.71.45.200
dns_select: selecting from 2 candidates
dns_select: precedence labels flags 0x2013, ipv6 scopes flags 0x4004, ipv4 scopes flags 0x4004
dns_select: rule 2, cand_0 scope (14) match 1, cand_1 scope (14) match 1
dns_select: rule 5, cand_0 label (1) match 1, cand_1 label (4) match 1
dns_select: rule 6, cand_0 precedence 40, cand_1 precedence 35
D (20513) esp-tls: [sock=54] Resolved IPv6 address: 64:FF9B::2247:2DC8
D (20523) esp-tls: [sock=54] Connecting to server. HOST: www.howsmyssl.com/a/check, Port: 443
D (20903) esp-tls: handshake in progress...
dns_tmr: dns_check_entries
dns_tmr: dns_check_entries
I (22753) example: Connection established...
I (22753) example: 106 bytes written
I (22753) example: Reading HTTP response...
dns_tmr: dns_check_entries
HTTP/1.1 200 OK
Access-Control-Allow-Origin: *
Connection: close
Content-Length: 2545
Content-Type: application/json
Strict-Transport-Security: max-age=631138519; includeSubdomains; preload
Vary: Accept-Encoding
Date: Tue, 27 Feb 2024 22:11:37 GMT

{"given_cipher_suites":["TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384","TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384","TLS_ECDHE_ECDSA_WITH_AES_256_CCM","TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384","TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384","TLS_ECDHE_ECDSA_WITH_AES_256_
CBC_SHA","TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA","TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8","TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384","TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384","TLS_ECDHE_ECDSA_WITH_ARIA_256_CBC_SHA384","TLS_ECDHE_RSA_WITH_ARIA_256_CBC_SHA384","TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256","TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256","TLS_ECDHE_ECDSA_WITH_AES_128_CCM","TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256","TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256","TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA","TLS_ECDHE_RSA_WITH_AE
S_128_CBC_SHA","TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8","TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256","TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256","TLS_ECDHE_ECDSA_WITH
_ARIA_128_CBC_SHA256","TLS_ECDHE_RSA_WITH_ARIA_128_CBC_SHA256","TLS_RSA_WITH_AES_256_GCM_SHA384","TLS_RSA_WITH_AES_256_CCM","TLS_RSA_WITH_AES_256_CBC_SHA256","TLS_RSA_WITH_AES_256_CBC_SHA","TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384","TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384","TLS_ECDH_RSA_WITH_AES_256_CBC_SHA","TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384","TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384","TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA","TLS_RSA_WITH_AES_256_CCM_8","TLS_ECDH_ECDSA_WITH_ARIA_256_GCM_SHA384","TLS_ECDH_RSA_WIT
H_ARIA_256_GCM_SHA384","TLS_RSA_WITH_ARIA_256_GCM_SHA384","TLS_ECDH_ECDSA_WITH_ARIA_256_CBC_SHA384","TLS_ECDH_RSA_WITH_ARIA_256_CBC_SHA384","TLS_RSA_WITH_ARIA_256_CBC_SHA384","TLS_RSA_WITH_AES_128_GCM_SHA256","TLS_RSA_WITH_AES_128_CCM","TLS_RSA_WITH_AES_128_CBC_SHA256","TLS_RSA_WITH_AES_128_CBC_SHA","TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256","TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256","TLS_ECDH_RSA_WITH_AES_128_CBC_SHA","TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256","TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256","TLS_ECDH_ECDS
A_WITH_AES_128_CBC_SHA","TLS_RSA_WITH_AES_128_CCM_8","TLS_ECDH_ECDSA_WITH_ARIA_128_GCM_SHA256","TLS_ECDH_RSA_WITH_ARIA_128_GCM_SHA256","TLS_RSA_WITH_ARIA_128_GCM_SHA256","TLS_ECDH_ECDSA_WITH_ARIA_128_CBC_SHA256","TLS_ECDH_RSA_WITH_ARIA_128_CBC_SHA256","TLS_RSA_WITH_ARIA_128_CBC_SHA256","TLS_EMPTY_RENEGOTIATION_INFO_SCSV"],"ephemeral_keys_supported":true,"session_ticket_supported":true,"tls_compression_supported":false,"unknown_cipher_suite_supported":false,"beast_vuln":false,"able_to_detect_n_minus_one_spl
itting":false,"insecure_cipher_suites":{},"tls_version":"TLS 1.2","rating":"Probably Okay"}
I (23393) example: connection closed
I (23393) example: 10...
dns_tmr: dns_check_entries
I (24393) example: 9...
```
