BLUFI API
=========

Overview
--------
BLUFI is a profile based GATT to config ESP32 WIFI to connect/disconnect AP or setup a softap and etc.
Use should concern these things: 
    1. The event sent from profile. Then you need to do something as the event indicate.
    2. Security reference. You can write your own Security functions such as symmetrical encryption/decryption and checksum functions.
       Even you can define the "Key Exchange/Negotiation" procedure.

Application Example
-------------------

Check `/examples <https://github.com/espressif/esp-idf/tree/master/examples>`_ folder of `espressif/esp-idf <https://github.com/espressif/esp-idf/>`_ repository, that contains the following example:

`12_blufi <https://github.com/espressif/esp-idf/blob/master/examples/12_blufi/main/>`_ 

This is a BLUFI demo. This demo can set ESP32's wifi to softap/station/softap&station mode and config wifi connections.


API Reference
-------------

Header Files
^^^^^^^^^^^^

  * `bt/bluedroid/api/include/esp_blufi_api.h <https://github.com/espressif/esp-idf/blob/master/components/bt/bluedroid/api/include/esp_blufi_api.h>`_

Macros
^^^^^^


Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: esp_blufi_event_cb_t
.. doxygentypedef:: esp_blufi_negotiate_data_handler_t
.. doxygentypedef:: esp_blufi_encrypt_func_t
.. doxygentypedef:: esp_blufi_decrypt_func_t
.. doxygentypedef:: esp_blufi_checksum_func_t

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: esp_blufi_cb_event_t
.. doxygenenum:: esp_blufi_sta_conn_state_t
.. doxygenenum:: esp_blufi_init_state_t
.. doxygenenum:: esp_blufi_deinit_state_t

Structures
^^^^^^^^^^

.. doxygenstruct:: esp_blufi_extra_info_t
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_init_finish_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_deinit_finish_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_set_wifi_mode_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_connect_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_disconnect_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_sta_bssid_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_sta_ssid_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_sta_passwd_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_softap_ssid_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_softap_passwd_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_softap_max_conn_num_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_softap_auth_mode_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_softap_channel_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_username_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_ca_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_client_cert_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_server_cert_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_client_pkey_evt_param
    :members:

.. doxygenstruct:: esp_blufi_cb_param_t::blufi_recv_server_pkey_evt_param
    :members:

.. doxygenstruct:: esp_blufi_callbacks_t
    :members:


Functions
^^^^^^^^^

.. doxygenfunction:: esp_blufi_register_callbacks
.. doxygenfunction:: esp_blufi_profile_init
.. doxygenfunction:: esp_blufi_profile_deinit
.. doxygenfunction:: esp_blufi_send_wifi_conn_report

