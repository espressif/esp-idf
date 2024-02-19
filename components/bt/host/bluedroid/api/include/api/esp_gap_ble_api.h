/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_GAP_BLE_API_H__
#define __ESP_GAP_BLE_API_H__

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BLE_ADV_DATA_FLAG data flag bit definition used for advertising data flag
 */
#define ESP_BLE_ADV_FLAG_LIMIT_DISC         (0x01 << 0)
#define ESP_BLE_ADV_FLAG_GEN_DISC           (0x01 << 1)
#define ESP_BLE_ADV_FLAG_BREDR_NOT_SPT      (0x01 << 2)
#define ESP_BLE_ADV_FLAG_DMT_CONTROLLER_SPT (0x01 << 3)
#define ESP_BLE_ADV_FLAG_DMT_HOST_SPT       (0x01 << 4)
#define ESP_BLE_ADV_FLAG_NON_LIMIT_DISC     (0x00 )


/// relate to BTM_LE_KEY_xxx in stack/btm_api.h
#define ESP_LE_KEY_NONE                    0           /*!< No encryption key */
#define ESP_LE_KEY_PENC                    (1 << 0)    /*!< encryption key, encryption information of peer device */
#define ESP_LE_KEY_PID                     (1 << 1)    /*!< identity key of the peer device */
#define ESP_LE_KEY_PCSRK                   (1 << 2)    /*!< peer SRK */
#define ESP_LE_KEY_PLK                     (1 << 3)    /*!< Link key*/
#define ESP_LE_KEY_LLK                     (ESP_LE_KEY_PLK << 4)      /*!< peer link key*/
#define ESP_LE_KEY_LENC                    (ESP_LE_KEY_PENC << 4)     /*!< master role security information:div */
#define ESP_LE_KEY_LID                     (ESP_LE_KEY_PID << 4)      /*!< master device ID key */
#define ESP_LE_KEY_LCSRK                   (ESP_LE_KEY_PCSRK << 4)    /*!< local CSRK has been deliver to peer */
typedef uint8_t esp_ble_key_type_t;

/// relate to BTM_LE_AUTH_xxx in stack/btm_api.h
#define ESP_LE_AUTH_NO_BOND                 0x00        /*!< 0  no bondingv*/
#define ESP_LE_AUTH_BOND                    0x01        /*!< 1 << 0 device in the bonding with peer */
#define ESP_LE_AUTH_REQ_MITM                (1 << 2)    /*!< 1 << 2 man in the middle attack */
#define ESP_LE_AUTH_REQ_BOND_MITM           (ESP_LE_AUTH_BOND | ESP_LE_AUTH_REQ_MITM) /*!< 0101 banding with man in the middle attack */
#define ESP_LE_AUTH_REQ_SC_ONLY             (1 << 3)                                  /*!< 1 << 3  secure connection */
#define ESP_LE_AUTH_REQ_SC_BOND             (ESP_LE_AUTH_BOND | ESP_LE_AUTH_REQ_SC_ONLY)            /*!< 1001 secure connection with band*/
#define ESP_LE_AUTH_REQ_SC_MITM             (ESP_LE_AUTH_REQ_MITM | ESP_LE_AUTH_REQ_SC_ONLY)        /*!< 1100 secure conn with MITM */
#define ESP_LE_AUTH_REQ_SC_MITM_BOND        (ESP_LE_AUTH_REQ_MITM | ESP_LE_AUTH_REQ_SC_ONLY | ESP_LE_AUTH_BOND)   /*!< 1101 SC with MITM and Bonding*/
typedef uint8_t   esp_ble_auth_req_t;         /*!< combination of the above bit pattern */

#define ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE 0    /*!< authentication disable*/
#define ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE  1    /*!< authentication enable*/

#define ESP_BLE_OOB_DISABLE 0    /*!< disbale the out of bond*/
#define ESP_BLE_OOB_ENABLE  1    /*!< enable the out of bond*/

/// relate to BTM_IO_CAP_xxx in stack/btm_api.h
#define ESP_IO_CAP_OUT                      0   /*!< DisplayOnly */
#define ESP_IO_CAP_IO                       1   /*!< DisplayYesNo */
#define ESP_IO_CAP_IN                       2   /*!< KeyboardOnly */
#define ESP_IO_CAP_NONE                     3   /*!< NoInputNoOutput */
#define ESP_IO_CAP_KBDISP                   4   /*!< Keyboard display */

#define ESP_BLE_APPEARANCE_UNKNOWN                 0x0000 /*!< relate to BTM_BLE_APPEARANCE_UNKNOWN in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_PHONE           0x0040 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_PHONE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_COMPUTER        0x0080 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_COMPUTER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_WATCH           0x00C0 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_WATCH in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_SPORTS_WATCH            0x00C1 /*!< relate to BTM_BLE_APPEARANCE_SPORTS_WATCH in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_CLOCK           0x0100 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_CLOCK in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_DISPLAY         0x0140 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_DISPLAY in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_REMOTE          0x0180 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_REMOTE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_EYEGLASSES      0x01C0 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_EYEGLASSES in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_TAG             0x0200 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_TAG in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_KEYRING         0x0240 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_KEYRING in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_MEDIA_PLAYER    0x0280 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_MEDIA_PLAYER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_BARCODE_SCANNER 0x02C0 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_BARCODE_SCANNER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_THERMOMETER     0x0300 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_THERMOMETER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_THERMOMETER_EAR         0x0301 /*!< relate to BTM_BLE_APPEARANCE_THERMOMETER_EAR in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_HEART_RATE      0x0340 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_HEART_RATE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HEART_RATE_BELT         0x0341 /*!< relate to BTM_BLE_APPEARANCE_HEART_RATE_BELT in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE  0x0380 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_BLOOD_PRESSURE_ARM      0x0381 /*!< relate to BTM_BLE_APPEARANCE_BLOOD_PRESSURE_ARM in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_BLOOD_PRESSURE_WRIST    0x0382 /*!< relate to BTM_BLE_APPEARANCE_BLOOD_PRESSURE_WRIST in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_HID             0x03C0 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_HID in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HID_KEYBOARD            0x03C1 /*!< relate to BTM_BLE_APPEARANCE_HID_KEYBOARD in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HID_MOUSE               0x03C2 /*!< relate to BTM_BLE_APPEARANCE_HID_MOUSE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HID_JOYSTICK            0x03C3 /*!< relate to BTM_BLE_APPEARANCE_HID_JOYSTICK in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HID_GAMEPAD             0x03C4 /*!< relate to BTM_BLE_APPEARANCE_HID_GAMEPAD in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HID_DIGITIZER_TABLET    0x03C5 /*!< relate to BTM_BLE_APPEARANCE_HID_DIGITIZER_TABLET in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HID_CARD_READER         0x03C6 /*!< relate to BTM_BLE_APPEARANCE_HID_CARD_READER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HID_DIGITAL_PEN         0x03C7 /*!< relate to BTM_BLE_APPEARANCE_HID_DIGITAL_PEN in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_HID_BARCODE_SCANNER     0x03C8 /*!< relate to BTM_BLE_APPEARANCE_HID_BARCODE_SCANNER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_GLUCOSE         0x0400 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_GLUCOSE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_WALKING         0x0440 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_WALKING in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_WALKING_IN_SHOE         0x0441 /*!< relate to BTM_BLE_APPEARANCE_WALKING_IN_SHOE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_WALKING_ON_SHOE         0x0442 /*!< relate to BTM_BLE_APPEARANCE_WALKING_ON_SHOE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_WALKING_ON_HIP          0x0443 /*!< relate to BTM_BLE_APPEARANCE_WALKING_ON_HIP in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_CYCLING         0x0480 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_CYCLING in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_CYCLING_COMPUTER        0x0481 /*!< relate to BTM_BLE_APPEARANCE_CYCLING_COMPUTER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_CYCLING_SPEED           0x0482 /*!< relate to BTM_BLE_APPEARANCE_CYCLING_SPEED in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_CYCLING_CADENCE         0x0483 /*!< relate to BTM_BLE_APPEARANCE_CYCLING_CADENCE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_CYCLING_POWER           0x0484 /*!< relate to BTM_BLE_APPEARANCE_CYCLING_POWER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_CYCLING_SPEED_CADENCE   0x0485 /*!< relate to BTM_BLE_APPEARANCE_CYCLING_SPEED_CADENCE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_PULSE_OXIMETER  0x0C40 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_PULSE_OXIMETER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP 0x0C41 /*!< relate to BTM_BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_PULSE_OXIMETER_WRIST    0x0C42 /*!< relate to BTM_BLE_APPEARANCE_PULSE_OXIMETER_WRIST in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_WEIGHT          0x0C80 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_WEIGHT in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE    0x0CC0 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_POWERED_WHEELCHAIR                  0x0CC1 /*!< relate to BTM_BLE_APPEARANCE_POWERED_WHEELCHAIR in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_MOBILITY_SCOOTER                    0x0CC2 /*!< relate to BTM_BLE_APPEARANCE_MOBILITY_SCOOTER in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_CONTINUOUS_GLUCOSE_MONITOR  0x0D00 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_CONTINUOUS_GLUCOSE_MONITOR in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_INSULIN_PUMP                0x0D40 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_INSULIN_PUMP in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_INSULIN_PUMP_DURABLE_PUMP           0x0D41 /*!< relate to BTM_BLE_APPEARANCE_INSULIN_PUMP_DURABLE_PUMP in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_INSULIN_PUMP_PATCH_PUMP             0x0D44 /*!< relate to BTM_BLE_APPEARANCE_INSULIN_PUMP_PATCH_PUMP in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_INSULIN_PEN                         0x0D48 /*!< relate to BTM_BLE_APPEARANCE_INSULIN_PEN in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_MEDICATION_DELIVERY         0x0D80 /*!< relate to BTM_BLE_APPEARANCE_GENERIC_MEDICATION_DELIVERY in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS  0x1440             /*!< relate to BTM_BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION 0x1441             /*!< relate to BTM_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV     0x1442 /*!< relate to BTM_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD         0x1443 /*!< relate to BTM_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD in stack/btm_ble_api.h */
#define ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV 0x1444 /*!< relate to BTM_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV in stack/btm_ble_api.h */

typedef uint8_t esp_ble_io_cap_t;               /*!< combination of the io capability */

#define    BLE_DTM_PKT_PAYLOAD_0x00   0x00       /*!< PRBS9 sequence ‘11111111100000111101...’ (in transmission order) as described in [Vol 6] Part F, Section 4.1.5 */
#define    BLE_DTM_PKT_PAYLOAD_0x01   0x01       /*!< Repeated ‘11110000’ (in transmission order) sequence as described in [Vol 6] Part F, Section 4.1.5 */
#define    BLE_DTM_PKT_PAYLOAD_0x02   0x02       /*!< Repeated ‘10101010’ (in transmission order) sequence as described in [Vol 6] Part F, Section 4.1.5 */
#define    BLE_DTM_PKT_PAYLOAD_0x03   0x03       /*!< PRBS15 sequence as described in [Vol 6] Part F, Section 4.1.5 */
#define    BLE_DTM_PKT_PAYLOAD_0x04   0x04       /*!< Repeated ‘11111111’ (in transmission order) sequence */
#define    BLE_DTM_PKT_PAYLOAD_0x05   0x05       /*!< Repeated ‘00000000’ (in transmission order) sequence */
#define    BLE_DTM_PKT_PAYLOAD_0x06   0x06       /*!< Repeated ‘00001111’ (in transmission order) sequence */
#define    BLE_DTM_PKT_PAYLOAD_0x07   0x07       /*!< Repeated ‘01010101’ (in transmission order) sequence */
#define    BLE_DTM_PKT_PAYLOAD_MAX    0x08       /*!< 0x08 ~ 0xFF, Reserved for future use */

typedef uint8_t esp_ble_dtm_pkt_payload_t;

