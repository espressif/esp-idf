TCP/IP Adapter
==============

The purpose of TCP/IP Adapter library is twofold:

- It provides an abstraction layer for the application on top of the TCP/IP stack. This will allow applications to choose between IP stacks in the future.
- The APIs it provides are thread safe, even if the underlying TCP/IP stack APIs are not.

ESP-IDF currently implements TCP/IP Adapter for the lwIP TCP/IP stack only. However, the adapter itself is TCP/IP implementation agnostic and different implementations are possible.

Some TCP/IP Adapter API functions are intended to be called by application code, for example to get/set interface IP addresses, configure DHCP. Other functions are intended for internal ESP-IDF use by the network driver layer.

In many cases, applications do not need to call TCP/IP Adapter APIs directly as they are called from the default network event handlers.

API Reference
-------------

.. include:: /_build/inc/tcpip_adapter.inc

