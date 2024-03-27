/* Local Ctrl Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include <stdint.h>
#include <sys/param.h>
#include <string.h>
#include <inttypes.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <mdns.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_local_ctrl.h>
#include <protocomm_ble.h>

static const char *TAG = "control";

#define SERVICE_NAME "my_esp_ctrl_device"

#if CONFIG_EXAMPLE_PROTOCOMM_SECURITY_VERSION_2
#if CONFIG_EXAMPLE_PROTOCOMM_SEC2_DEV_MODE
#define EXAMPLE_PROTOCOMM_SEC2_USERNAME          "wifiprov"
#define EXAMPLE_PROTOCOMM_SEC2_PWD               "abcd1234"

/* This salt,verifier has been generated for username = "localctrl" and password = "abcd1234"
 * IMPORTANT NOTE: For production cases, this must be unique to every device
 * and should come from device manufacturing partition.*/
static const char sec2_salt[] = {
    0x03, 0x6e, 0xe0, 0xc7, 0xbc, 0xb9, 0xed, 0xa8, 0x4c, 0x9e, 0xac, 0x97, 0xd9, 0x3d, 0xec, 0xf4
};

static const char sec2_verifier[] = {
    0x7c, 0x7c, 0x85, 0x47, 0x65, 0x08, 0x94, 0x6d, 0xd6, 0x36, 0xaf, 0x37, 0xd7, 0xe8, 0x91, 0x43,
    0x78, 0xcf, 0xfd, 0x61, 0x6c, 0x59, 0xd2, 0xf8, 0x39, 0x08, 0x12, 0x72, 0x38, 0xde, 0x9e, 0x24,
    0xa4, 0x70, 0x26, 0x1c, 0xdf, 0xa9, 0x03, 0xc2, 0xb2, 0x70, 0xe7, 0xb1, 0x32, 0x24, 0xda, 0x11,
    0x1d, 0x97, 0x18, 0xdc, 0x60, 0x72, 0x08, 0xcc, 0x9a, 0xc9, 0x0c, 0x48, 0x27, 0xe2, 0xae, 0x89,
    0xaa, 0x16, 0x25, 0xb8, 0x04, 0xd2, 0x1a, 0x9b, 0x3a, 0x8f, 0x37, 0xf6, 0xe4, 0x3a, 0x71, 0x2e,
    0xe1, 0x27, 0x86, 0x6e, 0xad, 0xce, 0x28, 0xff, 0x54, 0x46, 0x60, 0x1f, 0xb9, 0x96, 0x87, 0xdc,
    0x57, 0x40, 0xa7, 0xd4, 0x6c, 0xc9, 0x77, 0x54, 0xdc, 0x16, 0x82, 0xf0, 0xed, 0x35, 0x6a, 0xc4,
    0x70, 0xad, 0x3d, 0x90, 0xb5, 0x81, 0x94, 0x70, 0xd7, 0xbc, 0x65, 0xb2, 0xd5, 0x18, 0xe0, 0x2e,
    0xc3, 0xa5, 0xf9, 0x68, 0xdd, 0x64, 0x7b, 0xb8, 0xb7, 0x3c, 0x9c, 0xfc, 0x00, 0xd8, 0x71, 0x7e,
    0xb7, 0x9a, 0x7c, 0xb1, 0xb7, 0xc2, 0xc3, 0x18, 0x34, 0x29, 0x32, 0x43, 0x3e, 0x00, 0x99, 0xe9,
    0x82, 0x94, 0xe3, 0xd8, 0x2a, 0xb0, 0x96, 0x29, 0xb7, 0xdf, 0x0e, 0x5f, 0x08, 0x33, 0x40, 0x76,
    0x52, 0x91, 0x32, 0x00, 0x9f, 0x97, 0x2c, 0x89, 0x6c, 0x39, 0x1e, 0xc8, 0x28, 0x05, 0x44, 0x17,
    0x3f, 0x68, 0x02, 0x8a, 0x9f, 0x44, 0x61, 0xd1, 0xf5, 0xa1, 0x7e, 0x5a, 0x70, 0xd2, 0xc7, 0x23,
    0x81, 0xcb, 0x38, 0x68, 0xe4, 0x2c, 0x20, 0xbc, 0x40, 0x57, 0x76, 0x17, 0xbd, 0x08, 0xb8, 0x96,
    0xbc, 0x26, 0xeb, 0x32, 0x46, 0x69, 0x35, 0x05, 0x8c, 0x15, 0x70, 0xd9, 0x1b, 0xe9, 0xbe, 0xcc,
    0xa9, 0x38, 0xa6, 0x67, 0xf0, 0xad, 0x50, 0x13, 0x19, 0x72, 0x64, 0xbf, 0x52, 0xc2, 0x34, 0xe2,
    0x1b, 0x11, 0x79, 0x74, 0x72, 0xbd, 0x34, 0x5b, 0xb1, 0xe2, 0xfd, 0x66, 0x73, 0xfe, 0x71, 0x64,
    0x74, 0xd0, 0x4e, 0xbc, 0x51, 0x24, 0x19, 0x40, 0x87, 0x0e, 0x92, 0x40, 0xe6, 0x21, 0xe7, 0x2d,
    0x4e, 0x37, 0x76, 0x2f, 0x2e, 0xe2, 0x68, 0xc7, 0x89, 0xe8, 0x32, 0x13, 0x42, 0x06, 0x84, 0x84,
    0x53, 0x4a, 0xb3, 0x0c, 0x1b, 0x4c, 0x8d, 0x1c, 0x51, 0x97, 0x19, 0xab, 0xae, 0x77, 0xff, 0xdb,
    0xec, 0xf0, 0x10, 0x95, 0x34, 0x33, 0x6b, 0xcb, 0x3e, 0x84, 0x0f, 0xb9, 0xd8, 0x5f, 0xb8, 0xa0,
    0xb8, 0x55, 0x53, 0x3e, 0x70, 0xf7, 0x18, 0xf5, 0xce, 0x7b, 0x4e, 0xbf, 0x27, 0xce, 0xce, 0xa8,
    0xb3, 0xbe, 0x40, 0xc5, 0xc5, 0x32, 0x29, 0x3e, 0x71, 0x64, 0x9e, 0xde, 0x8c, 0xf6, 0x75, 0xa1,
    0xe6, 0xf6, 0x53, 0xc8, 0x31, 0xa8, 0x78, 0xde, 0x50, 0x40, 0xf7, 0x62, 0xde, 0x36, 0xb2, 0xba
};
#endif

