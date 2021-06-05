# OpenSSL Client Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to set up esp openssl client and communicate over ssl transport layer.

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
* Configure the openssl client endpoint URI under "Example Configuration", if "OPENSSL_CLIENT_URI_FROM_STDIN" is selected then the example application will connect to the URI it reads from stdin (used for testing)

* When using Make build system, set `Default serial port` under `Serial flasher config`.

* When using OPENSSL_CLIENT_URI_FROM_STRING configure target domain and port number under "Example Configuration"

* Please note that verification mode is VERIFY_PEER by default, that's why during connection to public host('www.baidu.com') it's needed to use 
  appropriate certificates('baidu_ca.crt'), or it is needed to change verify mode to VERIFY_NONE.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (2601) esp_netif_handlers: example_connect: sta ip: 192.168.1.191, mask: 255.255.255.0, gw: 192.168.1.1
I (2601) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.1.191
I (3601) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:260a:c4ff:fee7:a660, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (3601) example_connect: Connected to example_connect: sta
I (3611) example_connect: - IPv4 address: 192.168.1.191
I (3611) example_connect: - IPv6 address: fe80:0000:0000:0000:260a:c4ff:fee7:a660, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (3631) openssl_example: Test started
I (3631) openssl_example: Trying connect to www.baidu.com port 443 ...
I (3641) openssl_example: DNS lookup succeeded. IP=103.235.46.39
I (4101) openssl_example: OK
I (4101) openssl_example: Create SSL obj
I (4101) openssl_example: OK
I (4101) openssl_example: SSL verify mode = 0 connected to www.baidu.com port 443 ...
I (8091) openssl_example: OK
I (8091) openssl_example: SSL Connection Succeed

```
