Provisioning
============

:link_to_translation:`zh_CN:[中文]`

Breaking Changes
----------------

The return type of :cpp:func:`wifi_prov_mgr_deinit` has been changed from ``void`` to :cpp:type:`esp_err_t`. This change allows applications to properly handle potential failures during provisioning manager deinitialization.

Configuration Changes
---------------------

Protocomm Security Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The default values for protocomm security configuration options have been changed to improve security by default:

- :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0` now defaults to ``n`` (previously ``y``)
- :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1` now defaults to ``n`` (previously ``y``)

Projects that rely on protocomm security versions 0 or 1 will need to explicitly enable these options in their configuration. If your application uses protocomm security version 0 (no security) or version 1 (Curve25519 + AES-CTR), you must explicitly enable the corresponding configuration option in your project's ``sdkconfig`` or through ``menuconfig``:

- For security version 0: Enable :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0`
- For security version 1: Enable :ref:`CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_1`

This change was made to reduce code size by default and encourage the use of more secure protocomm implementations.
