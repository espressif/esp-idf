HTTPS Server
============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

This component is built on top of :doc:`esp_http_server`. The HTTPS server takes advantage of hook registration functions in the regular HTTP server to provide callback function for SSL session.

All documentation for :doc:`esp_http_server` applies also to a server you create this way.

Used APIs
---------

The following APIs of :doc:`esp_http_server` should not be used with :doc:`esp_https_server`, as they are used internally to handle secure sessions and to maintain internal state:

* "send", "receive" and "pending" callback registration functions - secure socket handling

  * :cpp:func:`httpd_sess_set_send_override`
  * :cpp:func:`httpd_sess_set_recv_override`
  * :cpp:func:`httpd_sess_set_pending_override`

* "transport context" - both global and session

  * :cpp:func:`httpd_sess_get_transport_ctx` - returns SSL used for the session
  * :cpp:func:`httpd_sess_set_transport_ctx`
  * :cpp:func:`httpd_get_global_transport_ctx` - returns the shared SSL context
  * :cpp:member:`httpd_config::global_transport_ctx`
  * :cpp:member:`httpd_config::global_transport_ctx_free_fn`
  * :cpp:member:`httpd_config::open_fn` - used to set up secure sockets

Everything else can be used without limitations.

Usage
-----

Please see the example :example:`protocols/https_server` to learn how to set up a secure server.

Basically, all you need is to generate a certificate, embed it into the firmware, and pass the init struct into the start function after the certificate address and lengths are correctly configured in the init struct.

The server can be started with or without SSL by changing a flag in the init struct - :cpp:member:`httpd_ssl_config::transport_mode`. This could be used, e.g., for testing or in trusted environments where you prefer speed over security.

Performance
-----------

The initial session setup can take about two seconds, or more with slower clock speed or more verbose logging. Subsequent requests through the open secure socket are much faster (down to under 100 ms).

Event Handling
--------------

ESP HTTPS Server has various events for which a handler can be triggered by the :doc:`../system/esp_event` when the particular event occurs. The handler has to be registered using :cpp:func:`esp_event_handler_register`. This helps in event handling for ESP HTTPS Server.

:cpp:enum:`esp_https_server_event_id_t` has all the events which can happen for ESP HTTPS server.

Expected data type for different ESP HTTPS server events in event loop:

    - HTTPS_SERVER_EVENT_ERROR          :   ``esp_https_server_last_error_t``
    - HTTPS_SERVER_EVENT_START          :   ``NULL``
    - HTTPS_SERVER_EVENT_ON_CONNECTED   :   ``NULL``
    - HTTPS_SERVER_EVENT_ON_DATA        :   ``int``
    - HTTPS_SERVER_EVENT_SENT_DATA      :   ``NULL``
    - HTTPS_SERVER_EVENT_DISCONNECTED   :   ``NULL``
    - HTTPS_SERVER_EVENT_STOP           :   ``NULL``

Application Examples
--------------------

- :example:`protocols/https_server/simple` demonstrates how to create an HTTPS server that returns a simple HTML page when visited.

- :example:`protocols/https_server/wss_server` demonstrates how to create an SSL server with a simple WebSocket request handler that supports handling multiple clients, PING-PONG mechanism, and sending asynchronous messages to all clients.

HTTPS Server Cert Selection Hook
--------------------------------

The ESP HTTPS Server component provides an option to set the server certification selection hook. This feature allows you to configure and use a certificate selection callback during server handshake. The callback helps to select a certificate to present to the client based on the TLS extensions supplied in the client hello message, such as ALPN and SNI.

To enable this feature, please enable :ref:`CONFIG_ESP_HTTPS_SERVER_CERT_SELECT_HOOK` in the ESP HTTPS Server menuconfig. Please note that the ESP-TLS option is only available when Mbedtls is used as the TLS stack for ESP-TLS (default behaviour).

When enabled, you can set the certificate selection callback using the :cpp:member:`httpd_ssl_config::cert_select_cb` member of the :cpp:type:`httpd_ssl_config_t` structure.

.. code-block:: c

  int cert_selection_callback(mbedtls_ssl_context *ssl)
    {
        /* Code that the callback should execute */
        return 0;
    }

    httpd_ssl_config_t cfg = {
        cert_select_cb = cert_section_callback,
    };


API Reference
-------------

.. include-build-file:: inc/esp_https_server.inc
