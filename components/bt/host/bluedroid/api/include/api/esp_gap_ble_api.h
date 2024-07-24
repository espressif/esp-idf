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

// These macros are used in setting the advertising data.
#define ESP_BLE_ADV_FLAG_LIMIT_DISC         (0x01 << 0)    /*!< Limited Discoverable Mode: Device is in limited discoverable mode.
                                                                It is typically used in scenarios where power is conserved on the device and the device only broadcasts its presence for a specific window of time. */
#define ESP_BLE_ADV_FLAG_GEN_DISC           (0x01 << 1)    /*!< General Discoverable Mode: Device is in general discoverable mode.
                                                                It is suitable for scenarios where you want the device to be discovered and connected by other BLE devices at any time. */
#define ESP_BLE_ADV_FLAG_BREDR_NOT_SPT      (0x01 << 2)    /*!< BR/EDR Not Supported: Device does not support (Classic Bluetooth) Basic Rate or (Classic Bluetooth) Enhanced Data Rate.
                                                                Ideal for devices that only require BLE functionality. */
#define ESP_BLE_ADV_FLAG_DMT_CONTROLLER_SPT (0x01 << 3)    /*!< Dual-Mode Controller Supported: Device's controller supports both LE and BR/EDR.
                                                                For scenarios that require compatibility between older and newer BLE devices. */
#define ESP_BLE_ADV_FLAG_DMT_HOST_SPT       (0x01 << 4)    /*!< Dual-Mode Host Supported: Device's host supports both LE and BR/EDR.
                                                                Ideal for devices that can handle both protocol stacks, often used for devices that support rich functionality. */
#define ESP_BLE_ADV_FLAG_NON_LIMIT_DISC     (0x00)         /*!< Non-Limited Discoverable Mode: Device is not in limited discoverable mode.
                                                                Typically used to save power or in situations where the device does not want to be discovered. */
typedef uint8_t esp_ble_adv_flag_t;     /*! Group define for `ESP_BLE_ADV_FLAG_xxx`. */

/*
  These macros are used in describing different types of keys used in communication and security protocols.
  Relates to 'BTM_LE_KEY_xxx in stack/btm_api.h' (Reserved for internal use).
*/
#define ESP_LE_KEY_NONE      0                                      /*!< No Encryption Key. */
#define ESP_LE_KEY_PENC                    (1 << 0)                 /*!< Peer Encryption Key: Used for encrypting communication, containing encryption information of the peer device. */
#define ESP_LE_KEY_PID                     (1 << 1)                 /*!< Peer Identity Resolving Key (IRK): Used for resolving the identity of the peer device. */
#define ESP_LE_KEY_PCSRK                   (1 << 2)                 /*!< Peer Connection Signature Resolving Key (CSRK): Used for verifying the authenticity of signed data from the peer device. */
#define ESP_LE_KEY_PLK                     (1 << 3)                 /*!< Peer Link Key: A key used to establish a secure connection with the peer device. */
#define ESP_LE_KEY_LLK                     (ESP_LE_KEY_PLK << 4)    /*!< Local Link Key: Equivalent to the peer link key, used in local device for secure connections. */
#define ESP_LE_KEY_LENC                    (ESP_LE_KEY_PENC << 4)   /*!< Local Encryption Key: Encryption key for the local device, providing master role security information. */
#define ESP_LE_KEY_LID                     (ESP_LE_KEY_PID << 4)    /*!< Local Identity Key: Identity key for the local device, used as the master device ID key. */
#define ESP_LE_KEY_LCSRK                   (ESP_LE_KEY_PCSRK << 4)  /*!< Local Connection Signature Resolving Key (CSRK): Indicates that the local CSRK has been delivered to the peer device. */
typedef uint8_t esp_ble_key_type_t;                                 /*!< Type definition for key types. */

/*
 These macros are used in defining the BLE authentication requirements.
 Relates to BTM_LE_AUTH_xxx in stack/btm_api.h (Reserved for internal use).
*/
#define ESP_LE_AUTH_NO_BOND             0x00                                        /*!< No bonding required: Devices will not store pairing information for future connections. */
#define ESP_LE_AUTH_BOND                0x01                                        /*!< Bonding: Device is bonded with the peer, storing pairing information for future secure connections. */
#define ESP_LE_AUTH_REQ_MITM            (1 << 2)                                    /*!< Man-in-the-Middle (MITM) protection required: Ensures that the communication channel is secure from eavesdropping and tampering. */
#define ESP_LE_AUTH_REQ_BOND_MITM       (ESP_LE_AUTH_BOND | ESP_LE_AUTH_REQ_MITM)   /*!<  Bonding with MITM Protection: Devices are bonded and the connection is protected against MITM attacks.*/
#define ESP_LE_AUTH_REQ_SC_ONLY         (1 << 3)                                    /*!< Secure Connections (SC) only: Uses Secure Connections for stronger security compared to Legacy Pairing. */
#define ESP_LE_AUTH_REQ_SC_BOND         (ESP_LE_AUTH_BOND | ESP_LE_AUTH_REQ_SC_ONLY)        /*!< Bonding with Secure Connections: Devices are bonded using Secure Connections. */
#define ESP_LE_AUTH_REQ_SC_MITM         (ESP_LE_AUTH_REQ_MITM | ESP_LE_AUTH_REQ_SC_ONLY)    /*!< Secure Connections with MITM Protection: Ensures the connection uses Secure Connections and is protected against MITM attacks. */
#define ESP_LE_AUTH_REQ_SC_MITM_BOND    (ESP_LE_AUTH_REQ_MITM | ESP_LE_AUTH_REQ_SC_ONLY | ESP_LE_AUTH_BOND)     /*!< Secure Connections with MITM Protection and Bonding: Devices are bonded using Secure Connections and are protected against MITM attacks. */
typedef uint8_t esp_ble_auth_req_t;                                                                             /*!< Type definition for Bluetooth Low Energy authentication requirements. */

// These macros are used in settings for authentication enable or disable.
#define ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE    0   /*!< The device will accept all connection requests, regardless of whether the other party is authenticated or not. */
#define ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE     1   /*!< The device only accepts connection requests with the specified authentication method to ensure the security of the connected device. */

// These macros are used in settings for  Out of Band data authentication enable or disable.
#define ESP_BLE_OOB_DISABLE    0   /*!< Out of Band data authentication disabled. The local device does not have the peer device's out-of-band authentication data. */
#define ESP_BLE_OOB_ENABLE     1   /*!< Out of Band data authentication enabled. The local device has the peer device's out-of-band authentication data. */

/*
 These macros define the various input/output capabilities.
 Relate to BTM_IO_CAP_xxx in stack/btm_api.h (Reserved for internal use).
*/
#define ESP_IO_CAP_OUT                      0   /*!< Display Only: Device has output capabilities only, typically a display. */
#define ESP_IO_CAP_IO                       1   /*!< Display and Yes/No Input: Device has both output capabilities (display) and simple input capabilities (Yes/No buttons). */
#define ESP_IO_CAP_IN                       2   /*!< Keyboard Only: Device has input capabilities only, typically a keyboard. */
#define ESP_IO_CAP_NONE                     3   /*!< No Input and No Output: Device has neither input nor output capabilities. */
#define ESP_IO_CAP_KBDISP                   4   /*!< Keyboard and Display: Device has both input capabilities (keyboard) and output capabilities (display). */
typedef uint8_t esp_ble_io_cap_t;      /*!< combination of the io capability */

/*
 These macros define the appearance.
 Relates to BTM_BLE_APPEARANCE_xxx in stack/btm_ble_api.h (Reserved for internal use).
*/
#define ESP_BLE_APPEARANCE_UNKNOWN                 0x0000   /*!< Device appearance is unknown. */
#define ESP_BLE_APPEARANCE_GENERIC_PHONE           0x0040   /*!< Generic phone. */
#define ESP_BLE_APPEARANCE_GENERIC_COMPUTER        0x0080   /*!< Generic computer. */
#define ESP_BLE_APPEARANCE_GENERIC_WATCH           0x00C0   /*!< Generic watch. */
#define ESP_BLE_APPEARANCE_SPORTS_WATCH            0x00C1   /*!< Sports watch. */
#define ESP_BLE_APPEARANCE_GENERIC_CLOCK           0x0100   /*!< Generic clock. */
#define ESP_BLE_APPEARANCE_GENERIC_DISPLAY         0x0140   /*!< Generic display. */
#define ESP_BLE_APPEARANCE_GENERIC_REMOTE          0x0180   /*!< Generic remote. */
#define ESP_BLE_APPEARANCE_GENERIC_EYEGLASSES      0x01C0   /*!< Generic eyeglasses. */
#define ESP_BLE_APPEARANCE_GENERIC_TAG             0x0200   /*!< Generic tag. */
#define ESP_BLE_APPEARANCE_GENERIC_KEYRING         0x0240   /*!< Generic keyring. */
#define ESP_BLE_APPEARANCE_GENERIC_MEDIA_PLAYER    0x0280   /*!< Generic barcode scanner. */
#define ESP_BLE_APPEARANCE_GENERIC_BARCODE_SCANNER 0x02C0   /*!< Generic thermometer. */
#define ESP_BLE_APPEARANCE_GENERIC_THERMOMETER     0x0300   /*!< Generic thermometer. */
#define ESP_BLE_APPEARANCE_THERMOMETER_EAR         0x0301   /*!< Thermometer (ear). */
#define ESP_BLE_APPEARANCE_GENERIC_HEART_RATE      0x0340   /*!< Generic heart rate monitor. */ 
#define ESP_BLE_APPEARANCE_HEART_RATE_BELT         0x0341   /*!< Heart rate monitor (belt). */  
#define ESP_BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE  0x0380   /*!< Generic blood pressure monitor. */
#define ESP_BLE_APPEARANCE_BLOOD_PRESSURE_ARM      0x0381   /*!< Blood pressure monitor (arm). */
#define ESP_BLE_APPEARANCE_BLOOD_PRESSURE_WRIST    0x0382   /*!< Blood pressure monitor (wrist). */
#define ESP_BLE_APPEARANCE_GENERIC_HID             0x03C0   /*!< Generic HID (Human Interface Device). */
#define ESP_BLE_APPEARANCE_HID_KEYBOARD            0x03C1   /*!< HID keyboard. */
#define ESP_BLE_APPEARANCE_HID_MOUSE               0x03C2   /*!< HID mouse. */
#define ESP_BLE_APPEARANCE_HID_JOYSTICK            0x03C3   /*!< HID joystick. */
#define ESP_BLE_APPEARANCE_HID_GAMEPAD             0x03C4   /*!< HID game pad. */ 
#define ESP_BLE_APPEARANCE_HID_DIGITIZER_TABLET    0x03C5   /*!< HID digitizer tablet. */
#define ESP_BLE_APPEARANCE_HID_CARD_READER         0x03C6   /*!< HID card reader. */ 
#define ESP_BLE_APPEARANCE_HID_DIGITAL_PEN         0x03C7   /*!< HID digital pen. */
#define ESP_BLE_APPEARANCE_HID_BARCODE_SCANNER     0x03C8   /*!< HID barcode scanner. */  
#define ESP_BLE_APPEARANCE_GENERIC_GLUCOSE         0x0400   /*!< Generic glucose meter. */
#define ESP_BLE_APPEARANCE_GENERIC_WALKING         0x0440   /*!< Generic pedometer. */ 
#define ESP_BLE_APPEARANCE_WALKING_IN_SHOE         0x0441   /*!< Pedometer (in shoe). */
#define ESP_BLE_APPEARANCE_WALKING_ON_SHOE         0x0442   /*!< Pedometer (on shoe). */ 
#define ESP_BLE_APPEARANCE_WALKING_ON_HIP          0x0443   /*!< Pedometer (on hip). */
#define ESP_BLE_APPEARANCE_GENERIC_CYCLING         0x0480   /*!< Generic cycling device. */  
#define ESP_BLE_APPEARANCE_CYCLING_COMPUTER        0x0481   /*!< Cycling computer. */  
#define ESP_BLE_APPEARANCE_CYCLING_SPEED           0x0482   /*!< Cycling speed sensor. */
#define ESP_BLE_APPEARANCE_CYCLING_CADENCE         0x0483   /*!< Cycling cadence sensor. */ 
#define ESP_BLE_APPEARANCE_CYCLING_POWER           0x0484   /*!< Cycling power meter. */  
#define ESP_BLE_APPEARANCE_CYCLING_SPEED_CADENCE   0x0485   /*!< Cycling speed and cadence sensor. */
#define ESP_BLE_APPEARANCE_STANDALONE_SPEAKER      0x0841   /*!< Standalone speaker. */ 
#define ESP_BLE_APPEARANCE_GENERIC_PULSE_OXIMETER  0x0C40   /*!< Generic pulse oxi meter. */ 
#define ESP_BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP 0x0C41  /*!< Pulse oxi meter (fingertip). */
#define ESP_BLE_APPEARANCE_PULSE_OXIMETER_WRIST    0x0C42   /*!< Pulse oxi meter (wrist). */
#define ESP_BLE_APPEARANCE_GENERIC_WEIGHT          0x0C80   /*!< Generic weight scale. */
#define ESP_BLE_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE    0x0CC0  /*!< Generic personal mobility device. */
#define ESP_BLE_APPEARANCE_POWERED_WHEELCHAIR                  0x0CC1  /*!< Powered wheelchair. */
#define ESP_BLE_APPEARANCE_MOBILITY_SCOOTER                    0x0CC2  /*!< Mobility scooter. */
#define ESP_BLE_APPEARANCE_GENERIC_CONTINUOUS_GLUCOSE_MONITOR  0x0D00  /*!< Generic continuous glucose monitor. */ 
#define ESP_BLE_APPEARANCE_GENERIC_INSULIN_PUMP                0x0D40  /*!< Generic insulin pump. */ 
#define ESP_BLE_APPEARANCE_INSULIN_PUMP_DURABLE_PUMP           0x0D41  /*!< Insulin pump (durable). */ 
#define ESP_BLE_APPEARANCE_INSULIN_PUMP_PATCH_PUMP             0x0D44  /*!< Insulin pump (patch). */
#define ESP_BLE_APPEARANCE_INSULIN_PEN                         0x0D48  /*!< Insulin pen. */
#define ESP_BLE_APPEARANCE_GENERIC_MEDICATION_DELIVERY         0x0D80  /*!< Generic medication delivery device. */ 
#define ESP_BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS  0x1440              /*!< Generic outdoor sports device. */  
#define ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION 0x1441              /*!< Outdoor sports device with location. */
#define ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV     0x1442  /*!< Outdoor sports device with location and navigation. */
#define ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD         0x1443  /*!< Outdoor sports location pod. */ 
#define ESP_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV 0x1444  /*!< Outdoor sports location pod with navigation. */
typedef uint16_t esp_ble_icon_t;    /*!< Group define for `ESP_BLE_APPEARANCE_xxx`. */

// These macros define the Direct Test Mode (DTM) testing.
#define BLE_DTM_PKT_PAYLOAD_0x00   0x00      /*!< This is used for Direct Test Mode (DTM) testing to ensure accurate data transmission. Pseudo-Random Binary Sequence 9 (PRBS9) sequence '11111111100000111101...' (in transmission order) as described in [Vol 6] Part F, Section 4.1.5. */
#define BLE_DTM_PKT_PAYLOAD_0x01   0x01      /*!< This is a fixed pattern used in DTM testing to check the receiver's bit error rate. repeated '11110000' */
#define BLE_DTM_PKT_PAYLOAD_0x02   0x02      /*!< This pattern is also used for DTM testing, providing an alternating bit sequence. repeated '10101010' */
#define BLE_DTM_PKT_PAYLOAD_0x03   0x03      /*!< This provides a longer pseudo-random sequence for more rigorous DTM testing. Pseudo-Random Binary Sequence 15 (PRBS15) sequence */
#define BLE_DTM_PKT_PAYLOAD_0x04   0x04      /*!< This sequence is used in DTM to test continuous transmission of high bits. Repeated '11111111' (in transmission order) sequence. */
#define BLE_DTM_PKT_PAYLOAD_0x05   0x05      /*!< This sequence tests continuous transmission of low bits in DTM. Repeated '00000000' (in transmission order) sequence. */
#define BLE_DTM_PKT_PAYLOAD_0x06   0x06      /*!< This pattern is used for DTM testing to verify the receiver's ability to handle different bit patterns. Repeated '00001111' (in transmission order) sequence. */
#define BLE_DTM_PKT_PAYLOAD_0x07   0x07      /*!< This alternating pattern is used in DTM to test receiver performance with rapid bit changes. Repeated '01010101' (in transmission order) sequence. */
#define BLE_DTM_PKT_PAYLOAD_MAX    0x08      /*!< Any value from 0x08 to 0xFF is not currently used and is reserved for potential future extensions to the DTM packet payloads. Reserved for future use. */
typedef uint8_t esp_ble_dtm_pkt_payload_t;   /*!< Bluetooth Low Energy Direct Test Mode Packet Payload Type */


/**
 * @brief Generic access profile Bluetooth LE callback event type
 */
typedef enum {
/* BLE_42_FEATURE_SUPPORT */
    ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT        = 0,       /*!< When advertising data set complete, the event comes. */
    ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT,             /*!< When scan response data set complete, the event comes. */
    ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT,                /*!< When scan parameters set complete, the event comes. */
    ESP_GAP_BLE_SCAN_RESULT_EVT,                            /*!< When one scan result ready, the event comes each time. */
    ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT,              /*!< When raw advertising data set complete, the event comes. */
    ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT,         /*!< When raw scan response data set complete, the event comes. */
    ESP_GAP_BLE_ADV_START_COMPLETE_EVT,                     /*!< When start advertising complete, the event comes. */
    ESP_GAP_BLE_SCAN_START_COMPLETE_EVT,                    /*!< When start scan complete, the event comes. */

/* BLE_INCLUDED */
    ESP_GAP_BLE_AUTH_CMPL_EVT = 8,                          /*!< When authentication completes, this event is generated. */
    ESP_GAP_BLE_KEY_EVT,                                    /*!< When bluetooth low energy peer device keys are available, this event occurs. */
    ESP_GAP_BLE_SEC_REQ_EVT,                                /*!< When a bluetooth low energy security request is made, this event is generated. */
    ESP_GAP_BLE_PASSKEY_NOTIF_EVT,                          /*!< When a passkey notification is received, this event occurs. */
    ESP_GAP_BLE_PASSKEY_REQ_EVT,                            /*!< When a passkey request is initiated, this event is generated. */
    ESP_GAP_BLE_OOB_REQ_EVT,                                /*!< When an Out-of-Band (OOB) request is made, this event occurs. */
    ESP_GAP_BLE_LOCAL_IR_EVT,                               /*!< When the BLE local IR (Identity Root) event occurs, this event is generated. */
    ESP_GAP_BLE_LOCAL_ER_EVT,                               /*!< When the BLE local ER (Encryption Root) event occurs, this event is generated. */
    ESP_GAP_BLE_NC_REQ_EVT,                                 /*!< When a Numeric Comparison request is made, this event occurs. */

/* BLE_42_FEATURE_SUPPORT */
    ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT,                      /*!< When stop advertisement complete, the event comes. */
    ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT,                     /*!< When stop scan complete, the event comes. */

/* BLE_INCLUDED */
    ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT = 19,              /*!< When set the static rand address complete, the event comes. */
    ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT,                     /*!< When update connection parameters complete, the event comes. */
    ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT,                /*!< When set packet length complete, the event comes. */
    ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT,             /*!< When  Enable/disable privacy on the local device complete, the event comes. */
    ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT,               /*!< When remove the bond device complete, the event comes. */
            ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT,                /*!< When clear the bond device clear complete, the event comes. Don't use it now, may remove when major update. */
            ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT,                  /*!< When get the bond device list complete, the event comes. Don't use it now, may remove when major update. */
    ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT,                     /*!< When read the received signal strength indication complete, the event comes. */
    ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT,              /*!< When add or remove whitelist complete, the event comes. */

/* BLE_42_FEATURE_SUPPORT */
    ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT,  /*!< When update duplicate exceptional list complete, the event comes. */

/* BLE_INCLUDED */
    ESP_GAP_BLE_SET_CHANNELS_EVT = 29,                           /*!< When setting bluetooth low energy channels complete, the event comes. */

/* BLE_50_FEATURE_SUPPORT */
    ESP_GAP_BLE_READ_PHY_COMPLETE_EVT,                           /*!< When reading physical layer (phy) complete, this event comes. */
    ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT,          /*!< When preferred default physical layer complete, this event comes. */
    ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT,                  /*!< When preferred physical layer complete , this event comes. */
    ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT,              /*!< When extended set random address complete, the event comes. */
    ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT,                 /*!< When extended advertising parameter complete, the event comes. */
    ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT,                   /*!< When extended advertising data complete, the event comes. */
    ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT,              /*!< When extended scan response data complete, the event comes. */
    ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT,                      /*!< When extended advertising start complete, the event comes. */
    ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT,                       /*!< When extended advertising stop complete, the event comes. */
    ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT,                 /*!< When extended advertising set remove complete, the event comes. */
    ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT,                  /*!< When extended advertising set clear complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT,            /*!< When periodic advertising parameter complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT,              /*!< When periodic advertising data complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT,                 /*!< When periodic advertising start complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT,                  /*!< When periodic advertising stop complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT,           /*!< When periodic advertising create synchronize complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT,           /*!< When extended advertising synchronize cancel complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT,        /*!< When extended advertising synchronize terminate complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT,               /*!< When extended advertising add device complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT,            /*!< When extended advertising remove device complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT,             /*!< When extended advertising clear device, the event comes. */
    ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT,                /*!< When extended scan parameter complete, the event comes. */
    ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT,                     /*!< When extended scan start complete, the event comes. */
    ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT,                      /*!< When extended scan stop complete, the event comes. */
    ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT,         /*!< When extended prefer connection parameter set complete, the event comes. */
    ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT,                         /*!< When bluetooth low energy physical layer update complete, the event comes. */
    ESP_GAP_BLE_EXT_ADV_REPORT_EVT,                              /*!< When extended advertising report complete, the event comes. */
    ESP_GAP_BLE_SCAN_TIMEOUT_EVT,                                /*!< When scan timeout complete, the event comes. */
    ESP_GAP_BLE_ADV_TERMINATED_EVT,                              /*!< When advertising terminate data complete, the event comes. */
    ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT,                           /*!< When scan req received complete, the event comes. */
    ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT,                    /*!< When channel select algorithm complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT,                         /*!< When periodic report advertising complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT,                      /*!< When periodic advertising synchronize lost complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT,                     /*!< When periodic advertising synchronize establish complete, the event comes. */

/* BLE_INCLUDED */
    ESP_GAP_BLE_SC_OOB_REQ_EVT,                                  /*!< When Secure Connection Out-of-Band request, the event comes.  */
    ESP_GAP_BLE_SC_CR_LOC_OOB_EVT,                               /*!< When Secure Connection create Out-of-Band data complete, the event comes. */
    ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT,                       /*!< When getting bluetooth device name complete, the event comes. */

/* if Bluetooth Low Energy Feature Periodic Advertising Synchronization Transfer Support */
    ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT,           /*!< When set periodic advertising receive enable complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT,            /*!< When periodic advertising sync transfer complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT,        /*!< When periodic advertising set info transfer complete, the event comes. */
    ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT,                    /*!< When set periodic advertising sync transfer params complete, the event comes. */
    ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_RECV_EVT,                /*!< When periodic advertising sync transfer received, the event comes. */

/* direct test mode */
    ESP_GAP_BLE_DTM_TEST_UPDATE_EVT,                             /*!< When direct test mode state changes, the event comes. */

/* BLE_INCLUDED */
    ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT,                          /*!< When clear advertising complete, the event comes. */
    ESP_GAP_BLE_SET_RPA_TIMEOUT_COMPLETE_EVT,                    /*!< When set the Resolvable Private Address (RPA) timeout completes, the event comes. */
    ESP_GAP_BLE_ADD_DEV_TO_RESOLVING_LIST_COMPLETE_EVT,          /*!< When add a device to the resolving list completes, the event comes. */
    ESP_GAP_BLE_VENDOR_CMD_COMPLETE_EVT,                         /*!< When vendor Host Controller Interface command complete, the event comes. */
    ESP_GAP_BLE_SET_PRIVACY_MODE_COMPLETE_EVT,                   /*!< When set privacy mode complete, the event comes. */
    ESP_GAP_BLE_EVT_MAX,                                         /*!< When maximum advertising event complete, the event comes. */
} esp_gap_ble_cb_event_t;

#define ESP_GAP_BLE_CHANNELS_LEN     5                           /*!< Channel Map length. 37 bits (bit 0~36) meaningful. Includes 37 data channel and 3 reserved channels. */
typedef uint8_t esp_gap_ble_channels[ESP_GAP_BLE_CHANNELS_LEN];  /*!< Array type representing Bluetooth LE Adaptive Frequency Hopping (AFH) channel map. */

#define ESP_GAP_BLE_ADD_WHITELIST_COMPLETE_EVT ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT    /*!< This is the old name, just for backwards compatibility. */
#define ESP_BLE_ADV_DATA_LEN_MAX               31                                           /*!< Advertising data maximum length. */
#define ESP_BLE_SCAN_RSP_DATA_LEN_MAX          31                                           /*!< Scan response data maximum length. */
#define VENDOR_HCI_CMD_MASK                    (0x3F << 10)                                 /*!< Mask to extract vendor-specific Host Controller Interface command opcode from a 16-bit value: 0xFC00. */

/**
 * @brief The type of advertising data(not adv_type).
 * Relates to BTM_BLE_AD_TYPE_xxx in stack/btm_ble_api.h (Reserved for internal use).
 */
typedef enum {
    ESP_BLE_AD_TYPE_FLAG                     = 0x01,    /*!< Flags for discovery and other Bluetooth LE capabilities. */
    ESP_BLE_AD_TYPE_16SRV_PART               = 0x02,    /*!< Partial list of 16-bit service UUIDs. */
    ESP_BLE_AD_TYPE_16SRV_CMPL               = 0x03,    /*!< Complete list of 16-bit service UUIDs. */
    ESP_BLE_AD_TYPE_32SRV_PART               = 0x04,    /*!< Partial list of 32-bit service UUIDs. */
    ESP_BLE_AD_TYPE_32SRV_CMPL               = 0x05,    /*!< Complete list of 32-bit service UUIDs. */
    ESP_BLE_AD_TYPE_128SRV_PART              = 0x06,    /*!< Partial list of 128-bit service UUIDs. */
    ESP_BLE_AD_TYPE_NAME_SHORT               = 0x08,    /*!< Shortened device name. */
    ESP_BLE_AD_TYPE_NAME_CMPL                = 0x09,    /*!< Complete device name. */
    ESP_BLE_AD_TYPE_TX_PWR                   = 0x0A,    /*!< Transmit power level. */
    ESP_BLE_AD_TYPE_DEV_CLASS                = 0x0D,    /*!< Device class. */
    ESP_BLE_AD_TYPE_SM_TK                    = 0x10,    /*!< Security Manager TK value. */
    ESP_BLE_AD_TYPE_SM_OOB_FLAG              = 0x11,    /*!< Security Manager Out of Band flags. */
    ESP_BLE_AD_TYPE_INT_RANGE                = 0x12,    /*!< Slave connection interval range. */
    ESP_BLE_AD_TYPE_SOL_SRV_UUID             = 0x14,    /*!< List of 16-bit service solicitation UUIDs. */
    ESP_BLE_AD_TYPE_128SOL_SRV_UUID          = 0x15,    /*!< List of 128-bit service solicitation UUIDs. */
    ESP_BLE_AD_TYPE_SERVICE_DATA             = 0x16,    /*!< Service data. */
    ESP_BLE_AD_TYPE_PUBLIC_TARGET            = 0x17,    /*!< Public target address. */
    ESP_BLE_AD_TYPE_RANDOM_TARGET            = 0x18,    /*!< Random target address. */
    ESP_BLE_AD_TYPE_APPEARANCE               = 0x19,    /*!< Appearance. */
    ESP_BLE_AD_TYPE_ADV_INT                  = 0x1A,    /*!< Advertising interval. */
    ESP_BLE_AD_TYPE_LE_DEV_ADDR              = 0x1b,    /*!< Bluetooth LE device address. */
    ESP_BLE_AD_TYPE_LE_ROLE                  = 0x1c,    /*!< Bluetooth LE role. */
    ESP_BLE_AD_TYPE_SPAIR_C256               = 0x1d,    /*!< Pairing data, C256. */
    ESP_BLE_AD_TYPE_SPAIR_R256               = 0x1e,    /*!< Pairing data, R256. */
    ESP_BLE_AD_TYPE_32SOL_SRV_UUID           = 0x1f,    /*!< List of 32-bit service solicitation UUIDs. */
    ESP_BLE_AD_TYPE_32SERVICE_DATA           = 0x20,    /*!< 32-bit service data. */
    ESP_BLE_AD_TYPE_128SERVICE_DATA          = 0x21,    /*!< 128-bit service data. */
    ESP_BLE_AD_TYPE_LE_SECURE_CONFIRM        = 0x22,    /*!< Bluetooth LE secure connections confirmation value. */
    ESP_BLE_AD_TYPE_LE_SECURE_RANDOM         = 0x23,    /*!< Bluetooth LE secure connections random value. */
    ESP_BLE_AD_TYPE_URI                      = 0x24,    /*!< Uniform Resource Identifier. */
    ESP_BLE_AD_TYPE_INDOOR_POSITION          = 0x25,    /*!< Indoor positioning. */
    ESP_BLE_AD_TYPE_TRANS_DISC_DATA          = 0x26,    /*!< Transport discovery data. */
    ESP_BLE_AD_TYPE_LE_SUPPORT_FEATURE       = 0x27,    /*!< Bluetooth LE supported features. */
    ESP_BLE_AD_TYPE_CHAN_MAP_UPDATE          = 0x28,    /*!< Channel map update indication. */
    ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE    = 0xFF,    /*!< Manufacturer specific data. */
} esp_ble_adv_data_type;

