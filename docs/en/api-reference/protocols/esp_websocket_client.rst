ESP WebSocket Client
====================

Overview
--------
The ESP WebSocket client is an implementation of `WebSocket protocol client <https://tools.ietf.org/html/rfc6455>`_ for {IDF_TARGET_NAME}

Features
--------
   * Supports WebSocket over TCP, TLS with mbedtls
   * Easy to setup with URI
   * Multiple instances (Multiple clients in one application)

Configuration
-------------
URI
^^^

-  Supports ``ws``, ``wss`` schemes
-  WebSocket samples:

   -  ``ws://echo.websocket.org``: WebSocket over TCP, default port 80
   -  ``wss://echo.websocket.org``: WebSocket over SSL, default port 443

Minimal configurations:

.. code:: c

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "ws://echo.websocket.org",
    };

The WebSocket client supports the use of both path and query in the URI. Sample:

.. code:: c

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "ws://echo.websocket.org/connectionhandler?id=104",
    };


If there are any options related to the URI in
:cpp:type:`esp_websocket_client_config_t`, the option defined by the URI will be
overridden. Sample:

.. code:: c

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "ws://echo.websocket.org:123",
        .port = 4567,
    };
    //WebSocket client will connect to websocket.org using port 4567

TLS
^^^

Configuration:

.. code:: c

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "wss://echo.websocket.org",
        .cert_pem = (const char *)websocket_org_pem_start,
    };

.. note:: If you want to verify the server, then you need to provide a certificate in PEM format, and provide to ``cert_pem`` in :cpp:type:`websocket_client_config_t`. If no certficate is provided then the TLS connection will default to not requiring verification.

PEM certificate for this example could be extracted from an openssl `s_client` command connecting to websocket.org.
In case a host operating system has `openssl` and `sed` packages installed, one could execute the following command to download and save the root or intermediate root certificate to a file (Note for Windows users: Both Linux like environment or Windows native packages may be used).
```
echo "" | openssl s_client -showcerts -connect websocket.org:443 | sed -n "1,/Root/d; /BEGIN/,/END/p" | openssl x509 -outform PEM >websocket_org.pem
```

This command will extract the second certificate in the chain and save it as a pem-file.

Subprotocol
^^^^^^^^^^^

The subprotocol field in the config struct can be used to request a subprotocol

.. code:: c

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "ws://websocket.org",
        .subprotocol = "soap",
    };

.. note:: The client is indifferent to the subprotocol field in the server response and will accept the connection no matter what the server replies.

For more options on :cpp:type:`esp_websocket_client_config_t`, please refer to API reference below

Events
------
* `WEBSOCKET_EVENT_CONNECTED`: The client has successfully established a connection to the server. The client is now ready to send and receive data. Contains no event data.
* `WEBSOCKET_EVENT_DISCONNECTED`: The client has aborted the connection due to the transport layer failing to read data, e.g. because the server is unavailable. Contains no event data.
* `WEBSOCKET_EVENT_DATA`: The client has successfully received and parsed a WebSocket frame. The event data contains a pointer to the payload data, the length of the payload data as well as the opcode of the received frame. A message may be fragmented into multiple events if the length exceeds the buffer size. This event will also be posted for non-payload frames, e.g. pong or connection close frames.
* `WEBSOCKET_EVENT_ERROR`: Not used in the current implementation of the client.

If the client handle is needed in the event handler it can be accessed through the pointer passed to the event handler:

.. code:: c

    esp_websocket_client_handle_t client = (esp_websocket_client_handle_t)handler_args;


Limitations and Known Issues
----------------------------
* The client is able to request the use of a subprotocol from the server during the handshake, but does not do any subprotocol related checks on the response from the server.

Application Example
-------------------
A simple WebSocket example that uses esp_websocket_client to establish a websocket connection and send/receive data with the `websocket.org <https://websocket.org>`_ server can be found here: :example:`protocols/websocket`.

Sending Text Data
^^^^^^^^^^^^^^^^^
The WebSocket client supports sending data as a text data frame, which informs the application layer that the payload data is text data encoded as UTF-8. Example:

.. code:: cpp

    esp_websocket_client_send_text(client, data, len, portMAX_DELAY);


API Reference
-------------

.. include-build-file:: inc/esp_websocket_client.inc

