/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_bt_defs.h"
#include "esp_pba_defs.h"

#define ESP_PBAC_INVALID_HANDLE     0               /*!< invalid handle value */

typedef uint16_t esp_pbac_conn_hdl_t;

/**
 * @brief PBA client callback events
 */
typedef enum {
    ESP_PBAC_INIT_EVT,                              /*!< PBA client initialized event */
    ESP_PBAC_DEINIT_EVT,                            /*!< PBA client de-initialized event */
    ESP_PBAC_CONNECTION_STATE_EVT,                  /*!< PBA client connection state changed event */
    ESP_PBAC_PULL_PHONE_BOOK_RESPONSE_EVT,          /*!< Response of pull phone book */
    ESP_PBAC_SET_PHONE_BOOK_RESPONSE_EVT,           /*!< Response of set phone book */
    ESP_PBAC_PULL_VCARD_LISTING_RESPONSE_EVT,       /*!< Response of pull vCard listing */
    ESP_PBAC_PULL_VCARD_ENTRY_RESPONSE_EVT,         /*!< Response of pull vCard entry */
} esp_pbac_event_t;

/**
 * @brief PBA client status code
 */
typedef enum {
    ESP_PBAC_SUCCESS = 0,                           /*!< Operation success */
    ESP_PBAC_FAILURE,                               /*!< Generic failure */
    ESP_PBAC_ALREADY_CONN,                          /*!< Connection to peer device already exist */
    ESP_PBAC_NO_RESOURCE,                           /*!< No more resource */
    ESP_PBAC_SDP_FAIL,                              /*!< Connection failed in SDP */
    ESP_PBAC_GOEP_FAIL,                             /*!< Operation failed in GOEP */
    ESP_PBAC_AUTH_FAIL,                             /*!< Connection failed in OBEX authentication */
    ESP_PBAC_DEINIT,                                /*!< Connection closed due to pba client is deinit */

    /* these error code is related to OBEX */
    ESP_PBAC_BAD_REQUEST            = 0xC0,         /*!< Server couldn't understand request */
    ESP_PBAC_UNAUTHORIZED           = 0xC1,         /*!< Unauthorized */
    ESP_PBAC_FORBIDDEN              = 0xC3,         /*!< Operation is understood but refused */
    ESP_PBAC_NOT_FOUND              = 0xC4,         /*!< Not found */
    ESP_PBAC_NOT_ACCEPTABLE         = 0xC6,         /*!< Not Acceptable */
    ESP_PBAC_PRECONDITION_FAILED    = 0xCC,         /*!< Precondition failed */
    ESP_PBAC_NOT_IMPLEMENTED        = 0xD1,         /*!< Not implemented */
    ESP_PBAC_SERVICE_UNAVAILABLE    = 0xD3,         /*!< Service unavailable */
} esp_pbac_status_t;

/**
 * @brief PBA client set phone book flags
 */
typedef enum {
    ESP_PBAC_SET_PHONE_BOOK_FLAGS_ROOT = 0x02,          /*!< Go back to root, name should set to empty string, not NULL */
    ESP_PBAC_SET_PHONE_BOOK_FLAGS_DOWN = 0x02,          /*!< Go down 1 level, name should set to child folder */
    ESP_PBAC_SET_PHONE_BOOK_FLAGS_UP = 0x03,            /*!< Go up 1 level, name is optional */
} esp_pbac_set_phone_book_flags_t;

/**
 * @brief PBA client pull phone book optional application parameter
 */
