ESP-NETIF Custom I/O Driver
===========================

This section outlines implementing a new I/O driver with esp-netif connection capabilities.
By convention the I/O driver has to register itself as an esp-netif driver and thus holds a dependency on esp-netif component
and is responsible for providing data path functions, post-attach callback and in most cases also default event handlers to define network interface
actions based on driver's lifecycle transitions.


Packet input/output
^^^^^^^^^^^^^^^^^^^

As shown in the diagram, the following three API functions for the packet data path must be defined for connecting with esp-netif:

* :cpp:func:`esp_netif_transmit()`
* :cpp:func:`esp_netif_free_rx_buffer()`
* :cpp:func:`esp_netif_receive()`

The first two functions for transmitting and freeing the rx buffer are provided as callbacks, i.e. they get called from
esp-netif (and its underlying TCP/IP stack) and I/O driver provides their implementation.

The receiving function on the other hand gets called from the I/O driver, so that the driver's code simply calls :cpp:func:`esp_netif_receive()`
on a new data received event.


Post attach callback
^^^^^^^^^^^^^^^^^^^^

A final part of the network interface initialization consists of attaching the esp-netif instance to the I/O driver, by means
of calling the following API:

.. code:: c

    esp_err_t esp_netif_attach(esp_netif_t *esp_netif, esp_netif_iodriver_handle driver_handle);

It is assumed that the ``esp_netif_iodriver_handle`` is a pointer to driver's object, a struct derived from ``struct esp_netif_driver_base_s``,
so that the first member of I/O driver structure must be this base structure with pointers to

* post-attach function callback
* related esp-netif instance

As a consequence the I/O driver has to create an instance of the struct per below:

.. code:: c

    typedef struct my_netif_driver_s {
            esp_netif_driver_base_t base;           /*!< base structure reserved as esp-netif driver */
            driver_impl             *h;             /*!< handle of driver implementation */
        } my_netif_driver_t;

with actual values of ``my_netif_driver_t::base.post_attach`` and the actual drivers handle ``my_netif_driver_t::h``.
So when the :cpp:func:`esp_netif_attach()` gets called from the initialization code, the post-attach callback from I/O driver's code
gets executed to mutually register callbacks between esp-netif and I/O driver instances. Typically the driver is started
as well in the post-attach callback. An example of a simple post-attach callback is outlined below:

.. code:: c

    static esp_err_t my_post_attach_start(esp_netif_t * esp_netif, void * args)
    {
        my_netif_driver_t *driver = args;
        const esp_netif_driver_ifconfig_t driver_ifconfig = {
                .driver_free_rx_buffer = my_free_rx_buf,
                .transmit = my_transmit,
                .handle = driver->driver_impl
        };
        driver->base.netif = esp_netif;
        ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_ifconfig));
        my_driver_start(driver->driver_impl);
        return ESP_OK;
    }


Default handlers
^^^^^^^^^^^^^^^^

I/O drivers also typically provide default definitions of lifecycle behaviour of related network interfaces based
on state transitions of I/O drivers. For example *driver start* ``->`` *network start*, etc.
An example of such a default handler is provided below:

.. code:: c

    esp_err_t my_driver_netif_set_default_handlers(my_netif_driver_t *driver, esp_netif_t * esp_netif)
    {
        driver_set_event_handler(driver->driver_impl, esp_netif_action_start, MY_DRV_EVENT_START, esp_netif);
        driver_set_event_handler(driver->driver_impl, esp_netif_action_stop, MY_DRV_EVENT_STOP, esp_netif);
        return ESP_OK;
    }


Network stack connection
------------------------

The packet data path functions for transmitting and freeing the rx buffer (defined in the I/O driver) are called from
the esp-netif, specifically from its TCP/IP stack connecting layer.

Note, that IDF provides several network stack configurations for the most common network interfaces, such as for the WiFi station or Ethernet.
These configurations are defined in :component_file:`esp_netif/include/esp_netif_defaults.h` and should be sufficient for most network drivers.
(In rare cases, expert users might want to define custom lwIP based interface layers; it is possible, but an explicit dependency to lwIP needs to be set)

The following API reference outlines these network stack interaction with the esp-netif:

.. include-build-file:: inc/esp_netif_net_stack.inc
