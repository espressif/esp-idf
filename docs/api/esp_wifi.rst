Wi-Fi API
=========

Overview
--------

`Instructions <http://esp-idf.readthedocs.io/en/latest/api/template.html>`_

Application Example
-------------------

`Instructions <http://esp-idf.readthedocs.io/en/latest/api/template.html>`_

Reference
---------

`Instructions <http://esp-idf.readthedocs.io/en/latest/api/template.html>`_

Macros
------

.. doxygendefine:: WIFI_INIT_CONFIG_DEFAULT


Typedefs
--------

.. doxygentypedef:: wifi_promiscuous_cb_t
.. doxygentypedef:: wifi_rxcb_t
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
.. doxygenfunction:: esp_wifi_kick_station
.. doxygenfunction:: esp_wifi_scan_start
.. doxygenfunction:: esp_wifi_scan_stop
.. doxygenfunction:: esp_wifi_get_ap_num
.. doxygenfunction:: esp_wifi_get_ap_list
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
.. doxygenfunction:: esp_wifi_get_station_list
.. doxygenfunction:: esp_wifi_free_station_list
.. doxygenfunction:: esp_wifi_set_storage
.. doxygenfunction:: esp_wifi_reg_rxcb
.. doxygenfunction:: esp_wifi_set_auto_connect
.. doxygenfunction:: esp_wifi_get_auto_connect
.. doxygenfunction:: esp_wifi_set_vendor_ie
.. doxygenfunction:: esp_wifi_set_vendor_ie_cb
