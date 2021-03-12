ESP HTTPS OTA
=============

Overview
--------

``esp_https_ota`` provides simplified APIs to perform firmware upgrades over HTTPS.
It's an abstraction layer over existing OTA APIs.

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
            esp_err_t ret = esp_https_ota(&config);
            if (ret == ESP_OK) {
                esp_restart();
            } else {
                return ESP_FAIL;
            }
            return ESP_OK;
        }

Partial Image Download over HTTPS
---------------------------------

To use partial image download feature, enable ``partial_http_download`` configuration in ``esp_https_ota_config_t``.
When this configuration is enabled, firmware image will be downloaded in multiple HTTP requests of specified size.
Maximum content length of each request can be specified by setting ``max_http_request_size`` to required value.

This option is useful while fetching image from a service like AWS S3, where mbedTLS Rx buffer size (:ref:`CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN`)
can be set to lower value which is not possible without enabling this configuration.

Default value of mbedTLS Rx buffer size is set to 16K. By using partial_http_download with max_http_request_size of 4K,
size of mbedTLS Rx buffer can be reduced to 4K. With this confiuration, memory saving of around 12K is expected.

Signature Verification
----------------------

For additional security, signature of OTA firmware images can be verified. For that, refer :ref:`secure-ota-updates`

API Reference
-------------

.. include-build-file:: inc/esp_https_ota.inc
