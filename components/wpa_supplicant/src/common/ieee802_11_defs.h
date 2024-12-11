/*
 * IEEE 802.11 Frame type definitions
 * Copyright (c) 2002-2019, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2007-2008 Intel Corporation
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef IEEE802_11_DEFS_H
#define IEEE802_11_DEFS_H

#include <utils/common.h>

/* IEEE 802.11 defines */

#define WLAN_FC_PVER		0x0003
#define WLAN_FC_TODS		0x0100
#define WLAN_FC_FROMDS		0x0200
#define WLAN_FC_MOREFRAG	0x0400
#define WLAN_FC_RETRY		0x0800
#define WLAN_FC_PWRMGT		0x1000
#define WLAN_FC_MOREDATA	0x2000
#define WLAN_FC_ISWEP		0x4000
#define WLAN_FC_ORDER		0x8000

#define WLAN_FC_GET_TYPE(fc)	(((fc) & 0x000c) >> 2)
#define WLAN_FC_GET_STYPE(fc)	(((fc) & 0x00f0) >> 4)

#define WLAN_GET_SEQ_FRAG(seq) ((seq) & (BIT(3) | BIT(2) | BIT(1) | BIT(0)))
#define WLAN_GET_SEQ_SEQ(seq) \
	(((seq) & (~(BIT(3) | BIT(2) | BIT(1) | BIT(0)))) >> 4)

#define WLAN_FC_TYPE_MGMT		0
#define WLAN_FC_TYPE_CTRL		1
#define WLAN_FC_TYPE_DATA		2

/* management */
#define WLAN_FC_STYPE_ASSOC_REQ		0
#define WLAN_FC_STYPE_ASSOC_RESP	1
#define WLAN_FC_STYPE_REASSOC_REQ	2
#define WLAN_FC_STYPE_REASSOC_RESP	3
#define WLAN_FC_STYPE_PROBE_REQ		4
#define WLAN_FC_STYPE_PROBE_RESP	5
#define WLAN_FC_STYPE_BEACON		8
#define WLAN_FC_STYPE_ATIM		9
#define WLAN_FC_STYPE_DISASSOC		10
#define WLAN_FC_STYPE_AUTH		11
#define WLAN_FC_STYPE_DEAUTH		12
#define WLAN_FC_STYPE_ACTION		13

/* control */
#define WLAN_FC_STYPE_PSPOLL		10
#define WLAN_FC_STYPE_RTS		11
#define WLAN_FC_STYPE_CTS		12
#define WLAN_FC_STYPE_ACK		13
#define WLAN_FC_STYPE_CFEND		14
#define WLAN_FC_STYPE_CFENDACK		15

/* data */
#define WLAN_FC_STYPE_DATA		0
#define WLAN_FC_STYPE_DATA_CFACK	1
#define WLAN_FC_STYPE_DATA_CFPOLL	2
#define WLAN_FC_STYPE_DATA_CFACKPOLL	3
#define WLAN_FC_STYPE_NULLFUNC		4
#define WLAN_FC_STYPE_CFACK		5
#define WLAN_FC_STYPE_CFPOLL		6
#define WLAN_FC_STYPE_CFACKPOLL		7
#define WLAN_FC_STYPE_QOS_DATA		8

/* Authentication algorithms */
#define WLAN_AUTH_OPEN			0
#define WLAN_AUTH_SHARED_KEY		1
#define WLAN_AUTH_FT			2
#define WLAN_AUTH_SAE           	3
#define WLAN_AUTH_LEAP			128

#define WLAN_AUTH_CHALLENGE_LEN 128

#define WLAN_CAPABILITY_ESS BIT(0)
#define WLAN_CAPABILITY_IBSS BIT(1)
#define WLAN_CAPABILITY_CF_POLLABLE BIT(2)
#define WLAN_CAPABILITY_CF_POLL_REQUEST BIT(3)
#define WLAN_CAPABILITY_PRIVACY BIT(4)
#define WLAN_CAPABILITY_SHORT_PREAMBLE BIT(5)
#define WLAN_CAPABILITY_PBCC BIT(6)
#define WLAN_CAPABILITY_CHANNEL_AGILITY BIT(7)
#define WLAN_CAPABILITY_SPECTRUM_MGMT BIT(8)
#define WLAN_CAPABILITY_QOS BIT(9)
#define WLAN_CAPABILITY_SHORT_SLOT_TIME BIT(10)
#define WLAN_CAPABILITY_APSD BIT(11)
#define WLAN_CAPABILITY_RADIO_MEASUREMENT BIT(12)
#define WLAN_CAPABILITY_DSSS_OFDM BIT(13)
#define WLAN_CAPABILITY_DELAYED_BLOCK_ACK BIT(14)
#define WLAN_CAPABILITY_IMM_BLOCK_ACK BIT(15)

