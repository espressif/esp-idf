/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_BLE_ISO_COMMON_API_H_
#define ESP_BLE_ISO_COMMON_API_H_

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "sdkconfig.h"
#include "esp_err.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/iso.h>

#include <../host/conn_internal.h>

#include "common/host.h"
#include "common/app/gap.h"
#include "common/app/gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert BIS index to bit
 *
 * The BIS indexes start from 0x01, so the lowest allowed bit is
 * BIT(0) that represents index 0x01. To synchronize to e.g. BIS
 * indexes 0x01 and 0x02, the bitfield value should be BIT(0) | BIT(1).
 * As a general notation, to sync to BIS index N use BIT(N - 1).
 *
 * @param   x               BIS index (1-based).
 */
#define ESP_BLE_ISO_BIS_INDEX_BIT(x)        BT_ISO_BIS_INDEX_BIT(x)

/*!< The minimum value for vendor specific data path ID */
#define ESP_BLE_ISO_DATA_PATH_VS_ID_MIN     BT_ISO_DATA_PATH_VS_ID_MIN
/*!< The maximum value for vendor specific data path ID */
#define ESP_BLE_ISO_DATA_PATH_VS_ID_MAX     BT_ISO_DATA_PATH_VS_ID_MAX

/*!< Minimum controller delay in microseconds */
#define ESP_BLE_ISO_CONTROLLER_DELAY_MIN    BT_ISO_CONTROLLER_DELAY_MIN
/*!< Maximum controller delay in microseconds */
#define ESP_BLE_ISO_CONTROLLER_DELAY_MAX    BT_ISO_CONTROLLER_DELAY_MAX

/*!< Unknown SDU interval */
#define ESP_BLE_ISO_SDU_INTERVAL_UNKNOWN    BT_ISO_SDU_INTERVAL_UNKNOWN
/*!< Minimum interval value in microseconds */
#define ESP_BLE_ISO_SDU_INTERVAL_MIN        BT_ISO_SDU_INTERVAL_MIN
/*!< Maximum interval value in microseconds */
#define ESP_BLE_ISO_SDU_INTERVAL_MAX        BT_ISO_SDU_INTERVAL_MAX

/*!< Minimum ISO interval (N * 1.25 ms) */
#define ESP_BLE_ISO_ISO_INTERVAL_MIN        BT_ISO_ISO_INTERVAL_MIN
/*!< Maximum ISO interval (N * 1.25 ms) */
#define ESP_BLE_ISO_ISO_INTERVAL_MAX        BT_ISO_ISO_INTERVAL_MAX

/*!< Minimum latency value in milliseconds */
#define ESP_BLE_ISO_LATENCY_MIN             BT_ISO_LATENCY_MIN
/*!< Maximum latency value in milliseconds */
#define ESP_BLE_ISO_LATENCY_MAX             BT_ISO_LATENCY_MAX

/*!< Packets will be sent sequentially between the channels in the group */
#define ESP_BLE_ISO_PACKING_SEQUENTIAL      BT_ISO_PACKING_SEQUENTIAL
/*!< Packets will be sent interleaved between the channels in the group */
#define ESP_BLE_ISO_PACKING_INTERLEAVED     BT_ISO_PACKING_INTERLEAVED

/*!< Packets may be framed or unframed */
#define ESP_BLE_ISO_FRAMING_UNFRAMED        BT_ISO_FRAMING_UNFRAMED
/*!< Packets are always framed */
#define ESP_BLE_ISO_FRAMING_FRAMED          BT_ISO_FRAMING_FRAMED

/*!< Maximum number of isochronous channels in a single group */
#define ESP_BLE_ISO_MAX_GROUP_ISO_COUNT     BT_ISO_MAX_GROUP_ISO_COUNT

/*!< Minimum SDU size */
#define ESP_BLE_ISO_MIN_SDU                 BT_ISO_MIN_SDU
/*!< Maximum SDU size */
#define ESP_BLE_ISO_MAX_SDU                 BT_ISO_MAX_SDU

/*!< Minimum PDU size */
#define ESP_BLE_ISO_CONNECTED_PDU_MIN       BT_ISO_CONNECTED_PDU_MIN
/*!< Minimum PDU size */
#define ESP_BLE_ISO_BROADCAST_PDU_MIN       BT_ISO_BROADCAST_PDU_MIN
/*!< Maximum PDU size */
#define ESP_BLE_ISO_PDU_MAX                 BT_ISO_PDU_MAX

