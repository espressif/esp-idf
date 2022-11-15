Bluetooth Low Energy
====================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid macros, types, and functions have been renamed:

    - :component_file:`bt/host/bluedroid/api/include/api/esp_gap_ble_api.h`

        - In :cpp:enum:`esp_gap_ble_cb_event_t`:

            - ``ESP_GAP_BLE_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT`` renamed to ``ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT``
            - ``ESP_GAP_BLE_SET_PREFERED_PHY_COMPLETE_EVT`` renamed to ``ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT``
            - ``ESP_GAP_BLE_CHANNEL_SELETE_ALGORITHM_EVT`` renamed to ``ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT``

        - ``esp_ble_wl_opration_t`` renamed to :cpp:enum:`esp_ble_wl_operation_t`
        - ``esp_ble_gap_cb_param_t.pkt_data_lenth_cmpl`` renamed to ``pkt_data_length_cmpl``
        - ``esp_ble_gap_cb_param_t.update_whitelist_cmpl.wl_opration`` renamed to ``wl_operation``
        - ``esp_ble_gap_set_prefered_default_phy`` renamed to :cpp:func:`esp_ble_gap_set_preferred_default_phy`
        - ``esp_ble_gap_set_prefered_phy`` renamed to :cpp:func:`esp_ble_gap_set_preferred_phy`

    - :component_file:`bt/host/bluedroid/api/include/api/esp_gatt_defs.h`

        - In :cpp:enum:`esp_gatt_status_t`:

            - ``ESP_GATT_ENCRYPED_MITM`` renamed to ``ESP_GATT_ENCRYPTED_MITM``
            - ``ESP_GATT_ENCRYPED_NO_MITM`` renamed to ``ESP_GATT_ENCRYPTED_NO_MITM``

Nimble
--------

    The following Nimble APIs have been removed:

    - :component_file:`bt/host/nimble/esp-hci/include/esp_nimble_hci.h`

        - Remove ``esp_err_t esp_nimble_hci_and_controller_init(void)``

            - Controller initialization, enable and HCI initialization calls have been moved to `nimble_port_init`. This function can be deleted directly.

        - Remove ``esp_err_t esp_nimble_hci_and_controller_deinit(void)``

            - Controller deinitialization, disable and HCI deinitialization calls have been moved to `nimble_port_deinit`. This function can be deleted directly.

ESP-BLE-MESH
------------

    The following ESP-BLE-MESH macro has been renamed:

    - :component_file:`bt/esp_ble_mesh/api/esp_ble_mesh_defs.h`

        - In :cpp:enum:`esp_ble_mesh_prov_cb_event_t`:

            - ``ESP_BLE_MESH_PROVISIONER_DRIECT_ERASE_SETTINGS_COMP_EVT`` renamed to ``ESP_BLE_MESH_PROVISIONER_DIRECT_ERASE_SETTINGS_COMP_EVT``
