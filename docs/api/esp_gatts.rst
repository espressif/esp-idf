GATT SERVER API
===============

Overview
--------

`Instructions`_

Application Example
-------------------

Check `/examples <https://github.com/espressif/esp-idf/tree/master/examples>`_ folder of `espressif/esp-idf <https://github.com/espressif/esp-idf/>`_ repository, that contains the following example:

`14_gatts_demo <https://github.com/espressif/esp-idf/blob/master/examples/14_gatt_server/main/gatts_demo.c>`_ 

This is a gatt server demo. Use gatt api to create a gatt server with send advertising. This gatt server can be connected and the service can be discovery.

`Instructions`_

.. _Instructions: template.html


API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `bt/bluedroid/api/include/esp_gatts_api.h <https://github.com/espressif/esp-idf/blob/master/components/bt/bluedroid/api/include/esp_gatts_api.h>`_

Macros
^^^^^^

.. doxygendefine:: ESP_GATT_PREP_WRITE_CANCEL
.. doxygendefine:: ESP_GATT_PREP_WRITE_EXEC

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_gatts_cb_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_gatts_cb_event_t

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_ble_gatts_cb_param_t
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_reg_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_read_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_write_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_exec_write_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_mtu_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_conf_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_create_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_add_incl_srvc_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_add_char_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_add_char_descr_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_delete_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_start_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_stop_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_connect_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_disconnect_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_congest_evt_param
    :members:

.. doxygenstruct:: esp_ble_gatts_cb_param_t::gatts_rsp_evt_param
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: esp_ble_gatts_register_callback
.. doxygenfunction:: esp_ble_gatts_app_register
.. doxygenfunction:: esp_ble_gatts_app_unregister
.. doxygenfunction:: esp_ble_gatts_create_service
.. doxygenfunction:: esp_ble_gatts_add_included_service
.. doxygenfunction:: esp_ble_gatts_add_char
.. doxygenfunction:: esp_ble_gatts_add_char_descr
.. doxygenfunction:: esp_ble_gatts_delete_service
.. doxygenfunction:: esp_ble_gatts_start_service
.. doxygenfunction:: esp_ble_gatts_stop_service
.. doxygenfunction:: esp_ble_gatts_send_indicate
.. doxygenfunction:: esp_ble_gatts_send_response
.. doxygenfunction:: esp_ble_gatts_open
.. doxygenfunction:: esp_ble_gatts_close

