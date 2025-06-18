| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Websocket echo server

(See the README.md file in the upper level 'examples' directory for more information about examples.)
This example demonstrates the HTTPD server using the WebSocket feature.

## How to Use Example

The example starts a websocket server on a local network. You need a websocket client to interact with the server (an example test
ws_server_example_test.py could be used as the simple websocket client). If you run ws_server_example_test.py and get
`ModuleNotFoundError: No module named 'websocket'`, then please install `websocket` by running `python -m pip install websocket-client`.

The server registers websocket handler which echoes back the received WebSocket frame. It also demonstrates
use of asynchronous send, which is triggered on reception of a certain message.

### Websocket support in `http_server`

Websocket echo server is build on top of the HTTP server component using [Websocket server](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/protocols/esp_http_server.html#websocket-server) configuration.
This feature is very limited, and a special care must be taken while implementing websocket URI handlers.

#### Configure URI handler

We register the URI handler with the standard API `httpd_register_uri_handler()` with `is_websocket` enabled and other optional parameters:

```c
static const httpd_uri_t ws_uri_handler_options = {
        ... // httpd options

        .is_websocket = true,               // Mandatory: set to `true` to handler websocket protocol
        .handle_ws_control_frames = false,  // Optional: set to `true` for the handler to receive control packets, too
        .supported_subprotocol = "chat",    // Optional: set supported subprotocol for this handler
};

```

#### Implement URI handler

The URI handler is called on every URI request, but also before the websocket handshake, so it is very important to check the request type before reading websocket frame:

```c
    // beginning of the ws URI handler
    if (req->method == HTTP_GET) {
        // action before ws handshake
        return ESP_OK;
    }
    // action after the handshake (read frames)
```

#### Handling incoming data

To receive websocket frames, use `httpd_ws_recv_frame()` after the websocket handshake, with `httpd_ws_frame_t` parameters set accordingly:
* `payload` to a valid buffer for the received data
* `len` set to `0` for the first call of `httpd_ws_recv_frame()`. Note that this value is used to indicate the packet length has been read in the previous call if we use dynamic buffers.

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

#### Handling outgoing data

Please note that the WebSocket HTTP server does not automatically fragment messages.
Each outgoing frame has the FIN flag set by default.
In case an application wants to send fragmented data, it must be done manually by setting the
`fragmented` option and using the `final` flag as described in [RFC6455, section 5.4](https://tools.ietf.org/html/rfc6455#section-5.4).


### Hardware Required

This example can be executed on any common development board, the only required interface is WiFi or Ethernet connection to a local network.

### Configure the project

* Open the project configuration menu (`idf.py menuconfig`)
* Configure Wi-Fi or Ethernet under "Example Connection Configuration" menu. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
```
I (4932) example_connect: Got IPv6 event!
I (4942) example_connect: Connected to Espressif
I (4942) example_connect: IPv4 address: 192.168.4.2
I (4952) example_connect: IPv6 address: fe80:xxxx
I (4962) ws_echo_server: Starting server on port: '80'
I (4962) ws_echo_server: Registering URI handlers
D (4962) httpd: httpd_thread: web server started
D (4972) httpd: httpd_server: doing select maxfd+1 = 56
D (4982) httpd_uri: httpd_register_uri_handler: [0] installed /ws
D (17552) httpd: httpd_server: processing listen socket 54
D (17552) httpd: httpd_accept_conn: newfd = 57
D (17552) httpd_sess: httpd_sess_new: fd = 57
D (17562) httpd: httpd_accept_conn: complete
D (17562) httpd: httpd_server: doing select maxfd+1 = 58
D (17572) httpd: httpd_server: processing socket 57
D (17572) httpd_sess: httpd_sess_process: httpd_req_new
D (17582) httpd_parse: httpd_req_new: New request, has WS? No, sd->ws_handler valid? No, sd->ws_close? No
D (17592) httpd_txrx: httpd_recv_with_opt: requested length = 128
D (17592) httpd_txrx: httpd_recv_with_opt: received length = 128
D (17602) httpd_parse: read_block: received HTTP request block size = 128
D (17612) httpd_parse: cb_url: message begin
D (17612) httpd_parse: cb_url: processing url = /ws
D (17622) httpd_parse: verify_url: received URI = /ws
D (17622) httpd_parse: cb_header_field: headers begin
D (17632) httpd_txrx: httpd_unrecv: length = 110
D (17632) httpd_parse: pause_parsing: paused
D (17632) httpd_parse: cb_header_field: processing field = Host
D (17642) httpd_txrx: httpd_recv_with_opt: requested length = 128
D (17652) httpd_txrx: httpd_recv_with_opt: pending length = 110
D (17652) httpd_parse: read_block: received HTTP request block size = 110
D (17662) httpd_parse: continue_parsing: skip pre-parsed data of size = 5
D (17672) httpd_parse: continue_parsing: un-paused
D (17682) httpd_parse: cb_header_field: processing field = Upgrade
D (17682) httpd_parse: cb_header_value: processing value = websocket
D (17692) httpd_parse: cb_header_field: processing field = Connection
D (17702) httpd_parse: cb_header_value: processing value = Upgrade
D (17702) httpd_parse: cb_header_field: processing field = Sec-WebSocket-Key
D (17712) httpd_parse: cb_header_value: processing value = gfhjgfhjfj
D (17722) httpd_parse: cb_header_field: processing field = Sec-WebSocket-Proto
D (17722) httpd_parse: parse_block: parsed block size = 110
D (17732) httpd_txrx: httpd_recv_with_opt: requested length = 128
D (17742) httpd_txrx: httpd_recv_with_opt: received length = 40
D (17742) httpd_parse: read_block: received HTTP request block size = 40
D (17752) httpd_parse: cb_header_field: processing field = col
D (17752) httpd_parse: cb_header_value: processing value = echo
D (17762) httpd_parse: cb_header_field: processing field = Sec-WebSocket-Version
D (17772) httpd_parse: cb_header_value: processing value = 13
D (17772) httpd_parse: cb_headers_complete: bytes read     = 169
D (17782) httpd_parse: cb_headers_complete: content length = 0
D (17792) httpd_parse: cb_headers_complete: Got an upgrade request
D (17792) httpd_parse: pause_parsing: paused
D (17802) httpd_parse: cb_no_body: message complete
D (17802) httpd_parse: httpd_parse_req: parsing complete
D (17812) httpd_uri: httpd_uri: request for /ws with type 1
D (17812) httpd_uri: httpd_find_uri_handler: [0] = /ws
D (17822) httpd_uri: httpd_uri: Responding WS handshake to sock 57
D (17822) httpd_ws: httpd_ws_respond_server_handshake: Server key before encoding: gfhjgfhjfj258EAFA5-E914-47DA-95CA-C5AB0DC85B11
D (17842) httpd_ws: httpd_ws_respond_server_handshake: Generated server key: Jg/fQVRsgwdDzYeG8yNBHRajUxw=
D (17852) httpd_sess: httpd_sess_process: httpd_req_delete
D (17852) httpd_sess: httpd_sess_process: success
D (17862) httpd: httpd_server: doing select maxfd+1 = 58
D (17892) httpd: httpd_server: processing socket 57
D (17892) httpd_sess: httpd_sess_process: httpd_req_new
D (17892) httpd_parse: httpd_req_new: New request, has WS? Yes, sd->ws_handler valid? Yes, sd->ws_close? No
D (17902) httpd_parse: httpd_req_new: New WS request from existing socket
D (17902) httpd_txrx: httpd_recv_with_opt: requested length = 1
D (17912) httpd_txrx: httpd_recv_with_opt: received length = 1
D (17912) httpd_ws: httpd_ws_get_frame_type: First byte received: 0x81
D (17922) httpd_txrx: httpd_recv_with_opt: requested length = 1
D (17932) httpd_txrx: httpd_recv_with_opt: received length = 1
D (17932) httpd_txrx: httpd_recv_with_opt: requested length = 4
D (17942) httpd_txrx: httpd_recv_with_opt: received length = 4
D (17942) httpd_txrx: httpd_recv_with_opt: requested length = 13
D (17952) httpd_txrx: httpd_recv_with_opt: received length = 13
I (17962) ws_echo_server: Got packet with message: Trigger async
I (17962) ws_echo_server: Packet type: 1
D (17972) httpd_sess: httpd_sess_process: httpd_req_delete
D (17972) httpd_sess: httpd_sess_process: success
D (17982) httpd: httpd_server: doing select maxfd+1 = 58
D (17982) httpd: httpd_server: processing ctrl message
D (17992) httpd: httpd_process_ctrl_msg: work
D (18002) httpd: httpd_server: doing select maxfd+1 = 58
```

See the README.md file in the upper level 'examples' directory for more information about examples.
