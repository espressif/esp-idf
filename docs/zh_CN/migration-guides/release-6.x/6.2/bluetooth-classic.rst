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

        - ``ESP_HF_IND_TYPE_MAX`` 和 ``ESP_HF_AT_RESPONSE_CODE_MAX`` 追加在原有枚举之后，已有取值不变。对这两个枚举做 ``switch`` 且没有 ``default`` 分支时，打开 ``-Werror=switch`` 将无法编译。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_ag_api.h`

        - ``esp_hf_ag_set_codec()`` 的参数类型改为 ``esp_hf_codec_mode_t``。传入 ``ESP_HF_CODEC_CVSD``、``ESP_HF_CODEC_MSBC``、``ESP_HF_CODEC_LC3``，或 ``ESP_HF_CODEC_NONE`` 以恢复协议栈默认偏好。

        - ``ESP_HF_WBS_RESPONSE_EVT`` （``wbs_rep``）：字段 ``codec`` 的类型改为 ``esp_hf_codec_mode_t``。字段名 ``codec`` 未改。新增 ``status`` 字段（``esp_bt_status_t``），用于报告请求是否成功。

        - ``ESP_HF_BCS_RESPONSE_EVT`` （``bcs_rep``）：字段 ``mode`` 的类型改为 ``esp_hf_codec_mode_t``。字段名 ``mode`` 未改。

        - ``esp_hf_ag_init()`` 已废弃，请改用 ``esp_hf_ag_init_with_cfg()``。旧函数仍会同时注册 HFP 和 HSP，与 ``ESP_HF_AG_INIT_CONFIG_DEFAULT()`` 相同。

        - 将 ``service_mask`` 设为 ``ESP_HF_AG_SERVICE_HSP`` 可只注册 HSP，设为 ``ESP_HF_AG_SERVICE_HFP`` 可只注册 HFP。掩码为 0 或包含未知位时返回 ``ESP_ERR_INVALID_ARG``。仅 HSP 的 AG 发布的是 Headset Audio Gateway（UUID ``0x1112``），对端必须是 HSP Headset。Bluedroid HF Client 和 ``hfp_hf`` 示例只查找 Hands-Free AG（UUID ``0x111F``），无法与之连接。

        - ``ESP_HF_CONNECTION_STATE_EVT`` （``conn_stat``）新增 ``peer_service``。RFCOMM 打开成功时为 ``ESP_HF_AG_SERVICE_HFP`` 或 ``ESP_HF_AG_SERVICE_HSP``，SLC 连接和断开时再次上报。打开失败时为 0。两边服务都注册时，出站连接可能从 HFP 回落到 HSP，不能用初始化掩码判断实际连上的服务。

        - 呼出、接听、拒接、挂断、音量控制以及音频连接和断开在 HSP 链路上仍然有效。处理这些呼叫时，协议栈仍可能提交指示器上报等仅 HFP 的结果。这些结果会被丢掉并打印 warning。振铃和 SCO 连接不受影响。

        - 以下接口仅 HFP 链路有效。HSP 链路上请求会被忽略，并打印 warning：``esp_hf_ag_ciev_report()``、``esp_hf_ag_cind_response()``、``esp_hf_ag_cops_response()``、``esp_hf_ag_clcc_response()``、``esp_hf_ag_cnum_response()``。``esp_hf_ag_set_codec()`` 同样仅 HFP 有效，HSP 上 ``ESP_HF_WBS_RESPONSE_EVT`` 会报告失败。``esp_hf_ag_vra_control()`` 在 HSP 链路或对端不支持语音识别时不会发送 ``+BVRA``，并打印 warning，但仍会按请求建立或断开音频连接。

        - ``esp_hf_ag_register_callback()`` 在 ``callback`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先返回 ``ESP_FAIL``。

        - ``esp_hf_ag_vra_control()`` 仅接受 ``ESP_HF_VR_STATE_ENABLED`` 和 ``ESP_HF_VR_STATE_DISABLED``，其他 ``value`` 返回 ``ESP_ERR_INVALID_ARG``。

        - ``esp_hf_ag_volume_control()`` 仅接受 ``ESP_HF_VOLUME_CONTROL_TARGET_SPK`` 和 ``ESP_HF_VOLUME_CONTROL_TARGET_MIC``，其他 ``type`` 返回 ``ESP_ERR_INVALID_ARG``。音量范围 0–15 未改。

        - ``esp_hf_ag_cmee_send()`` 在 ``response_code`` 超出 ``[ESP_HF_AT_RESPONSE_CODE_OK, ESP_HF_AT_RESPONSE_CODE_MAX)`` 时返回 ``ESP_ERR_INVALID_ARG``。``error_code`` 请传入已定义的 ``esp_hf_cme_err_t`` 取值，该枚举是稀疏的。

        - ``esp_hf_ag_unknown_at_send()`` 仍接受 NULL ``unat``，并发送默认的 ERROR 结果。

        - ``esp_hf_ag_ciev_report()`` 在 ``ind_type`` 小于 ``ESP_HF_IND_TYPE_CALL`` 或大于等于 ``ESP_HF_IND_TYPE_MAX`` 时返回 ``ESP_ERR_INVALID_ARG``。

        - ``esp_hf_ag_cind_response()`` 在通话、呼叫建立、网络、漫游或呼叫保持参数超出各自枚举时返回 ``ESP_ERR_INVALID_ARG``。信号强度和电池电量 0–5 的检查保持原样。

        - 除空回调以外，上述参数原先会被下发，调用返回 ``ESP_OK``。

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

        - ``esp_a2d_register_callback()`` 在 ``callback`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先返回 ``ESP_FAIL``。

        - ``esp_a2d_sink_disconnect()`` 和 ``esp_a2d_source_disconnect()`` 在 ``remote_bda`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先返回 ``ESP_FAIL``。

        - ``esp_a2d_sink_connect()`` 和 ``esp_a2d_source_connect()`` 在 ``remote_bda`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先会把空地址传给 ``memcpy``。

        - ``esp_a2d_sink_set_active_device()`` 先检查协议栈状态，再检查 ``conn_hdl``。协议栈未启用或正在反初始化时，非法句柄返回 ``ESP_ERR_INVALID_STATE``。原先返回 ``ESP_ERR_INVALID_ARG``。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_avrc_api.h`

        - 应用侧 AVRCP Controller 命令只能使用事务标签 ``0`` 到 ``ESP_USER_AVRC_TRANS_LABEL_MAX`` （13）。``14`` 和 ``15`` 保留给协议栈（A2DP Sink 切换活动设备时的 Play/Pause）。向发送类 API 传入 14 或 15 会返回 ``ESP_ERR_INVALID_ARG``。``ESP_AVRC_TRANS_LABEL_MAX`` 仍为 15（协议最大值）。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_spp_api.h`

        - ``ESP_SPP_MODE_CB`` 下的 ``esp_spp_write()`` 会把整包数据拷进发送缓冲并立即返回。``ESP_OK`` 表示数据已入队，不是已经发出。``len`` 永远无法放入发送缓冲时返回 ``ESP_ERR_INVALID_SIZE``，这一包不会入队。对于 ``ESP_ERR_INVALID_SIZE``，等待 ``ESP_SPP_CONG_EVT`` 没有用，因为这块缓冲永远装不下。``len`` 能够放入但当前剩余空间不足时返回 ``ESP_ERR_NO_MEM``，这一包不会入队。只有在收到 ``cong`` 为 false 的 ``ESP_SPP_CONG_EVT`` 之后，才能重试这一包。``ESP_OK`` 之后不要再提交同一包数据：协议栈内部之后的失败会保留已入队的数据，不需要应用重发。

        - ``ESP_SPP_CONG_EVT`` 现在只表示发送缓冲解除拥塞。``cong`` 固定为 false。进入拥塞由 ``esp_spp_write()`` 返回 ``ESP_ERR_NO_MEM`` 表示。如果代码在等 ``cong == true``，或者把每一次 ``ESP_SPP_CONG_EVT`` 都当成停止发送的信号，需要改掉。

        - ``ESP_SPP_WRITE_EVT`` 表示一块不超过对端 MTU 的数据已经提交给 RFCOMM。``status`` 为 ``ESP_SPP_SUCCESS``。提交失败不会通过它上报。``write.cong`` 仍可用来决定是否发下一包。不要在这个事件里重发同一包数据。

        - ``esp_spp_cb_param_t.write.cong`` 现在表示发送缓冲拥塞，不再表示 RFCOMM 拥塞。收到 ``cong`` 为 false 的 ``ESP_SPP_WRITE_EVT`` 后再发下一包仍然可用，但不推荐。请优先使用 ``esp_spp_write()`` 的返回值和 ``ESP_SPP_CONG_EVT``。

        - ``esp_spp_cfg_t::tx_buffer_size`` 必须在 ``ESP_SPP_MIN_TX_BUFFER_SIZE`` （100）到 ``ESP_SPP_MAX_TX_BUFFER_SIZE`` （``ESP_SPP_MAX_MTU * 10``）之间。两种模式传入 ``0`` 时，``esp_spp_enhanced_init()`` 都会改用 ``ESP_SPP_DEFAULT_TX_BUFFER_SIZE`` 并打印警告。其他超出该范围的值返回 ``ESP_ERR_INVALID_ARG``。``BT_SPP_DEFAULT_CONFIG()`` 用于 VFS 模式，``BT_SPP_CB_MODE_DEFAULT_CONFIG()`` 用于回调模式，两者都使用 ``ESP_SPP_DEFAULT_TX_BUFFER_SIZE``，当前等于最大值。

        - 每个已建立的 SPP 连接都会在 Bluedroid 内部分配一块 ``tx_buffer_size`` 字节的发送缓冲。``ESP_SPP_DEFAULT_TX_BUFFER_SIZE`` 当前是 9900 字节。最多 8 路连接时，回调模式应用的可用内部 RAM 比修改前少，因为修改前没有这块缓冲。

        - ``ESP_SPP_MODE_VFS`` 的 ``write()`` 仍会阻塞到数据入队。VFS 应用不处理 ``ESP_SPP_CONG_EVT`` 和 ``ESP_SPP_WRITE_EVT``。``tx_buffer_size`` 为 ``0`` 时使用默认值，其他值必须在上述范围内。

        - :example:`bluetooth/bluedroid/classic_bt/bt_spp_initiator` 在 ``ESP_SPP_OPEN_EVT`` 中创建任务，循环调用 ``esp_spp_write()``。返回 ``ESP_ERR_NO_MEM`` 时任务阻塞，直到 ``cong == false`` 的 ``ESP_SPP_CONG_EVT``。``ESP_SPP_CLOSE_EVT`` 停止该任务，任务随后自行删除。

          .. code-block:: c

              // 修改前：下一次写入由 ESP_SPP_WRITE_EVT 触发，
              // ESP_SPP_CONG_EVT 同时表示进入拥塞和解除拥塞。
              case ESP_SPP_WRITE_EVT:
                  esp_spp_write(param->write.handle, len, data);
                  break;
              case ESP_SPP_CONG_EVT:
                  if (!param->cong.cong) {
                      esp_spp_write(param->cong.handle, len, data);
                  }
                  break;

              // 修改后：在应用任务里写入。ESP_OK 表示已入队。
              // ESP_ERR_NO_MEM 则等到解除拥塞后再重试这一包。
              esp_err_t err = esp_spp_write(handle, len, data);
              if (err == ESP_ERR_NO_MEM) {
                  // 阻塞到 cong == false 的 ESP_SPP_CONG_EVT
              }
              // ESP_SPP_WRITE_EVT 只表示一块数据已经提交给 RFCOMM。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_client_api.h`

        - ``esp_hf_client_register_callback()`` 在 ``callback`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先返回 ``ESP_FAIL``。

        - ``esp_hf_client_volume_update()`` 仅接受 ``ESP_HF_VOLUME_CONTROL_TARGET_SPK`` 和 ``ESP_HF_VOLUME_CONTROL_TARGET_MIC``，其他 ``type`` 返回 ``ESP_ERR_INVALID_ARG``。音量范围 0–15 未改。

        - ``esp_hf_client_dial_memory()`` 在 ``location`` 小于 1 时返回 ``ESP_ERR_INVALID_ARG``。

        - ``esp_hf_client_send_chld_cmd()`` 在 ``chld`` 超出 ``ESP_HF_CHLD_TYPE_REL`` 到 ``ESP_HF_CHLD_TYPE_PRIV_X`` 时返回 ``ESP_ERR_INVALID_ARG``。``chld`` 为 ``ESP_HF_CHLD_TYPE_REL_X`` 或 ``ESP_HF_CHLD_TYPE_PRIV_X`` 且 ``idx`` 小于 1 时同样返回 ``ESP_ERR_INVALID_ARG``。

        - ``esp_hf_client_send_btrh_cmd()`` 在 ``btrh`` 超出 ``ESP_HF_BTRH_CMD_HOLD`` 到 ``ESP_HF_BTRH_CMD_REJECT`` 时返回 ``ESP_ERR_INVALID_ARG``。

        - 非法的音量目标、记忆拨号位置、CHLD 命令或 BTRH 命令原先会返回 ``ESP_OK``，随后在协议栈内被丢弃。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_l2cap_bt_api.h`

        - ``esp_bt_l2cap_register_callback()`` 在 ``callback`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先返回 ``ESP_FAIL``。

        - ``esp_bt_l2cap_connect()`` 和 ``esp_bt_l2cap_start_srv()`` 在 ``(psm & 0x0101) != 0x0001`` 时返回 ``ESP_ERR_INVALID_ARG`` （PSM 为偶数，或 bit 8 为 1）。``esp_bt_l2cap_connect()`` 在对端地址为 NULL 时仍返回 ``ESP_ERR_INVALID_ARG``。``esp_bt_l2cap_stop_srv()`` 仍会下发 PSM，``0`` 仍表示停止全部 server，与 ``esp_bt_l2cap_stop_all_srv()`` 相同。connect 和 start 上的非法 PSM 原先会被下发，调用返回 ``ESP_OK``。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_spp_api.h`

        - ``esp_spp_connect()`` 在 ``remote_scn`` 为 0 或大于等于 ``ESP_SPP_MAX_SCN`` （31），或 ``role`` 既不是 ``ESP_SPP_ROLE_MASTER`` 也不是 ``ESP_SPP_ROLE_SLAVE`` 时返回 ``ESP_ERR_INVALID_ARG``。对端地址为 NULL 的检查保持原样。

        - ``esp_spp_start_srv()`` 和 ``esp_spp_start_srv_with_cfg()`` 在 ``local_scn`` 大于等于 ``ESP_SPP_MAX_SCN``，或 ``role`` 既不是 master 也不是 slave 时返回 ``ESP_ERR_INVALID_ARG``。``local_scn`` 为 0 时仍由协议栈分配服务通道。这些取值原先会被下发，调用返回 ``ESP_OK``。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_sdp_api.h`

        - ``esp_sdp_register_callback()`` 在 ``callback`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先返回 ``ESP_FAIL``。

        - ``esp_sdp_search_record()`` 在 ``bd_addr`` 为 NULL，或 ``uuid.len`` 不是 ``ESP_UUID_LEN_16``、``ESP_UUID_LEN_32``、``ESP_UUID_LEN_128`` 时返回 ``ESP_ERR_INVALID_ARG``。空地址原先会在 ``memcpy`` 中出错。不支持的长度原先会被下发，调用返回 ``ESP_OK``。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_pbac_api.h`

        - ``esp_pbac_register_callback()`` 在 ``callback`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先返回 ``ESP_FAIL``。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_gap_bt_api.h`

        - ``esp_bt_gap_read_rssi_delta()`` 和 ``esp_bt_gap_read_acl_real_rssi()`` 在 ``remote_addr`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。空地址原先会传给 ``memcpy``。

        - ``esp_bt_gap_get_profile_status()`` 先检查协议栈状态，再检查 ``profile_status``。协议栈未启用且 ``profile_status`` 为 NULL 时返回 ``ESP_ERR_INVALID_STATE``。原先返回 ``ESP_ERR_INVALID_ARG``。

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_bt_device.h`

        - 新增 ``ESP_BT_DEV_COEX_TYPE_MAX``。``ESP_BT_DEV_COEX_TYPE_BLE`` 和 ``ESP_BT_DEV_COEX_TYPE_BT`` 的取值不变。没有 ``default`` 分支的 ``switch`` 需要按上面 HFP 新增枚举同样处理。

        - ``esp_bt_dev_coex_status_config()`` 在 ``type`` 小于 ``ESP_BT_DEV_COEX_TYPE_BLE`` 或大于等于 ``ESP_BT_DEV_COEX_TYPE_MAX`` 时返回 ``ESP_ERR_INVALID_ARG``。这些取值原先会被下发。

        - ``esp_bt_config_file_path_get()`` 和 ``esp_bt_config_file_path_update()`` 在 ``file_path`` 为 NULL 时返回 ``ESP_ERR_INVALID_ARG``。原先返回 ``ESP_FAIL``。路径长度不小于 ``NVS_NS_NAME_MAX_SIZE`` 时，``esp_bt_config_file_path_update()`` 仍返回 ``ESP_FAIL``。

    函数签名未改。原先合法的参数在请求成功下发时仍返回 ``ESP_OK``。只判断返回值是否等于 ``ESP_OK`` 的代码无需修改。把空指针失败当成 ``ESP_FAIL`` 来比较的代码需要同时接受 ``ESP_ERR_INVALID_ARG``。

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
