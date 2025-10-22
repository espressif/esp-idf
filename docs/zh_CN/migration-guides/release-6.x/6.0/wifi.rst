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

- **NAN：**
    -  已调整 NAN API，使其可在两个独立的 NAN 工作模式间共享：NAN 同步（NAN-Sync）与 NAN 非同步发现（NAN-USD）：
        - ``esp_wifi_nan_sync_start`` 与 ``esp_wifi_nan_sync_stop`` 取代原有的 ``esp_wifi_nan_start`` / ``esp_wifi_nan_stop``。
        - 以 ``esp_wifi_nan_*`` 为前缀的函数在 NAN-Sync 与 NAN 非同步发现（NAN-USD） 模式间通用（例如 ``esp_wifi_nan_publish_service``、``esp_wifi_nan_subscribe_service``）。
        - 初始化 NAN 同步配置时，请使用 ``WIFI_NAN_SYNC_CONFIG_DEFAULT()``，原 ``WIFI_NAN_CONFIG_DEFAULT()`` 已停用。
        - 结构体 ``wifi_nan_config_t`` 已重命名为 ``wifi_nan_sync_config_t``，例如需将 ``wifi_config_t.nan`` 等声明更新为新类型。
        - NAN 数据通道相关的 API 与结构体目前仅适用于 NAN 同步（NAN-Sync）集群。
    - Wi-Fi 事件 ``WIFI_EVENT_NAN_STARTED`` / ``WIFI_EVENT_NAN_STOPPED`` 已重命名为 ``WIFI_EVENT_NAN_SYNC_STARTED`` / ``WIFI_EVENT_NAN_SYNC_STOPPED``。
    - 函数 ``esp_wifi_nan_publish_service`` 的参数 ``ndp_resp_needed`` 已移至结构体 ``wifi_nan_publish_cfg_t``。
    - 结构体 ``wifi_nan_wfa_ssi_t`` 中的 ``proto`` 字段类型现为 ``uint8_t`` （之前为 ``wifi_nan_svc_proto_t``）。如果代码存储该字段的指针或依赖其大小，请按单个八位字节处理。
    - 结构体 ``wifi_nan_publish_cfg_t`` 和 ``wifi_nan_subscribe_cfg_t`` 新增 ``usd_discovery_flag`` 位域及对应的 ``wifi_nan_usd_config_t`` 成员。构造这些结构体时，请为新增字段提供指定初始化器，以避免字段偏移。

- **离信道操作：**
    - 结构体 :cpp:struct:`wifi_action_tx_req_t` 新增 ``bssid`` 字段，该字段会写入发送动作帧的 Address3。请显式初始化该字段；保持全零时将沿用之前的行为，由 Wi-Fi 驱动填入广播地址，从而避免帧携带意外的 BSSID。
    - 结构体 :cpp:struct:`wifi_roc_req_t` 新增 ``allow_broadcast`` 标志，用于控制在驻留信道期间是否将广播/组播动作帧传递给 ``rx_cb`` 回调。现有应用应结合默认值 ``false`` 评估是否需要启用该选项，以便在需要时进行广播节点发现。若动作帧的 Address3/BSSID 本身为广播地址，即便 ``allow_broadcast`` 保持 ``false``，回调仍会收到该帧。