/// GAP BLE callback event type
typedef enum {
    //BLE_42_FEATURE_SUPPORT
    ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT        = 0,       /*!< When advertising data set complete, the event comes */
    ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT,             /*!< When scan response data set complete, the event comes */
    ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT,                /*!< When scan parameters set complete, the event comes */
    ESP_GAP_BLE_SCAN_RESULT_EVT,                            /*!< When one scan result ready, the event comes each time */
    ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT,              /*!< When raw advertising data set complete, the event comes */
    ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT,         /*!< When raw scan response data set complete, the event comes */
    ESP_GAP_BLE_ADV_START_COMPLETE_EVT,                     /*!< When start advertising complete, the event comes */
    ESP_GAP_BLE_SCAN_START_COMPLETE_EVT,                    /*!< When start scan complete, the event comes */
    //BLE_INCLUDED
    ESP_GAP_BLE_AUTH_CMPL_EVT = 8,                          /*!< Authentication complete indication. */
    ESP_GAP_BLE_KEY_EVT,                                    /*!< BLE  key event for peer device keys */
    ESP_GAP_BLE_SEC_REQ_EVT,                                /*!< BLE  security request */
    ESP_GAP_BLE_PASSKEY_NOTIF_EVT,                          /*!< passkey notification event */
    ESP_GAP_BLE_PASSKEY_REQ_EVT,                            /*!< passkey request event */
    ESP_GAP_BLE_OOB_REQ_EVT,                                /*!< OOB request event */
    ESP_GAP_BLE_LOCAL_IR_EVT,                               /*!< BLE local IR (identity Root 128-bit random static value used to generate Long Term Key) event */
    ESP_GAP_BLE_LOCAL_ER_EVT,                               /*!< BLE local ER (Encryption Root vakue used to genrate identity resolving key) event */
    ESP_GAP_BLE_NC_REQ_EVT,                                 /*!< Numeric Comparison request event */
    //BLE_42_FEATURE_SUPPORT
    ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT,                      /*!< When stop adv complete, the event comes */
    ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT,                     /*!< When stop scan complete, the event comes */
    //BLE_INCLUDED
    ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT = 19,              /*!< When set the static rand address complete, the event comes */
    ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT,                     /*!< When update connection parameters complete, the event comes */
    ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT,                /*!< When set pkt length complete, the event comes */
    ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT,             /*!< When  Enable/disable privacy on the local device complete, the event comes */
    ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT,               /*!< When remove the bond device complete, the event comes */
    ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT,                /*!< When clear the bond device clear complete, the event comes */
    ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT,                  /*!< When get the bond device list complete, the event comes */
    ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT,                     /*!< When read the rssi complete, the event comes */
    ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT,              /*!< When add or remove whitelist complete, the event comes */
    //BLE_42_FEATURE_SUPPORT
    ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT,  /*!< When update duplicate exceptional list complete, the event comes */
    //BLE_INCLUDED
    ESP_GAP_BLE_SET_CHANNELS_EVT = 29,                           /*!< When setting BLE channels complete, the event comes */
    //BLE_50_FEATURE_SUPPORT
    ESP_GAP_BLE_READ_PHY_COMPLETE_EVT,                           /*!< when reading phy complete, this event comes */
    ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT,          /*!< when preferred default phy complete, this event comes */
    ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT,                  /*!< when preferred phy complete , this event comes */
    ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT,              /*!< when extended set random address complete, the event comes */
    ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT,                 /*!< when extended advertising parameter complete, the event comes */
    ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT,                   /*!< when extended advertising data complete, the event comes */
    ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT,              /*!< when extended scan response data complete, the event comes */
    ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT,                      /*!< when extended advertising start complete, the event comes */
    ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT,                       /*!< when extended advertising stop complete, the event comes */
    ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT,                 /*!< when extended advertising set remove complete, the event comes */
    ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT,                  /*!< when extended advertising set clear complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT,            /*!< when periodic advertising parameter complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT,              /*!< when periodic advertising data complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT,                 /*!< when periodic advertising start complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT,                  /*!< when periodic advertising stop complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT,           /*!< when periodic advertising create sync complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT,           /*!< when extended advertising sync cancel complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT,        /*!< when extended advertising sync terminate complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT,               /*!< when extended advertising add device complete , the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT,            /*!< when extended advertising remove device complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT,             /*!< when extended advertising clear device, the event comes */
    ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT,                /*!< when extended scan parameter complete, the event comes */
    ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT,                     /*!< when extended scan start complete, the event comes */
    ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT,                      /*!< when extended scan stop complete, the event comes */
    ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT,         /*!< when extended prefer connection parameter set complete, the event comes */
    ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT,                         /*!< when ble phy update complete, the event comes */
    ESP_GAP_BLE_EXT_ADV_REPORT_EVT,                              /*!< when extended advertising report complete, the event comes */
    ESP_GAP_BLE_SCAN_TIMEOUT_EVT,                                /*!< when scan timeout complete, the event comes */
    ESP_GAP_BLE_ADV_TERMINATED_EVT,                              /*!< when advertising terminate data complete, the event comes */
    ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT,                           /*!< when scan req received complete, the event comes */
    ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT,                    /*!< when channel select algorithm complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT,                         /*!< when periodic report advertising complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT,                      /*!< when periodic advertising sync lost complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT,                     /*!< when periodic advertising sync establish complete, the event comes */
    //BLE_INCLUDED
    ESP_GAP_BLE_SC_OOB_REQ_EVT,                                  /*!< Secure Connection OOB request event */
    ESP_GAP_BLE_SC_CR_LOC_OOB_EVT,                               /*!< Secure Connection create OOB data complete event */
    ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT,                       /*!< When getting BT device name complete, the event comes */
    //BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER
    ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT,           /*!< when set periodic advertising receive enable complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT,            /*!< when periodic advertising sync transfer complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT,        /*!< when periodic advertising set info transfer complete, the event comes */
    ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT,                    /*!< when set periodic advertising sync transfer params complete, the event comes */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_RECV_EVT,                /*!< when periodic advertising sync transfer received, the event comes */
    // DTM
    ESP_GAP_BLE_DTM_TEST_UPDATE_EVT,                             /*!< when direct test mode state changes, the event comes */
    // BLE_INCLUDED
    ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT,                          /*!< When clear advertising complete, the event comes */
    ESP_GAP_BLE_EVT_MAX,                                         /*!< when maximum advertising event complete, the event comes */
} esp_gap_ble_cb_event_t;

#define ESP_GAP_BLE_CHANNELS_LEN     5                           /*!< channel length*/
typedef uint8_t esp_gap_ble_channels[ESP_GAP_BLE_CHANNELS_LEN];

/// This is the old name, just for backwards compatibility
#define ESP_GAP_BLE_ADD_WHITELIST_COMPLETE_EVT ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT

/// Advertising data maximum length
#define ESP_BLE_ADV_DATA_LEN_MAX               31
/// Scan response data maximum length
#define ESP_BLE_SCAN_RSP_DATA_LEN_MAX          31

/* relate to BTM_BLE_AD_TYPE_xxx in stack/btm_ble_api.h */
/// The type of advertising data(not adv_type)
typedef enum {
    ESP_BLE_AD_TYPE_FLAG                     = 0x01,    /* relate to BTM_BLE_AD_TYPE_FLAG in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_16SRV_PART               = 0x02,    /* relate to BTM_BLE_AD_TYPE_16SRV_PART in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_16SRV_CMPL               = 0x03,    /* relate to BTM_BLE_AD_TYPE_16SRV_CMPL in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_32SRV_PART               = 0x04,    /* relate to BTM_BLE_AD_TYPE_32SRV_PART in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_32SRV_CMPL               = 0x05,    /* relate to BTM_BLE_AD_TYPE_32SRV_CMPL in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_128SRV_PART              = 0x06,    /* relate to BTM_BLE_AD_TYPE_128SRV_PART in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_128SRV_CMPL              = 0x07,    /* relate to BTM_BLE_AD_TYPE_128SRV_CMPL in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_NAME_SHORT               = 0x08,    /* relate to BTM_BLE_AD_TYPE_NAME_SHORT in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_NAME_CMPL                = 0x09,    /* relate to BTM_BLE_AD_TYPE_NAME_CMPL in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_TX_PWR                   = 0x0A,    /* relate to BTM_BLE_AD_TYPE_TX_PWR in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_DEV_CLASS                = 0x0D,    /* relate to BTM_BLE_AD_TYPE_DEV_CLASS in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_SM_TK                    = 0x10,    /* relate to BTM_BLE_AD_TYPE_SM_TK in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_SM_OOB_FLAG              = 0x11,    /* relate to BTM_BLE_AD_TYPE_SM_OOB_FLAG in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_INT_RANGE                = 0x12,    /* relate to BTM_BLE_AD_TYPE_INT_RANGE in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_SOL_SRV_UUID             = 0x14,    /* relate to BTM_BLE_AD_TYPE_SOL_SRV_UUID in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_128SOL_SRV_UUID          = 0x15,    /* relate to BTM_BLE_AD_TYPE_128SOL_SRV_UUID in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_SERVICE_DATA             = 0x16,    /* relate to BTM_BLE_AD_TYPE_SERVICE_DATA in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_PUBLIC_TARGET            = 0x17,    /* relate to BTM_BLE_AD_TYPE_PUBLIC_TARGET in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_RANDOM_TARGET            = 0x18,    /* relate to BTM_BLE_AD_TYPE_RANDOM_TARGET in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_APPEARANCE               = 0x19,    /* relate to BTM_BLE_AD_TYPE_APPEARANCE in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_ADV_INT                  = 0x1A,    /* relate to BTM_BLE_AD_TYPE_ADV_INT in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_LE_DEV_ADDR              = 0x1b,    /* relate to BTM_BLE_AD_TYPE_LE_DEV_ADDR in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_LE_ROLE                  = 0x1c,    /* relate to BTM_BLE_AD_TYPE_LE_ROLE in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_SPAIR_C256               = 0x1d,    /* relate to BTM_BLE_AD_TYPE_SPAIR_C256 in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_SPAIR_R256               = 0x1e,    /* relate to BTM_BLE_AD_TYPE_SPAIR_R256 in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_32SOL_SRV_UUID           = 0x1f,    /* relate to BTM_BLE_AD_TYPE_32SOL_SRV_UUID in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_32SERVICE_DATA           = 0x20,    /* relate to BTM_BLE_AD_TYPE_32SERVICE_DATA in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_128SERVICE_DATA          = 0x21,    /* relate to BTM_BLE_AD_TYPE_128SERVICE_DATA in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_LE_SECURE_CONFIRM        = 0x22,    /* relate to BTM_BLE_AD_TYPE_LE_SECURE_CONFIRM in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_LE_SECURE_RANDOM         = 0x23,    /* relate to BTM_BLE_AD_TYPE_LE_SECURE_RANDOM in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_URI                      = 0x24,    /* relate to BTM_BLE_AD_TYPE_URI in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_INDOOR_POSITION          = 0x25,    /* relate to BTM_BLE_AD_TYPE_INDOOR_POSITION in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_TRANS_DISC_DATA          = 0x26,    /* relate to BTM_BLE_AD_TYPE_TRANS_DISC_DATA in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_LE_SUPPORT_FEATURE       = 0x27,    /* relate to BTM_BLE_AD_TYPE_LE_SUPPORT_FEATURE in stack/btm_ble_api.h */
    ESP_BLE_AD_TYPE_CHAN_MAP_UPDATE          = 0x28,    /* relate to BTM_BLE_AD_TYPE_CHAN_MAP_UPDATE in stack/btm_ble_api.h */
    ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE    = 0xFF,    /* relate to BTM_BLE_AD_MANUFACTURER_SPECIFIC_TYPE in stack/btm_ble_api.h */
} esp_ble_adv_data_type;

/// Advertising mode
typedef enum {
    ADV_TYPE_IND                = 0x00,
    ADV_TYPE_DIRECT_IND_HIGH    = 0x01,
    ADV_TYPE_SCAN_IND           = 0x02,
    ADV_TYPE_NONCONN_IND        = 0x03,
    ADV_TYPE_DIRECT_IND_LOW     = 0x04,
} esp_ble_adv_type_t;

/// Advertising channel mask
typedef enum {
    ADV_CHNL_37     = 0x01,
    ADV_CHNL_38     = 0x02,
    ADV_CHNL_39     = 0x04,
    ADV_CHNL_ALL    = 0x07,
} esp_ble_adv_channel_t;

typedef enum {
    ///Allow both scan and connection requests from anyone
    ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY  = 0x00,
    ///Allow both scan req from White List devices only and connection req from anyone
    ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY,
    ///Allow both scan req from anyone and connection req from White List devices only
    ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST,
    ///Allow scan and connection requests from White List devices only
    ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST,
    ///Enumeration end value for advertising filter policy value check
} esp_ble_adv_filter_t;


/* relate to BTA_DM_BLE_SEC_xxx in bta/bta_api.h */
typedef enum {
    ESP_BLE_SEC_ENCRYPT = 1,            /*!< relate to BTA_DM_BLE_SEC_ENCRYPT in bta/bta_api.h. If the device has already
                                           bonded, the stack will used Long Term Key (LTK) to encrypt with the remote device directly.
                                           Else if the device hasn't bonded, the stack will used the default authentication request
                                           used the esp_ble_gap_set_security_param function set by the user. */
    ESP_BLE_SEC_ENCRYPT_NO_MITM,        /*!< relate to BTA_DM_BLE_SEC_ENCRYPT_NO_MITM in bta/bta_api.h. If the device has been already
                                           bonded, the stack will check the LTK (Long Term Key) Whether the authentication request has been met, and if met, use the LTK
                                           to encrypt with the remote device directly, else re-pair with the remote device.
                                           Else if the device hasn't been bonded, the stack will use NO MITM authentication request in the current link instead of
                                           using the authreq in the esp_ble_gap_set_security_param function set by the user. */
    ESP_BLE_SEC_ENCRYPT_MITM,           /*!< relate to BTA_DM_BLE_SEC_ENCRYPT_MITM in bta/bta_api.h. If the device has been already
                                           bonded, the stack will check the LTK (Long Term Key) whether the authentication request has been met, and if met, use the LTK
                                           to encrypt with the remote device directly, else re-pair with the remote device.
                                           Else if the device hasn't been bonded, the stack will use MITM authentication request in the current link instead of
                                           using the authreq in the esp_ble_gap_set_security_param function set by the user. */
}esp_ble_sec_act_t;

typedef enum {
    ESP_BLE_SM_PASSKEY = 0,
    /*!< Authentication requirements of local device */
    ESP_BLE_SM_AUTHEN_REQ_MODE,
    /*!< The IO capability of local device */
    ESP_BLE_SM_IOCAP_MODE,
    /*!< Initiator Key Distribution/Generation */
    ESP_BLE_SM_SET_INIT_KEY,
    /*!< Responder Key Distribution/Generation */
    ESP_BLE_SM_SET_RSP_KEY,
    /*!< Maximum Encryption key size to support */
    ESP_BLE_SM_MAX_KEY_SIZE,
    /*!< Minimum Encryption key size requirement from Peer */
    ESP_BLE_SM_MIN_KEY_SIZE,
    /*!< Set static Passkey */
    ESP_BLE_SM_SET_STATIC_PASSKEY,
    /*!< Reset static Passkey */
    ESP_BLE_SM_CLEAR_STATIC_PASSKEY,
    /*!< Accept only specified SMP Authentication requirement */
    ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH,
    /*!< Enable/Disable OOB support */
    ESP_BLE_SM_OOB_SUPPORT,
    /*!< Appl encryption key size */
    ESP_BLE_APP_ENC_KEY_SIZE,
    /*!< authentication max param */
    ESP_BLE_SM_MAX_PARAM,
} esp_ble_sm_param_t;

typedef enum {
    /// DTM TX start event
    DTM_TX_START_EVT  = 0x00,
    ///DTM RX start event
    DTM_RX_START_EVT,
    ///DTM test end event
    DTM_TEST_STOP_EVT,
} esp_ble_dtm_update_evt_t;

