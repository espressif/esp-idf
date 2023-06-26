/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <sdkconfig.h>
#ifdef CONFIG_ESP_HTTPS_SERVER_ENABLE
#include <esp_https_server.h>
#else
#include <esp_http_server.h>
#endif
#include <protocomm.h>

/**
 * @brief   Property description data structure, which is to be populated
 *          and passed to the `esp_local_ctrl_add_property()` function
 *
 * Once a property is added, its structure is available for read-only access
 * inside `get_prop_values()` and `set_prop_values()` handlers.
 */
typedef struct esp_local_ctrl_prop {
    /**
     * Unique name of property
     */
    char *name;

    /**
     * Type of property. This may be set to application defined enums
     */
    uint32_t type;

    /**
     * Size of the property value, which:
     * - if zero, the property can have values of variable size
     * - if non-zero, the property can have values of fixed size only,
     *   therefore, checks are performed internally by esp_local_ctrl
     *   when setting the value of such a property
     */
    size_t size;

    /**
     * Flags set for this property. This could be a bit field.
     * A flag may indicate property behavior, e.g. read-only / constant
     */
    uint32_t flags;

    /**
     * Pointer to some context data relevant for this property. This will
     * be available for use inside the `get_prop_values` and `set_prop_values`
     * handlers as a part of this property structure. When set, this is valid
     * throughout the lifetime of a property, till either the property is
     * removed or the esp_local_ctrl service is stopped.
     */
    void *ctx;

    /**
     * Function used by esp_local_ctrl to internally free the property
     * context when `esp_local_ctrl_remove_property()` or
     * `esp_local_ctrl_stop()` is called.
     */
    void (*ctx_free_fn)(void *ctx);
} esp_local_ctrl_prop_t;

/**
 * @brief   Property value data structure. This gets passed to the
 *          `get_prop_values()` and `set_prop_values()` handlers for
 *          the purpose of retrieving or setting the present value
 *          of a property.
 */
typedef struct esp_local_ctrl_prop_val {
    /**
     * Pointer to memory holding property value
     */
    void *data;

    /**
     * Size of property value
     */
    size_t size;

    /**
     * This may be set by the application in `get_prop_values()` handler
     * to tell `esp_local_ctrl` to call this function on the data pointer
     * above, for freeing its resources after sending the `get_prop_values`
     * response.
     */
    void (*free_fn)(void *data);
} esp_local_ctrl_prop_val_t;

/**
 * @brief   Handlers for receiving and responding to local
 *          control commands for getting and setting properties.
 */
typedef struct esp_local_ctrl_handlers {
    /**
     * @brief Handler function to be implemented for retrieving current
     *        values of properties
     *
     * @note  If any of the properties have fixed sizes, the size field of
     * corresponding element in `prop_values` need to be set
     *
     * @param[in]  props_count  Total elements in the props array
     * @param[in]  props        Array of properties, the current values for which
     *                          have been requested by the client
     * @param[out] prop_values  Array of empty property values, the elements of
     *                          which need to be populated with the current values
     *                          of those properties specified by props argument
     * @param[in]  usr_ctx      This provides value of the `usr_ctx` field of
     *                          `esp_local_ctrl_handlers_t` structure
     *
     * @return Returning different error codes will convey the corresponding
     *         protocol level errors to the client :
     *         - ESP_OK : Success
     *         - ESP_ERR_INVALID_ARG   : InvalidArgument
     *         - ESP_ERR_INVALID_STATE : InvalidProto
     *         - All other error codes : InternalError
     */
    esp_err_t (*get_prop_values)(size_t props_count,
                                 const esp_local_ctrl_prop_t props[],
                                 esp_local_ctrl_prop_val_t prop_values[],
                                 void *usr_ctx);

    /**
     * @brief Handler function to be implemented for changing values of properties
     *
     * @note  If any of the properties have variable sizes, the size field
     * of the corresponding element in `prop_values` must be checked
     * explicitly before making any assumptions on the size.
     *
     * @param[in]  props_count  Total elements in the props array
     * @param[in]  props        Array of properties, the values for which the
     *                          client requests to change
     * @param[in]  prop_values  Array of property values, the elements of which
     *                          need to be used for updating those properties
     *                          specified by props argument
     * @param[in]  usr_ctx      This provides value of the `usr_ctx` field of
     *                          `esp_local_ctrl_handlers_t` structure
     *
     * @return Returning different error codes will convey the corresponding
     *         protocol level errors to the client :
     *         - ESP_OK : Success
     *         - ESP_ERR_INVALID_ARG   : InvalidArgument
     *         - ESP_ERR_INVALID_STATE : InvalidProto
     *         - All other error codes : InternalError
     */
    esp_err_t (*set_prop_values)(size_t props_count,
                                 const esp_local_ctrl_prop_t props[],
                                 const esp_local_ctrl_prop_val_t prop_values[],
                                 void *usr_ctx);

    /**
     * Context pointer to be passed to above handler functions upon invocation.
     * This is different from the property level context, as this is valid
     * throughout the lifetime of the `esp_local_ctrl` service, and freed only
     * when the service is stopped.
     */
    void *usr_ctx;

    /**
     * Pointer to function which will be internally invoked on `usr_ctx` for
     * freeing the context resources when `esp_local_ctrl_stop()` is called.
     */
    void (*usr_ctx_free_fn)(void *usr_ctx);
} esp_local_ctrl_handlers_t;

