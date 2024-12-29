Wi-Fi
=====

:link_to_translation:`en:[English]`


Wi-Fi 扫描和连接
------------------------

    以下类型已被更改：

    - :component_file:`esp_wifi/include/esp_wifi_he_types.h`

        - :cpp:struct:`esp_wifi_htc_omc_t` 中:

            - ``uph_id``， ``ul_pw_headroom``， ``min_tx_pw_flag`` 字段被弃用

    - :component_file:`esp_wifi/include/esp_wifi_types_generic.h`

        - :cpp:struct:`wifi_ap_record_t` 中:

            - ``bandwidth`` 的类型从 ``uint8_t`` 更改为 ``wifi_bandwidth_t``
