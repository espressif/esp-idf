| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |


# ICMPv6 Ping example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The application first finds a network interface with an IPv6 address of **Global** or **Unique Local** type.
The application then creates a raw ICMPv6 socket and sends an ICMPv6 Echo Request to the destination ipv6 address, with the ipv6 address previously found over the interface as the source address. The application keeps waiting for an echo reply from the target destination.

## Hardware Required

This example can be run on any commonly available ESP32 development board.

## How to use example
### Build and execute for **ESP32** targets:

Set the target as:
```sh
idf.py set-target esp32  
```
#### Configure the project:
Set the following parameters under "***Example Connection Configuration***" Options:
* Set `WiFi SSID` of your IPv6 enabled WiFi network.
    Ensure the WiFi network supports SLAAC.
* Set `WiFi Password` of the selected Wifi network.
* Press '**`Esc`**' to go to the previous menu.

Set the following parameters under "***Example Configuration***" Options:
* Set the '**Destination IPV6 Address**' for the ICMPv6 ping example.
  **Destination IPV6 Address** can also be set manually by modifying the source file `icmpv6_ping.c`.
* Press '**`s`**' to save and '**`q`**' to quit the menu.

#### Build and execute as follows:
```sh
idf.py build 
idf.py flash monitor -p <COM PORT>
```

The output of the execution should be as follows:
```sh
I (7668) ICMPv6_PING: Interface: st1
I (7668) ICMPv6_PING: IPv6 address: fe80:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (7678) ICMPv6_PING: IPv6 address: 2001:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx, type: ESP_IP6_ADDR_IS_GLOBAL
I (7688) ICMPv6_PING: Source address: 2001:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
I (7698) ICMPv6_PING: Destination address: 2001:xxxx::xxxx:xxxx:xxxx:xxxx
I (7708) ICMPv6_PING: Interface name: st1
I (7708) ICMPv6_PING: ICMPv6 msg payload:
I (7708) ICMPv6_PING: 0x3ffc6e84   80 00 00 00 00 00 01 2c  54 65 73 74              |.......,Test|
I (7718) ICMPv6_PING: Sent ICMPv6 msg: type: 128, code: 0, id: 0, seqno: 300

I (7828) ICMPv6_PING: ICMPv6 msg payload:
I (7828) ICMPv6_PING: 0x3ffc6e5c   60 00 00 00 00 20 3a ff  20 xx xx xx xx xx xx xx  |`.... :. .......|
I (7838) ICMPv6_PING: 0x3ffc6e6c   xx xx xx xx xx xx xx xx  20 01 xx xx xx xx xx xx  |V.....!. .......|
I (7848) ICMPv6_PING: 0x3ffc6e7c   xx xx xx xx xx xx xx xx  88 00 1f f0 e0 00 00 00  |Z.%...A.........|
I (7858) ICMPv6_PING: 0x3ffc6e8c   20 01 0d b8                                       | ...|
I (7868) ICMPv6_PING: Received ICMPv6 msg: type: 136, code: 0, id: 224, seqno: 0

I (7868) ICMPv6_PING: ICMPv6 msg payload:
I (7878) ICMPv6_PING: 0x3ffc6e5c   60 00 00 00 00 0c 3a 40  20 xx xx xx xx xx xx xx  |`.... :. .......|
I (7838) ICMPv6_PING: 0x3ffc6e6c   xx xx xx xx xx xx xx xx  20 01 xx xx xx xx xx xx  |V.....!. .......|
I (7848) ICMPv6_PING: 0x3ffc6e7c   xx xx xx xx xx xx xx xx  81 00 8a a9 00 00 01 2c  |Z.%...A........,|
I (7908) ICMPv6_PING: 0x3ffc6e8c   54 65 73 74                                       |Test|
I (7918) ICMPv6_PING: Received ICMPv6 msg: type: 129, code: 0, id: 0, seqno: 300
```

## Troubleshooting

Set the destination IPv6 address to your PC and run the following command on a Linux terminal on your PC to see the request and reply.

```sh
sudo tcpdump -XX -i enp0s1 "icmp6 && ip6[40] == 128 || ip6[40] == 129" -vvv
```

## Note:
According to [RFC 2460 section 8.1](https://datatracker.ietf.org/doc/html/rfc2460#section-8.1), any transport or other upper-layer protocol that includes the
addresses from the IP header in its checksum computation must be modified for use over IPv6, to include the 128-bit IPv6 addresses instead of 32-bit IPv4 addresses.
ICMPv6 is one of such protocols, and the responsibility of checksum calculation is supposed to be part of the ICMPv6 header building process.
But LWIP does this for the user. LWIP expects the checksum field to be set to zero by the user, and it calculates and sets the ICMPv6 checksum value before transmitting the packet.
