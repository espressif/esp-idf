经典蓝牙
=================

:link_to_translation:`en:[English]`

Bluedroid
---------

    以下 Bluedroid API 已被废弃并移除

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_bt_device.h`

        - 移除 ``esp_err_t esp_bt_dev_set_device_name(const char *name)``

            - 设置设备名 API 已被替换为 ``esp_err_t esp_bt_gap_set_device_name(const char *name)`` or ``esp_err_t esp_ble_gap_set_device_name(const char *name)``。

        - 移除 ``esp_err_t esp_bt_dev_get_device_name(void)``

            - 获取设备名 API 已被替换为 ``esp_err_t esp_bt_gap_get_device_name(void)`` or ``esp_err_t esp_ble_gap_get_device_name(void)``。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_spp_api.h`

        - 移除 ``esp_err_t esp_spp_init(esp_spp_mode_t mode)``

            - SPP 初始化 API 已被替代为 ``esp_err_t esp_spp_enhanced_init(const esp_spp_cfg_t *cfg)``。
            - 新增配置结构体 ``esp_spp_cfg_t`` 作为 ``esp_spp_enhanced_init`` 的参数引入。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_ag_api.h`

        - 移除 ``esp_err_t esp_hf_ag_devices_status_indchange(...)``

            - 向 HFP 客户端发送指标报告 API 已被替换为 ``esp_hf_ag_ciev_report(...)``。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_gap_bt_api.h`

        - 移除 ``ESP_BT_GAP_RSSI_HIGH_THRLD`` 和 ``ESP_BT_GAP_RSSI_LOW_THRLD``

            - 表示接受信号强度阈值的宏 ``ESP_BT_GAP_RSSI_HIGH_THRLD`` 和 ``ESP_BT_GAP_RSSI_LOW_THRLD`` 已经被移除。

    以下 Bluedroid API 发生变更

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_sdp_api.h`

        - 结构体 ``esp_bluetooth_sdp_hdr_overlay_t`` 中的字段 ``user2_ptr_len`` 和 ``user2_ptr`` 被移除，因为 SDP 的 API 或者事件中不会用到该字段。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_a2dp_api.h`

        - 结构体 ``esp_a2d_media_ctrl_t`` 中的字段 ``ESP_A2D_MEDIA_CTRL_STOP`` 被移除，该字段被 ``ESP_A2D_MEDIA_CTRL_SUSPEND`` 替代。

        - 结构体 ``esp_a2d_mcc_t`` 中的字段 ``sbc, m12, m24, atrac`` 被移除并由字段 ``bc_info, m12_info, m24_info , atrc_info`` 替代。

        - 宏 ``ESP_A2D_SBC_CIE_ALLOC_MTHD_SRN`` 更名为 ``ESP_A2D_SBC_CIE_ALLOC_MTHD_SNR``。旧宏为兼容保留，但已废弃，使用时会产生编译期告警。