typedef struct {
    uint8_t include_property_selector       : 1;        /*!< 1 if app param include property_selector */
    uint8_t include_format                  : 1;        /*!< 1 if app param include format */
    uint8_t include_max_list_count          : 1;        /*!< 1 if app param include max_list_count */
    uint8_t include_list_start_offset       : 1;        /*!< 1 if app param include list_start_offset */
    uint8_t include_reset_new_missed_calls  : 1;        /*!< 1 if app param include reset_new_missed_calls */
    uint8_t include_vcard_selector          : 1;        /*!< 1 if app param include vcard_selector */
    uint8_t include_vcard_selector_operator : 1;        /*!< 1 if app param include vcard_selector_operator */
    uint8_t format;                                     /*!< 0x00 = 2.1, 0x01 = 3.0 */
    uint8_t reset_new_missed_calls;                     /*!< 0x01 = Reset */
    uint8_t vcard_selector_operator;                    /*!< 0x00 = OR, 0x01 = AND */
    uint16_t max_list_count;                            /*!< 0x0000 to 0xFFFF */
    uint16_t list_start_offset;                         /*!< 0x0000 to 0xFFFF */
    uint64_t property_selector;                         /*!< 64 bits mask */
    uint64_t vcard_selector;                            /*!< 64 bits mask */
} esp_pbac_pull_phone_book_app_param_t;

/**
 * @brief PBA client pull vCard listing optional application parameter
 */
typedef struct {
    uint8_t include_order                   : 1;        /*!< 1 if app param include order */
    uint8_t include_search_value            : 1;        /*!< 1 if app param include search_value */
    uint8_t include_search_property         : 1;        /*!< 1 if app param include search_property */
    uint8_t include_max_list_count          : 1;        /*!< 1 if app param include max_list_count */
    uint8_t include_list_start_offset       : 1;        /*!< 1 if app param include list_start_offset */
    uint8_t include_reset_new_missed_calls  : 1;        /*!< 1 if app param include reset_new_missed_calls */
    uint8_t include_vcard_selector          : 1;        /*!< 1 if app param include vcard_selector */
    uint8_t include_vcard_selector_operator : 1;        /*!< 1 if app param include vcard_selector_operator */
    uint8_t order;                                      /*!< 0x00 = indexed, 0x01 = alphanumeric */
    uint8_t search_property;                            /*!< 0x00 = Name, 0x01 = Number, 0x02 = Sound */
    uint8_t reset_new_missed_calls;                     /*!< 0x01 = Reset */
    uint8_t vcard_selector_operator;                    /*!< 0x00 = OR, 0x01 = AND */
    uint16_t max_list_count;                            /*!< 0x0000 to 0xFFFF */
    uint16_t list_start_offset;                         /*!< 0x0000 to 0xFFFF */
    char *search_value;                                 /*!< Text */
    uint64_t vcard_selector;                            /*!< 64 bits mask */
} esp_pbac_pull_vcard_listing_app_param_t;

/**
 * @brief PBA client pull vCard entry optional application parameter
 */
typedef struct {
    uint8_t include_property_selector : 1;              /*!< 1 if app param include property_selector */
    uint8_t include_format            : 1;              /*!< 1 if app param include format */
    uint8_t format;                                     /*!< 0x00 = 2.1, 0x01 = 3.0 */
    uint64_t property_selector;                         /*!< 64 bits mask */
} esp_pbac_pull_vcard_entry_app_param_t;

/**
 * @brief PBA client callback parameters
 */
