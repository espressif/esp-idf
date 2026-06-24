经典蓝牙
=================

:link_to_translation:`en:[English]`

Bluedroid
---------

- 以下 Bluedroid API 发生变更
    - :component_file:`/bt/host/bluedroid/api/include/api/esp_bt_main.h`

        - 函数 ``esp_bluedroid_init`` 已被弃用，并由 ``esp_bluedroid_init_with_cfg`` 取代。
        - 新增配置结构体 ``esp_bluedroid_config_t`` 作为 ``esp_bluedroid_init_with_cfg`` 的参数引入。该结构体中的 ``ssp_en`` 字段用于确定配对模式。

- 配置选项 ``CONFIG_BT_SSP_ENABLED`` 已被移除，因为配对模式现在可以在运行时进行配置。
