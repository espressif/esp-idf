ASIO port
=========

Overview
--------
Asio is a cross-platform C++ library, see https://think-async.com. It provides a consistent asynchronous model using a modern C++ approach.


ASIO documentation
^^^^^^^^^^^^^^^^^^
Please refer to the original asio documentation at https://think-async.com/Asio/Documentation.
Asio also comes with a number of examples which could be find under Documentation/Examples on that web site.

Supported features
^^^^^^^^^^^^^^^^^^
ESP platform port currently supports only network asynchronous socket operations; does not support serial port.
SSL/TLS support is disabled by default and could be enabled in component configuration menu by choosing TLS library from

- mbedTLS with OpenSSL translation layer (default option)
- wolfSSL

SSL support is very basic at this stage and it does include following features:

- Verification callbacks
- DH property files
- Certificates/private keys file APIs

Internal asio settings for ESP include

- EXCEPTIONS are enabled in ASIO if enabled in menuconfig
- TYPEID is enabled in ASIO if enabled in menuconfig

Application Example
-------------------
ESP examples are based on standard asio :example:`protocols/asio`:

- :example:`protocols/asio/udp_echo_server`
- :example:`protocols/asio/tcp_echo_server`
- :example:`protocols/asio/asio_chat`
- :example:`protocols/asio/ssl_client_server`

Please refer to the specific example README.md for details
