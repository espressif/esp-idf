| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# UDP Multicast Example

This example shows how to use the IPV4 & IPV6 UDP multicast features via the BSD-style sockets interface.

## Behaviour

The behaviour of the example is:

* Listens to specified multicast addresses (one IPV4 and/or one IPV6).
* Print any UDP packets received as ASCII text.
* If no packets are received it will periodically (after 2.5 seconds) send its own plaintext packet(s) to the multicast address(es).

## Configuration

Open the project configuration menu (`idf.py menuconfig`).

Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

The "Example Configuration" menu allows you to configure the details of the example:

* IP Mode: IPV4 & IPV6 dual, IPV4 only, or IPv6 only.
* Multicast addresses for IPV4 and/or IPV6.
* Enable multicast socket loopback (ie should the socket receive its own multicast transmissions.)
* Change the interface to add the multicast group on (default interface, or WiFi STA interface.) Both methods are valid.

## Implementation Details

In IPV4 & IPV6 dual mode, an IPV6 socket is created and the "dual mode" options described in [RFC4038](https://tools.ietf.org/html/rfc4038) are used to bind it to the default address for both IPV4 & IPV6 and join both the configured IPV4 & IPV6 multicast groups. Otherwise, a single socket of the appropriate type is created.

The socket is always bound to the default address, so it will also receive unicast packets. If you only want to receive multicast packets for a particular address, `bind()` to that  multicast address instead.

## Host Tools

There are many host-side tools which can be used to interact with the UDP multicast example. One command line tool is [socat](http://www.dest-unreach.org/socat/) which can send and receive many kinds of packets.

### Send IPV4 multicast via socat

```
echo "Hi there, IPv4!" | socat STDIO UDP4-DATAGRAM:232.10.11.12:3333,ip-multicast-if=(host_ip_addr)
```

Replace `232.10.11.12:3333` with the IPV4 multicast address and port, and `(host_ip_addr)` with the host's IP address (used to find the interface to send the multicast packet on.)

### Receive IPV4 multicast via socat

```
socat STDIO UDP4-RECVFROM:3333,ip-add-membership=232.10.11.12:(host_ip_addr)
```

Replace `:3333` and `232.10.11.12` with the port and IPV4 multicast address, respectively. Replace `(host_ip_addr)` with the host IP address, used to find the interface to listen on.

(The `,ip-add-membership=...` clause may not be necessary, depending on your network configuration.)

### Send IPV6 multicast via socat

```
echo "Hi there, IPV6!" | socat STDIO UDP6-DATAGRAM:[ff02::fc]:3333
```

Replace `[ff02::fc]:3333` with the IPV6 multicast address and port, respectively.

### Receive IPV6 multicast via socat

At time of writing this is not possible without patching socat. Use a different tool or programming language to receive IPV6 multicast packets.

## About Examples

See the README.md file in the upper level 'examples' directory for general information about examples.

