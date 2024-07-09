ESP-NETIF
*********

:link_to_translation:`zh_CN:[中文]`

The purpose of the ESP-NETIF library is twofold:

- It provides an abstraction layer for the application on top of the TCP/IP stack. This allows applications to choose between IP stacks in the future.
- The APIs it provides are thread-safe, even if the underlying TCP/IP stack APIs are not.

ESP-IDF currently implements ESP-NETIF for the lwIP TCP/IP stack only. However, the adapter itself is TCP/IP implementation-agnostic and allows different implementations.

It is also possible to use a custom TCP/IP stack with ESP-IDF, provided it implements BSD API. For more information on building ESP-IDF without lwIP, please refer to :idf_file:`components/esp_netif_stack/README.md`.

Some ESP-NETIF API functions are intended to be called by application code, for example, to get or set interface IP addresses, and configure DHCP. Other functions are intended for internal ESP-IDF use by the network driver layer.

In many cases, applications do not need to call ESP-NETIF APIs directly as they are called by the default network event handlers.

If you are only interested in using the most common network interfaces with default setting, please read :ref:`esp_netif_user` to see how you can initialize default interfaces and register event handlers.

If you would like to learn more about the library interaction with other components, please refer to the :ref:`esp-netif structure`.

In case your application needs to configure the network interfaces differently, e.g. setting a static IP address or just update the configuration runtime, please read :ref:`esp_netif_programmer`.

If you would like to develop your own network driver, implement support for a new TCP/IP stack or customize the ESP-NETIF in some other way, please refer to the :ref:`esp_netif_developer`.

.. _esp_netif_user:

ESP-NETIF User's Manual
=======================

It is usually just enough to create a default network interface after startup and destroy it upon closing (see :ref:`esp_netif_init`). It is also necessary to receive a notification upon assigning a new IP address or losing it (see :ref:`esp-netif-ip-events`).


.. _esp_netif_init:

Initialization
--------------

Since the ESP-NETIF component uses system events, the typical network startup code looks like this (note, that error handling is omitted for clarity):

.. code-block:: c

      // 1) Initialize the TCP/IP stack and the event loop
      esp_netif_init();
      esp_event_loop_create_default();

      // 2) Create the network interface handle
      esp_netif = esp_netif_new(&config);

      // 3) Create the communication driver (e.g. Ethernet) and it's network layer glue
      // and register the ESP-NETIF event (e.g. to bring the interface up upon link-up event)
      esp_netif_glue_t glue = driver_glue(driver);

      // 4) Attach the driver's glue layer to the network interface handle
      esp_netif_attach(esp_netif, glue);

      // 5) Register user-side event handlers
      esp_event_handler_register(DRIVER_EVENT, ...);  // to observe driver states, e.g. link-up
      esp_event_handler_register(IP_EVENT, ...);      // to observer ESP-NETIF states, e.g. get an IP


.. note::

    These 5 steps need to be performed in the exact order as shown above, mainly due to the default event loop, which is used by network interface drivers to register system events.

      - Default event loop needs to be created **before** initializing an interface driver (as it typically needs register system even handlers)
      - Registering application event handlers need to happen **after** calling :cpp:func:`esp_netif_attach`, because the event handlers are called in the order they were registered and we need to have the system handlers called first.

    Steps ``2)``, ``3)`` and ``4)`` are quite complex for most common use-cases, so  ESP-NETIF provides some pre-configured interfaces and convenience functions that create the most common network interfaces in their most common configurations.

.. note::

    Each network interface needs to be initialized separately, so if you would like to use multiple interfaces, you would have to run steps ``2)`` to ``5)`` for every interface. Set ``1)`` should be performed only once.


Creating and configuring the interface and attaching the communication driver to it (steps ``2)``, ``3)`` and ``4)``) is described in :ref:`create_esp_netif`.
Using the ESP-NETIF event handlers (step ``5)``) is described in :ref:`esp-netif-ip-events`.


.. _create_esp_netif:

Common network interfaces
^^^^^^^^^^^^^^^^^^^^^^^^^

As the initialization of network interfaces could be quite complex, ESP-NETIF provides some convenient methods of creating the most common onc, such as Wi-Fi and Ethernet.

Please refer to the following example to understand the initialization process of the default interface:


.. only:: SOC_WIFI_SUPPORTED

    - Wi-Fi Station: :example_file:`wifi/getting_started/station/main/station_example_main.c`

- Ethernet: :example_file:`ethernet/basic/main/ethernet_example_main.c`

- L2 TAP: :example_file:`protocols/l2tap/main/l2tap_main.c`

.. only:: CONFIG_ESP_WIFI_SOFTAP_SUPPORT

    - Wi-Fi Access Point: :example_file:`wifi/getting_started/softAP/main/softap_example_main.c`

.. only:: SOC_WIFI_SUPPORTED