#if (BLE_42_FEATURE_SUPPORT == TRUE)
/**
* @brief DTM TX parameters
*/
typedef struct
{
    uint8_t                         tx_channel;            /*!<  channel for sending test data, tx_channel = (Frequency -2402)/2, tx_channel range:0x00-0x27, Frequency range: 2402 MHz to 2480 MHz */
    uint8_t                         len_of_data;           /*!<  length in bytes of payload data in each packet */
    esp_ble_dtm_pkt_payload_t       pkt_payload;           /*!<  packet payload type. value range: 0x00-0x07 */
} esp_ble_dtm_tx_t;
/**
* @brief DTM RX parameters
*/
typedef struct
{
    uint8_t          rx_channel;            /*!<  channel for test data reception, rx_channel = (Frequency -2402)/2, tx_channel range:0x00-0x27, Frequency range: 2402 MHz to 2480 MHz */
} esp_ble_dtm_rx_t;

/// Advertising parameters
typedef struct {
    uint16_t                adv_int_min;        /*!< Minimum advertising interval for
                                                  undirected and low duty cycle directed advertising.
                                                  Range: 0x0020 to 0x4000 Default: N = 0x0800 (1.28 second)
                                                  Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec */
    uint16_t                adv_int_max;        /*!< Maximum advertising interval for
                                                  undirected and low duty cycle directed advertising.
                                                  Range: 0x0020 to 0x4000 Default: N = 0x0800 (1.28 second)
                                                  Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec Advertising max interval */
    esp_ble_adv_type_t      adv_type;           /*!< Advertising type */
    esp_ble_addr_type_t     own_addr_type;      /*!< Owner bluetooth device address type */
    esp_bd_addr_t           peer_addr;          /*!< Peer device bluetooth device address */
    esp_ble_addr_type_t     peer_addr_type;     /*!< Peer device bluetooth device address type, only support public address type and random address type */
    esp_ble_adv_channel_t   channel_map;        /*!< Advertising channel map */
    esp_ble_adv_filter_t    adv_filter_policy;  /*!< Advertising filter policy */
} esp_ble_adv_params_t;

/// Advertising data content, according to "Supplement to the Bluetooth Core Specification"
typedef struct {
    bool                    set_scan_rsp;           /*!< Set this advertising data as scan response or not*/
    bool                    include_name;           /*!< Advertising data include device name or not */
    bool                    include_txpower;        /*!< Advertising data include TX power */
    int                     min_interval;           /*!< Advertising data show slave preferred connection min interval.
                                                    The connection interval in the following manner:
                                                    connIntervalmin = Conn_Interval_Min * 1.25 ms
                                                    Conn_Interval_Min range: 0x0006 to 0x0C80
                                                    Value of 0xFFFF indicates no specific minimum.
                                                    Values not defined above are reserved for future use.*/

    int                     max_interval;           /*!< Advertising data show slave preferred connection max interval.
                                                    The connection interval in the following manner:
                                                    connIntervalmax = Conn_Interval_Max * 1.25 ms
                                                    Conn_Interval_Max range: 0x0006 to 0x0C80
                                                    Conn_Interval_Max shall be equal to or greater than the Conn_Interval_Min.
                                                    Value of 0xFFFF indicates no specific maximum.
                                                    Values not defined above are reserved for future use.*/

    int                     appearance;             /*!< External appearance of device */
    uint16_t                manufacturer_len;       /*!< Manufacturer data length */
    uint8_t                 *p_manufacturer_data;   /*!< Manufacturer data point */
    uint16_t                service_data_len;       /*!< Service data length */
    uint8_t                 *p_service_data;        /*!< Service data point */
    uint16_t                service_uuid_len;       /*!< Service uuid length */
    uint8_t                 *p_service_uuid;        /*!< Service uuid array point */
    uint8_t                 flag;                   /*!< Advertising flag of discovery mode, see BLE_ADV_DATA_FLAG detail */
} esp_ble_adv_data_t;

#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)

/// Ble scan type
typedef enum {
    BLE_SCAN_TYPE_PASSIVE   =   0x0,            /*!< Passive scan */
    BLE_SCAN_TYPE_ACTIVE    =   0x1,            /*!< Active scan */
} esp_ble_scan_type_t;

/// Ble scan filter type
typedef enum {
    BLE_SCAN_FILTER_ALLOW_ALL           = 0x0,  /*!< Accept all :
                                                  1. advertisement packets except directed advertising packets not addressed to this device (default). */
    BLE_SCAN_FILTER_ALLOW_ONLY_WLST     = 0x1,  /*!< Accept only :
                                                  1. advertisement packets from devices where the advertiser’s address is in the White list.
                                                  2. Directed advertising packets which are not addressed for this device shall be ignored. */
    BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR   = 0x2,  /*!< Accept all :
                                                  1. undirected advertisement packets, and
                                                  2. directed advertising packets where the initiator address is a resolvable private address, and
                                                  3. directed advertising packets addressed to this device. */
    BLE_SCAN_FILTER_ALLOW_WLIST_RPA_DIR = 0x3,  /*!< Accept all :
                                                  1. advertisement packets from devices where the advertiser’s address is in the White list, and
                                                  2. directed advertising packets where the initiator address is a resolvable private address, and
                                                  3. directed advertising packets addressed to this device.*/
} esp_ble_scan_filter_t;

/// Ble scan duplicate type
typedef enum {
    BLE_SCAN_DUPLICATE_DISABLE           = 0x0,  /*!< the Link Layer should generate advertising reports to the host for each packet received */
    BLE_SCAN_DUPLICATE_ENABLE            = 0x1,  /*!< the Link Layer should filter out duplicate advertising reports to the Host */
    #if (BLE_50_FEATURE_SUPPORT == TRUE)
    BLE_SCAN_DUPLICATE_ENABLE_RESET,             /*!< Duplicate filtering enabled, reset for each scan period, only supported in BLE 5.0. */
    #endif
    BLE_SCAN_DUPLICATE_MAX                       /*!< Reserved for future use. */
} esp_ble_scan_duplicate_t;
#if (BLE_42_FEATURE_SUPPORT == TRUE)
/// Ble scan parameters
typedef struct {
    esp_ble_scan_type_t     scan_type;              /*!< Scan type */
    esp_ble_addr_type_t     own_addr_type;          /*!< Owner address type */
    esp_ble_scan_filter_t   scan_filter_policy;     /*!< Scan filter policy */
    uint16_t                scan_interval;          /*!< Scan interval. This is defined as the time interval from
                                                      when the Controller started its last LE scan until it begins the subsequent LE scan.
                                                      Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
                                                      Time = N * 0.625 msec
                                                      Time Range: 2.5 msec to 10.24 seconds*/
    uint16_t                scan_window;            /*!< Scan window. The duration of the LE scan. LE_Scan_Window
                                                      shall be less than or equal to LE_Scan_Interval
                                                      Range: 0x0004 to 0x4000 Default: 0x0010 (10 ms)
                                                      Time = N * 0.625 msec
                                                      Time Range: 2.5 msec to 10240 msec */
    esp_ble_scan_duplicate_t  scan_duplicate;       /*!< The Scan_Duplicates parameter controls whether the Link Layer should filter out
                                                        duplicate advertising reports (BLE_SCAN_DUPLICATE_ENABLE) to the Host, or if the Link Layer should generate
                                                        advertising reports for each packet received */
} esp_ble_scan_params_t;
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
/// connection parameters information
typedef struct {
    uint16_t             interval;                  /*!< connection interval */
    uint16_t             latency;                   /*!< Slave latency for the connection in number of connection events. Range: 0x0000 to 0x01F3 */
    uint16_t             timeout;                   /*!< Supervision timeout for the LE Link. Range: 0x000A to 0x0C80.
                                                      Mandatory Range: 0x000A to 0x0C80 Time = N * 10 msec
                                                      Time Range: 100 msec to 32 seconds */
} esp_gap_conn_params_t;

/// Connection update parameters
typedef struct {
    esp_bd_addr_t bda;                              /*!< Bluetooth device address */
    uint16_t min_int;                               /*!< Min connection interval */
    uint16_t max_int;                               /*!< Max connection interval */
    uint16_t latency;                               /*!< Slave latency for the connection in number of connection events. Range: 0x0000 to 0x01F3 */
    uint16_t timeout;                               /*!< Supervision timeout for the LE Link. Range: 0x000A to 0x0C80.
                                                      Mandatory Range: 0x000A to 0x0C80 Time = N * 10 msec
                                                      Time Range: 100 msec to 32 seconds */
} esp_ble_conn_update_params_t;

/**
* @brief BLE pkt date length keys
*/
typedef struct
{
    uint16_t rx_len;                   /*!< pkt rx data length value */
    uint16_t tx_len;                   /*!< pkt tx data length value */
} esp_ble_pkt_data_length_params_t;

/**
* @brief BLE encryption keys
*/
typedef struct
{
    esp_bt_octet16_t     ltk;          /*!< The long term key*/
    esp_bt_octet8_t      rand;         /*!< The random number*/
    uint16_t             ediv;         /*!< The ediv value*/
    uint8_t              sec_level;    /*!< The security level of the security link*/
    uint8_t              key_size;     /*!< The key size(7~16) of the security link*/
} esp_ble_penc_keys_t;                 /*!< The key type*/

/**
* @brief  BLE CSRK keys
*/
typedef struct
{
    uint32_t            counter;      /*!< The counter */
    esp_bt_octet16_t    csrk;         /*!< The csrk key */
    uint8_t             sec_level;    /*!< The security level */
} esp_ble_pcsrk_keys_t;               /*!< The pcsrk key type */

/**
* @brief  BLE pid keys
*/
typedef struct
{
    esp_bt_octet16_t          irk;           /*!< The irk value */
    esp_ble_addr_type_t       addr_type;     /*!< The address type */
    esp_bd_addr_t             static_addr;   /*!< The static address */
} esp_ble_pid_keys_t;                        /*!< The pid key type */

/**
* @brief  BLE Encryption reproduction keys
*/
typedef struct
{
    esp_bt_octet16_t  ltk;                  /*!< The long term key */
    uint16_t          div;                  /*!< The div value */
    uint8_t           key_size;             /*!< The key size of the security link */
    uint8_t           sec_level;            /*!< The security level of the security link */
} esp_ble_lenc_keys_t;                      /*!< The  key type */

/**
* @brief  BLE SRK keys
*/
typedef struct
{
    uint32_t          counter;              /*!< The counter value */
    uint16_t          div;                  /*!< The div value */
    uint8_t           sec_level;            /*!< The security level of the security link */
    esp_bt_octet16_t  csrk;                 /*!< The csrk key value */
} esp_ble_lcsrk_keys;                       /*!< The csrk key type */

/**
* @brief  Structure associated with ESP_KEY_NOTIF_EVT
*/
typedef struct
{
    esp_bd_addr_t  bd_addr;        /*!< peer address */
    uint32_t       passkey;        /*!< the numeric value for comparison. If just_works, do not show this number to UI */
} esp_ble_sec_key_notif_t;         /*!< BLE key notify type*/

/**
* @brief  Structure of the security request
*/
typedef struct
{
    esp_bd_addr_t  bd_addr;        /*!< peer address */
} esp_ble_sec_req_t;               /*!< BLE security request type*/

/**
* @brief  union type of the security key value
*/
typedef union
{
    esp_ble_penc_keys_t   penc_key;       /*!< received peer encryption key */
    esp_ble_pcsrk_keys_t  pcsrk_key;      /*!< received peer device SRK */
    esp_ble_pid_keys_t    pid_key;        /*!< peer device ID key */
    esp_ble_lenc_keys_t   lenc_key;       /*!< local encryption reproduction keys LTK = = d1(ER,DIV,0)*/
    esp_ble_lcsrk_keys    lcsrk_key;      /*!< local device CSRK = d1(ER,DIV,1)*/
} esp_ble_key_value_t;                    /*!< ble key value type*/

/**
* @brief  struct type of the bond key information value
*/
typedef struct
{
    esp_ble_key_mask_t    key_mask;       /*!< the key mask to indicate witch key is present */
    esp_ble_penc_keys_t   penc_key;       /*!< received peer encryption key */
    esp_ble_pcsrk_keys_t  pcsrk_key;      /*!< received peer device SRK */
    esp_ble_pid_keys_t    pid_key;        /*!< peer device ID key */
} esp_ble_bond_key_info_t;                /*!< ble bond key information value type */

/**
* @brief  struct type of the bond device value
*/
typedef struct
{
    esp_bd_addr_t  bd_addr;               /*!< peer address */
    esp_ble_bond_key_info_t bond_key;     /*!< the bond key information */
} esp_ble_bond_dev_t;                     /*!< the ble bond device type */


/**
* @brief  union type of the security key value
*/
typedef struct
{
    esp_bd_addr_t               bd_addr;        /*!< peer address */
    esp_ble_key_type_t          key_type;       /*!< key type of the security link */
    esp_ble_key_value_t         p_key_value;    /*!< the pointer to the key value */
} esp_ble_key_t;                                /*!< the union to the ble key value type*/

/**
* @brief  structure type of the ble local id keys value
*/
typedef struct {
    esp_bt_octet16_t       ir;                  /*!< the 16 bits of the ir value */
    esp_bt_octet16_t       irk;                 /*!< the 16 bits of the ir key value */
    esp_bt_octet16_t       dhk;                 /*!< the 16 bits of the dh key value */
} esp_ble_local_id_keys_t;                      /*!< the structure of the ble local id keys value type*/

/**
* @brief  structure type of the ble local oob data value
*/
typedef struct {
    esp_bt_octet16_t        oob_c;              /*!< the 128 bits of confirmation value */
    esp_bt_octet16_t        oob_r;              /*!< the 128 bits of randomizer value */
} esp_ble_local_oob_data_t;

/**
  * @brief Structure associated with ESP_AUTH_CMPL_EVT
  */
