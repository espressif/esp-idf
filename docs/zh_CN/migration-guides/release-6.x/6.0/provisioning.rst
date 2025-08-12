配网
====

:link_to_translation:`en:[English]`

重大变更
--------

函数 :cpp:func:`wifi_prov_mgr_deinit` 的返回值类型已从 ``void`` 变更为 :cpp:type:`esp_err_t`。此项变更使应用程序在反初始化配网管理器时能够妥善处理可能出现的故障。

配置变更
--------

Protocomm 安全配置
~~~~~~~~~~~~~~~~~~

为了默认提高安全性，protocomm 安全配置选项的默认值已更改：

- :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0` 现在默认为 ``n`` （原为 ``y`` ）
- :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1` 现在默认为 ``n`` （原为 ``y`` ）

依赖 protocomm 安全版本 0 或 1 的项目需要在其配置中显式启用这些选项。如果您的应用程序使用 protocomm 安全版本 0（无安全）或版本 1（Curve25519 + AES-CTR），则必须在项目的 ``sdkconfig`` 中或通过 ``menuconfig`` 显式启用相应的配置选项：

- 对于安全版本 0：启用 :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`
- 对于安全版本 1：启用 :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1`

此项变更旨在默认减少代码大小，并鼓励使用更安全的 protocomm 实现。