/*!< Minimum burst number */
#define ESP_BLE_ISO_BN_MIN                  BT_ISO_BN_MIN
/*!< Maximum burst number */
#define ESP_BLE_ISO_BN_MAX                  BT_ISO_BN_MAX

/*!< Minimum flush timeout */
#define ESP_BLE_ISO_FT_MIN                  BT_ISO_FT_MIN
/*!< Maximum flush timeout */
#define ESP_BLE_ISO_FT_MAX                  BT_ISO_FT_MAX

/*!< Minimum number of subevents */
#define ESP_BLE_ISO_NSE_MIN                 BT_ISO_NSE_MIN
/*!< Maximum number of subevents */
#define ESP_BLE_ISO_NSE_MAX                 BT_ISO_NSE_MAX

/*!< Minimum BIG sync timeout value (N * 10 ms) */
#define ESP_BLE_ISO_SYNC_TIMEOUT_MIN        BT_ISO_SYNC_TIMEOUT_MIN
/*!< Maximum BIG sync timeout value (N * 10 ms) */
#define ESP_BLE_ISO_SYNC_TIMEOUT_MAX        BT_ISO_SYNC_TIMEOUT_MAX

/*!< Controller controlled maximum subevent count value */
#define ESP_BLE_ISO_SYNC_MSE_ANY            BT_ISO_SYNC_MSE_ANY
/*!< Minimum BIG sync maximum subevent count value */
#define ESP_BLE_ISO_SYNC_MSE_MIN            BT_ISO_SYNC_MSE_MIN
/*!< Maximum BIG sync maximum subevent count value */
#define ESP_BLE_ISO_SYNC_MSE_MAX            BT_ISO_SYNC_MSE_MAX

/*!< Minimum connected ISO retransmission value */
#define ESP_BLE_ISO_CONNECTED_RTN_MIN       BT_ISO_CONNECTED_RTN_MIN
/*!< Maximum connected ISO retransmission value */
#define ESP_BLE_ISO_CONNECTED_RTN_MAX       BT_ISO_CONNECTED_RTN_MAX
/*!< Minimum broadcast ISO retransmission value */
#define ESP_BLE_ISO_BROADCAST_RTN_MIN       BT_ISO_BROADCAST_RTN_MIN
/*!< Maximum broadcast ISO retransmission value */
#define ESP_BLE_ISO_BROADCAST_RTN_MAX       BT_ISO_BROADCAST_RTN_MAX

/*!< Broadcast code size */
#define ESP_BLE_ISO_BROADCAST_CODE_SIZE     BT_ISO_BROADCAST_CODE_SIZE

/*!< Lowest BIS index */
#define ESP_BLE_ISO_BIS_INDEX_MIN           BT_ISO_BIS_INDEX_MIN
/*!< Highest BIS index */
#define ESP_BLE_ISO_BIS_INDEX_MAX           BT_ISO_BIS_INDEX_MAX

/*!< Minimum Immediate Repetition Count */
#define ESP_BLE_ISO_IRC_MIN                 BT_ISO_IRC_MIN
/*!< Maximum Immediate Repetition Count */
#define ESP_BLE_ISO_IRC_MAX                 BT_ISO_IRC_MAX

/*!< Minimum pre-transmission offset */
#define ESP_BLE_ISO_PTO_MIN                 BT_ISO_PTO_MIN
/*!< Maximum pre-transmission offset */
#define ESP_BLE_ISO_PTO_MAX                 BT_ISO_PTO_MAX

/*!< No subinterval */
#define ESP_BLE_ISO_SUBINTERVAL_NONE        BT_ISO_SUBINTERVAL_NONE
/*!< Unknown subinterval */
#define ESP_BLE_ISO_SUBINTERVAL_UNKNOWN     BT_ISO_SUBINTERVAL_UNKNOWN
/*!< Minimum subinterval in microseconds */
#define ESP_BLE_ISO_SUBINTERVAL_MIN         BT_ISO_SUBINTERVAL_MIN
/*!< Maximum subinterval in microseconds */
#define ESP_BLE_ISO_SUBINTERVAL_MAX         BT_ISO_SUBINTERVAL_MAX