typedef union {
    /**
     * @brief  ESP_PBAC_CONNECTION_STATE_EVT
     */
    struct pbac_conn_stat_param {
        bool connected;                                 /*!< whether pba client is connected to server */
        esp_pbac_conn_hdl_t handle;                     /*!< connection handle, non zeros if exist */
        uint8_t peer_supported_repo;                    /*!< peer supported repositories */
        uint32_t peer_supported_feat;                   /*!< peer supported features */
        esp_bd_addr_t remote_bda;                       /*!< remote bluetooth device address */
        esp_pbac_status_t reason;                       /*!< reason if disconnect */
    } conn_stat;                                        /*!< PBA client connection status */

    /**
     * @brief  ESP_PBAC_PULL_PHONE_BOOK_RESPONSE_EVT
     */
    struct pbac_pull_phone_book_rsp_param {
        esp_pbac_conn_hdl_t handle;                                 /*!< PBA client connection handle */
        esp_pbac_status_t result;                                   /*!< operation result, ESP_PBAC_SUCCESS if success */
        bool final;                                                 /*!< whether this is the final response packet */
        uint8_t *data;                                              /*!< response data */
        uint16_t data_len;                                          /*!< response data len */
        /* The following are the application parameters */
        uint8_t include_phone_book_size          : 1;               /*!< 1 if app param include phone_book_size */
        uint8_t include_new_missed_calls         : 1;               /*!< 1 if app param include new_missed_calls */
        uint8_t include_primary_folder_version   : 1;               /*!< 1 if app param include primary_folder_version */
        uint8_t include_secondary_folder_version : 1;               /*!< 1 if app param include secondary_folder_version */
        uint8_t include_database_identifier      : 1;               /*!< 1 if app param include database_identifier */
        uint8_t new_missed_calls;                                   /*!< 0x00 to 0xFF */
        uint16_t phone_book_size;                                   /*!< 0x0000 to 0xFFFF */
        uint8_t *primary_folder_version;                            /*!< 0 to (2^128 -1) */
        uint8_t *secondary_folder_version;                          /*!< 0 to (2^128 -1) */
        uint8_t *database_identifier;                               /*!< 0 to (2^128 -1) */
    } pull_phone_book_rsp;                                          /*!< pull phone book response */

    /**
     * @brief  ESP_PBAC_SET_PHONE_BOOK_RESPONSE_EVT
     */
    struct pbac_set_phone_book_rsp_param {
        esp_pbac_conn_hdl_t handle;                                 /*!< PBA client connection handle */
        esp_pbac_status_t result;                                   /*!< operation result, ESP_PBAC_SUCCESS if success */
    } set_phone_book_rsp;                                           /*!< set phone book response, always the final response */

    /**
     * @brief  ESP_PBAC_PULL_VCARD_LISTING_RESPONSE_EVT
     */
    struct pbac_pull_vcard_listing_rsp_param {
        esp_pbac_conn_hdl_t handle;                                     /*!< PBA client connection handle */
        esp_pbac_status_t result;                                       /*!< operation result, ESP_PBAC_SUCCESS if success */
        bool final;                                                     /*!< whether this is the final response packet */
        uint8_t *data;                                                  /*!< response data */
        uint16_t data_len;                                              /*!< response data len */
        /* The following are the application parameters */
        uint8_t include_phone_book_size          : 1;                   /*!< 1 if app param include phone_book_size */
        uint8_t include_new_missed_calls         : 1;                   /*!< 1 if app param include new_missed_calls */
        uint8_t include_primary_folder_version   : 1;                   /*!< 1 if app param include primary_folder_version */
        uint8_t include_secondary_folder_version : 1;                   /*!< 1 if app param include secondary_folder_version */
        uint8_t include_database_identifier      : 1;                   /*!< 1 if app param include database_identifier */
        uint8_t new_missed_calls;                                       /*!< 0x00 to 0xFF */
        uint16_t phone_book_size;                                       /*!< 0x0000 to 0xFFFF */
        uint8_t *primary_folder_version;                                /*!< 0 to (2^128 -1) */
        uint8_t *secondary_folder_version;                              /*!< 0 to (2^128 -1) */
        uint8_t *database_identifier;                                   /*!< 0 to (2^128 -1) */
    } pull_vcard_listing_rsp;                                           /*!< pull vcard listing response */

    /**
     * @brief  ESP_PBAC_PULL_VCARD_ENTRY_RESPONSE_EVT
     */
    struct pbac_pull_vcard_entry_rsp_param {
        esp_pbac_conn_hdl_t handle;                                     /*!< PBA client connection handle */
        esp_pbac_status_t result;                                       /*!< operation result, ESP_PBAC_SUCCESS if success */
        bool final;                                                     /*!< whether this is the final response packet */
        uint8_t *data;                                                  /*!< response data */
        uint16_t data_len;                                              /*!< response data len */
        /* The following are the application parameters */
        uint8_t include_database_identifier : 1;                        /*!< 1 if app param include database_identifier */
        uint8_t *database_identifier;                                   /*!< 0 to (2^128 -1) */
    } pull_vcard_entry_rsp;                                             /*!< pull vcard listing response */
} esp_pbac_param_t;