/* Status codes (IEEE Std 802.11-2016, 9.4.1.9, Table 9-46) */
#define WLAN_STATUS_SUCCESS 0
#define WLAN_STATUS_UNSPECIFIED_FAILURE 1
#define WLAN_STATUS_CAPS_UNSUPPORTED 10
#define WLAN_STATUS_REASSOC_NO_ASSOC 11
#define WLAN_STATUS_ASSOC_DENIED_UNSPEC 12
#define WLAN_STATUS_NOT_SUPPORTED_AUTH_ALG 13
#define WLAN_STATUS_UNKNOWN_AUTH_TRANSACTION 14
#define WLAN_STATUS_CHALLENGE_FAIL 15
#define WLAN_STATUS_AUTH_TIMEOUT 16
#define WLAN_STATUS_AP_UNABLE_TO_HANDLE_NEW_STA 17
#define WLAN_STATUS_ASSOC_DENIED_RATES 18
#define WLAN_STATUS_ASSOC_DENIED_NOSHORT 19
#define WLAN_STATUS_SPEC_MGMT_REQUIRED 22
#define WLAN_STATUS_PWR_CAPABILITY_NOT_VALID 23
#define WLAN_STATUS_SUPPORTED_CHANNEL_NOT_VALID 24
#define WLAN_STATUS_ASSOC_DENIED_NO_SHORT_SLOT_TIME 25
#define WLAN_STATUS_ASSOC_DENIED_NO_HT 27
#define WLAN_STATUS_R0KH_UNREACHABLE 28
#define WLAN_STATUS_ASSOC_DENIED_NO_PCO 29
#define WLAN_STATUS_ASSOC_REJECTED_TEMPORARILY 30
#define WLAN_STATUS_ROBUST_MGMT_FRAME_POLICY_VIOLATION 31
#define WLAN_STATUS_UNSPECIFIED_QOS_FAILURE 32
#define WLAN_STATUS_DENIED_INSUFFICIENT_BANDWIDTH 33
#define WLAN_STATUS_DENIED_POOR_CHANNEL_CONDITIONS 34
#define WLAN_STATUS_DENIED_QOS_NOT_SUPPORTED 35
#define WLAN_STATUS_REQUEST_DECLINED 37
#define WLAN_STATUS_INVALID_PARAMETERS 38
#define WLAN_STATUS_REJECTED_WITH_SUGGESTED_CHANGES 39
#define WLAN_STATUS_INVALID_IE 40
#define WLAN_STATUS_GROUP_CIPHER_NOT_VALID 41
#define WLAN_STATUS_PAIRWISE_CIPHER_NOT_VALID 42
#define WLAN_STATUS_AKMP_NOT_VALID 43
#define WLAN_STATUS_UNSUPPORTED_RSN_IE_VERSION 44
#define WLAN_STATUS_INVALID_RSN_IE_CAPAB 45
#define WLAN_STATUS_CIPHER_REJECTED_PER_POLICY 46
#define WLAN_STATUS_TS_NOT_CREATED 47
#define WLAN_STATUS_DIRECT_LINK_NOT_ALLOWED 48
#define WLAN_STATUS_DEST_STA_NOT_PRESENT 49
#define WLAN_STATUS_DEST_STA_NOT_QOS_STA 50
#define WLAN_STATUS_ASSOC_DENIED_LISTEN_INT_TOO_LARGE 51
#define WLAN_STATUS_INVALID_FT_ACTION_FRAME_COUNT 52
#define WLAN_STATUS_INVALID_PMKID 53
#define WLAN_STATUS_INVALID_MDIE 54
#define WLAN_STATUS_INVALID_FTIE 55
#define WLAN_STATUS_GAS_ADV_PROTO_NOT_SUPPORTED 59
#define WLAN_STATUS_NO_OUTSTANDING_GAS_REQ 60
#define WLAN_STATUS_GAS_RESP_NOT_RECEIVED 61
#define WLAN_STATUS_STA_TIMED_OUT_WAITING_FOR_GAS_RESP 62
#define WLAN_STATUS_GAS_RESP_LARGER_THAN_LIMIT 63
#define WLAN_STATUS_REQ_REFUSED_HOME 64
#define WLAN_STATUS_ADV_SRV_UNREACHABLE 65
#define WLAN_STATUS_REQ_REFUSED_SSPN 67
#define WLAN_STATUS_REQ_REFUSED_UNAUTH_ACCESS 68
#define WLAN_STATUS_INVALID_RSNIE 72
#define WLAN_STATUS_ANTI_CLOGGING_TOKEN_REQ 76
#define WLAN_STATUS_FINITE_CYCLIC_GROUP_NOT_SUPPORTED 77
#define WLAN_STATUS_TRANSMISSION_FAILURE 79
#define WLAN_STATUS_REJECTED_WITH_SUGGESTED_BSS_TRANSITION 82
#define WLAN_STATUS_PENDING_ADMITTING_FST_SESSION 86
#define WLAN_STATUS_QUERY_RESP_OUTSTANDING 95
#define WLAN_STATUS_DENIED_WITH_SUGGESTED_BAND_AND_CHANNEL 99
#define WLAN_STATUS_ASSOC_DENIED_NO_VHT 104
#define WLAN_STATUS_UNKNOWN_PASSWORD_IDENTIFIER 123
#define WLAN_STATUS_SAE_HASH_TO_ELEMENT 126
#define WLAN_STATUS_SAE_PK 127

/* Reason codes (IEEE Std 802.11-2016, 9.4.1.7, Table 9-45) */
#define WLAN_REASON_UNSPECIFIED 1
#define WLAN_REASON_PREV_AUTH_NOT_VALID 2
#define WLAN_REASON_DEAUTH_LEAVING 3
#define WLAN_REASON_DISASSOC_DUE_TO_INACTIVITY 4
#define WLAN_REASON_DISASSOC_AP_BUSY 5
#define WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA 6
#define WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA 7
#define WLAN_REASON_DISASSOC_STA_HAS_LEFT 8
#define WLAN_REASON_STA_REQ_ASSOC_WITHOUT_AUTH 9
#define WLAN_REASON_PWR_CAPABILITY_NOT_VALID 10
#define WLAN_REASON_SUPPORTED_CHANNEL_NOT_VALID 11
#define WLAN_REASON_BSS_TRANSITION_DISASSOC 12
#define WLAN_REASON_INVALID_IE 13
#define WLAN_REASON_MICHAEL_MIC_FAILURE 14
#define WLAN_REASON_4WAY_HANDSHAKE_TIMEOUT 15
#define WLAN_REASON_GROUP_KEY_UPDATE_TIMEOUT 16
#define WLAN_REASON_IE_IN_4WAY_DIFFERS 17
#define WLAN_REASON_GROUP_CIPHER_NOT_VALID 18
#define WLAN_REASON_PAIRWISE_CIPHER_NOT_VALID 19
#define WLAN_REASON_AKMP_NOT_VALID 20
#define WLAN_REASON_UNSUPPORTED_RSN_IE_VERSION 21
#define WLAN_REASON_INVALID_RSN_IE_CAPAB 22
#define WLAN_REASON_IEEE_802_1X_AUTH_FAILED 23
#define WLAN_REASON_CIPHER_SUITE_REJECTED 24
#define WLAN_REASON_INVALID_PMKID 49
#define WLAN_REASON_INVALID_MDE 50

