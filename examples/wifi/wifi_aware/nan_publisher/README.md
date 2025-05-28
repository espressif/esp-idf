| Supported Targets | ESP32 | ESP32-C5 | ESP32-S2 |
| ----------------- | ----- | -------- | -------- |

# NAN Publisher Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Introduction
Neighbor Awareness Networking (NAN) is a protocol that allows Wi-Fi devices to discover services in their proximity. NAN uses direct device-to-device communication and does not require any Internet or AP connection.
Multiple NAN devices which are in the vicinity will form a NAN cluster which allows them to communicate with each other. Devices within a NAN cluster can advertise or look for services using NAN Service Discovery protocols.
A Publisher advertises a service and a Subscriber searches for a service. A subscriber either listens passively (Passive mode) or sends out broadcast Action frames (Active mode). Similarly a publisher either broadcasts its services (Unsolicited mode) or replies to Active subscribers (Solicited Mode). Matching of services is done by service name and optionally matching filters.

## How to use example
Publishing and Subscribing is possible dynamically on the NAN Interface. For simplicity this example demonstrates only the Publish method with some configuration options. Use option `Example Configuration -> NAN Service Name` for naming the Publish service. Select `Publish Type` and optionally set the `Matching Filter` and `Reply message` in the same menu. Another device in the vicinity running the Subscriber example will discover the Publisher. A service match happens when the service name (case insensitive) matches as well as the Matching filter(s) matche(s) (case sensitive). After this the Subscriber may send a Follow-up or initiate Datapath with the Publisher for further communication. The Publisher will send a reply to the Follow-up and will accept the Datapath request given that it has enough resources.

## Example Output

Usecase 1 : Publisher with a Follow-up exchange with a Subscriber
```
I (449) phy_init: phy_version 1800,e7ef680,Apr 13 2021,11:45:08
I (549) wifi:mode : NAN (18:fe:34:72:50:c8)
Set Vif NAN
I (559) NAN: State Init => Scanning

I (3559) NAN: Start own Cluster, Id 50:6f:9a:01:cc:4d
I (3559) wifi:Init max length of beacon: 752/752
I (3559) wifi:Init max length of beacon: 752/752
I (3559) NAN: State Scanning => AnchorMaster

I (3794) nan_app: NAN Discovery started.
I (5604) NAN: State Master => AnchorMaster

I (8244) nan_app: Received message 'Hello' from Peer e0:e2:e6:6a:7a:fe [Peer Service id - 5]
I (8244) nan_pub: Replied 'Welcome' to the Subscriber...
```

Usecase 2 : Publisher accepting Datapath request by a Subscriber
```
I (449) phy_init: phy_version 1800,e7ef680,Apr 13 2021,11:45:08
I (559) wifi:mode : NAN (18:fe:34:72:50:c8)
Set Vif NAN
I (559) NAN: State Init => Scanning

I (3559) NAN: Join Cluster with Id 50:6f:9a:01:cc:4d
I (3559) wifi:Init max length of beacon: 752/752
I (3559) wifi:Init max length of beacon: 752/752
I (3559) NAN: State Scanning => Master

I (3559) nan_pub: NAN Discovery started.
I (3569) nan_pub: Publishing Service test with ID 5
E (18809) wifi:Committed slots 0x3fff0000 for Peer 18:fe:34:72:50:b3 on Channel 6
I (18809) nan_pub: Datapath setup with 18:fe:34:72:50:b3
I (20349) nan_pub: Got IPv6 event: Interface "nan_pub: nan" address: fe80:0000:0000:0000:1afe:34ff:fe72:50ca, type: ESP_IP6_ADDR_IS_LINK_LOCAL
W (20929) wifi:<ba-add>idx:5 (ifx:2, 18:fe:34:72:50:b3), tid:0, ssn:0, winSize:64
```
