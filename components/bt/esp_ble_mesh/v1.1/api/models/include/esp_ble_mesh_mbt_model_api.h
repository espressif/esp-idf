/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_MBT_MODEL_API_H_
#define _ESP_BLE_MESH_MBT_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_MODEL_OP_BLOB_TRANSFER_GET         ESP_BLE_MESH_MODEL_OP_2(0x83, 0x00)
#define ESP_BLE_MESH_MODEL_OP_BLOB_TRANSFER_START       ESP_BLE_MESH_MODEL_OP_2(0x83, 0x01)
#define ESP_BLE_MESH_MODEL_OP_BLOB_TRANSFER_CANCEL      ESP_BLE_MESH_MODEL_OP_2(0x83, 0x02)
#define ESP_BLE_MESH_MODEL_OP_BLOB_TRANSFER_STATUS      ESP_BLE_MESH_MODEL_OP_2(0x83, 0x03)
#define ESP_BLE_MESH_MODEL_OP_BLOB_BLOCK_GET            ESP_BLE_MESH_MODEL_OP_2(0x83, 0x05)
#define ESP_BLE_MESH_MODEL_OP_BLOB_BLOCK_START          ESP_BLE_MESH_MODEL_OP_2(0x83, 0x04)
#define ESP_BLE_MESH_MODEL_OP_BLOB_PARTIAL_BLOCK_REPORT ESP_BLE_MESH_MODEL_OP_1(0x65)
#define ESP_BLE_MESH_MODEL_OP_BLOB_BLOCK_STATUS         ESP_BLE_MESH_MODEL_OP_1(0x67)
#define ESP_BLE_MESH_MODEL_OP_BLOB_CHUNK_TRANSFER       ESP_BLE_MESH_MODEL_OP_1(0x66)
#define ESP_BLE_MESH_MODEL_OP_BLOB_INFORMATION_GET      ESP_BLE_MESH_MODEL_OP_2(0x83, 0x06)
#define ESP_BLE_MESH_MODEL_OP_BLOB_INFORMATION_STATUS   ESP_BLE_MESH_MODEL_OP_2(0x83, 0x07)

#define ESP_BLE_MESH_BLOB_ID_SIZE   8

/** @def    ESP_BLE_MESH_MODEL_MBT_CLI
 *
 *  @brief  Define a new BLOB Transfer Client model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a BLOB Transfer Client model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New BLOB Transfer Client model instance.
 */
#define ESP_BLE_MESH_MODEL_MBT_CLI(cli_pub, cli_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_MBT_CLI, \
                               NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_MBT_SRV
 *
 *  @brief  Define a new BLOB Transfer Server model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a BLOB Transfer Server model.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_blob_trans_srv_t.
 *
 *  @return New BLOB Transfer Server model instance.
 */
#define ESP_BLE_MESH_MODEL_MBT_SRV(srv_pub, srv_data) \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_MBT_SRV, \
                               NULL, srv_pub, srv_data)

/** BLOB Transfer Server model context */
typedef struct {
    esp_ble_mesh_model_t *model;                            /*!< Pointer to BLOB Transfer Server model */
} esp_ble_mesh_mbt_srv_t;

/** Parameters of BLOB receiver */
typedef struct {
    uint16_t unicast_addr;                                  /*!< Unicast address of the server */
    uint8_t  retrieved_transfer_phase;                      /*!< Retrieved transfer phase of the server */
    uint8_t  status:4;                                      /*!< Status of the last operation */
    uint16_t blocks_not_received_len;                       /*!< Indicates the length which blocks were not received by the server. */
    uint8_t *blocks_not_received;                           /*!< Indicates which blocks were not received by the server. */
    uint16_t missing_chunks_len;                            /*!< Indicates which chunks were not received in the current block */
    uint8_t *missing_chunks;                                /*!< Indicates which chunks were not received by the server in the current block */
/* The followings are the additional information contained in status messages. */
    uint8_t  transfer_mode:2;                               /*!< BLOB transfer mode */
    uint8_t  expected_blob_id[ESP_BLE_MESH_BLOB_ID_SIZE];   /*!< Expected BLOB identifier list */
    uint32_t blob_size;                                     /*!< BLOB size in octets */
    uint8_t  block_size_log;                                /*!< Indicates the block size */
    uint16_t transfer_mtu_size;                             /*!< MTU size in octets */
    bool block_status_recv;                                 /*!< Indicate if Blob Block Status is received as a response. */
} esp_ble_mesh_blob_receiver_t;                             /*!< Structure of BLOB receiver */