/*!< Audio Input (Host to Controller) Data Path Direction */
#define ESP_BLE_ISO_DATA_PATH_DIR_INPUT     BT_HCI_DATAPATH_DIR_HOST_TO_CTLR
/*!< Audio Output (Controller to Host) Data Path Direction */
#define ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT    BT_HCI_DATAPATH_DIR_CTLR_TO_HOST

/*!< Value to set the ISO data path over HCI */
#define ESP_BLE_ISO_DATA_PATH_HCI           BT_ISO_DATA_PATH_HCI

/*!< Transparent Coding Format */
#define ESP_BLE_ISO_CODING_FORMAT_TRANSPARENT   BT_HCI_CODING_FORMAT_TRANSPARENT
/*!< LC3 Coding Format */
#define ESP_BLE_ISO_CODING_FORMAT_LC3           BT_HCI_CODING_FORMAT_LC3

/*!< The ISO packet is valid */
#define ESP_BLE_ISO_FLAGS_VALID             BT_ISO_FLAGS_VALID
/*!< The ISO packet may possibly contain errors */
#define ESP_BLE_ISO_FLAGS_ERROR             BT_ISO_FLAGS_ERROR
/*!< The ISO packet was lost */
#define ESP_BLE_ISO_FLAGS_LOST              BT_ISO_FLAGS_LOST
/*!< Timestamp is valid */
#define ESP_BLE_ISO_FLAGS_TS                BT_ISO_FLAGS_TS

/*!< LE 1M PHY */
#define ESP_BLE_ISO_PHY_1M                  BT_GAP_LE_PHY_1M
/*!< LE 2M PHY */
#define ESP_BLE_ISO_PHY_2M                  BT_GAP_LE_PHY_2M
/*!< LE Coded PHY, coding scheme not specified */
#define ESP_BLE_ISO_PHY_CODED               BT_GAP_LE_PHY_CODED

/*!< Unknown */
#define ESP_BLE_ISO_SCA_UNKNOWN             BT_GAP_SCA_UNKNOWN
/*!< 251 ppm to 500 ppm */
#define ESP_BLE_ISO_SCA_251_500             BT_GAP_SCA_251_500
/*!< 151 ppm to 250 ppm */
#define ESP_BLE_ISO_SCA_151_250             BT_GAP_SCA_151_250
/*!< 101 ppm to 150 ppm */
#define ESP_BLE_ISO_SCA_101_150             BT_GAP_SCA_101_150
/*!< 76 ppm to 100 ppm */
#define ESP_BLE_ISO_SCA_76_100              BT_GAP_SCA_76_100
/*!< 51 ppm to 75 ppm */
#define ESP_BLE_ISO_SCA_51_75               BT_GAP_SCA_51_75
/*!< 31 ppm to 50 ppm */
#define ESP_BLE_ISO_SCA_31_50               BT_GAP_SCA_31_50
/*!< 21 ppm to 30 ppm */
#define ESP_BLE_ISO_SCA_21_30               BT_GAP_SCA_21_30
/*!< 0 ppm to 20 ppm */
#define ESP_BLE_ISO_SCA_0_20                BT_GAP_SCA_0_20

/*!< Security Level 1: No encryption and no authentication */
#define ESP_BLE_ISO_SECURITY_NONE           BT_SECURITY_L1
/*!< Security Level 2: Encryption and no authentication (no MITM) */
#define ESP_BLE_ISO_SECURITY_NO_MITM        BT_SECURITY_L2
/*!< Security Level 3: Encryption and authentication (MITM) */
#define ESP_BLE_ISO_SECURITY_MITM           BT_SECURITY_L3

/** Connection structure. */
typedef struct bt_conn                      esp_ble_conn_t;

/** ISO Channel structure. */
typedef struct bt_iso_chan                  esp_ble_iso_chan_t;

/** ISO Channel operations structure. */
typedef struct bt_iso_chan_ops              esp_ble_iso_chan_ops_t;

/** ISO Channel QoS structure. */
typedef struct bt_iso_chan_qos              esp_ble_iso_chan_qos_t;

/** ISO Channel IO QoS structure. */
typedef struct bt_iso_chan_io_qos           esp_ble_iso_chan_io_qos_t;

/** ISO Channel Data Path structure. */
typedef struct bt_iso_chan_path             esp_ble_iso_chan_path_t;

/** Connected Isochronous Group (CIG). */
typedef struct bt_iso_cig                   esp_ble_iso_cig_t;

