ESP-NETIF
=========

The purpose of ESP-NETIF library is twofold:

- It provides an abstraction layer for the application on top of the TCP/IP stack. This will allow applications to choose between IP stacks in the future.
- The APIs it provides are thread safe, even if the underlying TCP/IP stack APIs are not.

ESP-IDF currently implements ESP-NETIF for the lwIP TCP/IP stack only. However, the adapter itself is TCP/IP implementation agnostic and different implementations are possible.

Some ESP-NETIF API functions are intended to be called by application code, for example to get/set interface IP addresses, configure DHCP. Other functions are intended for internal ESP-IDF use by the network driver layer.

In many cases, applications do not need to call ESP-NETIF APIs directly as they are called from the default network event handlers.

ESP-NETIF component is a successor of the tcpip_adapter, former network interface abstraction, which has become deprecated since IDF v4.1.
Please refer to the :doc:`/api-reference/network/tcpip_adapter_migration` section in case existing applications to be ported to use the esp-netif API instead.

ESP-NETIF architecture
----------------------

.. code-block:: text

                     |          (A) USER CODE                 |
                     |                                        |
        .............| init          settings      events     |
        .            +----------------------------------------+
        .               .                |           *
        .               .                |           *
    --------+        +===========================+   *     +-----------------------+
            |        | new/config     get/set    |   *     |                       |
            |        |                           |...*.....| init                  |
            |        |---------------------------|   *     |                       |
      init  |        |                           |****     |                       |
      start |********|  event handler            |*********|  DHCP                 |
      stop  |        |                           |         |                       |
            |        |---------------------------|         |                       |
            |        |                           |         |    NETIF              |
      +-----|        |                           |         +-----------------+     |
      | glue|----<---|  esp_netif_transmit       |--<------| netif_output    |     |
      |     |        |                           |         |                 |     |
      |     |---->---|  esp_netif_receive        |-->------| netif_input     |     |
      |     |        |                           |         + ----------------+     |
      |     |....<...|  esp_netif_free_rx_buffer |...<.....| packet buffer         |
      +-----|        |                           |         |                       |
            |        |                           |         |         (D)           |
      (B)   |        |          (C)              |         +-----------------------+
    --------+        +===========================+
    communication                                                NETWORK STACK
    DRIVER                   ESP-NETIF


Data and event flow in the diagram
----------------------------------

* ``........``     Initialization line from user code to ESP-NETIF and communication driver

* ``--<--->--``    Data packets going from communication media to TCP/IP stack and back

* ``********``     Events aggregated in ESP-NETIF propagates to driver, user code and network stack

* ``|``           User settings and runtime configuration

ESP-NETIF interaction
---------------------

A) User code, boiler plate
^^^^^^^^^^^^^^^^^^^^^^^^^^

Overall application interaction with a specific IO driver for communication media and configured TCP/IP network stack
is abstracted using ESP-NETIF APIs and outlined as below:

A) Initialization code

  1) Initializes IO driver
  2) Creates a new instance of ESP-NETIF and configure with

    * ESP-NETIF specific options (flags, behaviour, name)
    * Network stack options (netif init and input functions, not publicly available)
    * IO driver specific options (transmit, free rx buffer functions, IO driver handle)

  3) Attaches the IO driver handle to the ESP-NETIF instance created in the above steps
  4) Configures event handlers

    * use default handlers for common interfaces defined in IO drivers; or define a specific handlers for customised behaviour/new interfaces
    * register handlers for app related events (such as IP lost/acquired)

B) Interaction with network interfaces using ESP-NETIF API

  * Getting and setting TCP/IP related parameters (DHCP, IP, etc)
  * Receiving IP events (connect/disconnect)
  * Controlling application lifecycle (set interface up/down)


B) Communication driver, IO driver, media driver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Communication driver plays these two important roles in relation with ESP-NETIF:

1) Event handlers: Define behaviour patterns of interaction with ESP-NETIF (for example: ethernet link-up -> turn netif on)

2) Glue IO layer: Adapts the input/output functions to use ESP-NETIF transmit, receive and free receive buffer

  * Installs driver_transmit to appropriate ESP-NETIF object, so that outgoing packets from network stack are passed to the IO driver
  * Calls :cpp:func:`esp_netif_receive()` to pass incoming data to network stack


C) ESP-NETIF, former tcpip_adapter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ESP-NETIF is an intermediary between an IO driver and a network stack, connecting packet data path between these two.
As that it provides a set of interfaces for attaching a driver to ESP-NETIF object (runtime) and
configuring a network stack (compile time). In addition to that a set of API is provided to control network interface lifecycle
and its TCP/IP properties. As an overview, the ESP-NETIF public interface could be divided into these 6 groups:

1) Initialization APIs (to create and configure ESP-NETIF instance)
2) Input/Output API (for passing data between IO driver and network stack)
3) Event or Action API

  * Used for network interface lifecycle management
  * ESP-NETIF provides building blocks for designing event handlers

4) Setters and Getters for basic network interface properties
5) Network stack abstraction: enabling user interaction with TCP/IP stack

  * Set interface up or down
  * DHCP server and client API
  * DNS API

6) Driver conversion utilities


D) Network stack
^^^^^^^^^^^^^^^^

Network stack has no public interaction with application code with regard to public interfaces and shall be fully abstracted by
ESP-NETIF API.


ESP-NETIF programmer's manual
-----------------------------

Please refer to the example section for basic initialization of default interfaces:

- WiFi Station: :example_file:`wifi/getting_started/station/main/station_example_main.c`
- WiFi Access Point: :example_file:`wifi/getting_started/softAP/main/softap_example_main.c`
- Ethernet: :example_file:`ethernet/basic/main/ethernet_example_main.c`

For more specific cases please consult this guide: :doc:`/api-reference/network/esp_netif_driver`.


WiFi default initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The initialization code as well as registering event handlers for default interfaces,
such as softAP and station, are provided in two separate APIs to facilitate simple startup code for most applications:

* :cpp:func:`esp_netif_create_default_wifi_ap()`
* :cpp:func:`esp_netif_create_default_wifi_sta()`

Please note that these functions return the ``esp_netif`` handle, i.e. a pointer to a network interface object allocated and
configured with default settings, which as a consequence, means that:

* The created object has to be destroyed if a network de-initialization is provided by an application.
* These *default* interfaces must not be created multiple times, unless the created handle is deleted using :cpp:func:`esp_netif_destroy()`.
* When using Wifi in ``AP+STA`` mode, both these interfaces has to be created.


API Reference
-------------

.. include-build-file:: inc/esp_netif.inc


WiFi default API reference
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. include-build-file:: inc/esp_wifi_default.inc
