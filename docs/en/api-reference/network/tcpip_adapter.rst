TCP/IP Adapter
==============

The purpose of TCP/IP Adapter library is twofold. First, it provides an abstraction layer for the application on top of the IP stack, to allow applications to choose between IP stacks in the future. Second, the APIs it provides are thread safe, even if the underlying IP stack APIs are not.

In many cases, application does not need to call TCP/IP Adapter APIs itself. A few cases when such calls are needed are related to :doc:`event handling <../../api-guides/event-handling>`.


The aim of this adapter is to provide an abstraction layer between the TCP/IP stack and the network interface layer (currently Wi-Fi or Ethernet), and general network interface management.

ESP-IDF currently implements TCP/IP Adapter for the :doc:`lwIP <lwip>` TCP/IP stack only. However, the adapter itself is TCP/IP implementation agnostic and different implementations are possible.

Some TCP/IP Adapter API functions are intended to be called by application code, for example to get/set interface IP addresses, configure DHCP. Other functions are intended for internal ESP-IDF use by the network driver layer.


API Reference
-------------

.. include:: /_build/inc/tcpip_adapter.inc
.. include:: /_build/inc/tcpip_adapter_lwip.inc
