系统
======

:link_to_translation:`zh_CN:[中文]`

系统时间
--------

* ``{IDF_TARGET_NAME}/rtc.h`` 已弃用，应使用 ``esp_rtc_time.h`` 替代。

日志
----

**Log V2** 是 ESP-IDF v5.5 版本提供的增强型可选日志功能。它与 **Log V1** 完全兼容，项目无需修改即可继续使用 **Log V1**。开发者可以通过 Kconfig 选项 :ref:`CONFIG_LOG_VERSION` 启用 **Log V2**。未来的 ESP-IDF 版本可能默认使用 **Log V2**。

**特性**

- 集中化日志处理、可动态设置的格式参数和更高的灵活性。
- 消耗更多的栈和内存，但二进制文件体积更小（具体数据详见文档 `日志库 <../../../system/log.rst>`_）。
- **Log V2** 的特定功能（如动态设置格式、执行上下文检测）不能向后兼容 **Log V1**。

如需使用 ``esp_log_write`` （或 ``esp_log_writev``）函数，请替换为 ``esp_log`` （或 ``esp_log_va``），以减少一层嵌套调用。

日志处理程序 (``esp_log``) 支持内部格式化，因此无需手动构建格式。可以通过 ``ESP_LOG_LEVEL_LOCAL`` 简化以下代码。

.. code-block:: c

    #if ESP_LOG_VERSION == 2
        ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, OT_PLAT_LOG_TAG, format, args);
    #else
        if (LOG_LOCAL_LEVEL >= ESP_LOG_ERROR) {
            esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR), OT_PLAT_LOG_TAG, LOG_COLOR_E "E(%lu) %s:", esp_log_timestamp(), OT_PLAT_LOG_TAG);
            esp_log_va(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR), OT_PLAT_LOG_TAG, format, args);
            esp_log(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR), OT_PLAT_LOG_TAG, LOG_RESET_COLOR "\n");
        }
    #endif

预加密 OTA
------------

``pre_encrypted_ota`` 示例已移至 `idf-extra-components <https://github.com/espressif/idf-extra-components/tree/master/esp_encrypted_img/examples/pre_encrypted_ota>`__ 仓库。
