Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

- Previously, the use of SDP APIs was affected by the ``CONFIG_BT_L2CAP_ENABLED`` configuration, although there was no relationship between them. The new Kconfig option ``CONFIG_BT_SDP_COMMON_ENABLED`` has been introduced to separate common SDP operations from Classic Bluetooth L2CAP functionality. It shall be enabled before calling SDP related APIs.
- The following Bluedroid API have been changed:

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_sdp_api.h`

        - Field ``user2_ptr_len`` and ``user2_ptr`` is deprecated in structure ``esp_bluetooth_sdp_hdr_overlay_t``, since they are not used in SDP record creation or searching.

Bluetooth Common APIs
---------------------

.. include:: ./bt_common.rst
