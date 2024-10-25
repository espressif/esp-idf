/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLOW_ID_ALL                  (8)
#define BTWT_ID_ALL                  (32)
#define BSS_MAX_COLOR                (63)

/**
  * @brief Access category
  */
typedef enum {
    ESP_WIFI_ACI_VO,    /**< voice traffic */
    ESP_WIFI_ACI_VI,    /**< video traffic */
    ESP_WIFI_ACI_BE,    /**< best effort traffic */
    ESP_WIFI_ACI_BK,    /**< background traffic */
    ESP_WIFI_ACI_MAX,   /**< the max value */
} esp_wifi_aci_t;

/**
  * @brief Channel state information(CSI) HE STBC CSI selection
  */
enum {
    ESP_CSI_ACQUIRE_STBC_HELTF1,            /**< HE STBC: select the first HE-LTF */
    ESP_CSI_ACQUIRE_STBC_HELTF2,            /**< HE STBC: select the second HE-LTF */
    ESP_CSI_ACQUIRE_STBC_SAMPLE_HELTFS,     /**< HE STBC: sample alternating scarier of HE-LTF1 and HE-LTF2 */
};

/**
  * @brief Channel state information(CSI) configuration type
  */
#if CONFIG_SOC_WIFI_MAC_VERSION_NUM == 3
typedef struct {
    uint32_t enable                 : 1;    /**< enable to acquire CSI */
    uint32_t acquire_csi_legacy     : 1;    /**< enable to acquire L-LTF */
    uint32_t acquire_csi_force_lltf : 1;    /**< enable to acquire L-LTF */
    uint32_t acquire_csi_ht20       : 1;    /**< enable to acquire HT-LTF when receiving an HT20 PPDU */
    uint32_t acquire_csi_ht40       : 1;    /**< enable to acquire HT-LTF when receiving an HT40 PPDU */
    uint32_t acquire_csi_vht        : 1;    /**< enable to acquire VHT-LTF when receiving an VHT20 PPDU */
    uint32_t acquire_csi_su         : 1;    /**< enable to acquire HE-LTF when receiving an HE20 SU PPDU */
    uint32_t acquire_csi_mu         : 1;    /**< enable to acquire HE-LTF when receiving an HE20 MU PPDU */
    uint32_t acquire_csi_dcm        : 1;    /**< enable to acquire HE-LTF when receiving an HE20 DCM applied PPDU */
    uint32_t acquire_csi_beamformed : 1;    /**< enable to acquire HE-LTF when receiving an HE20 Beamformed applied PPDU */
    uint32_t acquire_csi_he_stbc_mode: 2;   /**< when receiving an STBC applied HE PPDU,
                                                 0- acquire the complete HE-LTF1
                                                 1- acquire the complete HE-LTF2
                                                 2- sample evenly among the HE-LTF1 and HE-LTF2 */

    uint32_t val_scale_cfg           : 4;   /**< value 0-8 */
    uint32_t dump_ack_en             : 1;   /**< enable to dump 802.11 ACK frame, default disabled */
    uint32_t reserved                : 15;  /**< reserved */
} wifi_csi_acquire_config_t;
#else
typedef struct {
    uint32_t enable : 1;                    /**< enable to acquire CSI */
    uint32_t acquire_csi_legacy : 1;        /**< enable to acquire L-LTF when receiving a 11g PPDU */
    uint32_t acquire_csi_ht20 : 1;          /**< enable to acquire HT-LTF when receiving an HT20 PPDU */
    uint32_t acquire_csi_ht40 : 1;          /**< enable to acquire HT-LTF when receiving an HT40 PPDU */
    uint32_t acquire_csi_su : 1;            /**< enable to acquire HE-LTF when receiving an HE20 SU PPDU */
    uint32_t acquire_csi_mu : 1;            /**< enable to acquire HE-LTF when receiving an HE20 MU PPDU */
    uint32_t acquire_csi_dcm : 1;           /**< enable to acquire HE-LTF when receiving an HE20 DCM applied PPDU */
    uint32_t acquire_csi_beamformed : 1;    /**< enable to acquire HE-LTF when receiving an HE20 Beamformed applied PPDU */
    uint32_t acquire_csi_he_stbc : 2;       /**< when receiving an STBC applied HE PPDU,
                                                 0- acquire the complete HE-LTF1
                                                 1- acquire the complete HE-LTF2
                                                 2- sample evenly among the HE-LTF1 and HE-LTF2 */
    uint32_t val_scale_cfg : 2;             /**< value 0-3 */
    uint32_t dump_ack_en : 1;               /**< enable to dump 802.11 ACK frame, default disabled */
    uint32_t reserved : 19;                 /**< reserved */
} wifi_csi_acquire_config_t;
#endif

