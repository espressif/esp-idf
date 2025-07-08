配网
====

:link_to_translation:`en:[English]`

重大变更
--------

函数 :cpp:func:`wifi_prov_mgr_deinit` 的返回值类型已从 ``void`` 变更为 :cpp:type:`esp_err_t`。此项变更使应用程序在反初始化配网管理器时能够妥善处理可能出现的故障。