/**
 * @brief   Transport mode (BLE / HTTPD) over which the service will be provided
 *
 * This is forward declaration of a private structure, implemented internally
 * by `esp_local_ctrl`.
 */
typedef struct esp_local_ctrl_transport esp_local_ctrl_transport_t;

/**
 * @brief   Function for obtaining BLE transport mode
 */
const esp_local_ctrl_transport_t *esp_local_ctrl_get_transport_ble(void);

/**
 * @brief   Function for obtaining HTTPD transport mode
 */
const esp_local_ctrl_transport_t *esp_local_ctrl_get_transport_httpd(void);

#define ESP_LOCAL_CTRL_TRANSPORT_BLE   esp_local_ctrl_get_transport_ble()
#define ESP_LOCAL_CTRL_TRANSPORT_HTTPD esp_local_ctrl_get_transport_httpd()

/**
 * @brief   Configuration for transport mode BLE
 *
 * This is a forward declaration for `protocomm_ble_config_t`.
 * To use this, application must set CONFIG_BT_BLUEDROID_ENABLED
 * and include `protocomm_ble.h`.
 */
typedef struct protocomm_ble_config esp_local_ctrl_transport_config_ble_t;

/**
 * @brief   Configuration for transport mode HTTPD
 *
 * This is a forward declaration for `httpd_ssl_config_t` (for HTTPS)
 * or `httpd_config_t` (for HTTP)
 */
#ifdef CONFIG_ESP_HTTPS_SERVER_ENABLE
/* To use this, application must set CONFIG_ESP_HTTPS_SERVER_ENABLE
 * and include `esp_https_server.h`
 */
typedef struct httpd_ssl_config esp_local_ctrl_transport_config_httpd_t;
#else
typedef struct httpd_config esp_local_ctrl_transport_config_httpd_t;
#endif
/**
 * @brief   Transport mode (BLE / HTTPD) configuration
 */
typedef union {
    /**
     * This is same as `protocomm_ble_config_t`. See `protocomm_ble.h` for
     * available configuration parameters.
     */
    esp_local_ctrl_transport_config_ble_t *ble;

    /**
     * This is same as `httpd_ssl_config_t`. See `esp_https_server.h` for
     * available configuration parameters.
     */
    esp_local_ctrl_transport_config_httpd_t *httpd;
} esp_local_ctrl_transport_config_t;

/**
 * @brief   Security types for esp_local_control
 */
typedef enum esp_local_ctrl_proto_sec {
    PROTOCOM_SEC0 = 0,
    PROTOCOM_SEC1,
    PROTOCOM_SEC2,
    PROTOCOM_SEC_CUSTOM,
} esp_local_ctrl_proto_sec_t;

typedef protocomm_security1_params_t esp_local_ctrl_security1_params_t;
typedef protocomm_security2_params_t esp_local_ctrl_security2_params_t;

/**
 * Protocom security configs
 */