/**
 * @brief Advertising type
 *        - This enum represents the different types of Bluetooth Low Energy (LE) advertising types. Each types indicate how a device advertises its presence and availability for connections.
 *        - Use the connectable type if there is a subsequent need for the application to connect; use the scannable type if there is subsequent access to more scanning information.
 *        - More detail in spec: BLUETOOTH CORE SPECIFICATION Version 5.4 Vol 4, Part E and BLUETOOTH CORE SPECIFICATION Version 5.4 Vol 6, Part B 4.4.2.
 *        - Directed advertising exclude `adv_data`.
 */
typedef enum {
    ADV_TYPE_IND                = 0x00, /*!< Connectable and scannable undirected advertising (ADV_IND PDU).
                                             - Advertiser will send the connectable and scannable undirected packet,
                                               allowing a scanner or initiator to respond with
                                               either a scan request (SCAN_REQ PDU) or connect request (CONNECT_IND PDU). */
    ADV_TYPE_DIRECT_IND_HIGH    = 0x01, /*!< Connectable high duty cycle directed advertising (ADV_DIRECT_IND PDU, high duty cycle).
                                             - Advertising will send the connectable directed packet, allowing only the addressed initiator
                                               to respond with a connect request (CONNECT_IND PDU)
                                               so that both the advertiser and initiator will enter the Connection state.
                                             - This type can only using a high duty cycle (3.75 ms) to connect faster.
                                             - The Adv_Int_Min and Adv_Int_Max parameters are not used and shall be ignored. */
    ADV_TYPE_SCAN_IND           = 0x02, /*!< Scannable undirected advertising (ADV_SCAN_IND PDU). 
                                             - Advertiser will send the scannable undirected packet,
                                               allowing only a scanner to respond with a scan request (SCAN_REQ PDU)
                                               to request additional information about the advertiser. 
                                             - Not allowed to send connect request. */
    ADV_TYPE_NONCONN_IND        = 0x03, /*!< Non connectable undirected advertising (ADV_NONCONN_IND PDU). 
                                             - Advertiser will send the non-connectable and non-scannable undirected packet,
                                               allowing only a scanner to receive information from the advertiser.
                                             - Scanners are not allowed to send scanning requests, nor are they allowed to establish connections directly. */
    ADV_TYPE_DIRECT_IND_LOW     = 0x04  /*!< Connectable low duty cycle directed advertising (ADV_DIRECT_IND PDU, low duty cycle).
                                             - Advertiser will send the connectable directed packet,
                                               allowing only the addressed initiator to respond with a connect request (CONNECT_IND PDU)
                                               so that both the advertiser and initiator will enter the Connection state.
                                             - A user-set time limit will be used, and there will be no timeout stops. */
} esp_ble_adv_type_t;

/**
 * @brief Advertising channel mask
 *        - These channels are used for advertising packets in LE. At least one of these channels should be used for advertising. E.g.: ADV_CHNL_37 | ADV_CHNL_38.
 */
typedef enum {
    ADV_CHNL_37     = (1 << 0), /*!< Indicates the use of advertising channel 37 for advertising packets. */
    ADV_CHNL_38     = (1 << 1), /*!< Indicates the use of advertising channel 38 for advertising packets. */
    ADV_CHNL_39     = (1 << 2), /*!< Indicates the use of advertising channel 39 for advertising packets. */
    ADV_CHNL_ALL    = ADV_CHNL_37 | ADV_CHNL_38 | ADV_CHNL_39, /*!< Indicates that all three advertising channels (37, 38, and 39) are used for advertising packets. */
} esp_ble_adv_channel_t;

/**
 * @brief Advertising filter policies
 *        - These policies control how scan and connection requests are filtered during advertising. The filters can restrict requests based on whether the requesters are on the White List.
 *        - Update the White List by using `esp_ble_gap_update_whitelist`.
 */
typedef enum {
    ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY  = 0x00, /*!< Allow both scan and connection requests from any device. */

    ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = 0x01, /*!< Allow scan requests only from devices on the White List, but allow connection requests from any device. */

    ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = 0x02, /*!< Allow scan requests from any device, but allow connection requests only from devices on the White List. */

    ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = 0x03 /*!< Allow both scan and connection requests only from devices on the White List. */
} esp_ble_adv_filter_t;

/**
 * @brief Security actions related to `BTA_DM_BLE_SEC_xxx` in `bta/bta_api.h` (Reserved for internal use).
 * - These security actions determine the level of encryption and authentication required for LE connections.
 */
typedef enum {
    ESP_BLE_SEC_ENCRYPT = 1,     /*!< 
                                      - If the device has already bonded, the stack will use the Long Term Key (LTK) to encrypt communication with the remote device directly.
                                      - If the device hasn't bonded, the stack will use the default authentication request (no Man-In-The-Middle (MITM) authentication request).
                                        set by the user in `esp_ble_gap_set_security_param`. */
    ESP_BLE_SEC_ENCRYPT_NO_MITM, /*!< 
                                      - If the device has already bonded, the stack will check if the LTK meets the authentication request
                                        - If so, use it to encrypt communication with the remote device directly. 
                                        - Otherwise, it will re-pair with the remote device.
                                      - If the device hasn't bonded, the stack will use no MITM authentication request in the current link instead of
                                        the MITM authentication set by the user in `esp_ble_gap_set_security_param`. */
    ESP_BLE_SEC_ENCRYPT_MITM,    /*!< 
                                      - If the device has already bonded, the stack will check if the Long Term Key (LTK) meets the authentication request.
                                        - If so, use it to encrypt communication with the remote device directly.
                                        - Otherwise, it will re-pair with the remote device.
                                      - If the device hasn't bonded, the stack will use MITM authentication request in the current link instead of
                                        the MITM authentication set by the user in `esp_ble_gap_set_security_param`. */
} esp_ble_sec_act_t;

/**
 * @brief Enum types of security management param.
 */
typedef enum {
    ESP_BLE_SM_PASSKEY = 0,             /*!< Authentication method using passkey input on the local device size 6 byte.
                                             - Value: A 6-digit passkey value (uint32_t) between 000,000 and 999,999.
                                             - Scenario: User inputs a numeric passkey, and the remote device verifies it to ensure a secure pairing process.
                                             - E.g. Devices that require user input for PIN codes (6 bytes), such as smart locks or car systems. */
    ESP_BLE_SM_AUTHEN_REQ_MODE,         /*!< Authentication mode requirements of the local device, size 1 byte (unit_8).
                                             - Scenario: Defines the local device's need for pairing, encryption, and whether to use MITM (Man-In-The-Middle) protection or secure connections.
                                             - See `esp_ble_auth_req_t` for details. 
                                             - Group define for `ESP_LE_AUTH_xxx`. 
                                             - E.g. Devices with high security needs, like financial payment devices. */
    ESP_BLE_SM_IOCAP_MODE,              /*!< Input/output capabilities of the local device in pairing, size 1 byte (unit_8).
                                             - Scenario: Defines whether the device supports keyboard input, display output, or has no I/O capabilities, which affects the pairing method.
                                             - See `esp_bt_io_cap_t` for details. 
                                             - Group define for `ESP_BT_IO_CAP_xxx`. */
    ESP_BLE_SM_SET_INIT_KEY,            /*!< Initiator key distribution during pairing, size 1 byte (unit_8).
                                             - Scenario: Defines the types of keys distributed by the local device during pairing.
                                             - See `esp_ble_key_mask_t` for details.
                                             - Group define for `ESP_BLE_xxx_KEY_MASK`. */
    ESP_BLE_SM_SET_RSP_KEY,             /*!< Responder key distribution during pairing, the length of the value is 1 byte (uint8_t).
                                             - Scenario: Defines the types of keys distributed by the local device during pairing.
                                             - See `esp_ble_key_mask_t` for details.
                                             - Group define for `ESP_BLE_xxx_KEY_MASK`. */
    ESP_BLE_SM_MAX_KEY_SIZE,            /*!< Maximum encryption key size supported by the device.
                                             - The length of the value is 1 byte (uint8_t). The maximum key size shall be in the range 7 to 16 octets.
                                             - Scenario: Limits the maximum length of the encryption key that the local device can generate or accept. */
    ESP_BLE_SM_MIN_KEY_SIZE,            /*!< Minimum encryption key size required from the peer device, size 1 byte (7-16).
                                             - The length of the value is 1 byte (uint8_t). The maximum key size shall be in the range 7 to 16 octets.
                                             - Scenario: Ensures that the paired device meets the minimum security requirements, preventing weak encryption. */
    ESP_BLE_SM_SET_STATIC_PASSKEY,      /*!< Set a static passkey for pairing.
                                             - The value is a 6-digit passkey value (uint32_t) between 000,000 and 999,999.
                                             - Scenario: Uses a fixed passkey for pairing instead of a randomly generated one. */
    ESP_BLE_SM_CLEAR_STATIC_PASSKEY,    /*!< Clear previously set static passkey.
                                             - Scenario: Removes the static passkey to allow pairing using the default random passkey or other security methods. */
    ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH,  /*!< Accept only specified security authentication requirements, size 1 byte.
                                                     - Scenario: Ensures that the device only communicates with other devices that meet specific security standards, preventing insecure connections.
                                                     - Group define for `ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_xxx`. */
    ESP_BLE_SM_OOB_SUPPORT,             /*!< Out of Band (OOB) data support for pairing.
                                             - The length of the value is 1 byte (uint8_t).
                                             - Group define for `ESP_BLE_OOB_xxx`.
                                             - Scenario: Uses OOB data for pairing in scenarios where pairing information is not transmitted over LE broadcast channels.
                                             - E.g. High-security environments, such as pairing via NFC or QR code. */
    ESP_BLE_APP_ENC_KEY_SIZE,           /*!< Note: `ESP_BLE_APP_ENC_KEY_SIZE` is deprecated. */
    ESP_BLE_SM_MAX_PARAM,               /*!< End marker for the enumeration range of security manager parameters. */
} esp_ble_sm_param_t;

/**
 * @brief These annotations clarify the purpose of each enum member related to Direct Test Mode (DTM) testing in Bluetooth Low Energy.
 */
typedef enum {
    DTM_TX_START_EVT = 0x00,    /*!< Event generated when the Direct Test Mode TX test starts. */
    DTM_RX_START_EVT,           /*!< Event generated when the Direct Test Mode RX test starts. */
    DTM_TEST_STOP_EVT,          /*!< Event generated when the Direct Test Mode test ends. */
} esp_ble_dtm_update_evt_t;

/**
 * @brief Vendor HCI (Host Controller Interface) command parameters.
 */
typedef struct {
    uint16_t opcode;            /*!< Vendor HCI command opcode. */
    uint8_t param_len;          /*!< Length of the parameter. */
    uint8_t *p_param_buf;       /*!< Pointer to the parameter buffer. */
} esp_ble_vendor_cmd_params_t;

// if Bluetooth Low Energy 4.2 Feature Support
#if (BLE_42_FEATURE_SUPPORT == TRUE)
/**
* @brief Direct Test Mode TX parameters
*/
typedef struct
{
    uint8_t                         tx_channel;            /*!< channel for sending test data, tx_channel = (Frequency -2402)/2, tx_channel range:0x00-0x27, Frequency range: 2402 MHz to 2480 MHz */
    uint8_t                         len_of_data;           /*!< Length in bytes of payload data in each packet. */
    esp_ble_dtm_pkt_payload_t       pkt_payload;           /*!< Packet payload type. Value range: 0x00-0x07 */
} esp_ble_dtm_tx_t;

/**
* @brief Direct Test Mode RX parameters
*/
typedef struct
{
    uint8_t          rx_channel;            /*!< Channel for test data reception, rx_channel = (Frequency -2402)/2, tx_channel range:0x00-0x27, Frequency range: 2402 MHz to 2480 MHz */
} esp_ble_dtm_rx_t;

/**
 * @brief Advertising parameters.
 *        - More details in spec: BLUETOOTH CORE SPECIFICATION Version 5.4 Vol 4, Part E 7.8.5.
 */
typedef struct {
    uint16_t adv_int_min;                   /*!< Minimum advertising interval for undirected and low duty cycle directed advertising.
                                                 - Unit: 0.625ms.
                                                 - Range: 0x0020 to 0x4000 (20ms to 10.24s).
                                                 - Note: Expand use for setting smaller adv_int by setting in `menuconfig` first. */
    uint16_t adv_int_max;                   /*!< Maximum advertising interval for undirected and low duty cycle directed advertising.
                                                 - Unit: 0.625 ms.
                                                 - Range: 0x0020 to 0x4000 (20ms to 10.24s).
                                                 - adv_int_max shall be equal to or greater than adv_int_min, when both are equal, the interval is definite,
                                                   otherwise value in [adv_int_min,adv_int_max].
                                                 - Note: There will be a random delay (range:0 to 10ms) in each interval.*/
    esp_ble_adv_type_t adv_type;            /*!< Type of advertising. */
    esp_ble_addr_type_t own_addr_type;      /*!< Address type of the local device. */
    esp_bd_addr_t peer_addr;                /*!< Bluetooth device address of the peer device. */
    esp_ble_addr_type_t peer_addr_type;     /*!< Address type of the peer device. You can only set this to `BLE_ADDR_TYPE_PUBLIC` or `BLE_ADDR_TYPE_RANDOM`. */
    esp_ble_adv_channel_t channel_map;      /*!< Channel map for advertising. */
    esp_ble_adv_filter_t adv_filter_policy; /*!< Advertising filter policy. */
} esp_ble_adv_params_t;

/**
 * @brief Advertising/Scan response data structure. See more in Supplement to the Bluetooth Core Specification CSS v10, Part A.
 *        - All settings are optional, except for the `set_scan_rsp configuration`,
 *          the remaining configuration items are not selected (set to `0`) indicate that the current package does not contain the configuration item.
 *        - E.g.: Send an empty advertisement package, set `set_scan_rsp` to `false`, set others to `0`.
 *               Send an empty scan response package, set `set_scan_rsp` to `true`, set others to `0`.
 */
typedef struct {
    bool set_scan_rsp;              /*!< Whether data set as scan response data or advertising data.
                                         - True indicates a scan response data, false indicates an advertising data. */
    bool include_name;              /*!< Whether data includes the device name or not.
                                         - True indicates the device name is included, false indicates not included.
                                         - Please use `esp_ble_gap_set_device_name` to set device name, after that please use `esp_err_t esp_ble_gap_config_adv_data` again. */
    bool include_txpower;           /*!< Whether data includes TX power or not.
                                         - True indicates TX power is included, false indicates not included.
                                         - Please use `esp_ble_tx_power_set` to set TX power, after that please use `esp_err_t esp_ble_gap_config_adv_data` again. */
    int min_interval;               /*!< Slave preferred connection minimum interval.
                                         - Unit: 1.25ms.
                                         - Range: 0x0006 to 0x0C80 (7.5ms to 4s). */
    int max_interval;               /*!< Slave preferred connection maximum interval.
                                         - Unit: 1.25ms.
                                         - Range: 0x0006 to 0x0C80 (7.5ms to 4s).
                                         - max_interval shall be equal to or greater than min_interval. */
    int appearance;                 /*!< External appearance of the device.
                                         - The value is defined by the Bluetooth SIG and represents the appearance of the device.
                                         - See `esp_ble_icon_t` for details. 
                                         - Group define for `ESP_BLE_APPEARANCE_xxx`. */
    uint16_t manufacturer_len;      /*!< Manufacturer data length in bytes. */
    uint8_t *p_manufacturer_data;   /*!< Pointer to the manufacturer data.
                                         - Manufacturer data is typically used to transmit device vendor-specific information. */
    uint16_t service_data_len;      /*!< Service data length in bytes. */
    uint8_t *p_service_data;        /*!< Pointer to the service data.
                                         - The Service Data data type consists of a service UUID with the data associated with that service. */
    uint16_t service_uuid_len;      /*!< Service UUID list length in bytes. */
    uint8_t *p_service_uuid;        /*!< Pointer to the service UUID list.
                                         - The Service UUID data type is used to include a list of Service or Service Class UUIDs.
                                         - The UUID in the list is 16/32/128-bit Service UUIDs. */
    uint8_t flag;                   /*!< Advertising flag of discovery mode.
                                         - Used to specify the discovery mode and function of the device.
                                         - See `esp_ble_adv_flag_t` for details.
                                         - Group define for `ESP_BLE_ADV_FLAG_xxx`. */
} esp_ble_adv_data_t;// #if (BLE_42_FEATURE_SUPPORT == TRUE)
#endif

/**
 * @brief Bluetooth Low Energy (LE) scan type
 *  - If you need a scan response(`set_scan_rsp`) use `BLE_SCAN_TYPE_ACTIVE`.
 */
typedef enum {
    BLE_SCAN_TYPE_PASSIVE   =   0x0, /*!< Passive scan: No scan request PDUs are sent. */
    BLE_SCAN_TYPE_ACTIVE    =   0x1, /*!< Active scan: Scan request PDUs may be sent. */
} esp_ble_scan_type_t;

/**
 * @brief Bluetooth Low Energy (LE) scan filter type.
 * @note
 *        - Whitelist is used to specify the device. See more in spec BLUETOOTH CORE SPECIFICATION Version 5.4 Vol 6, Part B 4.3.1.
 */
typedef enum {
    BLE_SCAN_FILTER_ALLOW_ALL           = 0x0,  /*!< - Advertising and scan response Protocol Data Unit (PDU) are processed from all devices. (default). */
    BLE_SCAN_FILTER_ALLOW_ONLY_WLST     = 0x1,  /*!< - Accept Advertising packets from devices where the advertiser's address is in the White list.
                                                     - Accept directed advertising packets addressed to this device. */
    BLE_SCAN_FILTER_ALLOW_UND_RPA_DIR   = 0x2,  /*!< - Accept Undirected advertisement packets.
                                                     - Accept Directed advertising packets where the target address is a resolvable private address.
                                                     - Accept directed advertising packets addressed to this device. */
    BLE_SCAN_FILTER_ALLOW_WLIST_RPA_DIR = 0x3,  /*!< - Accept advertising packets from devices where the advertiser's address is in the White list.
                                                     - Accept directed advertising packets where the target address is a resolvable private address.
                                                     - Accept directed advertising packets addressed to this device. */
} esp_ble_scan_filter_t;

/**
 * @brief Bluetooth Low Energy (LE) scan duplicate type.
 *       - When setting to `BLE_SCAN_DUPLICATE_ENABLE` will reduce the number of broadcast packets.
 */
typedef enum {
    BLE_SCAN_DUPLICATE_DISABLE           = 0x0,  /*!< The Link Layer should generate advertising reports to the host for each packet received.
                                                      - This means every advertising packet, including duplicates, will be reported.
                                                      - Default: `adv_addr` 
                                                      Change duplicate types in `menuconfig`: `BT_CTRL_SCAN_DUPL_TYPE`
                                                      - `Scan Duplicate By Device Address (default)
                                                      - `Scan Duplicate By Advertising Data`
                                                      - `Scan Duplicate By Device Address And Advertising Data` */
    BLE_SCAN_DUPLICATE_ENABLE            = 0x1,  /*!< The Link Layer should filter out duplicate advertising reports to the host.
                                                      - This means only unique advertising packets will be reported, reducing redundant data. */

/* if Bluetooth Low Energy 5.0 Feature Support */  
    #if (BLE_50_FEATURE_SUPPORT == TRUE)
    BLE_SCAN_DUPLICATE_ENABLE_RESET,             /*!< Duplicate filtering enabled, reset for each scan period, only supported in bluetooth low energy 5.0 can not use in `esp_ble_gap_set_scan_params'. */
    #endif// #if (BLE_50_FEATURE_SUPPORT == TRUE)
    BLE_SCAN_DUPLICATE_MAX                       /*!< Reserved for future use. */
} esp_ble_scan_duplicate_t;

// if Bluetooth Low Energy 4.2 Feature Support
#if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief Bluetooth Low Energy (LE) scan parameters
 * - See more in BLUETOOTH CORE SPECIFICATION Version 5.4 Vol 4, Part E 7.8.10.
 */
typedef struct {
    esp_ble_scan_type_t      scan_type;              /*!< Scan type, specifies the type of scan to perform. */
    esp_ble_addr_type_t      own_addr_type;          /*!< Owner address type, specifies the Bluetooth address type used by the scanning device. */
    esp_ble_scan_filter_t    scan_filter_policy;     /*!< Scan filter policy, the scanning filter policy will be applied when receiving advertising packet or scan response packet. */
    uint16_t                 scan_interval;          /*!< Scan interval, Time between the start of two consecutive scanning windows.
                                                          - Unit: 0.625ms.
                                                          - Range: `0x0004` to `0x4000` (2.5ms to 10.24s). */
    uint16_t                 scan_window;            /*!< Scan window, Duration of the LE scan.
                                                          - Must be less than or equal to scan_interval.
                                                          - Unit: 0.625ms.
                                                          - Range: `0x0004` to `0x4000` (2.5ms to 10.24s). */
    esp_ble_scan_duplicate_t scan_duplicate;         /*!< Scan duplicates setting, Controls whether duplicate advertising reports are filtered.
                                                          - `scan_window` and `scan_duplicate` have different meanings, e.g. When setting `scan_duplicate` to 1s, `scan_window` to 0.8s,
                                                          in a two seconds duration the device will scan two times, each scan last 0.8s and switch to other channel and wait until next scan. */
} esp_ble_scan_params_t;
#endif// #if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief Connection parameters information for a Bluetooth Low Energy (LE) connection.
 */
typedef struct {
    uint16_t interval;                  /*!< Connection interval, defines the frequently the devices will exchange data.
                                             - Unit = 1.25ms
                                             - Range: 0x0006 (7.5ms) to 0x0C80 (4s) */
    uint16_t latency;                   /*!< Slave latency for the connection, specifies the number of connection events the slave can skip.
                                             - Range: 0x0000 to 0x01F3 (499)
                                             - A latency of 0 means no skipped events. */
    uint16_t timeout;                   /*!< Supervision timeout for the connection, defines the time the connection will be maintained without activity.
                                             - Time = 10ms
                                             - Range: 0x000A (100ms) to 0x0C80 (32s) */
} esp_gap_conn_params_t;

/**
 * @brief Parameters for updating LE connection parameters.
 */
typedef struct {
    esp_bd_addr_t bda;                              /*!< Bluetooth device address for which the connection parameters are being updated. */
    uint16_t min_int;                               /*!< Minimum connection interval.
                                                         - Unit = 1.25ms
                                                         - Range: 0x0006 (7.5ms) to 0x0C80 (4s) */
    uint16_t max_int;                               /*!< Maximum connection interval.
                                                         - Unit = 1.25ms
                                                         - Range: 0x0006 (7.5ms) to 0x0C80 (4s)
                                                         - `max_int` shall be equal to or greater than `min_int`, when both are equal, the interval is definite,
                                                            otherwise value in [min_int,max_int]. */
    uint16_t latency;                               /*!< Slave latency for the connection, specifies the number of connection events the slave can skip.
                                                         - Range: 0x0000 to 0x01F3 (499)
                                                         - A latency of 0 means no skipped events. */
    uint16_t timeout;                               /*!< Supervision timeout for the LE connection, defines the time the connection will be maintained without activity.
                                                         - Time = 10ms
                                                         - Range: 0x000A (100ms) to 0x0C80 (32s) */
} esp_ble_conn_update_params_t;

/**
 * @brief Packet data length parameters for Bluetooth Low Energy (BLE).
 */
typedef struct {
    uint16_t rx_len;                   /*!< Maximum length of received packets, defines the maximum size of incoming data packets.
                                           Value range: Dependent on BLE stack and controller configuration. */
    uint16_t tx_len;                   /*!< Maximum length of transmitted packets, defines the maximum size of outgoing data packets.
                                           Value range: Dependent on BLE stack and controller configuration. */
} esp_ble_pkt_data_length_params_t;

/**
 * @brief Bluetooth Low Energy (LE) encryption keys.
 *  - The key type used for encryption in LE security.
 */
typedef struct {
    esp_bt_octet16_t     ltk;          /*!< Long Term Key (LTK) used for encryption of data between devices.
                                            - Used for decrypting data in future connections. */
    esp_bt_octet8_t      rand;         /*!< Random Number used during the pairing process to ensure encryption keys are unique. */
    uint16_t             ediv;         /*!< Encrypted Diversifier (EDIV) used to generate the encryption key.
                                            - A 16-bit value generated during pairing to ensure key uniqueness. */
    uint8_t              sec_level;    /*!< Security level of the link.

                                           1: No Security (No Authentication, No Encryption)

                                           2: Security Mode 1 (No Encryption, Authentication Required)

                                           3: Security Mode 2 (Encryption, Authentication Optional)

                                           4: Security Mode 3 (Encryption with MITM Protection, Authentication Required)
                                           */
    uint8_t              key_size;     /*!< Key size used in the security link. Specifies the length of the encryption key in bytes.
                                            - Unit: byte
                                            - Range: 7 to 16 */
} esp_ble_penc_keys_t;

/**
 * @brief Bluetooth Low Energy (LE) connection signature resolving keys.
 *  - The Privacy-Enhanced Connection Signature Resolving Key type.
 */
typedef struct {
    uint32_t            counter;      /*!< Counter used to track the number of packets processed.
                                           - Ensures the uniqueness of each signature in the connection. */
    esp_bt_octet16_t    csrk;         /*!< Connection Signature Resolving Key (CSRK) used to sign data packets.
                                           - A 128-bit key used for integrity and authentication of data. */
    uint8_t             sec_level;    /*!< Security level associated with the CSRK.
                                           - Indicates the level of security used for signature resolution. */
} esp_ble_pcsrk_keys_t;

/**
 * @brief Bluetooth Low Energy (LE) pairing identification keys.
 * These keys are used for device identification and privacy.
 */
typedef struct {
    esp_bt_octet16_t          irk;           /*!< Identity Resolving Key (IRK) used to resolve device identity and protect privacy.*/
    esp_ble_addr_type_t       addr_type;     /*!< Address type of the device.
                                                  Defines whether the address is public or random. */
    esp_bd_addr_t             static_addr;   /*!< Static address used for identity resolution. */
} esp_ble_pid_keys_t;

/**
 * @brief Bluetooth Low Energy (BLE) encryption reproduction keys.
 * These keys are used for encryption in BLE security and are derived from the Long Term Key (LTK).
 */
typedef struct {
    esp_bt_octet16_t  ltk;                  /*!< Long Term Key (LTK) used for encryption. */
    uint16_t          div;                  /*!< Diversifier value used in the key generation. */
    uint8_t           key_size;             /*!< Key size of the security link. */
    uint8_t           sec_level;            /*!< Security level of the security link. */
} esp_ble_lenc_keys_t;

/**
 * @brief Bluetooth Low Energy (BLE) signature resolving keys.
 * These keys are used to resolve connection signatures and ensure data integrity.
 */
