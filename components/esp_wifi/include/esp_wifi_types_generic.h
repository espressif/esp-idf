/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_WIFI_TYPES_H__
#define __ESP_WIFI_TYPES_H__

#include <stdbool.h>
#include <stddef.h>
#include "sdkconfig.h"
#include "esp_event_base.h"
#include "esp_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_AP_DEFAULT_MAX_IDLE_PERIOD  292 /**< Default timeout for SoftAP BSS Max Idle. Unit: 1000TUs >**/

/**
  * @brief Wi-Fi mode type
  */
typedef enum {
    WIFI_MODE_NULL = 0,  /**< Null mode */
    WIFI_MODE_STA,       /**< Wi-Fi station mode */
    WIFI_MODE_AP,        /**< Wi-Fi soft-AP mode */
    WIFI_MODE_APSTA,     /**< Wi-Fi station + soft-AP mode */
    WIFI_MODE_NAN,       /**< Wi-Fi NAN mode */
    WIFI_MODE_MAX
} wifi_mode_t;

/**
  * @brief Wi-Fi interface type
  */
typedef enum {
    WIFI_IF_STA = ESP_IF_WIFI_STA,    /**< Station interface */
    WIFI_IF_AP  = ESP_IF_WIFI_AP,     /**< Soft-AP interface */
#if CONFIG_SOC_WIFI_NAN_SUPPORT || !CONFIG_SOC_WIFI_ENABLED
    WIFI_IF_NAN = ESP_IF_WIFI_NAN,    /**< NAN interface */
#endif
    WIFI_IF_MAX                       /**< Maximum number of interfaces */
} wifi_interface_t;

typedef enum {
    WIFI_OFFCHAN_TX_CANCEL,   /**< Cancel off-channel transmission */
    WIFI_OFFCHAN_TX_REQ,      /**< Request off-channel transmission */
} wifi_action_tx_t;

typedef enum {
    WIFI_ROC_CANCEL,    /**< Cancel remain on channel */
    WIFI_ROC_REQ,       /**< Request remain on channel */
} wifi_roc_t;
/**
  * @brief Wi-Fi country policy
  */
typedef enum {
    WIFI_COUNTRY_POLICY_AUTO,   /**< Country policy is auto, use the country info of AP to which the station is connected */
    WIFI_COUNTRY_POLICY_MANUAL, /**< Country policy is manual, always use the configured country info */
} wifi_country_policy_t;

/**
  * @brief Structure describing Wi-Fi country-based regional restrictions.
  */
typedef struct {
    char                  cc[3];   /**< Country code string */
    uint8_t               schan;   /**< Start channel of the allowed 2.4GHz Wi-Fi channels */
    uint8_t               nchan;   /**< Total channel number of the allowed 2.4GHz Wi-Fi channels */
    int8_t                max_tx_power;   /**< This field is used for getting Wi-Fi maximum transmitting power, call esp_wifi_set_max_tx_power to set the maximum transmitting power. */
    wifi_country_policy_t policy;  /**< Country policy */
#if CONFIG_SOC_WIFI_SUPPORT_5G
    uint32_t              wifi_5g_channel_mask;  /**< A bitmask representing the allowed 5GHz Wi-Fi channels.
                                                      Each bit in the mask corresponds to a specific channel as wifi_5g_channel_bit_t shown.
                                                      Bitmask set to 0 indicates 5GHz channels are allowed according to local regulatory rules.
                                                      Please note that configured bitmask takes effect only when policy is manual. */
#endif
} wifi_country_t;

/**
  * @brief Wi-Fi authmode type
  * Strength of authmodes
  * Personal Networks   : OPEN < WEP < WPA_PSK < OWE < WPA2_PSK = WPA_WPA2_PSK < WAPI_PSK < WPA3_PSK = WPA2_WPA3_PSK = DPP
  * Enterprise Networks : WIFI_AUTH_WPA_ENTERPRISE < WIFI_AUTH_WPA2_ENTERPRISE < WIFI_AUTH_WPA3_ENTERPRISE = WIFI_AUTH_WPA2_WPA3_ENTERPRISE < WIFI_AUTH_WPA3_ENT_192
  */
typedef enum {
    WIFI_AUTH_OPEN = 0,         /**< Authenticate mode : open */
    WIFI_AUTH_WEP,              /**< Authenticate mode : WEP */
    WIFI_AUTH_WPA_PSK,          /**< Authenticate mode : WPA_PSK */
    WIFI_AUTH_WPA2_PSK,         /**< Authenticate mode : WPA2_PSK */
    WIFI_AUTH_WPA_WPA2_PSK,     /**< Authenticate mode : WPA_WPA2_PSK */
    WIFI_AUTH_ENTERPRISE,       /**< Authenticate mode : Wi-Fi EAP security, treated the same as WIFI_AUTH_WPA2_ENTERPRISE */
    WIFI_AUTH_WPA2_ENTERPRISE = WIFI_AUTH_ENTERPRISE,  /**< Authenticate mode : WPA2-Enterprise security */
    WIFI_AUTH_WPA3_PSK,         /**< Authenticate mode : WPA3_PSK */
    WIFI_AUTH_WPA2_WPA3_PSK,    /**< Authenticate mode : WPA2_WPA3_PSK */
    WIFI_AUTH_WAPI_PSK,         /**< Authenticate mode : WAPI_PSK */
    WIFI_AUTH_OWE,              /**< Authenticate mode : OWE */
    WIFI_AUTH_WPA3_ENT_192,     /**< Authenticate mode : WPA3_ENT_SUITE_B_192_BIT */
    WIFI_AUTH_WPA3_EXT_PSK,     /**< This authentication mode will yield same result as WIFI_AUTH_WPA3_PSK and not recommended to be used. It will be deprecated in future, please use WIFI_AUTH_WPA3_PSK instead. */
    WIFI_AUTH_WPA3_EXT_PSK_MIXED_MODE, /**< This authentication mode will yield same result as WIFI_AUTH_WPA3_PSK and not recommended to be used. It will be deprecated in future, please use WIFI_AUTH_WPA3_PSK instead.*/
    WIFI_AUTH_DPP,              /**< Authenticate mode : DPP */
    WIFI_AUTH_WPA3_ENTERPRISE,  /**< Authenticate mode : WPA3-Enterprise Only Mode */
    WIFI_AUTH_WPA2_WPA3_ENTERPRISE, /**< Authenticate mode : WPA3-Enterprise Transition Mode */
    WIFI_AUTH_WPA_ENTERPRISE,   /**< Authenticate mode : WPA-Enterprise security */
    WIFI_AUTH_MAX
} wifi_auth_mode_t;

/**
  * @brief Wi-Fi disconnection reason codes
  *
  * These reason codes are used to indicate the cause of disconnection.
  */
typedef enum {
    WIFI_REASON_UNSPECIFIED                        = 1,     /**< Unspecified reason */
    WIFI_REASON_AUTH_EXPIRE                        = 2,     /**< Authentication expired */
    WIFI_REASON_AUTH_LEAVE                         = 3,     /**< Deauthentication due to leaving */
    WIFI_REASON_ASSOC_EXPIRE                       = 4,     /**< Deprecated, will be removed in next IDF major release */
    WIFI_REASON_DISASSOC_DUE_TO_INACTIVITY         = 4,     /**< Disassociated due to inactivity */
    WIFI_REASON_ASSOC_TOOMANY                      = 5,     /**< Too many associated stations */
    WIFI_REASON_NOT_AUTHED                         = 6,     /**< Deprecated, will be removed in next IDF major release */
    WIFI_REASON_CLASS2_FRAME_FROM_NONAUTH_STA      = 6,     /**< Class 2 frame received from nonauthenticated STA */
    WIFI_REASON_NOT_ASSOCED                        = 7,     /**< Deprecated, will be removed in next IDF major release */
    WIFI_REASON_CLASS3_FRAME_FROM_NONASSOC_STA     = 7,     /**< Class 3 frame received from nonassociated STA */
    WIFI_REASON_ASSOC_LEAVE                        = 8,     /**< Deassociated due to leaving */
    WIFI_REASON_ASSOC_NOT_AUTHED                   = 9,     /**< Association but not authenticated */
    WIFI_REASON_DISASSOC_PWRCAP_BAD                = 10,    /**< Disassociated due to poor power capability */
    WIFI_REASON_DISASSOC_SUPCHAN_BAD               = 11,    /**< Disassociated due to unsupported channel */
    WIFI_REASON_BSS_TRANSITION_DISASSOC            = 12,    /**< Disassociated due to BSS transition */
    WIFI_REASON_IE_INVALID                         = 13,    /**< Invalid Information Element (IE) */
    WIFI_REASON_MIC_FAILURE                        = 14,    /**< MIC failure */
    WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT             = 15,    /**< 4-way handshake timeout */
    WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT           = 16,    /**< Group key update timeout */
    WIFI_REASON_IE_IN_4WAY_DIFFERS                 = 17,    /**< IE differs in 4-way handshake */
    WIFI_REASON_GROUP_CIPHER_INVALID               = 18,    /**< Invalid group cipher */
    WIFI_REASON_PAIRWISE_CIPHER_INVALID            = 19,    /**< Invalid pairwise cipher */
    WIFI_REASON_AKMP_INVALID                       = 20,    /**< Invalid AKMP */
    WIFI_REASON_UNSUPP_RSN_IE_VERSION              = 21,    /**< Unsupported RSN IE version */
    WIFI_REASON_INVALID_RSN_IE_CAP                 = 22,    /**< Invalid RSN IE capabilities */
    WIFI_REASON_802_1X_AUTH_FAILED                 = 23,    /**< 802.1X authentication failed */
    WIFI_REASON_CIPHER_SUITE_REJECTED              = 24,    /**< Cipher suite rejected */
    WIFI_REASON_TDLS_PEER_UNREACHABLE              = 25,    /**< TDLS peer unreachable */
    WIFI_REASON_TDLS_UNSPECIFIED                   = 26,    /**< TDLS unspecified */
    WIFI_REASON_SSP_REQUESTED_DISASSOC             = 27,    /**< SSP requested disassociation */
    WIFI_REASON_NO_SSP_ROAMING_AGREEMENT           = 28,    /**< No SSP roaming agreement */
    WIFI_REASON_BAD_CIPHER_OR_AKM                  = 29,    /**< Bad cipher or AKM */
    WIFI_REASON_NOT_AUTHORIZED_THIS_LOCATION       = 30,    /**< Not authorized in this location */
    WIFI_REASON_SERVICE_CHANGE_PERCLUDES_TS        = 31,    /**< Service change precludes TS */
    WIFI_REASON_UNSPECIFIED_QOS                    = 32,    /**< Unspecified QoS reason */
    WIFI_REASON_NOT_ENOUGH_BANDWIDTH               = 33,    /**< Not enough bandwidth */
    WIFI_REASON_MISSING_ACKS                       = 34,    /**< Missing ACKs */
    WIFI_REASON_EXCEEDED_TXOP                      = 35,    /**< Exceeded TXOP */
    WIFI_REASON_STA_LEAVING                        = 36,    /**< Station leaving */
    WIFI_REASON_END_BA                             = 37,    /**< End of Block Ack (BA) */
    WIFI_REASON_UNKNOWN_BA                         = 38,    /**< Unknown Block Ack (BA) */
    WIFI_REASON_TIMEOUT                            = 39,    /**< Timeout */
    WIFI_REASON_PEER_INITIATED                     = 46,    /**< Peer initiated disassociation */
    WIFI_REASON_AP_INITIATED                       = 47,    /**< AP initiated disassociation */
    WIFI_REASON_INVALID_FT_ACTION_FRAME_COUNT      = 48,    /**< Invalid FT action frame count */
    WIFI_REASON_INVALID_PMKID                      = 49,    /**< Invalid PMKID */
    WIFI_REASON_INVALID_MDE                        = 50,    /**< Invalid MDE */
    WIFI_REASON_INVALID_FTE                        = 51,    /**< Invalid FTE */
    WIFI_REASON_TRANSMISSION_LINK_ESTABLISH_FAILED = 67,    /**< Transmission link establishment failed */
    WIFI_REASON_ALTERATIVE_CHANNEL_OCCUPIED        = 68,    /**< Alternative channel occupied */

    WIFI_REASON_BEACON_TIMEOUT                     = 200,    /**< Beacon timeout */
    WIFI_REASON_NO_AP_FOUND                        = 201,    /**< No AP found */
    WIFI_REASON_AUTH_FAIL                          = 202,    /**< Authentication failed */
    WIFI_REASON_ASSOC_FAIL                         = 203,    /**< Association failed */
    WIFI_REASON_HANDSHAKE_TIMEOUT                  = 204,    /**< Handshake timeout */
    WIFI_REASON_CONNECTION_FAIL                    = 205,    /**< Connection failed */
    WIFI_REASON_AP_TSF_RESET                       = 206,    /**< AP TSF reset */
    WIFI_REASON_ROAMING                            = 207,    /**< Roaming */
    WIFI_REASON_ASSOC_COMEBACK_TIME_TOO_LONG       = 208,    /**< Association comeback time too long */
    WIFI_REASON_SA_QUERY_TIMEOUT                   = 209,    /**< SA query timeout */
    WIFI_REASON_NO_AP_FOUND_W_COMPATIBLE_SECURITY  = 210,    /**< No AP found with compatible security */
    WIFI_REASON_NO_AP_FOUND_IN_AUTHMODE_THRESHOLD  = 211,    /**< No AP found in auth mode threshold */
    WIFI_REASON_NO_AP_FOUND_IN_RSSI_THRESHOLD      = 212,    /**< No AP found in RSSI threshold */
} wifi_err_reason_t;