/** Connected Isochronous Group (CIG) parameters. */
typedef struct bt_iso_cig_param             esp_ble_iso_cig_param_t;

/** ISO connection parameters structure. */
typedef struct bt_iso_connect_param         esp_ble_iso_connect_param_t;

/** ISO Accept Info Structure. */
typedef struct bt_iso_accept_info           esp_ble_iso_accept_info_t;

/** ISO Server structure. */
typedef struct bt_iso_server                esp_ble_iso_server_t;

/** ISO Unicast Info Structure. */
typedef struct bt_iso_unicast_info          esp_ble_iso_unicast_info_t;

/** ISO Unicast TX Info Structure. */
typedef struct bt_iso_unicast_tx_info       esp_ble_iso_unicast_tx_info_t;

/** ISO Broadcast callbacks */
typedef struct bt_iso_big_cb                esp_ble_iso_big_cb_t;

/** Broadcast Isochronous Group (BIG). */
typedef struct bt_iso_big                   esp_ble_iso_big_t;

/** Broadcast Isochronous Group (BIG) creation parameters. */
typedef struct bt_iso_big_create_param      esp_ble_iso_big_create_param_t;

/** Broadcast Isochronous Group (BIG) Sync Parameters. */
typedef struct bt_iso_big_sync_param        esp_ble_iso_big_sync_param_t;

/** Broadcast Isochronous Group (BIG) information. */
typedef struct bt_iso_biginfo               esp_ble_iso_biginfo_t;

/** ISO Broadcaster Info Structure. */
typedef struct bt_iso_broadcaster_info      esp_ble_iso_broadcaster_info_t;

/** ISO Synchronized Receiver Info Structure. */
typedef struct bt_iso_sync_receiver_info    esp_ble_iso_sync_receiver_info_t;

/** ISO channel Info Structure. */
typedef struct bt_iso_info                  esp_ble_iso_info_t;

/** ISO Meta Data structure for received ISO packets. */
typedef struct bt_iso_recv_info             esp_ble_iso_recv_info_t;

/** ISO Meta Data structure for transmitted ISO packets. */
typedef struct bt_iso_tx_info               esp_ble_iso_tx_info_t;

/** ISO Unicast & Broadcast TX Callback Info Structure. */
typedef struct bt_iso_tx_cb_info            esp_ble_iso_tx_cb_info_t;

/** Extended advertising information structure */
typedef struct {
    uint8_t adv_handle; /*!< Handle for the advertising set */
} esp_ble_iso_ext_adv_info_t;