/**
  * @brief HE variant HT Control field including OM(Operation mode)
  */
typedef struct {
    uint32_t id : 2;                                   /**< HE Variant ID = 3 */
    uint32_t ctrl_id : 4;                              /**< OM control ID: 1 */
    uint32_t rx_nss : 3;                               /**< the max. number of spatial streams for the reception, only accept 0. */
    uint32_t bw : 2;                                   /**< the operating channel width for both reception and transmission, only accept 0. */
    uint32_t ul_mu_disable : 1;                        /**< disable UL MU operations */
    uint32_t tx_nsts : 3;                              /**< the max. number of spatial streams for the transmission, only accept 0. */
    uint32_t er_su_disable : 1;                        /**< disable the reception of 242-tone HE ER SU PPDU */
    uint32_t dl_mu_mimo_resounding_recommendation : 1; /**< indicate the STA suggests the AP either resounding the channel or increase the channel sounding frequency with the STA */
    uint32_t ul_mu_data_disable : 1;                   /**< disable UL MU data operations */
    uint32_t padding : 14;                             /**< padding bits */
} esp_wifi_htc_omc_t;

/**
  * @brief TWT setup commands
  */
typedef enum {
    TWT_REQUEST,                   /**< request to join a TWT without providing a set of TWT parameters */
    TWT_SUGGEST,                   /**< request to join a TWT and offer a set of preferred TWT parameters but might accept alternative TWT parameters */
    TWT_DEMAND,                    /**< request to join a TWT and currently accept only the indicated TWT parameters */
    TWT_GROUPING,                  /**< for S1G STA */
    TWT_ACCEPT,                    /**< accept the TWT request with the TWT parameters, also used in unsolicited TWT response */
    TWT_ALTERNATE,                 /**< indicate a counter-offer of TWT parameters without creation of a TWT agreement */
    TWT_DICTATE,                   /**< indicate no TWT agreement is created, but one is likely to be accepted only if the requesting STA transmits a new TWT setup request with the indicated TWT parameters */
    TWT_REJECT,                    /**< indicate that the negotiation has ended in failure to create a new TWT agreement */
} wifi_twt_setup_cmds_t;

/**
  * @brief broadcast TWT setup config
  */
typedef struct {
    wifi_twt_setup_cmds_t setup_cmd;   /**< Indicates the type of TWT command*/
    uint8_t btwt_id;                  /**< When set up an broadcast TWT agreement, the broadcast twt id will be assigned by AP after a successful agreement setup.
                                         broadcast twt id could be specified to a value in the range of [1, 31], but it might be change by AP in the response.
                                         When change TWT parameters of the existing TWT agreement, broadcast twt id should be an existing one. The value range is [1, 31].*/
    uint16_t timeout_time_ms;         /**< Timeout times of receiving setup action frame response, default 5s*/
} wifi_btwt_setup_config_t;

/**
  * @brief Individual TWT setup config
  */