/* Information Element IDs (IEEE Std 802.11-2016, 9.4.2.1, Table 9-77) */
#define WLAN_EID_SSID 0
#define WLAN_EID_SUPP_RATES 1
#define WLAN_EID_DS_PARAMS 3
#define WLAN_EID_CF_PARAMS 4
#define WLAN_EID_TIM 5
#define WLAN_EID_IBSS_PARAMS 6
#define WLAN_EID_COUNTRY 7
#define WLAN_EID_CHALLENGE 16
#define WLAN_EID_PWR_CONSTRAINT 32
#define WLAN_EID_PWR_CAPABILITY 33
#define WLAN_EID_TPC_REQUEST 34
#define WLAN_EID_TPC_REPORT 35
#define WLAN_EID_SUPPORTED_CHANNELS 36
#define WLAN_EID_CHANNEL_SWITCH 37
#define WLAN_EID_MEASURE_REQUEST 38
#define WLAN_EID_MEASURE_REPORT 39
#define WLAN_EID_QUIET 40
#define WLAN_EID_IBSS_DFS 41
#define WLAN_EID_ERP_INFO 42
#define WLAN_EID_HT_CAP 45
#define WLAN_EID_RSN 48
#define WLAN_EID_EXT_SUPP_RATES 50
#define WLAN_EID_NEIGHBOR_REPORT 52
#define WLAN_EID_MOBILITY_DOMAIN 54
#define WLAN_EID_FAST_BSS_TRANSITION 55
#define WLAN_EID_TIMEOUT_INTERVAL 56
#define WLAN_EID_RIC_DATA 57
#define WLAN_EID_SUPPORTED_OPERATING_CLASSES 59
#define WLAN_EID_HT_OPERATION 61
#define WLAN_EID_SECONDARY_CHANNEL_OFFSET 62
#define WLAN_EID_WAPI 68
#define WLAN_EID_RRM_ENABLED_CAPABILITIES 70
#define WLAN_EID_20_40_BSS_COEXISTENCE 72
#define WLAN_EID_20_40_BSS_INTOLERANT 73
#define WLAN_EID_OVERLAPPING_BSS_SCAN_PARAMS 74
#define WLAN_EID_MMIE 76
#define WLAN_EID_EXT_CAPAB 127
#define WLAN_EID_VENDOR_SPECIFIC 221
#define WLAN_EID_CAG_NUMBER 237
#define WLAN_EID_AP_CSN 239
#define WLAN_EID_FILS_INDICATION 240
#define WLAN_EID_DILS 241
#define WLAN_EID_FRAGMENT 242
#define WLAN_EID_RSNX 244
#define WLAN_EID_EXTENSION 255

/* Element ID Extension (EID 255) values */
#define WLAN_EID_EXT_ASSOC_DELAY_INFO 1
#define WLAN_EID_EXT_FILS_REQ_PARAMS 2
#define WLAN_EID_EXT_FILS_KEY_CONFIRM 3
#define WLAN_EID_EXT_FILS_SESSION 4
#define WLAN_EID_EXT_FILS_HLP_CONTAINER 5
#define WLAN_EID_EXT_FILS_IP_ADDR_ASSIGN 6
#define WLAN_EID_EXT_KEY_DELIVERY 7
#define WLAN_EID_EXT_WRAPPED_DATA 8
#define WLAN_EID_EXT_FTM_SYNC_INFO 9
#define WLAN_EID_EXT_EXTENDED_REQUEST 10
#define WLAN_EID_EXT_ESTIMATED_SERVICE_PARAMS 11
#define WLAN_EID_EXT_FILS_PUBLIC_KEY 12
#define WLAN_EID_EXT_FILS_NONCE 13
#define WLAN_EID_EXT_FUTURE_CHANNEL_GUIDANCE 14
#define WLAN_EID_EXT_OWE_DH_PARAM 32
#define WLAN_EID_EXT_PASSWORD_IDENTIFIER 33
#define WLAN_EID_EXT_HE_CAPABILITIES 35
#define WLAN_EID_EXT_HE_OPERATION 36
#define WLAN_EID_EXT_REJECTED_GROUPS 92
#define WLAN_EID_EXT_ANTI_CLOGGING_TOKEN 93
#define WLAN_EID_EXT_AKM_SUITE_SELECTOR 114

/* Extended RSN Capabilities */
/* bits 0-3: Field length (n-1) */
#define WLAN_RSNX_CAPAB_SAE_H2E 5
#define WLAN_RSNX_CAPAB_SAE_PK 6

#define WLAN_EXT_CAPAB_20_40_COEX 0
#define WLAN_EXT_CAPAB_BSS_TRANSITION 19

/* Action frame categories (IEEE Std 802.11-2016, 9.4.1.11, Table 9-76) */
#define WLAN_ACTION_SPECTRUM_MGMT 0
#define WLAN_ACTION_QOS 1
#define WLAN_ACTION_DLS 2
#define WLAN_ACTION_BLOCK_ACK 3
#define WLAN_ACTION_PUBLIC 4
#define WLAN_ACTION_RADIO_MEASUREMENT 5
#define WLAN_ACTION_FT 6
#define WLAN_ACTION_HT 7
#define WLAN_ACTION_SA_QUERY 8
#define WLAN_ACTION_WNM 10
#define WLAN_ACTION_UNPROTECTED_WNM 11
#define WLAN_ACTION_WMM 17 /* WMM Specification 1.1 */

/* Public action codes (IEEE Std 802.11-2016, 9.6.8.1, Table 9-307) */
#define WLAN_PA_VENDOR_SPECIFIC 9
#define WLAN_PA_GAS_INITIAL_REQ 10
#define WLAN_PA_GAS_INITIAL_RESP 11
#define WLAN_PA_GAS_COMEBACK_REQ 12
#define WLAN_PA_GAS_COMEBACK_RESP 13

/* SA Query Action frame (IEEE 802.11w/D8.0, 7.4.9) */
#define WLAN_SA_QUERY_REQUEST 0
#define WLAN_SA_QUERY_RESPONSE 1

#define WLAN_SA_QUERY_TR_ID_LEN 2

/* Timeout Interval Type */
#define WLAN_TIMEOUT_REASSOC_DEADLINE 1
#define WLAN_TIMEOUT_KEY_LIFETIME 2
#define WLAN_TIMEOUT_ASSOC_COMEBACK 3

#define OUI_WFA 0x506f9a
#define DPP_OUI_TYPE 0x1A

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif /* _MSC_VER */

struct ieee80211_hdr {
	le16 frame_control;
	le16 duration_id;
	u8 addr1[6];
	u8 addr2[6];
	u8 addr3[6];
	le16 seq_ctrl;
	/* followed by 'u8 addr4[6];' if ToDS and FromDS is set in data frame
	 */
} STRUCT_PACKED;

#define IEEE80211_DA_FROMDS addr1
#define IEEE80211_BSSID_FROMDS addr2
#define IEEE80211_SA_FROMDS addr3

#define IEEE80211_HDRLEN (sizeof(struct ieee80211_hdr))

#define IEEE80211_FC(type, stype) host_to_le16((type << 2) | (stype << 4))
/* Radio Measurement Action codes */
#define WLAN_RRM_RADIO_MEASUREMENT_REQUEST 0
#define WLAN_RRM_RADIO_MEASUREMENT_REPORT 1
#define WLAN_RRM_LINK_MEASUREMENT_REQUEST 2
#define WLAN_RRM_LINK_MEASUREMENT_REPORT 3
#define WLAN_RRM_NEIGHBOR_REPORT_REQUEST 4
#define WLAN_RRM_NEIGHBOR_REPORT_RESPONSE 5