/** Parameters of BLOB Information Status */
typedef struct {
    uint8_t  min_block_size_log;                        /*!< Min Block Size Log */
    uint8_t  max_block_size_log;                        /*!< Max Block Size Log */
    uint16_t max_total_chunks;                          /*!< Max Total Chunks */
    uint16_t max_chunk_size;                            /*!< Max Chunk Size */
    uint32_t max_blob_size;                             /*!< Max BLOB Size */
    uint16_t server_mtu_size;                           /*!< Server MTU size */
    uint8_t  supported_transfer_mode;                   /*!< Supported Transfer Mode */
} esp_ble_mesh_blob_capabilities_t;                     /*!< Parameters of BLOB Information Status */

/** Parameters of BLOB retrieve capabilities */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to BLOB Transfer Server model */
    uint8_t   msg_role;                                 /*!< Role of the device - Node/Provisioner */

    uint16_t  multicast_addr;                           /*!< Multicast Address state */
    uint16_t  app_idx;                                  /*!< AppKey Index state */
    uint8_t   transfer_ttl;                             /*!< Transfer TTL state */
    uint8_t   unicast_addr_count;                       /*!< The count of unicast address */
    uint16_t *unicast_addr;                             /*!< Unicast address list */
} esp_ble_mesh_retrieve_capabilities_t;                 /*!< Parameters of BLOB retrieve capabilities */

/** Parameters of BLOB transfer */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to BLOB Transfer Server model */
    uint8_t   msg_role;                                 /*!< Role of the device - Node/Provisioner */

    uint8_t   unicast_addr_count;                       /*!< The count of unicast address */
    uint16_t *unicast_addr;                             /*!< Unicast address list */
    uint16_t  multicast_addr;                           /*!< Multicast Address state */
    uint16_t  app_idx;                                  /*!< AppKey Index state */
    uint8_t   transfer_ttl;                             /*!< Transfer TTL state */
    uint8_t   blob_id[ESP_BLE_MESH_BLOB_ID_SIZE];       /*!< BLOB identifier list */
    uint32_t  blob_size;                                /*!< BLOB size in octets */
    uint8_t  *blob_data;                                /*!< BLOB data */
    uint8_t   transfer_mode;                            /*!< BLOB transfer mode */
    uint16_t  client_timeout_base;                      /*!< Time wait for messages from the serve */
} esp_ble_mesh_transfer_blob_t;                         /*!< Parameters of BLOB transfer */

/** Parameters of BLOB Block Status message */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to BLOB Transfer Server model */
    uint8_t  msg_role;                                  /*!< Role of the device - Node/Provisioner */

    uint16_t multicast_addr;                            /*!< Multicast Address state */
    uint16_t app_idx;                                   /*!< AppKey Index state */
    uint8_t  transfer_ttl;                              /*!< Transfer TTL state */

    uint16_t block_number;                              /*!< Block number of the current block */
    uint16_t chunk_size;                                /*!< Chunk Size (in octets) for the current block */
} esp_ble_mesh_send_block_t;                            /*!< BLOB Block Status message structure */

/** Parameters of BLOB send message */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to BLOB Transfer Server model */
    uint8_t  msg_role;                                  /*!< Role of the device - Node/Provisioner */

    uint16_t multicast_addr;                            /*!< Multicast Address state */
    uint16_t app_idx;                                   /*!< AppKey Index state */
    uint8_t  transfer_ttl;                              /*!< Transfer TTL state */
} esp_ble_mesh_send_data_t;                             /*!< Parameters of BLOB send message */

