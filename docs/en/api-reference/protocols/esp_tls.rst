ESP-TLS
=======

Overview
--------

The ESP-TLS component provides a simplified API interface for accessing the commonly used TLS functionality. 
It supports common scenarios like CA certification validation, SNI, ALPN negotiation, non-blocking connection among others. 
All the configuration can be specified in the esp_tls_cfg_t data structure. Once done, TLS communication can be conducted using the following APIs:
* esp_tls_conn_new(): for opening a new TLS connection
* esp_tls_conn_read/write(): for reading/writing from the connection
* esp_tls_conn_delete(): for freeing up the connection
Any application layer protocol like HTTP1, HTTP2 etc can be executed on top of this layer.                       

Application Example
-------------------

Simple HTTPS example that uses ESP-TLS to establish a secure socket connection: :example:`protocols/https_request`.

API Reference
-------------

.. include:: /_build/inc/esp_tls.inc

