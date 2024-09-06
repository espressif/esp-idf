经典蓝牙
=================

:link_to_translation:`en:[English]`

Bluedroid
---------

- 之前，SDP 相关 API 的使用受到配置项 ``CONFIG_BT_L2CAP_ENABLED`` 影响，但是这两者并没有太大关联。新的配置项 ``CONFIG_BT_SDP_COMMON_ENABLED`` 将 SDP 通用功能从经典蓝牙 L2CAP 功能中分离出来，在使用 SDP 相关 API 前需要先使能该配置项。
- 以下 Bluedroid API 发生变更

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_sdp_api.h`

        - 结构体 ``esp_bluetooth_sdp_hdr_overlay_t`` 被重命名为 ``esp_bluetooth_sdp_hdr_t``
        - 结构体 ``esp_bluetooth_sdp_hdr_overlay_t`` 中的字段 ``uuid``, ``user1_ptr_len`` 和 ``user1_ptr`` 被移动到结构体 ``esp_bluetooth_sdp_raw_record_t`` 中
        - 结构体 ``esp_bluetooth_sdp_hdr_overlay_t`` 中的字段 ``user2_ptr_len`` 和 ``user2_ptr`` 被删除