Wi-Fi Default Initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The initialization code as well as registering event handlers for default interfaces, such as softAP and station, are provided in separate APIs to facilitate simple startup code for most applications:

* :cpp:func:`esp_netif_create_default_wifi_sta()`

.. only:: CONFIG_ESP_WIFI_SOFTAP_SUPPORT

    * :cpp:func:`esp_netif_create_default_wifi_ap()`

.. only:: SOC_WIFI_SUPPORTED

    Please note that these functions return the ``esp_netif`` handle, i.e., a pointer to a network interface object allocated and configured with default settings, as a consequence, which means that:

    * The created object has to be destroyed if a network de-initialization is provided by an application using :cpp:func:`esp_netif_destroy_default_wifi()`.

    * These *default* interfaces must not be created multiple times unless the created handle is deleted using :cpp:func:`esp_netif_destroy()`.


.. only:: CONFIG_ESP_WIFI_SOFTAP_SUPPORT

    * When using Wi-Fi in ``AP+STA`` mode, both these interfaces have to be created.

.. _esp-netif-ip-events:

IP Events
---------

In the last section of :ref:`esp_netif_init` code (step ``5)``), we register two sets of event handlers:

* Communication driver events: To be notified about driver's lifecycle states, for example when Wi-Fi station joined an AP, or when it gets disconnected. Handling these events is out of the scope of ESP-NETIF component, it is only worth mentioning that the same events are also used by ESP-NETIF to set the network interface to a desired state of, so if an application uses the driver's events to assume specific states of the network interface, it need to register their handlers **after** registering system handlers (which typically happen when attaching the driver to the interface). That is why the handler registration is performed in the last step of the :ref:`esp_netif_init` code.

* IP events: To be notified about IP address changes, that is when we get assigned a new address or when the valid address is lost. Specific types of these events are listed in :cpp:type:`ip_event_t`, each common interface has the related pair of ``GOT_IP`` and ``LOST_IP`` events.

.. note::

    Lost IP events are triggered by a timer configurable by :ref:`CONFIG_ESP_NETIF_IP_LOST_TIMER_INTERVAL`. The timer is started upon losing the IP address and the event will be raised after the configured interval, which is 120s by default. The event could be disabled when setting the interval to 0.



.. _esp-netif structure:

ESP-NETIF Architecture
----------------------

.. code-block:: text


                         |          (A) USER CODE                 |
                         |                 Apps                   |
        .................| init          settings      events     |
        .                +----------------------------------------+
        .                   .                |           *
        .                   .                |           *
    --------+            +===========================+   *     +-----------------------+
            |            | new/config   get/set/apps |   *     | init                  |
            |            |                           |...*.....| Apps (DHCP, SNTP)     |
            |            |---------------------------|   *     |                       |
      init  |            |                           |****     |                       |
      start |************|  event handler            |*********|  DHCP                 |
      stop  |            |                           |         |                       |
            |            |---------------------------|         |                       |
            |            |                           |         |    NETIF              |
      +-----|            |                           |         +-----------------+     |
      | glue|---<----|---|  esp_netif_transmit       |--<------| netif_output    |     |
      |     |        |   |                           |         |                 |     |
      |     |--->----|---|  esp_netif_receive        |-->------| netif_input     |     |
      |     |        |   |                           |         + ----------------+     |
      |     |...<....|...|  esp_netif_free_rx_buffer |...<.....| packet buffer         |
      +-----|     |  |   |                           |         |                       |
            |     |  |   |                           |         |         (D)           |
      (B)   |     |  |   |          (C)              |         +-----------------------+
    --------+     |  |   +===========================+
  COMMUNICATION   |  |                                               NETWORK STACK
  DRIVER          |  |           ESP-NETIF
                  |  |                                         +------------------+
                  |  |   +---------------------------+.........| open/close       |
                  |  |   |                           |         |                  |
                  |  -<--|  l2tap_write              |-----<---|  write           |
                  |      |                           |         |                  |
                  ---->--|  esp_vfs_l2tap_eth_filter |----->---|  read            |
                         |                           |         |                  |
                         |            (E)            |         +------------------+
                         +---------------------------+
                                                                     USER CODE
                               ESP-NETIF L2 TAP


Data and Event Flow in the Diagram
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* ``........``     Initialization line from user code to ESP-NETIF and communication driver

* ``--<--->--``    Data packets going from communication media to TCP/IP stack and back

* ``********``     Events aggregated in ESP-NETIF propagate to the driver, user code, and network stack

* ``|``            User settings and runtime configuration

ESP-NETIF Interaction
^^^^^^^^^^^^^^^^^^^^^

A) User Code, Boilerplate
'''''''''''''''''''''''''

Overall application interaction with a specific IO driver for communication media and configured TCP/IP network stack is abstracted using ESP-NETIF APIs and is outlined as below:

