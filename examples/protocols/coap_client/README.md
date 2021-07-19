
# CoAP client example

(See the README.md file in the upper level 'examples' directory for more information about examples.)
This CoAP client example is very simplified adaptation of one of the
[libcoap](https://github.com/obgm/libcoap) examples.

CoAP client example will connect your ESP32 device to a CoAP server, send off a GET request and
fetch the response data from CoAP server.  The client can be extended to PUT / POST / DELETE requests,
as well as supporting the Observer extensions [RFC7641](https://tools.ietf.org/html/rfc7641).

If the URI is prefixed with coaps:// instead of coap://, then the CoAP client will attempt to use
the DTLS protocol using the defined Pre-Shared Keys(PSK) or Public Key Infrastructure (PKI) which the
CoAP server needs to know about.

If the URI is prefixed with coap+tcp://, then the CoAP will try to use TCP for the communication.

NOTE: coaps+tcp:// is not currently supported, even though both libcoap and MbedTLS support it.

The Constrained Application Protocol (CoAP) is a specialized web transfer protocol for use with
constrained nodes and constrained networks in the Internet of Things.   
The protocol is designed for machine-to-machine (M2M) applications such as smart energy and
building automation.

Please refer to [RFC7252](https://www.rfc-editor.org/rfc/pdfrfc/rfc7252.txt.pdf) for more details.

## How to use example

### Configure the project

```
idf.py menuconfig
```

Example Connection Configuration  --->
 * Set WiFi SSID under Example Configuration
 * Set WiFi Password under Example Configuration
Example CoAP Client Configuration  --->
 * Set CoAP Target Uri
 * If PSK, Set CoAP Preshared Key to use in connection to the server
 * If PSK, Set CoAP PSK Client identity (username)
Component config  --->
  CoAP Configuration  --->
    * Set encryption method definition, PSK (default) or PKI
    * Enable CoAP debugging if required

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py build
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
Prerequisite: we startup a CoAP server on coap server example,
or use the default of coaps://californium.eclipseprojects.io.

and you could receive data from CoAP server if succeed,  
such as the following log:

```
...
I (5104) esp_netif_handlers: example_connect: sta ip: 192.168.0.103, mask: 255.255.255.0, gw: 192.168.0.1
I (5104) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.0.103
I (5604) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:32ae:a4ff:fec5:3234, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5604) example_connect: Connected to example_connect: sta
I (5614) example_connect: - IPv4 address: 192.168.0.103
I (5614) example_connect: - IPv6 address: fe80:0000:0000:0000:32ae:a4ff:fec5:3234, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (6004) CoAP_client: DNS lookup succeeded. IP=35.185.40.182
Received:
****************************************************************
CoAP RFC 7252                                  Cf 3.0.0-SNAPSHOT
****************************************************************
This server is using the Eclipse Californium (Cf) CoAP framework
published under EPL+EDL: http://www.eclipse.org/californium/

(c) 2014-2020 Institute for Pervasive Computing, ETH Zurich and others
****************************************************************
...
```

## libcoap Documentation
This can be found at https://libcoap.net/doc/reference/4.2.0/

## Troubleshooting
* Please make sure Target Url includes valid `host`, optional `port`,
optional `path`, and begins with `coap://`, `coaps://` or `coap+tcp://`
for a coap server that supports TCP
(not all do including coap+tcp://californium.eclipseprojects.io).

* CoAP logging can be enabled by running 'idf.py menuconfig -> Component config -> CoAP Configuration' and setting appropriate log level
