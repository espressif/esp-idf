Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    - :component_file:`bt/host/bluedroid/api/include/api/esp_hf_defs.h`

        - In :cpp:enum:`esp_hf_cme_err_t`

            - ``ESP_HF_CME_MEMEORY_FULL`` renamed to ``ESP_HF_CME_MEMORY_FULL``
            - ``ESP_HF_CME_MEMEORY_FAILURE`` renamed to ``ESP_HF_CME_MEMORY_FAILURE``

    - :component_file:`bt/host/bluedroid/api/include/api/esp_hf_ag_api.h`

        - ``esp_bt_hf_init(esp_bd_addr_t remote_addr)`` changes into ``esp_hf_ag_init(void)``

        - ``esp_bt_hf_deinit(esp_bd_addr_t remote_addr)`` changes into ``esp_hf_ag_deinit(void)``

            Along with this change, the `bt_bdaddr_t init` and  `bt_bdaddr_t deinit` has been removed from `union btc_arg_t`.

        - ``esp_bt_hf_register_callback`` changes into ``esp_hf_ag_register_callback``

        - ``esp_bt_hf_connect`` changes into ``esp_hf_ag_slc_connect``

        - ``esp_bt_hf_disconnect`` changes into ``esp_hf_ag_slc_disconnect``

        - ``esp_bt_hf_connect_audio`` changes into ``esp_hf_ag_audio_connect``

        - ``esp_bt_hf_disconnect_audio`` changes into ``esp_hf_ag_audio_disconnect``

        - ``esp_bt_hf_vra`` changes into ``esp_hf_ag_vra_control``

        - ``esp_bt_hf_volume_control`` changes into ``esp_hf_ag_volume_control``

        - ``esp_hf_unat_response`` changes into ``esp_hf_ag_unknown_at_send``

        - ``esp_bt_hf_cmee_response`` changes into ``esp_hf_ag_cmee_send``

        - ``esp_bt_hf_indchange_notification`` changes into ``esp_hf_ag_devices_status_indchange``

        - ``esp_bt_hf_cind_response`` changes into ``esp_hf_ag_cind_response``

        - ``esp_bt_hf_cops_response`` changes into ``esp_hf_ag_cops_response``

        - ``esp_bt_hf_clcc_response`` changes into ``esp_hf_ag_clcc_response``

        - ``esp_bt_hf_cnum_response`` changes into ``esp_hf_ag_cnum_response``

        - ``esp_bt_hf_bsir`` changes into ``esp_hf_ag_bsir``

        - ``esp_bt_hf_answer_call`` changes into ``esp_hf_ag_answer_call``

        - ``esp_bt_hf_reject_call`` changes into ``esp_hf_ag_reject_call``

        - ``esp_bt_hf_out_call`` changes into ``esp_hf_ag_out_call``

        - ``esp_bt_hf_end_call`` changes into ``esp_hf_ag_end_call``

        - ``esp_bt_hf_register_data_callback`` changes into ``esp_hf_ag_register_data_callback``

        - ``esp_hf_outgoing_data_ready`` changes into ``esp_hf_ag_outgoing_data_ready``