typedef struct
{
    esp_bd_addr_t         bd_addr;               /*!< BD address peer device. */
    bool                  key_present;           /*!< Valid link key value in key element */
    esp_link_key          key;                   /*!< Link key associated with peer device. */
    uint8_t               key_type;              /*!< The type of Link Key */
    bool                  success;               /*!< TRUE of authentication succeeded, FALSE if failed. */
    uint8_t               fail_reason;           /*!< The HCI reason/error code for when success=FALSE */
    esp_ble_addr_type_t   addr_type;             /*!< Peer device address type */
    esp_bt_dev_type_t     dev_type;              /*!< Device type */
    esp_ble_auth_req_t    auth_mode;             /*!< authentication mode */
} esp_ble_auth_cmpl_t;                           /*!< The ble authentication complete cb type */

/**
  * @brief union associated with ble security
  */
typedef union
{
    esp_ble_sec_key_notif_t    key_notif;      /*!< passkey notification */
    esp_ble_sec_req_t          ble_req;        /*!< BLE SMP related request */
    esp_ble_key_t              ble_key;        /*!< BLE SMP keys used when pairing */
    esp_ble_local_id_keys_t    ble_id_keys;    /*!< BLE IR event */
    esp_ble_local_oob_data_t   oob_data;       /*!< BLE SMP secure connection OOB data */
    esp_ble_auth_cmpl_t        auth_cmpl;      /*!< Authentication complete indication. */
} esp_ble_sec_t;                               /*!< BLE security type */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
/// Sub Event of ESP_GAP_BLE_SCAN_RESULT_EVT
typedef enum {
    ESP_GAP_SEARCH_INQ_RES_EVT             = 0,      /*!< Inquiry result for a peer device. */
    ESP_GAP_SEARCH_INQ_CMPL_EVT            = 1,      /*!< Inquiry complete. */
    ESP_GAP_SEARCH_DISC_RES_EVT            = 2,      /*!< Discovery result for a peer device. */
    ESP_GAP_SEARCH_DISC_BLE_RES_EVT        = 3,      /*!< Discovery result for BLE GATT based service on a peer device. */
    ESP_GAP_SEARCH_DISC_CMPL_EVT           = 4,      /*!< Discovery complete. */
    ESP_GAP_SEARCH_DI_DISC_CMPL_EVT        = 5,      /*!< Discovery complete. */
    ESP_GAP_SEARCH_SEARCH_CANCEL_CMPL_EVT  = 6,      /*!< Search cancelled */
    ESP_GAP_SEARCH_INQ_DISCARD_NUM_EVT     = 7,      /*!< The number of pkt discarded by flow control */
} esp_gap_search_evt_t;
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
/**
 * @brief Ble scan result event type, to indicate the
 *        result is scan response or advertising data or other
 */
typedef enum {
    ESP_BLE_EVT_CONN_ADV         = 0x00,        /*!< Connectable undirected advertising (ADV_IND) */
    ESP_BLE_EVT_CONN_DIR_ADV     = 0x01,        /*!< Connectable directed advertising (ADV_DIRECT_IND) */
    ESP_BLE_EVT_DISC_ADV         = 0x02,        /*!< Scannable undirected advertising (ADV_SCAN_IND) */
    ESP_BLE_EVT_NON_CONN_ADV     = 0x03,        /*!< Non connectable undirected advertising (ADV_NONCONN_IND) */
    ESP_BLE_EVT_SCAN_RSP         = 0x04,        /*!< Scan Response (SCAN_RSP) */
} esp_ble_evt_type_t;

typedef enum{
    ESP_BLE_WHITELIST_REMOVE     = 0X00,    /*!< remove mac from whitelist */
    ESP_BLE_WHITELIST_ADD        = 0X01,    /*!< add address to whitelist */
    ESP_BLE_WHITELIST_CLEAR      = 0x02,    /*!< clear all device in whitelist */
} esp_ble_wl_operation_t;
#if (BLE_42_FEATURE_SUPPORT == TRUE)
typedef enum {
    ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_ADD      = 0,  /*!< Add device info into duplicate scan exceptional list */
    ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_REMOVE,        /*!< Remove device info from duplicate scan exceptional list */
    ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_CLEAN,         /*!< Clean duplicate scan exceptional list */
} esp_bt_duplicate_exceptional_subcode_type_t;
#endif //#if (BLE_42_FEATURE_SUPPORT == TRUE)

#define BLE_BIT(n) (1UL<<(n))
#if (BLE_42_FEATURE_SUPPORT == TRUE)
typedef enum {
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_ADV_ADDR       = 0,  /*!< BLE advertising address , device info will be added into ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_ADDR_LIST */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_LINK_ID,        /*!< BLE mesh link ID, it is for BLE mesh, device info will be added into ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_LINK_ID_LIST */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_BEACON_TYPE,    /*!< BLE mesh beacon AD type, the format is | Len | 0x2B | Beacon Type | Beacon Data | */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROV_SRV_ADV,   /*!< BLE mesh provisioning service uuid, the format is | 0x02 | 0x01 | flags | 0x03 | 0x03 | 0x1827 | .... |` */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROXY_SRV_ADV,  /*!< BLE mesh adv with proxy service uuid, the format is | 0x02 | 0x01 | flags | 0x03 | 0x03 | 0x1828 | .... |` */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROXY_SOLIC_ADV,  /*!< BLE mesh adv with proxy service uuid, the format is | 0x02 | 0x01 | flags | 0x03 | 0x03 | 0x1859 | .... |` */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_URI_ADV,        /*!< BLE mesh URI adv, the format is ...| Len | 0x24 | data |... */
} esp_ble_duplicate_exceptional_info_type_t;

typedef enum {
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_ADDR_LIST                  = BLE_BIT(0),             /*!< duplicate scan exceptional addr list */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_LINK_ID_LIST          = BLE_BIT(1),             /*!< duplicate scan exceptional mesh link ID list */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_BEACON_TYPE_LIST      = BLE_BIT(2),             /*!< duplicate scan exceptional mesh beacon type list */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_PROV_SRV_ADV_LIST     = BLE_BIT(3),             /*!< duplicate scan exceptional mesh adv with provisioning service uuid */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_PROXY_SRV_ADV_LIST    = BLE_BIT(4),             /*!< duplicate scan exceptional mesh adv with proxy service uuid */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_PROXY_SOLIC_ADV_LIST  = BLE_BIT(5),             /*!< duplicate scan exceptional mesh adv with proxy solicitation PDU uuid */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_URI_ADV_LIST          = BLE_BIT(6),             /*!< duplicate scan exceptional URI list */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_ALL_LIST                   = 0xFFFF,                 /*!< duplicate scan exceptional all list */
} esp_duplicate_scan_exceptional_list_type_t;

typedef uint8_t esp_duplicate_info_t[ESP_BD_ADDR_LEN];

#endif //#if (BLE_42_FEATURE_SUPPORT == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED      (0 << 0) /*!< Non-Connectable and Non-Scannable Undirected advertising */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE                          (1 << 0) /*!< Connectable advertising */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE                            (1 << 1) /*!< Scannable advertising */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_DIRECTED                             (1 << 2) /*!< Directed advertising */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_HD_DIRECTED                          (1 << 3) /*!< High Duty Cycle Directed Connectable advertising (<= 3.75 ms Advertising Interval) */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY                               (1 << 4) /*!< Use legacy advertising PDUs */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_ANON_ADV                             (1 << 5) /*!< Omit advertiser's address from all PDUs ("anonymous advertising") */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_INCLUDE_TX_PWR                       (1 << 6) /*!< Include TxPower in the extended header of the advertising PDU */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_MASK                                 (0x7F)   /*!< Reserved for future use */

/*!<  If extended advertising PDU types are being used (bit 4 = 0) then:
    The advertisement shall not be both connectable and scannable.
    High duty cycle directed connectable advertising (<= 3.75 ms advertising interval) shall not be used (bit 3 = 0)
*/
/*!< ADV_IND */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_IND        (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE)
/*!< ADV_DIRECT_IND (low duty cycle) */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_LD_DIR     (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_DIRECTED)
/*!< ADV_DIRECT_IND (high duty cycle) */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_HD_DIR     (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_HD_DIRECTED)
/*!< ADV_SCAN_IND */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_SCAN       (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE)
/*!< ADV_NONCONN_IND */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_NONCONN    (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY)
typedef uint16_t esp_ble_ext_adv_type_mask_t;

#define ESP_BLE_GAP_PHY_1M                             1 /*!< Secondery Advertisement PHY is LE1M */
#define ESP_BLE_GAP_PHY_2M                             2 /*!< Secondery Advertisement PHY is LE2M */
#define ESP_BLE_GAP_PHY_CODED                          3 /*!< Secondery Advertisement PHY is LE Coded */
typedef uint8_t esp_ble_gap_phy_t;

#define ESP_BLE_GAP_NO_PREFER_TRANSMIT_PHY     (1<<0) /*!< No Prefer TX PHY supported by controller */
#define ESP_BLE_GAP_NO_PREFER_RECEIVE_PHY      (1<<1) /*!< No Prefer RX PHY supported by controller */
typedef uint8_t esp_ble_gap_all_phys_t;

/// Primary phy only support 1M and LE coded phy
#define ESP_BLE_GAP_PRI_PHY_1M     ESP_BLE_GAP_PHY_1M     /*!< Primary Phy is LE1M */
#define ESP_BLE_GAP_PRI_PHY_CODED  ESP_BLE_GAP_PHY_CODED  /*!< Primary Phy is LE CODED */
typedef uint8_t esp_ble_gap_pri_phy_t; // primary phy

#define ESP_BLE_GAP_PHY_1M_PREF_MASK                   (1 << 0) /*!< The Host prefers use the LE1M transmitter or reciever PHY */
#define ESP_BLE_GAP_PHY_2M_PREF_MASK                   (1 << 1) /*!< The Host prefers use the LE2M transmitter or reciever PHY */
#define ESP_BLE_GAP_PHY_CODED_PREF_MASK                (1 << 2) /*!< The Host prefers use the LE CODED transmitter or reciever PHY */
typedef uint8_t esp_ble_gap_phy_mask_t;

#define ESP_BLE_GAP_PHY_OPTIONS_NO_PREF                  0 /*!< The Host has no preferred coding when transmitting on the LE Coded PHY */
#define ESP_BLE_GAP_PHY_OPTIONS_PREF_S2_CODING           1 /*!< The Host prefers that S=2 coding be used when transmitting on the LE Coded PHY */
#define ESP_BLE_GAP_PHY_OPTIONS_PREF_S8_CODING           2 /*!< The Host prefers that S=8 coding be used when transmitting on the LE Coded PHY */
typedef uint16_t esp_ble_gap_prefer_phy_options_t;

#define ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK           0x01 /*!< Scan Advertisements on the LE1M PHY */
#define ESP_BLE_GAP_EXT_SCAN_CFG_CODE_MASK             0x02 /*!< Scan advertisements on the LE coded PHY */
typedef uint8_t esp_ble_ext_scan_cfg_mask_t;

/// Advertising data
#define ESP_BLE_GAP_EXT_ADV_DATA_COMPLETE              0x00 /*!< extended advertising data compete */
#define ESP_BLE_GAP_EXT_ADV_DATA_INCOMPLETE            0x01 /*!< extended advertising data incomplete */
#define ESP_BLE_GAP_EXT_ADV_DATA_TRUNCATED             0x02 /*!< extended advertising data truncated mode */
typedef uint8_t esp_ble_gap_ext_adv_data_status_t;

/// Advertising SYNC policy
#define ESP_BLE_GAP_SYNC_POLICY_BY_ADV_INFO       0 /*!< sync policy by advertising info */
#define ESP_BLE_GAP_SYNC_POLICY_BY_PERIODIC_LIST  1 /*!< periodic advertising sync policy */
typedef uint8_t esp_ble_gap_sync_t;

/// Advertising report
#define ESP_BLE_ADV_REPORT_EXT_ADV_IND                    (1<<0) /*!< advertising report with extended advertising indication type */
#define ESP_BLE_ADV_REPORT_EXT_SCAN_IND                   (1<<1) /*!< advertising report with extended scan indication type */
#define ESP_BLE_ADV_REPORT_EXT_DIRECT_ADV                 (1<<2) /*!< advertising report with extended direct advertising indication type */
#define ESP_BLE_ADV_REPORT_EXT_SCAN_RSP                   (1<<3) /*!< advertising report with extended scan response indication type */

/*!< Bluetooth 5.0, Vol 2, Part E, 7.7.65.13 */
#define ESP_BLE_LEGACY_ADV_TYPE_IND                       (0x13) /*!< advertising report with legacy advertising indication type */
#define ESP_BLE_LEGACY_ADV_TYPE_DIRECT_IND                (0x15) /*!< advertising report with legacy direct indication type */
#define ESP_BLE_LEGACY_ADV_TYPE_SCAN_IND                  (0x12) /*!< advertising report with legacy scan indication type */
#define ESP_BLE_LEGACY_ADV_TYPE_NONCON_IND                (0x10) /*!< advertising report with legacy non connectable indication type */
#define ESP_BLE_LEGACY_ADV_TYPE_SCAN_RSP_TO_ADV_IND       (0x1b) /*!< advertising report with legacy scan response indication type */
#define ESP_BLE_LEGACY_ADV_TYPE_SCAN_RSP_TO_ADV_SCAN_IND  (0x1a) /*!< advertising report with legacy advertising with scan response indication type */

typedef uint8_t esp_ble_gap_adv_type_t;

/// Extend advertising tx power, range: [-127, +126] dBm
#define EXT_ADV_TX_PWR_NO_PREFERENCE                      (127) /*!< host has no preference for tx power */

