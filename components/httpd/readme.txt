This is a basic HTTP server with WebSockets based on httpd from LwIP.

WebSockets implementation supports binary and text modes. Multiple sockets are supported. Continuation frames are not implemented.
By default, a WebSocket is closed after 20 seconds of inactivity to conserve memory. This behavior can be changed by overriding `WS_TIMEOUT` option.

To enable debugging extra flags `-DLWIP_DEBUG=1 -DHTTPD_DEBUG=LWIP_DBG_ON` should be passed at compile-time.

This module expects your project to provide "fsdata.c" created with "makefsdata" utility.
See examples/http_server.

Maintained by lujji (https://github.com/lujji/esp-httpd).
