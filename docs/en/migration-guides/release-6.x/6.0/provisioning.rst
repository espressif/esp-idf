Provisioning
==================

:link_to_translation:`zh_CN:[中文]`

Breaking Changes
----------------

The return type of :cpp:func:`wifi_prov_mgr_deinit` has been changed from ``void`` to :cpp:type:`esp_err_t`. This change allows applications to properly handle potential failures during provisioning manager deinitialization.
