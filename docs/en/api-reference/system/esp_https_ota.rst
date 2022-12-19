ESP HTTPS OTA
=============

Overview
--------

``esp_https_ota`` provides simplified APIs to perform firmware upgrades over HTTPS. It's an abstraction layer over existing OTA APIs.

Application Example
-------------------

    .. highlight:: c

    ::

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


Server Verification
-------------------

Please refer to :ref:`ESP-TLS: TLS Server Verification <esp_tls_server_verification>` for more information on server verification. The root certificate (in PEM format) needs to be provided to the :cpp:member:`esp_http_client_config_t::cert_pem` member.

.. note:: The server-endpoint **root** certificate should be used for verification instead of any intermediate ones from the certificate chain. The reason being that the root certificate has the maximum validity and usually remains the same for a long period of time. Users can also use the ``ESP x509 Certificate Bundle`` feature for verification, which covers most of the trusted root certificates (using the :cpp:member:`esp_http_client_config_t::crt_bundle_attach` member).

Partial Image Download over HTTPS
---------------------------------

To use partial image download feature, enable ``partial_http_download`` configuration in ``esp_https_ota_config_t``. When this configuration is enabled, firmware image will be downloaded in multiple HTTP requests of specified size. Maximum content length of each request can be specified by setting ``max_http_request_size`` to required value.

This option is useful while fetching image from a service like AWS S3, where mbedTLS Rx buffer size (:ref:`CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN`) can be set to lower value which is not possible without enabling this configuration.

Default value of mbedTLS Rx buffer size is set to 16K. By using partial_http_download with max_http_request_size of 4K, size of mbedTLS Rx buffer can be reduced to 4K. With this configuration, memory saving of around 12K is expected.

Signature Verification
----------------------

For additional security, signature of OTA firmware images can be verified. For that, refer :ref:`secure-ota-updates`

Advanced APIs
-------------

``esp_https_ota`` also provides advanced APIs which can be used if more information and control is needed during the OTA process.

Example that uses advanced ESP_HTTPS_OTA APIs: :example:`system/ota/advanced_https_ota`.


.. _ota_updates_pre-encrypted-firmware:

OTA Upgrades with Pre-Encrypted Firmware
----------------------------------------

To perform OTA upgrades with Pre-Encrypted Firmware, please enable :ref:`CONFIG_ESP_HTTPS_OTA_DECRYPT_CB` in component menuconfig.

Example that performs OTA upgrade with Pre-Encrypted Firmware: :example:`system/ota/pre_encrypted_ota`.

OTA System Events
-----------------

ESP HTTPS OTA has various events for which a handler can be triggered by :doc:`the Event Loop library <../system/esp_event>` when the particular event occurs. The handler has to be registered using :cpp:func:`esp_event_handler_register`. This helps in event handling for ESP HTTPS OTA.
:cpp:enum:`esp_https_ota_event_t` has all the events which can happen when performing OTA upgrade using ESP HTTPS OTA.

Event Handler Example
^^^^^^^^^^^^^^^^^^^^^

    .. highlight:: c

    ::

        /* Event handler for catching system events */
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

Expected data type for different ESP HTTPS OTA events in the system event loop:
    - ESP_HTTPS_OTA_START                     : ``NULL``
    - ESP_HTTPS_OTA_CONNECTED                 : ``NULL``
    - ESP_HTTPS_OTA_GET_IMG_DESC              : ``NULL``
    - ESP_HTTPS_OTA_VERIFY_CHIP_ID            : ``esp_chip_id_t``
    - ESP_HTTPS_OTA_DECRYPT_CB                : ``NULL``
    - ESP_HTTPS_OTA_WRITE_FLASH               : ``int``
    - ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION     : ``esp_partition_subtype_t``
    - ESP_HTTPS_OTA_FINISH                    : ``NULL``
    - ESP_HTTPS_OTA_ABORT                     : ``NULL``

API Reference
-------------

.. include-build-file:: inc/esp_https_ota.inc
