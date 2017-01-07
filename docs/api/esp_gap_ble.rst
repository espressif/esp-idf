GAP API
========

Overview
--------

`Instructions`_

Application Example
-------------------

Check `/examples <https://github.com/espressif/esp-idf/tree/master/examples>`_ folder of `espressif/esp-idf <https://github.com/espressif/esp-idf/>`_ repository, that contains the following example:

`14_gatts_demo <https://github.com/espressif/esp-idf/blob/master/examples/14_gatt_server/main/gatts_demo.c>`_ 
`15_gattc_demo <https://github.com/espressif/esp-idf/blob/master/examples/15_gatt_client/main/gattc_demo.c>`_ 

The two demos use different gap api, such like advertising, scan, set device name and others.

`Instructions`_

.. _Instructions: template.html


API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `bt/bluedroid/api/include/esp_gap_ble_api.h <https://github.com/espressif/esp-idf/blob/master/components/bt/bluedroid/api/include/esp_gap_ble.h>`_


Macros
^^^^^^

.. doxygendefine:: ESP_BLE_ADV_FLAG_LIMIT_DISC
.. doxygendefine:: ESP_BLE_ADV_FLAG_GEN_DISC
.. doxygendefine:: ESP_BLE_ADV_FLAG_BREDR_NOT_SPT
.. doxygendefine:: ESP_BLE_ADV_FLAG_DMT_CONTROLLER_SPT
.. doxygendefine:: ESP_BLE_ADV_FLAG_DMT_HOST_SPT
.. doxygendefine:: ESP_BLE_ADV_FLAG_NON_LIMIT_DISC
.. doxygendefine:: ESP_BLE_ADV_DATA_LEN_MAX

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_gap_ble_cb_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_gap_ble_cb_event_t
.. doxygenenum:: esp_ble_adv_data_type
.. doxygenenum:: esp_ble_adv_type_t
.. doxygenenum:: esp_ble_adv_channel_t
.. doxygenenum:: esp_ble_adv_filter_t
.. doxygenenum:: esp_ble_own_addr_src_t
.. doxygenenum:: esp_ble_scan_type_t
.. doxygenenum:: esp_ble_scan_filter_t
.. doxygenenum:: esp_gap_search_evt_t
.. doxygenenum:: esp_ble_evt_type_t

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_ble_adv_params_t
    :members:

.. doxygenstruct:: esp_ble_adv_data_t
    :members:

.. doxygenstruct:: esp_ble_scan_params_t
    :members:

.. doxygenstruct:: esp_ble_conn_update_params_t
    :members:

.. doxygenstruct:: esp_ble_gap_cb_param_t
    :members:

.. doxygenstruct:: esp_ble_gap_cb_param_t::ble_adv_data_cmpl_evt_param
    :members:

.. doxygenstruct:: esp_ble_gap_cb_param_t::ble_scan_rsp_data_cmpl_evt_param
    :members:

.. doxygenstruct:: esp_ble_gap_cb_param_t::ble_scan_param_cmpl_evt_param
    :members:

.. doxygenstruct:: esp_ble_gap_cb_param_t::ble_scan_result_evt_param
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: esp_ble_gap_register_callback
.. doxygenfunction:: esp_ble_gap_config_adv_data
.. doxygenfunction:: esp_ble_gap_set_scan_params
.. doxygenfunction:: esp_ble_gap_start_scanning
.. doxygenfunction:: esp_ble_gap_stop_scanning
.. doxygenfunction:: esp_ble_gap_start_advertising
.. doxygenfunction:: esp_ble_gap_stop_advertising
.. doxygenfunction:: esp_ble_gap_update_conn_params
.. doxygenfunction:: esp_ble_gap_set_pkt_data_len
.. doxygenfunction:: esp_ble_gap_set_rand_addr
.. doxygenfunction:: esp_ble_gap_config_local_privacy
.. doxygenfunction:: esp_ble_gap_set_device_name
.. doxygenfunction:: esp_ble_resolve_adv_data

