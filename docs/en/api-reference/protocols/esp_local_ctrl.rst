ESP Local Control
=================

Overview
--------
ESP Local Control (**esp_local_ctrl**) component in  ESP-IDF provides capability to control an ESP device over Wi-Fi + HTTPS or BLE. It provides access to application defined **properties** that are available for reading / writing via a set of configurable handlers.

Initialization of the **esp_local_ctrl** service over BLE transport is performed as follows:

    .. highlight:: c

    ::

        esp_local_ctrl_config_t config = {
            .transport = ESP_LOCAL_CTRL_TRANSPORT_BLE,
            .transport_config = {
                .ble = & (protocomm_ble_config_t) {
                    .device_name  = SERVICE_NAME,
                    .service_uuid = {
                        /* LSB <---------------------------------------
                        * ---------------------------------------> MSB */
                        0x21, 0xd5, 0x3b, 0x8d, 0xbd, 0x75, 0x68, 0x8a,
                        0xb4, 0x42, 0xeb, 0x31, 0x4a, 0x1e, 0x98, 0x3d
                    }
                }
            },
            .handlers = {
                /* User defined handler functions */
                .get_prop_values = get_property_values,
                .set_prop_values = set_property_values,
                .usr_ctx         = NULL,
                .usr_ctx_free_fn = NULL
            },
            /* Maximum number of properties that may be set */
            .max_properties = 10
        };

        /* Start esp_local_ctrl service */
        ESP_ERROR_CHECK(esp_local_ctrl_start(&config));


Similarly for HTTPS transport:

    .. highlight:: c

    ::

        /* Set the configuration */
        httpd_ssl_config_t https_conf = HTTPD_SSL_CONFIG_DEFAULT();

        /* Load server certificate */
        extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
        extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");
        https_conf.cacert_pem = cacert_pem_start;
        https_conf.cacert_len = cacert_pem_end - cacert_pem_start;

        /* Load server private key */
        extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
        extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
        https_conf.prvtkey_pem = prvtkey_pem_start;
        https_conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

        esp_local_ctrl_config_t config = {
            .transport = ESP_LOCAL_CTRL_TRANSPORT_HTTPD,
            .transport_config = {
                .httpd = &https_conf
            },
            .handlers = {
                /* User defined handler functions */
                .get_prop_values = get_property_values,
                .set_prop_values = set_property_values,
                .usr_ctx         = NULL,
                .usr_ctx_free_fn = NULL
            },
            /* Maximum number of properties that may be set */
            .max_properties = 10
        };

        /* Start esp_local_ctrl service */
        ESP_ERROR_CHECK(esp_local_ctrl_start(&config));


Creating a property
-------------------

Now that we know how to start the **esp_local_ctrl** service, let's add a property to it. Each property must have a unique `name` (string), a `type` (e.g. enum), `flags` (bit fields) and `size`.

The `size` is to be kept 0, if we want our property value to be of variable length (e.g. if its a string or bytestream). For fixed length property value data-types, like int, float, etc., setting the `size` field to the right value, helps **esp_local_ctrl** to perform internal checks on arguments received with write requests.

The interpretation of `type` and `flags` fields is totally upto the application, hence they may be used as enumerations, bitfields, or even simple integers. One way is to use `type` values to classify properties, while `flags` to specify characteristics of a property.

Here is an example property which is to function as a timestamp. It is assumed that the application defines `TYPE_TIMESTAMP` and `READONLY`, which are used for setting the `type` and `flags` fields here.

    .. highlight:: c

    ::

        /* Create a timestamp property */
        esp_local_ctrl_prop_t timestamp = {
            .name        = "timestamp",
            .type        = TYPE_TIMESTAMP,
            .size        = sizeof(int32_t),
            .flags       = READONLY,
            .ctx         = func_get_time,
            .ctx_free_fn = NULL
        };

        /* Now register the property */
        esp_local_ctrl_add_property(&timestamp);