/**
* @brief ext adv parameters
*/
typedef struct {
    esp_ble_ext_adv_type_mask_t type;   /*!< ext adv type */
    uint32_t interval_min;              /*!< ext adv minimum interval */
    uint32_t interval_max;              /*!< ext adv maximum interval */
    esp_ble_adv_channel_t channel_map;  /*!< ext adv channel map */
    esp_ble_addr_type_t own_addr_type;  /*!< ext adv own address type */
    esp_ble_addr_type_t peer_addr_type; /*!< ext adv peer address type */
    esp_bd_addr_t peer_addr;            /*!< ext adv peer address */
    esp_ble_adv_filter_t filter_policy; /*!< ext adv filter policy */
    int8_t tx_power;                    /*!< ext adv tx power */
    esp_ble_gap_pri_phy_t primary_phy;  /*!< ext adv primary phy */
    uint8_t max_skip;                   /*!< ext adv maximum skip */
    esp_ble_gap_phy_t secondary_phy;    /*!< ext adv secondary phy */
    uint8_t sid;                        /*!< ext adv sid */
    bool scan_req_notif;                /*!< ext adv scan request event notify */
} esp_ble_gap_ext_adv_params_t;

/**
* @brief ext scan config
*/
typedef struct {
    esp_ble_scan_type_t scan_type; /*!< ext scan type */
    uint16_t scan_interval;        /*!< ext scan interval */
    uint16_t scan_window;          /*!< ext scan window */
} esp_ble_ext_scan_cfg_t;

/**
* @brief ext scan parameters
*/
typedef struct {
    esp_ble_addr_type_t own_addr_type;        /*!< ext scan own address type */
    esp_ble_scan_filter_t filter_policy;      /*!< ext scan filter policy */
    esp_ble_scan_duplicate_t  scan_duplicate; /*!< ext scan duplicate scan */
    esp_ble_ext_scan_cfg_mask_t cfg_mask;     /*!< ext scan config mask */
    esp_ble_ext_scan_cfg_t uncoded_cfg;       /*!< ext scan uncoded config parameters */
    esp_ble_ext_scan_cfg_t coded_cfg;         /*!< ext scan coded config parameters */
} esp_ble_ext_scan_params_t;

/**
* @brief create extend connection parameters
*/
typedef struct {
    uint16_t scan_interval;       /*!< init scan interval */
    uint16_t scan_window;         /*!< init scan window */
    uint16_t interval_min;        /*!< minimum interval */
    uint16_t interval_max;        /*!< maximum interval */
    uint16_t latency;             /*!< ext scan type */
    uint16_t supervision_timeout; /*!< connection supervision timeout */
    uint16_t min_ce_len;          /*!< minimum ce length */
    uint16_t max_ce_len;          /*!< maximum ce length */
} esp_ble_gap_conn_params_t;

/**
* @brief extend adv enable parameters
*/
typedef struct {
    uint8_t instance;        /*!< advertising handle */
    int duration;            /*!< advertising duration */
    int max_events;          /*!< maximum number of extended advertising events */
} esp_ble_gap_ext_adv_t;

/**
* @brief periodic adv parameters
*/
typedef struct {
    uint16_t interval_min;     /*!< periodic advertising minimum interval */
    uint16_t interval_max;     /*!< periodic advertising maximum interval */
    uint8_t  properties;       /*!< periodic advertising properties */
} esp_ble_gap_periodic_adv_params_t;

/**
* @brief periodic adv sync parameters
*/
typedef struct {
    esp_ble_gap_sync_t filter_policy;       /*!< Configures the filter policy for periodic advertising sync:
                                                 0: Use Advertising SID, Advertiser Address Type, and Advertiser Address parameters to determine the advertiser to listen to.
                                                 1: Use the Periodic Advertiser List to determine the advertiser to listen to. */
    #if (CONFIG_BT_BLE_FEAT_CREATE_SYNC_ENH)
    esp_ble_gap_sync_t reports_disabled;    /*!< Supported only by esp32c2, esp32c6, and esp32h2; can be set by menuconfig:
                                                 0: Reporting initially enabled.
                                                 1: Reporting initially disabled. */
    esp_ble_gap_sync_t filter_duplicates;   /*!< Supported only by esp32c2, esp32c6, and esp32h2; can be set by menuconfig:
                                                 0: Duplicate filtering initially disabled.
                                                 1: Duplicate filtering initially enabled. */
    #endif
    uint8_t sid;                            /*!< SID of the periodic advertising */
    esp_ble_addr_type_t addr_type;          /*!< Address type of the periodic advertising */
    esp_bd_addr_t addr;                     /*!< Address of the periodic advertising */
    uint16_t skip;                          /*!< Maximum number of periodic advertising events that can be skipped */
    uint16_t sync_timeout;                  /*!< Synchronization timeout */
} esp_ble_gap_periodic_adv_sync_params_t;

/**
* @brief extend adv report parameters
*/
typedef struct {
    // uint8_t props;
    // uint8_t legacy_event_type;
    esp_ble_gap_adv_type_t event_type;              /*!< extend advertising type */
    uint8_t addr_type;                              /*!< extend advertising address type */
    esp_bd_addr_t addr;                             /*!< extend advertising address */
    esp_ble_gap_pri_phy_t primary_phy;              /*!< extend advertising primary phy */
    esp_ble_gap_phy_t secondly_phy;                 /*!< extend advertising secondary phy */
    uint8_t sid;                                    /*!< extend advertising sid */
    uint8_t tx_power;                               /*!< extend advertising tx power */
    int8_t rssi;                                    /*!< extend advertising rssi */
    uint16_t per_adv_interval;                      /*!< periodic advertising interval */
    uint8_t dir_addr_type;                          /*!< direct address type */
    esp_bd_addr_t dir_addr;                         /*!< direct address */
    esp_ble_gap_ext_adv_data_status_t data_status;  /*!< data type */
    uint8_t adv_data_len;                           /*!< extend advertising data length */
    uint8_t adv_data[251];                          /*!< extend advertising data */
} esp_ble_gap_ext_adv_reprot_t;

/**
* @brief periodic adv report parameters
*/
typedef struct {
    uint16_t sync_handle;                          /*!< periodic advertising train handle */
    uint8_t tx_power;                              /*!< periodic advertising tx power*/
    int8_t rssi;                                   /*!< periodic advertising rssi */
    esp_ble_gap_ext_adv_data_status_t data_status; /*!< periodic advertising data type*/
    uint8_t data_length;                           /*!< periodic advertising data length */
    uint8_t data[251];                             /*!< periodic advertising data */
} esp_ble_gap_periodic_adv_report_t;

/**
* @brief perodic adv sync establish parameters
*/
typedef struct {
    uint8_t status;                               /*!< periodic advertising sync status */
    uint16_t sync_handle;                         /*!< periodic advertising train handle */
    uint8_t sid;                                  /*!< periodic advertising sid */
    esp_ble_addr_type_t addr_type;                /*!< periodic advertising address type */
    esp_bd_addr_t adv_addr;                       /*!< periodic advertising address */
    esp_ble_gap_phy_t adv_phy;                    /*!< periodic advertising adv phy type */
    uint16_t period_adv_interval;                 /*!< periodic advertising interval */
    uint8_t adv_clk_accuracy;                     /*!< periodic advertising clock accuracy */
} esp_ble_gap_periodic_adv_sync_estab_t;

/**
* @brief DTM TX parameters
*/
typedef struct
{
    uint8_t                     tx_channel;            /*!<  channel for sending test data, tx_channel = (Frequency -2402)/2, tx_channel range:0x00-0x27, Frequency range: 2402 MHz to 2480 MHz */
    uint8_t                     len_of_data;           /*!<  length in bytes of payload data in each packet */
    esp_ble_dtm_pkt_payload_t   pkt_payload;           /*!<  packet payload type. value range: 0x00-0x07 */
    esp_ble_gap_phy_t           phy;                   /*!<  the phy type used by the transmitter, coded phy with S=2:0x04 */
} esp_ble_dtm_enh_tx_t;

/**
* @brief DTM RX parameters
*/
typedef struct
{
    uint8_t             rx_channel;            /*!<  channel for test data reception, rx_channel = (Frequency -2402)/2, tx_channel range:0x00-0x27, Frequency range: 2402 MHz to 2480 MHz */
    esp_ble_gap_phy_t   phy;                   /*!<  the phy type used by the receiver, 1M phy: 0x01, 2M phy:0x02, coded phy:0x03 */
    uint8_t             modulation_idx;        /*!<  modulation index, 0x00:standard modulation index, 0x01:stable modulation index */
} esp_ble_dtm_enh_rx_t;

#endif //#if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
/// Periodic advertising sync trans mode
#define ESP_BLE_GAP_PAST_MODE_NO_SYNC_EVT                 (0x00) /*!< No attempt is made to sync and no periodic adv sync transfer received event */
#define ESP_BLE_GAP_PAST_MODE_NO_REPORT_EVT               (0x01) /*!< An periodic adv sync transfer received event and no periodic adv report events */
#define ESP_BLE_GAP_PAST_MODE_DUP_FILTER_DISABLED         (0x02) /*!< Periodic adv report events will be enabled with duplicate filtering disabled */
#define ESP_BLE_GAP_PAST_MODE_DUP_FILTER_ENABLED          (0x03) /*!< Periodic adv report events will be enabled with duplicate filtering enabled */
typedef uint8_t esp_ble_gap_past_mode_t;

/**
* @brief periodic adv sync transfer parameters
*/
typedef struct {
    esp_ble_gap_past_mode_t mode;       /*!< periodic advertising sync transfer mode */
    uint16_t skip;                      /*!< the number of periodic advertising packets that can be skipped */
    uint16_t sync_timeout;              /*!< synchronization timeout for the periodic advertising train */
    uint8_t cte_type;                   /*!< periodic advertising sync transfer CET type */
} esp_ble_gap_past_params_t;
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

/**
 * @brief Gap callback parameters union
 */
typedef union {
    /**
     * @brief ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT
     */
    struct ble_get_dev_name_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the get device name success status */
        char *name;                                 /*!< Name of bluetooth device */
    } get_dev_name_cmpl;                            /*!< Event parameter of ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    /**
     * @brief ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT
     */
    struct ble_adv_data_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set advertising data operation success status */
    } adv_data_cmpl;                                /*!< Event parameter of ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT
     */
    struct ble_scan_rsp_data_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set scan response data operation success status */
    } scan_rsp_data_cmpl;                           /*!< Event parameter of ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT
     */
    struct ble_scan_param_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set scan param operation success status */
    } scan_param_cmpl;                              /*!< Event parameter of ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_RESULT_EVT
     */
    struct ble_scan_result_evt_param {
        esp_gap_search_evt_t search_evt;            /*!< Search event type */
        esp_bd_addr_t bda;                          /*!< Bluetooth device address which has been searched */
        esp_bt_dev_type_t dev_type;                 /*!< Device type */
        esp_ble_addr_type_t ble_addr_type;          /*!< Ble device address type */
        esp_ble_evt_type_t ble_evt_type;            /*!< Ble scan result event type */
        int rssi;                                   /*!< Searched device's RSSI */
        uint8_t  ble_adv[ESP_BLE_ADV_DATA_LEN_MAX + ESP_BLE_SCAN_RSP_DATA_LEN_MAX];     /*!< Received EIR */
        int flag;                                   /*!< Advertising data flag bit */
        int num_resps;                              /*!< Scan result number */
        uint8_t adv_data_len;                       /*!< Adv data length */
        uint8_t scan_rsp_len;                       /*!< Scan response length */
        uint32_t num_dis;                          /*!< The number of discard packets */
    } scan_rst;                                     /*!< Event parameter of ESP_GAP_BLE_SCAN_RESULT_EVT */
    /**
     * @brief ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT
     */
    struct ble_adv_data_raw_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set raw advertising data operation success status */
    } adv_data_raw_cmpl;                            /*!< Event parameter of ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT
     */
    struct ble_scan_rsp_data_raw_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set raw advertising data operation success status */
    } scan_rsp_data_raw_cmpl;                       /*!< Event parameter of ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_ADV_START_COMPLETE_EVT
     */
    struct ble_adv_start_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate advertising start operation success status */
    } adv_start_cmpl;                               /*!< Event parameter of ESP_GAP_BLE_ADV_START_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_START_COMPLETE_EVT
     */
    struct ble_scan_start_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate scan start operation success status */
    } scan_start_cmpl;                              /*!< Event parameter of ESP_GAP_BLE_SCAN_START_COMPLETE_EVT */
