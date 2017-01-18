Wi-Fi
=====

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Simple code showing how to connect ESP32 module to an Access Point: `esp-idf-template <https://github.com/espressif/esp-idf-template>`_.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `esp32/include/esp_wifi.h <https://github.com/espressif/esp-idf/blob/master/components/esp32/include/esp_wifi.h>`_

Macros
------

.. doxygendefine:: WIFI_INIT_CONFIG_DEFAULT


Type Definitions
----------------

.. doxygentypedef:: wifi_promiscuous_cb_t
.. doxygentypedef:: esp_vendor_ie_cb_t

Functions
---------

.. doxygenfunction:: esp_wifi_init
.. doxygenfunction:: esp_wifi_deinit
.. doxygenfunction:: esp_wifi_set_mode
.. doxygenfunction:: esp_wifi_get_mode
.. doxygenfunction:: esp_wifi_start
.. doxygenfunction:: esp_wifi_stop
.. doxygenfunction:: esp_wifi_connect
.. doxygenfunction:: esp_wifi_disconnect
.. doxygenfunction:: esp_wifi_clear_fast_connect
.. doxygenfunction:: esp_wifi_deauth_sta
.. doxygenfunction:: esp_wifi_scan_start
.. doxygenfunction:: esp_wifi_scan_stop
.. doxygenfunction:: esp_wifi_scan_get_ap_num
.. doxygenfunction:: esp_wifi_scan_get_ap_records
.. doxygenfunction:: esp_wifi_sta_get_ap_info
.. doxygenfunction:: esp_wifi_set_ps
.. doxygenfunction:: esp_wifi_get_ps
.. doxygenfunction:: esp_wifi_set_protocol
.. doxygenfunction:: esp_wifi_get_protocol
.. doxygenfunction:: esp_wifi_set_bandwidth
.. doxygenfunction:: esp_wifi_get_bandwidth
.. doxygenfunction:: esp_wifi_set_channel
.. doxygenfunction:: esp_wifi_get_channel
.. doxygenfunction:: esp_wifi_set_country
.. doxygenfunction:: esp_wifi_get_country
.. doxygenfunction:: esp_wifi_set_mac
.. doxygenfunction:: esp_wifi_get_mac
.. doxygenfunction:: esp_wifi_set_promiscuous_rx_cb
.. doxygenfunction:: esp_wifi_set_promiscuous
.. doxygenfunction:: esp_wifi_get_promiscuous
.. doxygenfunction:: esp_wifi_set_config
.. doxygenfunction:: esp_wifi_get_config
.. doxygenfunction:: esp_wifi_ap_get_sta_list
.. doxygenfunction:: esp_wifi_set_storage
.. doxygenfunction:: esp_wifi_set_auto_connect
.. doxygenfunction:: esp_wifi_get_auto_connect
.. doxygenfunction:: esp_wifi_set_vendor_ie
.. doxygenfunction:: esp_wifi_set_vendor_ie_cb


