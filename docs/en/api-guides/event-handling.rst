Event Handling
==============

Several ESP-IDF components use *events* to inform application about state changes, such as connection or disconnection. This document gives an overview of these event mechanisms.

Wi-Fi, Ethernet, and IP Events
------------------------------

Before the introduction of :doc:`esp_event library<../api-reference/system/esp_event>`, events from Wi-Fi driver, Ethernet driver, and TCP/IP stack were dispatched using the so-called *legacy event loop*. The following sections explain each of the methods.

esp_event Library Event Loop
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

esp_event library is designed to supersede the legacy event loop for the purposes of event handling in ESP-IDF. In the legacy event loop, all possible event types and event data structures had to be defined in :cpp:type:`system_event_id_t` enumeration and :cpp:type:`system_event_info_t` union, which made it impossible to send custom events to the event loop, and use the event loop for other kinds of events (e.g. Mesh). Legacy event loop also supported only one event handler function, therefore application components could not handle some of Wi-Fi or IP events themselves, and required application to forward these events from its event handler function.

See :doc:`esp_event library API reference<../api-reference/system/esp_event>` for general information on using this library. Wi-Fi, Ethernet, and IP events are sent to the :ref:`default event loop <esp-event-default-loops>` provided by this library.

.. _legacy-event-loop:

Legacy Event Loop
~~~~~~~~~~~~~~~~~

This event loop implementation is started using :cpp:func:`esp_event_loop_init` function. Application typically supplies an *event handler*, a function with the following signature::

    esp_err_t event_handler(void *ctx, system_event_t *event)
    {
    }

Both the pointer to event handler function, and an arbitrary context pointer are passed to :cpp:func:`esp_event_loop_init`. 

When Wi-Fi, Ethernet, or IP stack generate an event, this event is sent to a high-priority ``event`` task via a queue. Application-provided event handler function is called in the context of this task. Event task stack size and event queue size can be adjusted using :ref:`CONFIG_ESP_SYSTEM_EVENT_TASK_STACK_SIZE` and :ref:`CONFIG_ESP_SYSTEM_EVENT_QUEUE_SIZE` options, respectively.

Event handler receives a pointer to the event structure (:cpp:type:`system_event_t`) which describes current event. This structure follows a *tagged union* pattern: ``event_id`` member indicates the type of event, and ``event_info`` member is a union of description structures. Application event handler will typically use ``switch(event->event_id)`` to handle different kinds of events.

If application event handler needs to relay the event to some other task, it is important to note that event pointer passed to the event handler is a pointer to temporary structure. To pass the event to another task, application has to make a copy of the entire structure.


Event IDs and Corresponding Data Structures
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