#endif //#if (BLE_42_FEATURE_SUPPORT == TRUE)
    esp_ble_sec_t ble_security;                     /*!< ble gap security union type */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    /**
     * @brief ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT
     */
    struct ble_scan_stop_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate scan stop operation success status */
    } scan_stop_cmpl;                               /*!< Event parameter of ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT
     */
    struct ble_adv_stop_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate adv stop operation success status */
    } adv_stop_cmpl;                                /*!< Event parameter of ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT
     */
    struct ble_adv_clear_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate adv clear operation success status */
    } adv_clear_cmpl;                                /*!< Event parameter of ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT */
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    /**
     * @brief ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT
     */
    struct ble_set_rand_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate set static rand address operation success status */
    } set_rand_addr_cmpl;                           /*!< Event parameter of ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT */
    /**
     * @brief ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT
     */
    struct ble_update_conn_params_evt_param {
        esp_bt_status_t status;                    /*!< Indicate update connection parameters success status */
        esp_bd_addr_t bda;                         /*!< Bluetooth device address */
        uint16_t min_int;                          /*!< Min connection interval */
        uint16_t max_int;                          /*!< Max connection interval */
        uint16_t latency;                          /*!< Slave latency for the connection in number of connection events. Range: 0x0000 to 0x01F3 */
        uint16_t conn_int;                         /*!< Current connection interval */
        uint16_t timeout;                          /*!< Supervision timeout for the LE Link. Range: 0x000A to 0x0C80.
                                                     Mandatory Range: 0x000A to 0x0C80 Time = N * 10 msec */
    } update_conn_params;                          /*!< Event parameter of ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT */
    /**
     * @brief ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT
     */
    struct ble_pkt_data_length_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set pkt data length operation success status */
        esp_ble_pkt_data_length_params_t params;    /*!<  pkt data length value */
    } pkt_data_length_cmpl;                          /*!< Event parameter of ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT
     */
    struct ble_local_privacy_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the set local privacy operation success status */
    } local_privacy_cmpl;                           /*!< Event parameter of ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT
     */
    struct ble_remove_bond_dev_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the remove bond device operation success status */
        esp_bd_addr_t bd_addr;                      /*!< The device address which has been remove from the bond list */
    } remove_bond_dev_cmpl;                         /*!< Event parameter of ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT
     */
    struct ble_clear_bond_dev_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the clear bond device operation success status */
    } clear_bond_dev_cmpl;                          /*!< Event parameter of ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT
     */
    struct ble_get_bond_dev_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the get bond device operation success status */
        uint8_t dev_num;                            /*!< Indicate the get number device in the bond list */
        esp_ble_bond_dev_t *bond_dev;               /*!< the pointer to the bond device Structure */
    } get_bond_dev_cmpl;                            /*!< Event parameter of ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT
     */
    struct ble_read_rssi_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the read adv tx power operation success status */
        int8_t rssi;                                /*!< The ble remote device rssi value, the range is from -127 to 20, the unit is dbm,
                                                         if the RSSI cannot be read, the RSSI metric shall be set to 127. */
        esp_bd_addr_t remote_addr;                  /*!< The remote device address */
    } read_rssi_cmpl;                               /*!< Event parameter of ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT
     */
    struct ble_update_whitelist_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate the add or remove whitelist operation success status */
        esp_ble_wl_operation_t wl_operation;        /*!< The value is ESP_BLE_WHITELIST_ADD if add address to whitelist operation success, ESP_BLE_WHITELIST_REMOVE if remove address from the whitelist operation success */
    } update_whitelist_cmpl;                        /*!< Event parameter of ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT */
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    /**
     * @brief ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT
     */
    struct ble_update_duplicate_exceptional_list_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate update duplicate scan exceptional list operation success status */
        uint8_t         subcode;                    /*!< Define in esp_bt_duplicate_exceptional_subcode_type_t */
        uint16_t         length;                     /*!< The length of device_info */
        esp_duplicate_info_t device_info;           /*!< device information, when subcode is ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_CLEAN, the value is invalid */
    } update_duplicate_exceptional_list_cmpl;       /*!< Event parameter of ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT */
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    /**
     * @brief ESP_GAP_BLE_SET_CHANNELS_EVT
      */
    struct ble_set_channels_evt_param {
        esp_bt_status_t stat;                       /*!< BLE set channel status */
    } ble_set_channels;                             /*!< Event parameter of ESP_GAP_BLE_SET_CHANNELS_EVT */

#if (BLE_50_FEATURE_SUPPORT == TRUE)
    /**
     * @brief ESP_GAP_BLE_READ_PHY_COMPLETE_EVT
     */
    struct ble_read_phy_cmpl_evt_param {
        esp_bt_status_t status;                   /*!< read phy complete status */
        esp_bd_addr_t bda;                        /*!< read phy address */
        esp_ble_gap_phy_t tx_phy;                 /*!< tx phy type */
        esp_ble_gap_phy_t rx_phy;                 /*!< rx phy type */
    } read_phy;                                   /*!< Event parameter of ESP_GAP_BLE_READ_PHY_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT
     */
    struct ble_set_perf_def_phy_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate perf default phy set status */
    } set_perf_def_phy;                             /*!< Event parameter of ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT
     */
    struct ble_set_perf_phy_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate perf phy set status */
    } set_perf_phy;                                 /*!< Event parameter of ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT
     */
    struct ble_ext_adv_set_rand_addr_cmpl_evt_param {
        esp_bt_status_t status;                      /*!< Indicate extend advertising random address set status */
    } ext_adv_set_rand_addr;                         /*!< Event parameter of ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT
     */
    struct ble_ext_adv_set_params_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate extend advertising parameters set status */
    } ext_adv_set_params;                           /*!< Event parameter of ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT
     */
     struct ble_ext_adv_data_set_cmpl_evt_param {
        esp_bt_status_t status;                      /*!< Indicate extend advertising data set status */
    } ext_adv_data_set;                              /*!< Event parameter of ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT
     */
    struct ble_ext_adv_scan_rsp_set_cmpl_evt_param {
        esp_bt_status_t status;                      /*!< Indicate extend advertising scan response data set status */
    } scan_rsp_set;                                  /*!< Event parameter of ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT
     */
    struct ble_ext_adv_start_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate advertising start operation success status */
    } ext_adv_start;                                /*!< Event parameter of ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT
     */
    struct ble_ext_adv_stop_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate advertising stop operation success status */
    } ext_adv_stop;                                 /*!< Event parameter of ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT
     */
    struct ble_ext_adv_set_remove_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate advertising stop operation success status */
    } ext_adv_remove;                               /*!< Event parameter of ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT
     */
    struct ble_ext_adv_set_clear_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicate advertising stop operation success status */
    } ext_adv_clear;                                /*!< Event parameter of ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT
     */
    struct ble_periodic_adv_set_params_cmpl_param {
        esp_bt_status_t status;                    /*!< Indicate periodic advertisingparameters set status */
    } peroid_adv_set_params;                       /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT
     */
    struct ble_periodic_adv_data_set_cmpl_param {
        esp_bt_status_t status;                    /*!< Indicate periodic advertising data set status */
    } period_adv_data_set;                         /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT
     */
    struct ble_periodic_adv_start_cmpl_param {
        esp_bt_status_t status;                   /*!< Indicate periodic advertising start status */
    } period_adv_start;                           /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT
     */
    struct ble_periodic_adv_stop_cmpl_param {
        esp_bt_status_t status;                  /*!< Indicate periodic advertising stop status */
    } period_adv_stop;                           /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT
     */
    struct ble_period_adv_create_sync_cmpl_param {
        esp_bt_status_t status;                  /*!< Indicate periodic advertising create sync status */
    } period_adv_create_sync;                    /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT
     */
    struct ble_period_adv_sync_cancel_cmpl_param {
        esp_bt_status_t status;                  /*!< Indicate periodic advertising sync cancel status */
    } period_adv_sync_cancel;                    /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT */
     /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT
     */
    struct ble_period_adv_sync_terminate_cmpl_param {
        esp_bt_status_t status;                  /*!< Indicate periodic advertising sync terminate status */
    } period_adv_sync_term;                      /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT
     */
    struct ble_period_adv_add_dev_cmpl_param {
        esp_bt_status_t status;                 /*!< Indicate periodic advertising device list add status */
    } period_adv_add_dev;                       /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT
     */
    struct ble_period_adv_remove_dev_cmpl_param {
        esp_bt_status_t status;                /*!< Indicate periodic advertising device list remove status */
    } period_adv_remove_dev;                   /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT
     */
    struct ble_period_adv_clear_dev_cmpl_param {
        esp_bt_status_t status;               /*!< Indicate periodic advertising device list clean status */
    } period_adv_clear_dev;                   /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT
     */
    struct ble_set_ext_scan_params_cmpl_param {
        esp_bt_status_t status;              /*!< Indicate extend advertising parameters set status */
    } set_ext_scan_params;                   /*!< Event parameter of ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT
     */
    struct ble_ext_scan_start_cmpl_param {
        esp_bt_status_t status;             /*!< Indicate extend advertising start status */
    } ext_scan_start;                       /*!< Event parameter of ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT
     */
    struct ble_ext_scan_stop_cmpl_param {
        esp_bt_status_t status;            /*!< Indicate extend advertising stop status */
    } ext_scan_stop;                       /*!< Event parameter of ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT
     */
    struct ble_ext_conn_params_set_cmpl_param {
        esp_bt_status_t status;            /*!< Indicate extend connection parameters set status */
    } ext_conn_params_set;                 /*!< Event parameter of ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_ADV_TERMINATED_EVT
     */
    struct ble_adv_terminate_param {
        uint8_t status;                   /*!< Indicate adv terminate status */
        /*  status 0x3c indicates that advertising for a fixed duration completed or,
            for directed advertising, that advertising completed without a connection
            being created;
            status 0x00 indicates that advertising successfully ended with a connection being created.
        */
        uint8_t adv_instance;           /*!< extend advertising handle */
        uint16_t conn_idx;              /*!< connection index */
        uint8_t completed_event;        /*!< the number of completed extend advertising events */
    } adv_terminate;                    /*!< Event parameter of ESP_GAP_BLE_ADV_TERMINATED_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT
     */
    struct ble_scan_req_received_param {
        uint8_t adv_instance;                /*!< extend advertising handle */
        esp_ble_addr_type_t scan_addr_type;  /*!< scanner address type */
        esp_bd_addr_t scan_addr;             /*!< scanner address */
    } scan_req_received;                     /*!< Event parameter of ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT */
    /**
     * @brief ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT
     */
    struct ble_channel_sel_alg_param {
        uint16_t conn_handle;              /*!< connection handle */
        uint8_t channel_sel_alg;           /*!< channel selection algorithm */
    } channel_sel_alg;                     /*!< Event parameter of ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT
     */
    struct ble_periodic_adv_sync_lost_param {
        uint16_t sync_handle;                 /*!< sync handle */
    } periodic_adv_sync_lost;                 /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT
     */
    struct ble_periodic_adv_sync_estab_param {
        uint8_t status;                      /*!< periodic advertising sync status */
        uint16_t sync_handle;                /*!< periodic advertising sync handle */
        uint8_t sid;                         /*!< periodic advertising sid */
        esp_ble_addr_type_t adv_addr_type;   /*!< periodic advertising address type */
        esp_bd_addr_t adv_addr;              /*!< periodic advertising address */
        esp_ble_gap_phy_t adv_phy;           /*!< periodic advertising phy type */
        uint16_t period_adv_interval;        /*!< periodic advertising interval */
        uint8_t adv_clk_accuracy;            /*!< periodic advertising clock accuracy */
    } periodic_adv_sync_estab;               /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT */
    /**
     * @brief ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT
     */
    struct ble_phy_update_cmpl_param {
        esp_bt_status_t status;             /*!< phy update status */
        esp_bd_addr_t bda;                  /*!< address */
        esp_ble_gap_phy_t tx_phy;           /*!< tx phy type */
        esp_ble_gap_phy_t rx_phy;           /*!< rx phy type */
    } phy_update;                           /*!< Event parameter of ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_EXT_ADV_REPORT_EVT
     */
    struct ble_ext_adv_report_param {
        esp_ble_gap_ext_adv_reprot_t params;   /*!< extend advertising report parameters */
    } ext_adv_report;                          /*!< Event parameter of ESP_GAP_BLE_EXT_ADV_REPORT_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT
     */
    struct ble_periodic_adv_report_param {
        esp_ble_gap_periodic_adv_report_t params; /*!< periodic advertising report parameters */
    } period_adv_report;                          /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT */
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT
     */
    struct ble_periodic_adv_recv_enable_cmpl_param {
        esp_bt_status_t status;             /*!< Set periodic advertising receive enable status */
    } period_adv_recv_enable;               /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT
     */
    struct ble_periodic_adv_sync_trans_cmpl_param {
        esp_bt_status_t status;             /*!< Periodic advertising sync transfer status */
        esp_bd_addr_t bda;                  /*!< The remote device address */
    } period_adv_sync_trans;                /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT
     */
    struct ble_periodic_adv_set_info_trans_cmpl_param {
        esp_bt_status_t status;             /*!< Periodic advertising set info transfer status */
        esp_bd_addr_t bda;                  /*!< The remote device address */
    } period_adv_set_info_trans;            /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT
     */
    struct ble_set_past_params_cmpl_param {
        esp_bt_status_t status;             /*!< Set periodic advertising sync transfer params status */
        esp_bd_addr_t bda;                  /*!< The remote device address */
    } set_past_params;                      /*!< Event parameter of ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT */
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_RECV_EVT
     */
    struct ble_periodic_adv_sync_trans_recv_param {
        esp_bt_status_t status;             /*!< Periodic advertising sync transfer received status */
        esp_bd_addr_t bda;                  /*!< The remote device address */
        uint16_t service_data;              /*!< The value provided by the peer device */
        uint16_t sync_handle;               /*!< Periodic advertising sync handle */
        uint8_t adv_sid;                    /*!< Periodic advertising set id */
        uint8_t adv_addr_type;              /*!< Periodic advertiser address type */
        esp_bd_addr_t adv_addr;             /*!< Periodic advertiser address */
        esp_ble_gap_phy_t adv_phy;          /*!< Periodic advertising PHY */
        uint16_t adv_interval;              /*!< Periodic advertising interval */
        uint8_t adv_clk_accuracy;           /*!< Periodic advertising clock accuracy */
    } past_received;                        /*!< Event parameter of ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_RECV_EVT */
#endif // #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
    /**
     * @brief ESP_GAP_BLE_DTM_TEST_UPDATE_EVT
     */
    struct ble_dtm_state_update_evt_param {
        esp_bt_status_t status;                     /*!< Indicate DTM operation success status */
        esp_ble_dtm_update_evt_t update_evt;        /*!< DTM state change event, 0x00: DTM TX start, 0x01: DTM RX start, 0x02:DTM end */
        uint16_t num_of_pkt;                        /*!< number of packets received, only valid if update_evt is DTM_TEST_STOP_EVT and shall be reported as 0 for a transmitter */
    } dtm_state_update;                             /*!< Event parameter of ESP_GAP_BLE_DTM_TEST_UPDATE_EVT */
} esp_ble_gap_cb_param_t;

/**
 * @brief GAP callback function type
 * @param event : Event type
 * @param param : Point to callback parameter, currently is union type
 */
