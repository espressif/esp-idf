Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid APIs have been deprecated and removed:

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_bt_device.h`

        - Remove ``esp_err_t esp_bt_dev_set_device_name(const char *name)``

            - Set device name API has been replaced by ``esp_err_t esp_bt_gap_set_device_name(const char *name)`` or ``esp_err_t esp_ble_gap_set_device_name(const char *name)``. The original function has been deprecated.

        - Remove ``esp_err_t esp_bt_dev_get_device_name(void)``

            - Get device name API has been replaced by ``esp_err_t esp_bt_gap_get_device_name(void)`` or ``esp_err_t esp_ble_gap_get_device_name(void)``. The original function has been deprecated.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_spp_api.h`

        - Remove ``esp_err_t esp_spp_init(esp_spp_mode_t mode)``

            - SPP init API has been replaced by ``esp_err_t esp_spp_enhanced_init(const esp_spp_cfg_t *cfg)``.
            - A new configuration structure ``esp_spp_cfg_t`` has been introduced as a parameter for ``esp_spp_enhanced_init``.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_hf_ag_api.h`

        - Remove ``esp_err_t esp_hf_ag_devices_status_indchange(...)``

            - Sending indicator reports to the HFP client API has been replaced by ``esp_hf_ag_ciev_report(...)``.

     - :component_file:`/bt/host/bluedroid/api/include/api/esp_gap_bt_api.h`

        - Remove ``ESP_BT_GAP_RSSI_HIGH_THRLD`` and ``ESP_BT_GAP_RSSI_LOW_THRLD``

            - Macros of low and high RSSI threshold have been removed.

    The following Bluedroid API have been changed:

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_sdp_api.h`

        - Field ``user2_ptr_len`` and ``user2_ptr`` is removed in structure ``esp_bluetooth_sdp_hdr_overlay_t``, since they are not used in SDP record creation or searching.

    - :component_file:`/bt/host/bluedroid/api/include/api/esp_a2dp_api.h`

        - Field ``ESP_A2D_MEDIA_CTRL_STOP`` is removed in structure ``esp_a2d_media_ctrl_t``, and is replaced by ``ESP_A2D_MEDIA_CTRL_SUSPEND``.

        - Field ``sbc, m12, m24 and atrac`` is removed and replaced by ``sbc_info, m12_info, m24_info and atrc_info`` in structure ``esp_a2d_mcc_t``.

        - Macro ``ESP_A2D_SBC_CIE_ALLOC_MTHD_SRN`` is renamed to ``ESP_A2D_SBC_CIE_ALLOC_MTHD_SNR``. The old macro is kept for backward compatibility, but it is deprecated and will emit a compile-time warning.
