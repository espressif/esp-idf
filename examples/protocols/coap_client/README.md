
# CoAP client example

(See the README.md file in the upper level 'examples' directory for more information about examples.)
this CoAP client example is adaptation of one of the [libcoap](https://github.com/obgm/libcoap) example.

CoAP client example would connect your ESP32 device to any CoAP server, fetch data from CoAP server and upstream data to CoAP server.

The Constrained Application Protocol (CoAP) is a specialized web transfer protocol for use with constrained nodes and constrained networks in the Internet of Things.   
The protocol is designed for machine-to-machine (M2M) applications such as smart energy and building automation.

please refer to [RFC7252](https://www.rfc-editor.org/rfc/pdfrfc/rfc7252.txt.pdf) for more details.

## How to use example

### Configure the project

```
make menuconfig
```

* Set serial port under Serial Flasher config
* Set Target Uri under Example Configuration
* Set WiFi SSID under Example Configuration
* Set WiFi Password under Example Configuration

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
Prerequisite: we startup a CoAP server on coap server example.  

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
I (2582) CoAP_client: DNS lookup succeeded. IP=192.168.3.84
Received: Hello World!
E (8102) CoAP_client: select timeout
E (13102) CoAP_client: select timeout
...
```

## Troubleshooting
* Please make sure Target Url includes valid `host`, `port`, `path`, and begins with `coap://`  
