Provisioning
============

Protocomm
---------

The :cpp:func:`protocomm_set_security` API now takes a parameter ``sec_params`` as input instead of ``pop`` (deprecated).
This parameter should contain the structure (containing the security parameters) as required by the protocol version used.

For example when using security version 2, the ``sec_params`` parameter should contain the pointer to the structure of type :cpp:type:`protocomm_security2_params_t`.

Wi-Fi Provisioning
------------------

The :cpp:func:`wifi_prov_mgr_start_provisioning` API now takes a parameter ``wifi_prov_sec_params`` as input instead of ``pop``.
This parameter should contain the structure (containing the security parameters) as required by the protocol version used.

For example when using security version 2, the ``wifi_prov_sec_params`` parameter should contain the pointer to the structure of type :cpp:type:`wifi_prov_security2_params_t`.

ESP Local Control
-----------------
The `pop` field in :cpp:type:`esp_local_ctrl_proto_sec_cfg_t` is now deprecated, use ``sec_params`` field instead of ``pop``.
This field should contain the structure (containing the security parameters) as required by the protocol version used.

For example when using security version 2, the ``sec_params`` field should contain pointer to the structure of type :cpp:type:`esp_local_ctrl_security2_params_t`.
