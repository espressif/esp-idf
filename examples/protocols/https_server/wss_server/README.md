# HTTP Websocket server with SSL support

This example creates a SSL server and employs a simple Websocket request handler. It demonstrates handling multiple clients from the server including:
* PING-PONG mechanism
* Sending asynchronous messages to all clients

See the `esp_https_server` component documentation for details.

Before using the example, open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### Websocket support in `http_server`

Please refer to the documentation of [Websocket server](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/protocols/esp_http_server.html#websocket-server) feature in the documentation,
or to the description of using websocket handlers in httpd in the [simple ws echo](../../http_server/ws_echo_server/README.md#how-to-use-example) example.

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