/* Radio Measurement capabilities (from RM Enabled Capabilities element)
 * IEEE Std 802.11-2016, 9.4.2.45, Table 9-157 */
/* byte 1 (out of 5) */
#define WLAN_RRM_CAPS_LINK_MEASUREMENT BIT(0)
#define WLAN_RRM_CAPS_NEIGHBOR_REPORT BIT(1)
#define WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE BIT(4)
#define WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE BIT(5)
#define WLAN_RRM_CAPS_BEACON_REPORT_TABLE BIT(6)

/*
 * IEEE P802.11-REVmc/D5.0 Table 9-81 - Measurement type definitions for
 * measurement requests
 */
enum measure_type {
	MEASURE_TYPE_RPI_HIST = 2,
	MEASURE_TYPE_BEACON = 5,
	MEASURE_TYPE_LCI = 8,
	MEASURE_TYPE_LOCATION_CIVIC = 11,
	MEASURE_TYPE_MEASURE_PAUSE = 255,
};

/* IEEE Std 802.11-2012 Table 8-71 - Location subject definition */
enum location_subject {
	LOCATION_SUBJECT_LOCAL = 0,
	LOCATION_SUBJECT_REMOTE = 1,
	LOCATION_SUBJECT_3RD_PARTY = 2,
};

/*
 * IEEE P802.11-REVmc/D5.0 Table 9-94 - Optional subelement IDs for LCI request
 */
enum lci_req_subelem {
	LCI_REQ_SUBELEM_AZIMUTH_REQ = 1,
	LCI_REQ_SUBELEM_ORIGINATOR_MAC_ADDR = 2,
	LCI_REQ_SUBELEM_TARGET_MAC_ADDR = 3,
	LCI_REQ_SUBELEM_MAX_AGE = 4,
};

#ifdef ESP_SUPPLICANT
struct ieee80211_pa_vendor {
    u8 oui[3];
    u8 wfa_stype;
    u8 vendor_data[];
} STRUCT_PACKED;

struct ieee80211_gas_resp {
    u8 diag_token;
    u16 status_code;
    u16 comeback_delay;
    u8 type;
    u8 length;
    u8 data[];
} STRUCT_PACKED;

struct ieee80211_public_action {
    u8 action;
    union {
        struct ieee80211_pa_vendor pa_vendor_spec;
        struct ieee80211_gas_resp pa_gas_resp;
    } v;
} STRUCT_PACKED;

struct ieee80211_action {
    u8 category;
    union {
        struct ieee80211_public_action public_action;
    } u;
} STRUCT_PACKED;
#endif /* ESP_SUPPLICANT */

#define IEEE80211_MAX_MMPDU_SIZE 2304
struct ieee80211_ht_capabilities {
	le16 ht_capabilities_info;
	u8 a_mpdu_params;
	u8 supported_mcs_set[16];
	le16 ht_extended_capabilities;
	le32 tx_bf_capability_info;
	u8 asel_capabilities;
} STRUCT_PACKED;


/* HT Operation element */
struct ieee80211_ht_operation {
	u8 primary_chan;
	/* Five octets of HT Operation Information */
	u8 ht_param; /* B0..B7 */
	le16 operation_mode; /* B8..B23 */
	le16 param; /* B24..B39 */
	u8 basic_mcs_set[16];
} STRUCT_PACKED;

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _MSC_VER */

#define ERP_INFO_NON_ERP_PRESENT BIT(0)
#define ERP_INFO_USE_PROTECTION BIT(1)
#define ERP_INFO_BARKER_PREAMBLE_MODE BIT(2)

#define OVERLAPPING_BSS_TRANS_DELAY_FACTOR 5

/* HT Capabilities Info field within HT Capabilities element */
#define HT_CAP_INFO_LDPC_CODING_CAP		((u16) BIT(0))
#define HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET	((u16) BIT(1))
#define HT_CAP_INFO_SMPS_MASK			((u16) (BIT(2) | BIT(3)))
#define HT_CAP_INFO_SMPS_STATIC			((u16) 0)
#define HT_CAP_INFO_SMPS_DYNAMIC		((u16) BIT(2))
#define HT_CAP_INFO_SMPS_DISABLED		((u16) (BIT(2) | BIT(3)))
#define HT_CAP_INFO_GREEN_FIELD			((u16) BIT(4))
#define HT_CAP_INFO_SHORT_GI20MHZ		((u16) BIT(5))
#define HT_CAP_INFO_SHORT_GI40MHZ		((u16) BIT(6))
#define HT_CAP_INFO_TX_STBC			((u16) BIT(7))
#define HT_CAP_INFO_RX_STBC_MASK		((u16) (BIT(8) | BIT(9)))
#define HT_CAP_INFO_RX_STBC_1			((u16) BIT(8))
#define HT_CAP_INFO_RX_STBC_12			((u16) BIT(9))
#define HT_CAP_INFO_RX_STBC_123			((u16) (BIT(8) | BIT(9)))
#define HT_CAP_INFO_DELAYED_BA			((u16) BIT(10))
#define HT_CAP_INFO_MAX_AMSDU_SIZE		((u16) BIT(11))
#define HT_CAP_INFO_DSSS_CCK40MHZ		((u16) BIT(12))
/* B13 - Reserved (was PSMP support during P802.11n development) */
#define HT_CAP_INFO_40MHZ_INTOLERANT		((u16) BIT(14))
#define HT_CAP_INFO_LSIG_TXOP_PROTECT_SUPPORT	((u16) BIT(15))

/* HT Extended Capabilities field within HT Capabilities element */
#define EXT_HT_CAP_INFO_PCO			((u16) BIT(0))
#define EXT_HT_CAP_INFO_PCO_TRANS_TIME_MASK	((u16) (BIT(1) | BIT(2)))
#define EXT_HT_CAP_INFO_TRANS_TIME_OFFSET	1
/* B3..B7 - Reserved */
#define EXT_HT_CAP_INFO_MCS_FEEDBACK_MASK	((u16) (BIT(8) | BIT(9)))
#define EXT_HT_CAP_INFO_MCS_FEEDBACK_OFFSET	8
#define EXT_HT_CAP_INFO_HTC_SUPPORT		((u16) BIT(10))
#define EXT_HT_CAP_INFO_RD_RESPONDER		((u16) BIT(11))
/* B12..B15 - Reserved */

