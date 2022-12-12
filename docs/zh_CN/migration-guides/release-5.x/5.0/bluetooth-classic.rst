经典蓝牙
=================

:link_to_translation:`en:[English]`

Bluedroid
---------

    - :component_file:`bt/host/bluedroid/api/include/api/esp_hf_defs.h`

        - 在 :cpp:enum:`esp_hf_cme_err_t` 中

            - ``ESP_HF_CME_MEMEORY_FULL`` 改名为 ``ESP_HF_CME_MEMORY_FULL``
            - ``ESP_HF_CME_MEMEORY_FAILURE`` 改名为 ``ESP_HF_CME_MEMORY_FAILURE``

    - :component_file:`bt/host/bluedroid/api/include/api/esp_hf_ag_api.h`

        - ``esp_bt_hf_init(esp_bd_addr_t remote_addr)`` 改为 ``esp_hf_ag_init(void)``

        - ``esp_bt_hf_deinit(esp_bd_addr_t remote_addr)`` 改为 ``esp_hf_ag_deinit(void)``

            除此之外，`bt_bdaddr_t init` 和 `bt_bdaddr_t deinit` 已经从 `union btc_arg_t` 中移除。

        - ``esp_bt_hf_register_callback`` 改为 ``esp_hf_ag_register_callback``

        - ``esp_bt_hf_connect`` 改为 ``esp_hf_ag_slc_connect``

        - ``esp_bt_hf_disconnect`` 改为 ``esp_hf_ag_slc_disconnect``

        - ``esp_bt_hf_connect_audio`` 改为 ``esp_hf_ag_audio_connect``

        - ``esp_bt_hf_disconnect_audio`` 改为 ``esp_hf_ag_audio_disconnect``

        - ``esp_bt_hf_vra`` 改为 ``esp_hf_ag_vra_control``

        - ``esp_bt_hf_volume_control`` 改为 ``esp_hf_ag_volume_control``

        - ``esp_hf_unat_response`` 改为 ``esp_hf_ag_unknown_at_send``

        - ``esp_bt_hf_cmee_response`` 改为 ``esp_hf_ag_cmee_send``

        - ``esp_bt_hf_indchange_notification`` 改为 ``esp_hf_ag_devices_status_indchange``

        - ``esp_bt_hf_cind_response`` 改为 ``esp_hf_ag_cind_response``

        - ``esp_bt_hf_cops_response`` 改为 ``esp_hf_ag_cops_response``

        - ``esp_bt_hf_clcc_response`` 改为 ``esp_hf_ag_clcc_response``

        - ``esp_bt_hf_cnum_response`` 改为 ``esp_hf_ag_cnum_response``

        - ``esp_bt_hf_bsir`` 改为 ``esp_hf_ag_bsir``

        - ``esp_bt_hf_answer_call`` 改为 ``esp_hf_ag_answer_call``

        - ``esp_bt_hf_reject_call`` 改为 ``esp_hf_ag_reject_call``

        - ``esp_bt_hf_out_call`` 改为 ``esp_hf_ag_out_call``

        - ``esp_bt_hf_end_call`` 改为 ``esp_hf_ag_end_call``

        - ``esp_bt_hf_register_data_callback`` 改为 ``esp_hf_ag_register_data_callback``

        - ``esp_hf_outgoing_data_ready`` 改为 ``esp_hf_ag_outgoing_data_ready``
