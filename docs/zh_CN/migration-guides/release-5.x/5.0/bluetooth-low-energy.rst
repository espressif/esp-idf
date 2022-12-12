低功耗蓝牙
====================

:link_to_translation:`en:[English]`

Bluedroid
---------

    以下 Bluedroid 宏、类型和函数已被重命名：

    - :component_file:`bt/host/bluedroid/api/include/api/esp_gap_ble_api.h`

        - :cpp:enum:`esp_gap_ble_cb_event_t` 中：

            - ``ESP_GAP_BLE_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT`` 改名为 ``ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT``
            - ``ESP_GAP_BLE_SET_PREFERED_PHY_COMPLETE_EVT`` 改名为 ``ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT``
            - ``ESP_GAP_BLE_CHANNEL_SELETE_ALGORITHM_EVT`` 改名为 ``ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT``

        - ``esp_ble_wl_opration_t`` 改名为 :cpp:enum:`esp_ble_wl_operation_t`
        - ``esp_ble_gap_cb_param_t.pkt_data_lenth_cmpl`` 改名为 ``pkt_data_length_cmpl``
        - ``esp_ble_gap_cb_param_t.update_whitelist_cmpl.wl_opration`` 改名为 ``wl_operation``
        - ``esp_ble_gap_set_prefered_default_phy`` 改名为 :cpp:func:`esp_ble_gap_set_preferred_default_phy`
        - ``esp_ble_gap_set_prefered_phy`` 改名为 :cpp:func:`esp_ble_gap_set_preferred_phy`

    - :component_file:`bt/host/bluedroid/api/include/api/esp_gatt_defs.h`

        - :cpp:enum:`esp_gatt_status_t` 中：

            - ``ESP_GATT_ENCRYPED_MITM`` 改名为 ``ESP_GATT_ENCRYPTED_MITM``
            - ``ESP_GATT_ENCRYPED_NO_MITM`` 改名为 ``ESP_GATT_ENCRYPTED_NO_MITM``

Nimble
--------

    以下 Nimble API 已被移除：

    - :component_file:`bt/host/nimble/esp-hci/include/esp_nimble_hci.h`

        - 移除 ``esp_err_t esp_nimble_hci_and_controller_init(void)``

            - 控制器初始化、使能以及 HCI 初始化的调用已经被移到 `nimble_port_init` 中。可直接删除该函数。

        - 移除 ``esp_err_t esp_nimble_hci_and_controller_deinit(void)``

            - 控制器去初始化、禁用以及 HCI 去初始化的调用已经被移到 `nimble_port_deinit` 中。可直接删除该函数。

ESP-BLE-MESH
------------

    以下 ESP-BLE-MESH 宏已被重命名：

    - :component_file:`bt/esp_ble_mesh/api/esp_ble_mesh_defs.h`

        - :cpp:enum:`esp_ble_mesh_prov_cb_event_t` 中:

            - ``ESP_BLE_MESH_PROVISIONER_DRIECT_ERASE_SETTINGS_COMP_EVT`` 改名为 ``ESP_BLE_MESH_PROVISIONER_DIRECT_ERASE_SETTINGS_COMP_EVT``