typedef void (* esp_gap_ble_cb_t)(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/**
 * @brief           This function is called to occur gap event, such as scan result
 *
 * @param[in]       callback: callback function
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_register_callback(esp_gap_ble_cb_t callback);

/**
 * @brief           This function is called to get the current gap callback
 *
 * @return
 *                  - esp_gap_ble_cb_t : callback function
 *
 */
esp_gap_ble_cb_t esp_ble_gap_get_callback(void);

#if (BLE_42_FEATURE_SUPPORT == TRUE)
/**
 * @brief           This function is called to override the BTA default ADV parameters.
 *
 * @param[in]       adv_data: Pointer to User defined ADV data structure. This
 *                  memory space can not be freed until callback of config_adv_data
 *                  is received.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_config_adv_data (esp_ble_adv_data_t *adv_data);



/**
 * @brief           This function is called to set scan parameters
 *
 * @param[in]       scan_params: Pointer to User defined scan_params data structure. This
 *                  memory space can not be freed until callback of set_scan_params
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params);


/**
 * @brief           This procedure keep the device scanning the peer device which advertising on the air
 *
 * @param[in]       duration: Keeping the scanning time, the unit is second.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_start_scanning(uint32_t duration);


/**
 * @brief          This function call to stop the device scanning the peer device which advertising on the air
 * @return
 *                 - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_stop_scanning(void);

/**
 * @brief           This function is called to start advertising.
 *
 * @param[in]       adv_params: pointer to User defined adv_params data structure.

 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_start_advertising (esp_ble_adv_params_t *adv_params);



/**
 * @brief           This function is called to stop advertising.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_stop_advertising(void);
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)


/**
 * @brief           Update connection parameters, can only be used when connection is up.
 *
 * @param[in]       params   -  connection update parameters
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params);


/**
 * @brief           This function is to set maximum LE data packet size
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_set_pkt_data_len(esp_bd_addr_t remote_device, uint16_t tx_data_length);

/**
 * @brief           This function allows configuring either a Non-Resolvable Private Address or a Static Random Address
 *
 * @param[in]       rand_addr: The address to be configured. Refer to the table below for possible address subtypes:
 *
 *               | address [47:46] | Address Type             |
 *               |-----------------|--------------------------|
 *               |      0b00       | Non-Resolvable Private   |
 *               |                 | Address                  |
 *               |-----------------|--------------------------|
 *               |      0b11       | Static Random Address    |
 *               |-----------------|--------------------------|
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr);

/**
 * @brief           This function clears the random address for the application
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_clear_rand_addr(void);



/**
 * @brief           Enable/disable privacy (including address resolution) on the local device
 *
 * @param[in]       privacy_enable   - enable/disable privacy on remote device.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_config_local_privacy (bool privacy_enable);

/**
 * @brief           set local gap appearance icon
 *
 *
 * @param[in]       icon   - External appearance value, these values are defined by the Bluetooth SIG, please refer to
 *                  https://www.bluetooth.com/specifications/assigned-numbers/
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_config_local_icon (uint16_t icon);

/**
* @brief            Add or remove device from white list
*
* @param[in]        add_remove: the value is true if added the ble device to the white list, and false remove to the white list.
* @param[in]        remote_bda: the remote device address add/remove from the white list.
* @param[in]        wl_addr_type: whitelist address type
* @return
*                     - ESP_OK : success
*                     - other  : failed
*
*/
esp_err_t esp_ble_gap_update_whitelist(bool add_remove, esp_bd_addr_t remote_bda, esp_ble_wl_addr_type_t wl_addr_type);

/**
* @brief            Clear all white list
*
* @return
*                     - ESP_OK : success
*                     - other  : failed
*
*/
esp_err_t esp_ble_gap_clear_whitelist(void);

/**
* @brief            Get the whitelist size in the controller
*
* @param[out]       length: the white list length.
* @return
*                     - ESP_OK : success
*                     - other  : failed
*
*/
esp_err_t esp_ble_gap_get_whitelist_size(uint16_t *length);
#if (BLE_42_FEATURE_SUPPORT == TRUE)
/**
* @brief            This function is called to set the preferred connection
*                   parameters when default connection parameter is not desired before connecting.
*                   This API can only be used in the master role.
*
* @param[in]        bd_addr: BD address of the peripheral
* @param[in]        min_conn_int: minimum preferred connection interval
* @param[in]        max_conn_int: maximum preferred connection interval
* @param[in]        slave_latency: preferred slave latency
* @param[in]        supervision_tout: preferred supervision timeout
*
* @return
*                   - ESP_OK : success
*                   - other  : failed
*
*/
esp_err_t esp_ble_gap_set_prefer_conn_params(esp_bd_addr_t bd_addr,
                                                                 uint16_t min_conn_int, uint16_t max_conn_int,
                                                                 uint16_t slave_latency, uint16_t supervision_tout);
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)
/**
 * @brief           Set device name to the local device
 *                  Note: This API don't affect the advertising data
 *
 * @param[in]       name   -  device name.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_set_device_name(const char *name);

/**
 * @brief           Get device name of the local device
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_get_device_name(void);

/**
 * @brief          This function is called to get local used address and address type.
 *                 uint8_t *esp_bt_dev_get_address(void) get the public address
 *
 * @param[in]       local_used_addr - current local used ble address (six bytes)
 * @param[in]       addr_type   - ble address type
 *
 * @return          - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_get_local_used_addr(esp_bd_addr_t local_used_addr, uint8_t * addr_type);
/**
 * @brief          This function is called to get ADV data for a specific type.
 *
 * @param[in]       adv_data - pointer of ADV data which to be resolved
 * @param[in]       type   - finding ADV data type
 * @param[out]      length - return the length of ADV data not including type
 *
 * @return          pointer of ADV data
 *
 */
uint8_t *esp_ble_resolve_adv_data(uint8_t *adv_data, uint8_t type, uint8_t *length);
#if (BLE_42_FEATURE_SUPPORT == TRUE)
/**
 * @brief           This function is called to set raw advertising data. User need to fill
 *                  ADV data by self.
 *
 * @param[in]       raw_data : raw advertising data with the format: [Length 1][Data Type 1][Data 1][Length 2][Data Type 2][Data 2] ...
 * @param[in]       raw_data_len : raw advertising data length , less than 31 bytes
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 *
 */
esp_err_t esp_ble_gap_config_adv_data_raw(uint8_t *raw_data, uint32_t raw_data_len);

/**
 * @brief           This function is called to set raw scan response data. User need to fill
 *                  scan response data by self.
 *
 * @param[in]       raw_data : raw scan response data
 * @param[in]       raw_data_len : raw scan response data length , less than 31 bytes
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_ble_gap_config_scan_rsp_data_raw(uint8_t *raw_data, uint32_t raw_data_len);
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief           This function is called to read the RSSI of remote device.
 *                  The address of link policy results are returned in the gap callback function with
 *                  ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT event.
 *
 * @param[in]       remote_addr : The remote connection device address.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_ble_gap_read_rssi(esp_bd_addr_t remote_addr);
#if (BLE_42_FEATURE_SUPPORT == TRUE)
/**
 * @brief           This function is called to add a device info into the duplicate scan exceptional list.
 *
 *
 * @param[in]       type: device info type, it is defined in esp_ble_duplicate_exceptional_info_type_t
 *                  when type is MESH_BEACON_TYPE, MESH_PROV_SRV_ADV or MESH_PROXY_SRV_ADV , device_info is invalid.
 * @param[in]       device_info: the device information.
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_ble_gap_add_duplicate_scan_exceptional_device(esp_ble_duplicate_exceptional_info_type_t type, esp_duplicate_info_t device_info);

/**
 * @brief           This function is called to remove a device info from the duplicate scan exceptional list.
 *
 *
 * @param[in]       type: device info type, it is defined in esp_ble_duplicate_exceptional_info_type_t
 *                  when type is MESH_BEACON_TYPE, MESH_PROV_SRV_ADV or MESH_PROXY_SRV_ADV , device_info is invalid.
 * @param[in]       device_info: the device information.
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_ble_gap_remove_duplicate_scan_exceptional_device(esp_ble_duplicate_exceptional_info_type_t type, esp_duplicate_info_t device_info);

/**
 * @brief           This function is called to clean the duplicate scan exceptional list.
 *                  This API will delete all device information in the duplicate scan exceptional list.
 *
 *
 * @param[in]       list_type: duplicate scan exceptional list type, the value can be one or more of esp_duplicate_scan_exceptional_list_type_t.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_ble_gap_clean_duplicate_scan_exceptional_list(esp_duplicate_scan_exceptional_list_type_t list_type);
#endif // #if (BLE_42_FEATURE_SUPPORT == TRUE)

#if (SMP_INCLUDED == TRUE)
/**
* @brief             Set a GAP security parameter value. Overrides the default value.
*
*                    Secure connection is highly recommended to avoid some major
*                    vulnerabilities like 'Impersonation in the Pin Pairing Protocol'
*                    (CVE-2020-26555) and 'Authentication of the LE Legacy Pairing
*                    Protocol'.
*
*                    To accept only `secure connection mode`, it is necessary do as following:
*
*                    1. Set bit `ESP_LE_AUTH_REQ_SC_ONLY` (`param_type` is
*                    `ESP_BLE_SM_AUTHEN_REQ_MODE`), bit `ESP_LE_AUTH_BOND` and bit
*                    `ESP_LE_AUTH_REQ_MITM` is optional as required.
*
*                    2. Set to `ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE` (`param_type` is
*                    `ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH`).
*
* @param[in]       param_type : the type of the param which to be set
* @param[in]       value  : the param value
* @param[in]       len : the length of the param value
*
* @return            - ESP_OK : success
*                       - other  : failed
*
*/
esp_err_t esp_ble_gap_set_security_param(esp_ble_sm_param_t param_type,
                                         void *value, uint8_t len);

/**
* @brief             Grant security request access.
*
* @param[in]       bd_addr : BD address of the peer
* @param[in]       accept  :  accept the security request or not
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_security_rsp(esp_bd_addr_t bd_addr,  bool accept);


/**
* @brief             Set a gap parameter value. Use this function to change
*                    the default GAP parameter values.
*
* @param[in]       bd_addr : the address of the peer device need to encryption
* @param[in]       sec_act  : This is the security action to indicate
*                                   what kind of BLE security level is required for
*                                   the BLE link if the BLE is supported
*
* @return            - ESP_OK : success
*                       - other  : failed
*
*/
esp_err_t esp_ble_set_encryption(esp_bd_addr_t bd_addr, esp_ble_sec_act_t sec_act);

/**
* @brief          Reply the key value to the peer device in the legacy connection stage.
*
* @param[in]      bd_addr : BD address of the peer
* @param[in]      accept : passkey entry successful or declined.
* @param[in]      passkey : passkey value, must be a 6 digit number,
*                                     can be lead by 0.
*
* @return            - ESP_OK : success
*                  - other  : failed
*
*/
esp_err_t esp_ble_passkey_reply(esp_bd_addr_t bd_addr, bool accept, uint32_t passkey);


/**
* @brief           Reply the confirm value to the peer device in the secure connection stage.
*
* @param[in]       bd_addr : BD address of the peer device
* @param[in]       accept : numbers to compare are the same or different.
*
* @return            - ESP_OK : success
*                       - other  : failed
*
*/
esp_err_t esp_ble_confirm_reply(esp_bd_addr_t bd_addr, bool accept);

/**
* @brief           Removes a device from the security database list of
*                  peer device. It manages unpairing event while connected.
*
* @param[in]       bd_addr : BD address of the peer device
*
* @return            - ESP_OK : success
*                       - other  : failed
*
*/
esp_err_t esp_ble_remove_bond_device(esp_bd_addr_t bd_addr);

/**
* @brief           Get the device number from the security database list of peer device.
*                  It will return the device bonded number immediately.
*
* @return          - >= 0 : bonded devices number.
*                  - ESP_FAIL  : failed
*
*/
int esp_ble_get_bond_device_num(void);


/**
* @brief           Get the device from the security database list of peer device.
*                  It will return the device bonded information immediately.
* @param[inout]    dev_num: Indicate the dev_list array(buffer) size as input.
*                           If dev_num is large enough, it means the actual number as output.
*                           Suggest that dev_num value equal to esp_ble_get_bond_device_num().
*
* @param[out]      dev_list: an array(buffer) of `esp_ble_bond_dev_t` type. Use for storing the bonded devices address.
*                            The dev_list should be allocated by who call this API.
* @return          - ESP_OK : success
*                  - other  : failed
*
*/
esp_err_t esp_ble_get_bond_device_list(int *dev_num, esp_ble_bond_dev_t *dev_list);

/**
* @brief           This function is called to provide the OOB data for
*                  SMP in response to ESP_GAP_BLE_OOB_REQ_EVT
*
* @param[in]       bd_addr: BD address of the peer device.
* @param[in]       TK: Temporary Key value, the TK value shall be a 128-bit random number
* @param[in]       len: length of temporary key, should always be 128-bit
*
* @return          - ESP_OK : success
*                  - other  : failed
*
*/
esp_err_t esp_ble_oob_req_reply(esp_bd_addr_t bd_addr, uint8_t *TK, uint8_t len);

/**
* @brief           This function is called to provide the OOB data for
*                  SMP in response to ESP_GAP_BLE_SC_OOB_REQ_EVT
*
* @param[in]       bd_addr: BD address of the peer device.
* @param[in]       p_c: Confirmation value, it shall be a 128-bit random number
* @param[in]       p_r: Randomizer value, it should be a 128-bit random number
*
* @return          - ESP_OK : success
*                  - other  : failed
*
*/
esp_err_t esp_ble_sc_oob_req_reply(esp_bd_addr_t bd_addr, uint8_t p_c[16], uint8_t p_r[16]);

/**
* @brief           This function is called to create the OOB data for
*                  SMP when secure connection
*
* @return          - ESP_OK : success
*                  - other  : failed
*
*/
esp_err_t esp_ble_create_sc_oob_data(void);
#endif /* #if (SMP_INCLUDED == TRUE) */