/**
  * @brief Wi-Fi second channel type
  */
typedef enum {
    WIFI_SECOND_CHAN_NONE = 0,  /**< The channel width is HT20 */
    WIFI_SECOND_CHAN_ABOVE,     /**< The channel width is HT40 and the secondary channel is above the primary channel */
    WIFI_SECOND_CHAN_BELOW,     /**< The channel width is HT40 and the secondary channel is below the primary channel */
} wifi_second_chan_t;

#define WIFI_ACTIVE_SCAN_MIN_DEFAULT_TIME 0             /**< Default minimum active scan time per channel */
#define WIFI_ACTIVE_SCAN_MAX_DEFAULT_TIME 120           /**< Default maximum active scan time per channel */
#define WIFI_PASSIVE_SCAN_DEFAULT_TIME 360              /**< Default passive scan time per channel */
#define WIFI_SCAN_HOME_CHANNEL_DWELL_DEFAULT_TIME 30    /**< Default time spent at home channel between scanning consecutive channels */

#define WIFI_SCAN_PARAMS_DEFAULT_CONFIG() { \
    .scan_time.active.min = WIFI_ACTIVE_SCAN_MIN_DEFAULT_TIME, \
    .scan_time.active.max = WIFI_ACTIVE_SCAN_MAX_DEFAULT_TIME, \
    .scan_time.passive = WIFI_PASSIVE_SCAN_DEFAULT_TIME, \
    .home_chan_dwell_time = WIFI_SCAN_HOME_CHANNEL_DWELL_DEFAULT_TIME\
}

/**
  * @brief Wi-Fi scan type
  */
typedef enum {
    WIFI_SCAN_TYPE_ACTIVE = 0,  /**< Active scan */
    WIFI_SCAN_TYPE_PASSIVE,     /**< Passive scan */
} wifi_scan_type_t;

/**
  * @brief Range of active scan times per channel
  */
typedef struct {
    uint32_t min;  /**< Minimum active scan time per channel, units: millisecond */
    uint32_t max;  /**< Maximum active scan time per channel, units: millisecond, values above 1500 ms may
                                          cause station to disconnect from AP and are not recommended.  */
} wifi_active_scan_time_t;

/**
  * @brief Aggregate of active & passive scan time per channel
  */
typedef struct {
    wifi_active_scan_time_t active;  /**< Active scan time per channel, units: millisecond. */
    uint32_t passive;                /**< Passive scan time per channel, units: millisecond, values above 1500 ms may
                                          cause station to disconnect from AP and are not recommended. */
} wifi_scan_time_t;

/**
  * @brief Channel bitmap for setting specific channels to be scanned
  */
typedef struct {
    uint16_t ghz_2_channels;     /**< Represents 2.4 GHz channels, that bits can be set as wifi_2g_channel_bit_t shown. */
    uint32_t ghz_5_channels;     /**< Represents 5 GHz channels, that bits can be set as wifi_5g_channel_bit_t shown. */
} wifi_scan_channel_bitmap_t;

/**
  * @brief Parameters for an SSID scan
  */
typedef struct {
    uint8_t *ssid;                                     /**< SSID of AP */
    uint8_t *bssid;                                    /**< MAC address of AP */
    uint8_t channel;                                   /**< Channel, scan the specific channel */
    bool show_hidden;                                  /**< Enable it to scan AP whose SSID is hidden */
    wifi_scan_type_t scan_type;                        /**< Scan type, active or passive */
    wifi_scan_time_t scan_time;                        /**< Scan time per channel */
    uint8_t home_chan_dwell_time;                      /**< Time spent at home channel between scanning consecutive channels. */
    wifi_scan_channel_bitmap_t channel_bitmap;         /**< Channel bitmap for setting specific channels to be scanned.
                                                            Please note that the 'channel' parameter above needs to be set to 0 to allow scanning by bitmap.
                                                            Also, note that only allowed channels configured by wifi_country_t can be scanned. */
} wifi_scan_config_t;

/**
  * @brief Parameters default scan configurations
  */
typedef struct {
    wifi_scan_time_t scan_time;  /**< Scan time per channel */
    uint8_t home_chan_dwell_time;/**< Time spent at home channel between scanning consecutive channels.*/
} wifi_scan_default_params_t;

/**
  * @brief Wi-Fi cipher type
  */
typedef enum {
    WIFI_CIPHER_TYPE_NONE = 0,   /**< The cipher type is none */
    WIFI_CIPHER_TYPE_WEP40,      /**< The cipher type is WEP40 */
    WIFI_CIPHER_TYPE_WEP104,     /**< The cipher type is WEP104 */
    WIFI_CIPHER_TYPE_TKIP,       /**< The cipher type is TKIP */
    WIFI_CIPHER_TYPE_CCMP,       /**< The cipher type is CCMP */
    WIFI_CIPHER_TYPE_TKIP_CCMP,  /**< The cipher type is TKIP and CCMP */
    WIFI_CIPHER_TYPE_AES_CMAC128,/**< The cipher type is AES-CMAC-128 */
    WIFI_CIPHER_TYPE_SMS4,       /**< The cipher type is SMS4 */
    WIFI_CIPHER_TYPE_GCMP,       /**< The cipher type is GCMP */
    WIFI_CIPHER_TYPE_GCMP256,    /**< The cipher type is GCMP-256 */
    WIFI_CIPHER_TYPE_AES_GMAC128,/**< The cipher type is AES-GMAC-128 */
    WIFI_CIPHER_TYPE_AES_GMAC256,/**< The cipher type is AES-GMAC-256 */
    WIFI_CIPHER_TYPE_UNKNOWN,    /**< The cipher type is unknown */
} wifi_cipher_type_t;

/**
  * @brief Wi-Fi bandwidth type
  */
typedef enum {
    WIFI_BW_HT20   = 1,       /**< Bandwidth is HT20      */
    WIFI_BW20 = WIFI_BW_HT20, /**< Bandwidth is 20 MHz    */
    WIFI_BW_HT40   = 2,       /**< Bandwidth is HT40      */
    WIFI_BW40 = WIFI_BW_HT40, /**< Bandwidth is 40 MHz    */
    WIFI_BW80      = 3,       /**< Bandwidth is 80 MHz    */
    WIFI_BW160     = 4,       /**< Bandwidth is 160 MHz   */
    WIFI_BW80_BW80 = 5,       /**< Bandwidth is 80 + 80 MHz */
} wifi_bandwidth_t;

/**
  * @brief Wi-Fi antenna
  */
typedef enum {
    WIFI_ANT_ANT0,          /**< Wi-Fi antenna 0 */
    WIFI_ANT_ANT1,          /**< Wi-Fi antenna 1 */
    WIFI_ANT_MAX,           /**< Invalid Wi-Fi antenna */
} wifi_ant_t;

/**
  * @brief Description of a Wi-Fi AP HE Info
  */
typedef struct {
    uint8_t bss_color: 6;                 /**< The BSS Color value associated with the AP's corresponding BSS */
    uint8_t partial_bss_color: 1;         /**< Indicates whether an AID assignment rule is based on the BSS color */
    uint8_t bss_color_disabled: 1;        /**< Indicates whether the BSS color usage is disabled */
    uint8_t bssid_index;                  /**< In a M-BSSID set, identifies the non-transmitted BSSID */
} wifi_he_ap_info_t;

/**
  * @brief Description of a Wi-Fi AP
  */
typedef struct {
    uint8_t bssid[6];                     /**< MAC address of AP */
    uint8_t ssid[33];                     /**< SSID of AP */
    uint8_t primary;                      /**< Channel of AP */
    wifi_second_chan_t second;            /**< Secondary channel of AP */
    int8_t  rssi;                         /**< Signal strength of AP. Note that in some rare cases where signal strength is very strong, RSSI values can be slightly positive */
    wifi_auth_mode_t authmode;            /**< Auth mode of AP */
    wifi_cipher_type_t pairwise_cipher;   /**< Pairwise cipher of AP */
    wifi_cipher_type_t group_cipher;      /**< Group cipher of AP */
    wifi_ant_t ant;                       /**< Antenna used to receive beacon from AP */
    uint32_t phy_11b: 1;                  /**< Bit: 0 flag to identify if 11b mode is enabled or not */
    uint32_t phy_11g: 1;                  /**< Bit: 1 flag to identify if 11g mode is enabled or not */
    uint32_t phy_11n: 1;                  /**< Bit: 2 flag to identify if 11n mode is enabled or not */
    uint32_t phy_lr: 1;                   /**< Bit: 3 flag to identify if low rate is enabled or not */
    uint32_t phy_11a: 1;                  /**< Bit: 4 flag to identify if 11ax mode is enabled or not */
    uint32_t phy_11ac: 1;                 /**< Bit: 5 flag to identify if 11ax mode is enabled or not */
    uint32_t phy_11ax: 1;                 /**< Bit: 6 flag to identify if 11ax mode is enabled or not */
    uint32_t wps: 1;                      /**< Bit: 7 flag to identify if WPS is supported or not */
    uint32_t ftm_responder: 1;            /**< Bit: 8 flag to identify if FTM is supported in responder mode */
    uint32_t ftm_initiator: 1;            /**< Bit: 9 flag to identify if FTM is supported in initiator mode */
    uint32_t reserved: 22;                /**< Bit: 10..31 reserved */
    wifi_country_t country;               /**< Country information of AP */
    wifi_he_ap_info_t he_ap;              /**< HE AP info */
    wifi_bandwidth_t bandwidth;           /**< Bandwidth of AP */
    uint8_t vht_ch_freq1;                 /**< This fields are used only AP bandwidth is 80 and 160 MHz, to transmit the center channel
                                               frequency of the BSS. For AP bandwidth is 80 + 80 MHz, it is the center channel frequency
                                               of the lower frequency segment.*/
    uint8_t vht_ch_freq2;                 /**< this fields are used only AP bandwidth is 80 + 80 MHz, and is used to transmit the center
                                               channel frequency of the second segment. */
} wifi_ap_record_t;

/**
  * @brief Wi-Fi scan method
  */
typedef enum {
    WIFI_FAST_SCAN = 0,                   /**< Do fast scan, scan will end after find SSID match AP */
    WIFI_ALL_CHANNEL_SCAN,                /**< All channel scan, scan will end after scan all the channel */
} wifi_scan_method_t;

/**
  * @brief Wi-Fi sort AP method
  */
typedef enum {
    WIFI_CONNECT_AP_BY_SIGNAL = 0,        /**< Sort match AP in scan list by RSSI */
    WIFI_CONNECT_AP_BY_SECURITY,          /**< Sort match AP in scan list by security mode */
} wifi_sort_method_t;

/**
  * @brief Structure describing parameters for a Wi-Fi fast scan
  */
typedef struct {
    int8_t              rssi;             /**< The minimum rssi to accept in the fast scan mode. Defaults to -127 if set to >= 0 */
    wifi_auth_mode_t    authmode;         /**< The weakest auth mode to accept in the fast scan mode
                                               Note: In case this value is not set and password is set as per WPA2 standards(password len >= 8), it will be defaulted to WPA2 and device won't connect to deprecated WEP/WPA networks. Please set auth mode threshold as WIFI_AUTH_WEP/WIFI_AUTH_WPA_PSK to connect to WEP/WPA networks */
    uint8_t             rssi_5g_adjustment; /**< The RSSI value of the 5G AP is within the rssi_5g_adjustment range compared to the 2G AP, the 5G AP will be given priority for connection. */
} wifi_scan_threshold_t;