+------------------------------------+-----------------------------------------------+
| Event ID                           | Event data structure                          |
| (legacy event ID)                  |                                               |
+------------------------------------+-----------------------------------------------+
| **Wi-Fi**                                                                          |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_WIFI_READY              | n/a                                           |
| (SYSTEM_EVENT_WIFI_READY)          |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_SCAN_DONE               | :cpp:class:`wifi_event_sta_scan_done_t`       |
| (SYSTEM_EVENT_SCAN_DONE)           |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_START               | n/a                                           |
| (SYSTEM_EVENT_STA_START)           |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_STOP                | n/a                                           |
| (SYSTEM_EVENT_STA_STOP)            |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_CONNECTED           | :cpp:class:`wifi_event_sta_connected_t`       |
| (SYSTEM_EVENT_STA_CONNECTED)       |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_DISCONNECTED        | :cpp:class:`wifi_event_sta_disconnected_t`    |
| (SYSTEM_EVENT_STA_DISCONNECTED)    |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_AUTHMODE_CHANGE     | :cpp:class:`wifi_event_sta_authmode_change_t` |
| (SYSTEM_EVENT_STA_AUTHMODE_CHANGE) |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_WPS_ER_SUCCESS      | n/a                                           |
| (SYSTEM_EVENT_STA_WPS_ER_SUCCESS)  |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_WPS_ER_FAILED       | :cpp:type:`wifi_event_sta_wps_fail_reason_t`  |
| (SYSTEM_EVENT_STA_WPS_ER_FAILED)   |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_WPS_ER_TIMEOUT      | n/a                                           |
| (SYSTEM_EVENT_STA_WPS_ER_TIMEOUT)  |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_STA_WPS_ER_PIN          | :cpp:class:`wifi_event_sta_wps_er_pin_t`      |
| (SYSTEM_EVENT_STA_WPS_ER_PIN)      |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_AP_START                | n/a                                           |
| (SYSTEM_EVENT_AP_START)            |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_AP_STOP                 | n/a                                           |
| (SYSTEM_EVENT_AP_STOP)             |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_AP_STACONNECTED         | :cpp:class:`wifi_event_ap_staconnected_t`     |
| (SYSTEM_EVENT_AP_STACONNECTED)     |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_AP_STADISCONNECTED      | :cpp:class:`wifi_event_ap_stadisconnected_t`  |
| (SYSTEM_EVENT_AP_STADISCONNECTED)  |                                               |
+------------------------------------+-----------------------------------------------+
| WIFI_EVENT_AP_PROBEREQRECVED       | :cpp:class:`wifi_event_ap_probe_req_rx_t`     |
| (SYSTEM_EVENT_AP_PROBEREQRECVED)   |                                               |
+------------------------------------+-----------------------------------------------+
| **Ethernet**                                                                       |
+------------------------------------+-----------------------------------------------+
| ETHERNET_EVENT_START               | n/a                                           |
| (SYSTEM_EVENT_ETH_START)           |                                               |
+------------------------------------+-----------------------------------------------+
| ETHERNET_EVENT_STOP                | n/a                                           |
| (SYSTEM_EVENT_ETH_STOP)            |                                               |
+------------------------------------+-----------------------------------------------+
| ETHERNET_EVENT_CONNECTED           | n/a                                           |
| (SYSTEM_EVENT_ETH_CONNECTED)       |                                               |
+------------------------------------+-----------------------------------------------+
| ETHERNET_EVENT_DISCONNECTED        | n/a                                           |
| (SYSTEM_EVENT_ETH_DISCONNECTED)    |                                               |
+------------------------------------+-----------------------------------------------+
| **IP**                                                                             |
+------------------------------------+-----------------------------------------------+
| IP_EVENT_STA_GOT_IP                | :cpp:class:`ip_event_got_ip_t`                |
| (SYSTEM_EVENT_STA_GOT_IP)          |                                               |
+------------------------------------+-----------------------------------------------+
| IP_EVENT_STA_LOST_IP               | n/a                                           |
| (SYSTEM_EVENT_STA_LOST_IP)         |                                               |
+------------------------------------+-----------------------------------------------+
| IP_EVENT_AP_STAIPASSIGNED          | n/a                                           |
| (SYSTEM_EVENT_AP_STAIPASSIGNED)    |                                               |
+------------------------------------+-----------------------------------------------+
| IP_EVENT_GOT_IP6                   | :cpp:class:`ip_event_got_ip6_t`               |
| (SYSTEM_EVENT_GOT_IP6)             |                                               |
+------------------------------------+-----------------------------------------------+
| IP_EVENT_ETH_GOT_IP                | :cpp:class:`ip_event_got_ip_t`                |
| (SYSTEM_EVENT_ETH_GOT_IP)          |                                               |
+------------------------------------+-----------------------------------------------+

Mesh Events
-----------

ESP-MESH uses a system similar to the :ref:`legacy-event-loop` to deliver events to the application. See :ref:`mesh-events` for details.

Bluetooth Events
----------------

Various modules of the Bluetooth stack deliver events to applications via dedicated callback functions. Callback functions receive the event type (enumerated value) and event data (union of structures for each event type). The following list gives the registration API name, event enumeration type, and event parameters type.

* BLE GAP: :cpp:func:`esp_ble_gap_register_callback`, :cpp:type:`esp_gap_ble_cb_event_t`, :cpp:type:`esp_ble_gap_cb_param_t`.
* BT GAP: :cpp:func:`esp_bt_gap_register_callback`, :cpp:type:`esp_bt_gap_cb_event_t`, :cpp:type:`esp_bt_gap_cb_param_t`.
* GATTC: :cpp:func:`esp_ble_gattc_register_callback`, :cpp:type:`esp_bt_gattc_cb_event_t`, :cpp:type:`esp_bt_gattc_cb_param_t`.
* GATTS: :cpp:func:`esp_ble_gatts_register_callback`, :cpp:type:`esp_bt_gatts_cb_event_t`, :cpp:type:`esp_bt_gatts_cb_param_t`.
* SPP: :cpp:func:`esp_spp_register_callback`, :cpp:type:`esp_spp_cb_event_t`, :cpp:type:`esp_spp_cb_param_t`.
* Blufi: :cpp:func:`esp_blufi_register_callbacks`, :cpp:type:`esp_blufi_cb_event_t`, :cpp:type:`esp_blufi_cb_param_t`.
* A2DP: :cpp:func:`esp_a2d_register_callback`, :cpp:type:`esp_a2d_cb_event_t`, :cpp:type:`esp_a2d_cb_param_t`.
* AVRC: :cpp:func:`esp_avrc_ct_register_callback`, :cpp:type:`esp_avrc_ct_cb_event_t`, :cpp:type:`esp_avrc_ct_cb_param_t`.
* HFP Client: :cpp:func:`esp_hf_client_register_callback`, :cpp:type:`esp_hf_client_cb_event_t`, :cpp:type:`esp_hf_client_cb_param_t`.
