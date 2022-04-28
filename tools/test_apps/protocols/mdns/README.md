| Supported Targets | ESP32 | ESP32-S2 | ESP32-C3 |
| ----------------- | ----- | -------- | -------- |

# mDNS test project

Main purpose of this application is to test the mDNS library to correctly advertise, lookup services and hosts.
The "app_test.py" logically separated from two sets of test cases 1. "ESP32 board sends queries -> Host sends answers" and 2. "Host sends queries" -> "ESP32 board sends answers".
Two first sets of test cases are starting by 'test_query_' and the second ones by 'start_case'.

## Runtime settings

1. For first sets of test cases python creates and sends dns queries using "dpkt" library
2. For the second sets of test cases this app waits for user input to provide test case(e.g. CONFIG_TEST_QUERY_HOST, CONFIG_TEST_QUERY_HOST_ASYNC or CONFIG_TEST_QUERY_SERVICE)
In order to run both of them just needed to set up the project and run by 'python app_test.py'

## Test app workflow

- mDNS is initialized with hostname and instance name defined through the project configuration and `_http._tcp` service is added to be advertised
- A delegated host `esp32-delegated._local` is added and another `_http._tcp` service is added for this host.
- WiFi STA is started and tries to connect to the access point defined through the project configuration

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)

* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](README.md) for more details.
* Set `mDNS Hostname` as host name prefix for the device and its instance name in `mDNS Instance Name`

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

```
idf.py -p PORT flash monitor
```

- Wait for WiFi to connect to your access point
- You can now ping the device at `[board-hostname].local`, where `[board-hostname]` is preconfigured hostname, `esp32-mdns` by default.
- You can also browse for `_http._tcp` on the same network to find the advertised service
- Note that for purpose of CI tests, configuration options of `MDNS_RESOLVE_TEST_SERVICES` and `MDNS_ADD_MAC_TO_HOSTNAME` are available, but disabled by default. If enabled, then the hostname suffix of last 3 bytes from device MAC address is added, e.g. `esp32-mdns-80FFFF`, and a query for test service is issued.


(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Hardware Required
This test-app can be executed on any ESP32 board, the only required interface is WiFi and connection to a local network and tls server.
