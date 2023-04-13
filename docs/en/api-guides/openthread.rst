OpenThread
==========

`OpenThread <https://github.com/openthread/openthread>`_ is a IP stack running on the 802.15.4 MAC layer which features mesh network and low power consumption.

Mode of the OpenThread stack
----------------------------

OpenThread can run under the following modes on Espressif chips:

Standalone node
+++++++++++++++

The full OpenThread stack and the application layer runs on the same chip. This mode is available on chips with 15.4 radio such as ESP32-H2.

Radio Co-Processor (RCP)
++++++++++++++++++++++++

The chip will be connected to another host running the OpenThread IP stack. It will send and received 15.4 packets on behalf of the host. This mode is available on chips with 15.4 radio such as ESP32-H2. The underlying transport between the chip and the host can be SPI or UART. For sake of latency, we recommend to use SPI as the underlying transport.

OpenThread host
+++++++++++++++

For chips without 15.4 radio, it can be connected to an RCP and run OpenThread under host mode. This mode enables OpenThread on Wi-Fi chips such as ESP32, ESP32-S2, ESP32-S3 and ESP32-C3. The following diagram shows how devices work under different modes:

.. blockdiag::
    :caption: OpenThread device modes
    :align: center

    blockdiag openthread-device-modes {

        # global attributes
        node_height = 90;
        node_width = 220;
        span_width = 100
        default_shape = roundedbox;
        default_group_color = none;

        # node labels
        HOST_NODE [label="OpenThread \nhost\n(ESP32)", fontsize=14];
        RCP [label="Radio \nCo-Processor\n(ESP32-H2)", fontsize=14];
        STANDALONE [label="Standalone \nnode\n (ESP32-H2)", fontsize=14];

        # node connections + labels
        RCP -> STANDALONE [label="15.4 radio", dir=both, style=dashed];

        # arrange nodes vertically
        group {
           orientation = portrait;
           HOST_NODE -> RCP [label="SPI", dir=both];
        }
    }


How To Write an OpenThread Application
--------------------------------------

The OpenThread :example:`openthread/ot_cli` example will be a good place to start at. It demonstrates basic OpenThread initialization and simple socket-based server and client.

Before OpenThread initialization
++++++++++++++++++++++++++++++++

- s1.1 The main task calls :cpp:func:`esp_vfs_eventfd_register` to initialize the eventfd virtual filesystem. The eventfd file system is used for task notification in the OpenThread driver.

- s1.2 The main task calls :cpp:func:`nvs_flash_init` to initialize the NVS where the Thread network data is stored.

- s1.3 **Optional**, The main task calls :cpp:func:`esp_netif_init` only when it wants to create the network interface for Thread.

- s1.4: The main task calls :cpp:func:`esp_event_loop_create` to create the system Event task and initialize an application event's callback function.

OpenThread stack initialization
+++++++++++++++++++++++++++++++
- s2.1: Call :cpp:func:`esp_openthread_init` to initialize the OpenThread stack.

OpenThread network interface initialization
+++++++++++++++++++++++++++++++++++++++++++
The whole stage is **optional** and only required if the application wants to create the network interface for Thread.
- s3.1: Call :cpp:func:`esp_netif_new` with `ESP_NETIF_DEFAULT_OPENTHREAD` to create the interface.
- s3.2: Call :cpp:func:`esp_openthread_netif_glue_init` to create the OpenThread interface handlers.
- s3.3: Call :cpp:func:`esp_netif_attach` to attach the handlers to the interface.

The OpenThread main loop
++++++++++++++++++++++++

- s4.3: Call :cpp:func:`esp_openthread_launch_mainloop` to launch the OpenThread main loop. Note that this is a busy loop and will not return until the OpenThread stack is terminated.

Calling OpenThread APIs
++++++++++++++++++++++++

The OpenThread APIs are not thread-safe. When calling OpenThread APIs from other tasks, make sure to hold the lock with :cpp:func:`esp_openthread_lock_acquire` and release the lock with :cpp:func:`esp_openthread_lock_release` afterwards.

Deinitialization
++++++++++++++++

The following steps are required to deintialize the OpenThread stack:
- Call :cpp:func:`esp_netif_destroy` and :cpp:func:`esp_openthread_netif_glue_deinit` to deintialize the OpenThread network interface if you have created one.
- Call :cpp:func:`esp_openthread_deinit` to deintialize the OpenThread stack.

The OpenThread border router
----------------------------

The OpenThread border router connects the Thread network with other IP networks. It will provide IPv6 connectivity, service registration and commission functionality.
To launch an OpenThread border router on a ESP chip, you need to connect an RCP to a Wi-Fi capable chip such as ESP32.
Call :cpp:func:`esp_openthread_border_router_init` during the initialization will launch all the border routing functionalities.

You may refer to the :example:`openthread/ot_br` example and the README for further border router details.


