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

Signature Verification
----------------------

For additional security, signature of OTA firmware images can be verified. For that, refer :ref:`secure-ota-updates`

API Reference
-------------

.. include:: /_build/inc/esp_https_ota.inc