typedef struct {
    wifi_twt_setup_cmds_t setup_cmd;    /**< Indicates the type of TWT command */
    uint16_t trigger : 1;               /**< 1: a trigger-enabled individual TWT, 0: a non-trigger-enabled individual TWT */
    uint16_t flow_type : 1;             /**< 0: an announced individual TWT, 1: an unannounced individual TWT */
    uint16_t flow_id : 3;                /**< When set up an individual TWT agreement, the flow id will be assigned by AP after a successful agreement setup.
                                             flow_id could be specified to a value in the range of [0, 7], but it might be changed by AP in the response.
                                             When change TWT parameters of the existing TWT agreement, flow_id should be an existing one. The value range is [0, 7]. */
    uint16_t wake_invl_expn : 5;        /**< Individual TWT Wake Interval Exponent. The value range is [0, 31]. */
    uint16_t wake_duration_unit : 1;    /**< Individual TWT Wake duration unit, 0: 256us 1: TU (TU = 1024us)*/
    uint16_t reserved : 5;              /**< bit: 11.15 reserved */
    uint8_t min_wake_dura;              /**< Nominal Minimum Wake Duration, indicates the minimum amount of time, in unit of 256 us, that the individual TWT requesting STA expects that it needs to be awake. The value range is [1, 255]. */
    uint16_t wake_invl_mant;            /**< Individual TWT Wake Interval Mantissa. The value range is [1, 65535]. */
    uint16_t twt_id;                    /**< Individual TWT connection id, the value range is [0, 32767]. */
    uint16_t timeout_time_ms;           /**< Timeout times of receiving setup action frame response, default 5s*/
} wifi_twt_setup_config_t;

typedef wifi_twt_setup_config_t wifi_itwt_setup_config_t;

/**
  * @brief HE SU GI and LTF types
  */
typedef enum {
    HE_SU_ERSU_1_LTF_0_8_US_GI,    /**< 1 LTF and 0.8 us GI */
    HE_SU_ERSU_2_LTF_0_8_US_GI,    /**< 2 LTF and 0.8 us GI */
    HE_SU_ERSU_2_LTF_1_6_US_GI,    /**< 2 LTF and 1.6 us GI */
    HE_SU_ERSU_4_LTF_3_2_US_GI,    /**< 4 LTF and 3.2 us GI */
} he_su_gi_and_ltf_type_t;

/**
  * @brief Reception format
  */
typedef enum {
    RX_BB_FORMAT_11B      = 0,           /**< the reception frame is a 11b MPDU */
    RX_BB_FORMAT_11G      = 1,           /**< the reception frame is a 11g MPDU */
    RX_BB_FORMAT_11A = RX_BB_FORMAT_11G, /**< the reception frame is a 11a MPDU */
    RX_BB_FORMAT_HT       = 2,           /**< the reception frame is a HT MPDU */
    RX_BB_FORMAT_VHT      = 3,           /**< the reception frame is a VHT MPDU */
    RX_BB_FORMAT_HE_SU    = 4,           /**< the reception frame is a HE SU MPDU */
    RX_BB_FORMAT_HE_MU    = 5,           /**< the reception frame is a HE MU MPDU */
    RX_BB_FORMAT_HE_ERSU  = 6,           /**< the reception frame is a HE ER SU MPDU */
    RX_BB_FORMAT_HE_TB    = 7,           /**< the reception frame is a HE TB MPDU */
} wifi_rx_bb_format_t;

/**
  * @brief RxControl Info
  */
