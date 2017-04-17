Bluetooth A2DP API
==================

Overview
--------

`Instructions`_

.. _Instructions: ../template.html

Application Example
-------------------

Check :example:`bluetooth` folder in ESP-IDF examples, which contains the following examples:

:example:`bluetooth/a2dp_sink`

  This is a A2DP sink client demo. This demo can be discovered and connected by A2DP source device and receive the audio stream from remote device.


API Reference
-------------

Header Files
^^^^^^^^^^^^

  * :component_file:`bt/bluedroid/api/include/esp_a2dp_api.h`

Macros
^^^^^^

.. doxygendefine:: ESP_A2D_MCT_SBC
.. doxygendefine:: ESP_A2D_MCT_M12
.. doxygendefine:: ESP_A2D_MCT_M24
.. doxygendefine:: ESP_A2D_MCT_ATRAC
.. doxygendefine:: ESP_A2D_MCT_NON_A2DP
.. doxygendefine:: ESP_A2D_CIE_LEN_SBC
.. doxygendefine:: ESP_A2D_CIE_LEN_M12
.. doxygendefine:: ESP_A2D_CIE_LEN_M24
.. doxygendefine:: ESP_A2D_CIE_LEN_ATRAC

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_a2d_mct_t
.. doxygentypedef:: esp_a2d_cb_t
.. doxygentypedef:: esp_a2d_data_cb_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_a2d_connection_state_t
.. doxygenenum:: esp_a2d_disc_rsn_t
.. doxygenenum:: esp_a2d_audio_state_t
.. doxygenenum:: esp_a2d_cb_event_t

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_a2d_cb_param_t
    :members:

.. doxygenstruct:: esp_a2d_cb_param_t::a2d_conn_stat_param
    :members:

.. doxygenstruct:: esp_a2d_cb_param_t::a2d_audio_stat_param
    :members:

.. doxygenstruct:: esp_a2d_cb_param_t::a2d_audio_cfg_param
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: esp_a2d_register_callback
.. doxygenfunction:: esp_a2d_register_data_callback
.. doxygenfunction:: esp_a2d_sink_init
.. doxygenfunction:: esp_a2d_sink_deinit
.. doxygenfunction:: esp_a2d_sink_connect
.. doxygenfunction:: esp_a2d_sink_disconnect

