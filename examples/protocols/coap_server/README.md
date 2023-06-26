| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |


# CoAP server example

(See the README.md file in the upper level 'examples' directory for more information about examples.)
This CoAP server example is very simplified adaptation of one of the
[libcoap](https://github.com/obgm/libcoap) examples.

CoAP server example will startup a daemon task, receive requests / data from CoAP client and transmit
data to CoAP client.

If the incoming request requests the use of DTLS (connecting to port 5684), then the CoAP server will
try to establish a DTLS session using the previously defined Pre-Shared Key (PSK) - which
must be the same as the one that the CoAP client is using, or Public Key Infrastructure (PKI) where
the PKI information must match as requested.

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
    * Disable CoAP client functionality to reduce code size unless this server is a proxy
Example CoAP Server Configuration  --->
 * If PSK, Set CoAP Preshared Key to use for connections to the server

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py build
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
current CoAP server would startup a daemon task,
and the log is such as the following:

```
...
I (332) wifi: mode : sta (30:ae:a4:04:1b:7c)
I (1672) wifi: n:11 0, o:1 0, ap:255 255, sta:11 0, prof:1
I (1672) wifi: state: init -> auth (b0)
I (1682) wifi: state: auth -> assoc (0)
I (1692) wifi: state: assoc -> run (10)
I (1692) wifi: connected with huawei_cw, channel 11
I (1692) wifi: pm start, type: 1

I (2622) event: sta ip: 192.168.3.84, mask: 255.255.255.0, gw: 192.168.3.1
I (2622) CoAP_server: Connected to AP
...
```

If a CoAP client queries the `/Espressif` resource, CoAP server will return `"Hello World!"`
until a CoAP client does a PUT with different data.

## libcoap Documentation
This can be found at [libcoap Documentation](https://libcoap.net/documentation.html).
The current API is 4.3.0.

## libcoap Specific Issues
These can be raised at [libcoap Issues](https://github.com/obgm/libcoap/issues).

## Troubleshooting
* Please make sure CoAP client fetchs or puts data under path: `/Espressif` or
fetches `/.well-known/core`

* CoAP logging can be enabled by running 'idf.py menuconfig -> Component config -> CoAP Configuration -> Enable CoAP debugging'
and setting appropriate log level.  If Mbed TLS logging is required, this needs to be configured separately under mbedTLS
Component Configuration and the CoAP logging level set to mbedTLS.

* CoAP library does not support IPv6 only configuration, so it is necessary to enable `LWIP_IPv4`
