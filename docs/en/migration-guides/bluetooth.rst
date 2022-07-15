Migrate Bluetooth to ESP-IDF 5.0
================================

.. only:: SOC_CLASSIC_BT_SUPPORTED

    Bluedroid
    ---------

    The following Bluedroid macros, types, and functions have been renamed:

    - :component_file:`bt/host/bluedroid/api/include/api/esp_gap_ble_api.h`

        - In :cpp:enum:`esp_gap_ble_cb_event_t`

            - ``ESP_GAP_BLE_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT`` renamed to ``ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT``
            - ``ESP_GAP_BLE_SET_PREFERED_PHY_COMPLETE_EVT`` renamed to ``ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT``
            - ``ESP_GAP_BLE_CHANNEL_SELETE_ALGORITHM_EVT`` renamed to ``ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT``

        - ``esp_ble_wl_opration_t`` renamed to :cpp:enum:`esp_ble_wl_operation_t`
        - ``esp_ble_gap_cb_param_t.pkt_data_lenth_cmpl`` renamed to ``pkt_data_length_cmpl``
        - ``esp_ble_gap_cb_param_t.update_whitelist_cmpl.wl_opration`` renamed to ``wl_operation``
        - ``esp_ble_gap_set_prefered_default_phy`` renamed to :cpp:func:`esp_ble_gap_set_preferred_default_phy`
        - ``esp_ble_gap_set_prefered_phy`` renamed to :cpp:func:`esp_ble_gap_set_preferred_phy`

    - :component_file:`bt/host/bluedroid/api/include/api/esp_gatt_defs.h`

        - In :cpp:enum:`esp_gatt_status_t`

            - ``ESP_GATT_ENCRYPED_MITM`` renamed to ``ESP_GATT_ENCRYPTED_MITM``
            - ``ESP_GATT_ENCRYPED_NO_MITM`` renamed to ``ESP_GATT_ENCRYPTED_NO_MITM``

    - :component_file:`bt/host/bluedroid/api/include/api/esp_hf_defs.h`

        - In :cpp:enum:`esp_hf_cme_err_t`

            - ``ESP_HF_CME_MEMEORY_FULL`` renamed to ``ESP_HF_CME_MEMORY_FULL``
            - ``ESP_HF_CME_MEMEORY_FAILURE`` renamed to ``ESP_HF_CME_MEMORY_FAILURE``