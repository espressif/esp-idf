| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |


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
 * Set WiFi SSID
 * Set WiFi Password
Component config  --->
  CoAP Configuration  --->
    * Set encryption method definition, PSK (default) or PKI
    * Enable CoAP debugging if required
    * Disable CoAP using TCP if this is not required (TCP needed for TLS)
    * Disable CoAP server functionality to reduce code size
Example CoAP Client Configuration  --->
 * Set CoAP Target Uri
 * If PSK, Set CoAP Preshared Key to use in connection to the server
 * If PSK, Set CoAP PSK Client identity (username)

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
or use the default of coap://californium.eclipseprojects.io.

and you could receive data from CoAP server if succeed,
such as the following log:

```
...
I (332) wifi: mode : sta (30:ae:a4:04:1b:7c)
I (1672) wifi: n:11 0, o:1 0, ap:255 255, sta:11 0, prof:1
I (1672) wifi: state: init -> auth (b0)
I (1682) wifi: state: auth -> assoc (0)
I (1692) wifi: state: assoc -> run (10)
I (1692) wifi: connected with huawei_cw, channel 11
I (1692) wifi: pm start, type: 1

I (2582) event: sta ip: 192.168.3.89, mask: 255.255.255.0, gw: 192.168.3.1
I (2582) CoAP_client: Connected to AP
I (2582) CoAP_client: DNS lookup succeeded. IP=35.185.40.182
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
This can be found at [libcoap Documentation](https://libcoap.net/documentation.html).
The current API is 4.3.0.

## libcoap Specific Issues
These can be raised at [libcoap Issues](https://github.com/obgm/libcoap/issues).

## Troubleshooting
* Please make sure Target Url includes valid `host`, optional `port`,
optional `path`, and begins with `coap://`, `coaps://`, `coap+tcp://` or `coaps+tcp://`
(not all hosts support TCP/TLS including coap+tcp://californium.eclipseprojects.io).

* CoAP logging can be enabled by running 'idf.py menuconfig -> Component config -> CoAP Configuration -> Enable CoAP debugging'
and setting appropriate log level.  If Mbed TLS logging is required, this needs to be configured separately under mbedTLS
Component Configuration and the CoAP logging level set to mbedTLS.

* CoAP library does not support IPv6 only configuration, so it is necessary to enable `LWIP_IPv4`
