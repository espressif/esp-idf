| Supported Targets | ESP32 | ESP32-C5 | ESP32-S2 |
| ----------------- | ----- | -------- | -------- |

# NAN Console Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Introduction
Neighbor Awareness Networking (NAN) is a protocol that allows Wi-Fi devices to discover services in their proximity. NAN uses direct device-to-device communication and does not require any Internet or AP connection.
Multiple NAN devices which are in the vicinity will form a NAN cluster which allows them to communicate with each other. Devices within a NAN cluster can advertise or look for services using NAN Service Discovery protocols.
A Publisher advertises a service and a Subscriber searches for a service. A subscriber either listens passively (Passive mode) or sends out broadcast Action frames (Active mode). Similarly a publisher either broadcasts its services (Unsolicited mode) or replies to Active subscribers (Solicited Mode). Matching of services is done by service name and optionally matching filters. Once a Subscriber gets a match to the service, it can either send Follow-up frames or negotiate using NDP frames to establish a datapath. After NDP is setup both devices will obtain a IPv6 address and can use it for further communication.

## How to use example
With the console users can configure NAN and its services on the fly. Flashing the example will first show a brief guide on the commands available on the console -

```bash
==================================================================
 |       Basic Steps to test NAN Discovery & Datapath               |
 |                                                                  |
 |  Below are short commands that use defaults, use 'help'          |
 |  to see detailed command parameters available                    |
 |                                                                  |
 |  # NAN Discovery -                                               |
 |  1. To start NAN issue 'nan -S', to stop it use 'nan -T'         |
 |  2. Publish OR Subscribe a service using 'publish' OR 'subscribe'|
 |  4. After service match, send a text to Peer with service id 5 - |
 |      > send -p 5 -t Hello                                        |
 |  5. Cancel an ongoing service with id 5 -                        |
 |      > publish -C -i 5                                           |
 |                                                                  |
 |  # NAN Datapath -                                                |
 |  1. Subscriber can initiate datapath using 'ndp -I -p [pub_id]'  |
 |  2. After NDP setup, use 'ping [Peer's IPv6]' to test datapath   |
 |  3. Terminate the NDP using 'ndp -D'                             |
 |                                                                  |
 ====================================================================

nan>
```

Mode detailed information about command parameters is given below -

1. **Starting NAN**
>nan> nan -S
   *OR*
>nan> nan -S -p 2 -c 6 -w 5
>p - Master Preference
>c - Operating channel
>w - Warmup time

2. **Stopping NAN**
> nan> nan -T

3. **Publish a service with name 'ESP_NAN-Service'**
>nan> publish
   *OR*
>nan> publish -n test -t 0 -f GREEN
>n - Service name
>t - Type (0: Unsolicited, 1: Solicited)
>f - Matching filter

4. **Subscribe to a NAN service 'ESP_NAN-Service'**
>nan> subscribe
   *OR*
>nan> subscribe -n test -t 0 -f GREEN
>n - Service name
>t - Type (0: passive, 1: active)
>f - Matching filter

5. **Cancel a NAN service**
> Cancel a Subscribe service with Inst Id 5
>nan> subscribe -C -i 5
> Cancel a Publish service with Inst Id 5
>nan> publish -C -i 5

6. **Send a Follow-up message**
*To send message to amatched Publish OR Subscribe service*
>nan> send -p 5 -t Hello
>p - Peer’s service instance id
>t - Message

7. **Send a Datapath request**
*To Initiate NDP Req to the last matched Publisher -*
>nan> ndp -I -p 5
>p - Publish Id

9. **Start a IPv6 Ping with Peer**
>nan> ping FE80::E2E2:E6FF:FE7B:C132

10. **Terminate the current NAN Datapath**
>nan> ndp -T -d 5
>d - NDP id

## NAN Datapath Example Output using defaults
Device 1 : Publish a service with default configuration -

```
nan> nan -S
I (28125) NAN: State Init => Scanning

I (31135) NAN: Start own Cluster, Id 50:6f:9a:01:0e:76
I (31135) NAN: State Scanning => AnchorMaster

I (31135) nan_app: NAN Discovery started.
nan>
nan> publish
I (77245) nan_console: Publishing service 'ESP_NAN-Service' [Publish id - 5]
nan>
I (21775) nan_app: NDP confirmed with NDP id 1 [Peer IPv6 - FE80::E2E2:E6FF:FE6A:7AFE]
I (23625) nan_app: NAN Data Interface ready [IPv6 - fe80:0000:0000:0000:e2e2:e6ff:fe7b:c132, type - LINK_LOCAL]
I (30265) nan_app: NDP id 1 with Peer e0:e2:e6:6a:7a:fe terminated (reason: 0)
```

