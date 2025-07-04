ESP HTTPS OTA 升级
====================

:link_to_translation:`en:[English]`

概述
--------

``esp_https_ota`` 是现有 OTA（空中升级）API 的抽象层，其中提供了简化的 API，能够通过 HTTPS 升级固件。

    .. code-block:: c

        esp_err_t do_firmware_upgrade()
        {
            esp_http_client_config_t config = {
                .url = CONFIG_FIRMWARE_UPGRADE_URL,
                .cert_pem = (char *)server_cert_pem_start,
            };
            esp_https_ota_config_t ota_config = {
                .http_config = &config,
            };
            esp_err_t ret = esp_https_ota(&ota_config);
            if (ret == ESP_OK) {
                esp_restart();
            } else {
                return ESP_FAIL;
            }
            return ESP_OK;
        }


服务器验证
-------------------

验证服务器时，应将 PEM 格式的根证书提供给 :cpp:member:`esp_http_client_config_t::cert_pem` 成员。如需了解有关服务器验证的更多信息，请参阅 :ref:`esp_tls_server_verification`。

.. note::

    应使用服务器端点的 **根** 证书应用于验证，而不能使用证书链中的任何中间证书，因为根证书有效期最长，且通常长时间维持不变。用户还可以通过 :cpp:member:`esp_http_client_config_t::crt_bundle_attach` 成员使用 ``ESP x509 证书包`` 功能进行验证，其中涵盖了大多数受信任的根证书。

通过 HTTPS 下载部分镜像
---------------------------------

要使用部分镜像下载功能，请启用 ``esp_https_ota_config_t`` 中的 ``partial_http_download`` 配置。启用此配置后，固件镜像将通过多个指定大小的 HTTP 请求进行下载。将 ``max_http_request_size`` 设置为所需值，即可指定每个请求的最大内容长度。

在从 AWS S3 等服务获取镜像时，这一选项非常有用。在启用该选项时，可以将 mbedTLS Rx 的 buffer 大小（即 :ref:`CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN`）设置为较小的值。不启用此配置时，无法将其设置为较小值。

mbedTLS Rx buffer 的默认大小为 16 KB，但如果将 ``partial_http_download`` 的 ``max_http_request_size`` 设置为 4 KB，便能将 mbedTLS Rx 的 buffer 减小到 4 KB。使用这一配置方式预计可以节省约 12 KB 内存。

.. note::
    如果服务器使用分块传输编码，则无法进行部分下载，因为无法预先获知总内容长度。

OTA 恢复
--------

在 :cpp:struct:`esp_https_ota_config_t` 中启用 ``ota_resumption`` 配置，即可使用 OTA 恢复功能。启用此功能后，先前失败的 OTA 镜像下载便可以从中断处继续，无需重新开始整个 OTA 过程。此功能是基于 HTTP 的部分范围请求功能实现的。

要指定镜像下载的续传位置，需要在 :cpp:struct:`esp_https_ota_config_t` 中设置 ``ota_image_bytes_written`` 字段。此字段的值表示在上一次尝试过程中已写入到 OTA 分区的字节数。

如需了解更多，请参阅示例：:example:`system/ota/advanced_https_ota`，该示例演示了 OTA 恢复功能。在此示例中， OTA 的中断状态保存在 NVS 中，从而使 OTA 过程能够从上次保存的状态中无缝恢复，并继续下载。

签名验证
-----------------

要进一步提升安全性，还可以验证 OTA 固件镜像的签名。更多内容请参考 :ref:`secure-ota-updates`。

.. _ota_updates_pre-encrypted-firmware:

使用预加密固件进行 OTA 升级
----------------------------------------

预加密固件完全独立于 :doc:`../../security/flash-encryption` 方案，主要原因如下：

 * flash 加密方案推荐各个设备使用在内部生成的唯一加密密钥，因此在 OTA 更新服务器上预加密固件并不可行。

 * flash 加密方案依赖 flash 偏移，会基于不同的 flash 偏移量生成不同的密文，因此根据分区槽（如 ``ota_0``、``ota_1`` 等）来管理不同的 OTA 更新镜像较为困难。

 * 即使设备未启用 flash 加密，仍可能要求进行 OTA 的固件镜像保持加密。

无论底层传输安全性如何，预加密固件的分发都能确保固件镜像在从服务器到设备的 **传输过程中** 保持加密状态。首先，预加密软件层在设备上通过网络接收并解密固件，然后使用平台 flash 加密（如果已启用）重新加密内容，最后写入 flash。

设计
^^^^

预加密固件是一种 **传输安全方案**，用于确保固件镜像在从 OTA 服务器传输到设备的过程中始终处于加密状态（与底层传输安全无关）。这种方案与 :doc:`../../security/flash-encryption` 在多个关键方面有所不同：

* **密钥管理**：使用外部管理的加密密钥，而不是每个设备内部生成的唯一密钥
* **独立于 flash 偏移**：无论固件烧录在哪个 flash 分区（``ota_0``、``ota_1`` 等），生成的密文内容一致
* **传输保护**：在固件传输过程中提供加密保护，不涉及设备本地存储安全

