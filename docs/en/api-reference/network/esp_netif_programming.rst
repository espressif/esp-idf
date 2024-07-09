ESP-NETIF Programmers Manual
============================

:link_to_translation:`zh_CN:[中文]`

.. _esp_netif_set_ip:

Configure IP, Gateway, and DNS
------------------------------

Typically, IP addresses -- including the gateway, network mask, and DNS servers -- are automatically obtained through a DHCP server or Router Advertisement services. Notifications regarding IP address assignments are received via the ``IP_EVENT``, which provides the relevant IP address information. You can also retrieve the current address details using the function :cpp:func:`esp_netif_get_ip_info()`. This function returns a structure, :cpp:type:`esp_netif_ip_info_t`, that contains the IPv4 address of the network interface, along with its network mask and gateway address. Similarly, the function :cpp:func:`esp_netif_get_all_ip6()` can be used to obtain all IPv6 addresses associated with the interface.

In order to configure static IP addresses and DNS servers, it's necessary to disable or stop DHCP client (which is enabled by default on some network interfaces, such as the default Ethernet, or the default WiFi station). Please refer to the example :example:`/protocols/static_ip` for more details.

To set IPv4 address, you can use :cpp:func:`esp_netif_set_ip_info()`. For IPv6, these two functions can be used for adding or removing addresses: :cpp:func:`esp_netif_add_ip6_address()`,  :cpp:func:`esp_netif_remove_ip6_address()`.
To configure DNS servers, please use :cpp:func:`esp_netif_set_dns_info()` API.

.. _esp_netif_set_dhcp:

Configure DHCP options
----------------------

Some network interfaces are pre-configured to use either a DHCP client (commonly for Ethernet interfaces) or a DHCP server (typically for Wi-Fi software access points). When manually creating a custom network interface, the configuration flags :cpp:type:`esp_netif_flags_t` are used to specify the behavior of the interface. Adding :cpp:enumerator:`ESP_NETIF_DHCP_CLIENT` or :cpp:enumerator:`ESP_NETIF_DHCP_SERVER` will enable the DHCP client or server, respectively.

It is important to note that these two options are mutually exclusive and cannot be changed at runtime. If an interface is configured as a DHCP client upon creation, it cannot later be used as a DHCP server. The only option is to destroy the existing network interface and create a new one with the desired configuration.

To set or get a specific DHCP option, the common type :cpp:type:`esp_netif_dhcp_option_id_t` is used for both the DHCP server and client. However, not all options are supported for both. For details on the available options for the DHCP client, refer to the API documentation for :cpp:func:`esp_netif_dhcpc_option()`. Similarly, for the options available for the DHCP server, consult the API documentation for :cpp:func:`esp_netif_dhcps_option()`.

.. _esp_netif-sntp-api:

SNTP Service
------------

A brief introduction to SNTP, its initialization code, and basic modes can be found in Section :ref:`system-time-sntp-sync` in :doc:`System Time </api-reference/system/system_time>`.

This section provides more details on specific use cases for the SNTP service, such as using statically configured servers, DHCP-provided servers, or both. The workflow is typically straightforward:

1. Initialize and configure the service using :cpp:func:`esp_netif_sntp_init()`. This function can only be called once unless the SNTP service has been destroyed using :cpp:func:`esp_netif_sntp_deinit()`.
2. Start the service with :cpp:func:`esp_netif_sntp_start()`. This step is not necessary if the service was auto-started in the previous step (default behavior). However, it can be useful to start the service explicitly after connecting if DHCP-provided NTP servers are being used. Note that this option needs to be enabled before connecting, but the SNTP service should only be started afterward.
3. Wait for the system time to synchronize using :cpp:func:`esp_netif_sntp_sync_wait()` (if required).
4. Stop and destroy the service using :cpp:func:`esp_netif_sntp_deinit()`.


Basic Mode with Statically Defined Server(s)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Initialize the module with the default configuration after connecting to the network. Note that it is possible to provide multiple NTP servers in the configuration struct:

.. code-block:: c

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(2,
                               ESP_SNTP_SERVER_LIST("time.windows.com", "pool.ntp.org" ) );
    esp_netif_sntp_init(&config);

.. note::

    If you want to configure multiple SNTP servers, update the lwIP configuration option :ref:`CONFIG_LWIP_SNTP_MAX_SERVERS`.


Use DHCP-Obtained SNTP Server(s)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First, you need to enable the lwIP configuration option :ref:`CONFIG_LWIP_DHCP_GET_NTP_SRV`. Then, initialize the SNTP module with the DHCP option, without specifying an NTP server.

.. code-block:: c

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(0, {} );
    config.start = false;                       // start the SNTP service explicitly
    config.server_from_dhcp = true;             // accept the NTP offer from the DHCP server
    esp_netif_sntp_init(&config);

