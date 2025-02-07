/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BLE_CTE_API_H__
#define __ESP_BLE_CTE_API_H__

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// BLE CTE callback event type
typedef enum {
    ESP_BLE_CTE_SET_CONNLESS_TRANS_PARAMS_CMPL_EVT = 0,     /*!< When CTE set connectionless transmit parameters complete, the event comes */
    ESP_BLE_CTE_SET_CONNLESS_TRANS_ENABLE_CMPL_EVT,         /*!< When CTE set connectionless transmit enable complete, the event comes */
    ESP_BLE_CTE_SET_CONNLESS_IQ_SAMPLING_ENABLE_CMPL_EVT,   /*!< When CTE set connectionless IQ sampling enable complete, the event comes */
    ESP_BLE_CTE_SET_CONN_RECV_PARAMS_CMPL_EVT,              /*!< When CTE set connection receive parameters complete, the event comes */
    ESP_BLE_CTE_SET_CONN_TRANS_PARAMS_CMPL_EVT,             /*!< When CTE set connection transmit parameters complete, the event comes */
    ESP_BLE_CTE_SET_CONN_REQ_ENABLE_CMPL_EVT,               /*!< When CTE set connection CTE request enable complete, the event comes */
    ESP_BLE_CTE_SET_CONN_RSP_ENABLE_CMPL_EVT,               /*!< When CTE set connection CTE response enable complete, the event comes */
    ESP_BLE_CTE_READ_ANT_INFOR_CMPL_EVT,                    /*!< When CTE read antenna information complete, the event comes */
    ESP_BLE_CTE_CONNLESS_IQ_REPORT_EVT,                     /*!< When the device receives CTE connectionless IQ information, the event comes */
    ESP_BLE_CTE_CONN_IQ_REPORT_EVT,                         /*!< When the device receives CTE connection IQ information , the event comes */
    ESP_BLE_CTE_REQUEST_FAILED_EVT,                         /*!< When CTE request progress failed , the event comes */
    ESP_BLE_CTE_EVT_MAX,                                    /*!< when maximum advertising event complete, the event comes */
} esp_ble_cte_cb_event_t;

#define ESP_BLE_CTE_SAMPLING_DISABLE              (0x00)
#define ESP_BLE_CTE_SAMPLING_ENABLE               (0x01)

#define ESP_BLE_CTE_ADV_WITH_CTE_DISABLE          (0x00)
#define ESP_BLE_CTE_ADV_WITH_CTE_ENABLE           (0x01)

// Constant Tone Extension length in 8 µs units
#define ESP_BLE_CTE_MIN_CTE_LENGTH                (0x02)
#define ESP_BLE_CTE_MAX_CTE_LENGTH                (0x14)

#define ESP_BLE_CTE_TYPE_AOA                      (0x00) /*!< AoA Constant Tone Extension */
#define ESP_BLE_CTE_TYPE_AOD_WITH_1US             (0x01) /*!< AoD Constant Tone Extension with 1 µs slots */
#define ESP_BLE_CTE_TYPE_AOD_WITH_2US             (0x02) /*!< AoD Constant Tone Extension with 2 µs slots */
typedef uint8_t   esp_ble_cte_type_t;

#define ESP_BLE_CTE_MIN_CTE_COUNT                 (0x01)
#define ESP_BLE_CTE_MAX_CTE_COUNT                 (0x10)

#define ESP_BLE_CTE_MIN_SWITCHING_PATTERN_LENGTH  (0x02)
#define ESP_BLE_CTE_MAX_SWITCHING_PATTERN_LENGTH  (0x4B)

#define ESP_BLE_CTE_SLOT_DURATION_1US             (0x01)
#define ESP_BLE_CTE_SLOT_DURATION_2US             (0x02)
typedef uint8_t   esp_ble_cte_slot_dur_type_t;

#define ESP_BLE_CTE_MIN_SAMPLED_CTES              (0x00)
#define ESP_BLE_CTE_MAX_SAMPLED_CTES              (0x10)