/* Transmit Beanforming Capabilities within HT Capabilities element */
#define TX_BF_CAP_IMPLICIT_TXBF_RX_CAP ((u32) BIT(0))
#define TX_BF_CAP_RX_STAGGERED_SOUNDING_CAP ((u32) BIT(1))
#define TX_BF_CAP_TX_STAGGERED_SOUNDING_CAP ((u32) BIT(2))
#define TX_BF_CAP_RX_NDP_CAP ((u32) BIT(3))
#define TX_BF_CAP_TX_NDP_CAP ((u32) BIT(4))
#define TX_BF_CAP_IMPLICIT_TX_BF_CAP ((u32) BIT(5))
#define TX_BF_CAP_CALIBRATION_MASK ((u32) (BIT(6) | BIT(7))
#define TX_BF_CAP_CALIB_OFFSET 6
#define TX_BF_CAP_EXPLICIT_CSI_TXBF_CAP ((u32) BIT(8))
#define TX_BF_CAP_EXPLICIT_NONCOMPR_STEERING_CAP ((u32) BIT(9))
#define TX_BF_CAP_EXPLICIT_COMPR_STEERING_CAP ((u32) BIT(10))
#define TX_BF_CAP_EXPLICIT_TX_BF_CSI_FEEDBACK_MASK ((u32) (BIT(10) | BIT(11)))
#define TX_BF_CAP_EXPLICIT_BF_CSI_FEEDBACK_OFFSET 11
#define TX_BF_CAP_EXPLICIT_UNCOMPR_STEERING_MATRIX_FEEDBACK_OFFSET 13
#define TX_BF_CAP_EXPLICIT_COMPRESSED_STEERING_MATRIX_FEEDBACK_OFFSET 15
#define TX_BF_CAP_MINIMAL_GROUPING_OFFSET 17
#define TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_OFFSET 19
#define TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET 21
#define TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET 23
#define TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_OFFSET 25
#define TX_BF_CAP_CHANNEL_ESTIMATION_CAP_MASK ((u32) (BIT(27) | BIT(28)))
#define TX_BF_CAP_CHANNEL_ESTIMATION_CAP_OFFSET 27
/* B29..B31 - Reserved */

/* ASEL Capability field within HT Capabilities element */
#define ASEL_CAP_ASEL_CAPABLE ((u8) BIT(0))
#define ASEL_CAP_EXPLICIT_CSI_FEEDBACK_BASED_TX_AS_CAP ((u8) BIT(1))
#define ASEL_CAP_ANT_INDICES_FEEDBACK_BASED_TX_AS_CAP ((u8) BIT(2))
#define ASEL_CAP_EXPLICIT_CSI_FEEDBACK_CAP ((u8) BIT(3))
#define ASEL_CAP_ANT_INDICES_FEEDBACK_CAP ((u8) BIT(4))
#define ASEL_CAP_RX_AS_CAP ((u8) BIT(5))
#define ASEL_CAP_TX_SOUNDING_PPDUS_CAP ((u8) BIT(6))
/* B7 - Reserved */

/* First octet of HT Operation Information within HT Operation element */
#define HT_INFO_HT_PARAM_SECONDARY_CHNL_OFF_MASK	((u8) BIT(0) | BIT(1))
#define HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE		((u8) BIT(0))
#define HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW		((u8) BIT(0) | BIT(1))
#define HT_INFO_HT_PARAM_STA_CHNL_WIDTH			((u8) BIT(2))
#define HT_INFO_HT_PARAM_RIFS_MODE			((u8) BIT(3))
/* B4..B7 - Reserved */

/* HT Protection (B8..B9 of HT Operation Information) */
#define HT_PROT_NO_PROTECTION           0
#define HT_PROT_NONMEMBER_PROTECTION    1
#define HT_PROT_20MHZ_PROTECTION        2
#define HT_PROT_NON_HT_MIXED            3
/* Bits within ieee80211_ht_operation::operation_mode (BIT(0) maps to B8 in
 * HT Operation Information) */
#define HT_OPER_OP_MODE_HT_PROT_MASK ((u16) (BIT(0) | BIT(1))) /* B8..B9 */
#define HT_OPER_OP_MODE_NON_GF_HT_STAS_PRESENT	((u16) BIT(2)) /* B10 */
/* BIT(3), i.e., B11 in HT Operation Information field - Reserved */
#define HT_OPER_OP_MODE_OBSS_NON_HT_STAS_PRESENT	((u16) BIT(4)) /* B12 */
/* BIT(5)..BIT(15), i.e., B13..B23 - Reserved */

/* Last two octets of HT Operation Information (BIT(0) = B24) */
/* B24..B29 - Reserved */
#define HT_OPER_PARAM_DUAL_BEACON			((u16) BIT(6))
#define HT_OPER_PARAM_DUAL_CTS_PROTECTION		((u16) BIT(7))
#define HT_OPER_PARAM_STBC_BEACON			((u16) BIT(8))
#define HT_OPER_PARAM_LSIG_TXOP_PROT_FULL_SUPP		((u16) BIT(9))
#define HT_OPER_PARAM_PCO_ACTIVE			((u16) BIT(10))
#define HT_OPER_PARAM_PCO_PHASE				((u16) BIT(11))
/* B36..B39 - Reserved */

#define MULTI_AP_SUB_ELEM_TYPE 0x06
#define MULTI_AP_TEAR_DOWN BIT(4)
#define MULTI_AP_FRONTHAUL_BSS BIT(5)
#define MULTI_AP_BACKHAUL_BSS BIT(6)
#define MULTI_AP_BACKHAUL_STA BIT(7)

#define WMM_OUI_TYPE 2
#define WMM_OUI_SUBTYPE_INFORMATION_ELEMENT 0
#define WMM_OUI_SUBTYPE_PARAMETER_ELEMENT 1
#define WMM_OUI_SUBTYPE_TSPEC_ELEMENT 2
#define WMM_VERSION 1

#define WMM_ACTION_CODE_ADDTS_REQ 0
#define WMM_ACTION_CODE_ADDTS_RESP 1
#define WMM_ACTION_CODE_DELTS 2

#define WMM_ADDTS_STATUS_ADMISSION_ACCEPTED 0
#define WMM_ADDTS_STATUS_INVALID_PARAMETERS 1
/* 2 - Reserved */
#define WMM_ADDTS_STATUS_REFUSED 3
/* 4-255 - Reserved */

/* WMM TSPEC Direction Field Values */
#define WMM_TSPEC_DIRECTION_UPLINK 0
#define WMM_TSPEC_DIRECTION_DOWNLINK 1
/* 2 - Reserved */
#define WMM_TSPEC_DIRECTION_BI_DIRECTIONAL 3