typedef struct {
    uint32_t          counter;              /*!< Counter value used to maintain uniqueness of signatures.
                                            Tracks the number of packets processed. */
    uint16_t          div;                  /*!< Diversify value used in signature generation.
                                            A 16-bit value that helps in generating unique signatures. */
    uint8_t           sec_level;            /*!< Security level of the signature resolving key.
                                            Indicates the level of security applied to the signature. */
    esp_bt_octet16_t  csrk;                 /*!< Connection Signature Resolving Key (CSRK) used for signing data packets. */
} esp_ble_lcsrk_keys_t;

/**
 * @brief Structure associated with ESP_KEY_NOTIF_EVT (Key Notification Event).
 * Contains information about key notifications in BLE security events.
 */
typedef struct {
    esp_bd_addr_t  bd_addr;        /*!< Bluetooth device address of the peer.
                                    Identifies the remote device involved in the key exchange. */
    uint32_t       passkey;        /*!< Numeric value for comparison during pairing.
                                    If the pairing method is Just Works, this value may not be displayed to the user. */
} esp_ble_sec_key_notif_t;

/**
 * @brief Structure for Bluetooth Low Energy (BLE) security requests.
 * Contains information related to security requests from a peer device.
 */
typedef struct {
    esp_bd_addr_t  bd_addr;        /*!< Bluetooth device address of the peer.
                                        Identifies the remote device making the security request. */
} esp_ble_sec_req_t;

/**
 * @brief Union type for Bluetooth Low Energy (BLE) key values.
 * This union allows for different types of BLE keys to be accessed in a single structure.
 */
typedef union {
    esp_ble_penc_keys_t   penc_key;       /*!< Received peer encryption key (Long Term Key, LTK).
                                                Used for encrypting data in BLE connections. */
    esp_ble_pcsrk_keys_t  pcsrk_key;      /*!< Received peer device Secure Resolving Key (CSRK).
                                                Used for signing data packets to ensure integrity. */
    esp_ble_pid_keys_t    pid_key;        /*!< Peer device Identity Resolving Key (IRK).
                                                Used for device identity resolution and privacy. */
    esp_ble_lenc_keys_t   lenc_key;       /*!< Local encryption reproduction keys derived from LTK.
                                                Used for local encryption operations. */
    esp_ble_lcsrk_keys_t  lcsrk_key;      /*!< Local device Connection Signature Resolving Key (CSRK).
                                                Used for local signing of data packets. */
} esp_ble_key_value_t;

/**
 * @brief Structure type for bond key information.
 *  - Contains the bond key information for a bonded device.
 */
typedef struct {
    esp_ble_key_mask_t    key_mask;       /*!< The key mask to indicate which keys are present.
                                                - Determining which keys are available for a specific bonded device. */
    esp_ble_penc_keys_t   penc_key;       /*!< Received peer encryption key (Long Term Key, LTK).
                                                - Used for decrypting data from the bonded device. */
    esp_ble_pcsrk_keys_t  pcsrk_key;      /*!< Received peer device Secure Resolving Key (CSRK).
                                                - Used for verifying signatures from the bonded device. */
    esp_ble_pid_keys_t    pid_key;        /*!< Peer device Identity Resolving Key (IRK).
                                                - Used for device identity and privacy management. */
} esp_ble_bond_key_info_t;

/**
 * @brief Structure type for bonded device information.
 *  - Contains information about a bonded device and its associated bond key.
 */
typedef struct {
    esp_bd_addr_t  bd_addr;               /*!< Bluetooth device address of the bonded peer.
                                                Identifies the remote device. */
    esp_ble_bond_key_info_t bond_key;     /*!< Bond key information associated with the bonded device. */
} esp_ble_bond_dev_t;          

/**
 * @brief Structure type for security key value.
 *  - Contains information about the security key and its type.
 */
typedef struct {
    esp_bd_addr_t               bd_addr;        /*!< Bluetooth device address of the peer.
                                                     - Identifies the remote device for which the security key is associated. */
    esp_ble_key_type_t          key_type;       /*!< Key type of the security link.
                                                     -Indicates the type of key being used. */
    esp_ble_key_value_t         p_key_value;    /*!< Pointer to the key value.
                                                     - Points to the specific key data based on the `key_type`. */
} esp_ble_key_t;

/**
 * @brief Structure for Bluetooth Low Energy (LE) local identity keys.
 *  - Contains local identity keys used in the BLE pairing process.
 */
typedef struct {
    esp_bt_octet16_t       ir;                  /*!< The 16 bytes of the Identity Root value.
                                                     - Used to derive Identity Resolving Key (IRK) in LE. */
    esp_bt_octet16_t       irk;                 /*!< The 16 bytes of the Identity Resolving Key (IRK).
                                                     - Used to resolve device identity. */
    esp_bt_octet16_t       dhk;                 /*!< The 16 bytes of the Diffie-Hellman key (DHK).
                                                     - Used for secure key exchange in LE pairing. */
} esp_ble_local_id_keys_t;

/**
 * @brief Structure for Bluetooth Low Energy (BLE) local Out-of-Band (OOB) data.
 *  - Contains OOB data used for secure pairing when OOB is employed.
 */
typedef struct {
    esp_bt_octet16_t        oob_c;              /*!< The 128-bit confirmation value.
                                                     - Used to confirm the pairing process in OOB scenarios. */
    esp_bt_octet16_t        oob_r;              /*!< The 128-bit random value.
                                                     - Used to generate secure pairing data. */
} esp_ble_local_oob_data_t;

/**
 * @brief Enumeration of authentication failure reasons in BLE pairing.
 * Lists possible reasons for authentication failure.
 */
typedef enum {
    // Failure reason defined in Bluetooth Core Spec 5.0 Vol3, Part H, 3.5.5
    ESP_AUTH_SMP_PASSKEY_FAIL = 78,         /*!< User input of passkey failed. */
    ESP_AUTH_SMP_OOB_FAIL,                  /*!< OOB data is not available. */
    ESP_AUTH_SMP_PAIR_AUTH_FAIL,            /*!< Authentication requirements cannot be met. */
    ESP_AUTH_SMP_CONFIRM_VALUE_FAIL,        /*!< Confirm value does not match the calculated comparison value. */
    ESP_AUTH_SMP_PAIR_NOT_SUPPORT,          /*!< Pairing is not supported by the device. */
    ESP_AUTH_SMP_ENC_KEY_SIZE,              /*!< Resultant encryption key size is not long enough. */
    ESP_AUTH_SMP_INVALID_CMD,               /*!< SMP command received is not supported. */
    ESP_AUTH_SMP_UNKNOWN_ERR,               /*!< Pairing failed due to an unspecified reason. */
    ESP_AUTH_SMP_REPEATED_ATTEMPT,          /*!< Pairing or authentication procedure is disallowed. */
    ESP_AUTH_SMP_INVALID_PARAMETERS,        /*!< Command length is invalid or a parameter is out of range. */
    ESP_AUTH_SMP_DHKEY_CHK_FAIL,            /*!< DHKey Check value received does not match the calculated value. */
    ESP_AUTH_SMP_NUM_COMP_FAIL,             /*!< Confirm values in the numeric comparison protocol do not match. */
    ESP_AUTH_SMP_BR_PARING_IN_PROGR,        /*!< Pairing Request over BR/EDR transport is in progress. */
    ESP_AUTH_SMP_XTRANS_DERIVE_NOT_ALLOW,   /*!< BR/EDR Link Key or BLE LTK cannot be used for derivation. */
    // Failure reason defined in Bluedroid Host
    ESP_AUTH_SMP_INTERNAL_ERR,              /*!< Internal error in pairing procedure. */
    ESP_AUTH_SMP_UNKNOWN_IO,                /*!< Unknown IO capability, unable to decide association model. */
    ESP_AUTH_SMP_INIT_FAIL,                 /*!< SMP pairing initiation failed. */
    ESP_AUTH_SMP_CONFIRM_FAIL,              /*!< Confirm value does not match. */
    ESP_AUTH_SMP_BUSY,                      /*!< Pending security request ongoing. */
    ESP_AUTH_SMP_ENC_FAIL,                  /*!< Controller failed to start encryption. */
    ESP_AUTH_SMP_STARTED,                   /*!< SMP pairing process started. */
    ESP_AUTH_SMP_RSP_TIMEOUT,               /*!< Security Manager timeout due to no SMP command received. */
    ESP_AUTH_SMP_DIV_NOT_AVAIL,             /*!< Encrypted Diversify value not available. */
    ESP_AUTH_SMP_UNSPEC_ERR,                /*!< Unspecified failure reason. */
    ESP_AUTH_SMP_CONN_TOUT,                 /*!< Pairing process failed due to connection timeout. */
} esp_ble_auth_fail_rsn_t;

/**
 * @brief Structure associated with `ESP_AUTH_CMPL_EVT`.
 *  - Represents the Bluetooth Low Energy authentication complete callback type.
 */
typedef struct {
    esp_bd_addr_t         bd_addr;               /*!< Bluetooth device address of the peer device. */
    bool                  key_present;           /*!< Indicates if a valid link key is present. */
    esp_link_key          key;                   /*!< Link key associated with the peer device. */
    uint8_t               key_type;              /*!< The type of Link Key (e.g., Long Term Key, Short Term Key). */
    bool                  success;               /*!< TRUE if authentication succeeded, FALSE if failed. */
    uint8_t               fail_reason;           /*!< Error code for failure, if authentication failed. */
    esp_ble_addr_type_t   addr_type;             /*!< Peer device address type. */
    esp_bt_dev_type_t     dev_type;              /*!< Device type. */
    esp_ble_auth_req_t    auth_mode;             /*!< Authentication mode used. */
} esp_ble_auth_cmpl_t;

/**
 * @brief Union type associated with Bluetooth Low Energy (BLE) security.
 * Contains different types of BLE security-related data.
 */
typedef union
{
    esp_ble_sec_key_notif_t    key_notif;      /*!< Passkey notification information. */
    esp_ble_sec_req_t          ble_req;        /*!< BLE Security Manager Protocol (SMP) related request information. */
    esp_ble_key_t              ble_key;        /*!< BLE SMP keys used during pairing. */
    esp_ble_local_id_keys_t    ble_id_keys;    /*!< Local Identity Root keys and related values. */
    esp_ble_local_oob_data_t   oob_data;       /*!< OOB data for secure connection. */
    esp_ble_auth_cmpl_t        auth_cmpl;      /*!< Authentication completion information. */
} esp_ble_sec_t;  

// if Bluetooth Low Energy 4.2 Feature Support
#if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief Sub-events of the ESP_GAP_BLE_SCAN_RESULT_EVT event.
 * These sub-events provide detailed results of the BLE scan operation.
 */
typedef enum {
    ESP_GAP_SEARCH_INQ_RES_EVT             = 0,      /*!< Inquiry result for a peer device.
                                                     Indicates that a device matching the search criteria has been found. */
    ESP_GAP_SEARCH_INQ_CMPL_EVT            = 1,      /*!< Inquiry complete.
                                                     Indicates that the inquiry process has been completed. */
    ESP_GAP_SEARCH_DISC_RES_EVT            = 2,      /*!< Discovery result for a peer device.
                                                     Provides information about a device discovered during the discovery process. */
    ESP_GAP_SEARCH_DISC_BLE_RES_EVT        = 3,      /*!< Discovery result for BLE GATT-based service on a peer device.
                                                     Provides details about a BLE GATT service discovered on a peer device. */
    ESP_GAP_SEARCH_DISC_CMPL_EVT           = 4,      /*!< Discovery complete.
                                                     Indicates that the discovery process has been completed. */
    ESP_GAP_SEARCH_DI_DISC_CMPL_EVT        = 5,      /*!< Discovery complete.
                                                     Used for discovery-related processes, indicating completion. */
    ESP_GAP_SEARCH_SEARCH_CANCEL_CMPL_EVT  = 6,      /*!< Search cancelled.
                                                     Indicates that the search operation has been cancelled. */
    ESP_GAP_SEARCH_INQ_DISCARD_NUM_EVT     = 7,      /*!< Number of packets discarded by flow control.
                                                     Provides the count of packets discarded due to flow control limitations. */
} esp_gap_search_evt_t;
#endif// #if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief Bluetooth Low Energy (BLE) scan result event types.
 * These types indicate the nature of the BLE advertising or scan response data.
 */
typedef enum {
    ESP_BLE_EVT_CONN_ADV         = 0x00,        /*!< Connectable undirected advertising (ADV_IND).
                                                     Advertising packets that allow a device to connect to the advertiser. */
    ESP_BLE_EVT_CONN_DIR_ADV     = 0x01,        /*!< Connectable directed advertising (ADV_DIRECT_IND).
                                                     Advertising packets that are directed at a specific device with the intention to establish a connection. */
    ESP_BLE_EVT_DISC_ADV         = 0x02,        /*!< Scannable undirected advertising (ADV_SCAN_IND).
                                                     Advertising packets that allow devices to scan for the advertiser but do not allow direct connection. */
    ESP_BLE_EVT_NON_CONN_ADV     = 0x03,        /*!< Non-connectable undirected advertising (ADV_NONCONN_IND).
                                                     Advertising packets that do not support connection requests and are typically used for advertising information. */
    ESP_BLE_EVT_SCAN_RSP         = 0x04,        /*!< Scan Response (SCAN_RSP).
                                                     Response packets sent after a scan request to provide additional information about the advertiser. */
} esp_ble_evt_type_t;

/**
 * @brief Bluetooth Low Energy (BLE) whitelist operation types.
 * These operations manage devices in the whitelist for scanning and connection filtering.
 */
typedef enum {
    ESP_BLE_WHITELIST_REMOVE     = 0x00,    /*!< Remove a device from the whitelist.
                                                 Used to exclude a specific device from the whitelist. */
    ESP_BLE_WHITELIST_ADD        = 0x01,    /*!< Add a MAC address to the whitelist.
                                                 Used to include a specific device in the whitelist. */
    ESP_BLE_WHITELIST_CLEAR      = 0x02,    /*!< Clear all devices from the whitelist.
                                                 Removes all devices from the whitelist. */
} esp_ble_wl_operation_t;

// if Bluetooth Low Energy 4.2 Feature Support 
#if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief Bluetooth Low Energy (BLE) duplicate exceptional list operations.
 * This enumeration defines the types of operations that can be performed on the duplicate scan exceptional list.
 */
typedef enum {
    ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_ADD      = 0,  /*!< Add device information into the duplicate scan exceptional list.
                                                         This operation includes specific device information in the list to handle exceptional cases for duplicate scans. */
    ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_REMOVE,        /*!< Remove device information from the duplicate scan exceptional list.
                                                         This operation removes specific device information from the list, effectively excluding it from exceptional handling. */
    ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_CLEAN,         /*!< Clean the duplicate scan exceptional list.
                                                         This operation clears all entries from the exceptional list, resetting it to an empty state. */
} esp_bt_duplicate_exceptional_subcode_type_t;
#endif// #if (BLE_42_FEATURE_SUPPORT == TRUE)

#define BLE_BIT(n) (1UL<<(n))   /*!< This macro defines a bitwise operation to create a bitmask with only the n-th bit set. */

// if Bluetooth Low Energy 4.2 Feature Support 
#if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief Enumeration for different types of exceptional scan information in Bluetooth Low Energy (LE).
 *  - This enumeration defines various types of exceptional scan information that can be used to manage
 *    specific types of LE advertising and scanning data. These types help in identifying and categorizing
 *    exceptional devices or advertisements based on their specific characteristics.
 */
typedef enum {
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_ADV_ADDR       = 0,  /*!< Advertising Address: Represents a Bluetooth Low Energy (BLE) advertising address.
                                                                    Devices with this type of information are added to the ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_ADDR_LIST.
                                                                    This list is used to manage devices based on their advertising addresses. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_LINK_ID,        /*!< Mesh Link ID: Represents the BLE mesh link ID. This is used specifically for BLE mesh networks.
                                                                    Devices with this type of information are added to the ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_LINK_ID_LIST.
                                                                    This list helps in managing devices within a BLE mesh network based on their link IDs. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_BEACON_TYPE,    /*!< Mesh Beacon Type: Represents the BLE mesh beacon advertising data type.
                                                                    The format includes | Length | 0x2B | Beacon Type | Beacon Data |.
                                                                    This type is used to handle and manage BLE mesh beacons, which broadcast specific beacon types and data. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROV_SRV_ADV,   /*!< Mesh Provisioning Service Advertising: Represents the BLE mesh provisioning service UUID in advertising.
                                                                    The format includes | 0x02 | 0x01 | Flags | 0x03 | 0x03 | 0x1827 | .... |.
                                                                    This type helps in identifying devices advertising the mesh provisioning service, which is used for setting up mesh networks. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROXY_SRV_ADV,  /*!< Mesh Proxy Service Advertising: Represents the BLE mesh proxy service UUID in advertising.
                                                                    The format includes | 0x02 | 0x01 | Flags | 0x03 | 0x03 | 0x1828 | .... |.
                                                                    This type is used to identify devices advertising the mesh proxy service, which allows communication between different parts of a mesh network. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_PROXY_SOLIC_ADV,  /*!< Mesh Proxy Solicitation Advertising: Represents BLE mesh proxy solicitation advertising with a specific UUID.
                                                                    The format includes | 0x02 | 0x01 | Flags | 0x03 | 0x03 | 0x1859 | .... |.
                                                                    This type is used for identifying devices that are soliciting connections through proxy services in a BLE mesh network. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_INFO_MESH_URI_ADV,        /*!< Mesh Uniform Resource Identifier (URI) Advertising: Represents BLE mesh URI advertising.
                                                                    The format includes ...| Length | 0x24 | Data |...
                                                                    This type helps in managing devices that advertise URIs, which can provide resources or information relevant to a BLE mesh network. */
} esp_ble_duplicate_exceptional_info_type_t;

/**
 * @brief Enumeration for different types of duplicate scan exceptional lists in Bluetooth Low Energy (BLE).
 * This enumeration defines various lists that manage exceptional cases for duplicate scanning.
 * These lists help in identifying and categorizing exceptional devices or advertisements based on specific characteristics.
 */
typedef enum {
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_ADDR_LIST                  = BLE_BIT(0),  /*!< Duplicate Scan Exceptional Address List: This list contains BLE advertising addresses that are considered exceptions for duplicate scanning.
                                                                                    Devices with addresses in this list will not be treated as duplicates. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_LINK_ID_LIST          = BLE_BIT(1),  /*!< Duplicate Scan Exceptional Mesh Link ID List: This list contains BLE mesh link IDs that are considered exceptions for duplicate scanning.
                                                                                    Devices with link IDs in this list will not be treated as duplicates within the mesh network. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_BEACON_TYPE_LIST      = BLE_BIT(2),  /*!< Duplicate Scan Exceptional Mesh Beacon Type List: This list contains BLE mesh beacon types that are considered exceptions for duplicate scanning. 
                                                                                    Beacon types in this list will not be treated as duplicates. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_PROV_SRV_ADV_LIST     = BLE_BIT(3),  /*!< Duplicate Scan Exceptional Mesh Provisioning Service Advertising List: This list contains BLE mesh provisioning service UUIDs that are considered exceptions for duplicate scanning.
                                                                                    Devices advertising provisioning services with UUIDs in this list will not be treated as duplicates. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_PROXY_SRV_ADV_LIST    = BLE_BIT(4),  /*!< Duplicate Scan Exceptional Mesh Proxy Service Advertising List: This list contains BLE mesh proxy service UUIDs that are considered exceptions for duplicate scanning.
                                                                                    Devices advertising proxy services with UUIDs in this list will not be treated as duplicates. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_PROXY_SOLIC_ADV_LIST  = BLE_BIT(5),  /*!< Duplicate Scan Exceptional Mesh Proxy Solicitation Advertising List: This list contains BLE mesh proxy solicitation UUIDs that are considered exceptions for duplicate scanning.
                                                                                    Devices advertising proxy solicitation with UUIDs in this list will not be treated as duplicates. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_MESH_URI_ADV_LIST          = BLE_BIT(6),  /*!< Duplicate Scan Exceptional Mesh Uniform Resource Identifier (URI) Advertising List: This list contains BLE mesh URIs that are considered exceptions for duplicate scanning. 
                                                                                    Devices advertising URIs in this list will not be treated as duplicates. */
    ESP_BLE_DUPLICATE_SCAN_EXCEPTIONAL_ALL_LIST                   = 0xFFFF,      /*!< Duplicate Scan Exceptional All List: This list includes all types of BLE exceptional cases for duplicate scanning. 
                                                                                    Devices in this list, regardless of their specific characteristics, will not be treated as duplicates. */
} esp_duplicate_scan_exceptional_list_type_t;

typedef uint8_t esp_duplicate_info_t[ESP_BD_ADDR_LEN];

#endif// #if (BLE_42_FEATURE_SUPPORT == TRUE)

/* if Bluetooth Low Energy 5.0 Feature Support */ 
#if (BLE_50_FEATURE_SUPPORT == TRUE)

//Definitions for setting extended advertising properties in Bluetooth Low Energy (BLE).
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_NONCONN_NONSCANNABLE_UNDIRECTED      (0 << 0)  /*!< Non-Connectable and Non-Scannable Undirected Advertising: The device does not accept connections or scan requests. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE                          (1 << 0)  /*!< Connectable Advertising: The device accepts connections from other devices. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE                            (1 << 1)  /*!< Scannable Advertising: The device accepts scan requests from other devices, allowing them to request more information. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_DIRECTED                             (1 << 2)  /*!< Directed Advertising: The advertising is targeted towards a specific device, indicated by the Target Address. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_HD_DIRECTED                          (1 << 3)  /*!< High Duty Cycle Directed Connectable Advertising: The device uses a high duty cycle (<= 3.75ms Advertising Interval) for directed connectable advertising, often used for fast reconnection. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY                               (1 << 4)  /*!< Use Legacy Advertising Protocol Data Units: The advertising uses legacy Protocol Data Unit(PDU) formats, compatible with older BLE devices. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_ANON_ADV                             (1 << 5)  /*!< Anonymous Advertising: The advertiser's address is omitted from all PDUs, enhancing privacy. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_INCLUDE_TX_PWR                       (1 << 6)  /*!< Include Transmit Power in Extended Header: The advertising PDU includes the transmit power level in its extended header, allowing receivers to estimate the range. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_MASK                                 (0x7F)    /*!< Reserved for Future Use: This mask covers all defined property bits (0-6), allowing for future expansion. */

/*
 If extended advertising Protocol Data Unit types are being used (bit 4 = 0) then:
 The advertisement shall not be both connectable and scannable.
 High duty cycle directed connectable advertising (<=3.75 ms advertising interval) shall not be used (bit 3 = 0)
*/
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_IND        (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE)     /*!< Legacy Advertising Indication (ADV_IND)
                                                                                                        This macro sets the properties for legacy connectable and scannable undirected advertising.
                                                                                                        Connectable: Allows other devices to initiate a connection.
                                                                                                        Scannable: Allows other devices to send scan requests for more information.
                                                                                                        Legacy: Uses legacy Protocol Data Unit (PDU) formats. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_LD_DIR     (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_DIRECTED)      /*!< Legacy Low Duty Cycle Directed Advertising Indication (ADV_DIRECT_IND)
                                                                                                        This macro sets the properties for legacy connectable and directed low duty cycle advertising.
                                                                                                        Connectable: Allows other devices to initiate a connection.
                                                                                                        Directed: Targets a specific device, indicated by the Target Address.
                                                                                                        Legacy: Uses legacy PDU formats.
                                                                                                        Low Duty Cycle: Does not use high duty cycle (<= 3.75ms advertising interval). */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_HD_DIR     (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_CONNECTABLE |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_HD_DIRECTED)   /*!< Legacy High Duty Cycle Directed Advertising Indication (ADV_DIRECT_IND)
                                                                                                        This macro sets the properties for legacy connectable and directed high duty cycle advertising.
                                                                                                        Connectable: Allows other devices to initiate a connection.
                                                                                                        Directed: Targets a specific device, indicated by the Target Address.
                                                                                                        Legacy: Uses legacy PDU formats.
                                                                                                        High Duty Cycle: Uses high duty cycle (<= 3.75ms advertising interval) for fast reconnection. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_SCAN       (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY |\
                                                        ESP_BLE_GAP_SET_EXT_ADV_PROP_SCANNABLE)     /*!< Legacy Scannable Indication (ADV_SCAN_IND)
                                                                                                        This macro sets the properties for legacy scannable undirected advertising.
                                                                                                        Scannable: Allows other devices to send scan requests for more information.
                                                                                                        Legacy: Uses legacy PDU formats. */
#define ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY_NONCONN    (ESP_BLE_GAP_SET_EXT_ADV_PROP_LEGACY)        /*!< Legacy Non-Connectable Indication (ADV_NONCONN_IND)
                                                                                                        This macro sets the properties for legacy non-connectable undirected advertising.
                                                                                                        Legacy: Uses legacy PDU formats.
                                                                                                        Non-Connectable: Does not allow other devices to initiate a connection. */
typedef uint16_t esp_ble_ext_adv_type_mask_t;

/*
 Bluetooth Low Energy (BLE) Physical Layer settings for secondary advertisements.
 These macros define the physical layer (PHY) options for BLE secondary advertising.
*/
#define ESP_BLE_GAP_PHY_1M                             1    /*!< Low Energy 1 Megabit per second (LE 1M PHY)
                                                                This PHY option is used for BLE secondary advertisements at a data rate of 1Mbps.
                                                                It is the default PHY for most BLE operations, offering a balance between range and data rate. */
#define ESP_BLE_GAP_PHY_2M                             2    /*!< Low Energy 2 Megabit per second (LE 2M PHY)
                                                                This PHY option is used for BLE secondary advertisements at a higher data rate of 2Mbps.
                                                                It provides faster data transfer rates compared to LE 1M PHY, but with a reduced range. */
#define ESP_BLE_GAP_PHY_CODED                          3    /*!< Low Energy Coded PHY (LE Coded PHY).
                                                                This PHY option is used for BLE secondary advertisements with enhanced range capabilities.
                                                                LE Coded PHY uses coding schemes to achieve longer range and better reliability, albeit at lower data rates. 
                                                                It is particularly useful for applications requiring extended range and robustness in signal transmission. */
typedef uint8_t esp_ble_gap_phy_t;

#define ESP_BLE_GAP_NO_PREFER_TRANSMIT_PHY     (1<<0) /*!< No Prefer TX Physical Layer supported by controller. */
#define ESP_BLE_GAP_NO_PREFER_RECEIVE_PHY      (1<<1) /*!< No Prefer RX Physical Layer supported by controller. */
typedef uint8_t esp_ble_gap_all_phys_t;
 
/*
 Bluetooth Low Energy (BLE) Physical Layer preferences for transmission and reception.
 These macros define the options for specifying whether the controller has no preference for the TX (transmit) and RX (receive) PHYs.
*/
#define ESP_BLE_GAP_PRI_PHY_1M     ESP_BLE_GAP_PHY_1M   /*!< No Preference for Transmit Physical Layer
                                                            This macro indicates that the BLE controller has no preference for the TX PHY.
                                                            The controller will choose the appropriate PHY for transmission based on its internal criteria and conditions.
                                                            It allows flexibility in selecting the PHY for data transmission, optimizing for factors such as range, speed, and reliability. */
#define ESP_BLE_GAP_PRI_PHY_CODED  ESP_BLE_GAP_PHY_CODED /*!< No Preference for Receive Physical Layer
                                                            This macro indicates that the BLE controller has no preference for the RX PHY.
                                                            The controller will select the suitable PHY for receiving data based on its internal criteria and conditions.
                                                            It provides flexibility in choosing the PHY for data reception, ensuring optimal performance for various BLE applications. */
typedef uint8_t esp_ble_gap_pri_phy_t; 
 
/*
 BLE Physical Layer Preference Masks
 These macros define the preference masks for selecting the BLE Physical Layer (PHY) for transmission and reception.
*/
#define ESP_BLE_GAP_PHY_1M_PREF_MASK                   (1 << 0) /*!< Low Energy 1 Megabit per second (1M) PHY Preference
                                                                    This macro indicates that the Host prefers to use the 1M PHY for both transmission and reception.
                                                                    The 1M PHY offers a balance between range and data rate, making it suitable for general BLE applications. */
#define ESP_BLE_GAP_PHY_2M_PREF_MASK                   (1 << 1) /*!< Low Energy 2 Megabit per second (2M) PHY Preference
                                                                    This macro indicates that the Host prefers to use the 2M PHY for both transmission and reception.
                                                                    The 2M PHY provides a higher data rate, which is beneficial for applications requiring faster data transfer. */