/** Parameters of determine Block Status message */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to BLOB Transfer Server model */
    uint8_t  msg_role;                                  /*!< Role of the device - Node/Provisioner */

    uint16_t multicast_addr;                            /*!< Multicast Address state */
    uint16_t app_idx;                                   /*!< AppKey Index state */
    uint8_t  transfer_ttl;                              /*!< Transfer TTL state */
} esp_ble_mesh_determine_block_status_t;                /*!< Parameters of determine Block Status message */

/** Parameters of determine Block Status message */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to BLOB Transfer Server model */
    uint8_t   msg_role;                                 /*!< Role of the device - Node/Provisioner */

    uint16_t  multicast_addr;                           /*!< Multicast Address state */
    uint16_t  app_idx;                                  /*!< AppKey Index state */
    uint8_t   transfer_ttl;                             /*!< Transfer TTL state */
    uint8_t   unicast_addr_count;                       /*!< The count of unicast address */
    uint16_t *unicast_addr;                             /*!< Unicast address list */
} esp_ble_mesh_determine_transfer_status_t;             /*!< Parameters of determine Block Status message */

/** Parameters of cancel transfer message */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to BLOB Transfer Server model */
    uint8_t   msg_role;                                 /*!< Role of the device - Node/Provisioner */

    uint16_t  multicast_addr;                           /*!< Multicast Address state */
    uint16_t  app_idx;                                  /*!< AppKey Index state */
    uint8_t   transfer_ttl;                             /*!< Transfer TTL state */
    uint8_t   unicast_addr_count;                       /*!< The count of unicast address */
    uint16_t *unicast_addr;                             /*!< Unicast address list */

    uint8_t   blob_id[ESP_BLE_MESH_BLOB_ID_SIZE];       /*!< BLOB identifier list */
} esp_ble_mesh_cancel_transfer_t;                       /*!< Parameters of cancel transfer message */

/**
 * @brief BLOB Transfer Client model procedure result
                                */
#define ESP_BLE_MESH_MBT_CLIENT_RESULT_COMPLETE     0x00
#define ESP_BLE_MESH_MBT_CLIENT_RESULT_FAIL         0x01

/**
 * @brief BLOB Transfer Client model callback values union
 */