/**
  * @brief Wi-Fi power save type
  */
typedef enum {
    WIFI_PS_NONE,        /**< No power save */
    WIFI_PS_MIN_MODEM,   /**< Minimum modem power saving. In this mode, station wakes up to receive beacon every DTIM period */
    WIFI_PS_MAX_MODEM,   /**< Maximum modem power saving. In this mode, interval to receive beacons is determined by the listen_interval parameter in wifi_sta_config_t */
} wifi_ps_type_t;

/**
  * @brief Argument structure for Wi-Fi band
  */
typedef enum {
    WIFI_BAND_2G = 1,                   /**< Band is 2.4 GHz */
    WIFI_BAND_5G = 2,                   /**< Band is 5 GHz */
} wifi_band_t;

/**
  * @brief Argument structure for Wi-Fi band mode
  */
typedef enum {
    WIFI_BAND_MODE_2G_ONLY = 1,         /**< Wi-Fi band mode is 2.4 GHz only */
    WIFI_BAND_MODE_5G_ONLY = 2,         /**< Wi-Fi band mode is 5 GHz only */
    WIFI_BAND_MODE_AUTO = 3,            /**< Wi-Fi band mode is 2.4 GHz + 5 GHz */
} wifi_band_mode_t;

#ifndef BIT
#define BIT(nr)  (1 << (nr))
#endif

#define CHANNEL_TO_BIT_NUMBER(channel) ((channel >= 1 && channel <= 14) ? (channel) : \
    ((channel >= 36 && channel <= 64 && (channel - 36) % 4 == 0) ? ((channel - 36) / 4 + 1) : \
    ((channel >= 100 && channel <= 144 && (channel - 100) % 4 == 0) ? ((channel - 100) / 4 + 9) : \
    ((channel >= 149 && channel <= 177 && (channel - 149) % 4 == 0) ? ((channel - 149) / 4 + 21) : 0))))

#define BIT_NUMBER_TO_CHANNEL(bit_number, band) ((band == WIFI_BAND_2G) ? (bit_number) : \
    ((bit_number >= 1 && bit_number <= 8) ? ((bit_number - 1) * 4 + 36) : \
    ((bit_number >= 9 && bit_number <= 20) ? ((bit_number - 9) * 4 + 100) : \
    ((bit_number >= 21 && bit_number <= 28) ? ((bit_number - 21) * 4 + 149) : 0))))

#define CHANNEL_TO_BIT(channel) (BIT(CHANNEL_TO_BIT_NUMBER(channel)))

/** Argument structure for 2.4G channels */
typedef enum {
    WIFI_CHANNEL_1 = BIT(1),   /**< Wi-Fi channel 1 */
    WIFI_CHANNEL_2 = BIT(2),   /**< Wi-Fi channel 2 */
    WIFI_CHANNEL_3 = BIT(3),   /**< Wi-Fi channel 3 */
    WIFI_CHANNEL_4 = BIT(4),   /**< Wi-Fi channel 4 */
    WIFI_CHANNEL_5 = BIT(5),   /**< Wi-Fi channel 5 */
    WIFI_CHANNEL_6 = BIT(6),   /**< Wi-Fi channel 6 */
    WIFI_CHANNEL_7 = BIT(7),   /**< Wi-Fi channel 7 */
    WIFI_CHANNEL_8 = BIT(8),   /**< Wi-Fi channel 8 */
    WIFI_CHANNEL_9 = BIT(9),   /**< Wi-Fi channel 9 */
    WIFI_CHANNEL_10 = BIT(10), /**< Wi-Fi channel 10 */
    WIFI_CHANNEL_11 = BIT(11), /**< Wi-Fi channel 11 */
    WIFI_CHANNEL_12 = BIT(12), /**< Wi-Fi channel 12 */
    WIFI_CHANNEL_13 = BIT(13), /**< Wi-Fi channel 13 */
    WIFI_CHANNEL_14 = BIT(14), /**< Wi-Fi channel 14 */
} wifi_2g_channel_bit_t;

/** Argument structure for 5G channels */
typedef enum {
    WIFI_CHANNEL_36 = BIT(1),   /**< Wi-Fi channel 36 */
    WIFI_CHANNEL_40 = BIT(2),   /**< Wi-Fi channel 40 */
    WIFI_CHANNEL_44 = BIT(3),   /**< Wi-Fi channel 44 */
    WIFI_CHANNEL_48 = BIT(4),   /**< Wi-Fi channel 48 */
    WIFI_CHANNEL_52 = BIT(5),   /**< Wi-Fi channel 52 */
    WIFI_CHANNEL_56 = BIT(6),   /**< Wi-Fi channel 56 */
    WIFI_CHANNEL_60 = BIT(7),   /**< Wi-Fi channel 60 */
    WIFI_CHANNEL_64 = BIT(8),   /**< Wi-Fi channel 64 */
    WIFI_CHANNEL_100 = BIT(9),  /**< Wi-Fi channel 100 */
    WIFI_CHANNEL_104 = BIT(10), /**< Wi-Fi channel 104 */
    WIFI_CHANNEL_108 = BIT(11), /**< Wi-Fi channel 108 */
    WIFI_CHANNEL_112 = BIT(12), /**< Wi-Fi channel 112 */
    WIFI_CHANNEL_116 = BIT(13), /**< Wi-Fi channel 116 */
    WIFI_CHANNEL_120 = BIT(14), /**< Wi-Fi channel 120 */
    WIFI_CHANNEL_124 = BIT(15), /**< Wi-Fi channel 124 */
    WIFI_CHANNEL_128 = BIT(16), /**< Wi-Fi channel 128 */
    WIFI_CHANNEL_132 = BIT(17), /**< Wi-Fi channel 132 */
    WIFI_CHANNEL_136 = BIT(18), /**< Wi-Fi channel 136 */
    WIFI_CHANNEL_140 = BIT(19), /**< Wi-Fi channel 140 */
    WIFI_CHANNEL_144 = BIT(20), /**< Wi-Fi channel 144 */
    WIFI_CHANNEL_149 = BIT(21), /**< Wi-Fi channel 149 */
    WIFI_CHANNEL_153 = BIT(22), /**< Wi-Fi channel 153 */
    WIFI_CHANNEL_157 = BIT(23), /**< Wi-Fi channel 157 */
    WIFI_CHANNEL_161 = BIT(24), /**< Wi-Fi channel 161 */
    WIFI_CHANNEL_165 = BIT(25), /**< Wi-Fi channel 165 */
    WIFI_CHANNEL_169 = BIT(26), /**< Wi-Fi channel 169 */
    WIFI_CHANNEL_173 = BIT(27), /**< Wi-Fi channel 173 */
    WIFI_CHANNEL_177 = BIT(28), /**< Wi-Fi channel 177 */
} wifi_5g_channel_bit_t;

#define WIFI_PROTOCOL_11B         0x1     /**< 802.11b protocol */
#define WIFI_PROTOCOL_11G         0x2     /**< 802.11g protocol */
#define WIFI_PROTOCOL_11N         0x4     /**< 802.11n protocol */
#define WIFI_PROTOCOL_LR          0x8     /**< Low Rate protocol */
#define WIFI_PROTOCOL_11A         0x10    /**< 802.11a protocol */
#define WIFI_PROTOCOL_11AC        0x20    /**< 802.11ac protocol */
#define WIFI_PROTOCOL_11AX        0x40    /**< 802.11ax protocol */

/**
  * @brief Description of a Wi-Fi protocols
  */
typedef struct {
    uint16_t ghz_2g;            /**< Represents 2.4 GHz protocol, support 802.11b or 802.11g or 802.11n or 802.11ax or LR mode */
    uint16_t ghz_5g;            /**< Represents 5 GHz protocol, support 802.11a or 802.11n or 802.11ac or 802.11ax */
} wifi_protocols_t;

/**
  * @brief Description of a Wi-Fi band bandwidths
  */
typedef struct {
    wifi_bandwidth_t ghz_2g;       /**< Represents 2.4 GHz bandwidth */
    wifi_bandwidth_t ghz_5g;       /**< Represents 5 GHz bandwidth */
} wifi_bandwidths_t;

/**
  * @brief Configuration structure for Protected Management Frame
  */
typedef struct {
    bool capable;            /**< Deprecated variable. Device will always connect in PMF mode if other device also advertises PMF capability. */
    bool required;           /**< Advertises that Protected Management Frame is required. Device will not associate to non-PMF capable devices. */
} wifi_pmf_config_t;

/**
  * @brief Configuration for SAE PWE derivation
  */
typedef enum {
    WPA3_SAE_PWE_UNSPECIFIED,
    WPA3_SAE_PWE_HUNT_AND_PECK,
    WPA3_SAE_PWE_HASH_TO_ELEMENT,
    WPA3_SAE_PWE_BOTH,
} wifi_sae_pwe_method_t;

/**
  * @brief Configuration for SAE-PK
  */
typedef enum {
    WPA3_SAE_PK_MODE_AUTOMATIC = 0,
    WPA3_SAE_PK_MODE_ONLY = 1,
    WPA3_SAE_PK_MODE_DISABLED = 2,
} wifi_sae_pk_mode_t;

/**
  * @brief Configuration structure for BSS max idle
  */
typedef struct {
    uint16_t period;                /**< Sets BSS Max idle period (1 Unit = 1000TUs OR 1.024 Seconds). If there are no frames for this period from a STA, SoftAP will disassociate due to inactivity. Setting it to 0 disables the feature */
    bool protected_keep_alive;      /**< Requires clients to use protected keep alive frames for BSS Max Idle period */
} wifi_bss_max_idle_config_t;

/**
  * @brief Soft-AP configuration settings for the device
  */
typedef struct {
    uint8_t ssid[32];                         /**< SSID of soft-AP. If ssid_len field is 0, this must be a Null terminated string. Otherwise, length is set according to ssid_len. */
    uint8_t password[64];                     /**< Password of soft-AP. */
    uint8_t ssid_len;                         /**< Optional length of SSID field. */
    uint8_t channel;                          /**< Channel of soft-AP */
    wifi_auth_mode_t authmode;                /**< Auth mode of soft-AP. Do not support AUTH_WEP, AUTH_WAPI_PSK and AUTH_OWE in soft-AP mode. When the auth mode is set to WPA2_PSK, WPA2_WPA3_PSK or WPA3_PSK, the pairwise cipher will be overwritten with WIFI_CIPHER_TYPE_CCMP.  */
    uint8_t ssid_hidden;                      /**< Broadcast SSID or not, default 0, broadcast the SSID */
    uint8_t max_connection;                   /**< Max number of stations allowed to connect in */
    uint16_t beacon_interval;                 /**< Beacon interval which should be multiples of 100. Unit: TU(time unit, 1 TU = 1024 us). Range: 100 ~ 60000. Default value: 100 */
    uint8_t csa_count;                        /**< Channel Switch Announcement Count. Notify the station that the channel will switch after the csa_count beacon intervals. Default value: 3 */
    uint8_t dtim_period;                      /**< Dtim period of soft-AP. Range: 1 ~ 10. Default value: 1 */
    wifi_cipher_type_t pairwise_cipher;       /**< Pairwise cipher of SoftAP, group cipher will be derived using this. Cipher values are valid starting from WIFI_CIPHER_TYPE_TKIP, enum values before that will be considered as invalid and default cipher suites(TKIP+CCMP) will be used. Valid cipher suites in softAP mode are WIFI_CIPHER_TYPE_TKIP, WIFI_CIPHER_TYPE_CCMP and WIFI_CIPHER_TYPE_TKIP_CCMP. */
    bool ftm_responder;                       /**< Enable FTM Responder mode */
    wifi_pmf_config_t pmf_cfg;                /**< Configuration for Protected Management Frame */
    wifi_sae_pwe_method_t sae_pwe_h2e;        /**< Configuration for SAE PWE derivation method */
    uint8_t transition_disable;               /**< Whether to enable transition disable feature */
    uint8_t sae_ext;                          /**< Enable SAE EXT feature. SOC_GCMP_SUPPORT is required for this feature. */
    wifi_bss_max_idle_config_t bss_max_idle_cfg;  /**< Configuration for bss max idle, effective if CONFIG_WIFI_BSS_MAX_IDLE_SUPPORT is enabled */
    uint16_t gtk_rekey_interval;              /**< GTK rekeying interval in seconds. If set to 0, GTK rekeying is disabled. Range: 60 ~ 65535 including 0. */
} wifi_ap_config_t;