#define ESP_BLE_GAP_PHY_CODED_PREF_MASK                (1 << 2) /*!< Low Energy Coded PHY Preference
                                                                    This macro indicates that the Host prefers to use the coded PHY for both transmission and reception.
                                                                    The coded PHY enhances range and reliability, especially in challenging environments, by using error correction techniques. */
typedef uint8_t esp_ble_gap_phy_mask_t;

/*
 BLE PHY Options for Coded PHY
 These macros define the preference options for coding schemes used when transmitting on the BLE Coded PHY.
*/
#define ESP_BLE_GAP_PHY_OPTIONS_NO_PREF                  0 /*!< No Preference for Coding Scheme
                                                                This macro indicates that the Host has no preference for the coding scheme when transmitting on the BLE Coded PHY.
                                                                The controller is free to choose any available coding scheme. */
#define ESP_BLE_GAP_PHY_OPTIONS_PREF_S2_CODING           1 /*!< Preference for S=2 Coding Scheme
                                                                This macro indicates that the Host prefers to use S=2 coding when transmitting on the BLE Coded PHY.
                                                                The S=2 coding scheme provides a moderate trade-off between range and data rate.
                                                                It doubles the range compared to the 1M PHY but reduces the data rate. */
#define ESP_BLE_GAP_PHY_OPTIONS_PREF_S8_CODING           2 /*!< Preference for S=8 Coding Scheme
                                                                This macro indicates that the Host prefers to use S=8 coding when transmitting on the BLE Coded PHY.
                                                                The S=8 coding scheme significantly enhances the range at the cost of a reduced data rate.
                                                                It is suitable for applications requiring long-range communication. */
typedef uint16_t esp_ble_gap_prefer_phy_options_t;

/*
 Extended Scanning Configuration Mask
 These macros define the configuration options for scanning BLE advertisements on different Physical Layers.
*/
#define ESP_BLE_GAP_EXT_SCAN_CFG_UNCODE_MASK           0x01 /*!< Scan on 1M PHY.
                                                                This macro enables scanning of BLE advertisements on the Low Energy 1Megabit per second (1M) Physical Layer.
                                                                The 1M PHY is the standard BLE PHY providing a balance between data rate and range. */
#define ESP_BLE_GAP_EXT_SCAN_CFG_CODE_MASK             0x02 /*!< Scan on Coded PHY
                                                                This macro enables scanning of BLE advertisements on the Low Energy coded Physical Layer.
                                                                The coded PHY enhances the range of BLE communication at the cost of a reduced data rate.
                                                                It is suitable for applications that require long-range scanning. */
typedef uint8_t esp_ble_ext_scan_cfg_mask_t;

/*
 Extended Advertising Data Status
 These macros define the status of extended advertising data packets in Bluetooth Low Energy (BLE).
*/
#define ESP_BLE_GAP_EXT_ADV_DATA_COMPLETE              0x00 /*!< Extended Advertising Data Complete
                                                                This macro indicates that the extended advertising data is complete and contains all the necessary information.
                                                                Use this status when the entire dataset is included in the advertising packet. */
#define ESP_BLE_GAP_EXT_ADV_DATA_INCOMPLETE            0x01 /*!< Extended Advertising Data Incomplete
                                                                This macro indicates that the extended advertising data is incomplete.
                                                                Use this status when the dataset is too large to fit in a single advertising packet and additional packets are expected. */
#define ESP_BLE_GAP_EXT_ADV_DATA_TRUNCATED             0x02 /*!< Extended Advertising Data Truncated
                                                                This macro indicates that the extended advertising data is in truncated mode.
                                                                Use this status when the advertising packet contains only a portion of the dataset, and no further packets will be sent. */
typedef uint8_t esp_ble_gap_ext_adv_data_status_t;

/*
 Synchronous Connection Policy
 These macros define the policies for establishing synchronous connections in Bluetooth Low Energy (BLE). 
*/
#define ESP_BLE_GAP_SYNC_POLICY_BY_ADV_INFO       0 /*!< Synchronous Connection by Advertising Information
                                                        This macro indicates that the synchronous connection policy is based on the advertising information received.
                                                        Use this policy when the connection parameters are determined by the advertising packets. */
#define ESP_BLE_GAP_SYNC_POLICY_BY_PERIODIC_LIST  1 /*!< Periodic Advertising Synchronous Connection Policy
                                                        This macro indicates that the synchronous connection policy is based on a periodic advertising list.
                                                        Use this policy when the connection parameters are determined by entries in a predefined periodic advertising list. */
typedef uint8_t esp_ble_gap_sync_t;

/*
 Advertising Report Types
 These macros define the types of advertising reports that can be received in Bluetooth Low Energy (BLE) scanning.
*/
#define ESP_BLE_ADV_REPORT_EXT_ADV_IND                    (1<<0)    /*!< Extended Advertising Indication Report
                                                                        This macro indicates that the advertising report corresponds to an extended advertising indication type.
                                                                        Extended advertising indications are used in Bluetooth LE 5.0 and later to advertise additional data and support larger payloads. */
#define ESP_BLE_ADV_REPORT_EXT_SCAN_IND                   (1<<1)    /*!< Extended Scan Indication Report
                                                                        This macro indicates that the advertising report corresponds to an extended scan indication type.
                                                                        Extended scan indications are used to inform scanners about the presence of devices in a more detailed manner, including larger advertising data. */
#define ESP_BLE_ADV_REPORT_EXT_DIRECT_ADV                 (1<<2)    /*!< Extended Direct Advertising Indication Report
                                                                        This macro indicates that the advertising report corresponds to an extended direct advertising indication type.
                                                                        Direct advertising indications are used for directed advertisements, which are targeted towards a specific device address. */
#define ESP_BLE_ADV_REPORT_EXT_SCAN_RSP                   (1<<3)    /*!< Extended Scan Response Indication Report
                                                                        This macro indicates that the advertising report corresponds to an extended scan response indication type.
                                                                        Extended scan responses provide additional data in response to a scan request and can include more extensive information about the advertising device. */

/*
 Legacy Advertising Report Types
 These macros define the types of advertising reports that can be received in Bluetooth Low Energy (BLE) scanning, specifically for legacy advertising types.
*/
#define ESP_BLE_LEGACY_ADV_TYPE_IND                       (0x13)    /*!< Legacy Advertising Indication Report
                                                                        This macro indicates that the advertising report corresponds to a legacy advertising indication type.
                                                                        Legacy advertising indications are used in Bluetooth LE versions before 5.0, where the advertising data and payload are limited compared to extended advertising. */
#define ESP_BLE_LEGACY_ADV_TYPE_DIRECT_IND                (0x15)    /*!< Legacy Direct Advertising Indication Report
                                                                        This macro indicates that the advertising report corresponds to a legacy direct advertising indication type.
                                                                        Direct advertising indications are used for targeted advertising, where the advertisement is directed towards a specific device address. */
#define ESP_BLE_LEGACY_ADV_TYPE_SCAN_IND                  (0x12)    /*!< Legacy Scan Indication Report
                                                                        This macro indicates that the advertising report corresponds to a legacy scan indication type.
                                                                        Scan indications are used to advertise the device's presence in a way that allows other devices to perform a scan and discover it. */
#define ESP_BLE_LEGACY_ADV_TYPE_NONCON_IND                (0x10)    /*!< Legacy Non-Connectable Advertising Indication Report
                                                                        This macro indicates that the advertising report corresponds to a legacy non-connectable advertising indication type.
                                                                        Non-connectable advertisements are used for broadcasting information to devices without establishing a connection. */
#define ESP_BLE_LEGACY_ADV_TYPE_SCAN_RSP_TO_ADV_IND       (0x1b)    /*!< Legacy Scan Response to Advertising Indication Report
                                                                        This macro indicates that the advertising report corresponds to a legacy scan response to advertising indication type.
                                                                        This type of report is used when a scan response is sent in response to an advertising packet, providing additional information about the advertiser. */
#define ESP_BLE_LEGACY_ADV_TYPE_SCAN_RSP_TO_ADV_SCAN_IND  (0x1a)    /*!< Legacy Scan Response to Advertising with Scan Indication Report
                                                                        This macro indicates that the advertising report corresponds to a legacy advertising with scan response indication type.
                                                                        This report is used for advertisements that include a scan response, providing additional information in response to a scan request. */
typedef uint8_t esp_ble_gap_adv_type_t;

#define EXT_ADV_TX_PWR_NO_PREFERENCE                      (127) /*!< Extended Advertising Transmission Power Preference
                                                                    This macro value indicates that the host does not have a specific preference for the transmission power level used during extended advertising.
                                                                    When set to 127, it signifies that the host is indifferent to the power level and the BLE stack can select the power level based on other factors such as hardware capabilities or regulatory requirements.
                                                                    Transmission power levels are specified in decibels milli watts (dBm).
                                                                    The range of valid transmission power values is from -127dBm (minimum) to +126dBm (maximum).
                                                                    Setting this value allows the BLE stack to choose an appropriate power level based on the device's configuration and operational environment. */

/**
 * @brief Extended Advertising Parameters
 * This structure holds the parameters used to configure extended advertising in Bluetooth Low Energy (BLE). 
 */
typedef struct {
    esp_ble_ext_adv_type_mask_t type;    /*!< Extended advertising type mask. Specifies the type of extended advertising being used. Possible values include connectable, scannable, and non-connectable types. */
    uint32_t interval_min;               /*!< Minimum interval between extended advertising events, in units of 0.625 ms. This value sets the shortest duration between consecutive advertising events. The interval range is from 0x0020 (20ms) to 0xFFFF (10.24s). */
    uint32_t interval_max;               /*!< Maximum interval between extended advertising events, in units of 0.625 ms. This value sets the longest duration between consecutive advertising events. The interval range is from 0x0020 (20ms) to 0xFFFF (10.24s). */
    esp_ble_adv_channel_t channel_map;   /*!< Channel map for extended advertising. Defines the advertising channels that will be used for transmitting advertising packets. Typically, this includes channels 37, 38, and 39. */
    esp_ble_addr_type_t own_addr_type;   /*!< Address type of the local device for extended advertising. This can be public, random, or any other valid address type. */
    esp_ble_addr_type_t peer_addr_type;  /*!< Address type of the peer device to which the advertising is directed (if applicable). This can be public, random, or any other valid address type. */
    esp_bd_addr_t peer_addr;             /*!< Address of the peer device to which the advertising is directed (if applicable). This field is used for directed advertising types. */
    esp_ble_adv_filter_t filter_policy;  /*!< Filter policy for extended advertising. Determines which devices' scan requests and connections will be accepted or rejected based on their address and other criteria. */
    int8_t tx_power;                     /*!< Transmission power level for extended advertising, in dBm. This value specifies the power at which advertising packets are transmitted. Typical range is from -127dBm (lowest) to +126dBm (highest), with 127 indicating no preference. */
    esp_ble_gap_pri_phy_t primary_phy;   /*!< Primary Physical Layer used for extended advertising. Specifies whether the advertising will use 1M, 2M, or Coded PHY. */
    uint8_t max_skip;                    /*!< Maximum number of advertising events to skip. This value controls how many consecutive advertising events can be skipped before resuming advertising. */
    esp_ble_gap_phy_t secondary_phy;     /*!< Secondary Physical Layer used for extended advertising, if applicable. Specifies whether the secondary advertising will use 1M, 2M, or Coded PHY. */
    uint8_t sid;                         /*!< Advertising SID (Advertising Set ID) for extended advertising. Used to identify the advertising set when multiple sets are being used. */
    bool scan_req_notif;                 /*!< Flag indicating whether scan request notifications should be sent. If true, the device will notify when a scan request is received. */
} esp_ble_gap_ext_adv_params_t;

/**
 * @brief Extended Scan Configuration
 * This structure holds configuration parameters for extended scanning in Bluetooth Low Energy (BLE). 
 */
typedef struct {
    esp_ble_scan_type_t scan_type; /*!< Type of scan to be performed. This determines whether the scan is active or passive:
                                      `ESP_BLE_SCAN_TYPE_ACTIVE`: Active scan where scan requests are sent.
                                      `ESP_BLE_SCAN_TYPE_PASSIVE`: Passive scan where only advertising packets are received. */
    uint16_t scan_interval;        /*!< Interval between consecutive scanning events, in units of 0.625 ms. This value defines how often the scanning is performed. The interval range is from 0x0004 (2.5ms) to 0xFFFF (10.24s). */
    uint16_t scan_window;          /*!< Duration of each scanning event, in units of 0.625 ms. This value sets how long the scanner will listen for advertising packets during each scanning event. The window range is from 0x0004 (2.5ms) to 0xFFFF (10.24s). */
} esp_ble_ext_scan_cfg_t;

/**
 * @brief Extended Scan Parameters
 * This structure holds parameters used to configure extended scanning operations.
 */
typedef struct {
    esp_ble_addr_type_t own_addr_type;        /*!< Address type of the local device used for extended scanning. */
    esp_ble_scan_filter_t filter_policy;      /*!< Filter policy for extended scanning. Determines which devices' advertising packets will be accepted or rejected based on their address and other criteria. */
    esp_ble_scan_duplicate_t scan_duplicate;  /*!< Duplicate filtering policy for scanning. */
    esp_ble_ext_scan_cfg_mask_t cfg_mask;     /*!< Mask specifying which extended scan configuration parameters are set. */
    esp_ble_ext_scan_cfg_t uncoded_cfg;       /*!< Configuration parameters for extended scanning on the Low Energy 1M and 2M PHYs. This includes scan interval and window settings for these PHYs. */
    esp_ble_ext_scan_cfg_t coded_cfg;         /*!< Configuration parameters for extended scanning on the Low Energy Coded PHY. This includes scan interval and window settings for this PHY. */
} esp_ble_ext_scan_params_t;

/**
 * @brief Create Extended Connection Parameters
 * This structure holds the parameters used to establish and maintain an extended BLE connection. 
 */
typedef struct {
    uint16_t scan_interval;       /*!< Initial scan interval, in units of 0.625ms. Determines how often the scanning is performed. The interval range is from 0x0004 (2.5ms) to 0xFFFF (10.24s). */
    uint16_t scan_window;         /*!< Initial scan window, in units of 0.625ms. Specifies the duration of each scan event. The window range is from 0x0004 (2.5ms) to 0xFFFF (10.24s). */
    uint16_t interval_min;        /*!< Minimum connection interval, in units of 1.25ms. The minimum time between two consecutive connection events. The interval range is from 0x0006 (7.5ms) to 0x0C80 (4s). */
    uint16_t interval_max;        /*!< Maximum connection interval, in units of 1.25ms. The maximum time between two consecutive connection events. The interval range is from 0x0006 (7.5ms) to 0x0C80 (4s). */
    uint16_t latency;             /*!< Connection latency, which specifies the number of connection events that the slave device can skip. This helps to reduce the power consumption by skipping connection events. The latency range is from 0x0000 (0) to 0x01F3 (499). */
    uint16_t supervision_timeout; /*!< Connection supervision timeout, in units of 10ms. The timeout period during which the master expects the slave to respond to connection events. If the slave does not respond within this time, the connection is considered lost. The timeout range is from 0x000A (100ms) to 0x0C80 (32s). */
    uint16_t min_ce_len;          /*!< Minimum connection event length, in units of 1.25ms. Defines the minimum length of each connection event. This value can be used to adjust how much time is allocated for data transfer during each connection event. */
    uint16_t max_ce_len;          /*!< Maximum connection event length, in units of 1.25ms. Defines the maximum length of each connection event. This value can be used to adjust how much time is allocated for data transfer during each connection event. */
} esp_ble_gap_conn_params_t;

/**
 * @brief Extended Advertising Enable Parameters
 * This structure holds the parameters used to enable extended advertising. 
 */
typedef struct {
    uint8_t instance;        /*!< Advertising handle, which identifies the advertising instance. Each advertising instance must have a unique handle. */
    int duration;            /*!< Advertising duration, in units of 1s. Specifies how long the advertising should continue before stopping automatically. A value of 0 means that advertising will continue indefinitely. */
    int max_events;          /*!< Maximum number of extended advertising events to be transmitted. If this value is greater than 0, advertising will stop after the specified number of events have been transmitted. */
} esp_ble_gap_ext_adv_t;

/**
 * @brief Periodic Advertising Parameters
 * This structure holds the parameters used for periodic advertising, which allows BLE devices to send periodic advertising packets that can be synchronized with other devices.
 */
typedef struct {
    uint16_t interval_min;     /*!< Minimum interval between consecutive periodic advertising events, in units of 1.25ms. The range is from 0x0006 (7.5ms) to 0xFFFF (81.92s). */
    uint16_t interval_max;     /*!< Maximum interval between consecutive periodic advertising events, in units of 1.25ms. The range is from 0x0006 (7.5ms) to 0xFFFF (81.92s). */
    uint8_t  properties;       /*!< Periodic advertising properties, such as whether the periodic advertising is high duty cycle or low duty cycle. */
} esp_ble_gap_periodic_adv_params_t;

/**
 * @brief Periodic Advertising Synchronization Parameters
 * This structure is used to define parameters for synchronizing with periodic advertising events.
 */
typedef struct {
    esp_ble_gap_sync_t filter_policy;       /*!< Configures the filter policy for periodic advertising synchronization:
                                                 - `0`: Use Advertising Service Identifier, Advertiser Address Type, and Advertiser Address parameters to determine the advertiser to listen to.
                                                 - `1`: Use the Periodic Advertiser List to determine the advertiser to listen to.
                                                 This policy helps in filtering which periodic advertising events the device should sync with based on the given criteria. */
    
    #if (CONFIG_BT_BLE_FEAT_CREATE_SYNC_ENH)
    esp_ble_gap_sync_t reports_disabled;    /*!< Supported only by specific ESP32 models (esp32c2, esp32c6, and esp32h2); configurable via menuconfig:
                                                 - `0`: Reporting initially enabled. The device will start reporting periodic advertising data as soon as synchronization is established.
                                                 - `1`: Reporting initially disabled. Reporting of periodic advertising data will be disabled until explicitly enabled.
                                                 This option allows control over whether periodic advertising data reporting is active at the start. */
    
    esp_ble_gap_sync_t filter_duplicates;   /*!< Supported only by specific ESP32 models (esp32c2, esp32c6, and esp32h2); configurable via menuconfig:
                                                 - `0`: Duplicate filtering initially disabled. All received periodic advertising packets will be processed, including duplicates.
                                                 - `1`: Duplicate filtering initially enabled. The device will filter out duplicate periodic advertising packets to avoid processing the same data multiple times.
                                                 This setting controls whether duplicate packets are filtered out, which can help reduce unnecessary processing of repeated data. */
    #endif
    
    uint8_t sid;                            /*!< Advertising Set ID (SID). This identifier helps to filter and identify the specific service whose periodic advertising packets the device should synchronize with. */
    esp_ble_addr_type_t addr_type;          /*!< Address type of the periodic advertising. This specifies the type of address used by the advertiser, such as public, random, etc. */
    esp_bd_addr_t addr;                     /*!< Address of the periodic advertising device. This is the Bluetooth address of the device that is broadcasting the periodic advertising packets. */
    uint16_t skip;                          /*!< Maximum number of periodic advertising events that can be skipped before synchronization is lost. This parameter helps manage synchronization reliability by specifying how many events can be missed before resynchronizing. */
    uint16_t sync_timeout;                  /*!< Synchronization timeout, in units of 10ms. This is the maximum time allowed for maintaining synchronization with the periodic advertising device. If the synchronization is lost or not maintained within this timeout, the synchronization process may need to be restarted. */
} esp_ble_gap_periodic_adv_sync_params_t;


/**
 * @brief Extended Advertising Report Parameters
 * This structure is used to represent the information obtained from an extended advertising report. 
 */
typedef struct {
    esp_ble_gap_adv_type_t event_type;              /*!< Extended advertising event type. Indicates the type of extended advertising event. */
    uint8_t addr_type;                              /*!< Extended advertising address type. Specifies the type of Bluetooth address used by the advertiser (e.g., public, random). */
    esp_bd_addr_t addr;                             /*!< Extended advertising address. The Bluetooth address of the device that is broadcasting the extended advertising packets. */
    esp_ble_gap_pri_phy_t primary_phy;              /*!< Extended advertising primary Physical Layer. Indicates the PHY (Physical Layer) used for the primary advertising channel (e.g., 1M, 2M, Coded). */
    esp_ble_gap_phy_t secondly_phy;                 /*!< Extended advertising secondary Physical Layer. Indicates the PHY used for the secondary advertising channel. */
    uint8_t sid;                                    /*!< Extended advertising Advertising Set ID (SID). Identifies the specific service associated with the advertising data. */
    uint8_t tx_power;                               /*!< Extended advertising transmission power. The power level of the transmitted advertising packets, measured in dBm. */
    int8_t rssi;                                    /*!< Extended advertising Received Signal Strength Indication (RSSI). The strength of the received signal, measured in dBm. Range: -127 dBm to +20 dBm. */
    uint16_t per_adv_interval;                      /*!< Periodic advertising interval. The interval at which the periodic advertising packets are broadcasted. */
    uint8_t dir_addr_type;                          /*!< Direct address type. The address type used in directed advertising (e.g., public, random). */
    esp_bd_addr_t dir_addr;                         /*!< Direct address. The Bluetooth address of the device being targeted by directed advertising. */
    esp_ble_gap_ext_adv_data_status_t data_status;  /*!< Data status. Indicates the status of the extended advertising data (e.g., complete, incomplete, truncated). */
    uint8_t adv_data_len;                           /*!< Extended advertising data length. The length of the advertising data in bytes. */
    uint8_t adv_data[251];                          /*!< Extended advertising data. The actual data payload of the extended advertising packet, up to 251 bytes. */
} esp_ble_gap_ext_adv_report_t;

/**
 * @brief Periodic Advertising Report Parameters
 * This structure is used to represent the information obtained from a periodic advertising report. 
 */
typedef struct {
    uint16_t sync_handle;                          /*!< Periodic advertising synchronization handle. A handle used to identify the synchronization with a specific periodic advertising train. */
    uint8_t tx_power;                              /*!< Periodic advertising transmission power. The power level of the transmitted periodic advertising packets, measured in dBm. */
    int8_t rssi;                                   /*!< Periodic advertising Received Signal Strength Indication (RSSI). The strength of the received periodic advertising signal, measured in dBm. */
    esp_ble_gap_ext_adv_data_status_t data_status; /*!< Data status. Indicates the status of the periodic advertising data (e.g., complete, incomplete, truncated). */
    uint8_t data_length;                           /*!< Periodic advertising data length. The length of the periodic advertising data in bytes. */
    uint8_t data[251];                             /*!< Periodic advertising data. The actual data payload of the periodic advertising packet, up to 251 bytes. */
} esp_ble_gap_periodic_adv_report_t;

/**
 * @brief Periodic Advertising Synchronization Establish Parameters
 * This structure is used to represent the information obtained when a device successfully establishes synchronization with a periodic advertising train.
 */
typedef struct {
    uint8_t status;                               /*!< Periodic advertising synchronization status. Indicates the result of the synchronization process. */
    uint16_t sync_handle;                         /*!< Periodic advertising train handle. A unique handle identifying the synchronized periodic advertising train. */
    uint8_t sid;                                  /*!< Periodic advertising Signal Strength Indication. An identifier for the periodic advertising train, often used for filtering and synchronization. */
    esp_ble_addr_type_t addr_type;                /*!< Periodic advertising address type. Specifies the type of Bluetooth address used by the advertiser (e.g., public or random). */
    esp_bd_addr_t adv_addr;                       /*!< Periodic advertising address. The Bluetooth address of the device that is broadcasting the periodic advertising packets. */
    esp_ble_gap_phy_t adv_phy;                    /*!< Periodic advertising Physical Layer type. Indicates the PHY used for the periodic advertising channel (e.g., 1M, 2M, coded). */
    uint16_t period_adv_interval;                 /*!< Periodic advertising interval. The interval at which the periodic advertising packets are broadcasted. */
    uint8_t adv_clk_accuracy;                     /*!< Periodic advertising clock accuracy. Indicates the accuracy of the advertiser's clock, used to synchronize with the periodic advertising train. */
} esp_ble_gap_periodic_adv_sync_estab_t;

/**
 * @brief Device Under Test Manager TX Parameters
 * This structure is used to configure the parameters for transmitting test data in Bluetooth Low Energy tests.
 */
typedef struct
{
    uint8_t                     tx_channel;            /*!< Channel for sending test data. Channel number is calculated as (Frequency - 2402) / 2. Range: 0x00 - 0x27, corresponding to frequencies from 2402 MHz to 2480 MHz. */
    uint8_t                     len_of_data;           /*!< Length in bytes of payload data in each packet. Specifies the size of the test data payload. */
    esp_ble_dtm_pkt_payload_t   pkt_payload;           /*!< Packet payload type. Specifies the type of data payload used in the test packets. Range: 0x00 - 0x07. */
    esp_ble_gap_phy_t           phy;                   /*!< Physical Layer type used by the transmitter. Indicates the PHY layer (e.g., 1M, 2M, coded with S=2). */
} esp_ble_dtm_enh_tx_t;

/**
 * @brief Device Under Test Manager RX Parameters
 * This structure is used to configure the parameters for receiving test data in Bluetooth Low Energy tests.
 */
typedef struct
{
    uint8_t             rx_channel;            /*!< Channel for test data reception. Channel number is calculated as (Frequency - 2402) / 2. Range: 0x00 - 0x27, corresponding to frequencies from 2402 MHz to 2480MHz. */
    esp_ble_gap_phy_t   phy;                   /*!< Physical Layer type used by the receiver. Indicates the PHY layer (e.g., 1M, 2M, coded). */
    uint8_t             modulation_idx;        /*!< Modulation index. Defines the modulation characteristics of the received signal. 0x00: standard modulation index, 0x01: stable modulation index. */
} esp_ble_dtm_enh_rx_t;

#endif// #if (BLE_50_FEATURE_SUPPORT == TRUE)

/* if Bluetooth Low Energy Feature Periodic Advertising Synchronization Transfer Support 
 * Periodic advertising sync trans mode*/ 
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

/*
 GAP Periodic Advertising Synchronization Transfer (PAST) Mode.
 These macros define how periodic advertising synchronization and reporting are handled in BLE (Bluetooth Low Energy) GAP (Generic Access Profile).
*/
#define ESP_BLE_GAP_PAST_MODE_NO_SYNC_EVT                 (0x00)    /*!< No attempt is made to synchronization and no periodic advertising synchronization transfer received event. */
#define ESP_BLE_GAP_PAST_MODE_NO_REPORT_EVT               (0x01)    /*!< A periodic advertising synchronization transfer received event is generated, but no periodic advertising report events are generated. */
#define ESP_BLE_GAP_PAST_MODE_DUP_FILTER_DISABLED         (0x02)    /*!< Periodic advertising report events are generated with duplicate filtering disabled. */
#define ESP_BLE_GAP_PAST_MODE_DUP_FILTER_ENABLED          (0x03)    /*!< Periodic advertising report events are generated with duplicate filtering enabled. */
typedef uint8_t esp_ble_gap_past_mode_t;

/**
 * @brief Periodic Advertising Synchronization Transfer Parameters
 * This structure defines the parameters used for configuring periodic advertising synchronization transfer in Bluetooth Low Energy (BLE).
 */
typedef struct {
    esp_ble_gap_past_mode_t mode;       /*!< The mode for periodic advertising synchronization transfer. */
    uint16_t skip;                      /*!< The maximum number of periodic advertising packets that can be skipped. 
                                            This value determines how many periodic advertising events can be missed before synchronization is considered lost or requires re-synchronization. */
    uint16_t sync_timeout;              /*!< The timeout value for synchronization with the periodic advertising train.
                                            This timeout specifies the maximum time duration to wait for periodic advertising packets before considering synchronization as lost or needing re-establishment. */

    uint8_t cte_type;                   /*!< The Constant Tone Extension (CTE) type for periodic advertising synchronization transfer.
                                             CTE is an extension to the advertising packets to provide better direction finding capabilities. 
                                             This parameter specifies the type of CTE that will be used for periodic advertising synchronization. 
                                             Values can vary based on the CTE types supported, such as `0` for no CTE, `1` for a specific CTE type, etc. */
} esp_ble_gap_past_params_t;
#endif// #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

/**
 * @brief Privacy Modes for Bluetooth Low Energy (BLE)
 * This enumeration defines the different privacy modes that can be configured for a peer device in BLE.
 */