Once connected, you can start the service using:

.. code-block:: c

    esp_netif_sntp_start();

.. note::

    It is also possible to start the service during initialization (with the default ``config.start=true``). However, this may cause the initial SNTP request to fail since you are not connected yet, which could result in a back-off period for subsequent requests.


Use Both Static and Dynamic Servers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This scenario is similar to using DHCP-provided SNTP servers. However, in this configuration, you need to ensure that the static server configuration is refreshed when obtaining NTP servers via DHCP. The underlying lwIP code removes the existing list of NTP servers when DHCP-provided information is accepted. Therefore, the ESP-NETIF SNTP module retains the statically configured server(s) and appends them to the list after obtaining a DHCP lease.

The typical configuration now looks as per below, providing the specific ``IP_EVENT`` to update the config and index of the first server to reconfigure (for example setting ``config.index_of_first_server=1`` would keep the DHCP provided server at index 0, and the statically configured server at index 1).

.. code-block:: c

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    config.start = false;                       // start the SNTP service explicitly (after connecting)
    config.server_from_dhcp = true;             // accept the NTP offers from DHCP server
    config.renew_servers_after_new_IP = true;   // let esp-netif update the configured SNTP server(s) after receiving the DHCP lease
    config.index_of_first_server = 1;           // updates from server num 1, leaving server 0 (from DHCP) intact
    config.ip_event_to_renew = IP_EVENT_STA_GOT_IP;  // IP event on which you refresh your configuration

Then you start the service normally with  :cpp:func:`esp_netif_sntp_start()`.


.. _esp_netif_l2tap:

L2 TAP Interface Usage
----------------------

The ESP-NETIF L2 TAP interface is used to access Data Link Layer, please refer to the :ref:`esp-netif structure` diagram to see how L2 TAP interacts with ESP-NETIF and application.

From a user perspective, the ESP-NETIF L2 TAP interface is accessed using file descriptors of VFS, which provides file-like interfacing (using functions like ``open()``, ``read()``, ``write()``, etc). To learn more, refer to :doc:`/api-reference/storage/vfs`.

There is only one ESP-NETIF L2 TAP interface device (path name) available, but you can open multiple file descriptors from it, each with its own unique configuration. Think of the ESP-NETIF L2 TAP interface as a general gateway to the Layer 2 network infrastructure. The key point is that each file descriptor can be individually configured, which is crucial. For example, a file descriptor can be set up to access a specific network interface identified by if_key (like ETH_DEF) and to filter specific types of frames (such as filtering by Ethernet type for IEEE 802.3 frames).

This filtering is essential because the ESP-NETIF L2 TAP works alongside the IP stack, meaning that IP-related traffic (like IP, ARP, etc.) should not be sent directly to your application. Although this option is still possible, it is not recommended in standard use cases. The benefit of filtering is that it allows your application to receive only the frame types it cares about, while other traffic is either routed to different L2 TAP file descriptors or handled by the IP stack.


Initialization
^^^^^^^^^^^^^^
To be able to use the ESP-NETIF L2 TAP interface, it needs to be enabled in Kconfig by :ref:`CONFIG_ESP_NETIF_L2_TAP` first and then registered by :cpp:func:`esp_vfs_l2tap_intf_register()` prior usage of any VFS function.

``open()``
^^^^^^^^^^
Once the ESP-NETIF L2 TAP is registered, it can be opened at path name "/dev/net/tap". The same path name can be opened multiple times up to :ref:`CONFIG_ESP_NETIF_L2_TAP_MAX_FDS` and multiple file descriptors with a different configuration may access the Data Link Layer frames.

The ESP-NETIF L2 TAP can be opened with the ``O_NONBLOCK`` file status flag to make sure the ``read()`` does not block. Note that the ``write()`` may block in the current implementation when accessing a Network interface since it is a shared resource among multiple ESP-NETIF L2 TAP file descriptors and IP stack, and there is currently no queuing mechanism deployed. The file status flag can be retrieved and modified using ``fcntl()``.

On success, ``open()`` returns the new file descriptor (a nonnegative integer). On error, -1 is returned, and ``errno`` is set to indicate the error.