/**
* @brief           This function is to disconnect the physical connection of the peer device
*                  gattc may have multiple virtual GATT server connections when multiple app_id registered.
*                  esp_ble_gattc_close (esp_gatt_if_t gattc_if, uint16_t conn_id) only close one virtual GATT server connection.
*                  if there exist other virtual GATT server connections, it does not disconnect the physical connection.
*                  esp_ble_gap_disconnect(esp_bd_addr_t remote_device) disconnect the physical connection directly.
*
*
*
* @param[in]       remote_device : BD address of the peer device
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_disconnect(esp_bd_addr_t remote_device);

/**
* @brief           This function is called to read the connection
*                  parameters information of the device
*
* @param[in]       bd_addr: BD address of the peer device.
* @param[out]      conn_params: the connection parameters information
*
* @return          - ESP_OK : success
*                  - other  : failed
*
*/
esp_err_t esp_ble_get_current_conn_params(esp_bd_addr_t bd_addr, esp_gap_conn_params_t *conn_params);

/**
* @brief            BLE set channels
*
* @param[in]        channels :   The n th such field (in the range 0 to 36) contains the value for the link layer channel index n.
*                                0 means channel n is bad.
*                                1 means channel n is unknown.
*                                The most significant bits are reserved and shall be set to 0.
*                                At least one channel shall be marked as unknown.
*
* @return           - ESP_OK : success
*                   - ESP_ERR_INVALID_STATE: if bluetooth stack is not yet enabled
*                   - other  : failed
*
*/
esp_err_t esp_gap_ble_set_channels(esp_gap_ble_channels channels);

/**
* @brief           This function is called to authorized a link after Authentication(MITM protection)
*
* @param[in]       bd_addr: BD address of the peer device.
* @param[out]      authorize: Authorized the link or not.
*
* @return          - ESP_OK : success
*                  - other  : failed
*
*/
esp_err_t esp_gap_ble_set_authorization(esp_bd_addr_t bd_addr, bool authorize);

#if (BLE_50_FEATURE_SUPPORT == TRUE)

/**
* @brief           This function is used to read the current transmitter PHY
*                  and receiver PHY on the connection identified by remote address.
*
* @param[in]       bd_addr : BD address of the peer device
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_read_phy(esp_bd_addr_t bd_addr);

/**
* @brief           This function is used to allows the Host to specify its preferred values
*                  for the transmitter PHY and receiver PHY to be used for all subsequent connections
*                  over the LE transport.
*
* @param[in]       tx_phy_mask : indicates the transmitter PHYs that the Host prefers the Controller to use
* @param[in]       rx_phy_mask : indicates the receiver PHYs that the Host prefers the Controller to use
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_set_preferred_default_phy(esp_ble_gap_phy_mask_t tx_phy_mask, esp_ble_gap_phy_mask_t rx_phy_mask);
/**
* @brief           This function is used to set the PHY preferences for the connection identified by the remote address.
*                  The Controller might not be able to make the change (e.g. because the peer does not support the requested PHY)
*                  or may decide that the current PHY is preferable.
*
* @param[in]       bd_addr : remote address
* @param[in]       all_phys_mask : a bit field that allows the Host to specify
* @param[in]       tx_phy_mask : a bit field that indicates the transmitter PHYs that the Host prefers the Controller to use
* @param[in]       rx_phy_mask : a bit field that indicates the receiver PHYs that the Host prefers the Controller to use
* @param[in]       phy_options : a bit field that allows the Host to specify options for PHYs
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_set_preferred_phy(esp_bd_addr_t bd_addr,
                                       esp_ble_gap_all_phys_t all_phys_mask,
                                       esp_ble_gap_phy_mask_t tx_phy_mask,
                                       esp_ble_gap_phy_mask_t rx_phy_mask,
                                       esp_ble_gap_prefer_phy_options_t phy_options);

/**
* @brief           This function is used by the Host to set the random device address specified by the Random_Address parameter.
*
* @param[in]       instance  : Used to identify an advertising set
* @param[in]       rand_addr : Random Device Address
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_ext_adv_set_rand_addr(uint8_t instance, esp_bd_addr_t rand_addr);

/**
* @brief           This function is used by the Host to set the advertising parameters.
*
* @param[in]       instance : identifies the advertising set whose parameters are being configured.
* @param[in]       params   : advertising parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_ext_adv_set_params(uint8_t instance, const esp_ble_gap_ext_adv_params_t *params);

/**
* @brief           This function is used to set the data used in advertising PDUs that have a data field
*
* @param[in]       instance : identifies the advertising set whose data are being configured
* @param[in]       length   : data length
* @param[in]       data     : data information
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_config_ext_adv_data_raw(uint8_t instance, uint16_t length, const uint8_t *data);

/**
* @brief           This function is used to provide scan response data used in scanning response PDUs
*
* @param[in]       instance : identifies the advertising set whose response data are being configured.
* @param[in]       length : responsedata length
* @param[in]       scan_rsp_data : response data information
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_config_ext_scan_rsp_data_raw(uint8_t instance, uint16_t length,
                                                                   const uint8_t *scan_rsp_data);
/**
* @brief           This function is used to request the Controller to enable one or more
*                  advertising sets using the advertising sets identified by the instance parameter.
*
* @param[in]       num_adv : Number of advertising sets to enable or disable
* @param[in]       ext_adv : adv parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_ext_adv_start(uint8_t num_adv, const esp_ble_gap_ext_adv_t *ext_adv);

/**
* @brief           This function is used to request the Controller to disable one or more
*                  advertising sets using the advertising sets identified by the instance parameter.
*
* @param[in]       num_adv : Number of advertising sets to enable or disable
* @param[in]       ext_adv_inst : ext adv instance
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_ext_adv_stop(uint8_t num_adv, const uint8_t *ext_adv_inst);

/**
* @brief           This function is used to remove an advertising set from the Controller.
*
* @param[in]       instance : Used to identify an advertising set
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_ext_adv_set_remove(uint8_t instance);

/**
* @brief           This function is used to remove all existing advertising sets from the Controller.
*
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_ext_adv_set_clear(void);

/**
* @brief           This function is used by the Host to set the parameters for periodic advertising.
*
* @param[in]       instance : identifies the advertising set whose periodic advertising parameters are being configured.
* @param[in]       params : periodic adv parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_set_params(uint8_t instance, const esp_ble_gap_periodic_adv_params_t *params);

#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)
/**
* @brief           This function is used to set the data used in periodic advertising PDUs.
*
* @param[in]       instance : identifies the advertising set whose periodic advertising parameters are being configured.
* @param[in]       length : the length of periodic data
* @param[in]       data : periodic data information
* @param[in]       only_update_did : If true, only the Advertising DID of the periodic advertising will be updated, and the length and data parameters will be ignored.
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_config_periodic_adv_data_raw(uint8_t instance, uint16_t length,
                                                                           const uint8_t *data, bool only_update_did);
#else
/**
* @brief           This function is used to set the data used in periodic advertising PDUs.
*
* @param[in]       instance : identifies the advertising set whose periodic advertising parameters are being configured.
* @param[in]       length : the length of periodic data
* @param[in]       data : periodic data information
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_config_periodic_adv_data_raw(uint8_t instance, uint16_t length,
                                                                           const uint8_t *data);
#endif

#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)
/**
* @brief           This function is used to request the Controller to enable the periodic advertising for the advertising set specified
*
* @param[in]       instance : Used to identify an advertising set
* @param[in]       include_adi : If true, the ADI (Advertising Data Info) field will be included in AUX_SYNC_IND PDUs
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_start(uint8_t instance,bool include_adi);
#else
/**
* @brief           This function is used to request the Controller to enable the periodic advertising for the advertising set specified
*
* @param[in]       instance : Used to identify an advertising set
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_start(uint8_t instance);
#endif

/**
* @brief           This function is used to request the Controller to disable the periodic advertising for the advertising set specified
*
* @param[in]       instance : Used to identify an advertising set
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_stop(uint8_t instance);

/**
* @brief           This function is used to set the extended scan parameters to be used on the advertising channels.
*
* @param[in]       params : scan parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_set_ext_scan_params(const esp_ble_ext_scan_params_t *params);

/**
* @brief           This function is used to enable scanning.
*
* @param[in]       duration  Scan duration time, where Time = N * 10 ms. Range: 0x0001 to 0xFFFF.
* @param[in]       period    Time interval from when the Controller started its last Scan Duration until it begins the subsequent Scan Duration.
*                            Time = N * 1.28 sec. Range: 0x0001 to 0xFFFF.
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_start_ext_scan(uint32_t duration, uint16_t period);

/**
* @brief           This function is used to disable scanning.
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_stop_ext_scan(void);

/**
* @brief           This function is used to synchronize with periodic advertising from an advertiser and begin receiving periodic advertising packets.
*
* @param[in]       params : sync parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_create_sync(const esp_ble_gap_periodic_adv_sync_params_t *params);

/**
* @brief           This function is used to cancel the LE_Periodic_Advertising_Create_Sync command while it is pending.
*
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_sync_cancel(void);

/**
* @brief           This function is used to stop reception of the periodic advertising identified by the Sync Handle parameter.
*
* @param[in]       sync_handle : identify the periodic advertiser
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_sync_terminate(uint16_t sync_handle);

/**
* @brief           This function is used to add a single device to the Periodic Advertiser list stored in the Controller
*
* @param[in]       addr_type : address type
* @param[in]       addr : Device Address
* @param[in]       sid : Advertising SID subfield in the ADI field used to identify the Periodic Advertising
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_add_dev_to_list(esp_ble_addr_type_t addr_type,
                                                                         esp_bd_addr_t addr,
                                                                         uint8_t sid);

/**
* @brief           This function is used to remove one device from the list of Periodic Advertisers stored in the Controller.
*                  Removals from the Periodic Advertisers List take effect immediately.
*
* @param[in]       addr_type : address type
* @param[in]       addr : Device Address
* @param[in]       sid : Advertising SID subfield in the ADI field used to identify the Periodic Advertising
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_remove_dev_from_list(esp_ble_addr_type_t addr_type,
                                                                         esp_bd_addr_t addr,
                                                                         uint8_t sid);
/**
* @brief           This function is used to remove all devices from the list of Periodic Advertisers in the Controller.
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_clear_dev(void);

/**
* @brief           This function is used to set aux connection parameters
*
* @param[in]       addr : device address
* @param[in]       phy_mask : indicates the PHY(s) on which the advertising packets should be received on the primary advertising channel and the PHYs for which connection parameters have been specified.
* @param[in]       phy_1m_conn_params : Scan connectable advertisements on the LE 1M PHY. Connection parameters for the LE 1M PHY are provided.
* @param[in]       phy_2m_conn_params : Connection parameters for the LE 2M PHY are provided.
* @param[in]       phy_coded_conn_params : Scan connectable advertisements on the LE Coded PHY. Connection parameters for the LE Coded PHY are provided.
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_prefer_ext_connect_params_set(esp_bd_addr_t addr,
                                                    esp_ble_gap_phy_mask_t phy_mask,
                                                    const esp_ble_gap_conn_params_t *phy_1m_conn_params,
                                                    const esp_ble_gap_conn_params_t *phy_2m_conn_params,
                                                    const esp_ble_gap_conn_params_t *phy_coded_conn_params);

#endif //#if (BLE_50_FEATURE_SUPPORT == TRUE)

#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
/**
* @brief           This function is used to set periodic advertising receive enable
*
* @param[in]       sync_handle : Handle of periodic advertising sync
* @param[in]       enable : Determines whether reporting and duplicate filtering are enabled or disabled
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_recv_enable(uint16_t sync_handle, uint8_t enable);

/**
* @brief           This function is used to transfer periodic advertising sync
*
* @param[in]       addr : Peer device address
* @param[in]       service_data : Service data used by Host
* @param[in]       sync_handle : Handle of periodic advertising sync
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_sync_trans(esp_bd_addr_t addr,
                                              uint16_t service_data, uint16_t sync_handle);

/**
* @brief           This function is used to transfer periodic advertising set info
*
* @param[in]       addr : Peer device address
* @param[in]       service_data : Service data used by Host
* @param[in]       adv_handle : Handle of advertising set
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_periodic_adv_set_info_trans(esp_bd_addr_t addr,
                                                  uint16_t service_data, uint8_t adv_handle);

/**
* @brief           This function is used to set periodic advertising sync transfer params
*
* @param[in]       addr : Peer device address
* @param[in]       params : Params of periodic advertising sync transfer
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_set_periodic_adv_sync_trans_params(esp_bd_addr_t addr,
                                                         const esp_ble_gap_past_params_t *params);
#endif //#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

#if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
* @brief           This function is used to start a test where the DUT generates reference packets
*                  at a fixed interval.
*
* @param[in]       tx_params : DTM Transmitter parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_dtm_tx_start(const esp_ble_dtm_tx_t *tx_params);

/**
* @brief           This function is used to start a test where the DUT receives test reference packets
*                  at a fixed interval.
*
* @param[in]       rx_params : DTM Receiver parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_dtm_rx_start(const esp_ble_dtm_rx_t *rx_params);
#endif //#if (BLE_42_FEATURE_SUPPORT == TRUE)

#if (BLE_50_FEATURE_SUPPORT == TRUE)

/**
* @brief           This function is used to start a test where the DUT generates reference packets
*                  at a fixed interval.
*
* @param[in]       tx_params : DTM Transmitter parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_dtm_enh_tx_start(const esp_ble_dtm_enh_tx_t *tx_params);

/**
* @brief           This function is used to start a test where the DUT receives test reference packets
*                  at a fixed interval.
*
* @param[in]       rx_params : DTM Receiver parameters
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_dtm_enh_rx_start(const esp_ble_dtm_enh_rx_t *rx_params);
#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)

/**
* @brief           This function is used to stop any test which is in progress
*
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_dtm_stop(void);

/**
* @brief           This function is used to clear legacy advertising
*
*
* @return            - ESP_OK : success
*                    - other  : failed
*
*/
esp_err_t esp_ble_gap_clear_advertising(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GAP_BLE_API_H__ */