#define ESP_BLE_CTE_TYPES_AOA_RESPONSE            (0x01)
#define ESP_BLE_CTE_TYPES_AOD_RESPONSE_WITH_1US   (0x02)
#define ESP_BLE_CTE_TYPES_AOD_RESPONSE_WITH_2US   (0x04)
#define ESP_BLE_CTE_TYPES_ALL     (ESP_BLE_CTE_TYPES_AOA_RESPONSE | ESP_BLE_CTE_TYPES_AOD_RESPONSE_WITH_1US | ESP_BLE_CTE_TYPES_AOD_RESPONSE_WITH_2US)
typedef uint8_t   esp_ble_cte_conn_cte_types_t;

#define ESP_BLE_CTE_REQUEST_FOR_CONNECTION_DISABLE      (0x00)
#define ESP_BLE_CTE_REQUEST_FOR_CONNECTION_ENABLE       (0x01)

#define ESP_BLE_CTE_MIN_REQUESTED_CTE_LENGTH            (0x02)
#define ESP_BLE_CTE_MAX_REQUESTED_CTE_LENGTH            (0x14)

#define ESP_BLE_CTE_RESPONSE_FOR_CONNECTION_DISABLE     (0x00)
#define ESP_BLE_CTE_RESPONSE_FOR_CONNECTION_ENABLE      (0x01)

typedef struct {
    uint8_t adv_handle;                    /*!< Used to identify an advertising set */
    uint8_t cte_len;                       /*!< Constant Tone Extension length in 8 µs units, range: 0x02 to 0x14 */
    esp_ble_cte_type_t cte_type;           /*!< AoA or AoD Constant Tone Extension */
    uint8_t cte_count;                     /*!< The number of Constant Tone Extensions to transmit in each periodic advertising interval, range: 0x01 to 0x10 */
    uint8_t switching_pattern_len;         /*!< The number of Antenna IDs in the pattern, range: 0x02 to 0x4B */
    uint8_t *antenna_ids;                  /*!< Antenna ID in the pattern */
} __attribute__((packed)) esp_ble_cte_connless_trans_params_t;

typedef struct {
    uint8_t adv_handle;    /*!< Identifier for the advertising set in which Constant Tone Extension is being enabled or disabled */
    uint8_t cte_enable;    /*!< Advertising with Constant Tone Extension is enabled or disabled */
} __attribute__((packed)) esp_ble_cte_trans_enable_params_t;

typedef struct {
    uint16_t sync_handle;                   /*!< Identifier for the periodic advertising train */
    uint8_t sampling_en;                    /*!< Enable or disable connectionless IQ sampling */
    esp_ble_cte_slot_dur_type_t slot_dur;   /*!< Switching and sampling slots, 1 us or 2 us */
    uint8_t max_sampled_ctes;               /*!< The maximum number of CTE to sample and report in each periodic advertising interval, range: 0x00 - 0x10 */
    uint8_t switching_pattern_len;          /*!< The number of Antenna IDs in the pattern, range: 0x02 to 0x4B */
    uint8_t *antenna_ids;                   /*!< Antenna ID in the pattern */
} __attribute__((packed)) esp_ble_cte_iq_sampling_params_t;

typedef struct {
    uint16_t conn_handle;                  /*!< Connection_Handle */
    uint8_t sampling_en;                   /*!< Enable or disable connection IQ sampling */
    esp_ble_cte_slot_dur_type_t slot_dur;  /*!< Switching and sampling slots, 1 us or 2 us */
    uint8_t switching_pattern_len;         /*!< The number of Antenna IDs in the pattern, range: 0x02 to 0x4B */
    uint8_t *antenna_ids;                  /*!< Antenna ID in the pattern */
} __attribute__((packed)) esp_ble_cte_recv_params_params_t;

typedef struct {
    uint16_t conn_handle;                     /*!< Connection_Handle */
    esp_ble_cte_conn_cte_types_t cte_types;   /*!< Allow AoA or AoD Constant Tone Extension Response */
    uint8_t switching_pattern_len;            /*!< The number of Antenna IDs in the pattern, range: 0x02 to 0x4B */
    uint8_t *antenna_ids;                     /*!< Antenna ID in the pattern */
} __attribute__((packed)) esp_ble_cte_conn_trans_params_t;