typedef union {
    /** Retrieve capabilities status */
    struct {
        int error_code;                                 /*!< Result of starting Retrieve Capabilities procedure */
        esp_ble_mesh_retrieve_capabilities_t input;     /*!< Input of starting Retrieve Capabilities procedure */
    } retrieve_capabilities_status;                     /*!< Retrieve capabilities status */
    /** Transfer BLOB status */
    struct {
        int error_code;                                 /*!< Result of starting Transfer BLOB procedure */
        esp_ble_mesh_transfer_blob_t input;             /*!< Input of starting Transfer BLOB procedure */
    } transfer_blob_status;                             /*!< Transfer BLOB status */
    /** Send block status */
    struct {
        int error_code;                                 /*!< Result of starting Send Block sub-procedure */
        esp_ble_mesh_send_block_t input;                /*!< Input of starting Send Block sub-procedure */
    } send_block_status;                                /*!< Send block status */
    /** Send data status */
    struct {
        int error_code;                                 /*!< Result of starting Send Data sub-procedure */
        esp_ble_mesh_send_data_t input;                 /*!< Input of starting Send Data sub-procedure */
    } send_data_status;                                 /*!< Send data status */
    /** Determine block status */
    struct {
        int error_code;                                 /*!< Result of starting Determine Block Status sub-procedure */
        esp_ble_mesh_determine_block_status_t input;    /*!< Input of starting Determine Block Status sub-procedure */
    } determine_block_status_status;                    /*!< Determine block status */
    /** Determine transfer status */
    struct {
        int error_code;                                 /*!< Result of starting Determine Transfer Status procedure */
        esp_ble_mesh_determine_transfer_status_t input; /*!< Input of starting Determine Transfer Status procedure */
    } determine_transfer_status_status;                 /*!< Determine transfer status */
    /** Cancel transfer status */
    struct {
        int error_code;                                 /*!< Result of starting Cancel Transfer procedure */
        esp_ble_mesh_cancel_transfer_t input;           /*!< Input of starting Cancel Transfer procedure */
    } cancel_transfer_status;                           /*!< Cancel transfer status */
    /** Retrieve capabilities complete */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint8_t result;                                 /*!< Result of Retrieve Capabilities procedure */
    } retrieve_capabilities_comp;                       /*!< Retrieve capabilities complete */
    /** Transfer BLOB complete */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint8_t result;                                 /*!< Result of Transfer BLOB procedure */
    } transfer_blob_comp;                               /*!< Transfer BLOB complete */
    /** Send block complete */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint8_t result;                                 /*!< Result of Send Block sub-procedure */
    } send_block_comp;                                  /*!< Send block complete */
    /** Send data complete */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint8_t result;                                 /*!< Result of Send Data sub-procedure */
    } send_data_comp;                                   /*!< Send data complete */
    /** Determine block status complete */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint8_t result;                                 /*!< Result of Determine Block Status sub-procedure */
    } determine_block_status_comp;                      /*!< Determine block status complete */
    /** Determine transfer status complete */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint8_t result;                                 /*!< Result of Determine Transfer Status procedure */
    } determine_transfer_status_comp;                   /*!< Determine transfer status complete */
    /** Cancel transfer complete */
    struct {
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint8_t result;                                 /*!< Result of Cancel Transfer procedure */
    } cancel_transfer_comp;                             /*!< Cancel transfer complete */
    /** Set transfer TTL */
    struct {
        int error_code;                                 /*!< Result of setting Transfer TTL state */
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint8_t transfer_ttl;                           /*!< Transfer TTL state */
    } set_transfer_ttl;                                 /*!< Set transfer TTL */
    /** Clear transfer TTL */
    struct {
        int error_code;                                 /*!< Result of clearing Transfer TTL state */
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
    } clear_transfer_ttl;                               /*!< Clear transfer TTL */
    /** Set application index */
    struct {
        int error_code;                                 /*!< Result of setting AppKey Index state */
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint16_t app_idx;                               /*!< AppKey Index state */
    } set_app_idx;                                      /*!< Set application index */
    /** Clear application index */
    struct {
        int error_code;                                 /*!< Result of clearing AppKey Index state */
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
    } clear_app_idx;                                    /*!< Clear application index */
    /** Set multicast address */
    struct {
        int error_code;                                 /*!< Result of setting Multicast Address state */
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
        uint16_t multicast_addr;                        /*!< Multicast Address state */
    } set_multicast_addr;                               /*!< Set multicast address */
    /** Clear multicast address */
    struct {
        int error_code;                                 /*!< Result of clearing Multicast Address state */
        esp_ble_mesh_model_t *model;                    /*!< Pointer to the BLOB Transfer Client model */
    } clear_multicast_addr;                             /*!< Clear multicast address */
} esp_ble_mesh_mbt_client_cb_value_t;                   /*!< BLOB Transfer Client model callback values union */

/** BLOB Transfer Client model callback parameters */
typedef struct {
    esp_ble_mesh_mbt_client_cb_value_t value;           /*!< BLOB Transfer Client model callback values */
} esp_ble_mesh_mbt_client_cb_param_t;                   /*!< BLOB Transfer Client model callback parameters */

/**
 * This enum value is the event of BLOB Transfer Client model.
 * Note: The idea of status/complete event comes from HCI Commands.
 */
