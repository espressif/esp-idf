Wi-Fi Provisioning
==================

Overview
--------

This component provides protocomm endpoint handler - `wifi_prov_config_data_handler` - and related protobuf framework which can be used for Wi-Fi configuration in the context of device provisioning, though it may be used in non-provisioning cases as well.

The configuration consists of three commands :
    * `get_status` - For querying the Wi-Fi connection status
    * `set_config` - For setting the Wi-Fi connection credentials
    * `apply_config` - For applying the credentials saved during `set_config` and (re)start the Wi-Fi station

The way this is supposed to work is that the desired Wi-Fi configuration for the ESP32, which is to run as a station and thus connect to an AP with certain credentials, is to be sent during `set_config`. Then `apply_config` is supposed to start (or restart) the Wi-Fi in station mode with the previously set AP credentials. Afterwords, `get_config` command is used to probe the device continuously for Wi-Fi connection status, to ensure that the connection was indeed successful. If the connection failed, then appropriate status code along with disconnection reason, is to be conveyed through `get_config`.

Application Example
-------------------

    .. highlight:: c

    ::

        esp_err_t get_status_handler(wifi_prov_config_get_data_t *resp_data, wifi_prov_ctx_t **ctx)
        {
            /* Fill the wifi_prov_config_get_data_t structure
             * with Wi-Fi station connection status information. */

            return ESP_OK;
        }

        esp_err_t set_config_handler(const wifi_prov_config_set_data_t *req_data, wifi_prov_ctx_t **ctx)
        {
            /* Copy contents of req_data->ssid and req_data->password
             * which are Wi-Fi AP credentials to which the device will connect */

             return ESP_OK;
        }

        esp_err_t apply_config_handler(wifi_prov_ctx_t **ctx)
        {
            /* Apply the Wi-Fi STA credentials saved during set_config */

            return ESP_OK;
        }

        /* Structure with various config command handlers to be passed
         * as private data during endpoint registration with protocomm */
        wifi_prov_config_handlers_t wifi_prov_handlers = {
            .get_status_handler   = get_status_handler,
            .set_config_handler   = set_config_handler,
            .apply_config_handler = apply_config_handler,
            .ctx = NULL
        };

        /* Set the endpoint handler */
        protocomm_add_endpoint(pc, "wifi_config_endpoint",
                               wifi_prov_config_data_handler,
                               (void *) &wifi_prov_handlers);

API Reference
-------------

.. include:: /_build/inc/wifi_config.inc