typedef struct {
    uint16_t conn_handle;                      /*!< Connection_Handle */
    uint8_t enable;                            /*!< Enable or disable Constant Tone Extension Request for the connection */
    uint16_t cte_req_interval;                 /*!< Requested interval for initiating the CTE Request procedure in number of underlying connection events, range: 0x0000 - 0xFFFF */
    uint8_t req_cte_len;                       /*!< Minimum length of the Constant Tone Extension being requested in 8 µs units, range: 0x02 - 0x14 */
    esp_ble_cte_type_t req_cte_Type;           /*!< AoA or AoD Constant Tone Extension, range: 0x00 - 0x02 */
} __attribute__((packed)) esp_ble_cte_req_en_params_t;

typedef struct {
    uint16_t conn_handle;          /*!< Connection_Handle */
    uint8_t enable;                /*!< Enable or disable Constant Tone Extension Response for the connection */
} __attribute__((packed)) esp_ble_cte_rsp_en_params_t;

/**
 * @brief CTE callback parameters union
 */
typedef union {
    /**
     * @brief ESP_BLE_CTE_SET_CONNLESS_TRANS_PARAMS_CMPL_EVT
     */
    struct ble_set_trans_params_cmpl_evt_param {
        esp_bt_status_t status;           /*!< Indicate setting transmit parameters status, status = (controller error code | 0x100) if status is not equal to 0 */
    } set_trans_params_cmpl;              /*!< Event parameter of ESP_BLE_CTE_SET_CONNLESS_TRANS_PARAMS_CMPL_EVT */

    /**
     * @brief ESP_BLE_CTE_SET_CONNLESS_TRANS_ENABLE_CMPL_EVT
     */
    struct ble_set_trans_enable_cmpl_evt_param {
        esp_bt_status_t status;           /*!< Indicate setting transmit enable status, status = (controller error code | 0x100) if status is not equal to 0 */
    } set_trans_enable_cmpl;              /*!< Event parameter of ESP_BLE_CTE_SET_CONNLESS_TRANS_ENABLE_CMPL_EVT */

    /**
     * @brief ESP_BLE_CTE_SET_CONNLESS_IQ_SAMPLING_ENABLE_CMPL_EVT
     */
    struct ble_set_iq_sampling_en_cmpl_evt_param {
        esp_bt_status_t status;             /*!< Indicate setting IQ sampling enable status, status = (controller error code | 0x100) if status is not equal to 0 */
        uint16_t sync_handle;               /*!< Sync_Handle identifying the periodic advertising */
    } iq_sampling_enable_cmpl;              /*!< Event parameter of ESP_BLE_CTE_SET_CONNLESS_IQ_SAMPLING_ENABLE_CMPL_EVT */

    /**
     * @brief ESP_BLE_CTE_SET_CONN_RECV_PARAMS_CMPL_EVT
     */
    struct ble_set_conn_recv_params_cmpl_evt_param {
        esp_bt_status_t status;           /*!< Indicate setting received parameters status, status = (controller error code | 0x100) if status is not equal to 0 */
        uint16_t conn_handle;             /*!< The connection identifier */
    } conn_recv_params_cmpl;              /*!< Event parameter of ESP_BLE_CTE_SET_CONN_RECV_PARAMS_CMPL_EVT */

    /**
     * @brief ESP_BLE_CTE_SET_CONN_TRANS_PARAMS_CMPL_EVT
     */
    struct ble_set_conn_trans_params_cmpl_evt_param {
        esp_bt_status_t status;            /*!< Indicate setting connection transmit parameters status, status = (controller error code | 0x100) if status is not equal to 0 */
        uint16_t conn_handle;              /*!< The connection identifier */
    } conn_trans_params_cmpl;              /*!< Event parameter of ESP_BLE_CTE_SET_CONN_TRANS_PARAMS_CMPL_EVT */

    /**
     * @brief ESP_BLE_CTE_SET_CONN_REQ_ENABLE_CMPL_EVT
     */
    struct ble_set_conn_req_en_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Indicate setting connection request enable status, status = (controller error code | 0x100) if status is not equal to 0 */
        uint16_t conn_handle;        /*!< The connection identifier */
    } conn_req_en_cmpl;              /*!< Event parameter of ESP_BLE_CTE_SET_CONN_REQ_ENABLE_CMPL_EVT */

    /**
     * @brief ESP_BLE_CTE_SET_CONN_RSP_ENABLE_CMPL_EVT
     */
    struct ble_set_conn_rsp_en_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Indicate setting connection response enable status, status = (controller error code | 0x100) if status is not equal to 0 */
        uint16_t conn_handle;        /*!< The connection identifier */
    } conn_rsp_en_cmpl;              /*!< Event parameter of ESP_BLE_CTE_SET_CONN_RSP_ENABLE_CMPL_EVT */

    /**
     * @brief ESP_BLE_CTE_READ_ANT_INFOR_CMPL_EVT
     */
    struct ble_read_ant_infor_cmpl_evt_param {
        esp_bt_status_t status;                      /*!< Indicate reading antenna information status, status = (controller error code | 0x100) if status is not equal to 0 */
        uint8_t supported_switching_sampling_rates;  /*!< bit 0: 1 µs switching supported for AoD transmission
                                                          bit 1: 1 µs sampling supported for AoD reception
                                                          bit 2: 1 µs switching and sampling supported for AoA reception */
        uint8_t num_antennae;                        /*!< The number of antennae supported by the Controller */
        uint8_t max_switching_pattern_len;           /*!< Maximum length of antenna switching pattern supported by the Controller */
        uint8_t max_cte_len;                         /*!< Maximum length of a transmitted Constant Tone Extension supported in 8 µs units */
    } read_ant_infor_cmpl;                           /*!< Event parameter of ESP_BLE_CTE_READ_ANT_INFOR_CMPL_EVT */

    /**
     * @brief ESP_BLE_CTE_CONNLESS_IQ_REPORT_EVT
     */
    struct ble_cte_connless_iq_rpt_evt_param {
        uint16_t sync_handle;                      /*!< Sync_Handle identifying the periodic advertising train */
        uint8_t channel_idx;                       /*!< The index of the channel on which the packet was received */
        int16_t rssi;                              /*!< RSSI of the packet, Range: -1270 to +200, Units: 0.1 dBm */
        uint8_t rssi_ant_id;                       /*!< Antenna ID */
        uint8_t cte_type;                          /*!< The type of Constant Tone Extension, range: 0x00 - 0x02 */
        uint8_t slot_dur;                          /*!< Switching and sampling slots, 1 us or 2us, range: 0x01 - 0x02 */
        uint8_t pkt_status;                        /*!< indicates whether the received packet had a valid CRC, range: 0x00 - 0x02 and 0xFF */
        uint16_t periodic_evt_counter;             /*!< The value of paEventCounter for the reported AUX_SYNC_IND PDU */
        uint8_t sample_count;                      /*!< Total number of sample pairs, range: 0x00 and 0x09 - 0x52*/
        uint8_t *i_sample;                         /*!< I sample for the reported packet. No valid sample available if value is 0x80 */
        uint8_t *q_sample;                         /*!< Q sample for the reported packet. No valid sample available if value is 0x80 */
    } connless_iq_rpt;                             /*!< Event parameter of ESP_BLE_CTE_CONNLESS_IQ_REPORT_EVT */

    /**
     * @brief ESP_BLE_CTE_CONN_IQ_REPORT_EVT
     */
    struct ble_cte_conn_iq_rpt_evt_param {
        uint16_t conn_handle;                    /*!< Connection_Handle */
        uint8_t rx_phy;                          /*!< The receiver PHY for the connection, range: 0x01 - 0x02 */
        uint8_t data_channel_idx;                /*!< The index of the data channel on which the Data Physical Channel PDU was received, range: 0x00 - 0x24 */
        int16_t rssi;                            /*!< RSSI of the packet, range: -1270 - +200, units: 0.1 dBm */
        uint8_t rssi_ant_id;                     /*!< ID of the antenna on which the RSSI is measured */
        uint8_t cte_type;                        /*!< AoA or AoD Constant Tone Extension, range: 0x00 - 0x02 */
        uint8_t slot_dur;                        /*!< Switching and sampling slots, range: 0x01 - 0x02 */
        uint8_t pkt_status;                      /*!< indicates whether the received packet had a valid CRC, range: 0x00 - 0x02 and 0xFF */
        uint16_t conn_evt_counter;               /*!< The value of connEventCounter for the reported PDU */
        uint8_t sample_count;                    /*!< Total number of sample pairs, range: 0x00 and 0x09 - 0x52 */
        uint8_t *i_sample;                       /*!< I sample for the reported PDU. No valid sample available if value is 0x80 */
        uint8_t *q_sample;                       /*!< Q sample for the reported PDU,. No valid sample available if value is 0x80 */
    } conn_iq_rpt;                               /*!< Event parameter of ESP_BLE_CTE_CONN_IQ_REPORT_EVT */

    /**
     * @brief ESP_BLE_CTE_REQUEST_FAILED_EVT
     */
    struct ble_cte_req_failed_evt_param{
        uint8_t  reason;                        /*!< value: 0x00, LL_CTE_RSP PDU received successfully but without a Constant Tone Extension field;
                                                    value: 0x01 to 0xFF, Peer rejected the request, see [Vol 1] Part F controller error codes and descriptions */
        uint16_t conn_handle;                   /*!< Connection_Handle */
    } req_failed_evt;                           /*!< Event parameter of ESP_BLE_CTE_REQUEST_FAILED_EVT */

} esp_ble_cte_cb_param_t;

