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

    建议替换：

    .. code-block:: c

        // 修改前
        esp_hf_ag_set_codec(bda, ESP_HF_WBS_YES);
        if (param->bcs_rep.mode == ESP_HF_WBS_NO) { /* CVSD */ }

        // 修改后
        esp_hf_ag_set_codec(bda, ESP_HF_CODEC_MSBC);
        if (param->bcs_rep.mode == ESP_HF_CODEC_CVSD) { /* CVSD */ }
        else if (param->bcs_rep.mode == ESP_HF_CODEC_LC3) { /* LC3-SWB */ }
