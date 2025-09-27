Wi-Fi
=====

本文档概述了 v6.0 版本中与 Wi-Fi 相关的重大更改。

已移除的函数和类型
------------------

以下函数和类型已被移除。

- **DPP：**
    - DPP 事件回调 ``esp_supp_dpp_event_cb_t`` 和事件枚举 ``esp_supp_dpp_event_t`` 已被移除。请直接使用 Wi-Fi 事件（例如 ``WIFI_EVENT_DPP_URI_READY``、``WIFI_EVENT_DPP_CFG_RECVD``、``WIFI_EVENT_DPP_FAILED``）。

- **RRM：** 函数 ``esp_rrm_send_neighbor_rep_request`` 已被移除。请改用 ``esp_rrm_send_neighbor_report_request``。

- **天线配置：**
    - 函数 ``esp_wifi_set_ant_gpio``、``esp_wifi_get_ant_gpio``、``esp_wifi_set_ant`` 和 ``esp_wifi_get_ant`` 已被移除。请改用 ``esp_phy`` 组件中的等效函数：``esp_phy_set_ant_gpio``、``esp_phy_get_ant_gpio``、``esp_phy_set_ant`` 和 ``esp_phy_get_ant``。

- **ESP-NOW：** 函数 ``esp_wifi_config_espnow_rate`` 已被移除。请改用 ``esp_now_set_peer_rate_config``。

已移除的变量、宏和枚举值
------------------------

- **认证模式：** 认证模式 ``WIFI_AUTH_WPA3_EXT_PSK`` 和 ``WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE`` 已被移除。请改用 ``WIFI_AUTH_WPA3_PSK``。

- **邻居报告事件：** 在 ``wifi_event_neighbor_report_t`` 结构体中，``report`` 字段已被移除。请改用 ``n_report`` 字段，它是一个灵活的数组成员。

- **Wi-Fi 接口：** 头文件 ``components/esp_wifi/include/esp_interface.h`` 已被移除。``wifi_interface_t`` 枚举现在定义在 :project_file:`components/esp_wifi/include/esp_wifi_types_generic.h` 中。枚举值（例如 ``WIFI_IF_STA``、``WIFI_IF_AP``）不再基于 ``ESP_IF_WIFI_STA``、``ESP_IF_WIFI_AP`` 等定义。

- **Wi-Fi 接口宏：** 宏 ``ESP_IF_WIFI_STA`` 和 ``ESP_IF_WIFI_AP`` 已被移除。请直接使用枚举值 ``WIFI_IF_STA`` 和 ``WIFI_IF_AP``。

- **Wi-Fi 断开连接原因：** 断开连接原因代码 ``WIFI_REASON_ASSOC_EXPIRE``、``WIFI_REASON_NOT_AUTHED`` 和 ``WIFI_REASON_NOT_ASSOCED`` 已被移除。请改用 ``WIFI_REASON_AUTH_EXPIRE``、``WIFI_REASON_CLASS2_FRAME_FROM_NONAUTH_STA`` 和 ``WIFI_REASON_CLASS3_FRAME_FROM_NONASSOC_STA``。

- **NAN：**
    - 结构体 ``wifi_nan_publish_cfg_t``、``wifi_nan_subscribe_cfg_t``、``wifi_nan_followup_params_t``、``wifi_event_ndp_indication_t``、``wifi_event_ndp_confirm_t`` 中的字段 ``svc_info`` 已被移除。请改用等效字段 ``ssi`` 和 ``ssi_len``。
    - 结构体 ``wifi_event_nan_receive_t`` 中的字段 ``peer_svc_info`` 已被移除。请改用等效字段 ``ssi`` 和 ``ssi_len``。

- **FTM：** 结构体 ``wifi_ftm_initiator_cfg_t`` 中的字段 ``use_get_report_api`` 和结构体 ``wifi_event_ftm_report_t`` 中的字段 ``ftm_report_data`` 已被移除。请改用 ``esp_wifi_ftm_get_report`` 获取原始 FTM 报告数据。

已修改的函数和类型
------------------

以下 Wi-Fi 函数已修改。

- **DPP：** 函数 ``esp_supp_dpp_init`` 不再接受回调函数，应调用为 ``esp_supp_dpp_init(void)``。

- **WPS：** 函数 ``esp_wifi_wps_start`` 不再接受 ``timeout_ms`` 参数。现在应调用为 ``esp_wifi_wps_start(void)``。

- **NAN：** 函数 ``esp_wifi_nan_publish_service`` 的参数 ``ndp_resp_needed`` 已移至结构体 ``wifi_nan_publish_cfg_t``。

