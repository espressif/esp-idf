经典蓝牙
========

:link_to_translation:`en:[English]`

Bluedroid
---------

    以下 Bluedroid API 已变更：

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_defs.h`

        - 类型 ``esp_hf_wbs_config_t`` 已替换为 ``esp_hf_codec_mode_t``，新的公开类型表示协商得到的编解码（CVSD， mSBC 或 LC3-SWB），而不再是“是否开启宽带语音”的开关。``esp_hf_wbs_config_t`` 保留为 ``esp_hf_codec_mode_t`` 的废弃别名。
        - 枚举名已重命名，数值 0 / 1 / 2 保持不变。旧的 ``ESP_HF_WBS_*`` 名称仍作为宏别名指向新枚举：

            .. list-table::
               :header-rows: 1
               :widths: 40 40 20

               * - 旧枚举
                 - 新枚举
                 - 取值
               * - ``ESP_HF_WBS_NONE``
                 - ``ESP_HF_CODEC_NONE``
                 - 0
               * - ``ESP_HF_WBS_NO``
                 - ``ESP_HF_CODEC_CVSD``
                 - 1
               * - ``ESP_HF_WBS_YES``
                 - ``ESP_HF_CODEC_MSBC``
                 - 2
               * - —
                 - ``ESP_HF_CODEC_LC3``
                 - 3

        - 现有对 ``ESP_HF_WBS_NO`` / ``ESP_HF_WBS_YES`` 的 ``switch`` 或比较仍可编译。新代码请使用新名称。

        - ``ESP_HF_CODEC_NONE`` 表示尚未协商编解码，或将偏好重置为协议栈默认值，并不表示不能使用 CVSD。``ESP_HF_CODEC_CVSD`` 表示当前选中的是窄带编解码，可能是对端不支持宽带、未开启宽带，或从 mSBC / LC3 回退。

        - ``ESP_HF_CODEC_LC3`` 为新增取值。若代码用编解码值去下标访问只有 3 项的表，需补充 LC3 项或增加 default 分支。

        - 不要把 ``esp_hf_codec_mode_t`` 当作能力位图。对端编解码能力通过 ``ESP_HF_BAC_RESPONSE_EVT`` 中的 ``ESP_HF_CODEC_CAP_CVSD`` / ``ESP_HF_CODEC_CAP_MSBC`` / ``ESP_HF_CODEC_CAP_LC3`` 单独上报。

        - 音频连接状态新增 ``ESP_HF_AUDIO_STATE_CONNECTED_LC3``，用于表示已建立的 LC3-SWB 链路，与现有的 ``ESP_HF_AUDIO_STATE_CONNECTED`` （CVSD）和 ``ESP_HF_AUDIO_STATE_CONNECTED_MSBC`` 并列。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_ag_api.h`

        - ``esp_hf_ag_set_codec()`` 的参数类型改为 ``esp_hf_codec_mode_t``。传入 ``ESP_HF_CODEC_CVSD``、``ESP_HF_CODEC_MSBC``、``ESP_HF_CODEC_LC3``，或 ``ESP_HF_CODEC_NONE`` 以恢复协议栈默认偏好。

        - ``ESP_HF_WBS_RESPONSE_EVT`` （``wbs_rep``）：字段 ``codec`` 的类型改为 ``esp_hf_codec_mode_t``。字段名 ``codec`` 未改。新增 ``status`` 字段（``esp_bt_status_t``），用于报告请求是否成功。

        - ``ESP_HF_BCS_RESPONSE_EVT`` （``bcs_rep``）：字段 ``mode`` 的类型改为 ``esp_hf_codec_mode_t``。字段名 ``mode`` 未改。

        - ``esp_hf_ag_init()`` 已废弃，请改用 ``esp_hf_ag_init_with_cfg()``。旧函数仍会同时注册 HFP 和 HSP，与 ``ESP_HF_AG_INIT_CONFIG_DEFAULT()`` 相同。

        - 将 ``service_mask`` 设为 ``ESP_HF_AG_SERVICE_HSP`` 可只注册 HSP，设为 ``ESP_HF_AG_SERVICE_HFP`` 可只注册 HFP。掩码为 0 或包含未知位时返回 ``ESP_ERR_INVALID_ARG``。仅 HSP 的 AG 发布的是 Headset Audio Gateway（UUID ``0x1112``），对端必须是 HSP Headset。Bluedroid HF Client 和 ``hfp_hf`` 示例只查找 Hands-Free AG（UUID ``0x111F``），无法与之连接。

        - ``ESP_HF_CONNECTION_STATE_EVT`` （``conn_stat``）新增 ``peer_service``。RFCOMM 打开成功时为 ``ESP_HF_AG_SERVICE_HFP`` 或 ``ESP_HF_AG_SERVICE_HSP``，SLC 连接和断开时再次上报。打开失败时为 0。两边服务都注册时，出站连接可能从 HFP 回落到 HSP，不能用初始化掩码判断实际连上的服务。

        - 呼出、接听、拒接、挂断、音量控制以及音频连接和断开在 HSP 链路上仍然有效。处理这些呼叫时，协议栈仍可能提交指示器上报等仅 HFP 的结果。这些结果会被丢掉并打印 warning。振铃和 SCO 连接不受影响。

        - 以下接口仅 HFP 链路有效。HSP 链路上请求会被忽略，并打印 warning：``esp_hf_ag_ciev_report()``、``esp_hf_ag_cind_response()``、``esp_hf_ag_cops_response()``、``esp_hf_ag_clcc_response()``、``esp_hf_ag_cnum_response()``。``esp_hf_ag_set_codec()`` 同样仅 HFP 有效，HSP 上 ``ESP_HF_WBS_RESPONSE_EVT`` 会报告失败。``esp_hf_ag_vra_control()`` 在 HSP 链路或对端不支持语音识别时不会发送 ``+BVRA``，并打印 warning，但仍会按请求建立或断开音频连接。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_a2dp_api.h`

        - ``ESP_A2D_MEDIA_CTRL_ACK_EVT`` （``media_ctrl_stat``）新增 ``conn_hdl``，表示被确认的媒体控制命令所属的 A2DP 连接句柄。

        - 断开连接和媒体控制可以按连接句柄指定目标连接。句柄来自 ``ESP_A2D_CONNECTION_STATE_EVT`` 上报的 ``conn_hdl``。原先按地址、或不带连接参数的 API 仍然保留，并标记为废弃：

            .. list-table::
               :header-rows: 1
               :widths: 50 50

               * - 废弃 API
                 - 替代 API
               * - ``esp_a2d_sink_disconnect()``
                 - ``esp_a2d_sink_disconnect_by_handle()``
               * - ``esp_a2d_source_disconnect()``
                 - ``esp_a2d_source_disconnect_by_handle()``
               * - ``esp_a2d_media_ctrl()``
                 - ``esp_a2d_media_ctrl_by_handle()``

        - 与当前 A2DP 连接不匹配的句柄不会生效。媒体控制会以 ``ESP_A2D_MEDIA_CTRL_ACK_FAILURE`` 应答。断开请求会被忽略。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_avrc_api.h`

        - 应用侧 AVRCP Controller 命令只能使用事务标签 ``0`` 到 ``ESP_USER_AVRC_TRANS_LABEL_MAX`` （13）。``14`` 和 ``15`` 保留给协议栈（A2DP Sink 切换活动设备时的 Play/Pause）。向发送类 API 传入 14 或 15 会返回 ``ESP_ERR_INVALID_ARG``。``ESP_AVRC_TRANS_LABEL_MAX`` 仍为 15（协议最大值）。

    建议替换：

    .. code-block:: c

        // 修改前
        esp_hf_ag_set_codec(bda, ESP_HF_WBS_YES);
        if (param->bcs_rep.mode == ESP_HF_WBS_NO) { /* CVSD */ }

        // 修改后
        esp_hf_ag_set_codec(bda, ESP_HF_CODEC_MSBC);
        if (param->bcs_rep.mode == ESP_HF_CODEC_CVSD) { /* CVSD */ }
        else if (param->bcs_rep.mode == ESP_HF_CODEC_LC3) { /* LC3-SWB */ }

        // 修改前
        esp_hf_ag_init();

        // 修改后：同时注册两个服务，与废弃函数相同
        esp_hf_ag_cfg_t cfg = ESP_HF_AG_INIT_CONFIG_DEFAULT();
        esp_hf_ag_init_with_cfg(&cfg);

        // 修改后：仅 HSP
        cfg.service_mask = ESP_HF_AG_SERVICE_HSP;
        esp_hf_ag_init_with_cfg(&cfg);