#define SAE_H2E_IDENTIFIER_LEN 32    /**< Length of the password identifier for H2E */

/**
  * @brief STA configuration settings for the device
  */
typedef struct {
    uint8_t ssid[32];                         /**< SSID of target AP. */
    uint8_t password[64];                     /**< Password of target AP. */
    wifi_scan_method_t scan_method;           /**< Do all channel scan or fast scan */
    bool bssid_set;                           /**< Whether set MAC address of target AP or not. Generally, station_config.bssid_set needs to be 0; and it needs to be 1 only when users need to check the MAC address of the AP.*/
    uint8_t bssid[6];                         /**< MAC address of target AP*/
    uint8_t channel;                          /**< Channel hint for target AP. For 2.4G AP, set to 1~13 to scan starting from the specified channel before connecting to AP. For 5G AP, set to 36~177 (36, 40, 44 ... 177) to scan starting from the specified channel before connecting to AP. Set to 0 for no preference */
    uint16_t listen_interval;                 /**< Listen interval for ESP32 station to receive beacon when WIFI_PS_MAX_MODEM is set. Units: AP beacon intervals. Defaults to 3 if set to 0. */
    wifi_sort_method_t sort_method;           /**< Sort the connect AP in the list by rssi or security mode */
    wifi_scan_threshold_t  threshold;         /**< When scan_threshold is set, only APs which have an auth mode that is more secure than the selected auth mode and a signal stronger than the minimum RSSI will be used. */
    wifi_pmf_config_t pmf_cfg;                /**< Configuration for Protected Management Frame. Will be advertised in RSN Capabilities in RSN IE. */
    uint32_t rm_enabled: 1;                   /**< Whether Radio Measurements are enabled for the connection */
    uint32_t btm_enabled: 1;                  /**< Whether BSS Transition Management is enabled for the connection. Note that when btm is enabled, the application itself should not set specific bssid (i.e using bssid_set and bssid in this config)or channel to connect to. This defeats the purpose of a BTM supported network, and hence if btm is supported and a specific bssid or channel is set in this config, it will be cleared from the config at the first disconnection or connection so that the device can roam to other BSS. It is recommended not to set BSSID when BTM is enabled.  */
    uint32_t mbo_enabled: 1;                  /**< Whether MBO is enabled for the connection. Note that when mbo is enabled, the application itself should not set specific bssid (i.e using bssid_set and bssid in this config)or channel to connect to. This defeats the purpose of a MBO supported network, and hence if btm is supported and a specific bssid or channel is set in this config, it will be cleared from the config at the first disconnection or connection so that the device can roam to other BSS. It is recommended not to set BSSID when MBO is enabled. Enabling mbo here, automatically enables btm and rm above.*/
    uint32_t ft_enabled: 1;                   /**< Whether FT is enabled for the connection */
    uint32_t owe_enabled: 1;                  /**< Whether OWE is enabled for the connection */
    uint32_t transition_disable: 1;           /**< Whether to enable transition disable feature */
    uint32_t reserved1: 26;                   /**< Reserved for future feature set */
    wifi_sae_pwe_method_t sae_pwe_h2e;        /**< Configuration for SAE PWE derivation method */
    wifi_sae_pk_mode_t sae_pk_mode;           /**< Configuration for SAE-PK (Public Key) Authentication method */
    uint8_t failure_retry_cnt;                /**< Number of connection retries station will do before moving to next AP. scan_method should be set as WIFI_ALL_CHANNEL_SCAN to use this config.
                                                   Note: Enabling this may cause connection time to increase in case best AP doesn't behave properly. */
    uint32_t he_dcm_set: 1;                                       /**< Whether DCM max.constellation for transmission and reception is set. */
    uint32_t he_dcm_max_constellation_tx: 2;                      /**< Indicate the max.constellation for DCM in TB PPDU the STA supported. 0: not supported. 1: BPSK, 2: QPSK, 3: 16-QAM. The default value is 3. */
    uint32_t he_dcm_max_constellation_rx: 2;                      /**< Indicate the max.constellation for DCM in both Data field and HE-SIG-B field the STA supported. 0: not supported. 1: BPSK, 2: QPSK, 3: 16-QAM. The default value is 3. */
    uint32_t he_mcs9_enabled: 1;                                  /**< Whether to support HE-MCS8 and HE-MCS9. The default value is 0. */
    uint32_t he_su_beamformee_disabled: 1;                        /**< Whether to disable support for operation as an SU beamformee. */
    uint32_t he_trig_su_bmforming_feedback_disabled: 1;           /**< Whether to disable support the transmission of SU feedback in an HE TB sounding sequence. */
    uint32_t he_trig_mu_bmforming_partial_feedback_disabled: 1;   /**< Whether to disable support the transmission of partial-bandwidth MU feedback in an HE TB sounding sequence. */
    uint32_t he_trig_cqi_feedback_disabled: 1;                    /**< Whether to disable support the transmission of CQI feedback in an HE TB sounding sequence. */
    uint32_t vht_su_beamformee_disabled: 1;                       /**< Whether to disable support for operation as an VHT SU beamformee. */
    uint32_t vht_mu_beamformee_disabled: 1;                       /**< Whether to disable support for operation as an VHT MU beamformee. */
    uint32_t vht_mcs8_enabled: 1;                                 /**< Whether to support VHT-MCS8. The default value is 0. */
    uint32_t reserved2: 19;                                       /**< Reserved for future feature set */
    uint8_t sae_h2e_identifier[SAE_H2E_IDENTIFIER_LEN];/**< Password identifier for H2E. this needs to be null terminated string */
} wifi_sta_config_t;

/**
  * @brief NAN Discovery start configuration
  */
typedef struct {
    uint8_t op_channel;    /**< NAN Discovery operating channel */
    uint8_t master_pref;   /**< Device's preference value to serve as NAN Master */
    uint8_t scan_time;     /**< Scan time in seconds while searching for a NAN cluster */
    uint16_t warm_up_sec;  /**< Warm up time before assuming NAN Anchor Master role */
} wifi_nan_config_t;

/**
  * @brief Configuration data for device's AP or STA or NAN.
  *
  * The usage of this union (for ap, sta or nan configuration) is determined by the accompanying
  * interface argument passed to esp_wifi_set_config() or esp_wifi_get_config()
  *
  */
typedef union {
    wifi_ap_config_t  ap;  /**< Configuration of AP */
    wifi_sta_config_t sta; /**< Configuration of STA */
    wifi_nan_config_t nan; /**< Configuration of NAN */
} wifi_config_t;

/**
  * @brief Description of STA associated with AP
  */
typedef struct {
    uint8_t mac[6];          /**< MAC address */
    int8_t  rssi;            /**< Current average rssi of sta connected */
    uint32_t phy_11b: 1;     /**< Bit: 0 flag to identify if 11b mode is enabled or not */
    uint32_t phy_11g: 1;     /**< Bit: 1 flag to identify if 11g mode is enabled or not */
    uint32_t phy_11n: 1;     /**< Bit: 2 flag to identify if 11n mode is enabled or not */
    uint32_t phy_lr: 1;      /**< Bit: 3 flag to identify if low rate is enabled or not */
    uint32_t phy_11a: 1;     /**< Bit: 4 flag to identify if 11ax mode is enabled or not */
    uint32_t phy_11ac: 1;    /**< Bit: 5 flag to identify if 11ax mode is enabled or not */
    uint32_t phy_11ax: 1;    /**< Bit: 6 flag to identify if 11ax mode is enabled or not */
    uint32_t is_mesh_child: 1; /**< Bit: 7 flag to identify mesh child */
    uint32_t reserved: 24;   /**< Bit: 8..31 reserved */
} wifi_sta_info_t;

/**
  * @brief Wi-Fi storage type
  */
typedef enum {
    WIFI_STORAGE_FLASH,  /**< All configuration will store in both memory and flash */
    WIFI_STORAGE_RAM,    /**< All configuration will only store in the memory */
} wifi_storage_t;

/**
  * @brief     Vendor Information Element type
  *
  * Determines the frame type that the IE will be associated with.
  */
typedef enum {
    WIFI_VND_IE_TYPE_BEACON,        /**< Beacon frame */
    WIFI_VND_IE_TYPE_PROBE_REQ,     /**< Probe request frame */
    WIFI_VND_IE_TYPE_PROBE_RESP,    /**< Probe response frame */
    WIFI_VND_IE_TYPE_ASSOC_REQ,     /**< Association request frame */
    WIFI_VND_IE_TYPE_ASSOC_RESP,    /**< Association response frame */
} wifi_vendor_ie_type_t;

/**
  * @brief     Vendor Information Element index
  *
  * Each IE type can have up to two associated vendor ID elements.
  */
typedef enum {
    WIFI_VND_IE_ID_0,    /**< Vendor ID element 0 */
    WIFI_VND_IE_ID_1,    /**< Vendor ID element 1 */
} wifi_vendor_ie_id_t;

#define WIFI_VENDOR_IE_ELEMENT_ID 0xDD    /**< Vendor Information Element ID */

/**
  * @brief     Operation PHY mode
  */
typedef enum {
    WIFI_PHY_MODE_LR,   /**< PHY mode for Low Rate */
    WIFI_PHY_MODE_11B,  /**< PHY mode for 11b */
    WIFI_PHY_MODE_11G,  /**< PHY mode for 11g */
    WIFI_PHY_MODE_11A,  /**< PHY mode for 11a */
    WIFI_PHY_MODE_HT20, /**< PHY mode for Bandwidth HT20 */
    WIFI_PHY_MODE_HT40, /**< PHY mode for Bandwidth HT40 */
    WIFI_PHY_MODE_HE20, /**< PHY mode for Bandwidth HE20 */
    WIFI_PHY_MODE_VHT20,/**< PHY mode for Bandwidth VHT20 */
} wifi_phy_mode_t;

/**
  * @brief Vendor Information Element header
  *
  * The first bytes of the Information Element will match this header. Payload follows.
  */
typedef struct {
    uint8_t element_id;      /**< Should be set to WIFI_VENDOR_IE_ELEMENT_ID (0xDD) */
    uint8_t length;          /**< Length of all bytes in the element data following this field. Minimum 4. */
    uint8_t vendor_oui[3];   /**< Vendor identifier (OUI). */
    uint8_t vendor_oui_type; /**< Vendor-specific OUI type. */
    uint8_t payload[0];      /**< Payload. Length is equal to value in 'length' field, minus 4. */
} vendor_ie_data_t;

/**
  * @brief Promiscuous frame type
  *
  * Passed to promiscuous mode RX callback to indicate the type of parameter in the buffer.
  *
  */
typedef enum {
    WIFI_PKT_MGMT,  /**< Management frame, indicates 'buf' argument is wifi_promiscuous_pkt_t */
    WIFI_PKT_CTRL,  /**< Control frame, indicates 'buf' argument is wifi_promiscuous_pkt_t */
    WIFI_PKT_DATA,  /**< Data frame, indicates 'buf' argument is wifi_promiscuous_pkt_t */
    WIFI_PKT_MISC,  /**< Other type, such as MIMO etc. 'buf' argument is wifi_promiscuous_pkt_t but the payload is zero length. */
} wifi_promiscuous_pkt_type_t;

#define WIFI_PROMIS_FILTER_MASK_ALL         (0xFFFFFFFF)  /**< Filter all packets */
#define WIFI_PROMIS_FILTER_MASK_MGMT        (1)           /**< Filter the packets with type of WIFI_PKT_MGMT */
#define WIFI_PROMIS_FILTER_MASK_CTRL        (1<<1)        /**< Filter the packets with type of WIFI_PKT_CTRL */
#define WIFI_PROMIS_FILTER_MASK_DATA        (1<<2)        /**< Filter the packets with type of WIFI_PKT_DATA */
#define WIFI_PROMIS_FILTER_MASK_MISC        (1<<3)        /**< Filter the packets with type of WIFI_PKT_MISC */
#define WIFI_PROMIS_FILTER_MASK_DATA_MPDU   (1<<4)        /**< Filter the MPDU which is a kind of WIFI_PKT_DATA */
#define WIFI_PROMIS_FILTER_MASK_DATA_AMPDU  (1<<5)        /**< Filter the AMPDU which is a kind of WIFI_PKT_DATA */
#define WIFI_PROMIS_FILTER_MASK_FCSFAIL     (1<<6)        /**< Filter the FCS failed packets, do not open it in general */