static esp_err_t example_get_sec2_salt(const char **salt, uint16_t *salt_len) {
#if CONFIG_EXAMPLE_PROTOCOMM_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded salt");
    *salt = sec2_salt;
    *salt_len = sizeof(sec2_salt);
    return ESP_OK;
#elif CONFIG_EXAMPLE_PROTOCOMM_SEC2_PROD_MODE
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}

static esp_err_t example_get_sec2_verifier(const char **verifier, uint16_t *verifier_len) {
#if CONFIG_EXAMPLE_PROTOCOMM_SEC2_DEV_MODE
    ESP_LOGI(TAG, "Development mode: using hard coded verifier");
    *verifier = sec2_verifier;
    *verifier_len = sizeof(sec2_verifier);
    return ESP_OK;
#elif CONFIG_EXAMPLE_PROTOCOMM_SEC2_PROD_MODE
    /* This code needs to be updated with appropriate implementation to provide verifier */
    ESP_LOGE(TAG, "Not implemented!");
    return ESP_FAIL;
#endif
}
#endif

/* Custom allowed property types */
enum property_types {
    PROP_TYPE_TIMESTAMP = 0,
    PROP_TYPE_INT32,
    PROP_TYPE_BOOLEAN,
    PROP_TYPE_STRING,
};

/* Custom flags that can be set for a property */
enum property_flags {
    PROP_FLAG_READONLY = (1 << 0)
};

/********* Handler functions for responding to control requests / commands *********/