typedef enum {
    ESP_BLE_MESH_MBT_CLIENT_RETRIEVE_CAPABILITIES_STATUS_EVT,
    ESP_BLE_MESH_MBT_CLIENT_TRANSFER_BLOB_STATUS_EVT,
    ESP_BLE_MESH_MBT_CLIENT_SEND_BLOCK_STATUS_EVT,
    ESP_BLE_MESH_MBT_CLIENT_SEND_DATA_STATUS_EVT,
    ESP_BLE_MESH_MBT_CLIENT_DETERMINE_BLOCK_STATUS_STATUS_EVT,
    ESP_BLE_MESH_MBT_CLIENT_DETERMINE_TRANSFER_STATUS_STATUS_EVT,
    ESP_BLE_MESH_MBT_CLIENT_CANCEL_TRANSFER_STATUS_EVT,
    ESP_BLE_MESH_MBT_CLIENT_RETRIEVE_CAPABILITIES_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_TRANSFER_BLOB_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_SEND_BLOCK_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_SEND_DATA_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_DETERMINE_BLOCK_STATUS_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_DETERMINE_TRANSFER_STATUS_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_CANCEL_TRANSFER_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_SET_TRANSFER_TTL_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_CLEAR_TRANSFER_TTL_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_SET_APP_IDX_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_CLEAR_APP_IDX_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_SET_MULTICAST_ADDR_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_CLEAR_MULTICAST_ADDR_COMP_EVT,
    ESP_BLE_MESH_MBT_CLIENT_EVT_MAX,
} esp_ble_mesh_mbt_client_cb_event_t;

/** Parameters of initialize BLOB receive */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to the BLOB Transfer Client model */

    uint8_t  blob_id[ESP_BLE_MESH_BLOB_ID_SIZE];        /*!< BLOB identifier list */
    uint16_t timeout;                                   /*!< Timeout */
    uint8_t  transfer_ttl;                              /*!< Transfer TTL state */
} esp_ble_mesh_initialize_blob_receive_t;               /*!< Structure of initialize BLOB receive */

/** Parameters of cancel BLOB receive */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to the BLOB Transfer Client model */

    uint8_t blob_id[ESP_BLE_MESH_BLOB_ID_SIZE];         /*!< BLOB identifier list */
} esp_ble_mesh_cancel_blob_receive_t;/*!< */

/** Parameters of cancel BLOB receive */
typedef struct {
    esp_ble_mesh_model_t *model;                        /*!< Pointer to the BLOB Transfer Client model */

    esp_ble_mesh_blob_capabilities_t caps;              /*!< Parameters of BLOB Information Status */
} esp_ble_mesh_set_blob_capabilities_t;/*!< */

/**
 * @brief BLOB Transfer Server model callback value union
 */
typedef union {
    /** Initialize BLOB receive complete */
    struct {
        int error_code;                                 /*!< Result of initializing BLOB receive */
        esp_ble_mesh_initialize_blob_receive_t input;   /*!< Input of initializing BLOB receive */
    } initialize_blob_receive_comp;                     /*!< Initialize BLOB receive complete */
    /** Cancel BLOB receive complete */
    struct {
        int error_code;                                 /*!< Result of canceling BLOB receive */
        esp_ble_mesh_cancel_blob_receive_t input;       /*!< Input of canceling BLOB receive */
    } cancel_blob_receive_comp;                         /*!< Cancel BLOB receive complete */
    /** Set BLOB capabilities complete */
    struct {
        int error_code;                                 /*!< Result of setting BLOB capabilities */
        esp_ble_mesh_set_blob_capabilities_t input;     /*!< Input of setting BLOB capabilities */
    } set_blob_capabilities_comp;                       /*!< Set BLOB capabilities complete */
    /** BLOB transfer get */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< BLOB Transfer Get message context */
    } blob_transfer_get;                                /*!< BLOB transfer get */
    /** BLOB transfer start */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< BLOB Transfer Start message context */
    } blob_transfer_start;                              /*!< BLOB transfer start */
    /** BLOB transfer cancel */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< BLOB Transfer Cancel message context */
    } blob_transfer_cancel;                             /*!< BLOB transfer cancel */
    /** BLOB block get */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< BLOB Block Get message context */
    } blob_block_get;                                   /*!< BLOB block get */
    /** BLOB block start */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< BLOB Block Start message context */
    } blob_block_start;                                 /*!< BLOB block start */
    /** BLOB chunk transfer */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< BLOB Chunk Transfer message context */
    } blob_chunk_transfer;                              /*!< BLOB chunk transfer */
    /** BLOB information get */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< BLOB Information Get message context */
    } blob_information_get;                             /*!< BLOB information get */
    /** Block receive complete */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< Information of receiving BLOB block completely */
    } block_receive_comp;                               /*!< Block receive complete */
    /** BLOB receive complete */
    struct {
        esp_ble_mesh_msg_ctx_t ctx;                     /*!< Information of receiving BLOB completely */
    } blob_receive_comp;                                /*!< BLOB receive complete */
} esp_ble_mesh_mbt_server_cb_value_t;                   /*!< BLOB Transfer Server model callback value union */