#define MBO_IE_VENDOR_TYPE 0x506f9a16
#define OSEN_IE_VENDOR_TYPE 0x506f9a12
#define SAE_PK_IE_VENDOR_TYPE 0x506f9a1f
#define SAE_PK_OUI_TYPE 0x1f
#define MBO_OUI_TYPE 22
#define OCE_STA BIT(0)
#define OCE_STA_CFON BIT(1)
#define OCE_AP BIT(2)

/*
 * WMM Information Element (used in (Re)Association Request frames; may also be
 * used in Beacon frames)
 */
struct wmm_information_element {
	/* Element ID: 221 (0xdd); Length: 7 */
	/* required fields for WMM version 1 */
	u8 oui[3]; /* 00:50:f2 */
	u8 oui_type; /* 2 */
	u8 oui_subtype; /* 0 */
	u8 version; /* 1 for WMM version 1.0 */
	u8 qos_info; /* AP/STA specific QoS info */

} STRUCT_PACKED;

#define WMM_AC_AIFSN_MASK 0x0f
#define WMM_AC_AIFNS_SHIFT 0
#define WMM_AC_ACM 0x10
#define WMM_AC_ACI_MASK 0x60
#define WMM_AC_ACI_SHIFT 5

#define WMM_AC_ECWMIN_MASK 0x0f
#define WMM_AC_ECWMIN_SHIFT 0
#define WMM_AC_ECWMAX_MASK 0xf0
#define WMM_AC_ECWMAX_SHIFT 4

struct wmm_ac_parameter {
	u8 aci_aifsn; /* AIFSN, ACM, ACI */
	u8 cw; /* ECWmin, ECWmax (CW = 2^ECW - 1) */
	le16 txop_limit;
}  STRUCT_PACKED;

/*
 * WMM Parameter Element (used in Beacon, Probe Response, and (Re)Association
 * Response frmaes)
 */
struct wmm_parameter_element {
	/* Element ID: 221 (0xdd); Length: 24 */
	/* required fields for WMM version 1 */
	u8 oui[3]; /* 00:50:f2 */
	u8 oui_type; /* 2 */
	u8 oui_subtype; /* 1 */
	u8 version; /* 1 for WMM version 1.0 */
	u8 qos_info; /* AP/STA specific QoS info */
	u8 reserved; /* 0 */
	struct wmm_ac_parameter ac[4]; /* AC_BE, AC_BK, AC_VI, AC_VO */

} STRUCT_PACKED;

/* WMM TSPEC Element */
struct wmm_tspec_element {
	u8 eid; /* 221 = 0xdd */
	u8 length; /* 6 + 55 = 61 */
	u8 oui[3]; /* 00:50:f2 */
	u8 oui_type; /* 2 */
	u8 oui_subtype; /* 2 */
	u8 version; /* 1 */
	/* WMM TSPEC body (55 octets): */
	u8 ts_info[3];
	le16 nominal_msdu_size;
	le16 maximum_msdu_size;
	le32 minimum_service_interval;
	le32 maximum_service_interval;
	le32 inactivity_interval;
	le32 suspension_interval;
	le32 service_start_time;
	le32 minimum_data_rate;
	le32 mean_data_rate;
	le32 peak_data_rate;
	le32 maximum_burst_size;
	le32 delay_bound;
	le32 minimum_phy_rate;
	le16 surplus_bandwidth_allowance;
	le16 medium_time;
} STRUCT_PACKED;


/* Access Categories / ACI to AC coding */
enum wmm_ac {
	WMM_AC_BE = 0 /* Best Effort */,
	WMM_AC_BK = 1 /* Background */,
	WMM_AC_VI = 2 /* Video */,
	WMM_AC_VO = 3 /* Voice */,
	WMM_AC_NUM = 4
};

/* MBO v0.0_r19, 4.2: MBO Attributes */
/* Table 4-5: MBO Attributes */
/* OCE v0.0.10, Table 4-3: OCE Attributes */
enum mbo_attr_id {
	MBO_ATTR_ID_AP_CAPA_IND = 1,
	MBO_ATTR_ID_NON_PREF_CHAN_REPORT = 2,
	MBO_ATTR_ID_CELL_DATA_CAPA = 3,
	MBO_ATTR_ID_ASSOC_DISALLOW = 4,
	MBO_ATTR_ID_CELL_DATA_PREF = 5,
	MBO_ATTR_ID_TRANSITION_REASON = 6,
	MBO_ATTR_ID_TRANSITION_REJECT_REASON = 7,
	MBO_ATTR_ID_ASSOC_RETRY_DELAY = 8,
	OCE_ATTR_ID_CAPA_IND = 101,
	OCE_ATTR_ID_RSSI_BASED_ASSOC_REJECT = 102,
	OCE_ATTR_ID_REDUCED_WAN_METRICS = 103,
	OCE_ATTR_ID_RNR_COMPLETENESS = 104,
};

/* MBO v0.0_r19, 4.2.1: MBO AP Capability Indication Attribute */
/* Table 4-7: MBO AP Capability Indication Field Values */
#define MBO_AP_CAPA_CELL_AWARE BIT(6)

/* MBO v0.0_r19, 4.2.2: Non-preferred Channel Report Attribute */
/* Table 4-10: Reason Code Field Values */
enum mbo_non_pref_chan_reason {
	MBO_NON_PREF_CHAN_REASON_UNSPECIFIED = 0,
	MBO_NON_PREF_CHAN_REASON_RSSI = 1,
	MBO_NON_PREF_CHAN_REASON_EXT_INTERFERENCE = 2,
	MBO_NON_PREF_CHAN_REASON_INT_INTERFERENCE = 3,
};

/* MBO v0.0_r19, 4.2.3: Cellular Data Capabilities Attribute */
/* Table 4-13: Cellular Data Connectivity Field */
enum mbo_cellular_capa {
	MBO_CELL_CAPA_AVAILABLE = 1,
	MBO_CELL_CAPA_NOT_AVAILABLE = 2,
	MBO_CELL_CAPA_NOT_SUPPORTED = 3,
};

/* MBO v0.0_r19, 4.2.7: Transition Rejection Reason Code Attribute */
/* Table 4-21: Transition Rejection Reason Code Field Values */
enum mbo_transition_reject_reason {
	MBO_TRANSITION_REJECT_REASON_UNSPECIFIED = 0,
	MBO_TRANSITION_REJECT_REASON_FRAME_LOSS = 1,
	MBO_TRANSITION_REJECT_REASON_DELAY = 2,
	MBO_TRANSITION_REJECT_REASON_QOS_CAPACITY = 3,
	MBO_TRANSITION_REJECT_REASON_RSSI = 4,
	MBO_TRANSITION_REJECT_REASON_INTERFERENCE = 5,
	MBO_TRANSITION_REJECT_REASON_SERVICES = 6,
};

