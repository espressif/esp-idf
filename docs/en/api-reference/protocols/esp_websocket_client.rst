ESP WebSocket Client
====================

Overview
--------
The ESP WebSocket client is an implementation of `WebSocket protocol client <https://tools.ietf.org/html/rfc6455>`_ for ESP32

Features
--------
   * supports WebSocket over TCP, SSL with mbedtls
   * Easy to setup with URI
   * Multiple instances (Multiple clients in one application)

Configuration
-------------
URI
^^^

-  Supports ``ws``, ``wss`` schemes
-  WebSocket samples:

   -  ``ws://websocket.org``: WebSocket over TCP, default port 80
   -  ``wss://websocket.org``: WebSocket over SSL, default port 443
  
-  Minimal configurations:

.. code:: c

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "ws://websocket.org",
    };

-  If there are any options related to the URI in
   ``esp_websocket_client_config_t``, the option defined by the URI will be
   overridden. Sample:

.. code:: c

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "ws://websocket.org:123",
        .port = 4567,
    };
    //WebSocket client will connect to websocket.org using port 4567

SSL
^^^

-  Get certificate from server, example: ``websocket.org``
   ``openssl s_client -showcerts -connect websocket.org:443 </dev/null 2>/dev/null|openssl x509 -outform PEM >websocket_org.pem``
-  Configuration:

.. code:: cpp

    const esp_websocket_client_config_t ws_cfg = {
        .uri = "wss://websocket.org",
        .cert_pem = (const char *)websocket_org_pem_start,
    };

For more options on ``esp_websocket_client_config_t``, please refer to API reference below

Application Example
-------------------
Simple WebSocket example that uses esp_websocket_client to establish a websocket connection and send/receive data with the `websocket.org <https://websocket.org>`_ Server: :example:`protocols/websocket`.


API Reference
-------------

.. include:: /_build/inc/esp_websocket_client.inc

