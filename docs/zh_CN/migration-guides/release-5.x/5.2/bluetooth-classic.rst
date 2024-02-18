经典蓝牙
=================

:link_to_translation:`en:[English]`

Bluedroid
---------

    以下 Bluedroid API 已被移除：

    - :component_file:`bt/host/bluedroid/api/include/api/esp_bt_main.h`

        - 移除 ``esp_err_t esp_bluedroid_init(void)``

            - Bluedroid 协议栈初始化 API 已被替换为 ``esp_err_t esp_bluedroid_init_with_cfg(esp_bluedroid_config_t *cfg)``。宏 ``BT_BLUEDROID_INIT_CONFIG_DEFAULT()`` 用于提供默认的初始化参数。原来的初始化函数可以直接删除。
