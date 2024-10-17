协议
====

:link_to_translation:`en:[English]`

ESP HTTPS OTA
-------------

重大变更（摘要）
~~~~~~~~~~~~~~~~

- 若 HTTP 标头中包含了镜像大小，并且 :cpp:member:`esp_https_ota_config_t::bulk_flash_erase` 已设置为 true，那么 flash 不会被整个擦除，而是将根据镜像大小进行擦除操作。
