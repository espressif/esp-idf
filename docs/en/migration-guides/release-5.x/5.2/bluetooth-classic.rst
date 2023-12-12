Bluetooth Classic
=================

:link_to_translation:`zh_CN:[中文]`

Bluedroid
---------

    The following Bluedroid API have been removed:

    - :component_file:`bt/host/bluedroid/api/include/api/esp_bt_main.h`

        - Remove ``esp_err_t esp_bluedroid_init(void)``

            - Bluedroid stack initialization API has been replaced by ``esp_err_t esp_bluedroid_init_with_cfg(esp_bluedroid_config_t *cfg)``. Macro ``BT_BLUEDROID_INIT_CONFIG_DEFAULT()`` provides the default configuration for the initialization. The original function can be deleted directly.
