GAP API
=======

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Check :example:`bluetooth` folder in ESP-IDF examples, which contains the following examples:

:example:`bluetooth/gatt_server`, :example:`bluetooth/gatt_client` 

  The two demos use different GAP APIs, such like advertising, scan, set device name and others.

API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`bt/bluedroid/api/include/esp_gap_ble_api.h`


Macros
^^^^^^

.. doxygendefine:: ESP_BLE_ADV_FLAG_LIMIT_DISC
.. doxygendefine:: ESP_BLE_ADV_FLAG_GEN_DISC
.. doxygendefine:: ESP_BLE_ADV_FLAG_BREDR_NOT_SPT
.. doxygendefine:: ESP_BLE_ADV_FLAG_DMT_CONTROLLER_SPT
.. doxygendefine:: ESP_BLE_ADV_FLAG_DMT_HOST_SPT
.. doxygendefine:: ESP_BLE_ADV_FLAG_NON_LIMIT_DISC
.. doxygendefine:: ESP_LE_KEY_NONE
.. doxygendefine:: ESP_LE_KEY_PENC
.. doxygendefine:: ESP_LE_KEY_PID
.. doxygendefine:: ESP_LE_KEY_PCSRK
.. doxygendefine:: ESP_LE_KEY_PLK
.. doxygendefine:: ESP_LE_KEY_LLK
.. doxygendefine:: ESP_LE_KEY_LENC
.. doxygendefine:: ESP_LE_KEY_LID
.. doxygendefine:: ESP_LE_KEY_LCSRK
.. doxygendefine:: ESP_LE_AUTH_NO_BOND
.. doxygendefine:: ESP_LE_AUTH_BOND
.. doxygendefine:: ESP_LE_AUTH_REQ_MITM
.. doxygendefine:: ESP_LE_AUTH_REQ_SC_ONLY
.. doxygendefine:: ESP_LE_AUTH_REQ_SC_BOND
.. doxygendefine:: ESP_LE_AUTH_REQ_SC_MITM
.. doxygendefine:: ESP_LE_AUTH_REQ_SC_MITM_BOND
.. doxygendefine:: ESP_IO_CAP_OUT
.. doxygendefine:: ESP_IO_CAP_IO
.. doxygendefine:: ESP_IO_CAP_IN
.. doxygendefine:: ESP_IO_CAP_NONE
.. doxygendefine:: ESP_IO_CAP_KBDISP
.. doxygendefine:: ESP_BLE_ADV_DATA_LEN_MAX
.. doxygendefine:: ESP_BLE_SCAN_RSP_DATA_LEN_MAX

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_ble_key_type_t
.. doxygentypedef:: esp_ble_auth_req_t
.. doxygentypedef:: esp_ble_io_cap_t
.. doxygentypedef:: esp_gap_ble_cb_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_gap_ble_cb_event_t
.. doxygenenum:: esp_ble_adv_data_type
.. doxygenenum:: esp_ble_adv_type_t
.. doxygenenum:: esp_ble_adv_channel_t
.. doxygenenum:: esp_ble_adv_filter_t
.. doxygenenum:: esp_ble_sec_act_t
.. doxygenenum:: esp_ble_sm_param_t
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

.. doxygenstruct:: esp_ble_penc_keys_t
    :members:

.. doxygenstruct:: esp_ble_pcsrk_keys_t
    :members:

.. doxygenstruct:: esp_ble_pid_keys_t
    :members:

.. doxygenstruct:: esp_ble_lenc_keys_t
    :members:

.. doxygenstruct:: esp_ble_lcsrk_keys
    :members:

.. doxygenstruct:: esp_ble_sec_key_notif_t
    :members:

.. doxygenstruct:: esp_ble_sec_req_t
    :members:

.. doxygenstruct:: esp_ble_key_value_t
    :members:

.. doxygenstruct:: esp_ble_key_t
    :members:

.. doxygenstruct:: esp_ble_local_id_keys_t
    :members:

.. doxygenstruct:: esp_ble_auth_cmpl_t
    :members:

.. doxygenstruct:: esp_ble_sec_t
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

.. doxygenstruct:: esp_ble_gap_cb_param_t::ble_adv_data_raw_cmpl_evt_param
    :members:

.. doxygenstruct:: esp_ble_gap_cb_param_t::ble_scan_rsp_data_raw_cmpl_evt_param
    :members:

.. doxygenstruct:: esp_ble_gap_cb_param_t::ble_adv_start_cmpl_evt_param
    :members:

.. doxygenstruct:: esp_ble_gap_cb_param_t::ble_scan_start_cmpl_evt_param
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
.. doxygenfunction:: esp_ble_gap_config_adv_data_raw
.. doxygenfunction:: esp_ble_gap_config_scan_rsp_data_raw
.. doxygenfunction:: esp_ble_gap_set_security_param
.. doxygenfunction:: esp_ble_gap_security_rsp
.. doxygenfunction:: esp_ble_set_encryption
.. doxygenfunction:: esp_ble_passkey_reply
.. doxygenfunction:: esp_ble_confirm_reply