/* OCE v0.0.10, 4.2.1: OCE Capability Indication Attribute */
#define OCE_RELEASE 1
#define OCE_RELEASE_MASK (BIT(0) | BIT(1) | BIT(2))
#define OCE_IS_STA_CFON BIT(3)
#define OCE_IS_NON_OCE_AP_PRESENT BIT(4)

/* IEEE 802.11v - WNM Action field values */
enum wnm_action {
	WNM_EVENT_REQ = 0,
	WNM_EVENT_REPORT = 1,
	WNM_DIAGNOSTIC_REQ = 2,
	WNM_DIAGNOSTIC_REPORT = 3,
	WNM_LOCATION_CFG_REQ = 4,
	WNM_LOCATION_CFG_RESP = 5,
	WNM_BSS_TRANS_MGMT_QUERY = 6,
	WNM_BSS_TRANS_MGMT_REQ = 7,
	WNM_BSS_TRANS_MGMT_RESP = 8,
	WNM_FMS_REQ = 9,
	WNM_FMS_RESP = 10,
	WNM_COLLOCATED_INTERFERENCE_REQ = 11,
	WNM_COLLOCATED_INTERFERENCE_REPORT = 12,
	WNM_TFS_REQ = 13,
	WNM_TFS_RESP = 14,
	WNM_TFS_NOTIFY = 15,
	WNM_SLEEP_MODE_REQ = 16,
	WNM_SLEEP_MODE_RESP = 17,
	WNM_TIM_BROADCAST_REQ = 18,
	WNM_TIM_BROADCAST_RESP = 19,
	WNM_QOS_TRAFFIC_CAPAB_UPDATE = 20,
	WNM_CHANNEL_USAGE_REQ = 21,
	WNM_CHANNEL_USAGE_RESP = 22,
	WNM_DMS_REQ = 23,
	WNM_DMS_RESP = 24,
	WNM_TIMING_MEASUREMENT_REQ = 25,
	WNM_NOTIFICATION_REQ = 26,
	WNM_NOTIFICATION_RESP = 27
};

/* IEEE 802.11v - BSS Transition Management Request - Request Mode */
#define WNM_BSS_TM_REQ_PREF_CAND_LIST_INCLUDED BIT(0)
#define WNM_BSS_TM_REQ_ABRIDGED BIT(1)
#define WNM_BSS_TM_REQ_DISASSOC_IMMINENT BIT(2)
#define WNM_BSS_TM_REQ_BSS_TERMINATION_INCLUDED BIT(3)
#define WNM_BSS_TM_REQ_ESS_DISASSOC_IMMINENT BIT(4)

/* IEEE Std 802.11-2012 - Table 8-253 */
enum bss_trans_mgmt_status_code {
	WNM_BSS_TM_ACCEPT = 0,
	WNM_BSS_TM_REJECT_UNSPECIFIED = 1,
	WNM_BSS_TM_REJECT_INSUFFICIENT_BEACON = 2,
	WNM_BSS_TM_REJECT_INSUFFICIENT_CAPABITY = 3,
	WNM_BSS_TM_REJECT_UNDESIRED = 4,
	WNM_BSS_TM_REJECT_DELAY_REQUEST = 5,
	WNM_BSS_TM_REJECT_STA_CANDIDATE_LIST_PROVIDED = 6,
	WNM_BSS_TM_REJECT_NO_SUITABLE_CANDIDATES = 7,
	WNM_BSS_TM_REJECT_LEAVING_ESS = 8
};

/*
 * IEEE P802.11-REVmc/D5.0 Table 9-150 - Optional subelement IDs for
 * neighbor report
 */
#define WNM_NEIGHBOR_TSF                         1
#define WNM_NEIGHBOR_CONDENSED_COUNTRY_STRING    2
#define WNM_NEIGHBOR_BSS_TRANSITION_CANDIDATE    3
#define WNM_NEIGHBOR_BSS_TERMINATION_DURATION    4
#define WNM_NEIGHBOR_BEARING                     5
#define WNM_NEIGHBOR_MEASUREMENT_REPORT         39
#define WNM_NEIGHBOR_MEASUREMENT_PILOT          66
#define WNM_NEIGHBOR_RRM_ENABLED_CAPABILITIES   70
#define WNM_NEIGHBOR_MULTIPLE_BSSID             71

struct tpc_report {
	u8 eid;
	u8 len;
	u8 tx_power;
	u8 link_margin;
} STRUCT_PACKED;

#define RRM_CAPABILITIES_IE_LEN 5

/* IEEE Std 802.11-2012, 8.5.7.4 - Link Measurement Request frame format */
struct rrm_link_measurement_request {
	u8 dialog_token;
	s8 tx_power;
	s8 max_tp;
	u8 variable[0];
} STRUCT_PACKED;

/* IEEE Std 802.11-2012, 8.5.7.5 - Link Measurement Report frame format */
struct rrm_link_measurement_report {
	u8 dialog_token;
	struct tpc_report tpc;
	u8 rx_ant_id;
	u8 tx_ant_id;
	u8 rcpi;
	u8 rsni;
	u8 variable[0];
} STRUCT_PACKED;

/* IEEE Std 802.11-2016, 9.4.2.21 - Measurement Request element */
struct rrm_measurement_request_element {
	u8 eid; /* Element ID */
	u8 len; /* Length */
	u8 token; /* Measurement Token */
	u8 mode; /* Measurement Request Mode */
	u8 type; /* Measurement Type */
	u8 variable[0]; /* Measurement Request */
} STRUCT_PACKED;

/* IEEE Std 802.11-2016, Figure 9-148 - Measurement Request Mode field */
#define MEASUREMENT_REQUEST_MODE_PARALLEL BIT(0)
#define MEASUREMENT_REQUEST_MODE_ENABLE BIT(1)
#define MEASUREMENT_REQUEST_MODE_REQUEST BIT(2)
#define MEASUREMENT_REQUEST_MODE_REPORT BIT(3)
#define MEASUREMENT_REQUEST_MODE_DURATION_MANDATORY BIT(4)

/* IEEE Std 802.11-2016, 9.4.2.21.7 - Beacon request */
struct rrm_measurement_beacon_request {
	u8 oper_class; /* Operating Class */
	u8 channel; /* Channel Number */
	le16 rand_interval; /* Randomization Interval (in TUs) */
	le16 duration; /* Measurement Duration (in TUs) */
	u8 mode; /* Measurement Mode */
	u8 bssid[ETH_ALEN]; /* BSSID */
	u8 variable[0]; /* Optional Subelements */
} STRUCT_PACKED;

