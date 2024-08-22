| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

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

```
I (5634) example_connect: - IPv4 address: 192.168.194.219
I (5634) example_connect: - IPv6 address: fe80:0000:0000:0000:266f:28ff:fe80:2c74, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5644) example: Start https_request example
I (5654) example: https_request using crt bundle
W (6514) wifi:<ba-add>idx:1 (ifx:0, ee:6d:19:60:f6:0e), tid:4, ssn:0, winSize:64
I (7074) esp-x509-crt-bundle: Certificate validated
I (9384) example: Connection established...
I (9384) example: 107 bytes written
I (9384) example: Reading HTTP response...
HTTP/1.1 200 OK
Content-Length: 2091
Access-Control-Allow-Origin: *
Connection: close
Content-Type: application/json
Date: Tue, 07 Sep 2021 08:30:00 GMT
Strict-Transport-Security: max-age=631138519; includeSubdomains; preload

{"given_cipher_suites":["TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384","TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384","TLS_DHE_RSA_WITH_AES_256_GCM_SHA384","TLS_ECDHE_ECDSA_WITH_AES_256_CCM","TLS_DHE_RSA_WITH_AES_256_CCM","TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384","TLS_ECDHE_RSA_WITH_AES_
256_CBC_SHA384","TLS_DHE_RSA_WITH_AES_256_CBC_SHA256","TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA","TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA","TLS_DHE_RSA_WITH_AES_256_CBC_SHA","TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8","TLS_DHE_RSA_WITH_AES_256_CCM_8","TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256","TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256","TLS_DHE_RSA_WITH_AES_128_GCM_SHA256","TLS_ECDHE_ECDSA_WITH_AES_128_CCM","TLS_DHE_RSA_WITH_AES_128_CCM","TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256","TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256","TLS_DHE
_RSA_WITH_AES_128_CBC_SHA256","TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA","TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA","TLS_DHE_RSA_WITH_AES_128_CBC_SHA","TLS_ECDHE_ECDSA
_WITH_AES_128_CCM_8","TLS_DHE_RSA_WITH_AES_128_CCM_8","TLS_RSA_WITH_AES_256_GCM_SHA384","TLS_RSA_WITH_AES_256_CCM","TLS_RSA_WITH_AES_256_CBC_SHA256","TLS_RSA_WITH_AES_256_CBC_SHA","TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384","TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384","TLS_ECDH_RSA_WITH_AES_256_CBC_SHA","TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384","TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384","TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA","TLS_RSA_WITH_AES_256_CCM_8","TLS_RSA_WITH_AES_128_GCM_SHA256","TLS_RSA_WITH_AES_128_CCM","TLS_RS
A_WITH_AES_128_CBC_SHA256","TLS_RSA_WITH_AES_128_CBC_SHA","TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256","TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256","TLS_ECDH_RSA_WITH_AES_128_CBC_SHA","TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256","TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256","TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA","TLS_RSA_WITH_AES_128_CCM_8","TLS_EMPTY_RENEGOTIATION_INFO_SCSV"],"ephemeral_keys_supported":true,"session_ticket_supported":true,"tls_compression_supported":false,"unknown_cipher_suite_supported":false,"beast_vuln":fal
se,"able_to_detect_n_minus_one_splitting":false,"insecure_cipher_suites":{},"tls_version":"TLS 1.2","rating":"Probably Okay"}
I (10204) example: connection closed
I (10204) example: 10...
I (11204) example: 9...
I (12204) example: 8...
I (13204) example: 7...
I (14204) example: 6...
I (15204) example: 5...
I (16204) example: 4...
```