Also notice that there is a ctx field, which is set to point to some custom `func_get_time()`. This can be used inside the property get / set handlers to retrieve timestamp.

Here is an example of `get_prop_values()` handler, which is used for retrieving the timestamp.

    .. highlight:: c

    ::

        static esp_err_t get_property_values(size_t props_count,
                                             const esp_local_ctrl_prop_t *props,
                                             esp_local_ctrl_prop_val_t *prop_values,
                                             void *usr_ctx)
        {
            for (uint32_t i = 0; i < props_count; i++) {
                ESP_LOGI(TAG, "Reading %s", props[i].name);
                if (props[i].type == TYPE_TIMESTAMP) {
                    /* Obtain the timer function from ctx */
                    int32_t (*func_get_time)(void) = props[i].ctx;

                    /* Use static variable for saving the value.
                     * This is essential because the value has to be
                     * valid even after this function returns.
                     * Alternative is to use dynamic allocation
                     * and set the free_fn field */
                    static int32_t ts = func_get_time();
                    prop_values[i].data = &ts;
                }
            }
            return ESP_OK;
        }


Here is an example of `set_prop_values()` handler. Notice how we restrict from writing to read-only properties.

    .. highlight:: c

    ::

        static esp_err_t set_property_values(size_t props_count,
                                             const esp_local_ctrl_prop_t *props,
                                             const esp_local_ctrl_prop_val_t *prop_values,
                                             void *usr_ctx)
        {
            for (uint32_t i = 0; i < props_count; i++) {
                if (props[i].flags & READONLY) {
                    ESP_LOGE(TAG, "Cannot write to read-only property %s", props[i].name);
                    return ESP_ERR_INVALID_ARG;
                } else {
                    ESP_LOGI(TAG, "Setting %s", props[i].name);

                    /* For keeping it simple, lets only log the incoming data */
                    ESP_LOG_BUFFER_HEX_LEVEL(TAG, prop_values[i].data,
                                             prop_values[i].size, ESP_LOG_INFO);
                }
            }
            return ESP_OK;
        }


For complete example see :example:`protocols/esp_local_ctrl`

Client Side Implementation
--------------------------

The client side implementation will have establish a protocomm session with the device first, over the supported mode of transport, and then send and receive protobuf messages understood by the **esp_local_ctrl** service. The service will translate these messages into requests and then call the appropriate handlers (set / get). Then, the generated response for each handler is again packed into a protobuf message and transmitted back to the client.

See below the various protobuf messages understood by the **esp_local_ctrl** service:

1. `get_prop_count` : This should simply return the total number of properties supported by the service
2. `get_prop_values` : This accepts an array of indices and should return the information (name, type, flags) and values of the properties corresponding to those indices
3. `set_prop_values` : This accepts an array of indices and an array of new values, which are used for setting the values of the properties corresponding to the indices

Note that indices may or may not be the same for a property, across multiple sessions. Therefore, the client must only use the names of the properties to uniquely identify them. So, every time a new session is established, the client should first call `get_prop_count` and then `get_prop_values`, hence form an index to name mapping for all properties. Now when calling `set_prop_values` for a set of properties, it must first convert the names to indexes, using the created mapping. As emphasized earlier, the client must refresh the index to name mapping every time a new session is established with the same device.

The various protocomm endpoints provided by **esp_local_ctrl** are listed below:

.. list-table:: Endpoints provided by ESP Local Control
   :widths: 10 25 50
   :header-rows: 1

   * - Endpoint Name (BLE + GATT Server)
     - URI (HTTPS Server + mDNS)
     - Description
   * - esp_local_ctrl/version
     - https://<mdns-hostname>.local/esp_local_ctrl/version
     - Endpoint used for retrieving version string
   * - esp_local_ctrl/control
     - https://<mdns-hostname>.local/esp_local_ctrl/control
     - Endpoint used for sending / receiving control messages


API Reference
-------------

.. include:: /_build/inc/esp_local_ctrl.inc
