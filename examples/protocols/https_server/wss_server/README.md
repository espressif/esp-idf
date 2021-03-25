# HTTP Websocket server with SSL support

This example creates a SSL server and employs a simple Websocket request handler. It demonstrates handling multiple clients from the server including:
* PING-PONG mechanism
* Sending asynchronous messages to all clients

See the `esp_https_server` component documentation for details.

Before using the example, open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

`httpd_ws_recv_frame` support two ways to get frame payload.
* Static buffer -- Allocate maximum expected packet length (either statically or dynamically) and call `httpd_ws_recv_frame()` referencing this buffer and it's size. (Unnecessarily large buffers might cause memory waste)

```
#define MAX_PAYLOAD_LEN 128
uint8_t buf[MAX_PAYLOAD_LEN] = { 0 };
httpd_ws_frame_t ws_pkt;
ws_pkt.payload = buf;
httpd_ws_recv_frame(req, &ws_pkt, MAX_PAYLOAD_LEN);
```
* Dynamic buffer -- Refer to the examples, which receive websocket data in these three steps:
   1) Call `httpd_ws_recv_frame()` with zero buffer size
   2) Allocate the size based on the received packet length
   3) Call `httpd_ws_recv_frame()` with the allocated buffer

## Certificates

You will need to approve a security exception in your browser. This is because of a self signed
certificate; this will be always the case, unless you preload the CA root into your browser/system
as trusted.

You can generate a new certificate using the OpenSSL command line tool:

```
openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -x509 -days 3650 -out cacert.pem -subj "/CN=ESP32 HTTPS server example"
```

Expiry time and metadata fields can be adjusted in the invocation.

Please see the openssl man pages (man openssl-req) for more details.

It is **strongly recommended** to not reuse the example certificate in your application;
it is included only for demonstration.
