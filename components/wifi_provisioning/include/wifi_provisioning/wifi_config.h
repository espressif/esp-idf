/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _WIFI_PROV_CONFIG_H_
#define _WIFI_PROV_CONFIG_H_

#include <lwip/ip4_addr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   WiFi STA status for conveying back to the provisioning master
 */
typedef enum {
    WIFI_PROV_STA_CONNECTING,
    WIFI_PROV_STA_CONNECTED,
    WIFI_PROV_STA_DISCONNECTED
} wifi_prov_sta_state_t;

/**
 * @brief   WiFi STA connection fail reason
 */
typedef enum {
    WIFI_PROV_STA_AUTH_ERROR,
    WIFI_PROV_STA_AP_NOT_FOUND
} wifi_prov_sta_fail_reason_t;

/**
 * @brief   WiFi STA connected status information
 */
typedef struct {
    /**
     * IP Address received by station
     */
    char    ip_addr[IP4ADDR_STRLEN_MAX];

    char    bssid[6];   /*!< BSSID of the AP to which connection was estalished */
    char    ssid[33];   /*!< SSID of the to which connection was estalished */
    uint8_t channel;    /*!< Channel of the AP */
    uint8_t auth_mode;  /*!< Authorization mode of the AP */
} wifi_prov_sta_conn_info_t;

/**
 * @brief   WiFi status data to be sent in response to `get_status` request from master
 */
typedef struct {
    wifi_prov_sta_state_t wifi_state;    /*!< WiFi state of the station */
    union {
        /**
         * Reason for disconnection (valid only when `wifi_state` is `WIFI_STATION_DISCONNECTED`)
         */
        wifi_prov_sta_fail_reason_t fail_reason;

        /**
         * Connection information (valid only when `wifi_state` is `WIFI_STATION_CONNECTED`)
         */
        wifi_prov_sta_conn_info_t   conn_info;
    };
} wifi_prov_config_get_data_t;

/**
 * @brief   WiFi config data received by slave during `set_config` request from master
 */
typedef struct {
    char    ssid[33];       /*!< SSID of the AP to which the slave is to be connected */
    char    password[64];   /*!< Password of the AP */
    char    bssid[6];       /*!< BSSID of the AP */
    uint8_t channel;        /*!< Channel of the AP */
} wifi_prov_config_set_data_t;

/**
 * @brief   Type of context data passed to each get/set/apply handler
 *           function set in `wifi_prov_config_handlers` structure.
 *
 * This is passed as an opaque pointer, thereby allowing it be defined
 * later in application code as per requirements.
 */
typedef struct wifi_prov_ctx wifi_prov_ctx_t;

/**
 * @brief   Internal handlers for receiving and responding to protocomm
 *          requests from master
 *
 * This is to be passed as priv_data for protocomm request handler
 * (refer to `wifi_prov_config_data_handler()`) when calling `protocomm_add_endpoint()`.
 */
typedef struct wifi_prov_config_handlers {
    /**
     * Handler function called when connection status
     * of the slave (in WiFi station mode) is requested
     */
    esp_err_t (*get_status_handler)(wifi_prov_config_get_data_t *resp_data,
                                    wifi_prov_ctx_t **ctx);

    /**
     * Handler function called when WiFi connection configuration
     * (eg. AP SSID, password, etc.) of the slave (in WiFi station mode)
     * is to be set to user provided values
     */
    esp_err_t (*set_config_handler)(const wifi_prov_config_set_data_t *req_data,
                                    wifi_prov_ctx_t **ctx);

    /**
     * Handler function for applying the configuration that was set in
     * `set_config_handler`. After applying the station may get connected to
     * the AP or may fail to connect. The slave must be ready to convey the
     * updated connection status information when `get_status_handler` is
     * invoked again by the master.
     */
    esp_err_t (*apply_config_handler)(wifi_prov_ctx_t **ctx);

    /**
     * Context pointer to be passed to above handler functions upon invocation
     */
    wifi_prov_ctx_t *ctx;
} wifi_prov_config_handlers_t;

/**
 * @brief   Handler for receiving and responding to requests from master
 *
 * This is to be registered as the `wifi_config` endpoint handler
 * (protocomm `protocomm_req_handler_t`) using `protocomm_add_endpoint()`
 */
esp_err_t wifi_prov_config_data_handler(uint32_t session_id, const uint8_t *inbuf, ssize_t inlen,
                                        uint8_t **outbuf, ssize_t *outlen, void *priv_data);

#ifdef __cplusplus
}
#endif

#endif
