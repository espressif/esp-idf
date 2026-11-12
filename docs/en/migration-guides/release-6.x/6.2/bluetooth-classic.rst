Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid APIs have been changed:

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_defs.h`

        - Type ``esp_hf_wbs_config_t`` is replaced by ``esp_hf_codec_mode_t``. The new type identifies the negotiated codec (CVSD, mSBC, or LC3-SWB), not a Wideband Speech on/off flag. ``esp_hf_wbs_config_t`` is kept as a deprecated alias of ``esp_hf_codec_mode_t``.

        - Enumerators are renamed. Numeric values 0 / 1 / 2 are unchanged. The old ``ESP_HF_WBS_*`` names remain as macros that alias the new enumerators:

            .. list-table::
               :header-rows: 1
               :widths: 40 40 20

               * - Old enumerator
                 - New enumerator
                 - Value
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

        - Existing ``switch`` statements and comparisons that use ``ESP_HF_WBS_NO`` or ``ESP_HF_WBS_YES`` still compile. Prefer the new names in new code.

        - ``ESP_HF_CODEC_NONE`` means no codec has been negotiated, or the preference is reset to the stack default. It does not mean that CVSD is unavailable. ``ESP_HF_CODEC_CVSD`` is the selected narrowband codec, whether Wideband Speech is unsupported, disabled, or the link fell back from mSBC / LC3.

        - ``ESP_HF_CODEC_LC3`` is new. Code that indexes a three-entry table with the codec value must add an LC3 entry or a default branch.

        - Do not use ``esp_hf_codec_mode_t`` as a capability bitmap. Peer codec capabilities are reported separately as ``ESP_HF_CODEC_CAP_CVSD`` / ``ESP_HF_CODEC_CAP_MSBC`` / ``ESP_HF_CODEC_CAP_LC3`` in ``ESP_HF_BAC_RESPONSE_EVT``.

        - Audio connection state ``ESP_HF_AUDIO_STATE_CONNECTED_LC3`` is added for an established LC3-SWB link, alongside the existing ``ESP_HF_AUDIO_STATE_CONNECTED`` (CVSD) and ``ESP_HF_AUDIO_STATE_CONNECTED_MSBC`` values.

        - ``ESP_HF_IND_TYPE_MAX`` and ``ESP_HF_AT_RESPONSE_CODE_MAX`` are appended after the last defined enumerator. Existing values are unchanged. A ``switch`` on either enumeration that has no ``default`` branch fails to compile when ``-Werror=switch`` is enabled.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_ag_api.h`

        - ``esp_hf_ag_set_codec()`` now takes ``esp_hf_codec_mode_t``. Pass ``ESP_HF_CODEC_CVSD``, ``ESP_HF_CODEC_MSBC``, ``ESP_HF_CODEC_LC3``, or ``ESP_HF_CODEC_NONE`` to restore the stack default.

        - ``ESP_HF_WBS_RESPONSE_EVT`` (``wbs_rep``): field ``codec`` is now ``esp_hf_codec_mode_t``. The field name ``codec`` is unchanged. A ``status`` field (``esp_bt_status_t``) is added to report whether the request succeeded.

        - ``ESP_HF_BCS_RESPONSE_EVT`` (``bcs_rep``): field ``mode`` is now ``esp_hf_codec_mode_t``. The field name ``mode`` is unchanged.

        - ``esp_hf_ag_init()`` is deprecated. Use ``esp_hf_ag_init_with_cfg()``. The old function still registers both HFP and HSP, which is the same as ``ESP_HF_AG_INIT_CONFIG_DEFAULT()``.

        - Set ``service_mask`` to ``ESP_HF_AG_SERVICE_HSP`` to register HSP only, or ``ESP_HF_AG_SERVICE_HFP`` to register HFP only. A zero mask or any unknown bit returns ``ESP_ERR_INVALID_ARG``. An HSP-only AG advertises Headset Audio Gateway (UUID ``0x1112``). The peer must be an HSP Headset. The Bluedroid HF Client and the ``hfp_hf`` example search only for Hands-Free AG (UUID ``0x111F``) and will not connect.

        - ``ESP_HF_CONNECTION_STATE_EVT`` (``conn_stat``) adds ``peer_service``. It is ``ESP_HF_AG_SERVICE_HFP`` or ``ESP_HF_AG_SERVICE_HSP`` for the service that opened, and is reported again on SLC connected and disconnect. It is 0 when the open fails. When both services are registered, an outgoing connection can fall back from HFP to HSP, so the initialized mask does not identify the connected service.

        - Call setup, call answer, call reject, call end, volume control, and audio connect or disconnect still apply on an HSP link. While handling those calls the stack may still submit HFP-only results such as indicator reports. Those results are dropped and a warning is logged. Ring and the SCO connection are unchanged.

        - The following calls apply to an HFP link only. On an HSP link the request is ignored and a warning is logged: ``esp_hf_ag_ciev_report()``, ``esp_hf_ag_cind_response()``, ``esp_hf_ag_cops_response()``, ``esp_hf_ag_clcc_response()``, and ``esp_hf_ag_cnum_response()``. ``esp_hf_ag_set_codec()`` is also HFP only. On HSP, ``ESP_HF_WBS_RESPONSE_EVT`` reports failure. ``esp_hf_ag_vra_control()`` does not send ``+BVRA`` on an HSP link or when the peer does not support voice recognition, and a warning is logged. Audio is still connected or disconnected according to the requested state.

        - ``esp_hf_ag_register_callback()`` returns ``ESP_ERR_INVALID_ARG`` when ``callback`` is NULL. It previously returned ``ESP_FAIL``.

        - ``esp_hf_ag_vra_control()`` returns ``ESP_ERR_INVALID_ARG`` unless ``value`` is ``ESP_HF_VR_STATE_ENABLED`` or ``ESP_HF_VR_STATE_DISABLED``.

        - ``esp_hf_ag_volume_control()`` returns ``ESP_ERR_INVALID_ARG`` unless ``type`` is ``ESP_HF_VOLUME_CONTROL_TARGET_SPK`` or ``ESP_HF_VOLUME_CONTROL_TARGET_MIC``. The volume range 0–15 is unchanged.

        - ``esp_hf_ag_cmee_send()`` returns ``ESP_ERR_INVALID_ARG`` when ``response_code`` is outside ``[ESP_HF_AT_RESPONSE_CODE_OK, ESP_HF_AT_RESPONSE_CODE_MAX)``. Pass a defined ``esp_hf_cme_err_t`` value; that enumeration is sparse.

        - ``esp_hf_ag_unknown_at_send()`` still accepts a NULL ``unat`` and sends the default ERROR result.

        - ``esp_hf_ag_ciev_report()`` returns ``ESP_ERR_INVALID_ARG`` when ``ind_type`` is less than ``ESP_HF_IND_TYPE_CALL`` or greater than or equal to ``ESP_HF_IND_TYPE_MAX``.

        - ``esp_hf_ag_cind_response()`` returns ``ESP_ERR_INVALID_ARG`` when the call, call-setup, network, roaming, or call-held argument is outside its enumeration. Signal and battery level 0–5 were already rejected.

        - Except for the NULL callback, the arguments above were previously forwarded and the call returned ``ESP_OK``.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_a2dp_api.h`

        - ``ESP_A2D_MEDIA_CTRL_ACK_EVT`` (``media_ctrl_stat``) now includes ``conn_hdl``, the A2DP connection handle of the command being acknowledged.

        - Disconnect and media control can target a connection by handle. The handle is the ``conn_hdl`` reported in ``ESP_A2D_CONNECTION_STATE_EVT``. The previous address-based and handle-less APIs remain, and are deprecated:

            .. list-table::
               :header-rows: 1
               :widths: 50 50

               * - Deprecated API
                 - Replacement
               * - ``esp_a2d_sink_disconnect()``
                 - ``esp_a2d_sink_disconnect_by_handle()``
               * - ``esp_a2d_source_disconnect()``
                 - ``esp_a2d_source_disconnect_by_handle()``
               * - ``esp_a2d_media_ctrl()``
                 - ``esp_a2d_media_ctrl_by_handle()``

        - A handle that does not match the active A2DP connection is not applied. Media control reports ``ESP_A2D_MEDIA_CTRL_ACK_FAILURE``. Disconnect is ignored.

        - ``esp_a2d_register_callback()`` returns ``ESP_ERR_INVALID_ARG`` when ``callback`` is NULL. It previously returned ``ESP_FAIL``.

        - ``esp_a2d_sink_disconnect()`` and ``esp_a2d_source_disconnect()`` return ``ESP_ERR_INVALID_ARG`` when ``remote_bda`` is NULL. They previously returned ``ESP_FAIL``.

        - ``esp_a2d_sink_connect()`` and ``esp_a2d_source_connect()`` return ``ESP_ERR_INVALID_ARG`` when ``remote_bda`` is NULL. A NULL address was previously passed to ``memcpy``.

        - ``esp_a2d_sink_set_active_device()`` checks the stack status before ``conn_hdl``. An invalid handle while the stack is disabled, or while deinitialization is in progress, returns ``ESP_ERR_INVALID_STATE``. It previously returned ``ESP_ERR_INVALID_ARG``.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_avrc_api.h`

        - The ``*_by_handle`` AVRCP Controller send APIs accept transaction labels ``0`` through ``ESP_USER_AVRC_TRANS_LABEL_MAX`` (13). Labels ``14`` and ``15`` are reserved for the stack (A2DP sink active-device Play/Pause). Passing 14 or 15 to those APIs returns ``ESP_ERR_INVALID_ARG``. The deprecated send APIs still accept ``0`` through ``ESP_AVRC_TRANS_LABEL_MAX`` (15).

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_spp_api.h`

        - ``esp_spp_write()`` in ``ESP_SPP_MODE_CB`` copies the whole buffer into the transmit buffer and returns immediately. ``ESP_OK`` means the data is queued, not that it has been sent. If ``len`` can never fit in the TX buffer, the call returns ``ESP_ERR_INVALID_SIZE`` and queues nothing. For ``ESP_ERR_INVALID_SIZE``, waiting for ``ESP_SPP_CONG_EVT`` does not help because the buffer never shrinks. If ``len`` can fit but the free space is smaller, the call returns ``ESP_ERR_NO_MEM`` and queues nothing. Retry that buffer only after ``ESP_SPP_CONG_EVT`` with ``cong`` equal to false. After ``ESP_OK``, do not submit the same buffer again: a later failure inside the stack keeps the queued data and is not a reason to resend it.

        - ``ESP_SPP_CONG_EVT`` is only the uncongest edge of the TX buffer. ``cong`` is always false. Becoming congested is ``ESP_ERR_NO_MEM`` from ``esp_spp_write()``. Update code that waits for ``cong == true``, or that treats every ``ESP_SPP_CONG_EVT`` as a signal to stop writing.

        - ``ESP_SPP_WRITE_EVT`` reports that one chunk, of at most the peer MTU, has been submitted to RFCOMM. ``status`` is ``ESP_SPP_SUCCESS``. A failed submission is not reported through it. ``write.cong`` can still gate the next packet. Do not resend the same buffer from this event.

        - ``esp_spp_cb_param_t.write.cong`` reports TX buffer congestion. It no longer reports RFCOMM congestion. Sending the next packet only after ``ESP_SPP_WRITE_EVT`` with ``cong`` equal to false still works, but is not recommended. Prefer the ``esp_spp_write()`` return value and ``ESP_SPP_CONG_EVT``.

        - ``esp_spp_cfg_t::tx_buffer_size`` must be from ``ESP_SPP_MIN_TX_BUFFER_SIZE`` (100) to ``ESP_SPP_MAX_TX_BUFFER_SIZE`` (``ESP_SPP_MAX_MTU * 10``). 0 is accepted in both modes: ``esp_spp_enhanced_init()`` uses ``ESP_SPP_DEFAULT_TX_BUFFER_SIZE`` and logs a warning. Any other size outside this range returns ``ESP_ERR_INVALID_ARG``. ``BT_SPP_DEFAULT_CONFIG()`` is VFS mode and ``BT_SPP_CB_MODE_DEFAULT_CONFIG()`` is callback mode. Both use ``ESP_SPP_DEFAULT_TX_BUFFER_SIZE``, which is currently equal to the maximum.

        - Each open SPP connection allocates a transmit buffer of ``tx_buffer_size`` bytes inside Bluedroid. ``ESP_SPP_DEFAULT_TX_BUFFER_SIZE`` is currently 9900 bytes. With up to 8 connections, callback-mode applications have less internal RAM available than before, when this buffer was not allocated.

        - ``ESP_SPP_MODE_VFS`` ``write()`` still blocks until the data is queued. VFS applications do not handle ``ESP_SPP_CONG_EVT`` or ``ESP_SPP_WRITE_EVT``. ``tx_buffer_size`` 0 uses the default; any other value must be in the range above.

        - :example:`bluetooth/bluedroid/classic_bt/bt_spp_initiator` creates a task on ``ESP_SPP_OPEN_EVT`` and loops on ``esp_spp_write()``. ``ESP_ERR_NO_MEM`` blocks that task until ``ESP_SPP_CONG_EVT`` with ``cong == false``. ``ESP_SPP_CLOSE_EVT`` stops the task, and the task deletes itself.

          .. code-block:: c

              // Before: the next write was started from ESP_SPP_WRITE_EVT,
              // and ESP_SPP_CONG_EVT reported both congested and uncongested.
              case ESP_SPP_WRITE_EVT:
                  esp_spp_write(param->write.handle, len, data);
                  break;
              case ESP_SPP_CONG_EVT:
                  if (!param->cong.cong) {
                      esp_spp_write(param->cong.handle, len, data);
                  }
                  break;

              // After: write from the application task. ESP_OK means queued.
              // ESP_ERR_NO_MEM waits for the uncongest event, then retries this buffer.
              esp_err_t err = esp_spp_write(handle, len, data);
              if (err == ESP_ERR_NO_MEM) {
                  // block until ESP_SPP_CONG_EVT with cong == false
              }
              // ESP_SPP_WRITE_EVT only reports that a chunk was submitted to RFCOMM.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_client_api.h`

        - ``esp_hf_client_register_callback()`` returns ``ESP_ERR_INVALID_ARG`` when ``callback`` is NULL. It previously returned ``ESP_FAIL``.

        - ``esp_hf_client_volume_update()`` returns ``ESP_ERR_INVALID_ARG`` unless ``type`` is ``ESP_HF_VOLUME_CONTROL_TARGET_SPK`` or ``ESP_HF_VOLUME_CONTROL_TARGET_MIC``. The volume range 0–15 is unchanged.

        - ``esp_hf_client_dial_memory()`` returns ``ESP_ERR_INVALID_ARG`` when ``location`` is less than 1.

        - ``esp_hf_client_send_chld_cmd()`` returns ``ESP_ERR_INVALID_ARG`` when ``chld`` is outside ``ESP_HF_CHLD_TYPE_REL`` through ``ESP_HF_CHLD_TYPE_PRIV_X``, or when ``idx`` is less than 1 for ``ESP_HF_CHLD_TYPE_REL_X`` or ``ESP_HF_CHLD_TYPE_PRIV_X``.

        - ``esp_hf_client_send_btrh_cmd()`` returns ``ESP_ERR_INVALID_ARG`` when ``btrh`` is outside ``ESP_HF_BTRH_CMD_HOLD`` through ``ESP_HF_BTRH_CMD_REJECT``.

        - An invalid volume target, memory location, CHLD command, or BTRH command was previously accepted with ``ESP_OK`` and then dropped in the stack.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_l2cap_bt_api.h`

        - ``esp_bt_l2cap_register_callback()`` returns ``ESP_ERR_INVALID_ARG`` when ``callback`` is NULL. It previously returned ``ESP_FAIL``.

        - ``esp_bt_l2cap_connect()`` and ``esp_bt_l2cap_start_srv()`` return ``ESP_ERR_INVALID_ARG`` when ``(psm & 0x0101) != 0x0001`` (the PSM is even, or bit 8 is set). ``esp_bt_l2cap_connect()`` still returns ``ESP_ERR_INVALID_ARG`` for a NULL peer address. ``esp_bt_l2cap_stop_srv()`` still forwards its PSM, and ``0`` still stops every server, the same as ``esp_bt_l2cap_stop_all_srv()``. An invalid PSM on connect or start was previously forwarded and the call returned ``ESP_OK``.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_spp_api.h`

        - ``esp_spp_connect()`` returns ``ESP_ERR_INVALID_ARG`` when ``remote_scn`` is 0 or greater than or equal to ``ESP_SPP_MAX_SCN`` (31), or when ``role`` is neither ``ESP_SPP_ROLE_MASTER`` nor ``ESP_SPP_ROLE_SLAVE``. A NULL peer address was already rejected.

        - ``esp_spp_start_srv()`` and ``esp_spp_start_srv_with_cfg()`` return ``ESP_ERR_INVALID_ARG`` when ``local_scn`` is greater than or equal to ``ESP_SPP_MAX_SCN``, or when ``role`` is neither master nor slave. ``local_scn`` 0 still asks the stack to allocate a server channel. These values were previously forwarded and the call returned ``ESP_OK``.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_sdp_api.h`

        - ``esp_sdp_register_callback()`` returns ``ESP_ERR_INVALID_ARG`` when ``callback`` is NULL. It previously returned ``ESP_FAIL``.

        - ``esp_sdp_search_record()`` returns ``ESP_ERR_INVALID_ARG`` when ``bd_addr`` is NULL, or when ``uuid.len`` is not ``ESP_UUID_LEN_16``, ``ESP_UUID_LEN_32``, or ``ESP_UUID_LEN_128``. A NULL address previously faulted in ``memcpy``. An unsupported length was forwarded and the call returned ``ESP_OK``.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_pbac_api.h`

        - ``esp_pbac_register_callback()`` returns ``ESP_ERR_INVALID_ARG`` when ``callback`` is NULL. It previously returned ``ESP_FAIL``.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_gap_bt_api.h`

        - ``esp_bt_gap_read_rssi_delta()`` and ``esp_bt_gap_read_acl_real_rssi()`` return ``ESP_ERR_INVALID_ARG`` when ``remote_addr`` is NULL. A NULL address was previously passed to ``memcpy``.

        - ``esp_bt_gap_get_profile_status()`` checks the stack status before ``profile_status``. A NULL ``profile_status`` while the stack is disabled returns ``ESP_ERR_INVALID_STATE``. It previously returned ``ESP_ERR_INVALID_ARG``.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_bt_device.h`

        - ``ESP_BT_DEV_COEX_TYPE_MAX`` is appended. ``ESP_BT_DEV_COEX_TYPE_BLE`` and ``ESP_BT_DEV_COEX_TYPE_BT`` keep their values. A ``switch`` without a ``default`` branch needs the same treatment as the new HFP enumerators.

        - ``esp_bt_dev_coex_status_config()`` returns ``ESP_ERR_INVALID_ARG`` when ``type`` is less than ``ESP_BT_DEV_COEX_TYPE_BLE`` or greater than or equal to ``ESP_BT_DEV_COEX_TYPE_MAX``. Those values were previously forwarded.

        - ``esp_bt_config_file_path_get()`` and ``esp_bt_config_file_path_update()`` return ``ESP_ERR_INVALID_ARG`` when ``file_path`` is NULL. They previously returned ``ESP_FAIL``. ``esp_bt_config_file_path_update()`` still returns ``ESP_FAIL`` when the path length is not less than ``NVS_NS_NAME_MAX_SIZE``.

    Function signatures are unchanged. An argument that was already valid still returns ``ESP_OK`` when the request is sent. Code that only tests the result against ``ESP_OK`` does not need to change. Code that treats a NULL-pointer failure as ``ESP_FAIL`` must accept ``ESP_ERR_INVALID_ARG``.

    Suggested replacements:

    .. code-block:: c

        // Before
        esp_hf_ag_set_codec(bda, ESP_HF_WBS_YES);
        if (param->bcs_rep.mode == ESP_HF_WBS_NO) { /* CVSD */ }

        // After
        esp_hf_ag_set_codec(bda, ESP_HF_CODEC_MSBC);
        if (param->bcs_rep.mode == ESP_HF_CODEC_CVSD) { /* CVSD */ }
        else if (param->bcs_rep.mode == ESP_HF_CODEC_LC3) { /* LC3-SWB */ }

        // Before
        esp_hf_ag_init();

        // After: both services, same as the deprecated function
        esp_hf_ag_cfg_t cfg = ESP_HF_AG_INIT_CONFIG_DEFAULT();
        esp_hf_ag_init_with_cfg(&cfg);

        // After: HSP only
        cfg.service_mask = ESP_HF_AG_SERVICE_HSP;
        esp_hf_ag_init_with_cfg(&cfg);
