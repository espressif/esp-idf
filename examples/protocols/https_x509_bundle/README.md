| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# HTTPS x509 Bundle Example

This example shows how to use the ESP certificate bundle utility to embed a bundle of x509 certificates and use them to
establish a simple HTTPS connection over a secure connection. The path of the certificates are specified using menuconfig.

See the README.md file in the upper level 'examples' directory for more information about examples.

## Example workflow
- ESP TLS is initialized with the certificate bundle option enabled.
- The application loops through the given URLs, establishing a secure TLS connection to all of them, verifying the server certificate included.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../README.md) for more details.
* If using a different folder than `certs` for storing certificates then update `Custom Certificate Bundle Path` under `Component config` - `mbedTLS` - `Certificate Bundle`

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
```
I (0) cpu_start: Starting scheduler on APP CPU.
I (491) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (4051) example_connect: Ethernet Link Up
I (5971) esp_netif_handlers: eth ip: 192.168.2.137, mask: 255.255.255.0, gw: 192.168.2.2
I (5971) example_connect: Connected to Ethernet
I (5971) example_connect: IPv4 address: 192.168.2.137
I (5971) example_connect: IPv6 address: fe80:0000:0000:0000:bedd:c2ff:fed4:a92b
I (5981) example: Connecting to 2 URLs
I (7100) esp-x509-crt-bundle: Certificate validated
I (8371) example: Connection established to https://www.howsmyssl.com/a/check
I (11730) esp-x509-crt-bundle: Certificate validated
I (11821) example: Connection established to https://espressif.com
I (12821) example: Completed 2 connections
I (12821) example: Starting over again...
```