Device 2 : Subscribe a service with default configuration and start a datapath with matched Publisher

```
nan> nan -S
I (142915) NAN: State Init => Scanning

I (145915) NAN: Join Cluster with Id 50:6f:9a:01:0e:76
I (145915) NAN: State Scanning => AnchorMaster

I (145915) nan_app: NAN Discovery started.
nan>
nan> subscribe
I (159685) nan_console: Subscribed to Service 'ESP_NAN-Service' [Subscribe id - 5]
nan> I (159845) nan_app: Service matched with e0:e2:e6:7b:c1:32 [Peer Publish id - 5]
nan>
nan> ndp -I -p 5
I (196125) nan_console: Initiated NDP with e0:e2:e6:7b:c1:32 [NDP id - 1]
nan> E (196425) wifi:Committed slots 0x7fff0000 for Peer e0:e2:e6:7b:c1:32 on Channel 6
I (196425) nan_app: NDP confirmed with NDP id 1 [Peer IPv6 - FE80::E2E2:E6FF:FE7B:C132]
I (197625) nan_app: NAN Data Interface ready [IPv6 - fe80:0000:0000:0000:e2e2:e6ff:fe6a:7afe, type - LINK_LOCAL]
nan> ping FE80::E2E2:E6FF:FE7B:C132
I (212975) nan_console: Pinging Peer with IPv6 addr FE80::E2E2:E6FF:FE7B:C132
nan> W (213495) wifi:<ba-add>idx:5 (ifx:2, e0:e2:e6:7b:c1:32), tid:0, ssn:0, winSize:64
I (213555) nan_console: 64 bytes from FE80::E2E2:E6FF:FE7B:C132 icmp_seq=1 ttl=0 time=577 ms
I (214015) nan_console: 64 bytes from FE80::E2E2:E6FF:FE7B:C132 icmp_seq=2 ttl=0 time=40 ms
I (215095) nan_console: 64 bytes from FE80::E2E2:E6FF:FE7B:C132 icmp_seq=3 ttl=0 time=121 ms
I (216165) nan_console: 64 bytes from FE80::E2E2:E6FF:FE7B:C132 icmp_seq=4 ttl=0 time=190 ms
I (217445) nan_console: 64 bytes from FE80::E2E2:E6FF:FE7B:C132 icmp_seq=5 ttl=0 time=475 ms
I (217975) nan_console:
--- FE80::E2E2:E6FF:FE7B:C132 ping statistics ---
I (217975) nan_console: 5 packets transmitted, 5 received, 0% packet loss, time 1403ms
nan>
nan> ndp -T -d 1
nan> I (28175) nan_app: NDP id 1 with Peer e0:e2:e6:7b:c1:32 terminated (reason: 0)
```

## NAN Follow-up example output using advanced commands

Device 1 : Publisher uses a filter and responds to a message from a Subscriber
```
nan> nan -S
I (142915) NAN: State Init => Scanning

I (145915) NAN: Join Cluster with Id 50:6f:9a:01:0e:76
I (145915) NAN: State Scanning => AnchorMaster

I (145915) nan_app: NAN Discovery started.
nan>
nan> publish -n TEST -f GREEN
I (61935) nan_console: Publishing service 'TEST' [Publish id - 5]
nan>
I (93635) nan_app: Received message 'Hello' from Peer e0:e2:e6:6a:7a:fe [Peer Service id - 5]
nan> send -p 5 -t Welcome
I (114605) nan_console: Sent message 'Welcome' to NAN Peer e0:e2:e6:6a:7a:fe.
```

Device 2 : Subscriber uses a filter and sends a message to the matched Publisher
```
nan> nan -S
I (142915) NAN: State Init => Scanning

I (145915) NAN: Join Cluster with Id 50:6f:9a:01:0e:76
I (145915) NAN: State Scanning => AnchorMaster

I (145915) nan_app: NAN Discovery started.
nan>
nan> subscribe -n TEST -f GREEN
I (159685) nan_console: Subscribed to Service 'TEST' [Subscribe id - 5]
nan> I (159845) nan_app: Service matched with e0:e2:e6:7b:c1:32 [Peer Publish id - 5]
nan> send -p 5 -t Hello
I (99445) nan_console: Sent message 'Hello' to NAN Peer e0:e2:e6:7b:c1:32.
nan>
I (120685) nan_app: Received message 'Welcome' from Peer e0:e2:e6:7b:c1:32 [Peer Service id - 5]
```