#if CONFIG_SOC_WIFI_MAC_VERSION_NUM == 3
typedef struct {
    signed rssi: 8;                               /**< the RSSI of the reception frame */
    unsigned rate: 5;                             /**< if cur_bb_format is RX_BB_FORMAT_11B, it's the transmission rate. otherwise it's Rate field of L-SIG */
    unsigned : 1;                                 /**< reserved */
    unsigned : 2;                                 /**< reserved */
    unsigned : 12;                                /**< reserved */
    unsigned rxmatch0: 1;                         /**< indicate whether the reception frame is from interface 0 */
    unsigned rxmatch1: 1;                         /**< indicate whether the reception frame is from interface 1 */
    unsigned rxmatch2: 1;                         /**< indicate whether the reception frame is from interface 2 */
    unsigned rxmatch3: 1;                         /**< indicate whether the reception frame is from interface 3 */
    uint32_t he_siga1;                            /**< HE-SIGA1 or HT-SIG or VHT-SIG */
    unsigned rxend_state: 8;                      /**< reception state, 0: successful, others: failure */
    uint16_t he_siga2;                            /**< HE-SIGA2 */
    unsigned : 7;                                 /**< reserved */
    unsigned is_group: 1;                         /**< indicate whether the reception is a group addressed frame */
    unsigned timestamp: 32;                       /**< timestamp. The local time when this packet is received. It is precise only if modem sleep or light sleep is not enabled. unit: microsecond */
    unsigned : 15;                                /**< reserved */
    unsigned : 15;                                /**< reserved */
    unsigned : 2;                                 /**< reserved */
    unsigned noise_floor: 8;                      /**< the noise floor of the reception frame */
    signed : 8;                                   /**< reserved */
    signed : 8;                                   /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned : 2;                                 /**< reserved */
    unsigned sigb_len: 10;                        /**< the sigb length */
    unsigned : 1;                                 /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned channel: 4;                          /**< the primary channel */
    unsigned second: 4;                           /**< the second channel if in HT40 */
    unsigned : 12;                                /**< reserved */
    unsigned : 4;                                 /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 7;                                 /**< reserved */
    unsigned : 2;                                 /**< reserved */
    unsigned : 4;                                 /**< reserved */
    unsigned : 2;                                 /**< reserved */
    unsigned : 11;                                /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 12;                                /**< reserved */
    unsigned : 12;                                /**< reserved */
    unsigned cur_bb_format: 4;                    /**< the format of the reception frame */
    unsigned rx_channel_estimate_len: 10;         /**< the length of the channel information */
    unsigned rx_channel_estimate_info_vld: 1;     /**< indicate the channel information is valid */
    unsigned : 5;                                 /**< reserved */
    unsigned : 21;                                /**< reserved */
    unsigned : 10;                                /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 3;                                 /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 6;                                 /**< reserved */
    unsigned : 21;                                /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 7;                                 /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned : 16;                                /**< reserved */
    unsigned sig_len: 14;                         /**< the length of the reception MPDU */
    unsigned : 2;                                 /**< reserved */
    unsigned dump_len: 14;                        /**< the length of the reception MPDU excluding the FCS */
    unsigned : 2;                                 /**< reserved */
    unsigned rx_state: 8;                         /**< reception state, 0: successful, others: failure */
    unsigned : 8;                                 /**< reserved */
    unsigned : 16;                                /**< reserved */
} __attribute__((packed)) esp_wifi_rxctrl_t;
#else
typedef struct {
    signed rssi : 8;                              /**< the RSSI of the reception frame */
    unsigned rate : 5;                            /**< if cur_bb_format is RX_BB_FORMAT_11B, it's the transmission rate. otherwise it's Rate field of L-SIG */
    unsigned : 1;                                 /**< reserved */
    unsigned : 2;                                 /**< reserved */
    unsigned : 12;                                /**< reserved */
    unsigned rxmatch0 : 1;                        /**< indicate whether the reception frame is from interface 0 */
    unsigned rxmatch1 : 1;                        /**< indicate whether the reception frame is from interface 1 */
    unsigned rxmatch2 : 1;                        /**< indicate whether the reception frame is from interface 2 */
    unsigned rxmatch3 : 1;                        /**< indicate whether the reception frame is from interface 3 */
    uint32_t he_siga1;                            /**< HE-SIGA1 or HT-SIG */
    unsigned rxend_state : 8;                     /**< reception state, 0: successful, others: failure */
    uint16_t he_siga2;                            /**< HE-SIGA2 */
    unsigned : 7;                                 /**< reserved */
    unsigned is_group : 1;                        /**< indicate whether the reception is a group addressed frame */
    unsigned timestamp : 32;                      /**< timestamp. The local time when this packet is received. It is precise only if modem sleep or light sleep is not enabled. unit: microsecond */
    unsigned : 15;                                /**< reserved */
    unsigned : 15;                                /**< reserved */
    unsigned : 2;                                 /**< reserved */
    signed noise_floor : 8;                       /**< the noise floor of the reception frame */
    unsigned channel : 4;                         /**< the primary channel */
    unsigned second : 4;                          /**< the second channel if in HT40 */
    unsigned : 8;                                 /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 2;                                 /**< reserved */
    unsigned : 4;                                 /**< reserved */
    unsigned : 2;                                 /**< reserved */
    unsigned rx_channel_estimate_len : 10;        /**< the length of the channel information */
    unsigned rx_channel_estimate_info_vld : 1;    /**< indicate the channel information is valid */
    unsigned : 1;                                 /**< reserved */
    unsigned : 11;                                /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 24;                                /**< reserved */
    unsigned cur_bb_format : 4;                   /**< the format of the reception frame */
    unsigned cur_single_mpdu : 1;                 /**< indicate whether the reception MPDU is a S-MPDU */
    unsigned : 3;                                 /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned he_sigb_len : 6;                     /**< the length of HE-SIGB */
    unsigned : 2;                                 /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned : 32;                                /**< reserved */
    unsigned : 7;                                 /**< reserved */
    unsigned : 1;                                 /**< reserved */
    unsigned : 8;                                 /**< reserved */
    unsigned : 16;                                /**< reserved */
    unsigned sig_len : 14;                        /**< the length of the reception MPDU */
    unsigned : 2;                                 /**< reserved */
    unsigned dump_len : 14;                       /**< the length of the reception MPDU excluding the FCS */
    unsigned : 2;                                 /**< reserved */
    unsigned rx_state : 8;                        /**< reception state, 0: successful, others: failure */
    unsigned : 24;                                /**< reserved */
} __attribute__((packed)) esp_wifi_rxctrl_t;
#endif