#define WIFI_PROMIS_CTRL_FILTER_MASK_ALL         (0xFF800000)  /**< Filter all control packets */
#define WIFI_PROMIS_CTRL_FILTER_MASK_WRAPPER     (1<<23)       /**< Filter the control packets with subtype of Control Wrapper */
#define WIFI_PROMIS_CTRL_FILTER_MASK_BAR         (1<<24)       /**< Filter the control packets with subtype of Block Ack Request */
#define WIFI_PROMIS_CTRL_FILTER_MASK_BA          (1<<25)       /**< Filter the control packets with subtype of Block Ack */
#define WIFI_PROMIS_CTRL_FILTER_MASK_PSPOLL      (1<<26)       /**< Filter the control packets with subtype of PS-Poll */
#define WIFI_PROMIS_CTRL_FILTER_MASK_RTS         (1<<27)       /**< Filter the control packets with subtype of RTS */
#define WIFI_PROMIS_CTRL_FILTER_MASK_CTS         (1<<28)       /**< Filter the control packets with subtype of CTS */
#define WIFI_PROMIS_CTRL_FILTER_MASK_ACK         (1<<29)       /**< Filter the control packets with subtype of ACK */
#define WIFI_PROMIS_CTRL_FILTER_MASK_CFEND       (1<<30)       /**< Filter the control packets with subtype of CF-END */
#define WIFI_PROMIS_CTRL_FILTER_MASK_CFENDACK    (1<<31)       /**< Filter the control packets with subtype of CF-END+CF-ACK */

/**
  * @brief Mask for filtering different packet types in promiscuous mode
  */
typedef struct {
    uint32_t filter_mask; /**< OR of one or more filter values WIFI_PROMIS_FILTER_* */
} wifi_promiscuous_filter_t;

#define WIFI_EVENT_MASK_ALL                 (0xFFFFFFFF)  /**< Mask all Wi-Fi events */
#define WIFI_EVENT_MASK_NONE                (0)           /**< Mask none of the Wi-Fi events */
#define WIFI_EVENT_MASK_AP_PROBEREQRECVED   (BIT(0))      /**< Mask SYSTEM_EVENT_AP_PROBEREQRECVED event */

/**
  * @brief CSI data type
  *
  */
typedef struct wifi_csi_info_t wifi_csi_info_t;

/**
  * @brief Wi-Fi GPIO configuration for antenna selection
  *
  */
typedef struct {
    uint8_t gpio_select: 1,           /**< Whether this GPIO is connected to external antenna switch */
            gpio_num: 7;              /**< The GPIO number that connects to external antenna switch */
} wifi_ant_gpio_t;

/**
  * @brief Wi-Fi GPIOs configuration for antenna selection
  *
  */
typedef struct {
    wifi_ant_gpio_t  gpio_cfg[4];  /**< The configurations of GPIOs that connect to external antenna switch */
} wifi_ant_gpio_config_t;

/**
  * @brief Wi-Fi antenna mode
  *
  */
typedef enum {
    WIFI_ANT_MODE_ANT0,          /**< Enable Wi-Fi antenna 0 only */
    WIFI_ANT_MODE_ANT1,          /**< Enable Wi-Fi antenna 1 only */
    WIFI_ANT_MODE_AUTO,          /**< Enable Wi-Fi antenna 0 and 1, automatically select an antenna */
    WIFI_ANT_MODE_MAX,           /**< Invalid Wi-Fi enabled antenna */
} wifi_ant_mode_t;

/**
  * @brief Wi-Fi antenna configuration
  *
  */
typedef struct {
    wifi_ant_mode_t rx_ant_mode;          /**< Wi-Fi antenna mode for receiving */
    wifi_ant_t      rx_ant_default;       /**< Default antenna mode for receiving, it's ignored if rx_ant_mode is not WIFI_ANT_MODE_AUTO */
    wifi_ant_mode_t tx_ant_mode;          /**< Wi-Fi antenna mode for transmission, it can be set to WIFI_ANT_MODE_AUTO only if rx_ant_mode is set to WIFI_ANT_MODE_AUTO */
    uint8_t         enabled_ant0: 4,      /**< Index (in antenna GPIO configuration) of enabled WIFI_ANT_MODE_ANT0 */
                    enabled_ant1: 4;      /**< Index (in antenna GPIO configuration) of enabled WIFI_ANT_MODE_ANT1 */
} wifi_ant_config_t;

/**
  * @brief     The Rx callback function of Action Tx operations
  *
  * @param     hdr Pointer to the IEEE 802.11 Header structure
  * @param     payload Pointer to the Payload following 802.11 Header
  * @param     len Length of the Payload
  * @param     channel Channel number the frame is received on
  *
  */
typedef int (* wifi_action_rx_cb_t)(uint8_t *hdr, uint8_t *payload,
                                    size_t len, uint8_t channel);
/**
  * @brief Action Frame Tx Request
  */
typedef struct {
    wifi_interface_t ifx;       /**< Wi-Fi interface to send request to */
    uint8_t dest_mac[6];        /**< Destination MAC address */
    wifi_action_tx_t type;      /**< ACTION TX operation type */
    uint8_t channel;            /**< Channel on which to perform ACTION TX Operation */
    uint32_t wait_time_ms;      /**< Duration to wait for on target channel */
    bool no_ack;                /**< Indicates no ack required */
    wifi_action_rx_cb_t rx_cb;  /**< Rx Callback to receive action frames */
    uint8_t op_id;              /**< Unique Identifier for operation provided by wifi driver */
    uint32_t data_len;          /**< Length of the appended Data */
    uint8_t data[0];            /**< Appended Data payload */
} wifi_action_tx_req_t;

/** Status codes for WIFI_EVENT_ROC_DONE evt */
typedef enum {
    WIFI_ROC_DONE = 0,         /**< ROC operation was completed successfully */
    WIFI_ROC_FAIL,             /**< ROC operation was cancelled */
} wifi_roc_done_status_t;

/**
  * @brief     The callback function executed when ROC operation has ended
  *
  * @param     context rxcb registered for the corresponding ROC operation
  * @param     op_id  ID of the corresponding ROC operation
  * @param     status status code of the ROC operation denoted
  *
  */
typedef void (* wifi_action_roc_done_cb_t)(uint32_t context, uint8_t op_id,
                                           wifi_roc_done_status_t status);

/**
 * @brief Remain on Channel request
 *
 *
 */
typedef struct {
    wifi_interface_t ifx;              /**< WiFi interface to send request to */
    wifi_roc_t type;                   /**< ROC operation type */
    uint8_t channel;                   /**< Channel on which to perform ROC Operation */
    wifi_second_chan_t sec_channel;    /**< Secondary channel */
    uint32_t wait_time_ms;             /**< Duration to wait for on target channel */
    wifi_action_rx_cb_t rx_cb;         /**< Rx Callback to receive any response */
    uint8_t op_id;                     /**< ID of this specific ROC operation provided by wifi driver */
    wifi_action_roc_done_cb_t done_cb; /**< Callback to function that will be called upon ROC done. If assigned, WIFI_EVENT_ROC_DONE event will not be posted */
} wifi_roc_req_t;

/**
  * @brief FTM Initiator configuration
  *
  */
typedef struct {
    uint8_t resp_mac[6];        /**< MAC address of the FTM Responder */
    uint8_t channel;            /**< Primary channel of the FTM Responder */
    uint8_t frm_count;          /**< No. of FTM frames requested in terms of 4 or 8 bursts (allowed values - 0(No pref), 16, 24, 32, 64) */
    uint16_t burst_period;      /**< Requested period between FTM bursts in 100's of milliseconds (allowed values 0(No pref) - 100) */
    bool use_get_report_api;    /**< True - Using esp_wifi_ftm_get_report to get FTM report, False - Using ftm_report_data from
                                     WIFI_EVENT_FTM_REPORT to get FTM report */
} wifi_ftm_initiator_cfg_t;

#define ESP_WIFI_NAN_MAX_SVC_SUPPORTED  2      /**< Maximum number of NAN services supported */
#define ESP_WIFI_NAN_DATAPATH_MAX_PEERS 2      /**< Maximum number of NAN datapath peers supported */

#define ESP_WIFI_NDP_ROLE_INITIATOR     1      /**< Initiator role for NAN Data Path */
#define ESP_WIFI_NDP_ROLE_RESPONDER     2      /**< Responder role for NAN Data Path */

#define ESP_WIFI_MAX_SVC_NAME_LEN       256    /**< Maximum length of NAN service name */
#define ESP_WIFI_MAX_FILTER_LEN         256    /**< Maximum length of NAN service filter */
#define ESP_WIFI_MAX_SVC_INFO_LEN       64     /**< Maximum length of NAN service info */
#define ESP_WIFI_MAX_FUP_SSI_LEN        2048   /**< Maximum length of NAN Service Specific Info in a Follow-up frame */
#define ESP_WIFI_MAX_SVC_SSI_LEN        512    /**< Maximum length of NAN Service Specific Info in Publish/Subscribe SDF's */
#define ESP_WIFI_MAX_NEIGHBOR_REP_LEN   64     /**< Maximum length of NAN Neighbor Report */
#define WIFI_OUI_LEN                    3      /**< Length of OUI bytes in IE or attributes */

/**
  * @brief Protocol types in NAN service specific info attribute
  *
  */
typedef enum {
    WIFI_SVC_PROTO_RESERVED     = 0,    /**< Value 0 Reserved */
    WIFI_SVC_PROTO_BONJOUR      = 1,    /**< Bonjour Protocol */
    WIFI_SVC_PROTO_GENERIC      = 2,    /**< Generic Service Protocol */
    WIFI_SVC_PROTO_CSA_MATTER   = 3,    /**< CSA Matter specific protocol */
    WIFI_SVC_PROTO_MAX,                 /**< Values 4-255 Reserved */
} wifi_nan_svc_proto_t;

/**
  * @brief WFA defined Protocol types in NAN service specific info attribute
  *
  */
typedef struct {
    uint8_t wfa_oui[WIFI_OUI_LEN];  /**< WFA OUI - 0x50, 0x6F, 0x9A */
    wifi_nan_svc_proto_t proto;     /**< WFA defined protocol types */
    uint8_t payload[0];             /**< Service Info payload */
} wifi_nan_wfa_ssi_t;

/**
  * @brief NAN Services types
  *
  */
typedef enum {
    NAN_PUBLISH_SOLICITED,  /**< Send unicast Publish frame to Subscribers that match the requirement */
    NAN_PUBLISH_UNSOLICITED,/**< Send broadcast Publish frames in every Discovery Window(DW) */
    NAN_SUBSCRIBE_ACTIVE,   /**< Send broadcast Subscribe frames in every DW */
    NAN_SUBSCRIBE_PASSIVE,  /**< Passively listens to Publish frames */
} wifi_nan_service_type_t;

/**
  * @brief NAN Publish service configuration parameters
  *
  */
typedef struct {
    char service_name[ESP_WIFI_MAX_SVC_NAME_LEN];   /**< Service name identifier */
    wifi_nan_service_type_t type;                   /**< Service type */
    char matching_filter[ESP_WIFI_MAX_FILTER_LEN];  /**< Comma separated filters for filtering services */
    char svc_info[ESP_WIFI_MAX_SVC_INFO_LEN];       /**< To be deprecated in next major release, use ssi instead */
    uint8_t single_replied_event: 1;                /**< Give single Replied event or every time */
    uint8_t datapath_reqd: 1;                       /**< NAN Datapath required for the service */
    uint8_t fsd_reqd: 1;                            /**< Further Service Discovery(FSD) required */
    uint8_t fsd_gas: 1;                             /**< 0 - Follow-up used for FSD, 1 - GAS used for FSD */
    uint8_t reserved: 4;                            /**< Reserved */
    uint16_t ssi_len;                               /**< Length of service specific info, maximum allowed length - ESP_WIFI_MAX_SVC_SSI_LEN */
    uint8_t *ssi;                                   /**< Service Specific Info of type wifi_nan_wfa_ssi_t for WFA defined protocols, otherwise proprietary and defined by Applications */
} wifi_nan_publish_cfg_t;