/**
 * @brief   Helper for parsing length-type-value data.
 *
 * @param   ltv         Length-type-value (LTV) encoded data.
 * @param   size        Size of the @p ltv data.
 * @param   func        Callback function which will be called for each element
 *                      that's found in the data. The callback should return
 *                      true to continue parsing, or false to stop parsing.
 * @param   user_data   User data to be passed to the callback.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_data_parse(const uint8_t ltv[], size_t size,
                                 bool (*func)(uint8_t type,
                                              const uint8_t *data,
                                              uint8_t data_len,
                                              void *user_data),
                                 void *user_data);

/**
 * @brief   Register an ISO server.
 *
 * @param   server  Pointer to the ISO server structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_server_register(esp_ble_iso_server_t *server);

/**
 * @brief   Unregister an ISO server.
 *
 * @param   server  Pointer to the ISO server structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_server_unregister(esp_ble_iso_server_t *server);

/**
 * @brief   Create a Connected ISO Group (CIG).
 *
 * @param   param   Parameters for CIG creation.
 * @param   out_cig Pointer to store created CIG.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_cig_create(esp_ble_iso_cig_param_t *param,
                                 esp_ble_iso_cig_t **out_cig);

/**
 * @brief   Reconfigure a Connected ISO Group.
 *
 * @param   cig     Pointer to the CIG to reconfigure.
 * @param   param   New parameters for the CIG.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_cig_reconfigure(esp_ble_iso_cig_t *cig,
                                      esp_ble_iso_cig_param_t *param);

/**
 * @brief   Terminate a Connected ISO Group.
 *
 * @param   cig Pointer to the CIG to terminate.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_cig_terminate(esp_ble_iso_cig_t *cig);

/**
 * @brief   Connect ISO channels.
 *
 * @param   param       Connection parameters.
 * @param   conn_handle Connection handle.
 * @param   count       Number of channels to connect.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_chan_connect(esp_ble_iso_connect_param_t *param,
                                   uint16_t conn_handle, size_t count);

/**
 * @brief   Disconnect an ISO channel.
 *
 * @param   chan    Pointer to the ISO channel.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_chan_disconnect(esp_ble_iso_chan_t *chan);

/**
 * @brief   Set up the ISO data path for an ISO channel.
 *
 * The channel must be associated with a BIS or CIS handle first which it is when the
 * esp_ble_iso_chan_ops_t.connected() callback is called.
 *
 * @param   chan    The channel to setup the ISO data path for.
 * @param   dir     The direction to setup, either ESP_BLE_ISO_DATA_PATH_DIR_INPUT
 *                  or ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT.
 *                  For ISO broadcast channels this can only be
 *                  ESP_BLE_ISO_DATA_PATH_DIR_INPUT, and for ISO sync receiver
 *                  channels this can only be ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT.
 * @param   path    The data path.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_setup_data_path(const esp_ble_iso_chan_t *chan, uint8_t dir,
                                      const esp_ble_iso_chan_path_t *path);

/**
 * @brief   Removes the ISO data path for an ISO channel.
 *
 * Removes the ISO data path configured by esp_ble_iso_setup_data_path() for the provided
 * @p dir.
 *
 * The data paths of CIS for Peripherals are deleted by the controller, and thus it is not
 * necessary (or possible) to remove data paths of CIS after they have disconnected for a
 * Peripheral, as per Bluetooth Core specification 6.0, Vol 4, Part E, Section 7.7.5.
 * The data paths for CIS for a Central remain valid, even after a disconnection, and thus
 * a Central device should call bt_iso_remove_data_path() on disconnect if it no longer
 * wants to use that CIS.
 * All data paths created by a Central are removed when the CIG is removed with
 * esp_ble_iso_cig_terminate().
 *
 * Any data paths associated with an ISO Sync Receiver BIG are removed by the controller
 * when the BIG sync is lost or terminated, and thus it is not necessary (or possible) to
 * remove data paths of ISO channels associated with a BIG for a Sync Receiver, as per
 * Bluetooth Core specification 6.0, Vol 4, Part E, Section 7.7.65.30.
 *
 * All data paths associated with an ISO Broadcaster BIG are removed when the BIG is
 * terminated by esp_ble_iso_big_terminate(), and thus it is not necessary (or possible)
 * to remove data paths of ISO channels associated with a BIG for a Broadcaster,
 * as per Bluetooth Core specification 6.0, Vol 4, Part E, Section 7.8.105.
 *
 * @param   chan    The channel to setup the ISO data path for.
 * @param   dir     The direction to setup, either ESP_BLE_ISO_DATA_PATH_DIR_INPUT or
 *                  ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT. For ISO broadcast channels this
 *                  can only be ESP_BLE_ISO_DATA_PATH_DIR_INPUT, and for ISO sync
 *                  receiver channels this can only be ESP_BLE_ISO_DATA_PATH_DIR_OUTPUT.

 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_remove_data_path(const esp_ble_iso_chan_t *chan, uint8_t dir);

/**
 * @brief   Registers callbacks for Broadcast Sources.
 *
 * @param   cb  Pointer to the callback structure.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_big_register_cb(esp_ble_iso_big_cb_t *cb);

/**
 * @brief   Add extended advertising for BIG.
 *
 * @param   info    Extended advertising information.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_big_ext_adv_add(esp_ble_iso_ext_adv_info_t *info);

/**
 * @brief   Delete extended advertising for BIG.
 *
 * @param   info    Extended advertising information.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_big_ext_adv_delete(esp_ble_iso_ext_adv_info_t *info);

/**
 * @brief   Create a Broadcast ISO Group (BIG).
 *
 * @param   adv_handle  Advertising handle.
 * @param   param       Parameters for BIG creation.
 * @param   out_big     Pointer to store created BIG.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_big_create(uint8_t adv_handle,
                                 esp_ble_iso_big_create_param_t *param,
                                 esp_ble_iso_big_t **out_big);

/**
 * @brief   Synchronize with a Broadcast ISO Group.
 *
 * @param   sync_handle Sync handle.
 * @param   param       Sync parameters.
 * @param   out_big     Pointer to store synchronized BIG.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_big_sync(uint16_t sync_handle,
                               esp_ble_iso_big_sync_param_t *param,
                               esp_ble_iso_big_t **out_big);

/**
 * @brief   Terminate a Broadcast ISO Group.
 *
 * @param   big Pointer to the BIG to terminate.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_big_terminate(esp_ble_iso_big_t *big);

/**
 * @brief   Get ISO channel information.
 *
 * @param   chan    Pointer to the ISO channel.
 * @param   info    Pointer to store channel information.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_chan_get_info(esp_ble_iso_chan_t *chan,
                                    esp_ble_iso_info_t *info);

/**
 * @brief   Get TX sync information for an ISO channel.
 *
 * @param   chan    Pointer to the ISO channel.
 * @param   info    Pointer to store TX sync information.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_chan_get_tx_sync(esp_ble_iso_chan_t *chan,
                                       esp_ble_iso_tx_info_t *info);

/**
 * @brief   Send data over an ISO channel.
 *
 * @param   chan    Pointer to the ISO channel.
 * @param   sdu     Pointer to the SDU data.
 * @param   sdu_len Length of the SDU.
 * @param   seq_num Sequence number.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_chan_send(esp_ble_iso_chan_t *chan,
                                const uint8_t *sdu,
                                uint16_t sdu_len,
                                uint16_t seq_num);

/**
 * @brief   Send timestamped data over an ISO channel.
 *
 * @param   chan    Pointer to the ISO channel.
 * @param   sdu     Pointer to the SDU data.
 * @param   sdu_len Length of the SDU.
 * @param   seq_num Sequence number.
 * @param   ts      Timestamp.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_chan_send_ts(esp_ble_iso_chan_t *chan,
                                   const uint8_t *sdu,
                                   uint16_t sdu_len,
                                   uint16_t seq_num,
                                   uint32_t ts);

/*!< ISO GAP Extended Scan Recv event */
#define ESP_BLE_ISO_GAP_EVENT_EXT_SCAN_RECV     BT_LE_GAP_APP_EVENT_EXT_SCAN_RECV
/*!< ISO GAP Periodic Sync Established event */
#define ESP_BLE_ISO_GAP_EVENT_PA_SYNC           BT_LE_GAP_APP_EVENT_PA_SYNC
/*!< ISO GAP Periodic Sync Transfer Received event */
#define ESP_BLE_ISO_GAP_EVENT_PA_SYNC_PAST      BT_LE_GAP_APP_EVENT_PA_SYNC_PAST
/*!< ISO GAP Periodic Sync Lost event */
#define ESP_BLE_ISO_GAP_EVENT_PA_SYNC_LOST      BT_LE_GAP_APP_EVENT_PA_SYNC_LOST
/*!< ISO GAP Connection Complete event */
#define ESP_BLE_ISO_GAP_EVENT_ACL_CONNECT       BT_LE_GAP_APP_EVENT_ACL_CONNECT
/*!< ISO GAP Disconnection Complete event */
#define ESP_BLE_ISO_GAP_EVENT_ACL_DISCONNECT    BT_LE_GAP_APP_EVENT_ACL_DISCONNECT
/*!< ISO GAP Security Changed event */
#define ESP_BLE_ISO_GAP_EVENT_SECURITY_CHANGE   BT_LE_GAP_APP_EVENT_SECURITY_CHANGE
/*!< ISO GAP BIGInfo Adv Report event */
#define ESP_BLE_ISO_GAP_EVENT_BIGINFO_RECV      BT_LE_GAP_APP_EVENT_BIGINFO_RECV
/** ISO GAP application event structure */
typedef struct bt_le_gap_app_event              esp_ble_iso_gap_app_event_t;

/** ISO initialization information structure */
typedef struct {
    bt_le_gap_app_cb gap_cb;    /*!< GAP event callbacks */
} esp_ble_iso_init_info_t;

/**
 * @brief   Post an application-layer GAP event for ISO internal usage.
 *
 * @note    This function is only needed while using NimBLE Host.
 *
 * @param   type    Event type.
 * @param   param   Event parameters.
 */
void esp_ble_iso_gap_app_post_event(uint8_t type, void *param);

/**
 * @brief   Initialize ISO common functionality.
 *
 * @param   info    Initialization information.
 *
 * @return  ESP_OK on success, or an error code on failure.
 */
esp_err_t esp_ble_iso_common_init(esp_ble_iso_init_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* ESP_BLE_ISO_COMMON_API_H_ */