static esp_err_t get_property_values(size_t props_count,
                                     const esp_local_ctrl_prop_t props[],
                                     esp_local_ctrl_prop_val_t prop_values[],
                                     void *usr_ctx)
{
    for (uint32_t i = 0; i < props_count; i++) {
        ESP_LOGI(TAG, "Reading property : %s", props[i].name);
        /* For the purpose of this example, to keep things simple
         * we have set the context pointer of each property to
         * point to its value (except for timestamp) */
        switch (props[i].type) {
            case PROP_TYPE_INT32:
            case PROP_TYPE_BOOLEAN:
                /* No need to set size for these types as sizes where
                 * specified when declaring the properties, unlike for
                 * string type. */
                prop_values[i].data = props[i].ctx;
                break;
            case PROP_TYPE_TIMESTAMP: {
                /* Get the time stamp */
                static int64_t ts = 0;
                ts = esp_timer_get_time();

                /* Set the current time. Since this is statically
                 * allocated, we don't need to provide a free_fn */
                prop_values[i].data = &ts;
                break;
            }
            case PROP_TYPE_STRING: {
                char **prop3_value = (char **) props[i].ctx;
                if (*prop3_value == NULL) {
                    prop_values[i].size = 0;
                    prop_values[i].data = NULL;
                } else {
                    /* We could try dynamically allocating the output value,
                     * and it should get freed automatically after use, as
                     * `esp_local_ctrl` internally calls the provided `free_fn` */
                    prop_values[i].size = strlen(*prop3_value);
                    prop_values[i].data = strdup(*prop3_value);
                    if (!prop_values[i].data) {
                        return ESP_ERR_NO_MEM;
                    }
                    prop_values[i].free_fn = free;
                }
            }
            default:
                break;
        }
    }
    return ESP_OK;
}

static esp_err_t set_property_values(size_t props_count,
                                     const esp_local_ctrl_prop_t props[],
                                     const esp_local_ctrl_prop_val_t prop_values[],
                                     void *usr_ctx)
{
    for (uint32_t i = 0; i < props_count; i++) {
        /* Cannot set the value of a read-only property */
        if (props[i].flags & PROP_FLAG_READONLY) {
            ESP_LOGE(TAG, "%s is read-only", props[i].name);
            return ESP_ERR_INVALID_ARG;
        }
        /* For the purpose of this example, to keep things simple
         * we have set the context pointer of each property to
         * point to its value (except for timestamp) */
        switch (props[i].type) {
            case PROP_TYPE_STRING: {
                    /* Free the previously set string */
                    char **prop3_value = (char **) props[i].ctx;
                    free(*prop3_value);
                    *prop3_value = NULL;

                    /* Copy the input string */
                    if (prop_values[i].size) {
                        *prop3_value = strndup((const char *)prop_values[i].data, prop_values[i].size);
                        if (*prop3_value == NULL) {
                            return ESP_ERR_NO_MEM;
                        }
                        ESP_LOGI(TAG, "Setting %s value to %s", props[i].name, (const char*)*prop3_value);
                    }
                }
                break;
            case PROP_TYPE_INT32: {
                    const int32_t *new_value = (const int32_t *) prop_values[i].data;
                    ESP_LOGI(TAG, "Setting %s value to %" PRId32, props[i].name, *new_value);
                    memcpy(props[i].ctx, new_value, sizeof(int32_t));
                }
                break;
            case PROP_TYPE_BOOLEAN: {
                    const bool *value = (const bool *) prop_values[i].data;
                    ESP_LOGI(TAG, "Setting %s value to %d", props[i].name, *value);
                    memcpy(props[i].ctx, value, sizeof(bool));
                }
                break;
            default:
                break;
        }
    }
    return ESP_OK;
}

/******************************************************************************/

/* A custom free_fn to free a pointer to a string as
 * well as the string being pointed to */
static void free_str(void *arg)
{
    char **ptr_to_strptr = (char **)arg;
    if (ptr_to_strptr) {
        free(*ptr_to_strptr);
        free(ptr_to_strptr);
    }
}

