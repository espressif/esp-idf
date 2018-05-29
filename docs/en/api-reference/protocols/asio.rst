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
ESP platform port currently supports only network asynchronous socket operations; does not support serial port and ssl.
Internal asio settings for ESP include
- EXCEPTIONS: Supported, choice in menuconfig
- SIGNAL, SIGACTION: Not supported
- EPOLL, EVENTFD: Not supported
- TYPEID: Disabled by default, but supported in toolchain and asio (provided stdlib recompiled with -frtti)

Application Example
-------------------
ESP examples are based on standard asio examples `examples/protocols/asio`:
- udp_echo_server
- tcp_echo_server
- chat_client
- chat_server
Please refer to the specific example README.md for details