/** BLOB Transfer Server model callback parameters */
typedef struct {
    esp_ble_mesh_mbt_server_cb_value_t value;           /*!< Value of the received blob transfer messages */
} esp_ble_mesh_mbt_server_cb_param_t;                   /*!< BLOB Transfer Server model callback parameters */

/** This enum value is the event of BLOB Transfer Server model */
typedef enum {
    ESP_BLE_MESH_MBT_SERVER_INITIALIZE_BLOB_RECEIVE_COMP_EVT,
    ESP_BLE_MESH_MBT_SERVER_CANCEL_BLOB_RECEIVE_COMP_EVT,
    ESP_BLE_MESH_MBT_SERVER_SET_BLOB_CAPABILITIES_COMP_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_TRANSFER_GET_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_TRANSFER_START_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_TRANSFER_CANCEL_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_BLOCK_GET_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_BLOCK_START_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_CHUNK_TRANSFER_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_INFORMATION_GET_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOCK_RECEIVE_COMP_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_RECEIVE_COMP_EVT,
    ESP_BLE_MESH_MBT_SERVER_BLOB_RECEIVE_TIMEOUT_EVT,
    ESP_BLE_MESH_MBT_SERVER_EVT_MAX,
} esp_ble_mesh_mbt_server_cb_event_t;

/**
 * @brief   BLOB Transfer Client model callback function type
 *
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_mbt_client_cb_t)(esp_ble_mesh_mbt_client_cb_event_t event,
                                              esp_ble_mesh_mbt_client_cb_param_t *param);

/**
 * @brief   BLOB Transfer Server model callback function type
 *
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_mbt_server_cb_t)(esp_ble_mesh_mbt_server_cb_event_t event,
                                              esp_ble_mesh_mbt_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh BLOB Transfer Client model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_mbt_client_callback(esp_ble_mesh_mbt_client_cb_t callback);

/**
 * @brief       Register BLE Mesh BLOB Transfer Server model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_mbt_server_callback(esp_ble_mesh_mbt_server_cb_t callback);

/**
 * @brief       BLOB Transfer Client starts Retrieve Capabilities procedure.
 *
 * @param[in]   input: The input of Retrieve Capabilities procedure.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_retrieve_capabilities(esp_ble_mesh_retrieve_capabilities_t *input);

/**
 * @brief       BLOB Transfer Client starts Transfer BLOB procedure.
 *
 * @param[in]   input: The input of Transfer BLOB procedure.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_transfer_blob(esp_ble_mesh_transfer_blob_t *input);

/**
 * @brief       BLOB Transfer Client starts Send Block sub-procedure.
 *
 * @param[in]   input: The input of Send Block sub-procedure.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_send_block(esp_ble_mesh_send_block_t *input);

/**
 * @brief       BLOB Transfer Client starts Send Data sub-procedure.
 *
 * @param[in]   input: The input of Send Data sub-procedure.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_send_data(esp_ble_mesh_send_data_t *input);

/**
 * @brief       BLOB Transfer Client starts Determine Block Status sub-procedure.
 *
 * @param[in]   input: The input of Determine Block Status sub-procedure.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_determine_block_status(esp_ble_mesh_determine_block_status_t *input);

/**
 * @brief       BLOB Transfer Client starts Determine Transfer Status procedure.
 *
 * @param[in]   input: The input of Determine Transfer Status procedure.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_determine_transfer_status(esp_ble_mesh_determine_transfer_status_t *input);

/**
 * @brief       BLOB Transfer Client starts Cancel Transfer procedure.
 *
 * @param[in]   input: The input of Cancel Transfer procedure.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_cancel_transfer(esp_ble_mesh_cancel_transfer_t *input);

/**
 * @brief       BLOB Transfer Client gets BLOB receiver.
 *
 * @param[in]   model: BLOB Transfer Client model.
 * @param[in]   unicast_addr: Unicast address of the BLOB receiver.
 *
 * @return      BLOB receiver on success or NULL on failure.
 *
 */