typedef enum{
    ESP_BLE_NETWORK_PRIVACY_MODE    = 0X00,    /*!< Network Privacy Mode for peer device. Network Mode will only accept the RPA. */
    ESP_BLE_DEVICE_PRIVACY_MODE     = 0X01,    /*!< Device Privacy Mode for peer device. Device Mode will accept the Identity Address and RPA. */
} esp_ble_privacy_mode_t;

/**
 * @brief Generic Access Profile (GAP) Callback Parameters Union
 * This union encapsulates the parameters for various GAP callback events in BLE. Each structure within the union corresponds to a different event type, providing event-specific information.
 */
typedef union {
    /**
     * @brief ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT
     * This event is triggered when the process of getting the Bluetooth device name is complete.
     */
    struct ble_get_dev_name_cmpl_evt_param {
        esp_bt_status_t status;   /*!< Status indicating whether getting the device name was successful or not.
                                        `ESP_BT_STATUS_SUCCESS` if successful.
                                        Error status if failed. */
        char *name;               /*!< The name of the Bluetooth device as a null-terminated string.
                                       This name is the friendly name assigned to the device, which can be used for identification and display purposes. */
    } get_dev_name_cmpl;  /*!< ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT */

// if Bluetooth Low Energy 4.2 Feature Support 
#if (BLE_42_FEATURE_SUPPORT == TRUE)

    /**
     * @brief GAP Advertisement Data Set Complete Event Parameter
     * This structure contains the parameters for the `ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT` event.
     * It provides information about the completion status of the advertising data set operation.
     */
    struct ble_adv_data_cmpl_evt_param {
        esp_bt_status_t status;   /*!< Indicates the success or failure of the set advertising data operation. */
    } adv_data_cmpl;  /*!< GAP Advertisement Data Set Complete Event Parameter */
    /**
     * @brief GAP Scan Response Data Set Complete Event Parameter
     * This structure contains the parameters for the `ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT` event.
     * It provides information about the completion status of the scan response data set operation.
     */
    struct ble_scan_rsp_data_cmpl_evt_param {
        esp_bt_status_t status;   /*!< Indicates the success or failure of the set scan response data operation. */
    } scan_rsp_data_cmpl;  /*!< ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT */
    /**
     * @brief GAP Scan Parameters Set Complete Event Parameter
     * This structure contains the parameters for the `ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT` event.
     * It provides information about the completion status of the scan parameters set operation.
     */
    struct ble_scan_param_cmpl_evt_param {
        esp_bt_status_t status;   /*!< Indicates the success or failure of the set scan parameters operation. */
    } scan_param_cmpl;   /*!< ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT */
    /**                 
     * @brief ESP_GAP_BLE_SCAN_RESULT_EVT
     * Event parameter for the ESP_GAP_BLE_SCAN_RESULT_EVT event.
     * This structure contains the parameters for the Bluetooth Low Energy (BLE) scan result event.
     */
    struct ble_scan_result_evt_param {
        esp_gap_search_evt_t search_evt;   /*!< The type of scan result event, such as device found, scan complete, etc. */
        esp_bd_addr_t bda;                 /*!< The Bluetooth device address of the discovered device. */
        esp_bt_dev_type_t dev_type;        /*!< The type of the device (e.g., BR/EDR, BLE, or DUAL). */
        esp_ble_addr_type_t ble_addr_type; /*!< The address type of the BLE device (e.g., public, random). */
        esp_ble_evt_type_t ble_evt_type;   /*!< The type of BLE scan result event (e.g., advertisement report). */
        int rssi;                          /*!< The Received Signal Strength Indication (RSSI) of the discovered device. */
        uint8_t ble_adv[ESP_BLE_ADV_DATA_LEN_MAX + ESP_BLE_SCAN_RSP_DATA_LEN_MAX]; /*!< The received advertisement and scan response data. */
        int flag;                          /*!< The advertising data flag bit, indicating the type of advertising data. */
        int num_resps;                     /*!< The number of responses received during the scan. */
        uint8_t adv_data_len;              /*!< The length of the received advertising data. */
        uint8_t scan_rsp_len;              /*!< The length of the received scan response data. */
        uint32_t num_dis;                  /*!< The number of discarded packets during the scan. */
    } scan_rst;    /*!< ESP_GAP_BLE_SCAN_RESULT_EVT */

    /**
     * @brief ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT event.
     * This structure contains the status of setting raw advertising data.
     */
    struct ble_adv_data_raw_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the set raw advertising data operation. */
    } adv_data_raw_cmpl;   /*!< ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT event.
     * This structure contains the status of setting raw scan response data.
     */
    struct ble_scan_rsp_data_raw_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Indicates the success status of the set raw scan response data operation. */
    } scan_rsp_data_raw_cmpl;  /*!< ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_ADV_START_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_ADV_START_COMPLETE_EVT event.
     * This structure contains the status of starting advertising.
     */
    struct ble_adv_start_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Indicates the success status of the advertising start operation. */
    } adv_start_cmpl;    /*!< ESP_GAP_BLE_ADV_START_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SCAN_START_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SCAN_START_COMPLETE_EVT event.
     * This structure contains the status of starting a scan.
     */
    struct ble_scan_start_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the scan start operation. */
    } scan_start_cmpl;  /*!< ESP_GAP_BLE_SCAN_START_COMPLETE_EVT */    
#endif// #if (BLE_42_FEATURE_SUPPORT == TRUE)

    esp_ble_sec_t ble_security;      /*!< Bluetooth Low Energy Generic Access Profile security union type. */

// if Bluetooth Low Energy 4.2 Feature Support 
#if (BLE_42_FEATURE_SUPPORT == TRUE)

    /**
     * @brief ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT event.
     * This structure contains the status of stopping a scan.
     */
    struct ble_scan_stop_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the scan stop operation. */
    } scan_stop_cmpl;  /*!< ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT */ 

    /**
     * @brief ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT event.
     * This structure contains the status of stopping advertising.
     */
    struct ble_adv_stop_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the advertising stop operation. */
    } adv_stop_cmpl;    /*!< ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT */ 

    /**
     * @brief ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT event.
     * This structure contains the status of clearing advertising data.
     */
    struct ble_adv_clear_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the advertising clear operation. */
    } adv_clear_cmpl;    /*!< ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT */    
#endif// #if (BLE_42_FEATURE_SUPPORT == TRUE)

    /**
     * @brief ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT
     * Event parameter for the ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT event.
     * This event is triggered when the operation to set a static random address is complete.
     */
    struct ble_set_rand_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the operation to set a static random address. */
    } set_rand_addr_cmpl;    /*!< ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT */ 

    /**
     * @brief ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT
     * Event parameter for the ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT event.
     * This event is triggered when the connection parameters update operation is complete.
     */
    struct ble_update_conn_params_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the connection parameters update operation. */
        esp_bd_addr_t bda;          /*!< Bluetooth Device Address (BD_ADDR) of the device for which the connection parameters were updated. */
        uint16_t min_int;           /*!< Minimum connection interval in units of 1.25 ms. The interval range is from 0x0006 (7.5 ms) to 0x0C80 (4 sec). */
        uint16_t max_int;           /*!< Maximum connection interval in units of 1.25 ms. The interval range is from 0x0006 (7.5 ms) to 0x0C80 (4 sec). */
        uint16_t latency;           /*!< Connection latency in number of connection events. This value determines how many connection events the slave device can skip. The latency range is from 0x0000 (0) to 0x01F3 (499). */
        uint16_t conn_int;          /*!< Current connection interval in units of 1.25 ms. */
        uint16_t timeout;           /*!< Supervision timeout for the connection in units of 10 ms. This is the maximum time between consecutive connection events before the connection is considered lost. The timeout range is from 0x000A (100 ms) to 0x0C80 (32 sec). */
    } update_conn_params;   /*!< ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT */ 

    /**
     * @brief ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT event.
     * This event is triggered when the operation to set the packet length is complete.
     */
    struct ble_pkt_data_length_cmpl_evt_param {
        esp_bt_status_t status;                     /*!< Indicates the success status of the operation to set the packet data length. */
        esp_ble_pkt_data_length_params_t params;    /*!< Contains the packet data length parameters such as maximum packet size and data length extension parameters. */
    } pkt_data_length_cmpl;    /*!< ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT event.
     * This event is triggered when the operation to set local privacy is complete.
     */
    struct ble_local_privacy_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the operation to set local privacy. */
    } local_privacy_cmpl;  /*!< ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SET_RPA_TIMEOUT_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SET_RPA_TIMEOUT_COMPLETE_EVT event.
     * This event is triggered when the operation to set the Resolvable Private Address (RPA) timeout is complete.
     */
    struct ble_rpa_timeout_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the operation to set the RPA timeout. */
    } set_rpa_timeout_cmpl;   /*!< ESP_GAP_BLE_SET_RPA_TIMEOUT_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_ADD_DEV_TO_RESOLVING_LIST_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_ADD_DEV_TO_RESOLVING_LIST_COMPLETE_EVT event.
     * This event is triggered when the operation to add a device to the Resolving List is complete.
     */
    struct ble_add_dev_to_resolving_list_cmpl_evt_param {
        esp_bt_status_t status;         /*!< Indicates the success status of the operation to add a device to the Resolving List. */
    } add_dev_to_resolving_list_cmpl; /*!< ESP_GAP_BLE_ADD_DEV_TO_RESOLVING_LIST_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT event.
     * This event is triggered when the operation to remove a bonded device is complete.
     */
    struct ble_remove_bond_dev_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the operation to remove a bonded device. */
        esp_bd_addr_t bd_addr;      /*!< Bluetooth Device Address (BD_ADDR) of the device that was removed from the bond list. */
    } remove_bond_dev_cmpl;  /*!< ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT event.
     * This event is triggered when the operation to clear all bonded devices is complete.
     */
    struct ble_clear_bond_dev_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Indicates the success status of the operation to clear all bonded devices. */
    } clear_bond_dev_cmpl;  /*!< ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT event.
     * This event is triggered when the operation to get the bonded devices is complete.
     */
    struct ble_get_bond_dev_cmpl_evt_param {
        esp_bt_status_t status;             /*!< Indicates the success status of the operation to get bonded devices. */
        uint8_t dev_num;                    /*!< Number of devices in the bond list. */
        esp_ble_bond_dev_t *bond_dev;       /*!< Pointer to an array of bonded device structures. */
    } get_bond_dev_cmpl;   /*!< ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT event.
     * This event is triggered when the operation to read the Received Signal Strength Indication (RSSI) is complete.
     */
    struct ble_read_rssi_cmpl_evt_param {
        esp_bt_status_t status;         /*!< Indicates the success status of the RSSI read operation. */
        int8_t rssi;                    /*!< Received Signal Strength Indication (RSSI) value of the remote device. The range is from -127 to 20 dBm. If the RSSI cannot be read, it is set to 127. */
        esp_bd_addr_t remote_addr;      /*!< Bluetooth Device Address (BD_ADDR) of the remote device whose RSSI was read. */
    } read_rssi_cmpl;  /*!< ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT event.
     * This event is triggered when the operation to update the whitelist (add or remove addresses) is complete.
     */
    struct ble_update_whitelist_cmpl_evt_param {
        esp_bt_status_t status;                 /*!< Indicates the success status of the whitelist update operation. */
        esp_ble_wl_operation_t wl_operation;    /*!< Operation performed on the whitelist: ESP_BLE_WHITELIST_ADD for adding an address, ESP_BLE_WHITELIST_REMOVE for removing an address. */
    } update_whitelist_cmpl;   /*!< ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT */

// if Bluetooth Low Energy 4.2 Feature Support     
#if (BLE_42_FEATURE_SUPPORT == TRUE)

    /**
     * @brief ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT event.
     * This event is triggered when the operation to update the duplicate exceptional list is complete.
     */
    struct ble_update_duplicate_exceptional_list_cmpl_evt_param {
        esp_bt_status_t status;                 /*!< Indicates the success status of the operation to update the duplicate exceptional list. */
        uint8_t subcode;                        /*!< Defines the specific operation performed on the duplicate exceptional list. This value is defined in the esp_bt_duplicate_exceptional_subcode_type_t enumeration. */
        uint16_t length;                        /*!< The length of the `device_info` structure. This value is valid only if the subcode indicates an operation that involves device information. */
        esp_duplicate_info_t device_info;       /*!< Device information related to the duplicate exceptional list. This field is valid only if the subcode is not `ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_CLEAN`. 
                                                    If the subcode is `ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_CLEAN`, the value of this field is invalid. */
    } update_duplicate_exceptional_list_cmpl;/*!< ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT */
#endif// #if (BLE_42_FEATURE_SUPPORT == TRUE)

    /**
     * @brief ESP_GAP_BLE_SET_CHANNELS_EVT
     * Event parameter for the ESP_GAP_BLE_SET_CHANNELS_EVT event.
     * This event is triggered when the operation to set Bluetooth Low Energy (BLE) channels is complete.
     */
    struct ble_set_channels_evt_param {
        esp_bt_status_t stat;       /*!< Indicates the status of the operation to set BLE channels.
                                        This status value helps determine if the operation was successful or if an error occurred. */
    } ble_set_channels;    /*!< ESP_GAP_BLE_SET_CHANNELS_EVT */ 