/*
 * IEEE Std 802.11-2016, Table 9-87 - Measurement Mode definitions for Beacon
 * request
 */
enum beacon_report_mode {
	BEACON_REPORT_MODE_PASSIVE = 0,
	BEACON_REPORT_MODE_ACTIVE = 1,
	BEACON_REPORT_MODE_TABLE = 2,
};

/* IEEE Std 802.11-2016, Table 9-88 - Beacon Request subelement IDs */
/* IEEE P802.11-REVmd/D2.0, Table 9-106 - Optional subelement IDs for
 * Beacon request */
#define WLAN_BEACON_REQUEST_SUBELEM_SSID	0
#define WLAN_BEACON_REQUEST_SUBELEM_INFO	1 /* Beacon Reporting */
#define WLAN_BEACON_REQUEST_SUBELEM_DETAIL	2 /* Reporting Detail */
#define WLAN_BEACON_REQUEST_SUBELEM_REQUEST	10
#define WLAN_BEACON_REQUEST_SUBELEM_AP_CHANNEL	51 /* AP Channel Report */
#define WLAN_BEACON_REQUEST_SUBELEM_LAST_INDICATION	164
#define WLAN_BEACON_REQUEST_SUBELEM_VENDOR	221

/*
 * IEEE Std 802.11-2016, Table 9-90 - Reporting Detail values
 */
enum beacon_report_detail {
	/* No fixed-length fields or elements */
	BEACON_REPORT_DETAIL_NONE = 0,
	/* All fixed-length fields and any requested elements in the Request
	 * element if present */
	BEACON_REPORT_DETAIL_REQUESTED_ONLY = 1,
	/* All fixed-length fields and elements (default, used when Reporting
	 * Detail subelement is not included in a Beacon request) */
	BEACON_REPORT_DETAIL_ALL_FIELDS_AND_ELEMENTS = 2,
};

/* IEEE Std 802.11-2016, 9.4.2.22 - Measurement Report element */
struct rrm_measurement_report_element {
	u8 eid; /* Element ID */
	u8 len; /* Length */
	u8 token; /* Measurement Token */
	u8 mode; /* Measurement Report Mode */
	u8 type; /* Measurement Type */
	u8 variable[0]; /* Measurement Report */
} STRUCT_PACKED;

/* IEEE Std 802.11-2016, Figure 9-192 - Measurement Report Mode field */
#define MEASUREMENT_REPORT_MODE_ACCEPT 0
#define MEASUREMENT_REPORT_MODE_REJECT_LATE BIT(0)
#define MEASUREMENT_REPORT_MODE_REJECT_INCAPABLE BIT(1)
#define MEASUREMENT_REPORT_MODE_REJECT_REFUSED BIT(2)

/* IEEE Std 802.11-2016, 9.4.2.22.7 - Beacon report */
struct rrm_measurement_beacon_report {
	u8 op_class; /* Operating Class */
	u8 channel; /* Channel Number */
	le64 start_time; /* Actual Measurement Start Time
			  * (in TSF of the BSS requesting the measurement) */
	le16 duration; /* in TUs */
	u8 report_info; /* Reported Frame Information */
	u8 rcpi; /* RCPI */
	u8 rsni; /* RSNI */
	u8 bssid[ETH_ALEN]; /* BSSID */
	u8 antenna_id; /* Antenna ID */
	le32 parent_tsf; /* Parent TSF */
	u8 variable[0]; /* Optional Subelements */
} STRUCT_PACKED;

/* IEEE Std 802.11-2016, Table 9-112 - Beacon report Subelement IDs */
/* IEEE P802.11-REVmd/D2.0, Table 9-130 - Optional subelement IDs for
 * Beacon report */
#define WLAN_BEACON_REPORT_SUBELEM_FRAME_BODY	1
#define WLAN_BEACON_REPORT_SUBELEM_FRAME_BODY_FRAGMENT_ID	2
#define WLAN_BEACON_REPORT_SUBELEM_LAST_INDICATION	164

/* IEEE P802.11-REVmd/D2.0, Table 9-232 - Data field format of the
 * Reported Frame Body Fragment ID subelement */
#define REPORTED_FRAME_BODY_SUBELEM_LEN		4
#define REPORTED_FRAME_BODY_MORE_FRAGMENTS	BIT(7)

/* IEEE P802.11-REVmd/D2.0, 9.4.2.21.7 - Beacon report  */
#define BEACON_REPORT_LAST_INDICATION_SUBELEM_LEN	3

/* IEEE Std 802.11ac-2013, Annex C - dot11PHYType */
enum phy_type {
	PHY_TYPE_UNSPECIFIED = 0,
	PHY_TYPE_FHSS = 1,
	PHY_TYPE_DSSS = 2,
	PHY_TYPE_IRBASEBAND = 3,
	PHY_TYPE_OFDM = 4,
	PHY_TYPE_HRDSSS = 5,
	PHY_TYPE_ERP = 6,
	PHY_TYPE_HT = 7,
	PHY_TYPE_DMG = 8,
	PHY_TYPE_VHT = 9,
};

/* IEEE P802.11-REVmc/D5.0, 9.4.2.37 - Neighbor Report element */
/* BSSID Information Field */
#define NEI_REP_BSSID_INFO_AP_NOT_REACH BIT(0)
#define NEI_REP_BSSID_INFO_AP_UNKNOWN_REACH BIT(1)
#define NEI_REP_BSSID_INFO_AP_REACHABLE (BIT(0) | BIT(1))
#define NEI_REP_BSSID_INFO_SECURITY BIT(2)
#define NEI_REP_BSSID_INFO_KEY_SCOPE BIT(3)
#define NEI_REP_BSSID_INFO_SPECTRUM_MGMT BIT(4)
#define NEI_REP_BSSID_INFO_QOS BIT(5)
#define NEI_REP_BSSID_INFO_APSD BIT(6)
#define NEI_REP_BSSID_INFO_RM BIT(7)
#define NEI_REP_BSSID_INFO_DELAYED_BA BIT(8)
#define NEI_REP_BSSID_INFO_IMM_BA BIT(9)
#define NEI_REP_BSSID_INFO_MOBILITY_DOMAIN BIT(10)
#define NEI_REP_BSSID_INFO_HT BIT(11)
#define NEI_REP_BSSID_INFO_VHT BIT(12)
#define NEI_REP_BSSID_INFO_FTM BIT(13)

#endif /* IEEE802_11_DEFS_H */