A) Initialization code

  1) Initializes IO driver
  2) Creates a new instance of ESP-NETIF and configure it with

    * ESP-NETIF specific options (flags, behavior, name)
    * Network stack options (netif init and input functions, not publicly available)
    * IO driver specific options (transmit, free rx buffer functions, IO driver handle)

  3) Attaches the IO driver handle to the ESP-NETIF instance created in the above steps
  4) Configures event handlers

    * Use default handlers for common interfaces defined in IO drivers; or define a specific handler for customized behavior or new interfaces
    * Register handlers for app-related events (such as IP lost or acquired)

B) Interaction with network interfaces using ESP-NETIF API

  1) Gets and sets TCP/IP-related parameters (DHCP, IP, etc)
  2) Receives IP events (connect or disconnect)
  3) Controls application lifecycle (set interface up or down)


B) Communication Driver, IO Driver, and Media Driver
''''''''''''''''''''''''''''''''''''''''''''''''''''

Communication driver plays these two important roles in relation to ESP-NETIF:

1) Event handlers: Defines behavior patterns of interaction with ESP-NETIF (e.g., ethernet link-up -> turn netif on)

2) Glue IO layer: Adapts the input or output functions to use ESP-NETIF transmit, receive, and free receive buffer

  * Installs driver_transmit to the appropriate ESP-NETIF object so that outgoing packets from the network stack are passed to the IO driver
  * Calls :cpp:func:`esp_netif_receive()` to pass incoming data to the network stack


C) ESP-NETIF
''''''''''''

ESP-NETIF serves as an intermediary between an IO driver and a network stack, connecting the packet data path between the two. It provides a set of interfaces for attaching a driver to an ESP-NETIF object at runtime and configures a network stack during compiling. Additionally, a set of APIs is provided to control the network interface lifecycle and its TCP/IP properties. As an overview, the ESP-NETIF public interface can be divided into six groups:

1) Initialization APIs (to create and configure ESP-NETIF instance)
2) Input or Output API (for passing data between IO driver and network stack)
3) Event or Action API

  * Used for network interface lifecycle management
  * ESP-NETIF provides building blocks for designing event handlers

4) Setters and Getters API for basic network interface properties
5) Network stack abstraction API: enabling user interaction with TCP/IP stack

  * Set interface up or down
  * DHCP server and client API
  * DNS API
  * :ref:`esp_netif-sntp-api`

6) Driver conversion utilities API


D) Network Stack
''''''''''''''''

The network stack has no public interaction with application code with regard to public interfaces and shall be fully abstracted by ESP-NETIF API.


E) ESP-NETIF L2 TAP Interface
'''''''''''''''''''''''''''''
The ESP-NETIF L2 TAP interface is a mechanism in ESP-IDF used to access Data Link Layer (L2 per OSI/ISO) for frame reception and transmission from the user application. Its typical usage in the embedded world might be the implementation of non-IP-related protocols, e.g., PTP, Wake on LAN. Note that only Ethernet (IEEE 802.3) is currently supported.

From a user perspective, the ESP-NETIF L2 TAP interface is accessed using file descriptors of VFS, which provides file-like interfacing (using functions like ``open()``, ``read()``, ``write()``, etc). To learn more, refer to :doc:`/api-reference/storage/vfs`.

There is only one ESP-NETIF L2 TAP interface device (path name) available. However multiple file descriptors with different configurations can be opened at a time since the ESP-NETIF L2 TAP interface can be understood as a generic entry point to the Layer 2 infrastructure. What is important is then the specific configuration of the particular file descriptor. It can be configured to give access to a specific Network Interface identified by ``if_key`` (e.g., `ETH_DEF`) and to filter only specific frames based on their type (e.g., Ethernet type in the case of IEEE 802.3). Filtering only specific frames is crucial since the ESP-NETIF L2 TAP needs to exist along with the IP stack and so the IP-related traffic (IP, ARP, etc.) should not be passed directly to the user application. Even though this option is still configurable, it is not recommended in standard use cases. Filtering is also advantageous from the perspective of the user's application, as it only gets access to the frame types it is interested in, and the remaining traffic is either passed to other L2 TAP file descriptors or to the IP stack.

.. _esp_netif_programmer:

ESP-NETIF Programmer's Manual
=============================

In same cases, it is not enough to simply initialize a network interface by default, start using it and connect to the local network. If so, please consult the programming guide: :doc:`/api-reference/network/esp_netif_programming`.

You would typically need to use specific sets of ESP-NETIF APIs in the following use-cases:

* :ref:`esp_netif_set_ip`
* :ref:`esp_netif_set_dhcp`
* :ref:`esp_netif-sntp-api`
* :ref:`esp_netif_l2tap`
* :ref:`esp_netif_other_events`
* :ref:`esp_netif_api_reference`

.. _esp_netif_developer:

ESP-NETIF Developer's Manual
============================

In some cases, user applications might need to customize ESP-NETIF, register custom drivers or even custom TCP/IP stacks. If so, please consult the programming guide: :doc:`/api-reference/network/esp_netif_driver`.