/**
  * @brief NAN Subscribe service configuration parameters
  *
  */
typedef struct {
    char service_name[ESP_WIFI_MAX_SVC_NAME_LEN];   /**< Service name identifier */
    wifi_nan_service_type_t type;                   /**< Service type */
    char matching_filter[ESP_WIFI_MAX_FILTER_LEN];  /**< Comma separated filters for filtering services */
    char svc_info[ESP_WIFI_MAX_SVC_INFO_LEN];       /**< To be deprecated in next major release, use ssi instead */
    uint8_t single_match_event: 1;                  /**< Give single Match event(per SSI update)  or every time */
    uint8_t datapath_reqd: 1;                       /**< NAN Datapath required for the service */
    uint8_t fsd_reqd: 1;                            /**< Further Service Discovery(FSD) required */
    uint8_t fsd_gas: 1;                             /**< 0 - Follow-up used for FSD, 1 - GAS used for FSD */
    uint8_t reserved: 4;                            /**< Reserved */
    uint16_t ssi_len;                               /**< Length of service specific info, maximum allowed length - ESP_WIFI_MAX_SVC_SSI_LEN */
    uint8_t *ssi;                                   /**< Service Specific Info of type wifi_nan_wfa_ssi_t for WFA defined protocols, otherwise proprietary and defined by Applications */
} wifi_nan_subscribe_cfg_t;

/**
  * @brief NAN Follow-up parameters
  *
  */
typedef struct {
    uint8_t inst_id;                                /**< Own service instance id */
    uint8_t peer_inst_id;                           /**< Peer's service instance id */
    uint8_t peer_mac[6];                            /**< Peer's MAC address */
    char svc_info[ESP_WIFI_MAX_SVC_INFO_LEN];       /**< To be deprecated in next major release, use ssi instead */
    uint16_t ssi_len;                               /**< Length of service specific info, maximum allowed length - ESP_WIFI_MAX_FUP_SSI_LEN */
    uint8_t *ssi;                                   /**< Service Specific Info of type wifi_nan_wfa_ssi_t for WFA defined protocols, otherwise proprietary and defined by Applications */
} wifi_nan_followup_params_t;

/**
  * @brief NAN Datapath Request parameters
  *
  */
typedef struct {
    uint8_t pub_id;         /**< Publisher's service instance id */
    uint8_t peer_mac[6];    /**< Peer's MAC address */
    bool confirm_required;  /**< NDP Confirm frame required */
} wifi_nan_datapath_req_t;

/**
  * @brief NAN Datapath Response parameters
  *
  */
typedef struct {
    bool accept;            /**< True - Accept incoming NDP, False - Reject it */
    uint8_t ndp_id;         /**< NAN Datapath Identifier */
    uint8_t peer_mac[6];    /**< Peer's MAC address */
} wifi_nan_datapath_resp_t;

/**
  * @brief NAN Datapath End parameters
  *
  */
typedef struct {
    uint8_t ndp_id;         /**< NAN Datapath Identifier */
    uint8_t peer_mac[6];    /**< Peer's MAC address */
} wifi_nan_datapath_end_req_t;

/**
  * @brief Wi-Fi PHY rate encodings
  *
  * @note Rate Table: MCS Rate and Guard Interval Information
  * |       MCS RATE              |          HT20           |          HT40           |          HE20           |         VHT20           |
  * |-----------------------------|-------------------------|-------------------------|-------------------------|-------------------------|
  * | WIFI_PHY_RATE_MCS0_LGI      |     6.5 Mbps (800 ns)   |    13.5 Mbps (800 ns)   |     8.1 Mbps (1600 ns)  |     6.5 Mbps (800 ns)   |
  * | WIFI_PHY_RATE_MCS1_LGI      |      13 Mbps (800 ns)   |      27 Mbps (800 ns)   |    16.3 Mbps (1600 ns)  |      13 Mbps (800 ns)   |
  * | WIFI_PHY_RATE_MCS2_LGI      |    19.5 Mbps (800 ns)   |    40.5 Mbps (800 ns)   |    24.4 Mbps (1600 ns)  |    19.5 Mbps (800 ns)   |
  * | WIFI_PHY_RATE_MCS3_LGI      |      26 Mbps (800 ns)   |      54 Mbps (800 ns)   |    32.5 Mbps (1600 ns)  |      26 Mbps (800 ns)   |
  * | WIFI_PHY_RATE_MCS4_LGI      |      39 Mbps (800 ns)   |      81 Mbps (800 ns)   |    48.8 Mbps (1600 ns)  |      39 Mbps (800 ns)   |
  * | WIFI_PHY_RATE_MCS5_LGI      |      52 Mbps (800 ns)   |     108 Mbps (800 ns)   |      65 Mbps (1600 ns)  |      52 Mbps (800 ns)   |
  * | WIFI_PHY_RATE_MCS6_LGI      |    58.5 Mbps (800 ns)   |   121.5 Mbps (800 ns)   |    73.1 Mbps (1600 ns)  |    58.5 Mbps (800 ns)   |
  * | WIFI_PHY_RATE_MCS7_LGI      |      65 Mbps (800 ns)   |     135 Mbps (800 ns)   |    81.3 Mbps (1600 ns)  |      65 Mbps (800 ns)   |
  * | WIFI_PHY_RATE_MCS8_LGI      |                         |                         |    97.5 Mbps (1600 ns)  |                         |
  * | WIFI_PHY_RATE_MCS9_LGI      |                         |                         |   108.3 Mbps (1600 ns)  |                         |
  *
  * @note
  * |       MCS RATE              |          HT20           |          HT40           |          HE20           |         VHT20           |
  * |-----------------------------|-------------------------|-------------------------|-------------------------|-------------------------|
  * | WIFI_PHY_RATE_MCS0_SGI      |     7.2 Mbps (400 ns)   |      15 Mbps (400 ns)   |      8.6 Mbps (800 ns)  |     7.2 Mbps (400 ns)   |
  * | WIFI_PHY_RATE_MCS1_SGI      |    14.4 Mbps (400 ns)   |      30 Mbps (400 ns)   |     17.2 Mbps (800 ns)  |    14.4 Mbps (400 ns)   |
  * | WIFI_PHY_RATE_MCS2_SGI      |    21.7 Mbps (400 ns)   |      45 Mbps (400 ns)   |     25.8 Mbps (800 ns)  |    21.7 Mbps (400 ns)   |
  * | WIFI_PHY_RATE_MCS3_SGI      |    28.9 Mbps (400 ns)   |      60 Mbps (400 ns)   |     34.4 Mbps (800 ns)  |    28.9 Mbps (400 ns)   |
  * | WIFI_PHY_RATE_MCS4_SGI      |    43.3 Mbps (400 ns)   |      90 Mbps (400 ns)   |     51.6 Mbps (800 ns)  |    43.3 Mbps (400 ns)   |
  * | WIFI_PHY_RATE_MCS5_SGI      |    57.8 Mbps (400 ns)   |     120 Mbps (400 ns)   |     68.8 Mbps (800 ns)  |    57.8 Mbps (400 ns)   |
  * | WIFI_PHY_RATE_MCS6_SGI      |      65 Mbps (400 ns)   |     135 Mbps (400 ns)   |     77.4 Mbps (800 ns)  |      65 Mbps (400 ns)   |
  * | WIFI_PHY_RATE_MCS7_SGI      |    72.2 Mbps (400 ns)   |     150 Mbps (400 ns)   |       86 Mbps (800 ns)  |    72.2 Mbps (400 ns)   |
  * | WIFI_PHY_RATE_MCS8_SGI      |                         |                         |    103.2 Mbps (800 ns)  |                         |
  * | WIFI_PHY_RATE_MCS9_SGI      |                         |                         |    114.7 Mbps (800 ns)  |                         |
  *
  */
typedef enum {
    WIFI_PHY_RATE_1M_L      = 0x00, /**< 1 Mbps with long preamble */
    WIFI_PHY_RATE_2M_L      = 0x01, /**< 2 Mbps with long preamble */
    WIFI_PHY_RATE_5M_L      = 0x02, /**< 5.5 Mbps with long preamble */
    WIFI_PHY_RATE_11M_L     = 0x03, /**< 11 Mbps with long preamble */
    WIFI_PHY_RATE_2M_S      = 0x05, /**< 2 Mbps with short preamble */
    WIFI_PHY_RATE_5M_S      = 0x06, /**< 5.5 Mbps with short preamble */
    WIFI_PHY_RATE_11M_S     = 0x07, /**< 11 Mbps with short preamble */
    WIFI_PHY_RATE_48M       = 0x08, /**< 48 Mbps */
    WIFI_PHY_RATE_24M       = 0x09, /**< 24 Mbps */
    WIFI_PHY_RATE_12M       = 0x0A, /**< 12 Mbps */
    WIFI_PHY_RATE_6M        = 0x0B, /**< 6 Mbps */
    WIFI_PHY_RATE_54M       = 0x0C, /**< 54 Mbps */
    WIFI_PHY_RATE_36M       = 0x0D, /**< 36 Mbps */
    WIFI_PHY_RATE_18M       = 0x0E, /**< 18 Mbps */
    WIFI_PHY_RATE_9M        = 0x0F, /**< 9 Mbps */

    WIFI_PHY_RATE_MCS0_LGI  = 0x10, /**< MCS0 with long GI */
    WIFI_PHY_RATE_MCS1_LGI  = 0x11, /**< MCS1 with long GI */
    WIFI_PHY_RATE_MCS2_LGI  = 0x12, /**< MCS2 with long GI */
    WIFI_PHY_RATE_MCS3_LGI  = 0x13, /**< MCS3 with long GI */
    WIFI_PHY_RATE_MCS4_LGI  = 0x14, /**< MCS4 with long GI */
    WIFI_PHY_RATE_MCS5_LGI  = 0x15, /**< MCS5 with long GI */
    WIFI_PHY_RATE_MCS6_LGI  = 0x16, /**< MCS6 with long GI */
    WIFI_PHY_RATE_MCS7_LGI  = 0x17, /**< MCS7 with long GI */
#if CONFIG_SOC_WIFI_HE_SUPPORT || !CONFIG_SOC_WIFI_SUPPORTED
    WIFI_PHY_RATE_MCS8_LGI,         /**< MCS8 with long GI */
    WIFI_PHY_RATE_MCS9_LGI,         /**< MCS9 with long GI */
#endif

    WIFI_PHY_RATE_MCS0_SGI,         /**< MCS0 with short GI */
    WIFI_PHY_RATE_MCS1_SGI,         /**< MCS1 with short GI */
    WIFI_PHY_RATE_MCS2_SGI,         /**< MCS2 with short GI */
    WIFI_PHY_RATE_MCS3_SGI,         /**< MCS3 with short GI */
    WIFI_PHY_RATE_MCS4_SGI,         /**< MCS4 with short GI */
    WIFI_PHY_RATE_MCS5_SGI,         /**< MCS5 with short GI */
    WIFI_PHY_RATE_MCS6_SGI,         /**< MCS6 with short GI */
    WIFI_PHY_RATE_MCS7_SGI,         /**< MCS7 with short GI */
#if CONFIG_SOC_WIFI_HE_SUPPORT || !CONFIG_SOC_WIFI_SUPPORTED
    WIFI_PHY_RATE_MCS8_SGI,         /**< MCS8 with short GI */
    WIFI_PHY_RATE_MCS9_SGI,         /**< MCS9 with short GI */
#endif
    WIFI_PHY_RATE_LORA_250K = 0x29, /**< Espressif-specific Long Range mode rate, 250 Kbps */
    WIFI_PHY_RATE_LORA_500K = 0x2A, /**< Espressif-specific Long Range mode rate, 500 Kbps */
    WIFI_PHY_RATE_MAX,
} wifi_phy_rate_t;

/**
  * @brief Wi-Fi event declarations
  */
