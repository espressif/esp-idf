# OpenThread command line example

## Overview

This example demonstrates an [OpenThread border router](https://openthread.io/guides/border-router).

## How to use example

### Hardware connection

To run this example, it's used to use an DevKit C board and connect PIN4 and PIN5 to the UART TX and RX port of another 15.4 capable radio co-processor ([RCP](https://openthread.io/platforms/co-processor?hl=en))

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

The device will automatically connect to the configured WiFi and Thread network and act as the border router.

## Using the border agent feature

You need to ot-commissioner on the host machine and another Thread end device running OpenThread cli.

You can find the guide to build and run ot-commissioner [here](https://openthread.io/guides/commissioner/build).

Make sure to configure the same PSKc as the one in sdkconfig in ot-commisioner's config file `non-ccm-config.json`

### Connect the commissioner to the border router

Note that the target address `192.168.1.100` shall match the actual WiFi IP address of the device.

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
