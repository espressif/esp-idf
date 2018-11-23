HTTPS server
============

Overview
--------

This component is built on top of `esp_http_server`. The HTTPS server takes advantage of hooks and function overrides in the regular HTTP server to provide encryption using OpenSSL.

All documentation for `esp_http_server` applies also to a server you create this way.

Used APIs
---------

The following API of `esp_http_server` should not be used with `esp_https_server`, as they are used internally to handle secure sessions and to maintain internal state:

* "send", "receive" and "pending" function overrides - secure socket handling

  * :cpp:func:`httpd_sess_set_send_override`
  * :cpp:func:`httpd_sess_set_recv_override`
  * :cpp:func:`httpd_sess_set_pending_override`
* "transport context" - both global and session

  * :cpp:func:`httpd_sess_get_transport_ctx` - returns SSL used for the session
  * :cpp:func:`httpd_sess_set_transport_ctx`
  * :cpp:func:`httpd_get_global_transport_ctx` - returns the shared SSL context
  * :c:member:`httpd_config_t.global_transport_ctx`
  * :c:member:`httpd_config_t.global_transport_ctx_free_fn`
  * :c:member:`httpd_config_t.open_fn` - used to set up secure sockets

Everything else can be used without limitations.

Usage
-----

Please see the example :example:`protocols/https_server` to learn how to set up a secure server.

Basically all you need is to generate a certificate, embed it in the firmware, and provide its pointers and lengths to the start function via the init struct.

The server can be started with or without SSL by changing a flag in the init struct - :c:member:`httpd_ssl_config.transport_mode`. This could be used e.g. for testing or in trusted environments where you prefer speed over security.

Performance
-----------

The initial session setup can take about two seconds, or more with slower clock speeds or more verbose logging. Subsequent requests through the open secure socket are much faster (down to under
100 ms).

API Reference
-------------

.. include:: /_build/inc/esp_https_server.inc