``ioctl()``
^^^^^^^^^^^
The newly opened ESP-NETIF L2 TAP file descriptor needs to be configured prior to its usage since it is not bounded to any specific Network Interface and no frame type filter is configured. The following configuration options are available to do so:

  * ``L2TAP_S_INTF_DEVICE`` - bounds the file descriptor to a specific Network Interface that is identified by its ``if_key``. ESP-NETIF Network Interface ``if_key`` is passed to ``ioctl()`` as the third parameter. Note that default Network Interfaces ``if_key``'s used in ESP-IDF can be found in :component_file:`esp_netif/include/esp_netif_defaults.h`.
  * ``L2TAP_S_DEVICE_DRV_HNDL`` - is another way to bound the file descriptor to a specific Network Interface. In this case, the Network interface is identified directly by IO Driver handle (e.g., :cpp:type:`esp_eth_handle_t` in case of Ethernet). The IO Driver handle is passed to ``ioctl()`` as the third parameter.
  * ``L2TAP_S_RCV_FILTER`` - sets the filter to frames with the type to be passed to the file descriptor. In the case of Ethernet frames, the frames are to be filtered based on the Length and Ethernet type field. In case the filter value is set less than or equal to 0x05DC, the Ethernet type field is considered to represent IEEE802.3 Length Field, and all frames with values in interval <0, 0x05DC> at that field are passed to the file descriptor. The IEEE802.2 logical link control (LLC) resolution is then expected to be performed by the user's application. In case the filter value is set greater than 0x05DC, the Ethernet type field is considered to represent protocol identification and only frames that are equal to the set value are to be passed to the file descriptor.

All above-set configuration options have a getter counterpart option to read the current settings.

.. warning::
    The file descriptor needs to be firstly bounded to a specific Network Interface by ``L2TAP_S_INTF_DEVICE`` or ``L2TAP_S_DEVICE_DRV_HNDL`` to make ``L2TAP_S_RCV_FILTER`` option available.

.. note::
    VLAN-tagged frames are currently not recognized. If the user needs to process VLAN-tagged frames, they need a set filter to be equal to the VLAN tag (i.e., 0x8100 or 0x88A8) and process the VLAN-tagged frames in the user application.

.. note::
    ``L2TAP_S_DEVICE_DRV_HNDL`` is particularly useful when the user's application does not require the usage of an IP stack and so ESP-NETIF is not required to be initialized too. As a result, Network Interface cannot be identified by its ``if_key`` and hence it needs to be identified directly by its IO Driver handle.

| On success, ``ioctl()`` returns 0. On error, -1 is returned, and ``errno`` is set to indicate the error.
| * EBADF - not a valid file descriptor.
| * EACCES - options change is denied in this state (e.g., file descriptor has not been bounded to Network interface yet).
| * EINVAL - invalid configuration argument. Ethernet type filter is already used by other file descriptors on that same Network interface.
| * ENODEV - no such Network Interface which is tried to be assigned to the file descriptor exists.
| * ENOSYS - unsupported operation, passed configuration option does not exist.

``fcntl()``
^^^^^^^^^^^
``fcntl()`` is used to manipulate with properties of opened ESP-NETIF L2 TAP file descriptor.

The following commands manipulate the status flags associated with the file descriptor:

  * ``F_GETFD`` - the function returns the file descriptor flags, and the third argument is ignored.
  * ``F_SETFD`` - sets the file descriptor flags to the value specified by the third argument. Zero is returned.

| On success, ``ioctl()`` returns 0. On error, -1 is returned, and ``errno`` is set to indicate the error.
| * EBADF - not a valid file descriptor.
| * ENOSYS - unsupported command.

``read()``
^^^^^^^^^^
Opened and configured ESP-NETIF L2 TAP file descriptor can be accessed by ``read()`` to get inbound frames. The read operation can be either blocking or non-blocking based on the actual state of the ``O_NONBLOCK`` file status flag. When the file status flag is set to blocking, the read operation waits until a frame is received and the context is switched to other tasks. When the file status flag is set to non-blocking, the read operation returns immediately. In such case, either a frame is returned if it was already queued or the function indicates the queue is empty. The number of queued frames associated with one file descriptor is limited by :ref:`CONFIG_ESP_NETIF_L2_TAP_RX_QUEUE_SIZE` Kconfig option. Once the number of queued frames reached a configured threshold, the newly arrived frames are dropped until the queue has enough room to accept incoming traffic (Tail Drop queue management).

| On success, ``read()`` returns the number of bytes read. Zero is returned when the size of the destination buffer is 0. On error, -1 is returned, and ``errno`` is set to indicate the error.
| * EBADF - not a valid file descriptor.
| * EAGAIN - the file descriptor has been marked non-blocking (``O_NONBLOCK``), and the read would block.

``write()``
^^^^^^^^^^^
A raw Data Link Layer frame can be sent to Network Interface via opened and configured ESP-NETIF L2 TAP file descriptor. The user's application is responsible to construct the whole frame except for fields which are added automatically by the physical interface device. The following fields need to be constructed by the user's application in case of an Ethernet link: source/destination MAC addresses, Ethernet type, actual protocol header, and user data. The length of these fields is as follows:

.. list-table::
    :header-rows: 1
    :widths: 20 20 20 30
    :align: center

    * - Destination MAC
      - Source MAC
      - Type/Length
      - Payload (protocol header/data)
    * - 6 B
      - 6 B
      - 2 B
      - 0-1486 B

In other words, there is no additional frame processing performed by the ESP-NETIF L2 TAP interface. It only checks the Ethernet type of the frame is the same as the filter configured in the file descriptor. If the Ethernet type is different, an error is returned and the frame is not sent. Note that the ``write()`` may block in the current implementation when accessing a Network interface since it is a shared resource among multiple ESP-NETIF L2 TAP file descriptors and IP stack, and there is currently no queuing mechanism deployed.

| On success, ``write()`` returns the number of bytes written. Zero is returned when the size of the input buffer is 0. On error, -1 is returned, and ``errno`` is set to indicate the error.
| * EBADF - not a valid file descriptor.
| * EBADMSG - The Ethernet type of the frame is different from the file descriptor configured filter.
| * EIO - Network interface not available or busy.

``close()``
^^^^^^^^^^^
Opened ESP-NETIF L2 TAP file descriptor can be closed by the ``close()`` to free its allocated resources. The ESP-NETIF L2 TAP implementation of ``close()`` may block. On the other hand, it is thread-safe and can be called from a different task than the file descriptor is actually used. If such a situation occurs and one task is blocked in the I/O operation and another task tries to close the file descriptor, the first task is unblocked. The first's task read operation then ends with an error.

| On success, ``close()`` returns zero. On error, -1 is returned, and ``errno`` is set to indicate the error.
| * EBADF - not a valid file descriptor.

``select()``
^^^^^^^^^^^^
Select is used in a standard way, just :ref:`CONFIG_VFS_SUPPORT_SELECT` needs to be enabled to make the ``select()`` function available.


.. _esp_netif_other_events:

IP Event: Transmit/Receive Packet
---------------------------------

This event, ``IP_EVENT_TX_RX``, is triggered for every transmitted or received IP packet. It provides information about packet transmission or reception, data length, and the ``esp_netif`` handle.

Enabling the Event
^^^^^^^^^^^^^^^^^^

**Compile Time:**

The feature can be completely disabled during compilation time using the flag :ref:`CONFIG_ESP_NETIF_REPORT_DATA_TRAFFIC` in the kconfig.

**Run Time:**

At runtime, you can enable or disable this event using the functions :cpp:func:`esp_netif_tx_rx_event_enable()` and :cpp:func:`esp_netif_tx_rx_event_disable()`.

Event Registration
^^^^^^^^^^^^^^^^^^

To handle this event, you need to register a handler using the following syntax:

.. code-block:: c

    static void
    tx_rx_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data)
    {
        ip_event_tx_rx_t *event = (ip_event_tx_rx_t *)event_data;

        if (event->dir == ESP_NETIF_TX) {
            ESP_LOGI(TAG, "Got TX event: Interface \"%s\" data len: %d", esp_netif_get_desc(event->esp_netif), event->len);
        } else if (event->dir == ESP_NETIF_RX) {
            ESP_LOGI(TAG, "Got RX event: Interface \"%s\" data len: %d", esp_netif_get_desc(event->esp_netif), event->len);
        } else {
            ESP_LOGI(TAG, "Got Unknown event: Interface \"%s\"", esp_netif_get_desc(event->esp_netif));
        }
    }

    esp_event_handler_register(IP_EVENT, IP_EVENT_TX_RX, &tx_rx_event_handler, NULL);

Here, ``tx_rx_event_handler`` is the name of the function that will handle the event.

Event Data Structure
^^^^^^^^^^^^^^^^^^^^

The event data structure, :cpp:class:`ip_event_tx_rx_t`, contains the following fields:

- :cpp:member:`ip_event_tx_rx_t::dir`: Indicates whether the packet was transmitted (``ESP_NETIF_TX``) or received (``ESP_NETIF_RX``).
- :cpp:member:`ip_event_tx_rx_t::len`: Length of the data frame.
- :cpp:member:`ip_event_tx_rx_t::esp_netif`: The network interface on which the packet was sent or received.


.. _esp_netif_api_reference:

API Reference
-------------

.. include-build-file:: inc/esp_netif.inc
.. include-build-file:: inc/esp_netif_sntp.inc
.. include-build-file:: inc/esp_netif_types.inc
.. include-build-file:: inc/esp_netif_ip_addr.inc
.. include-build-file:: inc/esp_vfs_l2tap.inc

.. only:: SOC_WIFI_SUPPORTED

    Wi-Fi Default API Reference
    ---------------------------

    .. include-build-file:: inc/esp_wifi_default.inc