typedef enum {
    WIFI_EVENT_WIFI_READY = 0,           /**< Wi-Fi ready */
    WIFI_EVENT_SCAN_DONE,                /**< Finished scanning AP */
    WIFI_EVENT_STA_START,                /**< Station start */
    WIFI_EVENT_STA_STOP,                 /**< Station stop */
    WIFI_EVENT_STA_CONNECTED,            /**< Station connected to AP */
    WIFI_EVENT_STA_DISCONNECTED,         /**< Station disconnected from AP */
    WIFI_EVENT_STA_AUTHMODE_CHANGE,      /**< The auth mode of AP connected by device's station changed */

    WIFI_EVENT_STA_WPS_ER_SUCCESS,       /**< Station WPS succeeds in enrollee mode */
    WIFI_EVENT_STA_WPS_ER_FAILED,        /**< Station WPS fails in enrollee mode */
    WIFI_EVENT_STA_WPS_ER_TIMEOUT,       /**< Station WPS timeout in enrollee mode */
    WIFI_EVENT_STA_WPS_ER_PIN,           /**< Station WPS pin code in enrollee mode */
    WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP,   /**< Station WPS overlap in enrollee mode */

    WIFI_EVENT_AP_START,                 /**< Soft-AP start */
    WIFI_EVENT_AP_STOP,                  /**< Soft-AP stop */
    WIFI_EVENT_AP_STACONNECTED,          /**< A station connected to Soft-AP */
    WIFI_EVENT_AP_STADISCONNECTED,       /**< A station disconnected from Soft-AP */
    WIFI_EVENT_AP_PROBEREQRECVED,        /**< Receive probe request packet in soft-AP interface */

    WIFI_EVENT_FTM_REPORT,               /**< Receive report of FTM procedure */

    /* Add next events after this only */
    WIFI_EVENT_STA_BSS_RSSI_LOW,         /**< AP's RSSI crossed configured threshold */
    WIFI_EVENT_ACTION_TX_STATUS,         /**< Status indication of Action Tx operation */
    WIFI_EVENT_ROC_DONE,                 /**< Remain-on-Channel operation complete */

    WIFI_EVENT_STA_BEACON_TIMEOUT,       /**< Station beacon timeout */

    WIFI_EVENT_CONNECTIONLESS_MODULE_WAKE_INTERVAL_START,   /**< Connectionless module wake interval start */
    /* Add next events after this only */

    WIFI_EVENT_AP_WPS_RG_SUCCESS,       /**< Soft-AP wps succeeds in registrar mode */
    WIFI_EVENT_AP_WPS_RG_FAILED,        /**< Soft-AP wps fails in registrar mode */
    WIFI_EVENT_AP_WPS_RG_TIMEOUT,       /**< Soft-AP wps timeout in registrar mode */
    WIFI_EVENT_AP_WPS_RG_PIN,           /**< Soft-AP wps pin code in registrar mode */
    WIFI_EVENT_AP_WPS_RG_PBC_OVERLAP,   /**< Soft-AP wps overlap in registrar mode */

    WIFI_EVENT_ITWT_SETUP,              /**< iTWT setup */
    WIFI_EVENT_ITWT_TEARDOWN,           /**< iTWT teardown */
    WIFI_EVENT_ITWT_PROBE,              /**< iTWT probe */
    WIFI_EVENT_ITWT_SUSPEND,            /**< iTWT suspend */
    WIFI_EVENT_TWT_WAKEUP,              /**< TWT wakeup */
    WIFI_EVENT_BTWT_SETUP,              /**< bTWT setup */
    WIFI_EVENT_BTWT_TEARDOWN,           /**< bTWT teardown*/

    WIFI_EVENT_NAN_STARTED,              /**< NAN Discovery has started */
    WIFI_EVENT_NAN_STOPPED,              /**< NAN Discovery has stopped */
    WIFI_EVENT_NAN_SVC_MATCH,            /**< NAN Service Discovery match found */
    WIFI_EVENT_NAN_REPLIED,              /**< Replied to a NAN peer with Service Discovery match */
    WIFI_EVENT_NAN_RECEIVE,              /**< Received a Follow-up message */
    WIFI_EVENT_NDP_INDICATION,           /**< Received NDP Request from a NAN Peer */
    WIFI_EVENT_NDP_CONFIRM,              /**< NDP Confirm Indication */
    WIFI_EVENT_NDP_TERMINATED,           /**< NAN Datapath terminated indication */
    WIFI_EVENT_HOME_CHANNEL_CHANGE,      /**< Wi-Fi home channel change，doesn't occur when scanning */

    WIFI_EVENT_STA_NEIGHBOR_REP,         /**< Received Neighbor Report response */

    WIFI_EVENT_AP_WRONG_PASSWORD,        /**< a station tried to connect with wrong password */

    WIFI_EVENT_STA_BEACON_OFFSET_UNSTABLE,  /**< Station sampled beacon offset unstable */
    WIFI_EVENT_MAX,                      /**< Invalid Wi-Fi event ID */
} wifi_event_t;

/** @cond **/
/** @brief Wi-Fi event base declaration */
ESP_EVENT_DECLARE_BASE(WIFI_EVENT);
/** @endcond **/

/**
  * @brief Argument structure for WIFI_EVENT_SCAN_DONE event
  */
typedef struct {
    uint32_t status;          /**< Status of scanning APs: 0 — success, 1 - failure */
    uint8_t  number;          /**< Number of scan results */
    uint8_t  scan_id;         /**< Scan sequence number, used for block scan */
} wifi_event_sta_scan_done_t;

/**
  * @brief Argument structure for WIFI_EVENT_STA_CONNECTED event
  */
typedef struct {
    uint8_t ssid[32];         /**< SSID of connected AP */
    uint8_t ssid_len;         /**< SSID length of connected AP */
    uint8_t bssid[6];         /**< BSSID of connected AP*/
    uint8_t channel;          /**< Channel of connected AP*/
    wifi_auth_mode_t authmode;/**< Authentication mode used by the connection*/
    uint16_t aid;             /**< Authentication id assigned by the connected AP */
} wifi_event_sta_connected_t;

/**
  * @brief Argument structure for WIFI_EVENT_STA_DISCONNECTED event
  */
typedef struct {
    uint8_t ssid[32];         /**< SSID of disconnected AP */
    uint8_t ssid_len;         /**< SSID length of disconnected AP */
    uint8_t bssid[6];         /**< BSSID of disconnected AP */
    uint8_t reason;           /**< Disconnection reason */
    int8_t  rssi;             /**< Disconnection RSSI */
} wifi_event_sta_disconnected_t;

/**
  * @brief Argument structure for WIFI_EVENT_STA_AUTHMODE_CHANGE event
  */
typedef struct {
    wifi_auth_mode_t old_mode;         /**< Old auth mode of AP */
    wifi_auth_mode_t new_mode;         /**< New auth mode of AP */
} wifi_event_sta_authmode_change_t;

/**
  * @brief Argument structure for WIFI_EVENT_STA_WPS_ER_PIN event
  */
typedef struct {
    uint8_t pin_code[8];         /**< PIN code of station in enrollee mode */
} wifi_event_sta_wps_er_pin_t;

/**
  * @brief Argument structure for WIFI_EVENT_STA_WPS_ER_FAILED event
  */
typedef enum {
    WPS_FAIL_REASON_NORMAL = 0,     /**< WPS normal fail reason */
    WPS_FAIL_REASON_RECV_M2D,       /**< WPS receive M2D frame */
    WPS_FAIL_REASON_RECV_DEAUTH,    /**< Recv deauth from AP while wps handshake */
    WPS_FAIL_REASON_MAX             /**< Max WPS fail reason */
} wifi_event_sta_wps_fail_reason_t;

#define MAX_SSID_LEN        32    /**< Maximum length of SSID */
#define MAX_PASSPHRASE_LEN  64    /**< Maximum length of passphrase */
#define MAX_WPS_AP_CRED     3     /**< Maximum number of AP credentials received from WPS handshake */

/**
  * @brief Argument structure for WIFI_EVENT_STA_WPS_ER_SUCCESS event
  */
typedef struct {
    uint8_t ap_cred_cnt;                        /**< Number of AP credentials received */
    struct {
        uint8_t ssid[MAX_SSID_LEN];             /**< SSID of AP */
        uint8_t passphrase[MAX_PASSPHRASE_LEN]; /**< Passphrase for the AP */
    } ap_cred[MAX_WPS_AP_CRED];                 /**< All AP credentials received from WPS handshake */
} wifi_event_sta_wps_er_success_t;

/**
  * @brief Argument structure for WIFI_EVENT_AP_STACONNECTED event
  */
typedef struct {
    uint8_t mac[6];           /**< MAC address of the station connected to Soft-AP */
    uint8_t aid;              /**< AID assigned by the Soft-AP to the connected station  */
    bool is_mesh_child;       /**< Flag indicating whether the connected station is a mesh child */
} wifi_event_ap_staconnected_t;

/**
  * @brief Argument structure for WIFI_EVENT_AP_STADISCONNECTED event
  */
typedef struct {
    uint8_t mac[6];           /**< MAC address of the station disconnects from the soft-AP */
    uint8_t aid;              /**< AID that the Soft-AP assigned to the disconnected station  */
    bool is_mesh_child;       /**< Flag indicating whether the disconnected station is a mesh child */
    uint16_t reason;           /**< Disconnection reason */
} wifi_event_ap_stadisconnected_t;

/**
  * @brief Argument structure for WIFI_EVENT_AP_PROBEREQRECVED event
  */
typedef struct {
    int rssi;                 /**< Received probe request signal strength */
    uint8_t mac[6];           /**< MAC address of the station which send probe request */
} wifi_event_ap_probe_req_rx_t;

/**
  * @brief Argument structure for WIFI_EVENT_STA_BSS_RSSI_LOW event
  */
typedef struct {
    int32_t rssi;                 /**< RSSI value of bss */
} wifi_event_bss_rssi_low_t;

/**
  * @brief Argument structure for WIFI_EVENT_HOME_CHANNEL_CHANGE event
  */
typedef struct {
    uint8_t            old_chan;   /**< Old home channel of the device */
    wifi_second_chan_t old_snd;    /**< Old second channel of the device */
    uint8_t            new_chan;   /**< New home channel of the device */
    wifi_second_chan_t new_snd;    /**< New second channel of the device */
} wifi_event_home_channel_change_t;

/**
  * @brief FTM operation status types
  *
  */
typedef enum {
    FTM_STATUS_SUCCESS = 0,     /**< FTM exchange is successful */
    FTM_STATUS_UNSUPPORTED,     /**< Peer does not support FTM */
    FTM_STATUS_CONF_REJECTED,   /**< Peer rejected FTM configuration in FTM Request */
    FTM_STATUS_NO_RESPONSE,     /**< Peer did not respond to FTM Requests */
    FTM_STATUS_FAIL,            /**< Unknown error during FTM exchange */
    FTM_STATUS_NO_VALID_MSMT,   /**< FTM session did not result in any valid measurements */
    FTM_STATUS_USER_TERM,       /**< User triggered termination */
} wifi_ftm_status_t;

/**
  * @brief Structure representing a report entry for Fine Timing Measurement (FTM) in Wi-Fi.
  *
  * This structure holds the information related to the FTM process between a Wi-Fi FTM Initiator
  * and a Wi-Fi FTM Responder. FTM is used for precise distance measurement by timing the exchange
  * of frames between devices.
  */
typedef struct {
    uint8_t dlog_token;     /**< Dialog Token of the FTM frame */
    int8_t rssi;            /**< RSSI of the FTM frame received */
    uint32_t rtt;           /**< Round Trip Time in pSec with a peer */
    uint64_t t1;            /**< Time of departure of FTM frame from FTM Responder in pSec */
    uint64_t t2;            /**< Time of arrival of FTM frame at FTM Initiator in pSec */
    uint64_t t3;            /**< Time of departure of ACK from FTM Initiator in pSec */
    uint64_t t4;            /**< Time of arrival of ACK at FTM Responder in pSec */
} wifi_ftm_report_entry_t;

/**
  * @brief Argument structure for WIFI_EVENT_FTM_REPORT event
  */
typedef struct {
    uint8_t peer_mac[6];                        /**< MAC address of the FTM Peer */
    wifi_ftm_status_t status;                   /**< Status of the FTM operation */
    uint32_t rtt_raw;                           /**< Raw average Round-Trip-Time with peer in Nano-Seconds */
    uint32_t rtt_est;                           /**< Estimated Round-Trip-Time with peer in Nano-Seconds */
    uint32_t dist_est;                          /**< Estimated one-way distance in Centi-Meters */
    wifi_ftm_report_entry_t *ftm_report_data;   /**< Pointer to FTM Report, should be freed after use. Note: Highly recommended
                                                     to use API esp_wifi_ftm_get_report to get the report instead of using this */
    uint8_t ftm_report_num_entries;             /**< Number of entries in the FTM Report data */
} wifi_event_ftm_report_t;