/**
 * @brief           PBA client callback function type
 *
 * @param           event : Event type
 *
 * @param           param : Pointer to callback parameter
 */
typedef void (*esp_pbac_callback_t)(esp_pbac_event_t event, esp_pbac_param_t *param);


/**
 * @brief           This function is called to register a user callbacks in PBA client.
 *
 * @param[in]       callback: pointer to the user callback function.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_register_callback(esp_pbac_callback_t callback);

/**
 * @brief           Initializes PBA client interface. This function should be called after bluedroid
 *                  enable successfully, and should be called after esp_pbac_register_callback.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_init(void);

/**
 * @brief           De-initializes PBA client interface. This will close all PBA client connection.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_deinit(void);

/**
 * @brief           Start the process to establish a connection to PBA server.
 *
 * @param[in]       bd_addr: peer bluetooth device address
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_connect(esp_bd_addr_t bd_addr);

/**
 * @brief           Disconnects from the current connected PBA server.
 *
 * @param[in]       handle: connection handle retrieved from ESP_PBAC_CONNECTION_STATE_EVT
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_disconnect(esp_pbac_conn_hdl_t handle);

/**
 * @brief           Send a request to pull phone book.
 *
 * @param[in]       handle: connection handle retrieved from ESP_PBAC_CONNECTION_STATE_EVT
 *
 * @param[in]       name: phone book object path and name, shall contain the absolute path
 *                        in the virtual folder architecture
 *
 * @param[in]       app_param: optional application parameter
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_pull_phone_book(esp_pbac_conn_hdl_t handle, const char *name, esp_pbac_pull_phone_book_app_param_t *app_param);

/**
 * @brief           Send a request to set the current folder in the virtual folder architecture.
 *
 * @param[in]       handle: connection handle retrieved from ESP_PBAC_CONNECTION_STATE_EVT
 *
 * @param[in]       flags: operation flags, one of ESP_PBAC_SET_PHONE_BOOK_FLAGS_XXX
 *
 * @param[in]       name: folder name, if flags is set to ROOT, name should be empty string (""),
 *                        if flags is set to UP, name is optional, if flags is set to DOWN, name
 *                        is mandatory
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_set_phone_book(esp_pbac_conn_hdl_t handle, esp_pbac_set_phone_book_flags_t flags, const char *name);

/**
 * @brief           Set the current folder in the virtual folder architecture, use absolute path.
 *
 * @param[in]       handle: connection handle retrieved from ESP_PBAC_CONNECTION_STATE_EVT
 *
 * @param[in]       path: absolute path of the folder intend to set. NULL or empty string will
 *                  set to ROOT
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_set_phone_book2(esp_pbac_conn_hdl_t handle, const char *path);

/**
 * @brief           Send a request to pull vCard listing.
 *
 * @param[in]       handle: connection handle retrieved from ESP_PBAC_CONNECTION_STATE_EVT
 *
 * @param[in]       name: specifies the name of the folder to be retrieved, uses relative paths,
 *                        shall not include any path information. An empty name (empty string "")
 *                        may be sent to retrieve the vCard Listing object of the current folder.
 *                        However, it is illegal to issue a pull vCard listing request with an
 *                        empty name header from the ‘telecom/’ folder
 *
 * @param[in]       app_param: optional application parameter
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_pull_vcard_listing(esp_pbac_conn_hdl_t handle, const char *name, esp_pbac_pull_vcard_listing_app_param_t *app_param);

/**
 * @brief           Send a request to pull vCard entry.
 *
 * @param[in]       handle: connection handle retrieved from ESP_PBAC_CONNECTION_STATE_EVT
 *
 * @param[in]       name: vCard name or, if supported, the X-BT-UID of the object to be retrieved.
 *                        uses relative paths,shall not include any path information
 *
 * @param[in]       app_param: optional application parameter
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_pbac_pull_vcard_entry(esp_pbac_conn_hdl_t handle, const char *name, esp_pbac_pull_vcard_entry_app_param_t *app_param);
