BT AVRCP APIs
=============

Overview
--------

Bluetooth AVRCP reference APIs.

`Instructions`_

Application Example
-------------------

`Instructions`_

.. _Instructions: ../template.html


API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`bt/bluedroid/api/include/esp_avrc_api.h`


Macros
^^^^^^


Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_avrc_ct_cb_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_avrc_features_t
.. doxygenenum:: esp_avrc_pt_cmd_t
.. doxygenenum:: esp_avrc_pt_cmd_state_t
.. doxygenenum:: esp_avrc_ct_cb_event_t

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_avrc_ct_cb_param_t
    :members:

.. doxygenstruct:: esp_avrc_ct_cb_param_t::avrc_ct_conn_stat_param
    :members:

.. doxygenstruct:: esp_avrc_ct_cb_param_t::avrc_ct_psth_rsp_param
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: esp_avrc_ct_register_callback
.. doxygenfunction:: esp_avrc_ct_init
.. doxygenfunction:: esp_avrc_ct_deinit
.. doxygenfunction:: esp_avrc_ct_send_passthrough_cmd

