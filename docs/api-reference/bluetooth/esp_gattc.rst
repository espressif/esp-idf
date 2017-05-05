GATT CLIENT API
===============

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Check :example:`bluetooth` folder in ESP-IDF examples, which contains the following examples:

:example:`bluetooth/gatt_client`

  This is a GATT client demo. This demo can scan devices, connect to the GATT server and discover the service.


API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`bt/bluedroid/api/include/esp_gattc_api.h`

Macros
^^^^^^

.. doxygendefine:: ESP_GATT_DEF_BLE_MTU_SIZE
.. doxygendefine:: ESP_GATT_MAX_MTU_SIZE

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_gattc_cb_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_gattc_cb_event_t

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_ble_gattc_cb_param_t
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_reg_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_open_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_close_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_cfg_mtu_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_search_cmpl_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_search_res_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_read_char_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_write_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_exec_cmpl_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_notify_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_srvc_chg_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_congest_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_get_char_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_get_descr_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_get_incl_srvc_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_reg_for_notify_evt_param
    :members:

.. doxygenstruct:: esp_ble_gattc_cb_param_t::gattc_unreg_for_notify_evt_param
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: esp_ble_gattc_register_callback
.. doxygenfunction:: esp_ble_gattc_app_register
.. doxygenfunction:: esp_ble_gattc_app_unregister
.. doxygenfunction:: esp_ble_gattc_open
.. doxygenfunction:: esp_ble_gattc_close
.. doxygenfunction:: esp_ble_gattc_config_mtu
.. doxygenfunction:: esp_ble_gattc_search_service
.. doxygenfunction:: esp_ble_gattc_get_characteristic
.. doxygenfunction:: esp_ble_gattc_get_descriptor
.. doxygenfunction:: esp_ble_gattc_get_included_service
.. doxygenfunction:: esp_ble_gattc_read_char
.. doxygenfunction:: esp_ble_gattc_read_char_descr
.. doxygenfunction:: esp_ble_gattc_write_char
.. doxygenfunction:: esp_ble_gattc_write_char_descr
.. doxygenfunction:: esp_ble_gattc_prepare_write
.. doxygenfunction:: esp_ble_gattc_prepare_write_char_descr
.. doxygenfunction:: esp_ble_gattc_execute_write
.. doxygenfunction:: esp_ble_gattc_register_for_notify
.. doxygenfunction:: esp_ble_gattc_unregister_for_notify