**重要安全提示**：预加密固件本身不提供设备级安全保护。固件被接收后在设备上解密，并按设备的 flash 加密配置存储。如需设备级安全措施，需另外启用 flash 加密功能。

该功能由 `esp_encrypted_img <https://github.com/espressif/idf-extra-components/tree/master/esp_encrypted_img>`_ 组件实现，该组件通过解密回调 (:cpp:member:`esp_https_ota_config_t::decrypt_cb`) 机制集成在 OTA 更新框架中。

有关镜像格式、密钥生成及实现细节的详细信息，请参阅 `esp_encrypted_img 组件文档 <https://github.com/espressif/idf-extra-components/tree/master/esp_encrypted_img>`_。

OTA 系统事件
-----------------

ESP HTTPS OTA 过程中可能发生各种系统事件。当特定事件发生时，会由 :doc:`../system/esp_event` 触发处理程序。此处理程序必须使用 :cpp:func:`esp_event_handler_register` 注册。这有助于 ESP HTTPS OTA 进行事件处理。

:cpp:enum:`esp_https_ota_event_t` 中包含了使用 ESP HTTPS OTA 升级时可能发生的所有事件。

事件处理程序示例
^^^^^^^^^^^^^^^^^^^^^

    .. code-block:: c

        /* 用于捕获系统事件的事件处理程序 */
        static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
        {
            if (event_base == ESP_HTTPS_OTA_EVENT) {
                switch (event_id) {
                    case ESP_HTTPS_OTA_START:
                        ESP_LOGI(TAG, "OTA started");
                        break;
                    case ESP_HTTPS_OTA_CONNECTED:
                        ESP_LOGI(TAG, "Connected to server");
                        break;
                    case ESP_HTTPS_OTA_GET_IMG_DESC:
                        ESP_LOGI(TAG, "Reading Image Description");
                        break;
                    case ESP_HTTPS_OTA_VERIFY_CHIP_ID:
                        ESP_LOGI(TAG, "Verifying chip id of new image: %d", *(esp_chip_id_t *)event_data);
                        break;
                    case ESP_HTTPS_OTA_VERIFY_CHIP_REVISION:
                        ESP_LOGI(TAG, "Verifying chip revision of new image: %d", *(uint16_t *)event_data);
                        break;
                    case ESP_HTTPS_OTA_DECRYPT_CB:
                        ESP_LOGI(TAG, "Callback to decrypt function");
                        break;
                    case ESP_HTTPS_OTA_WRITE_FLASH:
                        ESP_LOGD(TAG, "Writing to flash: %d written", *(int *)event_data);
                        break;
                    case ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION:
                        ESP_LOGI(TAG, "Boot partition updated. Next Partition: %d", *(esp_partition_subtype_t *)event_data);
                        break;
                    case ESP_HTTPS_OTA_FINISH:
                        ESP_LOGI(TAG, "OTA finish");
                        break;
                    case ESP_HTTPS_OTA_ABORT:
                        ESP_LOGI(TAG, "OTA abort");
                        break;
                }
            }
        }

系统事件循环中，不同 ESP HTTPS OTA 事件的预期数据类型如下所示：

    - ESP_HTTPS_OTA_START                     : ``NULL``
    - ESP_HTTPS_OTA_CONNECTED                 : ``NULL``
    - ESP_HTTPS_OTA_GET_IMG_DESC              : ``NULL``
    - ESP_HTTPS_OTA_VERIFY_CHIP_ID            : ``esp_chip_id_t``
    - ESP_HTTPS_OTA_VERIFY_CHIP_REVISION      : ``uint16_t``
    - ESP_HTTPS_OTA_DECRYPT_CB                : ``NULL``
    - ESP_HTTPS_OTA_WRITE_FLASH               : ``int``
    - ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION     : ``esp_partition_subtype_t``
    - ESP_HTTPS_OTA_FINISH                    : ``NULL``
    - ESP_HTTPS_OTA_ABORT                     : ``NULL``

应用示例
----------------

- :example:`system/ota/advanced_https_ota` 演示了如何在 {IDF_TARGET_NAME} 上使用 `esp_https_ota` 组件的 API 来使用 HTTPS OTA 更新功能。关于该示例适用的芯片，请参考 :example_file:`system/ota/advanced_https_ota/README.md`。

- :example:`system/ota/partitions_ota` 演示了如何使用 `esp_https_ota` 组件的 API 对多个分区（应用、引导加载程序、分区表、存储）进行 OTA 更新。

- :example:`system/ota/simple_ota_example` 演示了如何使用 `esp_https_ota` 组件的 API，通过特定的网络接口，如以太网或 Wi-Fi Station，在 {IDF_TARGET_NAME} 上进行固件升级。关于该示例适用的芯片，请参考 :example_file:`system/ota/simple_ota_example/README.md`。

API 参考
-------------

.. include-build-file:: inc/esp_https_ota.inc
