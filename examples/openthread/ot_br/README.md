# OpenThread Border Router Example

## Overview

This example demonstrates an [OpenThread border router](https://openthread.io/guides/border-router).

## How to use example

### Hardware Required

The following SoCs are required to run this example:
* An ESP32 series Wi-Fi SoC (ESP32, ESP32-C, ESP32-S, etc) loaded with this ot_br example.
* An ESP32-H2 802.15.4 SoC loaded with [ot_rcp](../ot_rcp) example.
* Another ESP32-H2 SoC loaded with [ot_cli](../ot_cli) example. Enable `OPENTHREAD_JOINER` option in menuconfig before compiling the example.

Connect the two SoCs via UART, below is an example setup with ESP32 DevKitC and ESP32-H2 DevKitC:
![thread_br](image/thread-border-router-esp32-esp32h2.jpg)

ESP32 pin | ESP32-H2 pin
----------|-------------
   GND    |      G 
   GPIO4  |      TX      
   GPIO5  |      RX

### Configure the project

```
idf.py menuconfig
```

You need to configure the `CONFIG_EXAMPLE_WIFI_SSID` and `CONFIG_EXAMPLE_WIFI_PASSWORD` with your access point's ssid and psk.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

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

The device will automatically connect to the configured Wi-Fi and Thread network and act as the border router.

## Using the border agent feature

You need to ot-commissioner on the host machine and another Thread end device running OpenThread cli.

You can find the guide to build and run ot-commissioner [here](https://openthread.io/guides/commissioner/build).

Make sure to configure the same PSKc as the one in sdkconfig in ot-commisioner's config file `non-ccm-config.json`

### Connect the commissioner to the border router

Note that the target address `192.168.1.100` shall match the actual Wi-Fi IP address of the device. `49154` is a port number used by the OT commissioner.

``` bash
$ commissioner-cli /usr/local/etc/commissioner/non-ccm-config.json
> start 192.168.1.100 49154
[done]
> active
true
[done]
```

You can also verify the commissioner connection from the border router's log:

```
I(59709) OPENTHREAD:[INFO]-MESH-CP-: DTLS started
I(65469) OPENTHREAD:[INFO]-MESH-CP-: Commissioner connected
I(65479) OPENTHREAD:[INFO]-MESH-CP-: Forwarded request to leader on c/lp
I(65489) OPENTHREAD:[INFO]-MESH-CP-: received petition
I(65489) OPENTHREAD:[INFO]-MESH-CP-: sent petition response
I(65489) OPENTHREAD:[INFO]-MESH-CP-: commissioner accepted: session ID=3077, ALOC=fd04:b642:9ba9:fcdc:0:ff:fe00:fc35
I(65499) OPENTHREAD:[INFO]-MESH-CP-: Sent to commissioner
I(65509) OPENTHREAD:[INFO]-CORE----: Notifier: StateChanged (0x00000201) [Ip6+ NetData]
I(65529) OPENTHREAD:[INFO]-BBR-----: PBBR state: None
I(65539) OPENTHREAD:[INFO]-BBR-----: Domain Prefix: ::/0, state: None
I(65559) OPENTHREAD:[INFO]-MESH-CP-: Forwarded request to leader on c/ag
W(65559) OPENTHREAD:[WARN]-MESH-CP-: Failed to notify commissioner on ProxyRx (c/ur): DestinationAddressFiltered
I(65579) OPENTHREAD:[INFO]-MESH-CP-: sent active dataset get response to fd04:b642:9ba9:fcdc:0:ff:fe00:c800
W(65579) OPENTHREAD:[WARN]-MESH-CP-: Failed to notify commissioner on ProxyRx (c/ur): DestinationAddressFiltered
I(65589) OPENTHREAD:[INFO]-MESH-CP-: Sent to commissioner
I(65629) OPENTHREAD:[INFO]-MESH-CP-: Forwarded request to leader on c/ag
W(65629) OPENTHREAD:[WARN]-MESH-CP-: Failed to notify commissioner on ProxyRx (c/ur): DestinationAddressFiltered
I(65649) OPENTHREAD:[INFO]-MESH-CP-: sent active dataset get response to fd04:b642:9ba9:fcdc:0:ff:fe00:c800
W(65649) OPENTHREAD:[WARN]-MESH-CP-: Failed to notify commissioner on ProxyRx (c/ur): DestinationAddressFiltered
I(65659) OPENTHREAD:[INFO]-MESH-CP-: Sent to commissioner
I(65689) OPENTHREAD:[INFO]-MESH-CP-: Proxy transmit sent to fd04:b642:9ba9:fcdc:0:ff:fe00:fc00
W(65689) OPENTHREAD:[WARN]-MESH-CP-: Failed to notify commissioner on ProxyRx (c/ur): DestinationAddressFiltered
I(65699) OPENTHREAD:[INFO]-MESH-CP-: sent pending dataset get response to fd04:b642:9ba9:fcdc:0:ff:fe00:fc35
I(65709) OPENTHREAD:[INFO]-MESH-CP-: Sent to commissioner on c/ur
I(65749) OPENTHREAD:[INFO]-MESH-CP-: Proxy transmit sent to fd04:b642:9ba9:fcdc:0:ff:fe00:fc00
W(65749) OPENTHREAD:[WARN]-MESH-CP-: Failed to notify commissioner on ProxyRx (c/ur): DestinationAddressFiltered
I(65759) OPENTHREAD:[INFO]-MESH-CP-: sent commissioning dataset set response
I(65769) OPENTHREAD:[INFO]-MESH-CP-: Sent to commissioner on c/ur
I(65769) OPENTHREAD:[INFO]-CORE----: Notifier: StateChanged (0x00000200) [NetData]
I(65789) OPENTHREAD:[INFO]-BBR-----: PBBR state: None

```

### Commission the joiner

In the OT commissioner cli, run:
``` bash
> joiner enableall meshcop J01NU5
[done]
>
```

In the joining device's cli, run:

```bash
> ifconfig up
Done
> joiner start J01NU5
Done
> Join success!
> thread start
Done
```

You can also find these log lines in the border router:

```
I(531219) OPENTHREAD:[INFO]-MESH-CP-: Received relay transmit
I(531229) OPENTHREAD:[INFO]-MESH-CP-: Received kek
I(531279) OPENTHREAD:[INFO]-MAC-----: Sent IPv6 UDP msg, len:85, chksum:14a0, to:92335c4b320830fb, sec:no, prio:net
I(531279) OPENTHREAD:[INFO]-MAC-----:     src:[fe80:0:0:0:ac2f:720a:6fe4:c837]:1000
I(531289) OPENTHREAD:[INFO]-MAC-----:     dst:[fe80:0:0:0:9033:5c4b:3208:30fb]:1000
I(531299) OPENTHREAD:[INFO]-MESH-CP-: Sending JOIN_ENT.ntf
I(531299) OPENTHREAD:[INFO]-MESH-CP-: Sent joiner entrust length = 161

......

I(552699) OPENTHREAD:[INFO]-MLE-----: Receive Child ID Request (fe80:0:0:0:8434:c5ec:fe9f:c088)
I(552729) OPENTHREAD:[INFO]-CORE----: [settings] Added ChildInfo {rloc:0xc801, extaddr:8634c5ecfe9fc088, timeout:240, mode:0x0f, version:3}
I(552729) OPENTHREAD:[INFO]-MLE-----: Send Child ID Response (fe80:0:0:0:8434:c5ec:fe9f:c088,0xc801)
I(552739) OPENTHREAD:[INFO]-CORE----: Notifier: StateChanged (0x00000400) [Child+]
I(552749) OPENTHREAD:[INFO]-UTIL----: Starting Child Supervision
```

The device has now joined the same Thread network based on the key set by the commissioner.

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

Now in the joining device, check the IP addresses:

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