typedef struct esp_local_ctrl_proto_sec_cfg {
     /**
     * This sets protocom security version, sec0/sec1 or custom
     * If custom, user must provide handle via `proto_sec_custom_handle` below
     */
    esp_local_ctrl_proto_sec_t version;

    /**
     * Custom security handle if security is set custom via `proto_sec` above
     * This handle must follow `protocomm_security_t` signature
     */
    void *custom_handle;

    /* Anonymous union */
    union {
        /**
         * Proof of possession to be used for local control. Could be NULL.
         */
        const void *pop __attribute__((deprecated("use sec_params field instead")));

        /**
         * Pointer to security params (NULL if not needed).
         * This is not needed for protocomm security 0
         * This pointer should hold the struct of type
         * esp_local_ctrl_security1_params_t for protocomm security 1
         * and esp_local_ctrl_security2_params_t for protocomm security 2 respectively. Could be NULL.
         */
        const void *sec_params;
    };
} esp_local_ctrl_proto_sec_cfg_t;

/**
 * @brief   Configuration structure to pass to `esp_local_ctrl_start()`
 */
typedef struct esp_local_ctrl_config {
    /**
     * Transport layer over which service will be provided
     */
    const esp_local_ctrl_transport_t *transport;

    /**
     * Transport layer over which service will be provided
     */
    esp_local_ctrl_transport_config_t transport_config;

    /**
     * Security version and POP
     */
    esp_local_ctrl_proto_sec_cfg_t proto_sec;

    /**
     * Register handlers for responding to get/set requests on properties
     */
    esp_local_ctrl_handlers_t handlers;

    /**
     * This limits the number of properties that are available at a time
     */
    size_t max_properties;
} esp_local_ctrl_config_t;

/**
 * @brief   Start local control service
 *
 * @param[in] config    Pointer to configuration structure
 *
 * @return
 *  - ESP_OK      : Success
 *  - ESP_FAIL    : Failure
 */
esp_err_t esp_local_ctrl_start(const esp_local_ctrl_config_t *config);

/**
 * @brief   Stop local control service
 */
esp_err_t esp_local_ctrl_stop(void);

/**
 * @brief   Add a new property
 *
 * This adds a new property and allocates internal resources for it.
 * The total number of properties that could be added is limited by
 * configuration option `max_properties`
 *
 * @param[in] prop    Property description structure
 *
 * @return
 *  - ESP_OK      : Success
 *  - ESP_FAIL    : Failure
 */
esp_err_t esp_local_ctrl_add_property(const esp_local_ctrl_prop_t *prop);

/**
 * @brief   Remove a property
 *
 * This finds a property by name, and releases the internal resources
 * which are associated with it.
 *
 * @param[in] name    Name of the property to remove
 *
 * @return
 *  - ESP_OK      : Success
 *  - ESP_ERR_NOT_FOUND : Failure
 */
esp_err_t esp_local_ctrl_remove_property(const char *name);

/**
 * @brief   Get property description structure by name
 *
 * This API may be used to get a property's context structure
 * `esp_local_ctrl_prop_t` when its name is known
 *
 * @param[in] name    Name of the property to find
 *
 * @return
 *  - Pointer to property
 *  - NULL if not found
 */
const esp_local_ctrl_prop_t *esp_local_ctrl_get_property(const char *name);

/**
 * @brief   Register protocomm handler for a custom endpoint
 *
 * This API can be called by the application to register a protocomm handler
 * for an endpoint after the local control service has started.
 *
 * @note In case of BLE transport the names and uuids of all custom
 * endpoints must be provided beforehand as a part of the `protocomm_ble_config_t`
 * structure set in `esp_local_ctrl_config_t`, and passed to `esp_local_ctrl_start()`.
 *
 * @param[in] ep_name   Name of the endpoint
 * @param[in] handler   Endpoint handler function
 * @param[in] user_ctx  User data
 *
 * @return
 *  - ESP_OK      : Success
 *  - ESP_FAIL    : Failure
 */
esp_err_t esp_local_ctrl_set_handler(const char *ep_name,
                                     protocomm_req_handler_t handler,
                                     void *user_ctx);

#ifdef __cplusplus
}
#endif