/* Function used by app_main to start the esp_local_ctrl service */
void start_esp_local_ctrl_service(void)
{
#ifdef CONFIG_EXAMPLE_LOCAL_CTRL_TRANSPORT_HTTP
#ifdef CONFIG_ESP_HTTPS_SERVER_ENABLE
    /* Set the configuration */
    httpd_ssl_config_t https_conf = HTTPD_SSL_CONFIG_DEFAULT();

    /* Load server certificate */
    extern const unsigned char servercert_start[] asm("_binary_servercert_pem_start");
    extern const unsigned char servercert_end[]   asm("_binary_servercert_pem_end");
    https_conf.servercert = servercert_start;
    https_conf.servercert_len = servercert_end - servercert_start;

    /* Load server private key */
    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    https_conf.prvtkey_pem = prvtkey_pem_start;
    https_conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
#else
    httpd_config_t http_conf = HTTPD_DEFAULT_CONFIG();
#endif
#else /* CONFIG_EXAMPLE_LOCAL_CTRL_TRANSPORT_BLE */
    protocomm_ble_config_t *ble_conf = & (protocomm_ble_config_t) {
        .device_name  = SERVICE_NAME,
        /* Set a random 128 bit UUID which will be included in the BLE advertisement
         * and will correspond to the primary GATT service that provides provisioning
         * endpoints as GATT characteristics. Each GATT characteristic will be
         * formed using the primary service UUID as base, with different auto assigned
         * 12th and 13th bytes (assume counting starts from 0th byte). The client side
         * applications must identify the endpoints by reading the User Characteristic
         * Description descriptor (0x2901) for each characteristic, which contains the
         * endpoint name of the characteristic */
        .service_uuid = {
            /* LSB <---------------------------------------
            * ---------------------------------------> MSB */
            0x21, 0xd5, 0x3b, 0x8d, 0xbd, 0x75, 0x68, 0x8a,
            0xb4, 0x42, 0xeb, 0x31, 0x4a, 0x1e, 0x98, 0x3d,
        }
    };
#endif /* CONFIG_EXAMPLE_LOCAL_CTRL_TRANSPORT_HTTP */

#ifdef CONFIG_EXAMPLE_PROTOCOMM_SECURITY_VERSION_1
    /* What is the security level that we want (0, 1, 2):
     *      - PROTOCOMM_SECURITY_0 is simply plain text communication.
     *      - PROTOCOMM_SECURITY_1 is secure communication which consists of secure handshake
     *          using X25519 key exchange and proof of possession (pop) and AES-CTR
     *          for encryption/decryption of messages.
     *      - PROTOCOMM_SECURITY_2 SRP6a based authentication and key exchange
     *        + AES-GCM encryption/decryption of messages
     */
    esp_local_ctrl_proto_sec_t security = PROTOCOM_SEC1;

    /* Do we want a proof-of-possession (ignored if Security 0 is selected):
     *      - this should be a string with length > 0
     *      - NULL if not used
     */
    const char *pop = "abcd1234";

    /* This is the structure for passing security parameters
     * for the protocomm security 1.
     */
    protocomm_security1_params_t sec_params = {
        .data = (const uint8_t *)pop,
        .len = strlen(pop),
    };

#elif CONFIG_EXAMPLE_PROTOCOMM_SECURITY_VERSION_2
    esp_local_ctrl_proto_sec_t security = PROTOCOM_SEC2;

    /* This is the structure for passing security parameters
     * for the protocomm security 2.
     */
    protocomm_security2_params_t sec_params = {};

    ESP_ERROR_CHECK(example_get_sec2_salt(&sec_params.salt, &sec_params.salt_len));
    ESP_ERROR_CHECK(example_get_sec2_verifier(&sec_params.verifier, &sec_params.verifier_len));

#else /* CONFIG_EXAMPLE_PROTOCOMM_SECURITY_VERSION_0 */
    esp_local_ctrl_proto_sec_t security = PROTOCOM_SEC0;
    const void *sec_params = NULL;

#endif
    esp_local_ctrl_config_t config = {
#ifdef CONFIG_EXAMPLE_LOCAL_CTRL_TRANSPORT_HTTP
        .transport = ESP_LOCAL_CTRL_TRANSPORT_HTTPD,
        .transport_config = {
#ifdef CONFIG_ESP_HTTPS_SERVER_ENABLE
            .httpd = &https_conf,
#else
            .httpd = &http_conf,
#endif
        },
#else   /* CONFIG_EXAMPLE_LOCAL_CTRL_TRANSPORT_BLE */
        .transport = ESP_LOCAL_CTRL_TRANSPORT_BLE,
        .transport_config = {
            .ble = ble_conf,
        },
#endif /* CONFIG_EXAMPLE_LOCAL_CTRL_TRANSPORT_HTTP */
        .proto_sec = {
            .version = security,
            .custom_handle = NULL,
            .sec_params = &sec_params,
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

#ifdef CONFIG_EXAMPLE_LOCAL_CTRL_TRANSPORT_HTTP
    mdns_init();
    mdns_hostname_set(SERVICE_NAME);
#endif

    /* Start esp_local_ctrl service */
    ESP_ERROR_CHECK(esp_local_ctrl_start(&config));
    ESP_LOGI(TAG, "esp_local_ctrl service started with name : %s", SERVICE_NAME);

    /* Create a timestamp property. The client should see this as a read-only property.
     * Property value is fetched using `esp_timer_get_time()` in the `get_prop_values`
     * handler */
    esp_local_ctrl_prop_t timestamp = {
        .name        = "timestamp (us)",
        .type        = PROP_TYPE_TIMESTAMP,
        .size        = sizeof(int64_t),
        .flags       = PROP_FLAG_READONLY,
        .ctx         = NULL,
        .ctx_free_fn = NULL
    };

    /* Create a writable integer property. Use dynamically allocated memory
     * for storing its value and pass it as context, so that it can be accessed
     * inside the set / get handlers. */
    int32_t *prop1_value = malloc(sizeof(int32_t));
    assert(prop1_value != NULL);

    /* Initialize the property value */
    *prop1_value = 123456789;

    /* Populate the property structure accordingly. Since, we would want the memory
     * occupied by the property value to be freed automatically upon call to
     * `esp_local_ctrl_stop()` or `esp_local_ctrl_remove_property()`, the `ctx_free_fn`
     * field will need to be set with the appropriate de-allocation function,
     * which in this case is simply `free()` */
    esp_local_ctrl_prop_t property1 = {
        .name        = "property1",
        .type        = PROP_TYPE_INT32,
        .size        = sizeof(int32_t),
        .flags       = 0,
        .ctx         = prop1_value,
        .ctx_free_fn = free
    };

    /* Create another read-only property. Just for demonstration, we use statically
     * allocated value. No `ctx_free_fn` needs to be set for this */
    static bool prop2_value = false;

    esp_local_ctrl_prop_t property2 = {
        .name        = "property2",
        .type        = PROP_TYPE_BOOLEAN,
        .size        = sizeof(bool),
        .flags       = PROP_FLAG_READONLY,
        .ctx         = &prop2_value,
        .ctx_free_fn = NULL
    };

    /* Create a variable sized property. Its context is a pointer for storing the
     * pointer to a dynamically allocate string, therefore it will require a
     * customized free function `free_str()` */
    char **prop3_value = calloc(1, sizeof(char *));
    assert(prop3_value != NULL);

    esp_local_ctrl_prop_t property3 = {
        .name        = "property3",
        .type        = PROP_TYPE_STRING,
        .size        = 0, // When zero, this is assumed to be of variable size
        .flags       = 0,
        .ctx         = prop3_value,
        .ctx_free_fn = free_str
    };

    /* Now register the properties */
    ESP_ERROR_CHECK(esp_local_ctrl_add_property(&timestamp));
    ESP_ERROR_CHECK(esp_local_ctrl_add_property(&property1));
    ESP_ERROR_CHECK(esp_local_ctrl_add_property(&property2));
    ESP_ERROR_CHECK(esp_local_ctrl_add_property(&property3));

    /* Just for fun, let us keep toggling the value
     * of the boolean property2, every 1 second */
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        prop2_value = !prop2_value;
    }
}