/**
 * @brief CTE callback function type
 * @param event : Event type
 * @param param : Point to callback parameter, currently is union type
 */
typedef void (* esp_ble_cte_cb_t)(esp_ble_cte_cb_event_t event, esp_ble_cte_cb_param_t *param);

/**
 * @brief           This function is called to occur cte event
 *
 * @param[in]       callback: callback function
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_register_callback(esp_ble_cte_cb_t callback);

/**
 * @brief           This function is called to get the current cte callback
 *
 * @return
 *                  - esp_ble_cte_cb_t : callback function
 *
 */
esp_ble_cte_cb_t esp_ble_cte_get_callback(void);

/**
 * @brief           This function is called to set parameters for the transmission of CTE in the periodic advertising.
 *
 * @param[in]       cte_trans_params: pointer to User defined cte_trans_params data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_set_connectionless_trans_params(esp_ble_cte_connless_trans_params_t *cte_trans_params);

/**
 * @brief           This function is called to request that the Controller enables or disables the use of CTE in the periodic advertising.
 *
 * @param[in]       cte_trans_enable: pointer to User defined cte_trans_enable data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_set_connectionless_trans_enable(esp_ble_cte_trans_enable_params_t *cte_trans_enable);

/**
 * @brief           This function is called to request that the Controller enables or disables capturing IQ samples from the CTE
*                   of periodic advertising packets.
 *
 * @param[in]       iq_sampling_en: pointer to User defined iq_sampling_en data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_set_connectionless_iq_sampling_enable(esp_ble_cte_iq_sampling_params_t *iq_sampling_en);

/**
 * @brief           This function is called to enable or disable sampling received CTE fields on the connection.
 *
 * @param[in]       cte_recv_params: pointer to User defined cte_recv_params data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_set_connection_receive_params(esp_ble_cte_recv_params_params_t *cte_recv_params);

/**
 * @brief           This function is called to set the parameters used for transmitting CTE requested by the peer
 *                  device on the connection.
 *
 * @param[in]       cte_conn_trans_params: pointer to User defined cte_conn_trans_params data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_set_connection_transmit_params(esp_ble_cte_conn_trans_params_t *cte_conn_trans_params);

/**
 * @brief           This function is called to request the Controller to start or stop initiating the CTE Request
 *                  procedure  on a connection.
 *
 * @param[in]       cte_conn_req_en: pointer to User defined cte_conn_req_en data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_connection_cte_request_enable(esp_ble_cte_req_en_params_t *cte_conn_req_en);

/**
 * @brief           This function is called to request the Controller to respond to LL_CTE_REQ PDUs with
 *                  LL_CTE_RSP PDUs on the specified connection.
 *
 * @param[in]       cte_conn_rsp_en: pointer to User defined cte_conn_rsp_en data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_connection_cte_response_enable(esp_ble_cte_rsp_en_params_t *cte_conn_rsp_en);

/**
 * @brief           This function is called to read the parameters of a transmitted CTE supported by the Controller.
 *
 * @param[in]       none.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_cte_read_antenna_information(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BLE_CTE_API_H__ */