const esp_ble_mesh_blob_receiver_t *esp_ble_mesh_mbt_client_get_blob_receiver(esp_ble_mesh_model_t *model,
                                                                              uint16_t unicast_addr);

/**
 * @brief       BLOB Transfer Client gets active BLOB receiver list.
 *
 * @param[in]   model: BLOB Transfer Client model.
 *
 * @return      Active BLOB receiver list on success or NULL on failure.
 *
 */
const esp_ble_mesh_blob_receiver_t **esp_ble_mesh_mbt_client_get_active_blob_receiver(esp_ble_mesh_model_t *model);

/**
 * @brief       BLOB Transfer Client gets BLOB transfer progress.
 *
 * @param[in]   model: BLOB Transfer Client model.
 * @param[in]   unicast_addr: Unicast address of the BLOB receiver.
 * @param[in]   block_percent: Block reception percent to be updated.
 * @param[in]   chunk_percent: Chunk reception percent of the current block to be updated.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_get_transfer_progress(esp_ble_mesh_model_t *model,
                                                        uint16_t unicast_addr,
                                                        uint8_t *block_percent,
                                                        uint8_t *chunk_percent);

/**
 * @brief       BLOB Transfer Client sets Transfer TTL state.
 *
 * @param[in]   model: BLOB Transfer Client model.
 * @param[in]   transfer_ttl: Transfer TTL state value.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_set_transfer_ttl(esp_ble_mesh_model_t *model,
                                                   uint8_t transfer_ttl);

/**
 * @brief       BLOB Transfer Client clear Transfer TTL state.
 *
 * @param[in]   model: BLOB Transfer Client model.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_clear_transfer_ttl(esp_ble_mesh_model_t *model);

/**
 * @brief       BLOB Transfer Client sets AppKey Index state.
 *
 * @param[in]   model: BLOB Transfer Client model.
 * @param[in]   app_idx: AppKey Index state value.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_set_app_idx(esp_ble_mesh_model_t *model,
                                              uint16_t app_idx);

/**
 * @brief       BLOB Transfer Client clear AppKey Index state.
 *
 * @param[in]   model: BLOB Transfer Client model.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_clear_app_idx(esp_ble_mesh_model_t *model);

/**
 * @brief       BLOB Transfer Client sets Multicast Address state.
 *
 * @param[in]   model: BLOB Transfer Client model.
 * @param[in]   multicast_addr: Multicast Address state value.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_set_multicast_addr(esp_ble_mesh_model_t *model,
                                                     uint16_t multicast_addr);

/**
 * @brief       BLOB Transfer Client clear Multicast Address state.
 *
 * @param[in]   model: BLOB Transfer Client model.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_client_clear_multicast_addr(esp_ble_mesh_model_t *model);

/**
 * @brief       BLOB Transfer Server initializes BLOB receive.
 *
 * @param[in]   input: The input of initializing BLOB receive.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_server_initialize_blob_receive(esp_ble_mesh_initialize_blob_receive_t *input);

/**
 * @brief       BLOB Transfer Server cancels BLOB receive.
 *
 * @param[in]   input: The input of cancelling BLOB receive.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_server_cancel_blob_receive(esp_ble_mesh_cancel_blob_receive_t *input);

/**
 * @brief       BLOB Transfer Server sets BLOB capabilities.
 *
 * @param[in]   input: The input of setting BLOB capabilities.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_server_set_blob_capabilities(esp_ble_mesh_set_blob_capabilities_t *input);

/**
 * @brief       BLOB Transfer Server gets current BLOB reception progress.
 *
 * @param[in]   model: BLOB Transfer Server model.
 * @param[in]   reception_progress: Reception progress to be updated.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_mbt_server_get_blob_reception_progress(esp_ble_mesh_model_t *model,
                                                              uint8_t *reception_progress);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_MBT_MODEL_API_H_ */
