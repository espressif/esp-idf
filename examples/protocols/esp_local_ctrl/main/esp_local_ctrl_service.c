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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <mdns.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_local_ctrl.h>
#include <esp_https_server.h>

static const char *TAG = "control";

#define SERVICE_NAME "my_esp_ctrl_device"

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
                    ESP_LOGI(TAG, "Setting %s value to %d", props[i].name, *new_value);
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
        .proto_sec = {
            .version = 0,
            .custom_handle = NULL,
            .pop = NULL,
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

    mdns_init();
    mdns_hostname_set(SERVICE_NAME);

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
        vTaskDelay(1000 / portTICK_RATE_MS);
        prop2_value = !prop2_value;
    }
}