/**
  * @brief bTWT setup status
  */
typedef enum {
    BTWT_SETUP_TXFAIL,       /**< station sends btwt setup request frame fail */
    BTWT_SETUP_SUCCESS,      /**< station receives btwt setup response frame and setup btwt sucessfully */
    BTWT_SETUP_TIMEOUT,      /**< timeout of receiving btwt setup response frame */
    BTWT_SETUP_FULL,         /**< indicate there is no available btwt id */
    BTWT_SETUP_INVALID_ARG,  /**< indicate invalid argument to setup btwt */
    BTWT_SETUP_INTERNAL_ERR, /**< indicate internal error to setup btwt */
} wifi_btwt_setup_status_t;

/** Argument structure for WIFI_EVENT_TWT_SET_UP event */
typedef struct {
    wifi_itwt_setup_config_t config;       /**< itwt setup config, this value is determined by the AP */
    esp_err_t status;                     /**< itwt setup status, 1: indicate setup success, others : indicate setup fail */
    uint8_t reason;                       /**< itwt setup frame tx fail reason */
    uint64_t target_wake_time;            /**< TWT SP start time */
} wifi_event_sta_itwt_setup_t;

/**
  * @brief iTWT teardown status
  */
typedef enum {
    ITWT_TEARDOWN_FAIL,               /**< station sends teardown frame fail */
    ITWT_TEARDOWN_SUCCESS,            /**< 1) station successfully sends teardown frame to AP; 2) station receives teardown frame from AP */
} wifi_itwt_teardown_status_t;

/** Argument structure for WIFI_EVENT_TWT_TEARDOWN event */
typedef struct {
    uint8_t flow_id;                     /**< flow id */
    wifi_itwt_teardown_status_t status;  /**< itwt teardown status */
} wifi_event_sta_itwt_teardown_t;

/** Argument structure for WIFI_EVENT_BTWT_SET_UP event */
typedef struct {
    wifi_btwt_setup_status_t status; /**< indicate btwt setup status */
    wifi_twt_setup_cmds_t setup_cmd; /**< indicate the type of TWT command */
    uint8_t btwt_id;                 /**< indicate btwt id */
    uint8_t min_wake_dura;           /**< Nominal Minimum Wake Duration, indicates the minimum amount of time, in unit of 256 us, that the TWT requesting STA expects that it needs to be awake. The value range is [1, 255]. */
    uint8_t wake_invl_expn;          /**< TWT Wake Interval Exponent. The value range is [0, 31]. */
    uint16_t wake_invl_mant;         /**< TWT Wake Interval Mantissa. The value range is [1, 65535]. */
    bool trigger;                    /**< 1: a trigger-enabled TWT, 0: a non-trigger-enabled TWT */
    uint8_t flow_type;               /**< 0: an announced TWT, 1: an unannounced TWT */
    uint8_t reason;                  /**< btwt setup frame tx fail reason */
    uint64_t target_wake_time;       /**< TWT SP start time */
} wifi_event_sta_btwt_setup_t;

