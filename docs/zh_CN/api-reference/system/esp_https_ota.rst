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


签名验证
----------------------

要进一步提升安全性，还可以验证 OTA 固件镜像的签名。更多内容请参考 :ref:`secure-ota-updates`。

.. _ota_updates_pre-encrypted-firmware:

使用预加密固件进行 OTA 升级
----------------------------------------

预加密固件完全独立于 :doc:`../../security/flash-encryption` 方案，主要原因如下：

 * flash 加密方案推荐各个设备使用在内部生成的唯一加密密钥，因此在 OTA 更新服务器上预加密固件并不可行。

 * flash 加密方案依赖 flash 偏移，会基于不同的 flash 偏移量生成不同的密文，因此根据分区槽（如 ``ota_0``、``ota_1`` 等）来管理不同的 OTA 更新镜像较为困难。

 * 即使设备未启用 flash 加密，仍可能要求进行 OTA 的固件镜像保持加密。

无论底层传输安全性如何，预加密固件的分发都能确保固件镜像在从服务器到设备的**传输过程中**保持加密状态。首先，预加密软件层在设备上通过网络接收并解密固件，然后使用平台 flash 加密（如果已启用）重新加密内容，最后写入 flash。

设计
^^^^

* 该方案需首先生成一个唯一的 RSA-3072 公钥—私钥对。公钥保留在 OTA 更新服务器上，用于加密，而私钥作为设备的一部分，例如内嵌于固件中，用于解密。
* 预加密固件使用 AES-GCM 密钥进行加密，并将该密钥（及其配置参数）作为标头附加到镜像中。
* 此外，AES-GCM 密钥使用 RSA 公钥进行加密，生成的镜像会托管到 OTA 更新服务器上。
* 在设备端，首先使用可用的 RSA 私钥解密镜像标头，从而获取 AES-GCM 密钥。
* 最后，使用 AES-GCM 密钥（和配置参数）解密镜像内容，并将其写入 flash。

整个工作流程由外部组件 `esp_encrypted_image <https://github.com/espressif/idf-extra-components/blob/master/esp_encrypted_img>`_ 管理，并通过解密回调 (:cpp:member:`esp_https_ota_config_t::decrypt_cb`) 机制插入到 OTA 更新框架中。

.. note::
    该支持方案基于 RSA-3072，必须使用平台安全功能保护设备端的私钥。

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
    - ESP_HTTPS_OTA_DECRYPT_CB                : ``NULL``
    - ESP_HTTPS_OTA_WRITE_FLASH               : ``int``
    - ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION     : ``esp_partition_subtype_t``
    - ESP_HTTPS_OTA_FINISH                    : ``NULL``
    - ESP_HTTPS_OTA_ABORT                     : ``NULL``

应用示例
----------------

- :example:`system/ota/pre_encrypted_ota` 演示了如何使用 `esp_encrypted_img` 组件的 API 和工具进行带预加密二进制文件的 OTA 更新，确保固件在网络通道上的机密性，但不保证其真实性。要进行带预加密固件的 OTA 升级，请在组件 `menuconfig` 中启用 :ref:`CONFIG_ESP_HTTPS_OTA_DECRYPT_CB`。

- :example:`system/ota/advanced_https_ota` 演示了如何在 {IDF_TARGET_NAME} 上使用 `esp_https_ota` 组件的 API 来使用 HTTPS OTA 更新功能。关于该示例适用的芯片，请参考 :example_file:`system/ota/advanced_https_ota/README.md`。

- :example:`system/ota/simple_ota_example` 演示了如何使用 `esp_https_ota` 组件的 API，通过特定的网络接口，如以太网或 Wi-Fi Station，在 {IDF_TARGET_NAME} 上进行固件升级。关于该示例适用的芯片，请参考 :example_file:`system/ota/simple_ota_example/README.md`。

API 参考
-------------

.. include-build-file:: inc/esp_https_ota.inc
