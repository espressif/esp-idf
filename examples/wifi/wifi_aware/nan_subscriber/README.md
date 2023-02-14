| Supported Targets | ESP32 | ESP32-S2 |
| ----------------- | ----- | -------- |

# NAN Subscriber Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Introduction
Neighbor Awareness Networking (NAN) is a protocol that allows Wi-Fi devices to discover services in their proximity. NAN uses direct device-to-device communication and does not require any Internet or AP connection.
Multiple NAN devices which are in the vicinity will form a NAN cluster which allows them to communicate with each other. Devices within a NAN cluster can advertise or look for services using NAN Service Discovery protocols.
A Publisher advertises a service and a Subscriber searches for a service. A subscriber either listens passively (Passive mode) or sends out broadcast Action frames (Active mode). Similarly a publisher either broadcasts its services (Unsolicited mode) or replies to Active subscribers (Solicited Mode). Matching of services is done by service name and optionally matching filters.

## How to use example
Publishing and Subscribing is possible dynamically on the NAN Interface. For simplicity this example demonstrates only the Subscribe method with some configuration options. Use option `Example Configuration -> NAN Service Name` for naming a service to subscribe to. Select `Subscribe Type` and optionally set the `Matching Filter`. Using option `Communication Type` it's possible to either send a message using Follow-up or initiate a datapath with the Publisher. The device will search for another device in the vicinity Publishing the same service. A service match happens when the service name (case insensitive) matches as well as the Matching filter(s) matche(s) (case sensitive). After this the Subscriber will initiate further communication as par configuration.

## Example Output

Usecase 1 : Subscriber sending a Follow-up to the Publisher
```
I (449) phy_init: phy_version 1800,e7ef680,Apr 13 2021,11:45:08
I (549) wifi:mode : NAN (18:fe:34:72:50:b1)
Set Vif NAN
I (549) NAN: State Init => Scanning

I (3549) NAN: Join Cluster with Id 50:6f:9a:01:cc:4d
I (3549) wifi:Init max length of beacon: 752/752
I (3549) wifi:Init max length of beacon: 752/752
I (3559) NAN: State Scanning => AnchorMaster

I (3762) nan_app: NAN Discovery started.
I (4222) nan_sub: NAN Publisher found for Serv ID 5
I (4222) nan_app: Service matched with e0:e2:e6:7b:c1:32 [Peer Publish id - 5]
I (4222) nan_sub: Sending message 'Hello' to Publisher e0:e2:e6:7b:c1:32 ...
I (4762) nan_app: Received message 'Welcome' from Peer e0:e2:e6:7b:c1:32 [Peer Service id - 5]
```

Usecase 2 : Subscriber setting up Datapath with the Publisher, then testing IPv6 ping
```
I (452) phy_init: phy_version 1800,e7ef680,Apr 13 2021,11:45:08
I (552) wifi:mode : NAN (18:fe:34:72:50:b1)
Set Vif NAN
I (552) NAN: State Init => Scanning

I (3552) NAN: Join Cluster with Id 50:6f:9a:01:cc:4d
I (3562) wifi:Init max length of beacon: 752/752
I (3562) wifi:Init max length of beacon: 752/752
I (3562) NAN: State Scanning => AnchorMaster

I (3562) nan_sub: NAN Discovery started.
I (3572) nan_sub: Subscribing to Service test with ID 5
I (3612) nan_sub: NAN Publisher found for Serv ID 5
E (3612) wifi:Committed slots 0x7fff0000 for Peer 18:fe:34:72:50:ca on Channel 6
I (3622) nan_sub: Datapath setup with 18:fe:34:72:50:ca
I (5452) nan_sub: Got IPv6 event: Interface "nan_sub: nan" address: fe80:0000:0000:0000:1afe:34ff:fe72:50b3, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5452) nan_sub: Pinging Peer with IPv6 addr FE80::1AFE:34FF:FE72:50CA
W (5722) wifi:<ba-add>idx:5 (ifx:2, 18:fe:34:72:50:ca), tid:0, ssn:0, winSize:64
I (5742) nan_sub: 64 bytes from FE80::1AFE:34FF:FE72:50CA icmp_seq=1 ttl=0 time=278 ms
I (7462) nan_sub: 64 bytes from FE80::1AFE:34FF:FE72:50CA icmp_seq=2 ttl=0 time=2 ms
I (7462) nan_sub: 64 bytes from FE80::1AFE:34FF:FE72:50CA icmp_seq=3 ttl=0 time=2 ms
I (8462) nan_sub: 64 bytes from FE80::1AFE:34FF:FE72:50CA icmp_seq=4 ttl=0 time=1 ms
I (9462) nan_sub: 64 bytes from FE80::1AFE:34FF:FE72:50CA icmp_seq=5 ttl=0 time=1 ms
I (10462) nan_sub:
--- FE80::1AFE:34FF:FE72:50CA ping statistics ---
I (10462) nan_sub: 5 packets transmitted, 4 received, 19% packet loss, time 1281ms
```