/**
  * @brief BTWT teardown status
  */
typedef enum {
    BTWT_TEARDOWN_FAIL,    /**< station sends teardown frame fail */
    BTWT_TEARDOWN_SUCCESS, /**< 1) station successfully sends teardown frame to AP; 2) station receives teardown frame from AP */
} wifi_btwt_teardown_status_t;

/** Argument structure for WIFI_EVENT_TWT_TEARDOWN event */
typedef struct {
    uint8_t btwt_id;                    /**< btwt id */
    wifi_btwt_teardown_status_t status; /**< btwt teardown status */
} wifi_event_sta_btwt_teardown_t;

/**
  * @brief iTWT probe status
  */
typedef enum {
    ITWT_PROBE_FAIL,               /**< station sends probe request fail */
    ITWT_PROBE_SUCCESS,            /**< 1) station receives beacon from AP; 2) station receives probe response from AP */
    ITWT_PROBE_TIMEOUT,            /**< 1) timeout of receiving ACK in response of previously probe request sending by station
                                        2) timeout of receiving probe response in response of previously probe request sending by station */
    ITWT_PROBE_STA_DISCONNECTED,   /**< station is not connected */
} wifi_itwt_probe_status_t;

/** Argument structure for WIFI_EVENT_ITWT_SEND_PROBE event */
typedef struct {
    wifi_itwt_probe_status_t status;     /**< probe status */
    uint8_t reason;                      /**< failure reason */
} wifi_event_sta_itwt_probe_t;

/** Argument structure for WIFI_EVENT_ITWT_SUSPEND event */
typedef struct {
    esp_err_t status;                    /**< suspend status */
    uint8_t flow_id_bitmap;              /**< bitmap of the suspended flow id */
    uint32_t actual_suspend_time_ms[8];  /**< the actual suspend time for each flow id, unit: ms */
} wifi_event_sta_itwt_suspend_t;

/**
  * @brief TWT types
  */
typedef enum {
    TWT_TYPE_INDIVIDUAL,                 /**< individual twt */
    TWT_TYPE_BROADCAST,                  /**< broadcast twt */
    TWT_TYPE_MAX,                        /**< the max value */
} wifi_twt_type_t;

/** Argument structure for twt configuration */
typedef struct {
    bool post_wakeup_event;              /**< post twt wakeup event */
    bool twt_enable_keep_alive;          /**< twt enable send qos null to keep alive */
} wifi_twt_config_t;

/** Argument structure for WIFI_EVENT_TWT_WAKEUP event */
typedef struct {
    wifi_twt_type_t twt_type;           /**< twt type */
    uint8_t flow_id;                    /**< flow id */
} wifi_event_sta_twt_wakeup_t;

/** Argument structure for twt information */
typedef struct {
    bool btwt_id_in_use;                      /**< indicate whether the btwt id is in use or not */
    uint16_t btwt_trigger : 1;                /**< 1: a trigger-enabled TWT, 0: a non-trigger-enabled TWT */
    uint16_t btwt_flow_type : 1;              /**< 0: an announced TWT, 1: an unannounced TWT */
    uint16_t btwt_recommendation : 3;         /**< indicate recommendations on the types of frames. 0: no constraints, [1, 3], [4, 7] reserved */
    uint16_t btwt_wake_interval_exponent : 5; /**< TWT Wake Interval Exponent. The value range is [0, 31]. */
    uint16_t btwt_rsvd : 6;                   /**< reserved */
    uint8_t btwt_wake_duration;               /**< TWT Wake duration unit, 0: 256us 1: TU (TU = 1024us) */
    uint16_t btwt_wake_interval_mantissa;     /**< TWT Wake Interval Mantissa. The value range is [1, 65535]. */
    uint16_t btwt_info_id : 5;                /**< btwt id */
    uint16_t btwt_info_persistence : 8;       /**< indicate the number of TBTTs during which the Broadcast TWT SPs corresponding to this broadcast TWT parameters set are present */
    uint16_t btwt_info_rsvd : 3;              /**< reserved */
} esp_wifi_btwt_info_t;

#ifdef __cplusplus
}
#endif