// if Bluetooth Low Energy 4.2 Feature Support
#if (BLE_50_FEATURE_SUPPORT == TRUE)

    /**
     * @brief ESP_GAP_BLE_READ_PHY_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_READ_PHY_COMPLETE_EVT event.
     * This event is triggered when the operation to read the Physical Layer (PHY) settings is complete.
     */
    struct ble_read_phy_cmpl_evt_param {
        esp_bt_status_t status;                   /*!< Status of the PHY read operation. Indicates whether the PHY read operation was successful or if an error occurred. */
        esp_bd_addr_t bda;                        /*!< Bluetooth Device Address of the device for which the PHY was read. */
        esp_ble_gap_phy_t tx_phy;                 /*!< Type of PHY used for transmission. */
        esp_ble_gap_phy_t rx_phy;                 /*!< Type of PHY used for reception. */
    } read_phy;     /*!< ESP_GAP_BLE_READ_PHY_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT event.
     * This event is triggered when the operation to set the default Physical Layer (PHY) is complete.
     */
    struct ble_set_perf_def_phy_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Status of the default PHY setting operation. Indicates whether the operation was successful or if an error occurred. */
    } set_perf_def_phy;   /*!< ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT event.
     * This event is triggered when the operation to set the preferred Physical Layer (PHY) is complete.
     */
    struct ble_set_perf_phy_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Status of the preferred PHY setting operation. Indicates whether the operation was successful or if an error occurred. */
    } set_perf_phy;   /*!< ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT event.
     * This event is triggered when the operation to set a random address for extended advertising is complete.
     */
    struct ble_ext_adv_set_rand_addr_cmpl_evt_param {
        esp_bt_status_t status;     /*!< Status of the extended advertising random address setting operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_adv_set_rand_addr;  /*!< ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT event.
     * This event is triggered when the operation to set parameters for extended advertising is complete.
     */
    struct ble_ext_adv_set_params_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Status of the extended advertising parameters setting operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_adv_set_params;   /*!< ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT event.
     * This event is triggered when the operation to set extended advertising data is complete.
     */
    struct ble_ext_adv_data_set_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Status of the extended advertising data setting operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_adv_data_set;    /*!< ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT event.
     * This event is triggered when the operation to set extended scan response data is complete.
     */
    struct ble_ext_adv_scan_rsp_set_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Status of the extended scan response data setting operation. Indicates whether the operation was successful or if an error occurred. */
    } scan_rsp_set;   /*!< ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT event.
     * This event is triggered when the operation to start extended advertising is complete.
     */
    struct ble_ext_adv_start_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Status of the extended advertising start operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_adv_start;    /*!< ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT event.
     * This event is triggered when the operation to stop extended advertising is complete.
     */
    struct ble_ext_adv_stop_cmpl_evt_param {
        esp_bt_status_t status;      /*!< Status of the extended advertising stop operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_adv_stop;     /*!< ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT event.
     * This event is triggered when the operation to remove an extended advertising set is complete.
     */
    struct ble_ext_adv_set_remove_cmpl_evt_param {
        esp_bt_status_t status;       /*!< Status of the extended advertising set removal operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_adv_remove;   /*!< ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT event.
     * This event is triggered when the operation to clear all extended advertising sets is complete.
     */
    struct ble_ext_adv_set_clear_cmpl_evt_param {
        esp_bt_status_t status;        /*!< Status of the extended advertising set clear operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_adv_clear;     /*!< ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT event.
     * This event is triggered when the operation to set parameters for periodic advertising is complete.
     */
    struct ble_periodic_adv_set_params_cmpl_param {
        esp_bt_status_t status;        /*!< Status of the periodic advertising parameters setting operation. Indicates whether the operation was successful or if an error occurred. */
    } peroid_adv_set_params;    /*!< ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT event.
     * This event is triggered when the operation to set periodic advertising data is complete.
     */
    struct ble_periodic_adv_data_set_cmpl_param {
        esp_bt_status_t status;        /*!< Status of the periodic advertising data setting operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_data_set;    /*!< ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT event.
     * This event is triggered when the operation to start periodic advertising is complete.
     */
    struct ble_periodic_adv_start_cmpl_param {
        esp_bt_status_t status;        /*!< Status of the periodic advertising start operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_start;     /*!< ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT event.
     * This event is triggered when the operation to stop periodic advertising is complete.
     */
    struct ble_periodic_adv_stop_cmpl_param {
        esp_bt_status_t status;        /*!< Status of the periodic advertising stop operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_stop;      /*!< ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT event.
     * This event is triggered when the operation to create a synchronization with periodic advertising is complete.
     */
    struct ble_period_adv_create_sync_cmpl_param {
        esp_bt_status_t status;        /*!< Status of the periodic advertising synchronization creation operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_create_sync;    /*!< ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT */


    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT event.
     * This event is triggered when the operation to cancel synchronization with periodic advertising is complete.
     */
    struct ble_period_adv_sync_cancel_cmpl_param {
        esp_bt_status_t status;       /*!< Status of the periodic advertising synchronization cancellation operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_sync_cancel;  /*!< ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT event.
     * This event is triggered when the operation to terminate synchronization with periodic advertising is complete.
     */
    struct ble_period_adv_sync_terminate_cmpl_param {
        esp_bt_status_t status;      /*!< Status of the periodic advertising synchronization termination operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_sync_term;    /*!< ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT event.
     * This event is triggered when the operation to add a device to the periodic advertising device list is complete.
     */
    struct ble_period_adv_add_dev_cmpl_param {
        esp_bt_status_t status;     /*!< Status of the operation to add a device to the periodic advertising device list. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_add_dev;   /*!< ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT */              
    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT event.
     * This event is triggered when the operation to remove a device from the periodic advertising device list is complete.
     */
    struct ble_period_adv_remove_dev_cmpl_param {
        esp_bt_status_t status;     /*!< Status of the periodic advertising device list removal operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_remove_dev;  /*!< ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT event.
     * This event is triggered when the operation to clear all devices from the periodic advertising device list is complete.
     */
    struct ble_period_adv_clear_dev_cmpl_param {
        esp_bt_status_t status;     /*!< Status of the periodic advertising device list clearing operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_clear_dev;     /*!< ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT event.
     * This event is triggered when the operation to set parameters for extended scanning is complete.
     */
    struct ble_set_ext_scan_params_cmpl_param {
        esp_bt_status_t status;     /*!< Status of the extended scan parameters setting operation. Indicates whether the operation was successful or if an error occurred. */
    } set_ext_scan_params;     /*!< ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT event.
     * This event is triggered when the operation to start extended scanning is complete.
     */
    struct ble_ext_scan_start_cmpl_param {
        esp_bt_status_t status;     /*!< Status of the extended scanning start operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_scan_start;         /*!< ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT event.
     * This event is triggered when the operation to stop extended scanning is complete.
     */
    struct ble_ext_scan_stop_cmpl_param {
        esp_bt_status_t status;     /*!< Status of the extended scanning stop operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_scan_stop;          /*!< ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT event.
     * This event is triggered when the operation to set preferred extended connection parameters is complete.
     */
    struct ble_ext_conn_params_set_cmpl_param {
        esp_bt_status_t status;     /*!< Status of the preferred extended connection parameters setting operation. Indicates whether the operation was successful or if an error occurred. */
    } ext_conn_params_set;      /*!< ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_ADV_TERMINATED_EVT
     * Event parameter for the ESP_GAP_BLE_ADV_TERMINATED_EVT event.
     * This event is triggered when extended advertising terminates.
     */
    struct ble_adv_terminate_param {
        uint8_t status;              /*!< Status of the advertising termination. A status value of 0x3c indicates that advertising for a fixed duration completed or that directed advertising completed without creating a connection. A status value of 0x00 indicates that advertising successfully ended with a connection being created. */
        uint8_t adv_instance;        /*!< Handle for the extended advertising instance. */
        uint16_t conn_idx;           /*!< Connection index for the connection established during advertising. */
        uint8_t completed_event;     /*!< Number of completed extended advertising events. */
    } adv_terminate;            /*!< ESP_GAP_BLE_ADV_TERMINATED_EVT */
    /**
     * @brief ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT
     * Event parameter for the ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT event.
     * This event is triggered when a scan request is received during extended advertising.
     */
    struct ble_scan_req_received_param {
        uint8_t adv_instance;                /*!< Handle for the extended advertising instance. */
        esp_ble_addr_type_t scan_addr_type;  /*!< Address type of the scanner. This indicates whether the scanner's address is public or random. */
        esp_bd_addr_t scan_addr;             /*!< Address of the scanner that sent the scan request. */
    } scan_req_received;        /*!< ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT */

    /**
     * @brief ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT
     * Event parameter for the ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT event.
     * This event is triggered when the channel selection algorithm is updated for a connection.
     */
    struct ble_channel_sel_alg_param {
        uint16_t conn_handle;              /*!< Handle for the connection. Used to identify the connection for which the channel selection algorithm is being updated. */
        uint8_t channel_sel_alg;           /*!< Channel selection algorithm used for the connection. This can indicate either the legacy or the new channel selection algorithm. */
    } channel_sel_alg;          /*!< ESP_GAP_BLE_CHANNEL_SELECT_ALGORITHM_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT event.
     * This event is triggered when synchronization with a periodic advertiser is lost.
     */
    struct ble_periodic_adv_sync_lost_param {
        uint16_t sync_handle;              /*!< Handle for the synchronization. Used to identify the synchronization that was lost. */
    } periodic_adv_sync_lost;    /*!< ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT event.
     * This event is triggered when synchronization with a periodic advertiser is established.
     */
    struct ble_periodic_adv_sync_estab_param {
        uint8_t status;                      /*!< Status of the periodic advertising synchronization. A status value of 0 indicates successful synchronization, while other values indicate errors. */
        uint16_t sync_handle;                /*!< Handle for the synchronization. Used to identify the established synchronization. */
        uint8_t sid;                         /*!< Service Identifier for the periodic advertising. Used to identify the type of service. */
        esp_ble_addr_type_t adv_addr_type;   /*!< Address type of the periodic advertiser. Indicates whether the advertiser's address is public or random. */
        esp_bd_addr_t adv_addr;              /*!< Address of the periodic advertiser. Used to identify the source of the periodic advertising. */
        esp_ble_gap_phy_t adv_phy;           /*!< Physical Layer type for the periodic advertising. Indicates the PHY used for advertising. */
        uint16_t period_adv_interval;        /*!< Interval between periodic advertising events. Specifies the period of the advertising interval. */
        uint8_t adv_clk_accuracy;            /*!< Clock accuracy of the periodic advertiser. Indicates how accurate the advertiser's clock is for timing. */
    } periodic_adv_sync_estab;  /*!< ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT */

    /**
     * @brief ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT event.
     * This event is triggered when the Physical Layer (PHY) update operation is complete for a connection.
     */
    struct ble_phy_update_cmpl_param {
        esp_bt_status_t status;             /*!< Status of the PHY update operation. Indicates whether the update was successful or if an error occurred. */
        esp_bd_addr_t bda;                  /*!< Address of the device for which the PHY update was performed. */
        esp_ble_gap_phy_t tx_phy;           /*!< Transmit PHY type after the update. Indicates the PHY used for transmitting data. */
        esp_ble_gap_phy_t rx_phy;           /*!< Receive PHY type after the update. Indicates the PHY used for receiving data. */
    } phy_update;    /*!< ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_EXT_ADV_REPORT_EVT
     * Event parameter for the ESP_GAP_BLE_EXT_ADV_REPORT_EVT event.
     * This event is triggered when an extended advertising report is received.
     */
    struct ble_ext_adv_report_param {
        esp_ble_gap_ext_adv_report_t params;        /*!< Parameters of the extended advertising report. Contains details about the extended advertising packet received. */
    } ext_adv_report;   /*!< ESP_GAP_BLE_EXT_ADV_REPORT_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT event.
     * This event is triggered when a periodic advertising report is received.
     */
    struct ble_periodic_adv_report_param {
        esp_ble_gap_periodic_adv_report_t params;   /*!< Parameters of the periodic advertising report. Contains details about the periodic advertising packet received. */
    } period_adv_report;  /*!< ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT */                   
#endif// #if (BLE_50_FEATURE_SUPPORT == TRUE)

/* if Bluetooth Low Energy Feature Periodic Advertising Synchronization Transfer Support */ 
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT event.
     * This event is triggered when the periodic advertising receive enable operation completes.
     */
    struct ble_periodic_adv_recv_enable_cmpl_param {
        esp_bt_status_t status;             /*!< Status of the periodic advertising receive enable operation. Indicates whether the operation was successful or if an error occurred. */
    } period_adv_recv_enable;   /*!< ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT event.
     * This event is triggered when the periodic advertising synchronization transfer operation completes.
     */
    struct ble_periodic_adv_sync_trans_cmpl_param {
        esp_bt_status_t status;             /*!< Status of the periodic advertising synchronization transfer operation. Indicates whether the transfer was successful or if an error occurred. */
        esp_bd_addr_t bda;                  /*!< Address of the remote device involved in the synchronization transfer. */
    } period_adv_sync_trans;    /*!< ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT event.
     * This event is triggered when the periodic advertising set information transfer operation completes.
     */
    struct ble_periodic_adv_set_info_trans_cmpl_param {
        esp_bt_status_t status;             /*!< Status of the periodic advertising set information transfer operation. Indicates whether the transfer was successful or if an error occurred. */
        esp_bd_addr_t bda;                  /*!< Address of the remote device involved in the information transfer. */
    } period_adv_set_info_trans;    /*!< ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT event.
     * This event is triggered when the periodic advertising synchronization transfer parameters are set.
     */
    struct ble_set_past_params_cmpl_param {
        esp_bt_status_t status;             /*!< Status of the set periodic advertising synchronization transfer parameters operation. Indicates whether the operation was successful or if an error occurred. */
        esp_bd_addr_t bda;                  /*!< Address of the remote device involved in the parameter setting. */
    } set_past_params;      /*!< ESP_GAP_BLE_SET_PAST_PARAMS_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_RECV_EVT
     * Event parameter for the ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_RECV_EVT event.
     * This event is triggered when a periodic advertising synchronization transfer is received.
     */
    struct ble_periodic_adv_sync_trans_recv_param {
        esp_bt_status_t status;             /*!< Status of the received periodic advertising synchronization transfer. Indicates whether the reception was successful or if an error occurred. */
        esp_bd_addr_t bda;                  /*!< Address of the remote device that sent the synchronization transfer. */
        uint16_t service_data;              /*!< Value provided by the peer device in the synchronization transfer. */
        uint16_t sync_handle;               /*!< Handle for the periodic advertising synchronization. Used to identify the synchronization instance. */
        uint8_t adv_sid;                    /*!< Service Identifier for the periodic advertising set. Used to identify the type of periodic advertising service. */
        uint8_t adv_addr_type;              /*!< Address type of the periodic advertiser. Indicates whether the advertiser's address is public or random. */
        esp_bd_addr_t adv_addr;             /*!< Address of the periodic advertiser. Used to identify the source of the periodic advertising. */
        esp_ble_gap_phy_t adv_phy;          /*!< Physical Layer type for the periodic advertising. Indicates the PHY used for periodic advertising. */
        uint16_t adv_interval;              /*!< Interval between periodic advertising events. Specifies the period of the advertising interval. */
        uint8_t adv_clk_accuracy;           /*!< Clock accuracy of the periodic advertiser. Indicates how accurate the advertiser's clock is for timing. */
    } past_received;   /*!< ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_RECV_EVT */         
#endif// #if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

    /**
     * @brief ESP_GAP_BLE_DTM_TEST_UPDATE_EVT
     * Event parameter for the ESP_GAP_BLE_DTM_TEST_UPDATE_EVT event.
     * This event is triggered when the Direct Test Mode (DTM) state changes.
     */
    struct ble_dtm_state_update_evt_param {
        esp_bt_status_t status;                     /*!< Status of the Direct Test Mode operation, indicating whether the operation was successful or if an error occurred. */
        esp_ble_dtm_update_evt_t update_evt;        /*!< Direct Test Mode state change event. Possible values are:
                                                    * 0x00: DTM TX start - Direct Test Mode transmitter started.
                                                    * 0x01: DTM RX start - Direct Test Mode receiver started.
                                                    * 0x02: DTM end - Direct Test Mode ended. */
        uint16_t num_of_pkt;                        /*!< Number of packets received, applicable only if update_evt is DTM_TEST_STOP_EVT (0x02). This field shall be reported as 0 for a transmitter. */
    } dtm_state_update;     /*!< ESP_GAP_BLE_DTM_TEST_UPDATE_EVT */

    /**
     * @brief ESP_GAP_BLE_VENDOR_CMD_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_VENDOR_CMD_COMPLETE_EVT event.
     * This event is triggered when a vendor-specific Host Controller Interface (HCI) command completes.
     */
    struct vendor_cmd_cmpl_evt_param {
        uint16_t opcode;                     /*!< Vendor-specific Host Controller Interface (HCI) command opcode. Identifies the specific vendor command. */
        uint16_t param_len;                  /*!< Length of the parameter buffer associated with the command response. */
        uint8_t *p_param_buf;               /*!< Pointer to the parameter buffer containing the command response data. */
    } vendor_cmd_cmpl;  /*!< ESP_GAP_BLE_VENDOR_CMD_COMPLETE_EVT */

    /**
     * @brief ESP_GAP_BLE_SET_PRIVACY_MODE_COMPLETE_EVT
     * Event parameter for the ESP_GAP_BLE_SET_PRIVACY_MODE_COMPLETE_EVT event.
     * This event is triggered when the privacy mode setting operation completes.
     */
    struct ble_set_privacy_mode_cmpl_evt_param {
        esp_bt_status_t status;             /*!< Status of the privacy mode set operation. Indicates whether the operation was successful or if an error occurred. */
    } set_privacy_mode_cmpl;    /*!< ESP_GAP_BLE_SET_PRIVACY_MODE_COMPLETE_EVT */
} esp_ble_gap_cb_param_t;

/**
 * @brief  Generic Access Profile (GAP) callback function type.
 *
 *  - This function type is used for handling LE GAP events.
 * 
 * @param[in] event : Event type.
 * 
 * @param[in] param : Pointer to callback parameter.
 */
typedef void (* esp_gap_ble_cb_t)(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/**
 * @brief  Register a callback function for handling Generic Access Profile (GAP) events.
 *
 * @param[in]       callback: Callback function.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 */
esp_err_t esp_ble_gap_register_callback(esp_gap_ble_cb_t callback);

/**
 * @brief  Get the current Generic Access Profile callback.
 *
 * @return
 *                  - esp_gap_ble_cb_t : Callback function.
 */
esp_gap_ble_cb_t esp_ble_gap_get_callback(void);

// if Bluetooth Low Energy 4.2 Feature Support
#if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief   Set the advertising or scan response data using a specified structure.
 * 
 *  - Can only set param in `esp_ble_adv_data_t`: `set_scan_rsp`£¬`include_name`£¬`include_txpower`£¬`min_interval`£¬`max_interval`£¬`appearance`£¬`manufacturer_len`£¬
 *                                                `*p_manufacturer_data`£¬`service_data_len`£¬`*p_service_data`£¬`service_uuid_len`£¬`*p_service_uuid`£¬`flag`.
 *    If you want to change sequences/messages or for versatile use (customize any data) use the apis bellow.
 *  - Please use `esp_ble_gap_config_adv_data_raw` to set advertising raw data.
 *  - Please use `esp_ble_gap_config_scan_rsp_data_raw` to set scan response raw data.
 *
 * @param[in]       adv_data: Pointer to advertising data structure.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - When setting to advertising data the function triggers `ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT`.
 *                  - When setting to scan response data the function triggers `ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT`.
 *                  - Configuration data must not exceed 31 bytes, the excess will be truncated.
 *                  - To set the scan response data, please set `set_scan_rsp` to `True` in `esp_ble_adv_data_t`.
 *
 */
esp_err_t esp_ble_gap_config_adv_data(esp_ble_adv_data_t *adv_data);

/**
 * @brief   Set the advertising data using raw data.
 *
 * @param[in]       raw_data: Pointer to the buffer containing the raw advertising data. The format should be as follows:
 *                  [Length 1][Data Type 1][Data 1][Length 2][Data Type 2][Data 2] ...
 *                  Where:
 *                  - [Length N]: Length of the advertising data segment N, including the data type byte.
 *                  - [Data Type N]: Type of the advertising data segment N, as defined by in `esp_ble_adv_data_type`.
 *                  - [Data N]: Actual data for segment N.
 * @param[in]       raw_data_len: Length of the raw advertising data in bytes. The value must be less than 31 bytes.
 *
 * @return          
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            The function triggers `ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_config_adv_data_raw(uint8_t *raw_data, uint32_t raw_data_len);

/**
 * @brief  Set the scan response data using raw data.
 *
 * @param[in]       raw_data: Pointer to the buffer containing the raw scan response data. The format should be as follows:
 *                  [Length 1][Data Type 1][Data 1][Length 2][Data Type 2][Data 2] ...
 *                  Where:
 *                  - [Length N]: Length of the scan response data segment N, including the data type byte.
 *                  - [Data Type N]: Type of the scan response data segment N, as defined in `esp_ble_adv_data_type`.
 *                  - [Data N]: Actual data for segment N.
 * @param[in]       raw_data_len: Length of the raw scan response data in bytes. The value must be less than 31 bytes.
 *
 * @return          
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            The function triggers `ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT`. 
 */
esp_err_t esp_ble_gap_config_scan_rsp_data_raw(uint8_t *raw_data, uint32_t raw_data_len);

/**
 * @brief  Set advertising parameters and start advertising.
 *
 * @param[in]       adv_params: Pointer to advertising parameters structure (`esp_ble_adv_params_t`).
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - This api can also be used in changing advertising parameters when advertising is enabled.
 *                  - The function triggers `ESP_GAP_BLE_ADV_START_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_start_advertising(esp_ble_adv_params_t *adv_params);

/**
 * @brief  Stop Bluetooth Low Energy (LE) advertising.
 *
 *  - This function halts the process of broadcasting advertising packets to other BLE devices,
 *   which were previously initiated by the `esp_ble_gap_start_advertising` function.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            The function triggers `ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_stop_advertising(void);

/**
 * @brief  Set scan parameters.
 *
 *  - Configure the parameters that determine how the LE device scans for LE devices.
 * 
 * @param[in]       scan_params: Pointer to scan parameters data structure (`esp_ble_scan_params_t`).
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - If the scanning is in progress, stop scan before set scan params using `esp_ble_gap_stop_scan`.
 *                  - The function triggers `ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_set_scan_params(esp_ble_scan_params_t *scan_params);

/**
 * @brief  Start a scanning procedure to search for Bluetooth Low Energy (LE) devices advertising.
 *
 * @param[in]       duration: The duration (unit: 1s) for which the device should keep scanning.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - For continuous scanning, set `duration` to `0`.
 *                  - The function triggers `ESP_GAP_BLE_SCAN_START_COMPLETE_EVT` and `ESP_GAP_BLE_SCAN_RESULT_EVT`.
 *                  - When start scanning complete, it triggers `ESP_GAP_BLE_SCAN_START_COMPLETE_EVT`.
 *                  - When a device is scanned, it triggers the `ESP_GAP_BLE_SCAN_RESULT_EVT`, with sub event set to `ESP_GAP_SEARCH_INQ_RES_EVT`.
 *                  - When the scanning duration expires, it triggers the `ESP_GAP_BLE_SCAN_RESULT_EVT`, with sub event set to `ESP_GAP_SEARCH_INQ_CMPL_EVT`.
 */
esp_err_t esp_ble_gap_start_scanning(uint32_t duration);

/**
 * @brief  Stop scanning procedure.
 *
 *  - It stops the ongoing scanning procedure that was initiated by calling `esp_ble_gap_start_scanning`.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - If scanning was not started or if scanning was already stopped, this function will still return `ESP_OK`,
 *                    indicating that the request to stop scanning was processed correctly.
 *                  - The function triggers `ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_stop_scanning(void);

/**
 * @brief  Get advertising data for a specific type in local advertising data.
 *
 *  - Advertising data in Bluetooth Low Energy (LE) can contain various types of information such as 
 *    device name, appearance, manufacturer data, etc see more in `esp_ble_adv_data_type`.
 *
 * @param[in]       adv_data: Pointer to the buffer containing the advertising data.
 * @param[in]       type: The type of advertising data see detail in `esp_ble_adv_data_type`.
 * @param[out]      length: Pointer to a variable where the length of the extracted advertising data (excluding the type)
 *                  will be stored.
 *
 * @return          Pointer to the extracted advertising data. If the specified type is found in the `adv_data`,
 *                  this function returns a pointer to the start of that data. If the type is not found, it returns `NULL`.
 *
 * @note            E.g. If you want to get the complete name in local advertising data, set type to `ESP_BLE_AD_TYPE_NAME_CMPL`.
 */
uint8_t *esp_ble_resolve_adv_data(uint8_t *adv_data, uint8_t type, uint8_t *length);

/**
 * @brief  Add a device's information to the duplicate scan exceptional list.
 *
 * @param[in]       type: The type of device information to add. 
 *                        - It determines the category of the device information and how it will be treated in the duplicate scan exceptional list:
 *                          - `MESH_BEACON_TYPE`: Indicates the device is a mesh beacon.
 *                          - `MESH_PROV_SRV_ADV`: Indicates the device is a mesh provisioning service advertiser.
 *                          - `MESH_PROXY_SRV_ADV`: Indicates the device is a mesh proxy service advertiser.
 *                        - For these types, `device_info` is considered invalid.
 * @param[in]       device_info: The information of the device to be added to the exceptional list.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            The function triggers `ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT`, subevent code `ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_ADD`.
 */
esp_err_t esp_ble_gap_add_duplicate_scan_exceptional_device(esp_ble_duplicate_exceptional_info_type_t type, esp_duplicate_info_t device_info);

/**
 * @brief  Remove a device's information from the duplicate scan exceptional list.
 *
 * @param[in]       type: The type of device information to remove.
 *                        - This is defined in the `esp_ble_duplicate_exceptional_info_type_t` enumeration.
 *                        - It specifies the category of the device information to be removed from the exceptional list:
 *                          - `MESH_BEACON_TYPE`: Indicates the device is a mesh beacon.
 *                          - `MESH_PROV_SRV_ADV`: Indicates the device is a mesh provisioning service advertiser.
 *                          - `MESH_PROXY_SRV_ADV`: Indicates the device is a mesh proxy service advertiser.
 *                        - For these types, `device_info` is considered invalid.
 * @param[in]       device_info: The information of the device to be removed from the exceptional list.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            The function triggers `ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT`, subevent code `ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_ADD`
 */
esp_err_t esp_ble_gap_remove_duplicate_scan_exceptional_device(esp_ble_duplicate_exceptional_info_type_t type, esp_duplicate_info_t device_info);

/**
 * @brief  Clean the duplicate scan exceptional list.
 *
 * @param[in]       list_type: The type of duplicate scan exceptional list to clean.
 *                             - This parameter is defined in the `esp_duplicate_scan_exceptional_list_type_t` enumeration and can include one or more types:
 *                               - `ESP_DUPLICATE_SCAN_EXCEPTIONAL_LIST_TYPE_BEACON`: For mesh beacon devices.
 *                               - `ESP_DUPLICATE_SCAN_EXCEPTIONAL_LIST_TYPE_PROVISIONING`: For mesh provisioning service advertisers.
 *                               - `ESP_DUPLICATE_SCAN_EXCEPTIONAL_LIST_TYPE_PROXY`: For mesh proxy service advertisers.
 *                             - The value can be a combination of these types, allowing you to specify which lists to clear.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            
 *                  - This function deletes all device information from the duplicate scan exceptional list. 
 *                  - The function triggers `ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT`, Subevent code `ESP_BLE_DUPLICATE_EXCEPTIONAL_LIST_CLEAN`
 */
esp_err_t esp_ble_gap_clean_duplicate_scan_exceptional_list(esp_duplicate_scan_exceptional_list_type_t list_type);

/**
 * @brief  This function is used to set the preferred connection parameters when default connection parameter is not desired before connecting.
 *
 * @param[in]       bd_addr: The Bluetooth Device Address (BD_ADDR) of the peripheral device for which the preferred connection parameters are being set.
 * @param[in]       min_conn_int: Preferred minimum connection interval.
 *                  - Unit: 1.25ms
 *                  - Range: 0x0006 to 0x0C80 (7.5ms to 4s)
 * @param[in]       max_conn_int: Preferred maximum connection interval.
 *                  - Unit: 1.25ms
 *                  - Range: 0x0006 to 0x0C80 (7.5ms to 4s)
 *                  - `max_conn_int` shall be equal to or greater than `min_conn_int`, when both are equal, the interval is definite, otherwise value in [min_conn_int,max_conn_int].
 * @param[in]       slave_latency: Preferred slave latency. This is the number of connection events that the slave device is allowed to skip.
 *                  - Range: 0x0000 to 0x01F3 (499)
 * @param[in]       supervision_tout: Preferred supervision timeout, in ms. This defines the time period within which the connection must be maintained to avoid disconnection.
 *                  - Unit: 10ms
 *                  - Range: 0x000A (100 ms) to 0x0C80 (32s)
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - It is applicable only when the device is in the initiator (as master role).
 *                  - The default value will exist before using this api:
 *                    - conn_int: 0x14 (25ms)
 *                    - slave_latency: 0x0000 (0)
 *                    - supervision_tout: 0x0258 (6s)
 *                  - Set `supervision_tout` larger to avoid interference.
 */
esp_err_t esp_ble_gap_set_prefer_conn_params(esp_bd_addr_t bd_addr,
                                             uint16_t min_conn_int, uint16_t max_conn_int,
                                             uint16_t slave_latency, uint16_t supervision_tout);
#endif// #if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief  Update the Bluetooth Low Energy (LE) connection parameters for an existing connection.
 *
 * @param[in]       params: Pointer to the structure containing the new connection parameters.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - The function triggers `ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT`.
 */
esp_err_t esp_ble_gap_update_conn_params(esp_ble_conn_update_params_t *params);

/**
 * @brief  Get the connection parameters of the peer device.
 *
 * @param[in]       bd_addr: The Bluetooth Device Address (BD_ADDR) of the peer device.
 * @param[out]      conn_params: Pointer to an `esp_gap_conn_params_t` structure where the connection 
 *                  parameters information will be stored.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 */
esp_err_t esp_ble_get_current_conn_params(esp_bd_addr_t bd_addr, esp_gap_conn_params_t *conn_params);

/**
 * @brief  Disconnect the physical connection of the peer device.
 *
 * @param[in]       remote_device: The Bluetooth Device Address (BD_ADDR) of the peer device 
 *                  to be disconnected.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - This function triggers `ESP_GATTC_DISCONNECT_EVT` when service setting to client.
 *                  - This function triggers `ESP_GATTS_DISCONNECT_EVT` when service setting to server.
 *                  - This function triggers `ESP_GATTC_DISCONNECT_EVT` and `ESP_GATTS_DISCONNECT_EVT` when service setting to client and role.
 *                  - This function is used to terminate the connection at the physical level, 
 *                    which means all virtual GATT server connections associated with the peer 
 *                    device will also be terminated.
 *                  - If there are virtual GATT server connections `esp_ble_gatts_close` can be used to close a specific GATT connection,
 *                    If there are virtual GATT client connections `esp_ble_gattc_close` can be used to close a specific GATT connection,
 *                    when all virtual connections are disconnected, physical connections are also disconnected.
 */
esp_err_t esp_ble_gap_disconnect(esp_bd_addr_t remote_device);

/**
 * @brief  Set the maximum LE packet data length.
 *
 * @param[in]       remote_device: Address of the remote LE device for which the data length is being set.
 *
 * @param[in]       tx_data_length: Maximum transmission payload size to be used for Link Layer (LL) Data Protocol Data Unit (PDU) on a given connection.
 *                                  Range: 0x001B (27) to 0x00FB (251).
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - The Controller may use smaller or larger values based on local information.
 *                  - The function triggers `ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT`.
 *                  - The default of data length is 27.
 */
esp_err_t esp_ble_gap_set_pkt_data_len(esp_bd_addr_t remote_device, uint16_t tx_data_length);

/**
 * @brief  Read the Received Signal Strength Indication (RSSI) of a remote Bluetooth device.
 *  - The Received Signal Strength Indication is a measurement of the signal strength of the Bluetooth signal received from a remote device. 
 *
 * @param[in]       remote_addr: The Bluetooth Device Address (BD_ADDR) of the remote device whose RSSI value is to be read.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            
 *                  - The RSSI value can provide insight into the quality of the connection with the remote device.
 *                  - The function triggers `ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_read_rssi(esp_bd_addr_t remote_addr);

/**
 * @brief  This function allows configuring either a Non-Resolvable Private Address (NRPA) or a Static Random Address.
 *
 *  - A Non-Resolvable Private Address is used to enhance privacy by preventing the device from being easily tracked,
 *    while a Static Random Address is a fixed, randomly generated address that does not change.
 *
 * @param[in]       rand_addr: The address to be configured. The address format and subtype are as follows:
 *
 *  | address [47:46] |              Address Type                  |        Corresponding API       |
 *  |-----------------|--------------------------------------------|--------------------------------|
 *  |      0b00       | Non-Resolvable Private <br> Address (NRPA) |  esp_ble_gap_addr_create_nrpa  |
 *  |      0b11       |            Static Random Address           | esp_ble_gap_addr_create_static |
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - The function triggers `ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT`.
 */
esp_err_t esp_ble_gap_set_rand_addr(esp_bd_addr_t rand_addr);

/**
 * @brief  Create a static device address.
 *
 *  - A Static Random Address is a fixed, randomly generated address that does not change.
 *
 * @param[out]      rand_addr: Pointer to the buffer where the static device address will be stored.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - The buffer pointed to by `rand_addr` should be allocated by the user and must be large enough
 *                    to hold a Bluetooth device address (BD_ADDR), which is 6 bytes.
 *                  - The function triggers `ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT`.
 */
esp_err_t esp_ble_gap_addr_create_static(esp_bd_addr_t rand_addr);

/**
 * @brief  Create a Non-Resolvable Private Address (NRPA).
 *
 *  - A Non-Resolvable Private Address (NRPA) is used to enhance privacy by preventing the device from being easily tracked.
 *
 * @param[out]      rand_addr: Pointer to the buffer where the NRPA will be stored.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - The function triggers `ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT`.
 */
esp_err_t esp_ble_gap_addr_create_nrpa(esp_bd_addr_t rand_addr);

/**
 * @brief  Set the length of time that the Bluetooth Controller uses a Resolvable Private Address (RPA)
 *         before generating and starting to use a new Resolvable Private Address.
 *
 *  - A Resolvable Private Address (RPA) is used to enhance privacy by allowing devices to periodically change their Bluetooth address 
 *    while still being able to resolve the address back to the original device.
 *
 *                  The timeout value specifies how long the current RPA should be used before the Controller switches to a new
 *                  RPA. The timeout must be within the range specified by the Bluetooth Specification (0x0001 to 0x0E10), which
 *                  corresponds to a time range of 1 second to 1 hour. The default timeout value is 0x0384 (900 sec or 15 minutes).
 *
 * @param[in]       rpa_timeout: The duration in sec for how long the Resolvable Private Address (RPA) is used before a
 *                  new RPA is generated.
 *                  - Unit: 1 sec.
 *                  - Range: 0x0001 to 0x0E10 (1 sec to 1h).
 *                  - Default: 0x0384 (15 min).
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - This function is currently not supported on the ESP32 but will be enabled in a future update.
 *                  - The function triggers `ESP_GAP_BLE_SET_RPA_TIMEOUT_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_set_resolvable_private_address_timeout(uint16_t rpa_timeout);

/**
 * @brief  Adds a device to the resolving list used to generate and resolve Resolvable Private Addresses (RPAs) in the Bluetooth Controller.
 *  - The resolving list is utilized to manage devices with which the Controller has previously exchanged Identity Resolving Keys (IRKs).
 *  - The added device will be used to generate and resolve Resolvable Private Addresses during Bluetooth communication.
 *
 * @param[in]       peer_addr: The peer identity address of the device to be added to the resolving list. This is a Bluetooth
 *                  device address (BD_ADDR) that identifies the peer device.
 * @param[in]       addr_type: The address type of the peer identity address. It can be either `BLE_ADDR_TYPE_PUBLIC` for a public
 *                  address or `BLE_ADDR_TYPE_RANDOM` for a random address.
 * @param[in]       peer_irk: The Identity Resolving Key (IRK) of the device. This key is used to generate and resolve Resolvable
 *                  Private Addresses for the device.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            This function should not be used when address resolution is enabled in the Controller and:
 *                      - Advertising (other than periodic advertising) is enabled,
 *                      - Scanning is enabled, or
 *                      - An HCI_LE_Create_Connection, HCI_LE_Extended_Create_Connection, or HCI_LE_Periodic_Advertising_Create_Sync
 *                        command is pending.
 *                  This command may be used at any time when address resolution is disabled in the Controller.
 *                  The added device must be set to Network Privacy mode.
 */
esp_err_t esp_ble_gap_add_device_to_resolving_list(esp_bd_addr_t peer_addr, uint8_t addr_type, uint8_t *peer_irk);

/**
 * @brief  Clears the random address for the application.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            Clearing the random address will not affect other Bluetooth settings or connections.
 */
esp_err_t esp_ble_gap_clear_rand_addr(void);

/**
 * @brief  This function enables or disables privacy including address resolution on the local device.
 *
 * @param[in]       privacy_enable: Boolean flag to enable or disable privacy on the local device.
 *                  - `true`  : Enable privacy using Resolvable Private Addresses (RPA).
 *                  - `false` : Disable privacy and use static or public addresses.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            Enabling privacy can impact the device's discoverability and connection process, as other devices 
 *                  may need to resolve the RPA to establish a connection.
 * 
 *                  The function triggers `ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_config_local_privacy(bool privacy_enable);

/**
 * @brief  This function sets the local Generic Access Profile (GAP) appearance icon.
 *
 * @param[in]       icon   - The appearance value to be set. These values are defined by the Bluetooth SIG and can be
 *                  referenced in the Bluetooth Assigned Numbers document available at:
 *                  https://www.bluetooth.com/specifications/assigned-numbers/
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 */
esp_err_t esp_ble_gap_config_local_icon(uint16_t icon);

/**
 * @brief  Add or remove a device from the whitelist.
 *
 * @param[in]       add_remove: The action to be performed on the whitelist. Set to `true` to add the device to the
 *                  whitelist, or `false` to remove the device from the whitelist.
 * @param[in]       remote_bda: The Bluetooth device address (BD_ADDR) of the device to be added or removed from the
 *                  whitelist. This address uniquely identifies the device.
 * @param[in]       wl_addr_type: The address type of the device being added or removed.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            The function triggers `ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT`.
 */
esp_err_t esp_ble_gap_update_whitelist(bool add_remove, esp_bd_addr_t remote_bda, esp_ble_wl_addr_type_t wl_addr_type);

/**
 * @brief  Clear all entries from the whitelist.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            Clearing the whitelist will affect the device's ability to connect to or interact with
 *                  previously whitelisted devices. Ensure that you reconfigure the whitelist as needed after clearing it.
 */
esp_err_t esp_ble_gap_clear_whitelist(void);

/**
 * @brief  Get the size of the whitelist in the controller.
 *
 * @param[out]       length: Pointer to a variable where the whitelist size will be stored. This variable will be
 *                  updated with the current number of entries that the whitelist can accommodate.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 */
esp_err_t esp_ble_gap_get_whitelist_size(uint16_t *length);

/**
 * @brief  Set the device name for the local Bluetooth Low Energy (LE) device.
 *
 * @param[in]       name: The device name to be set. This should be a null-terminated string for no more than 31 bytes.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            This API only updates the device name in the local device's settings. It does not affect the 
 *                  advertising data or any other parameters related to LE advertising.
 */
esp_err_t esp_ble_gap_set_device_name(const char *name);

/**
 * @brief           Retrieve the device name of the local Bluetooth Low Energy (LE) device.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            This function only retrieves the device name; it does not modify or set it.
 *                  To set or update the device name, use the `esp_ble_gap_set_device_name` function. Ensure that 
 *                  the device name has been previously set using `esp_ble_gap_set_device_name` before calling this 
 *                  function to obtain it.
 */
esp_err_t esp_ble_gap_get_device_name(void);

/**
 * @brief           Retrieve the local Bluetooth Low Energy (LE) device's currently used address and its type.
 *
 * @note            The address type can indicate whether the address is public, random (static), or resolvable
 *                  private. The function `esp_bt_dev_get_address` can be used to get the public address of the device.
 *
 * @param[out]      local_used_addr: Pointer to a buffer where the current local LE address will be stored. This buffer
 *                  should be large enough to hold the Bluetooth device address (BD_ADDR), which is 6 bytes.
 * @param[out]      addr_type: Pointer to a variable where the address type will be stored. This value indicates the type
 *                  of the address (e.g., public or random).
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - The function trigger `ESP_GAP_BLE_GET_DEV_NAME_COMPLETE_EVT`.
 *                  - The address type can indicate whether the address is public, random (static), or resolvable
 *                  private. The function `esp_bt_dev_get_address` can be used to get the public address of the device.
 */
esp_err_t esp_ble_gap_get_local_used_addr(esp_bd_addr_t local_used_addr, uint8_t * addr_type);

/* if Security Manager Protocol Feature Support */
#if (SMP_INCLUDED == TRUE)

/**
 * @brief  Set a Generic Access Profile (GAP) security parameter value. Overrides the default value.
 *
 * @param[in]       param_type : The type of the parameter to be set.
 * @param[in]       value  : Pointer to the parameter value to be set.
 * @param[in]       len : The length of the parameter value.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note
 *                  - Secure connection is highly recommended to avoid major vulnerabilities such as'Impersonation in the Pin Pairing Protocol' (CVE-2020-26555) 
 *                    and 'Authentication of the Low Energy Legacy Pairing Protocol'.
 *                  - To accept only secure connection mode, follow these steps:
 *                    - Set the bit `ESP_LE_AUTH_REQ_SC_ONLY` (param_type is `ESP_BLE_SM_AUTHEN_REQ_MODE`), 
 *                      and optionally set the bits `ESP_LE_AUTH_BOND` and `ESP_LE_AUTH_REQ_MITM` as required.
 *                    - Set to `ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_ENABLE` (param_type is `ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH`).
 */
esp_err_t esp_ble_gap_set_security_param(esp_ble_sm_param_t param_type, void *value, uint8_t len);

/**
 * @brief  Accept or reject the security request.
 *
 * @param[in]       bd_addr : Bluetooth Device address of the peer.
 * @param[in]       accept  : True indicate the security request is accepted, false indicate the security request is denied.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note
 *                  - This function used to response the `ESP_GAP_BLE_SEC_REQ_EVT`.
 *                  - This function may trigger `ESP_GAP_BLE_AUTH_CMPL_EVT`.
 */
esp_err_t esp_ble_gap_security_rsp(esp_bd_addr_t bd_addr, bool accept);

/**
 * @brief Start the pairing procedure and encryption procedure.
 *
 * @param[in]         bd_addr : The address of the peer Bluetooth device that needs encryption.
 * @param[in]         sec_act : The security action indicating the required Bluetooth Low Energy (LE) security level for 
 *                              the LE link.
 *
 * @return
 *                    - ESP_OK : Success.
 *                    - ESP_FAIL : Failed.
 */
esp_err_t esp_ble_set_encryption(esp_bd_addr_t bd_addr, esp_ble_sec_act_t sec_act);

/**
 * @brief  Reply the passkey value to the peer device during the legacy connection stage.
 *
 * @param[in]         bd_addr : The Bluetooth Device address of the peer.
 * @param[in]         accept  : True indicate local device accepts the entry device's pairing request and continues to complete the pairing process,
 *                              false indicate local device rejects the pairing request and the pairing process is declined.
 * @param[in]         passkey : The passkey value, which must be a 6-digit number (e.g., 000000 to 999999).
 *                              - Leading zeros are allowed.
 *
 * @return
 *                    - ESP_OK : Success.
 *                    - ESP_FAIL : Failed.
 *
 * @note              This function ralated to `ESP_GAP_BLE_AUTH_CMPL_EVT` and `ESP_GAP_BLE_PASSKEY_REQ_EVT`.
 */
esp_err_t esp_ble_passkey_reply(esp_bd_addr_t bd_addr, bool accept, uint32_t passkey);

/**
 * @brief  Reply the confirm value to the peer device during the secure connection stage.
 *
 * @param[in]       bd_addr : The Bluetooth Device address of the peer device.
 * @param[in]       accept  : True indicate accept the numeric compare request and continues to complete the pairing process,
 *                            false indicate reject numeric compare request and the pairing process is declined.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            This function may trigger ` ESP_GAP_BLE_NC_REQ_EVT`.
 */
esp_err_t esp_ble_confirm_reply(esp_bd_addr_t bd_addr, bool accept);

/**
 * @brief  This function is called to provide the Out of Band (OOB) data for Security Manager Protocol (SMP).
 *
 * @param[in]       bd_addr: Bluetooth Device Address (BD_ADDR) of the peer device.
 * @param[in]       TK: Temporary Key (TK) value, which shall be a 128-bit random number.
 * @param[in]       len: Length of the Temporary Key. It should always be 128 bits (16 bytes).
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            This function is called to response `ESP_GAP_BLE_OOB_REQ_EVT`.
 */
esp_err_t esp_ble_oob_req_reply(esp_bd_addr_t bd_addr, uint8_t *TK, uint8_t len);

/**
 * @brief  This function is called to provide the Out of Band (OOB) data for Security Manager Protocol (SMP) in Secure Connections (SC).
 *
 * @param[in]       bd_addr: Bluetooth Device Address (BD_ADDR) of the peer device.
 * @param[in]       p_c: Confirmation value, which shall be a 128-bit (16 bytes) random number.
 * @param[in]       p_r: Random value, which should be a 128-bit (16 bytes) random number.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            This function is called to response `ESP_GAP_BLE_SC_OOB_REQ_EVT`.
 */
esp_err_t esp_ble_sc_oob_req_reply(esp_bd_addr_t bd_addr, uint8_t p_c[16], uint8_t p_r[16]);

/**
 * @brief  This function is called to create the Out of Band (OOB) data for Security Manager Protocol (SMP) during Secure Connections (SC).
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 * 
 * @note            This function is called in response to ` ESP_GAP_BLE_SC_CR_LOC_OOB_EVT`.
 */
esp_err_t esp_ble_create_sc_oob_data(void);

/**
 * @brief  Removes a device from the security database list of peer devices.
 *  - It unbonds the local device from the peer device, removing any stored security information such as keys and pairing status.
 *
 * @param[in]       bd_addr : The Bluetooth Device address of the peer device.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 *
 * @note            This function may trigger `ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT`.
 */
esp_err_t esp_ble_remove_bond_device(esp_bd_addr_t bd_addr);

/**
 * @brief  Get the number of devices from the security database list of peer devices.
 *
 * @return
 *                  - >= 0 : The number of bonded devices.
 *                  - ESP_FAIL : Failed.
 */
int esp_ble_get_bond_device_num(void);

/**
 * @brief  Get the list of bonded devices from the security database of peer devices.
 * 
 * @param[inout]    dev_num: Indicates the size of the `dev_list` array (buffer) as input.
 *                           - If `dev_num` is large enough, it returns the actual number of bonded devices as output.
 *                           - Suggest that the `dev_num` value is equal to the value returned by `esp_ble_get_bond_device_num`.
 * 
 * @param[out]      dev_list: An array (buffer) of `esp_ble_bond_dev_t` type used to store the bonded devices' addresses.
 *                            - The `dev_list` should be allocated by the caller of this API.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 */
esp_err_t esp_ble_get_bond_device_list(int *dev_num, esp_ble_bond_dev_t *dev_list);

/**
 * @brief  Authorize a link after authentication, providing Man-in-the-Middle (MITM) protection.
 * - Authorization determines whether a previously authenticated link should be allowed or not.
 *
 * @param[in]       bd_addr: Bluetooth Device address of the peer device.
 * @param[in]       authorize: True indicate to authorize the link, allowing it to proceed, false indicate to deny authorization, prevent further communication with the peer device.
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_FAIL : Failed.
 */
esp_err_t esp_gap_ble_set_authorization(esp_bd_addr_t bd_addr, bool authorize);

#endif// #if (SMP_INCLUDED == TRUE)

/**
 * @brief  Set the channels for Bluetooth Low Energy (LE) communication.
 *
 * @param[in]       channels: A bit field where each bit represents the status of a LE channel.
 *                  - The n-th bit (in the range 0 to 36) represents the status of the link layer channel index n.
 *                    - `0` means channel n is marked as "bad".
 *                    - `1` means channel n is marked as "unknown".
 *                  - The most significant bits are reserved and should be set to `0`.
 *                  - At least one channel must be marked as "unknown".
 *
 * @return
 *                  - ESP_OK : Success.
 *                  - ESP_ERR_INVALID_STATE : If bluetooth stack is not yet enable.
 *                  - ESP_FAIL : Failed.
 *
 * @note            This function trigger `ESP_GAP_BLE_SET_CHANNELS_EVT`.
 */
esp_err_t esp_gap_ble_set_channels(esp_gap_ble_channels channels);

/* if Bluetooth Low Energy 5.0 Feature Support */
#if (BLE_50_FEATURE_SUPPORT == TRUE)

/**
 * @brief           This function is used to read the current Physical Layer (PHY) parameters
 *                  for the transmitter and receiver on the connection identified by the Bluetooth
 *                  Device Address (BD_ADDR) of the peer device.
 *                  
 *                  The Physical Layer (PHY) defines the radio frequency and modulation scheme used
 *                  for communication. This function retrieves the PHY information that is currently
 *                  being used for the connection, which includes details about the transmitter and receiver PHY.
 *
 * @param[in]       bd_addr: The Bluetooth Device Address (BD_ADDR) of the peer device. This address
 *                  identifies the connection for which PHY parameters are to be read.
 *
  * @return          
 *                  - ESP_OK : Success, the PHY parameters were read successfully.
 *                  - ESP_FAIL : Failed, the PHY parameters could not be read. Refer to the error codes for details.
 * 
 * @note            This function may trigger the following event:
 *                  - `ESP_GAP_BLE_READ_PHY_COMPLETE_EVT`: Triggered when the reading of physical layer (PHY) information is complete.
 *                  Notifies the application of the result of reading PHY information.
 */
esp_err_t esp_ble_gap_read_phy(esp_bd_addr_t bd_addr);

/**
 * @brief           This function allows the Host to specify its preferred values for the Physical Layer (PHY)
 *                  parameters to be used for both transmitting and receiving data in all subsequent Bluetooth Low Energy (BLE)
 *                  connections over the low energy transport.
 *                  
 *                  The Physical Layer (PHY) determines the radio frequency and modulation scheme used for Bluetooth communication.
 *                  This function lets the Host indicate which PHY parameters it prefers for both transmitting and receiving data
 *                  during BLE connections. The specified preferences will be considered by the Controller when establishing
 *                  new BLE connections.
 *
 * @param[in]       tx_phy_mask: Indicates the preferred Physical Layers for transmitting data. The mask is a bitfield
 *                  where each bit represents a different PHY option. For example, `ESP_BLE_PHY_LE_1M` for 1 Mbps PHY,
 *                  and `ESP_BLE_PHY_LE_2M` for 2 Mbps PHY.
 * @param[in]       rx_phy_mask: Indicates the preferred Physical Layers for receiving data. The mask is a bitfield
 *                  similar to `tx_phy_mask` and represents the preferred PHY options for reception.
 *
  * @return          
 *                  - ESP_OK : Success, the preferred PHY parameters were set successfully.
 *                  - ESP_FAIL : Failed, the preferred PHY parameters could not be set. Refer to the error codes for details.
 * 
 * @note            This function may trigger the following event:
 *                  - `ESP_GAP_BLE_SET_PREFERRED_DEFAULT_PHY_COMPLETE_EVT`: Triggered when setting the default physical layer (PHY) is complete, 
 *                  notifies the application of the result of setting the default PHY.
 */
esp_err_t esp_ble_gap_set_preferred_default_phy(esp_ble_gap_phy_mask_t tx_phy_mask, esp_ble_gap_phy_mask_t rx_phy_mask);

/**
 * @brief           This function sets the Physical Layer (PHY) preferences for a specific BLE connection identified by the remote address.
 *                  The Controller will attempt to apply these preferences, but it may not always succeed if:
 *                  - The peer device does not support the requested PHY options.
 *                  - The Controller determines that the current PHY settings are preferable.
 *                  
 *                  Physical Layer (PHY) settings affect the radio frequency and modulation scheme used for Bluetooth communication.
 *                  This function allows the Host to specify its preferred PHY parameters for transmitting and receiving data
 *                  for a particular connection. The PHY options may include data rates, modulation schemes, and other related settings.
 *
 * @param[in]       bd_addr: The Bluetooth Device Address of the remote peer device. This identifies the connection for which
 *                  the PHY preferences are being set.
 * @param[in]       all_phys_mask: A bit field specifying which Physical Layers are available for preference. This allows
 *                  the Host to specify which PHY options it wants the Controller to consider for both transmitting and receiving.
 * @param[in]       tx_phy_mask: A bit field indicating the preferred Physical Layers for transmitting data. This mask specifies
 *                  which PHY options the Host prefers the Controller to use for sending data.
 * @param[in]       rx_phy_mask: A bit field indicating the preferred Physical Layers for receiving data. This mask specifies
 *                  which PHY options the Host prefers the Controller to use for receiving data.
 * @param[in]       phy_options: A bit field allowing the Host to specify additional options for the Physical Layers, such as
 *                  preference for low latency or higher throughput.
 *
  * @return          
 *                  - ESP_OK : Success, the PHY preferences were set successfully for the specified connection.
 *                  - ESP_FAIL : Failed, the PHY preferences could not be set. Refer to the error codes for details.
 * 
 * @note            This function may trigger the following event:
 *                  - `ESP_GAP_BLE_SET_PREFERRED_PHY_COMPLETE_EVT`: Triggered when setting the physical layer (PHY) is complete,
 *                  notifies the application of the result of setting the PHY.
 */
esp_err_t esp_ble_gap_set_preferred_phy(esp_bd_addr_t bd_addr,
                                       esp_ble_gap_all_phys_t all_phys_mask,
                                       esp_ble_gap_phy_mask_t tx_phy_mask,
                                       esp_ble_gap_phy_mask_t rx_phy_mask,
                                       esp_ble_gap_prefer_phy_options_t phy_options);

/**
 * @brief           This function sets the random device address for a specific advertising instance. 
 *                  The random address will be used during advertising operations for the specified instance.
 *                  
 *                  Random addresses enhance privacy by periodically changing the address, helping to prevent 
 *                  device tracking and improving anonymity.
 *
 * @param[in]       instance  : Identifies the advertising set for which the random address is being set. 
 *                  Each advertising set is identified by a unique instance number, allowing multiple sets to 
 *                  be managed independently.
 * @param[in]       rand_addr : The random device address to be set. This should be a valid 6-byte Bluetooth 
 *                  Device Address (BD_ADDR) in hexadecimal format.
 *
  * @return          
 *                  - ESP_OK : Success. The random address has been successfully set for the specified instance.
 *                  - ESP_FAIL : Failed. An error occurred while setting the random address.
 *
 * @note            Ensure that the Bluetooth stack is initialized and the instance identifier is valid before
 *                  calling this function. If the random address format is incorrect or if the instance number 
 *                  is out of range, the function may return an error. 
 *                  
 *                  After setting the random address, observe the following events:
 *                  - `ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT`: Triggered when the Extended Ad Random Address setup is complete,
 *                  notifies the application of the result of setting the random address.
 */
esp_err_t esp_ble_gap_ext_adv_set_rand_addr(uint8_t instance, esp_bd_addr_t rand_addr);

/**
 * @brief           This function is used by the Host to configure the advertising parameters for a specific 
 *                  advertising instance. The parameters define how the advertising set will behave, including 
 *                  settings such as the advertising interval, type, and other configurations.
 *                  
 *                  Proper configuration of these parameters is essential for effective advertising and optimal 
 *                  performance of Bluetooth Low Energy (BLE) operations.
 *
 * @param[in]       instance : Identifies the advertising set whose parameters are being configured. 
 *                  Each advertising set is identified by a unique instance number, allowing multiple sets to 
 *                  be managed independently.
 * @param[in]       params   : Pointer to a structure containing the advertising parameters. The structure 
 *                  should be of type `esp_ble_gap_ext_adv_params_t`, which includes fields such as the advertising 
 *                  interval, advertising type, channel map, and other related settings.
 *
  * @return          
 *                  - ESP_OK : Success. The advertising parameters have been successfully set for the specified instance.
 *                  - ESP_FAIL : Failed. An error occurred while setting the advertising parameters.
 * 
 * @note            This function may trigger the following event:
 *                  - `ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT`: Triggered when the extended advertisement parameter setting is complete,
 *                  notifies the application of the result of setting the advertisement parameter.
 */
esp_err_t esp_ble_gap_ext_adv_set_params(uint8_t instance, const esp_ble_gap_ext_adv_params_t *params);

/**
 * @brief           This function is used by the Host to configure the raw advertising data for a specific 
 *                  advertising instance. The data field in the advertising Protocol Data Units (PDUs) can be 
 *                  set with this function, allowing customization of the information broadcasted during 
 *                  advertising operations.
 *                  
 *                  The raw advertising data should be provided in the format specified by the Bluetooth 
 *                  specification, with the length and content properly defined to match the requirements of 
 *                  the intended advertising configuration.
 *
 * @param[in]       instance : Identifies the advertising set whose data is being configured. This allows 
 *                  management of multiple advertising sets by specifying their unique instance numbers.
 * @param[in]       length   : The length of the advertising data in bytes. It should be within the allowed 
 *                  range and match the length of the provided `data`.
 * @param[in]       data     : Pointer to the raw advertising data. This data is used in the advertising PDUs 
 *                  and should be formatted according to the Bluetooth specification.
 *
  * @return          
 *                  - ESP_OK : Success. The raw advertising data has been successfully configured for the specified instance.
 *                  - ESP_FAIL : Failed. An error occurred while setting the advertising data.
 *
 * @note            Ensure that the `length` parameter matches the size of the `data` array. The Bluetooth stack 
 *                  must be initialized before calling this function, and the `instance` should refer to a valid 
 *                  advertising set. If the data length exceeds the allowed limit or the data format is incorrect, 
 *                  the function may return an error.
 *                  
 *                  After setting the advertising data, observe the following event:
 *                  - `ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT`: Indicates that the advertising data has been successfully set.
 */
esp_err_t esp_ble_gap_config_ext_adv_data_raw(uint8_t instance, uint16_t length, const uint8_t *data);

/**
 * @brief           This function is used to configure the raw scan response data for a specific advertising 
 *                  instance. The scan response data is used in scanning response Protocol Data Units (PDUs) 
 *                  to provide additional information to devices that are scanning for available advertising 
 *                  devices.
 *                  
 *                  By setting the scan response data, you can include extra details in the response to scanning 
 *                  requests, which may help in distinguishing your device or providing more information to 
 *                  potential connection requests.
 *
 * @param[in]       instance : Identifies the advertising set whose scan response data is being configured. 
 *                  This allows management of multiple advertising sets by specifying their unique instance numbers.
 * @param[in]       length   : The length of the scan response data in bytes. It should match the size of the 
 *                  provided `scan_rsp_data`.
 * @param[in]       scan_rsp_data : Pointer to the raw scan response data. This data is used in the scanning 
 *                  response PDUs and should be formatted according to the Bluetooth specification.
 *
  * @return          
 *                  - ESP_OK : Success. The scan response data has been successfully configured for the specified instance.
 *                  - ESP_FAIL : Failed. An error occurred while setting the scan response data.
 *
 * @note            Ensure that the `length` parameter corresponds to the size of the `scan_rsp_data` array. The 
 *                  Bluetooth stack must be initialized before calling this function, and the `instance` should 
 *                  refer to a valid advertising set. If the data length exceeds the allowed limit or the data 
 *                  format is incorrect, the function may return an error.
 *                  
 *                  After setting the scan response data, observe the following event:
 *                  - `ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT`: Indicates that the scan response data has been successfully set.
 */
esp_err_t esp_ble_gap_config_ext_scan_rsp_data_raw(uint8_t instance, uint16_t length,
                                                                   const uint8_t *scan_rsp_data);

/**
 * @brief           This function requests the Controller to enable one or more advertising sets, which are identified 
 *                  by the `instance` parameter. It allows the Host to start advertising operations with the specified 
 *                  parameters for each advertising set.
 *                  
 *                  The function can be used to manage multiple advertising sets simultaneously, allowing for flexible 
 *                  and dynamic advertising strategies. It is useful in scenarios where different advertising sets may 
 *                  be required for various purposes, such as different advertising intervals, data types, or 
 *                  channels.
 *
 * @param[in]       num_adv : The number of advertising sets to enable. This value specifies how many of the advertising 
 *                  sets identified in the `ext_adv` array should be started.
 * @param[in]       ext_adv : Pointer to an array of `esp_ble_gap_ext_adv_t` structures, each specifying the parameters 
 *                  for an advertising set. The array should have a length equal to `num_adv`, and each element in the 
 *                  array provides the configuration for one advertising set.
 *
  * @return          
 *                  - ESP_OK : Success. The advertising sets have been successfully started with the specified parameters.
 *                  - ESP_FAIL : Failed. An error occurred while starting the advertising sets.
 *
 * @note            Ensure that the `num_adv` parameter does not exceed the maximum number of supported advertising sets. 
 *                  The `ext_adv` array should be properly initialized and the Bluetooth stack must be enabled before 
 *                  calling this function. Each `esp_ble_gap_ext_adv_t` structure should be correctly configured with the 
 *                  desired advertising parameters.
 *                  
 *                  After starting the advertising sets, relevant events to monitor include:
 *                  - `ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT`: Triggered when the extension ad launch completes, notifying the app of the result of the ad launch.
 */
esp_err_t esp_ble_gap_ext_adv_start(uint8_t num_adv, const esp_ble_gap_ext_adv_t *ext_adv);

/**
 * @brief           This function requests the Controller to disable one or more advertising sets. The advertising sets 
 *                  are identified by the `ext_adv_inst` parameter, which provides the instance identifiers of the 
 *                  advertising sets to be stopped.
 *                  
 *                  The function allows the Host to stop advertising operations for specific advertising sets that 
 *                  were previously enabled. This is useful for managing and controlling advertising activity, such as 
 *                  when transitioning between different advertising strategies or when stopping advertising in response 
 *                  to specific conditions.
 *
 * @param[in]       num_adv       : The number of advertising sets to disable. This value specifies how many of the 
 *                                  advertising instances identified in the `ext_adv_inst` array should be stopped.
 * @param[in]       ext_adv_inst  : Pointer to an array of `uint8_t` values representing the instance identifiers of 
 *                                  the advertising sets to be stopped. The array should have a length equal to `num_adv`.
 *
  * @return          
 *                  - ESP_OK       : Success. The advertising sets have been successfully stopped as specified.
 *                  - ESP_FAIL       : failed. An error occurred while stopping the advertising sets.
 *
 * @note            Ensure that the `num_adv` parameter does not exceed the number of active advertising sets. The 
 *                  `ext_adv_inst` array should be properly initialized with valid instance identifiers, and the Bluetooth 
 *                  stack must be enabled before calling this function.
 *                  
 *                  After stopping the advertising sets, relevant events to monitor include:
 *                  - `ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT`: Triggered when the extended ad stop completes, notifying the app of the result of the ad stop.
 */
esp_err_t esp_ble_gap_ext_adv_stop(uint8_t num_adv, const uint8_t *ext_adv_inst);

/**
 * @brief           This function is used to remove an advertising set from the Controller. It effectively 
 *                  deletes the specified advertising set, freeing up resources associated with it and 
 *                  stopping any ongoing advertising operations related to that set.
 *                  
 *                  This function is useful when you want to clean up advertising sets that are no longer needed 
 *                  or to manage advertising resources dynamically. Removing an advertising set will stop its 
 *                  advertising activity and remove its configuration from the Controller.
 *
 * @param[in]       instance : The identifier of the advertising set to be removed. This instance should 
 *                              correspond to a valid advertising set that was previously created and may be active.
 *
  * @return          
 *                  - ESP_OK : Success. The advertising set has been successfully removed.
 *                  - ESP_FAIL : Failed. An error occurred while removing the advertising set.
 *
 * @note            Before calling this function, ensure that the advertising set is no longer in use or is 
 *                  stopped using `esp_ble_gap_ext_adv_stop`. The Bluetooth stack must be properly initialized 
 *                  and the instance identifier should be valid. Removing an advertising set may affect any 
 *                  ongoing advertising operations or configurations.
 *                  
 *                  Relevant events to monitor after removing an advertising set include:
 *                  - `ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT`: Indicates that the specified advertising set has 
 *                  been successfully removed from the Controller.
 */
esp_err_t esp_ble_gap_ext_adv_set_remove(uint8_t instance);

/**
 * @brief           This function is used to remove all existing advertising sets from the Controller. It clears 
 *                  all advertising configurations and stops any ongoing advertising activities related to those 
 *                  sets, freeing up resources associated with them.
 *                  
 *                  This function is useful for resetting the advertising state or cleaning up all advertising 
 *                  configurations at once. After calling this function, no advertising sets will be active, and 
 *                  you will need to reconfigure and restart any advertising sets if needed.
 *
  * @return          
 *                  - ESP_OK : Success. All advertising sets have been successfully removed and resources freed.
 *                  - ESP_FAIL : Failed. An error occurred while removing the advertising sets.
 *
 * @note            Before calling this function, ensure that no critical advertising operations are ongoing 
 *                  or that they can be safely stopped. The Bluetooth stack must be properly initialized. 
 *                  Removing all advertising sets will affect any active advertising activities and may require 
 *                  reconfiguration of advertising parameters if needed.
 *                  
 *                  Relevant events to monitor after clearing advertising sets include:
 *                  - `ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT`: Indicates that all advertising sets have been 
 *                  successfully cleared from the Controller.
 */
esp_err_t esp_ble_gap_ext_adv_set_clear(void);

/**
 * @brief           This function is used by the Host to set the parameters for periodic advertising.
 *                  Periodic advertising allows the device to periodically transmit data at a fixed interval, 
 *                  which can be used for broadcasting data in a continuous manner to be received by scanners 
 *                  or other devices. This function configures the parameters that control the periodic 
 *                  advertising behavior for a specified advertising instance.
 *
 * @param[in]       instance : The identifier for the advertising set whose periodic advertising parameters 
 *                  are being configured. This allows distinguishing between multiple advertising sets 
 *                  that may be active simultaneously.
 * @param[in]       params   : A pointer to a structure of type `esp_ble_gap_periodic_adv_params_t` that 
 *                  contains the periodic advertising parameters to be set. This structure includes settings 
 *                  such as the periodic advertising interval and other relevant configurations.
 *
  * @return          
 *                  - ESP_OK : Success. The periodic advertising parameters have been successfully set for the 
 *                    specified advertising instance.
 *                  - ESP_FAIL : Failed. An error occurred while setting the periodic advertising parameters. 
 *                    Possible error codes may indicate issues such as invalid parameters, uninitialized state, 
 *                    or other failures.
 *
 * @note            Ensure that the Bluetooth stack is properly initialized before calling this function. The 
 *                  specified `instance` must be valid and correctly correspond to an existing advertising set. 
 *                  Misconfiguration or invalid parameters may lead to failure. After setting the parameters, 
 *                  you may need to start periodic advertising using the appropriate function to activate the 
 *                  periodic advertising with the configured settings.
 * 
 *                  This function may trigger the following event:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT`: Triggered when the periodic advertisement parameter setting is complete,
 *                  notifies the application of the result of setting the periodic advertisement parameter.
 */
esp_err_t esp_ble_gap_periodic_adv_set_params(uint8_t instance, const esp_ble_gap_periodic_adv_params_t *params);

#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)

/**
 * @brief           This function is used to set the data used in periodic advertising Protocol Data Units (PDUs).
 *                  Periodic advertising allows a device to transmit periodic data at regular intervals, 
 *                  and this function configures the data that will be used for periodic advertising for a 
 *                  specified advertising instance.
 *
 * @param[in]       instance : The identifier for the advertising set whose periodic advertising data 
 *                  is being configured. This allows distinguishing between multiple advertising sets 
 *                  that may be active simultaneously.
 * @param[in]       length   : The length of the periodic advertising data. This specifies how many bytes 
 *                  of data are being provided in the `data` parameter. The length must be within the limits 
 *                  specified by the Bluetooth specification and the device's capabilities.
 * @param[in]       data     : A pointer to the periodic advertising data. This is the data that will be 
 *                  included in the periodic advertising PDUs. The data should be formatted according to the 
 *                  Bluetooth specification and the application's requirements.
 * @param[in]       only_update_did : A boolean flag indicating whether to update only the Advertising DID (Device 
 *                  Identifier) of the periodic advertising. If set to `true`, only the Advertising DID will be updated, 
 *                  and the `length` and `data` parameters will be ignored. If set to `false`, the provided `length` 
 *                  and `data` parameters will be used to configure the periodic advertising data.
 *
  * @return          
 *                  - ESP_OK : Success. The periodic advertising data has been successfully set for the 
 *                    specified advertising instance.
 *                  - ESP_FAIL : Failed. An error occurred while setting the periodic advertising data. 
 *                    Possible error codes may indicate issues such as invalid parameters, uninitialized state, 
 *                    or other failures.
 *
 * @note            Ensure that the Bluetooth stack is properly initialized before calling this function. 
 *                  The specified `instance` must be valid and correctly correspond to an existing advertising 
 *                  set. If `only_update_did` is set to `true`, the `length` and `data` parameters will be ignored, 
 *                  and only the Advertising DID will be updated. Ensure that the periodic advertising data complies 
 *                  with the Bluetooth specifications and the device's capabilities.
 *                  
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT`: Indicates that the periodic advertising 
 *                  data has been successfully set for the specified advertising instance.
 */
esp_err_t esp_ble_gap_config_periodic_adv_data_raw(uint8_t instance, uint16_t length,
                                                   const uint8_t *data, bool only_update_did);

#else
/**
 * @brief           This function is used to set the data used in periodic advertising Protocol Data Units (PDUs).
 *                  Periodic advertising allows devices to transmit data at regular intervals, and this function 
 *                  configures the data that will be used for periodic advertising for a specified advertising instance.
 *
 * @param[in]       instance : Identifies the advertising set whose periodic advertising data is being configured.
 *                  This parameter distinguishes between multiple advertising sets that may be active simultaneously.
 * @param[in]       length   : The length of the periodic advertising data. This specifies the number of bytes 
 *                  in the `data` parameter. Ensure that the length does not exceed the maximum allowed by the Bluetooth specification.
 * @param[in]       data     : A pointer to the periodic advertising data. This data will be included in the periodic advertising PDUs.
 *                  The format and content of this data should adhere to the Bluetooth specification and application requirements.
 *
  * @return          
 *                  - ESP_OK : Success. The periodic advertising data has been successfully set for the specified advertising instance.
 *                  - ESP_FAIL : Failed. An error occurred while setting the periodic advertising data. 
 *                    Possible error codes may indicate issues such as invalid parameters or an uninitialized state.
 *
 * @note            Ensure that the Bluetooth stack is properly initialized before calling this function. The `instance` must 
 *                  be valid and correspond to an existing advertising set. Verify that the `length` and `data` parameters are 
 *                  correctly specified and within allowed limits. Proper handling of errors will help in managing unsuccessful 
 *                  attempts to set the periodic advertising data.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT`: Indicates that the periodic advertising data has been 
 *                  successfully set for the specified advertising instance.
 */
esp_err_t esp_ble_gap_config_periodic_adv_data_raw(uint8_t instance, uint16_t length, const uint8_t *data);
#endif

#if (CONFIG_BT_BLE_FEAT_PERIODIC_ADV_ENH)

/**
 * @brief           This function is used to request the Controller to enable periodic advertising for the specified advertising set.
 *                  Periodic advertising allows a device to periodically send advertising packets, which can be useful for
 *                  applications that need to broadcast information at regular intervals.
 *
 * @param[in]       instance  : Identifies the advertising set for which periodic advertising is to be enabled.
 *                  This parameter is used to distinguish between multiple advertising sets that may be active simultaneously.
 * @param[in]       include_adi: If true, the ADI (Advertising Data Info) field will be included in AUX_SYNC_IND PDUs.
 *                  The ADI is used to provide additional information about the advertising data. When included, it can help
 *                  with synchronization and identification of periodic advertising packets.
 *
  * @return          
 *                  - ESP_OK : Success. Periodic advertising has been successfully enabled for the specified advertising instance.
 *                  - ESP_FAIL : Failed. An error occurred while enabling periodic advertising. Possible error codes may indicate 
 *                    issues such as invalid parameters or an uninitialized state.
 *
 * @note            Ensure that the Bluetooth stack is properly initialized before calling this function. The `instance` must
 *                  be valid and correspond to an existing advertising set. Verify that the `include_adi` parameter is set according
 *                  to your application requirements. Proper error handling will help manage unsuccessful attempts to start periodic
 *                  advertising.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT`: Indicates that the request to start periodic advertising has
 *                  been processed. This event will provide details about the success or failure of the operation.
 */
esp_err_t esp_ble_gap_periodic_adv_start(uint8_t instance, bool include_adi);

#else
/**
 * @brief           This function is used to request the Controller to enable periodic advertising for the specified advertising set.
 *                  Periodic advertising allows a device to periodically send advertising packets, which can be useful for
 *                  applications that need to broadcast information at regular intervals.
 *
 * @param[in]       instance : Identifies the advertising set for which periodic advertising is to be enabled.
 *                  This parameter is used to distinguish between multiple advertising sets that may be active simultaneously.
 *
  * @return          
 *                  - ESP_OK : Success. Periodic advertising has been successfully enabled for the specified advertising instance.
 *                  - ESP_FAIL : Failed. An error occurred while enabling periodic advertising. Possible error codes may indicate 
 *                    issues such as invalid parameters or an uninitialized state.
 *
 * @note            Ensure that the Bluetooth stack is properly initialized before calling this function. The `instance` must
 *                  be valid and correspond to an existing advertising set. Proper error handling will help manage unsuccessful attempts
 *                  to start periodic advertising.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT`: Indicates that the request to start periodic advertising has
 *                  been processed. This event will provide details about the success or failure of the operation.
 */
esp_err_t esp_ble_gap_periodic_adv_start(uint8_t instance);
#endif

/**
 * @brief           This function is used to request the Controller to disable periodic advertising for the specified advertising set.
 *                  Disabling periodic advertising stops the periodic transmission of advertising packets for the given set,
 *                  which can be useful for conserving resources or when periodic advertising is no longer needed.
 *
 * @param[in]       instance : Identifies the advertising set for which periodic advertising is to be disabled.
 *                  This parameter is used to distinguish between multiple advertising sets that may be active simultaneously.
 *
  * @return          
 *                  - ESP_OK : Success. Periodic advertising has been successfully disabled for the specified advertising instance.
 *                  - ESP_FAIL : Failed. An error occurred while disabling periodic advertising. Possible error codes may indicate 
 *                  issues such as invalid parameters or an uninitialized state.
 *
 * @note            Ensure that the Bluetooth stack is properly initialized before calling this function. The `instance` must
 *                  be valid and correspond to an existing advertising set. Proper error handling will help manage unsuccessful attempts
 *                  to stop periodic advertising.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT`: Indicates that the request to stop periodic advertising has
 *                  been processed. This event will provide details about the success or failure of the operation.
 */
esp_err_t esp_ble_gap_periodic_adv_stop(uint8_t instance);

/**
 * @brief           This function is used to set the extended scan parameters that define how the device will scan for advertising packets on the Bluetooth Low Energy (BLE) channels.
 *                  Extended scanning parameters allow customization of the scanning behavior, including the scan interval, window, and type, which can impact the discovery of advertising devices and overall scan efficiency.
 *
 * @param[in]       params : Pointer to a structure containing the extended scan parameters. This structure includes details such as scan interval, scan window, scan type, and other relevant parameters.
 *                  The `esp_ble_ext_scan_params_t` structure should be properly initialized before passing it to this function.
 *
  * @return          
 *                  - ESP_OK : Success. The extended scan parameters have been successfully set.
 *                  - ESP_FAIL : Failed. An error occurred while setting the scan parameters. Possible error codes may include invalid parameters or an uninitialized Bluetooth stack.
 *
 * @note            Ensure that the Bluetooth stack is initialized and that the `params` structure is correctly populated with valid values. Setting the scan parameters will affect the scanning behavior of the device.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT`: Triggered when the Set Extended Scan parameter is complete, notifies the application of the results of setting the scan parameter.
 */
esp_err_t esp_ble_gap_set_ext_scan_params(const esp_ble_ext_scan_params_t *params);

/**
 * @brief           This function is used to enable extended scanning for Bluetooth Low Energy (BLE) devices.
 *                  Extended scanning allows the device to search for advertising packets over a defined period and interval.
 *                  The duration specifies how long the device will actively scan, while the period defines the time between successive scans.
 *
 * @param[in]       duration : Scan duration time, where Time = N * 10 ms. 
 *                  The value must be in the range 0x0001 to 0xFFFF. A value of 0 indicates a continuous scan until explicitly stopped.
 * @param[in]       period   : Time interval from when the Controller started its last Scan Duration until it begins the subsequent Scan Duration.
 *                  Time = N * 1.28 sec. The value must be in the range 0x0001 to 0xFFFF.
 *                  A value of 0 indicates no periodic scanning.
 *
  * @return          
 *                  - ESP_OK : Success. The extended scanning process has been successfully started.
 *                  - ESP_FAIL : Failed. An error occurred while starting the scan. Possible issues could include invalid parameters or an uninitialized Bluetooth stack.
 *
 * @note            Ensure that the Bluetooth stack is initialized before calling this function. The scan duration and period parameters must be within the valid ranges.
 *                  
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT`: Indicates that the extended scan has started successfully or failed. This event provides details about the outcome of the scan initiation.
 */
esp_err_t esp_ble_gap_start_ext_scan(uint32_t duration, uint16_t period);

/**
 * @brief           This function is used to disable extended scanning.
 *                  When called, it stops the ongoing extended scan process that was previously initiated with `esp_ble_gap_start_ext_scan`.
 *                  
  * @return          
 *                  - ESP_OK : Success. The extended scanning process has been successfully stopped.
 *                  - ESP_FAIL : Failed. An error occurred while stopping the scan. Possible issues could include an uninitialized Bluetooth stack or an attempt to stop scanning when it is not active.
 *
 * @note            Ensure that the Bluetooth stack is initialized before calling this function. It is important to handle the completion of scanning operations to ensure resources are properly freed.
 *                  
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT`: Indicates that the extended scan has been stopped successfully or failed. This event provides details about the outcome of the scan termination.
 */
esp_err_t esp_ble_gap_stop_ext_scan(void);

/**
 * @brief           This function is used to synchronize with periodic advertising from an advertiser and begin receiving periodic advertising packets.
 *
 * @param[in]       params : Synchronization parameters used to define how the synchronization should occur.
 *                  These parameters include details such as the advertiser's address, the synchronization interval, and other related settings.
 *
  * @return          
 *                  - ESP_OK : Success. The synchronization with the periodic advertising has been initiated.
 *                  - ESP_FAIL : Failed. An error occurred while trying to synchronize with the periodic advertising. Possible issues could include incorrect parameters or an ongoing synchronization process.
 *
 * @note            Ensure that the parameters provided are correctly configured to match the advertiser's periodic advertising data.
 * 
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT`: Triggered when the creation of a periodic advertisement synchronization is complete,
 *                  notifying the application of the results of the creation of the synchronization.
 */
esp_err_t esp_ble_gap_periodic_adv_create_sync(const esp_ble_gap_periodic_adv_sync_params_t *params);

/**
 * @brief           This function is used to cancel the LE_Periodic_Advertising_Create_Sync command while it is pending.
 *
  * @return          
 *                  - ESP_OK : Success. The cancellation request was successfully processed, and any pending synchronization attempt will be aborted.
 *                  - ESP_FAIL : Failed. An error occurred while attempting to cancel the synchronization request. This could occur if no synchronization request is pending or if there is an issue with the Bluetooth stack.
 *
 * @note            This function should be used if you need to abort an ongoing synchronization request with periodic advertising before it completes.
 *                  
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT`: Indicates that the synchronization cancel request has been processed, and provides details about the cancellation status.
 */
esp_err_t esp_ble_gap_periodic_adv_sync_cancel(void);

/**
 * @brief           This function is used to stop reception of the periodic advertising identified by the Synchronization Handle parameter.
 *
 * @param[in]       sync_handle : A handle that identifies the periodic advertising synchronization to be terminated. This handle is obtained from the `esp_ble_gap_periodic_adv_create_sync` function when the synchronization was initially created.
 *
  * @return          
 *                  - ESP_OK : Success. The request to terminate the synchronization was successfully processed, and periodic advertising reception has been stopped.
 *                  - ESP_FAIL : Failed. An error occurred while attempting to terminate the synchronization. This could occur if the `sync_handle` is invalid or if there is an issue with the Bluetooth stack.
 *
 * @note            This function should be used if you need to stop receiving periodic advertising packets from a specific advertiser. It will terminate the synchronization process for the given handle.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT`: Triggered when the termination periodic advertisement synchronization completes, notifying the application of the result of the termination.
 */
esp_err_t esp_ble_gap_periodic_adv_sync_terminate(uint16_t sync_handle);

/**
 * @brief           This function is used to add a single device to the Periodic Advertiser list stored in the Controller.
 *
 * @param[in]       addr_type : Address type of the device to be added to the list. This specifies whether the address is public, random, etc.
 * @param[in]       addr : Device Address of the device to be added. This should be a valid 6-byte Bluetooth Device Address (BD_ADDR).
 * @param[in]       sid : Advertising Service Identifier (SID) used to identify the Periodic Advertising. This is a subfield in the Advertising Data Indicator field.
 *
  * @return          
 *                  - ESP_OK : Success. The device has been successfully added to the Periodic Advertiser list.
 *                  - ESP_FAIL : Failed. An error occurred while attempting to add the device to the list. This could be due to an invalid address or SID, or if the Bluetooth stack encountered an issue.
 *
 * @note            This function allows you to manage the Periodic Advertiser list by adding specific devices that you want to synchronize with. 
 *                  Adding a device to this list helps in filtering and processing periodic advertising data from that particular device.
 * 
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT`: Triggered when adding a device to the periodic ad list is complete, notifies the app of the results of adding the device.
 */
esp_err_t esp_ble_gap_periodic_adv_add_dev_to_list(esp_ble_addr_type_t addr_type,
                                                   esp_bd_addr_t addr,
                                                   uint8_t sid);

/**
 * @brief           This function is used to remove one device from the list of Periodic Advertisers stored in the Controller.
 *                  Removals from the Periodic Advertisers List take effect immediately.
 *
 * @param[in]       addr_type : Address type of the device to be removed from the list. This specifies whether the address is public, random, etc.
 * @param[in]       addr : Device Address of the device to be removed. This should be a valid 6-byte Bluetooth Device Address (BD_ADDR).
 * @param[in]       sid : Advertising Service Identifier (SID) used to identify the Periodic Advertising. This is a subfield in the Advertising Data Indicator field.
 *
  * @return          
 *                  - ESP_OK : Success. The device has been successfully removed from the Periodic Advertiser list.
 *                  - ESP_FAIL : Failed. An error occurred while attempting to remove the device from the list. This could be due to an invalid address or SID, or if the Bluetooth stack encountered an issue.
 *
 * @note            This function allows you to manage the Periodic Advertiser list by removing specific devices that you no longer wish to synchronize with. 
 *                  Removing a device from this list helps in filtering out periodic advertising data from that particular device.
 * 
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT`: Triggered when the removal of a device from the periodic advertisement list is complete,
 *                  notifying the application of the result of the removal.
 */
esp_err_t esp_ble_gap_periodic_adv_remove_dev_from_list(esp_ble_addr_type_t addr_type,
                                                        esp_bd_addr_t addr,
                                                        uint8_t sid);

/**
 * @brief           This function is used to remove all devices from the list of Periodic Advertisers in the Controller.
 *
  * @return          
 *                  - ESP_OK : Success. All devices have been successfully removed from the Periodic Advertiser list.
 *                  - ESP_FAIL : Failed. An error occurred while attempting to clear the list. This could be due to issues with the Bluetooth stack or if no devices were in the list.
 *
 * @note            This function clears the entire list of Periodic Advertisers, which means that no devices will be synchronized with periodic advertising data until new devices are added.
 *                  This operation is useful when you need to reset the list or when you want to stop synchronization with all previously added periodic advertisers.
 * 
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT`: Triggered when the clearing of a periodic advertising device is complete, notifying the application of the result of the clearing.
 */
esp_err_t esp_ble_gap_periodic_adv_clear_dev(void);

/**
 * @brief           This function is used to set Auxiliary connection parameters for extended advertising and scanning.
 *
 * @param[in]       addr : Device address of the peer device for which the connection parameters are being set.
 * @param[in]       phy_mask : Indicates the Physical Layer(s) on which the advertising packets should be received and the Physical Layers for which connection parameters have been specified.
 * @param[in]       phy_1m_conn_params : Connection parameters for scanning and connecting on the low energy 1M Physical Layer.
 * @param[in]       phy_2m_conn_params : Connection parameters for scanning and connecting on the low energy 2M Physical Layer.
 * @param[in]       phy_coded_conn_params : Connection parameters for scanning and connecting on the LE Coded Physical Layer.
 *
  * @return          
 *                  - ESP_OK : Success. The connection parameters have been successfully set.
 *                  - ESP_FAIL : Failed. An error occurred while setting the connection parameters. This could be due to invalid parameters or issues with the Bluetooth stack.
 *
 * @note            This function allows you to specify different connection parameters for various Physical Layers (1M, 2M, and Coded) in extended advertising and scanning scenarios.
 *                  The parameters provided will be used to establish a connection with the specified device, taking into account the preferred Physical Layers and their respective connection parameters.
 */
esp_err_t esp_ble_gap_prefer_ext_connect_params_set(esp_bd_addr_t addr,
                                                    esp_ble_gap_phy_mask_t phy_mask,
                                                    const esp_ble_gap_conn_params_t *phy_1m_conn_params,
                                                    const esp_ble_gap_conn_params_t *phy_2m_conn_params,
                                                    const esp_ble_gap_conn_params_t *phy_coded_conn_params);

#endif// #if (BLE_50_FEATURE_SUPPORT == TRUE)

/* if Bluetooth Low Energy Feature Periodic Advertising Synchronization Transfer Support  */ 
#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

/**
 * @brief           This function is used to enable or disable the reception of periodic advertising data.
 *
 * @param[in]       sync_handle : Handle of the periodic advertising synchronization. This handle is used to identify the specific periodic advertising sync instance.
 * @param[in]       enable : Determines whether periodic advertising data reporting and duplicate filtering should be enabled or disabled.
 *                           - `1` to enable periodic advertising data reception.
 *                           - `0` to disable periodic advertising data reception.
 *
  * @return          
 *                  - ESP_OK : Success. The periodic advertising receive settings have been successfully applied.
 *                  - ESP_FAIL : Failed. An error occurred while setting the periodic advertising receive parameters. This could be due to invalid handle or other issues with the Bluetooth stack.
 *
 * @note            Enabling periodic advertising data reception allows the device to start reporting and filtering received periodic advertising data based on the specified synchronization handle.
 *                  Disabling reception stops reporting and filtering of periodic advertising data. Make sure the synchronization handle provided is valid and corresponds to an active periodic advertising sync instance.
 *                  
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_RECV_ENABLE_COMPLETE_EVT`: This event indicates the result of the operation to enable or disable the reception of periodic advertising data.
 *                                                                         It provides feedback on whether the action was successful and any errors encountered.
 */
esp_err_t esp_ble_gap_periodic_adv_recv_enable(uint16_t sync_handle, uint8_t enable);

/**
 * @brief           This function is used to transfer synchronization data for periodic advertising.
 *
 * @param[in]       addr : The Bluetooth Device Address of the peer device. This address identifies the device with which periodic advertising synchronization data is being transferred.
 * @param[in]       service_data : Service data associated with the periodic advertising synchronization. This data is used by the Host for various purposes related to the service.
 * @param[in]       sync_handle : Handle of the periodic advertising synchronization instance. This handle identifies the specific sync instance for which the data is being transferred.
 *
  * @return          
 *                  - ESP_OK : Success. The synchronization data has been successfully transferred.
 *                  - ESP_FAIL : Failed. An error occurred during the transfer process. This could be due to invalid handle, address issues, or other Bluetooth stack errors.
 *
 * @note            Ensure that the synchronization handle is valid and corresponds to an active periodic advertising sync instance. The peer device address should be correct and match the intended device for synchronization.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_SYNC_TRANS_COMPLETE_EVT`: Triggered when the periodic advertisement synchronization transfer is complete, notifying the application of the transfer result.
 */
esp_err_t esp_ble_gap_periodic_adv_sync_trans(esp_bd_addr_t addr,
                                              uint16_t service_data, uint16_t sync_handle);

/**
 * @brief           This function is used to transfer periodic advertising set information.
 *
 * @param[in]       addr : The Bluetooth Device Address of the peer device. This address identifies the device with which periodic advertising set information is being transferred.
 * @param[in]       service_data : Service data associated with the periodic advertising set. This data is used by the Host for various purposes related to the advertising set.
 * @param[in]       adv_handle : Handle of the advertising set. This handle identifies the specific advertising set for which the information is being transferred.
 *
  * @return          
 *                  - ESP_OK : Success. The advertising set information has been successfully transferred.
 *                  - ESP_FAIL : Failed. An error occurred during the transfer process. This could be due to invalid handle, address issues, or other Bluetooth stack errors.
 *
 * @note            Ensure that the advertising handle is valid and corresponds to an active advertising set. The peer device address should be correct and match the intended device for the transfer.
 * 
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_PERIODIC_ADV_SET_INFO_TRANS_COMPLETE_EVT`: Triggered when the periodic advertisement settings information transfer is complete,
 *                  notifies the application of the results of the settings information transfer.
 */
esp_err_t esp_ble_gap_periodic_adv_set_info_trans(esp_bd_addr_t addr,
                                                  uint16_t service_data, uint8_t adv_handle);

/**
 * @brief           This function is used to set the parameters for transferring periodic advertising synchronization.
 *
 * @param[in]       addr : The Bluetooth Device Address of the peer device with which synchronization transfer is to be performed. This address identifies the target device for the synchronization.
 * @param[in]       params : Parameters for periodic advertising synchronization transfer. This structure should contain the specific configuration needed for synchronization.
 *
  * @return          
 *                  - ESP_OK : Success. The parameters for periodic advertising synchronization transfer have been successfully set.
 *                  - ESP_FAIL : Failed. An error occurred while setting the parameters. Possible reasons could include invalid address or parameter issues.
 *
 * @note            Ensure that the address is correct and that the parameters structure contains valid settings for synchronization transfer. This function is typically used in scenarios where the Host needs to configure or modify the synchronization settings for periodic advertising.
 */
esp_err_t esp_ble_gap_set_periodic_adv_sync_trans_params(esp_bd_addr_t addr,
                                                         const esp_ble_gap_past_params_t *params);

#endif//#if (BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)

// if Bluetooth Low Energy 4.2 Feature Support
#if (BLE_42_FEATURE_SUPPORT == TRUE)

/**
 * @brief           This function starts a test where the Device Under Test (DUT) generates reference packets at a fixed interval.
 *
 * @param[in]       tx_params : Parameters for Direct Test Mode (DTM) transmission. This structure should contain the configuration details for the transmitter, such as packet type, transmission frequency, and interval.
 *
  * @return          
 *                  - ESP_OK : Success. The Direct Test Mode transmitter has been started with the specified parameters.
 *                  - ESP_FAIL : Failed. An error occurred while starting the transmitter. Possible reasons include invalid parameters or hardware issues.
 *
 * @note            Ensure that the `tx_params` structure is correctly initialized with valid settings before calling this function. This function is typically used for testing and debugging purposes in BLE (Bluetooth Low Energy) scenarios.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_DTM_TEST_UPDATE_EVT`: This event provides updates related to the DTM test state. A notable subevent is:
 *                    - `DTM_TX_START_EVT`: This subevent indicates that the DTM transmitter has successfully started and is now generating reference packets. It is useful for confirming the initiation of the transmission test and for monitoring the ongoing transmission status.
 */
esp_err_t esp_ble_dtm_tx_start(const esp_ble_dtm_tx_t *tx_params);

/**
 * @brief           This function starts a test where the Device Under Test (DUT) receives reference packets at a fixed interval.
 *
 * @param[in]       rx_params : Parameters for Direct Test Mode (DTM) reception. This structure should contain the configuration details for the receiver, such as packet type, reception frequency, and interval.
 *
  * @return          
 *                  - ESP_OK : Success. The Direct Test Mode receiver has been started with the specified parameters.
 *                  - ESP_FAIL : Failed. An error occurred while starting the receiver. Possible reasons include invalid parameters or hardware issues.
 *
 * @note            Ensure that the `rx_params` structure is correctly initialized with valid settings before calling this function. This function is typically used for testing and debugging purposes in BLE (Bluetooth Low Energy) scenarios.
 * 
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_DTM_TEST_UPDATE_EVT`: This event provides updates related to the DTM test state. A notable subevent is:
 *                    - `DTM_RX_START_EVT`: This subevent indicates that the DTM receiver has successfully started and is now receiving reference packets. It is useful for confirming the initiation of the test and for monitoring the ongoing reception status.
 */
esp_err_t esp_ble_dtm_rx_start(const esp_ble_dtm_rx_t *rx_params);
#endif//#if (BLE_42_FEATURE_SUPPORT == TRUE)

/* if Bluetooth Low Energy 5.0 Feature Support */
#if (BLE_50_FEATURE_SUPPORT == TRUE)

/**
 * @brief           This function is used to start a test where the Device Under Test (DUT) generates reference packets
 *                  at a fixed interval, using enhanced Direct Test Mode (DTM) transmitter parameters.
 *
 * @param[in]       tx_params : Parameters for Direct Test Mode (DTM) enhanced transmitter. This structure should include the configuration details for the transmitter, such as packet type, transmission frequency, interval, and other relevant settings.
 *
  * @return          
 *                  - ESP_OK : Success. The Direct Test Mode transmitter has been started with the specified parameters.
 *                  - ESP_FAIL : Failed. An error occurred while starting the transmitter. Possible issues include invalid parameters or hardware malfunctions.
 *
 * @note            Ensure that the `tx_params` structure is properly initialized with valid settings before calling this function. This function is typically used for enhanced testing and debugging of BLE (Bluetooth Low Energy) functionalities.
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_DTM_TEST_UPDATE_EVT`:  A notable subevent is:-`DTM_TX_START_EVT`
 */
esp_err_t esp_ble_dtm_enh_tx_start(const esp_ble_dtm_enh_tx_t *tx_params);

/**
 * @brief           This function is used to start a test where the Device Under Test (DUT) receives test reference packets
 *                  at a fixed interval, using enhanced Direct Test Mode (DTM) receiver parameters.
 *
 * @param[in]       rx_params : Parameters for Direct Test Mode (DTM) enhanced receiver. This structure should include the configuration details for the receiver, such as packet type, reception frequency, and other relevant settings.
 *
  * @return          
 *                  - ESP_OK : Success. The Direct Test Mode receiver has been started with the specified parameters.
 *                  - ESP_FAIL : Failed. An error occurred while starting the receiver. Possible issues include invalid parameters or hardware malfunctions.
 *
 * @note            Ensure that the `rx_params` structure is properly initialized with valid settings before calling this function. This function is typically used for enhanced testing and debugging of BLE (Bluetooth Low Energy) functionalities.
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_DTM_TEST_UPDATE_EVT` A notable subevent is:-`DTM_RX_START_EVT`.
 */
esp_err_t esp_ble_dtm_enh_rx_start(const esp_ble_dtm_enh_rx_t *rx_params);
#endif //#if (BLE_50_FEATURE_SUPPORT == TRUE)

/**
 * @brief           This function is used to stop any Direct Test Mode (DTM) test that is currently in progress.
 *
  * @return          
 *                  - ESP_OK : Success. The Direct Test Mode test has been successfully stopped.
 *                  - ESP_FAIL : Failed. An error occurred while stopping the test. Possible reasons include no test in progress or hardware issues.
 *
 * @note            This function can be called to halt any ongoing DTM test, whether it is transmitting or receiving. It is useful for stopping tests that are no longer needed or in cases where you need to reinitialize the test parameters.
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_DTM_TEST_UPDATE_EVT` A notable subevent is:-`DTM_TEST_STOP_EVT`.
 */
esp_err_t esp_ble_dtm_stop(void);

/**
 * @brief           This function is used to clear any legacy advertising configurations.
 *
  * @return          
 *                  - ESP_OK : Success. The legacy advertising configurations have been successfully cleared.
 *                  - ESP_FAIL : Failed. An error occurred while clearing the advertising configurations. Possible reasons include no advertising configurations to clear or issues with the Bluetooth stack.
 *
 * @note            This function is useful for resetting or removing legacy advertising settings that were previously configured. It can help in managing or updating advertising configurations as needed.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_ADV_CLEAR_COMPLETE_EVT`: Indicates that the advertising data has been cleared. This event can confirm whether the advertising data has been successfully cleared.
 */
esp_err_t esp_ble_gap_clear_advertising(void);

/**
 * @brief           This function is called to send a vendor-specific Host Controller Interface (HCI) command.
 *
 * @param[in]       vendor_cmd_param : Parameters for the vendor-specific HCI command. This includes the command opcode, parameters, and any other necessary data specific to the vendor's implementation.
 *
  * @return          
 *                  - ESP_OK : Success. The vendor-specific HCI command was sent successfully.
 *                  - ESP_FAIL : Failed. The command could not be sent. Possible reasons include invalid parameters or issues with the Bluetooth stack.
 *
 * @note            Vendor-specific HCI commands are used to access features or configurations that are not covered by standard Bluetooth commands. The exact behavior and parameters depend on the vendor's implementation and the specific command being sent.
 *
 *                  Ensure that the `vendor_cmd_param` structure is correctly populated with valid data before calling this function. Refer to the vendor's documentation for details on supported commands and their parameters.
 *
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_VENDOR_CMD_COMPLETE_EVT`: This event indicates that the execution of the vendor-specific HCI command has completed. It is useful for confirming the completion of the command and for retrieving any results or status information provided by the command.
 *                                                           The specific details and outcomes will depend on the vendor's implementation and the command sent.
 */
esp_err_t esp_ble_gap_vendor_command_send(esp_ble_vendor_cmd_params_t *vendor_cmd_param);

/**
 * @brief           This function sets the privacy mode of the device in the resolving list.
 *
 * @note            This feature is not supported on ESP32.
 *
 * @param[in]       addr_type: The address type of the peer identity address. It can be either `BLE_ADDR_TYPE_PUBLIC` or `BLE_ADDR_TYPE_RANDOM`.
 * @param[in]       addr: The peer identity address of the device. This should be a 6-byte Bluetooth Device Address (BD_ADDR).
 * @param[in]       mode: The privacy mode to be set for the device. This is specified by the `esp_ble_privacy_mode_t` enumeration.
 *
  * @return          
 *                  - ESP_OK : Success. The privacy mode was successfully set.
 *                  - ESP_FAIL : Failed. The privacy mode could not be set. This could be due to an unsupported feature or an invalid parameter.
 *
 * @note            As indicated, this feature is not supported on ESP32, and calling this function on ESP32 may not have any effect.
 * 
 *                  Relevant events to monitor include:
 *                  - `ESP_GAP_BLE_SET_PRIVACY_MODE_COMPLETE_EVT`: Triggered when setting privacy mode is complete, notifies the application of the result of setting privacy mode.
 */
esp_err_t esp_ble_gap_set_privacy_mode(esp_ble_addr_type_t addr_type, esp_bd_addr_t addr, esp_ble_privacy_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_GAP_BLE_API_H__ */