#define WIFI_STATIS_BUFFER    (1<<0)    /**< Buffer status */
#define WIFI_STATIS_RXTX      (1<<1)    /**< RX/TX status */
#define WIFI_STATIS_HW        (1<<2)    /**< Hardware status */
#define WIFI_STATIS_DIAG      (1<<3)    /**< Diagnostic status */
#define WIFI_STATIS_PS        (1<<4)    /**< Power save status */
#define WIFI_STATIS_ALL       (-1)      /**< All status */

/** Status codes for WIFI_EVENT_ACTION_TX_STATUS evt */
/** There will be back to back events in success case TX_DONE and TX_DURATION_COMPLETED */
typedef enum {
    WIFI_ACTION_TX_DONE = 0,           /**< ACTION_TX operation was completed successfully */
    WIFI_ACTION_TX_FAILED,             /**< ACTION_TX operation failed during tx */
    WIFI_ACTION_TX_DURATION_COMPLETED, /**< ACTION_TX operation completed it's wait duration */
    WIFI_ACTION_TX_OP_CANCELLED,       /**< ACTION_TX operation was cancelled by application or higher priority operation */
} wifi_action_tx_status_type_t;

/** Argument structure for WIFI_EVENT_ACTION_TX_STATUS event */
typedef struct {
    wifi_interface_t ifx;                   /**< WiFi interface to send request to */
    uint32_t context;                       /**< Context to identify the request */
    wifi_action_tx_status_type_t status;    /**< Status of the operation */
    uint8_t op_id;                          /**< ID of the corresponding operation that was provided during action tx request */
    uint8_t channel;                        /**< Channel provided in tx request */
} wifi_event_action_tx_status_t;

/**
  * @brief Argument structure for WIFI_EVENT_ROC_DONE event
  */
typedef struct {
    uint32_t context;               /**< Context to identify the initiator of the request */
    wifi_roc_done_status_t status;  /**< ROC status */
    uint8_t op_id;                  /**< ID of the corresponding ROC operation */
    uint8_t channel;                /**< Channel provided in tx request */
} wifi_event_roc_done_t;

/**
  * @brief Argument structure for WIFI_EVENT_AP_WPS_RG_PIN event
  */
typedef struct {
    uint8_t pin_code[8];         /**< PIN code of station in enrollee mode */
} wifi_event_ap_wps_rg_pin_t;

/**
  * @brief WPS fail reason
  */
typedef enum {
    WPS_AP_FAIL_REASON_NORMAL = 0,     /**< WPS normal fail reason */
    WPS_AP_FAIL_REASON_CONFIG,         /**< WPS failed due to incorrect config */
    WPS_AP_FAIL_REASON_AUTH,           /**< WPS failed during auth */
    WPS_AP_FAIL_REASON_MAX,            /**< Max WPS fail reason */
} wps_fail_reason_t;

/**
  * @brief Argument structure for WIFI_EVENT_AP_WPS_RG_FAILED event
  */
typedef struct {
    wps_fail_reason_t reason;          /**< WPS failure reason wps_fail_reason_t */
    uint8_t peer_macaddr[6];           /**< Enrollee mac address */
} wifi_event_ap_wps_rg_fail_reason_t;

/**
  * @brief Argument structure for WIFI_EVENT_AP_WPS_RG_SUCCESS event
  */
typedef struct {
    uint8_t peer_macaddr[6];           /**< Enrollee mac address */
} wifi_event_ap_wps_rg_success_t;

/**
  * @brief Argument structure for WIFI_EVENT_NAN_SVC_MATCH event
  */
typedef struct {
    uint8_t subscribe_id;       /**< Subscribe Service Identifier */
    uint8_t publish_id;         /**< Publish Service Identifier */
    uint8_t pub_if_mac[6];      /**< NAN Interface MAC of the Publisher */
    bool update_pub_id;         /**< Indicates whether publisher's service ID needs to be updated */
    uint8_t datapath_reqd: 1;   /**< NAN Datapath required for the service */
    uint8_t fsd_reqd: 1;        /**< Further Service Discovery(FSD) required */
    uint8_t fsd_gas: 1;         /**< 0 - Follow-up used for FSD, 1 - GAS used for FSD */
    uint8_t reserved: 5;        /**< Reserved */
    uint32_t reserved_1;        /**< Reserved */
    uint32_t reserved_2;        /**< Reserved */
    uint8_t ssi_version;        /**< Indicates version of SSI in Publish instance, 0 if not available */
    uint16_t ssi_len;           /**< Length of service specific info */
    uint8_t ssi[];              /**< Service specific info of Publisher */
} wifi_event_nan_svc_match_t;

/**
  * @brief Argument structure for WIFI_EVENT_NAN_REPLIED event
  */
typedef struct {
    uint8_t publish_id;         /**< Publish Service Identifier */
    uint8_t subscribe_id;       /**< Subscribe Service Identifier */
    uint8_t sub_if_mac[6];      /**< NAN Interface MAC of the Subscriber */
    uint32_t reserved_1;        /**< Reserved */
    uint32_t reserved_2;        /**< Reserved */
    uint16_t ssi_len;           /**< Length of service specific info */
    uint8_t ssi[];              /**< Service specific info of Subscriber */
} wifi_event_nan_replied_t;

/**
  * @brief Argument structure for WIFI_EVENT_NAN_RECEIVE event
  */
typedef struct {
    uint8_t inst_id;                                 /**< Our Service Identifier */
    uint8_t peer_inst_id;                            /**< Peer's Service Identifier */
    uint8_t peer_if_mac[6];                          /**< Peer's NAN Interface MAC */
    uint8_t peer_svc_info[ESP_WIFI_MAX_SVC_INFO_LEN];/**< To be deprecated in next major release, use ssi instead */
    uint32_t reserved_1;                             /**< Reserved */
    uint32_t reserved_2;                             /**< Reserved */
    uint16_t ssi_len;                                /**< Length of service specific info */
    uint8_t ssi[];                                   /**< Service specific info from Follow-up */
} wifi_event_nan_receive_t;

/**
  * @brief Argument structure for WIFI_EVENT_NDP_INDICATION event
  */
typedef struct {
    uint8_t publish_id;                         /**< Publish Id for NAN Service */
    uint8_t ndp_id;                             /**< NDP instance id */
    uint8_t peer_nmi[6];                        /**< Peer's NAN Management Interface MAC */
    uint8_t peer_ndi[6];                        /**< Peer's NAN Data Interface MAC */
    uint8_t svc_info[ESP_WIFI_MAX_SVC_INFO_LEN];/**< To be deprecated in next major release, use ssi instead */
    uint32_t reserved_1;                        /**< Reserved */
    uint32_t reserved_2;                        /**< Reserved */
    uint16_t ssi_len;                           /**< Length of service specific info */
    uint8_t ssi[];                              /**< Service specific info from NDP/NDPE Attribute */
} wifi_event_ndp_indication_t;

/**
  * @brief Argument structure for WIFI_EVENT_NDP_CONFIRM event
  */
typedef struct {
    uint8_t status;                             /**< NDP status code */
    uint8_t ndp_id;                             /**< NDP instance id */
    uint8_t peer_nmi[6];                        /**< Peer's NAN Management Interface MAC */
    uint8_t peer_ndi[6];                        /**< Peer's NAN Data Interface MAC */
    uint8_t own_ndi[6];                         /**< Own NAN Data Interface MAC */
    uint8_t svc_info[ESP_WIFI_MAX_SVC_INFO_LEN];/**< To be deprecated in next major release, use ssi instead */
    uint32_t reserved_1;                        /**< Reserved */
    uint32_t reserved_2;                        /**< Reserved */
    uint16_t ssi_len;                           /**< Length of Service Specific Info */
    uint8_t ssi[];                              /**< Service specific info from NDP/NDPE Attribute */
} wifi_event_ndp_confirm_t;

/**
  * @brief Argument structure for WIFI_EVENT_NDP_TERMINATED event
  */
typedef struct {
    uint8_t reason;                             /**< Termination reason code */
    uint8_t ndp_id;                             /**< NDP instance id */
    uint8_t init_ndi[6];                        /**< Initiator's NAN Data Interface MAC */
} wifi_event_ndp_terminated_t;

/**
  * @brief Argument structure for WIFI_EVENT_STA_NEIGHBOR_REP event
  */
typedef struct {
    uint8_t report[ESP_WIFI_MAX_NEIGHBOR_REP_LEN];  /**< Neighbor Report received from the AP (will be deprecated in next major release, use n_report instead)*/
    uint16_t report_len;                            /**< Length of the report*/
    uint8_t n_report[];                             /**< Neighbor Report received from the AP*/
} wifi_event_neighbor_report_t;

/** Argument structure for WIFI_EVENT_AP_WRONG_PASSWORD event */
typedef struct {
    uint8_t mac[6];           /**< MAC address of the station trying to connect to Soft-AP */
} wifi_event_ap_wrong_password_t;

/**
  * @brief Argument structure for wifi_tx_rate_config
  */
typedef struct {
    wifi_phy_mode_t phymode;                 /**< Phymode of specified interface */
    wifi_phy_rate_t rate;                    /**< Rate of specified interface */
    bool ersu;                               /**< Using ERSU to send frame, ERSU is a transmission mode related to 802.11 ax.
                                                  ERSU is always used in long distance transmission, and its frame has lower rate compared with SU mode */
    bool dcm;                                /**< Using dcm rate to send frame */
} wifi_tx_rate_config_t;

#define WIFI_MAX_SUPPORT_COUNTRY_NUM 175 /**< max number of supported countries */
#ifdef CONFIG_SOC_WIFI_SUPPORT_5G
#define WIFI_MAX_REGULATORY_RULE_NUM  7 /**< max number of regulatory rules */
#else
#define WIFI_MAX_REGULATORY_RULE_NUM  2 /**< max number of regulatory rules */
#endif

/** Argument structure for regulatory rule */
typedef struct {
    uint8_t start_channel;      /**< start channel of regulatory rule */
    uint8_t end_channel;        /**< end channel of regulatory rule */
    uint16_t max_bandwidth : 3; /**< max bandwidth(MHz) of regulatory rule, 1:20M, 2:40M, 3:80M, 4:160M */
    uint16_t max_eirp : 6;      /**< indicates the maximum Equivalent Isotropically Radiated Power (EIRP), typically measured in dBm */
    uint16_t is_dfs : 1;        /**< flag to identify dfs channel */
    uint16_t reserved : 6;      /**< reserved */
} wifi_reg_rule_t;

/** Argument structure for regdomain */
typedef struct {
    uint8_t n_reg_rules;                                     /**< number of regulatory rules */
    wifi_reg_rule_t reg_rules[WIFI_MAX_REGULATORY_RULE_NUM]; /**< array of regulatory rules*/
} wifi_regulatory_t;

/** Argument structure for regdomain */
typedef struct {
    char cn[2];              /**< country code string */
    uint8_t regulatory_type; /**< regulatory type of country */
} wifi_regdomain_t;

/**
  * @brief Status of wifi sending data
  */
typedef enum {
    WIFI_SEND_SUCCESS = 0,    /**< Sending Wi-Fi data successfully */
    WIFI_SEND_FAIL,           /**< Sending Wi-Fi data fail */
} wifi_tx_status_t;

/**
  * @brief Information of wifi sending data
  */
typedef struct {
    uint8_t *des_addr;           /**< The address of the receive device */
    uint8_t *src_addr;           /**< The address of the sending device */
    wifi_interface_t ifidx;      /**< Interface of sending 80211 tx data */
    uint8_t *data;               /**< The data for 80211 tx, start from the MAC header */
    uint8_t data_len;            /**< The frame body length for 80211 tx, excluding the MAC header */
    wifi_phy_rate_t rate;        /**< Data rate */
    wifi_tx_status_t tx_status;  /**< Status of sending 80211 tx data */
} wifi_tx_info_t;

typedef wifi_tx_info_t esp_80211_tx_info_t;

/** Argument structure for WIFI_EVENT_STA_BEACON_OFFSET_UNSTABLE event */
typedef struct {
    float beacon_success_rate;                  /**< Received beacon success rate */
} wifi_event_sta_beacon_offset_unstable_t;

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_TYPES_H__ */
