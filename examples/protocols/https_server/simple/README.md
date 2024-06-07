| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# HTTP server with SSL support using OpenSSL

This example creates a SSL server that returns a simple HTML page when you visit its root URL.

See the `esp_https_server` component documentation for details.

## How to use example
Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Configure the project

```
idf.py menuconfig
```
Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Certificates

You will need to approve a security exception in your browser. This is because of a self signed
certificate; this will be always the case, unless you preload the CA root into your browser/system
as trusted.

You can generate a new certificate using the OpenSSL command line tool:

```
openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -x509 -days 3650 -out cacert.pem -subj "/CN=ESP32 HTTPS server example"
```

Expiry time and metadata fields can be adjusted in the invocation.

Please see the openssl man pages (man openssl-req) for more details.

It is **strongly recommended** to not reuse the example certificate in your application;
it is included only for demonstration.

## Example Output

```
I (8596) example: Starting server
I (8596) esp_https_server: Starting server
I (8596) esp_https_server: Server listening on port 443
I (8596) example: Registering URI handlers
I (8606) esp_netif_handlers: example_connect: sta ip: 192.168.194.219, mask: 255.255.255.0, gw: 192.168.194.27
I (8616) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.194.219
I (9596) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:266f:28ff:fe80:2c74, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (9596) example_connect: Connected to example_connect: sta
W (9606) wifi:<ba-add>idx:0 (ifx:0, ee:6d:19:60:f6:0e), tid:0, ssn:2, winSize:64
I (9616) example_connect: - IPv4 address: 192.168.194.219
I (9616) example_connect: - IPv6 address: fe80:0000:0000:0000:266f:28ff:fe80:2c74, type: ESP_IP6_ADDR_IS_LINK_LOCAL
W (14426) wifi:<ba-add>idx:1 (ifx:0, ee:6d:19:60:f6:0e), tid:4, ssn:0, winSize:64
I (84896) esp_https_server: performing session handshake
```
