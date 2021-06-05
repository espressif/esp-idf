# OpenSSL Server Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example connects to the ESP-OPENSSL server demo using ssl transport and and sends some messages.

## How to use example

### Python scripts

Script example_test.py could be used as a client part to the ESP-OPENSSL server demo,

```
python example_test.py
```
Note that this script is used in automated tests, as well, so the IDF test framework packages need to be imported;
please add `$IDF_PATH/tools/ci/python_packages` to `PYTHONPATH`.

### Hardware Required

This example can be executed on any ESP32 board, the only required interface is WiFi and connection to internet.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.
* When using Make build system, set `Default serial port` under `Serial flasher config`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (2609) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:260a:c4ff:fee7:a660, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (3609) esp_netif_handlers: example_connect: sta ip: 192.168.1.191, mask: 255.255.255.0, gw: 192.168.1.1
I (3609) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.1.191
I (3619) example_connect: Connected to example_connect: sta
I (3619) example_connect: - IPv4 address: 192.168.1.191
I (3629) example_connect: - IPv6 address: fe80:0000:0000:0000:260a:c4ff:fee7:a660, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (3639) OPENSSL_EXAMPLE: SSL server context create ......
I (3649) OPENSSL_EXAMPLE: OK
I (3649) OPENSSL_EXAMPLE: SSL server context set own certification......
I (3659) OPENSSL_EXAMPLE: OK
I (3659) OPENSSL_EXAMPLE: SSL server context set private key......
I (3669) OPENSSL_EXAMPLE: OK
I (3669) OPENSSL_EXAMPLE: SSL server create socket ......
I (3679) OPENSSL_EXAMPLE: OK
I (3679) OPENSSL_EXAMPLE: SSL server socket bind ......
I (3689) OPENSSL_EXAMPLE: OK
I (3689) OPENSSL_EXAMPLE: SSL server socket listen on 443 port
I (3699) OPENSSL_EXAMPLE: OK
I (3699) OPENSSL_EXAMPLE: SSL server create ......
I (3709) OPENSSL_EXAMPLE: OK
I (3709) OPENSSL_EXAMPLE: SSL server socket accept client ......
```
