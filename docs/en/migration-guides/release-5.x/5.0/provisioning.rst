Provisioning
============

:link_to_translation:`zh_CN:[中文]`

Protocomm
---------

The ``pop`` field in the :cpp:func:`protocomm_set_security` API is now deprecated. Please use the ``sec_params`` field instead of ``pop``. This parameter should contain the structure (including the security parameters) as required by the protocol version used.

For example, when using security version 2, the ``sec_params`` parameter should contain the pointer to the structure of type :cpp:type:`protocomm_security2_params_t`.

Wi-Fi Provisioning
------------------

.. list::
    * The ``pop`` field in the :cpp:func:`wifi_prov_mgr_start_provisioning` API is now deprecated. For backward compatibility, ``pop`` can be still passed as a string for security version 1. However, for security version 2, the ``wifi_prov_sec_params`` argument needs to be passed instead of ``pop``. This parameter should contain the structure (containing the security parameters) as required by the protocol version used. For example, when using security version 2, the ``wifi_prov_sec_params`` parameter should contain the pointer to the structure of type :cpp:type:`wifi_prov_security2_params_t`. For security 1, the behaviour and the usage of the API remain the same.

    * The API :cpp:func:`wifi_prov_mgr_is_provisioned` does not return :c:macro:`ESP_ERR_INVALID_STATE` error any more. This API now works without any dependency on provisioning manager initialization state.

ESP Local Control
-----------------

The ``pop`` field in the :cpp:type:`esp_local_ctrl_proto_sec_cfg_t` API is now deprecated. Please use the ``sec_params`` field instead of ``pop``. This field should contain the structure (containing the security parameters) as required by the protocol version used.

For example, when using security version 2, the ``sec_params`` field should contain pointer to the structure of type :cpp:type:`esp_local_ctrl_security2_params_t`.
