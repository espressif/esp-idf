| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# OpenThread Border Router Example

## Overview

This example demonstrates an [OpenThread border router](https://openthread.io/guides/border-router).

The ESP Thread Border Router SDK provides extra components and examples for putting the ESP Thread Border Router solution into production:

* [ESP Thread Border Router Docs](https://docs.espressif.com/projects/esp-thread-br)
* [ESP Thread Border Router Repo](https://github.com/espressif/esp-thread-br)

## How to use example

### Hardware Required
#### **Wi-Fi based Thread Border Router**
By default, two SoCs are required to run this example:
* An ESP32 series Wi-Fi SoC (ESP32, ESP32-C, ESP32-S, etc) loaded with this ot_br example.
* An IEEE 802.15.4 SoC (ESP32-H2) loaded with [ot_rcp](../ot_rcp) example.
* Another IEEE 802.15.4 SoC (ESP32-H2) loaded with [ot_cli](../ot_cli) example.

Connect the two SoCs via UART, below is an example setup with ESP32 DevKitC and ESP32-H2 DevKitC:
![thread_br](image/thread-border-router-esp32-esp32h2.jpg)

ESP32 pin | ESP32-H2 pin
----------|-------------
   GND    |      G
   GPIO4  |      TX
   GPIO5  |      RX

The example could also run on a single SoC which supports both Wi-Fi and Thread (e.g., ESP32-C6), but since there is only one RF path in ESP32-C6, which means Wi-Fi and Thread can't receive simultaneously, it has a significant impact on performance. Hence the two SoCs solution is recommended.

#### **Ethernet based Thread Border Router**
Similar to the previous Wi-Fi based Thread Border Route setup, but a device with Ethernet interface is required, such as [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-ethernet-kit.html)

### Configure the project

```
idf.py menuconfig
```
OpenThread Command Line is enabled with UART as the default interface. Additionally, USB JTAG is also supported and can be activated through the menuconfig:

```
Component config → ESP System Settings → Channel for console output → USB Serial/JTAG Controller
```

In order to run the example on single SoC which supports both Wi-Fi and Thread, the option `CONFIG_ESP_COEX_SW_COEXIST_ENABLE` and option `CONFIG_OPENTHREAD_RADIO_NATIVE` should be enabled. The two options are enabled by default for ESP32-C6 target.

Two ways are provided to setup the Thread Border Router in this example:

- Auto Start
Enable `OPENTHREAD_BR_AUTO_START`, configure the `CONFIG_EXAMPLE_WIFI_SSID` and `CONFIG_EXAMPLE_WIFI_PASSWORD` with your access point's ssid and psk.
The device will connect to Wi-Fi and form a Thread network automatically after bootup.

- Manual mode
Disable `OPENTHREAD_BR_AUTO_START` and enable `OPENTHREAD_CLI_ESP_EXTENSION`. `wifi` command will be added for connecting the device to the Wi-Fi network.

If the `CONFIG_EXAMPLE_CONNECT_ETHERNET` option is enabled, the device will connect to `Ethernet`, form a Thread network and act as a Ethernet based Thread Border Router.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```
If the `OPENTHREAD_BR_AUTO_START` option is enabled, The device will be connected to the configured Wi-Fi and Thread network automatically then act as the border router.

Otherwise, you need to manually configure the networks with CLI commands.

`wifi` command can be used to configure the Wi-Fi network.

```bash
> wifi
--wifi parameter---
connect
-s                   :     wifi ssid
-p                   :     wifi psk
---example---
join a wifi:
ssid: threadcertAP
psk: threadcertAP    :     wifi connect -s threadcertAP -p threadcertAP
state                :     get wifi state, disconnect or connect
---example---
get wifi state       :     wifi state
Done
```

To join a Wi-Fi network, please use the `wifi connect` command:

```bash
> wifi connect -s threadcertAP -p threadcertAP
ssid: threadcertAP
psk: threadcertAP
I (11331) wifi:wifi driver task: 3ffd06e4, prio:23, stack:6656, core=0
I (11331) system_api: Base MAC address is not set
I (11331) system_api: read default base MAC address from EFUSE
I (11341) wifi:wifi firmware version: 45c46a4
I (11341) wifi:wifi certification version: v7.0


..........

I (13741) esp_netif_handlers: sta ip: 192.168.3.10, mask: 255.255.255.0, gw: 192.168.3.1
W (13771) wifi:<ba-add>idx:0 (ifx:0, 02:0f:c1:32:3b:2b), tid:0, ssn:2, winSize:64
wifi sta is connected successfully
Done
```

To get the state of the Wi-Fi network:

```bash
> wifi state
connected
Done
```

For forming the Thread network, please refer to the [ot_cli_README](../ot_cli/README.md).

## Example Output

```bash
I (2729) esp_netif_handlers: example_connect: sta ip: 192.168.1.100, mask: 255.255.255.0, gw: 192.168.1.1
I (2729) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.1.100
I (3729) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:266f:28ff:fe80:2920, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (3729) example_connect: Connected to example_connect: sta
I (3739) example_connect: - IPv4 address: 192.168.1.100
I (3739) example_connect: - IPv6 address: fe80:0000:0000:0000:266f:28ff:fe80:2920, type: ESP_IP6_ADDR_IS_LINK_LOCAL

......


I(8139) OPENTHREAD:[INFO]-MLE-----: AttachState ParentReqReeds -> Idle
I(8139) OPENTHREAD:[NOTE]-MLE-----: Allocate router id 50
I(8139) OPENTHREAD:[NOTE]-MLE-----: RLOC16 fffe -> c800
I(8159) OPENTHREAD:[NOTE]-MLE-----: Role Detached -> Leader
```

## Bidirectional IPv6 connectivity

The border router will automatically publish the prefix and the route table rule to the Wi-Fi network via ICMPv6 router advertisement packages.

### Host configuration

The automatically configure your host's route table rules you need to set these sysctl options:

Please replace `wlan0` with the real name of your Wi-Fi network interface.
```
sudo sysctl -w net/ipv6/conf/wlan0/accept_ra=2
sudo sysctl -w net/ipv6/conf/wlan0/accept_ra_rt_info_max_plen=128
```

For mobile devices, the route table rules will be automatically configured after iOS 14 and Android 8.1.


### Testing IPv6 connectivity

Now in the Thread end device, check the IP addresses:

```
> ipaddr
fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5
fdde:ad00:beef:0:0:ff:fe00:c402
fdde:ad00:beef:0:ad4a:9a9a:3cd6:e423
fe80:0:0:0:f011:2951:569e:9c4a
```

You'll notice an IPv6 global prefix with only on address assigned under it. This is the routable address of this Thread node.
You can ping this address on your host:

``` bash
$ ping fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5
PING fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5(fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5) 56 data bytes
64 bytes from fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5: icmp_seq=1 ttl=63 time=459 ms
64 bytes from fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5: icmp_seq=2 ttl=63 time=109 ms
64 bytes from fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5: icmp_seq=3 ttl=63 time=119 ms
64 bytes from fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5: icmp_seq=4 ttl=63 time=117 ms
```

## Service discovery

The newly introduced service registration protocol([SRP](https://datatracker.ietf.org/doc/html/draft-ietf-dnssd-srp-10)) allows devices in the Thread network to register a service. The border router will forward the service to the Wi-Fi network via mDNS.

### Publish the service using SRP
Now we'll publish the service `my-service._test._udp` with hostname `test0` and port 12345

```
> srp client host name test0
Done
> srp client host address fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5
Done
> srp client service add my-service _test._udp 12345
Done
> srp client autostart enable
Done
```

This service will also become visible on the Wi-Fi network:

```bash
$ avahi-browse -r _test._udp -t

+ enp1s0 IPv6 my-service                                    _test._udp           local
= enp1s0 IPv6 my-service                                    _test._udp           local
   hostname = [test0.local]
   address = [fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5]
   port = [12345]
   txt = []
+ enp1s0 IPv4 my-service                                    _test._udp           local
= enp1s0 IPv4 my-service                                    _test._udp           local
   hostname = [test0.local]
   address = [fde6:75ff:def4:3bc3:9e9e:3ef:4245:28b5]
   port = [12345]
   txt = []
```

### Discovery delegate

First, the service `testhost._test._udp` need to be published using `avahi-publish-service` on the Wi-Fi network(for example Host).

```bash
$ avahi-publish-service testhost _test._udp 12345 test=1 dn="aabbbb"
```

Then get the border router's OMR prefix global unicast address(or ML-EID), and configure it on the Thread end device.

On the border router:
```
> ipaddr
fdde:ad00:beef:0:0:ff:fe00:fc10
fd9b:347f:93f7:1:1003:8f00:bcc1:3038
fdde:ad00:beef:0:0:ff:fe00:fc00
fdde:ad00:beef:0:0:ff:fe00:b800
fdde:ad00:beef:0:f891:287:866:776
fe80:0:0:0:77:bca6:6079:785b
Done
```

On the Thread end device:
```
> dns config fd9b:347f:93f7:1:1003:8f00:bcc1:3038
(or
> dns config fdde:ad00:beef:0:f891:287:866:776)
Done
```

Now the service published on the Host can be discovered on the Thread end device.
```
> dns resolve FA001208.default.service.arpa.
DNS response for FA001208.default.service.arpa. - fdde:ad00:beef:cafe:b939:26be:7516:b87e TTL:120
Done

> dns browse _test._udp.default.service.arpa.
DNS browse response for _test._udp.default.service.arpa.
testhost
    Port:5683, Priority:0, Weight:0, TTL:120
    Host:FA001208.default.service.arpa.
    HostAddress:fdde:ad00:beef:cafe:b939:26be:7516:b87e TTL:120
    TXT:[test=31, dn=616162626262] TTL:120
Done

> dns service testhost _test._udp.default.service.arpa.
DNS service resolution response for testhost for service _test._udp.default.service.arpa.
Port:5683, Priority:0, Weight:0, TTL:120
Host:FA001208.default.service.arpa.
HostAddress:fdde:ad00:beef:cafe:b939:26be:7516:b87e TTL:120
TXT:[test=31, dn=616162626262] TTL:120
Done
```
