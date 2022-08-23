配置
======

:link_to_translation:`en:[English]`

Protocomm
---------

:cpp:func:`protocomm_set_security` API 中的 ``pop`` 字段现已弃用。请使用 ``sec_params`` 字段来代替 ``pop``。此参数应包含所使用的协议版本所要求的结构（包括安全参数）。

例如，当使用安全版本 2 时，``sec_params`` 参数应包含指向 :cpp:type:`protocomm_security2_params_t` 类型结构的指针。

Wi-Fi 配置
-------------

:cpp:func:`wifi_prov_mgr_start_provisioning`  API 中的 ``pop`` 字段现已弃用。请使用 ``wifi_prov_sec_params`` 字段来代替 ``pop``。此参数应包含所使用的协议版本所要求的结构（包括安全参数）。

例如，当使用安全版本 2 时，``wifi_prov_sec_params`` 参数应包含指向 :cpp:type:`wifi_prov_security2_params_t` 类型结构的指针。

ESP 本地控制
-----------------

:cpp:type:`esp_local_ctrl_proto_sec_cfg_t` API 中的 ``pop`` 字段现已弃用。请使用 ``sec_params`` 字段来代替 ``pop``。此参数应包含所使用的协议版本所要求的结构（包括安全参数）。

例如，当使用安全版本 2 时，``sec_params`` 字段应包含指向 :cpp:type:`esp_local_ctrl_security2_params_t` 类型结构的指针。
