GATT CLIENT API
===============

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Check `/examples/bluetooth <https://github.com/espressif/esp-idf/tree/master/examples/bluetooth>`_ folder of `espressif/esp-idf <https://github.com/espressif/esp-idf>`_ repository, that contains the following example:

`gatt_client <https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/gatt_client>`_ 

  This is a gatt client demo. This demo can scan devices, connect to the gatt server and discover the service.


API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `bt/bluedroid/api/include/esp_gattc_api.h <https://github.com/espressif/esp-idf/blob/master/components/bt/bluedroid/api/include/esp_gattc_api.h>`_

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
.. doxygenfunction:: esp_ble_gattc_execute_write
.. doxygenfunction:: esp_ble_gattc_register_for_notify
.. doxygenfunction:: esp_ble_gattc_unregister_for_notify

