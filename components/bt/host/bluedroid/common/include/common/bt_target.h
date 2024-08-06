/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef BT_TARGET_H
#define BT_TARGET_H

#include <bt_common.h>

#ifndef BUILDCFG
#define BUILDCFG
#endif

/*
#if !defined(HAS_BDROID_BUILDCFG) && !defined(HAS_NO_BDROID_BUILDCFG)
#error "An Android.mk file did not include bdroid_CFLAGS and possibly not bdorid_C_INCLUDES"
#endif
*/

#ifdef HAS_BDROID_BUILDCFG
#include "bdroid_buildcfg.h"
#endif

#include "bluedroid_user_config.h"
#include "stack/bt_types.h"   /* This must be defined AFTER buildcfg.h */

#include "stack/dyn_mem.h"    /* defines static and/or dynamic memory for components */


/* OS Configuration from User config (eg: sdkconfig) */
#define BT_BTU_TASK_STACK_SIZE      UC_BTU_TASK_STACK_SIZE

#if (UC_BT_BLUEDROID_ESP_COEX_VSC == TRUE)
#define ESP_COEX_VSC_INCLUDED        TRUE
#else
#define ESP_COEX_VSC_INCLUDED        FALSE
#endif

#if (UC_BT_CONTROLLER_INCLUDED == TRUE)
#define BT_CONTROLLER_INCLUDED       TRUE
#else
#define BT_CONTROLLER_INCLUDED       FALSE
#endif

/******************************************************************************
**
** Classic BT features
**
******************************************************************************/
#if (UC_BT_CLASSIC_ENABLED == TRUE)
#define CLASSIC_BT_INCLUDED         TRUE
#define BTC_SM_INCLUDED             TRUE
#define BTC_PRF_QUEUE_INCLUDED      TRUE
#define BTC_GAP_BT_INCLUDED         TRUE
#define BTA_SDP_INCLUDED            TRUE
#define BTA_DM_PM_INCLUDED          TRUE
#define BTC_DM_PM_INCLUDED          TRUE
#define SDP_INCLUDED                TRUE
#define BTA_DM_QOS_INCLUDED         TRUE

#define ENC_KEY_SIZE_CTRL_MODE_NONE 0
#define ENC_KEY_SIZE_CTRL_MODE_STD  1
#define ENC_KEY_SIZE_CTRL_MODE_VSC  2
#define ENC_KEY_SIZE_CTRL_MODE      UC_BT_ENC_KEY_SIZE_CTRL_MODE

#if (UC_BT_A2DP_ENABLED == TRUE)
#define BTA_AR_INCLUDED             TRUE
#define BTA_AV_INCLUDED             TRUE
#define AVDT_INCLUDED               TRUE
#define A2D_INCLUDED                TRUE
#define AVCT_INCLUDED               TRUE
#define AVRC_INCLUDED               TRUE
#define BTC_AV_INCLUDED             TRUE
#define BTA_AV_SINK_INCLUDED        TRUE
#define BTC_AV_SINK_INCLUDED        TRUE
#define SBC_DEC_INCLUDED            TRUE
#define BTC_AV_SRC_INCLUDED         TRUE
#define SBC_ENC_INCLUDED            TRUE
#endif /* UC_BT_A2DP_ENABLED */

#if (UC_BT_SPP_ENABLED == TRUE)
#define RFCOMM_INCLUDED             TRUE
#define BTA_JV_INCLUDED             TRUE
#define BTA_JV_RFCOMM_INCLUDED      TRUE
#define BTC_SPP_INCLUDED            TRUE
#endif /* UC_BT_SPP_ENABLED */

#if (UC_BT_L2CAP_ENABLED == TRUE)
#define BTA_JV_INCLUDED             TRUE
#define BTC_L2CAP_INCLUDED          TRUE
#define BTC_SDP_INCLUDED            TRUE
#define VND_BT_JV_BTA_L2CAP         TRUE
#endif /* UC_BT_L2CAP_ENABLED */

#if (UC_BT_HFP_AG_ENABLED == TRUE) || (UC_BT_HFP_CLIENT_ENABLED == TRUE)
#ifndef RFCOMM_INCLUDED
#define RFCOMM_INCLUDED             TRUE
#endif
#ifndef BTM_SCO_INCLUDED
#define BTM_SCO_INCLUDED            TRUE
#endif
#ifndef SBC_DEC_INCLUDED
#define SBC_DEC_INCLUDED            TRUE
#endif
#ifndef SBC_ENC_INCLUDED
#define SBC_ENC_INCLUDED            TRUE
#endif
#ifndef PLC_INCLUDED
#define PLC_INCLUDED                TRUE
#endif

#if (UC_BT_HFP_AG_ENABLED == TRUE)
#ifndef BTM_MAX_SCO_LINKS_AG
#define BTM_MAX_SCO_LINKS_AG        (1)
#endif
#define BTC_HF_INCLUDED             TRUE
#define BTA_AG_INCLUDED             TRUE
#else
#ifndef BTM_MAX_SCO_LINKS_AG
#define BTM_MAX_SCO_LINKS_AG        (0)
#endif
#endif /* (UC_BT_HFP_AG_ENABLED == TRUE) */
#if (UC_BT_HFP_CLIENT_ENABLED == TRUE)
#ifndef BTM_MAX_SCO_LINKS_CLIENT
#define BTM_MAX_SCO_LINKS_CLIENT    (1)
#endif
#define BTC_HF_CLIENT_INCLUDED      TRUE
#define BTA_HF_INCLUDED             TRUE
#else
#ifndef BTM_MAX_SCO_LINKS_CLIENT
#define BTM_MAX_SCO_LINKS_CLIENT    (0)
#endif
#endif /* (UC_BT_HFP_CLIENT_ENABLED == TRUE) */

#ifndef BTM_MAX_SCO_LINKS
#define BTM_MAX_SCO_LINKS           (BTM_MAX_SCO_LINKS_AG + BTM_MAX_SCO_LINKS_CLIENT)
#endif
#endif /* (UC_BT_HFP_AG_ENABLED == TRUE) || (UC_BT_HFP_CLIENT_ENABLED == TRUE) */

#if UC_BT_HID_ENABLED
#define BT_HID_INCLUDED             TRUE
#endif /* UC_BT_HID_ENABLED */

#if UC_BT_HID_HOST_ENABLED
#define HID_HOST_INCLUDED           TRUE
#define BTA_HH_INCLUDED             TRUE
#define BTC_HH_INCLUDED             TRUE
#endif /* UC_BT_HID_HOST_ENABLED */

#if UC_BT_HID_DEVICE_ENABLED
#define HID_DEV_INCLUDED            TRUE
#define BTA_HD_INCLUDED             TRUE
#define BTC_HD_INCLUDED             TRUE
#endif /* UC_BT_HID_DEVICE_ENABLED */

#endif /* UC_BT_CLASSIC_ENABLED */

/* This is set to enable use of GAP L2CAP connections. */
#if (VND_BT_JV_BTA_L2CAP == TRUE)
#define BTA_JV_L2CAP_INCLUDED       TRUE
#define GAP_CONN_INCLUDED           TRUE
#endif /* VND_BT_JV_BTA_L2CAP */

#ifndef CLASSIC_BT_INCLUDED
#define CLASSIC_BT_INCLUDED         FALSE
#endif /* CLASSIC_BT_INCLUDED */

#ifndef CLASSIC_BT_GATT_INCLUDED
#define CLASSIC_BT_GATT_INCLUDED    FALSE
#endif /* CLASSIC_BT_GATT_INCLUDED */
/******************************************************************************
**
** BLE features
**
******************************************************************************/
#if (UC_BT_BLE_ENABLED ==TRUE)
#define BLE_INCLUDED              TRUE
#else
#define BLE_INCLUDED              FALSE
#endif /* UC_BT_BLE_ENABLED */

#if (UC_BT_BLE_50_FEATURES_SUPPORTED == TRUE)
#define BLE_50_FEATURE_SUPPORT   TRUE
#else
#define BLE_50_FEATURE_SUPPORT   FALSE
#endif

#if (UC_BT_BLE_42_FEATURES_SUPPORTED == TRUE || BLE_50_FEATURE_SUPPORT == FALSE)
#define BLE_42_FEATURE_SUPPORT   TRUE
#else
#define BLE_42_FEATURE_SUPPORT   FALSE
#endif

#if (UC_BT_BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER == TRUE)
#define BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER   TRUE
#else
#define BLE_FEAT_PERIODIC_ADV_SYNC_TRANSFER   FALSE
#endif

#if (UC_BT_BLE_FEAT_PERIODIC_ADV_ENH == TRUE)
#define BLE_FEAT_PERIODIC_ADV_ENH   TRUE
#else
#define BLE_FEAT_PERIODIC_ADV_ENH   FALSE
#endif

#if (UC_BT_BLE_FEAT_CREATE_SYNC_ENH == TRUE)
#define BLE_FEAT_CREATE_SYNC_ENH   TRUE
#else
#define BLE_FEAT_CREATE_SYNC_ENH   FALSE
#endif

#if (UC_BT_BLE_HIGH_DUTY_ADV_INTERVAL == TRUE)
#define BLE_HIGH_DUTY_ADV_INTERVAL TRUE
#else
#define BLE_HIGH_DUTY_ADV_INTERVAL FALSE
#endif

#if (UC_BT_BLE_RPA_SUPPORTED  == TRUE)
#define CONTROLLER_RPA_LIST_ENABLE   TRUE
#else
#define CONTROLLER_RPA_LIST_ENABLE   FALSE
#endif

#if (UC_BT_GATTS_ENABLE)
#define GATTS_INCLUDED              TRUE
#else
#define GATTS_INCLUDED              FALSE
#endif /* UC_BT_GATTS_ENABLE */

#if (UC_BT_GATTC_ENABLE)
#define GATTC_INCLUDED              TRUE
#else
#define GATTC_INCLUDED              FALSE
#endif  /* UC_BT_GATTC_ENABLE */

#if (UC_BT_GATTC_ENABLE && UC_BT_GATTC_CACHE_NVS_FLASH_ENABLED)
#define GATTC_CACHE_NVS             TRUE
#else
#define GATTC_CACHE_NVS             FALSE
#endif  /* UC_BT_GATTC_ENABLE && UC_BT_GATTC_CACHE_NVS_FLASH_ENABLED */

#if (UC_BT_GATTC_ENABLE && UC_BT_GATTC_CONNECT_RETRY_COUNT)
#define GATTC_CONNECT_RETRY_COUNT             UC_BT_GATTC_CONNECT_RETRY_COUNT
#else
#define GATTC_CONNECT_RETRY_COUNT             0
#endif  /* UC_BT_GATTC_ENABLE && UC_BT_GATTC_CONNECT_RETRY_COUNT */

#if (GATTC_CONNECT_RETRY_COUNT > 0)
#define GATTC_CONNECT_RETRY_EN     TRUE
#else
#define GATTC_CONNECT_RETRY_EN     FALSE
#endif

#ifdef UC_BT_GATTC_NOTIF_REG_MAX
#define BTA_GATTC_NOTIF_REG_MAX     UC_BT_GATTC_NOTIF_REG_MAX
#else
#define BTA_GATTC_NOTIF_REG_MAX     5
#endif

#if (UC_BT_SMP_ENABLE)
#define SMP_INCLUDED                TRUE
#if (BLE_INCLUDED == TRUE)
#define BLE_PRIVACY_SPT             TRUE
#else
#define BLE_PRIVACY_SPT             FALSE
#endif  /*BLE_INCLUDED*/
#else
#define SMP_INCLUDED                FALSE
#define BLE_PRIVACY_SPT             FALSE
#endif  /* UC_BT_SMP_ENABLE */

#if(UC_BT_SMP_SLAVE_CON_PARAMS_UPD_ENABLE)
#define SMP_SLAVE_CON_PARAMS_UPD_ENABLE     TRUE
#else
#define SMP_SLAVE_CON_PARAMS_UPD_ENABLE     FALSE
#endif /* UC_BT_SMP_SLAVE_CON_PARAMS_UPD_ENABLE */

#if (UC_BT_BLE_SMP_ID_RESET_ENABLE)
#define BLE_SMP_ID_RESET_ENABLE          TRUE
#else
#define BLE_SMP_ID_RESET_ENABLE          FALSE
#endif

#ifdef UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP
#define BLE_ADV_REPORT_FLOW_CONTROL         (UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP && BLE_INCLUDED)
#endif /* UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_SUPP */

#ifdef UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_NUM
#define BLE_ADV_REPORT_FLOW_CONTROL_NUM     UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_NUM
#endif /* UC_BTDM_BLE_ADV_REPORT_FLOW_CTRL_NUM */

#ifdef UC_BTDM_BLE_ADV_REPORT_DISCARD_THRSHOLD
#define BLE_ADV_REPORT_DISCARD_THRSHOLD     UC_BTDM_BLE_ADV_REPORT_DISCARD_THRSHOLD
#endif /* UC_BTDM_BLE_ADV_REPORT_DISCARD_THRSHOLD */

#ifdef UC_BT_ACL_CONNECTIONS
#define MAX_ACL_CONNECTIONS         UC_BT_ACL_CONNECTIONS
#define GATT_MAX_PHY_CHANNEL        UC_BT_ACL_CONNECTIONS
#endif  /* UC_BT_ACL_CONNECTIONS */

#ifdef UC_BT_MULTI_CONNECTION_ENBALE
#define BT_MULTI_CONNECTION_ENBALE  UC_BT_MULTI_CONNECTION_ENBALE
#endif

#if(BT_MULTI_CONNECTION_ENBALE && (CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3))
#define BLE_CE_LEN_MIN           5
#else
#define BLE_CE_LEN_MIN           0
#endif

#ifdef UC_BT_BLE_ESTAB_LINK_CONN_TOUT
#define BLE_ESTABLISH_LINK_CONNECTION_TIMEOUT UC_BT_BLE_ESTAB_LINK_CONN_TOUT
#endif

//------------------Added from bdroid_buildcfg.h---------------------
#ifndef L2CAP_EXTFEA_SUPPORTED_MASK
#define L2CAP_EXTFEA_SUPPORTED_MASK (L2CAP_EXTFEA_ENH_RETRANS | L2CAP_EXTFEA_STREAM_MODE | L2CAP_EXTFEA_NO_CRC | L2CAP_EXTFEA_FIXED_CHNLS)
#endif

#ifndef BTUI_OPS_FORMATS
#define BTUI_OPS_FORMATS (BTA_OP_VCARD21_MASK | BTA_OP_ANY_MASK)
#endif

#ifndef BTA_RFC_MTU_SIZE
#define BTA_RFC_MTU_SIZE (L2CAP_MTU_SIZE-L2CAP_MIN_OFFSET-RFCOMM_DATA_OVERHEAD)
#endif

#ifndef SBC_NO_PCM_CPY_OPTION
#define SBC_NO_PCM_CPY_OPTION FALSE
#endif

#ifndef BT_APP_DEMO
#define BT_APP_DEMO TRUE
#endif

#ifndef BTIF_INCLUDED
#define BTIF_INCLUDED FALSE
#endif

/******************************************************************************
**
** BTC-layer components
**
******************************************************************************/
#ifndef BTC_GAP_BT_INCLUDED
#define BTC_GAP_BT_INCLUDED FALSE
#endif

#ifndef BTC_PRF_QUEUE_INCLUDED
#define BTC_PRF_QUEUE_INCLUDED FALSE
#endif

#ifndef BTC_SM_INCLUDED
#define BTC_SM_INCLUDED FALSE
#endif

#ifndef BTC_AV_INCLUDED
#define BTC_AV_INCLUDED FALSE
#endif

#ifndef BTC_AV_SINK_INCLUDED
#define BTC_AV_SINK_INCLUDED FALSE
#endif

#ifndef BTC_AV_SRC_INCLUDED
#define BTC_AV_SRC_INCLUDED FALSE
#endif

#ifndef BTC_SPP_INCLUDED
#define BTC_SPP_INCLUDED FALSE
#endif

#ifndef BTC_HH_INCLUDED
#define BTC_HH_INCLUDED FALSE
#endif

#ifndef BTC_HD_INCLUDED
#define BTC_HD_INCLUDED FALSE
#endif

#ifndef SBC_DEC_INCLUDED
#define SBC_DEC_INCLUDED FALSE
#endif

#ifndef SBC_ENC_INCLUDED
#define SBC_ENC_INCLUDED FALSE
#endif

/******************************************************************************
**
** BTA-layer components
**
******************************************************************************/
#ifndef BTA_INCLUDED
#define BTA_INCLUDED TRUE
#endif

#ifndef BTA_DM_PM_INCLUDED
#define BTA_DM_PM_INCLUDED FALSE
#endif

#ifndef BTA_DM_QOS_INCLUDED
#define BTA_DM_QOS_INCLUDED FALSE
#endif

#ifndef BTA_PAN_INCLUDED
#define BTA_PAN_INCLUDED FALSE
#endif

#ifndef BTA_HD_INCLUDED
#define BTA_HD_INCLUDED FALSE
#endif

#ifndef BTA_HH_INCLUDED
#define BTA_HH_INCLUDED FALSE
#endif

#ifndef BTA_HH_ROLE
#define BTA_HH_ROLE BTA_MASTER_ROLE_PREF
#endif

#ifndef BTA_HH_LE_INCLUDED
#define BTA_HH_LE_INCLUDED FALSE
#endif

#ifndef BTA_AR_INCLUDED
#define BTA_AR_INCLUDED FALSE
#endif

#ifndef BTA_AV_INCLUDED
#define BTA_AV_INCLUDED FALSE
#endif

#ifndef BTA_AV_SINK_INCLUDED
#define BTA_AV_SINK_INCLUDED FALSE
#endif

#ifndef BTA_JV_INCLUDED
#define BTA_JV_INCLUDED FALSE
#endif

#ifndef BTA_SDP_INCLUDED
#define BTA_SDP_INCLUDED FALSE
#endif

/* This is set to enable use of GAP L2CAP connections. */
#ifndef VND_BT_JV_BTA_L2CAP
#define VND_BT_JV_BTA_L2CAP        FALSE
#endif

#ifndef BTA_JV_L2CAP_INCLUDED
#define BTA_JV_L2CAP_INCLUDED       FALSE
#endif

#ifndef GAP_CONN_INCLUDED
#define GAP_CONN_INCLUDED           FALSE
#endif

/******************************************************************************
**
** Stack-layer components
**
******************************************************************************/
#ifndef AVCT_INCLUDED
#define AVCT_INCLUDED FALSE
#endif

#ifndef AVDT_INCLUDED
#define AVDT_INCLUDED FALSE
#endif

/******************************************************************************
**
** Parameter Configurations for components
**
******************************************************************************/
#ifndef BTA_DISABLE_DELAY
#define BTA_DISABLE_DELAY 200 /* in milliseconds */
#endif

#ifndef BTA_SYS_TIMER_PERIOD
#define BTA_SYS_TIMER_PERIOD  100
#endif

#ifndef SBC_FOR_EMBEDDED_LINUX
#define SBC_FOR_EMBEDDED_LINUX TRUE
#endif

#ifndef AVDT_VERSION
#define AVDT_VERSION  0x0102
#endif

#ifndef BTA_AG_AT_MAX_LEN
#define BTA_AG_AT_MAX_LEN  512
#endif

#ifndef BTA_AVRCP_FF_RW_SUPPORT
#define BTA_AVRCP_FF_RW_SUPPORT FALSE
#endif

#ifndef BTA_AG_SCO_PKT_TYPES
#define BTA_AG_SCO_PKT_TYPES  (BTM_SCO_LINK_ONLY_MASK | BTM_SCO_PKT_TYPES_MASK_EV3 |  BTM_SCO_PKT_TYPES_MASK_NO_3_EV3 | BTM_SCO_PKT_TYPES_MASK_NO_2_EV5 | BTM_SCO_PKT_TYPES_MASK_NO_3_EV5)
#endif

#ifndef BTA_AV_RET_TOUT
#define BTA_AV_RET_TOUT 15
#endif

#ifndef PORCHE_PAIRING_CONFLICT
#define PORCHE_PAIRING_CONFLICT  TRUE
#endif

#ifndef BTA_AV_CO_CP_SCMS_T
#define BTA_AV_CO_CP_SCMS_T  FALSE
#endif

#if UC_BT_BLE_HOST_QUEUE_CONGESTION_CHECK
#define SCAN_QUEUE_CONGEST_CHECK  TRUE
#else
#define SCAN_QUEUE_CONGEST_CHECK  FALSE
#endif

#ifdef UC_CONFIG_BT_GATTS_PPCP_CHAR_GAP
#define BTM_PERIPHERAL_ENABLED   UC_CONFIG_BT_GATTS_PPCP_CHAR_GAP
#endif

#ifdef UC_BT_GATTS_SEND_SERVICE_CHANGE_MODE
#define GATTS_SEND_SERVICE_CHANGE_MODE UC_BT_GATTS_SEND_SERVICE_CHANGE_MODE
#endif

#if (UC_BT_GATTS_ROBUST_CACHING_ENABLED == TRUE)
#define GATTS_ROBUST_CACHING_ENABLED TRUE
#else
#define GATTS_ROBUST_CACHING_ENABLED FALSE
#endif

#if (UC_BT_GATTS_DEVICE_NAME_WRITABLE == TRUE)
#define GATTS_DEVICE_NAME_WRITABLE TRUE
#else
#define GATTS_DEVICE_NAME_WRITABLE FALSE
#endif

#if (UC_BT_GATTS_APPEARANCE_WRITABLE == TRUE)
#define GATTS_APPEARANCE_WRITABLE TRUE
#else
#define GATTS_APPEARANCE_WRITABLE FALSE
#endif

#ifdef UC_BT_BLE_ACT_SCAN_REP_ADV_SCAN
#define BTM_BLE_ACTIVE_SCAN_REPORT_ADV_SCAN_RSP_INDIVIDUALLY    UC_BT_BLE_ACT_SCAN_REP_ADV_SCAN
#endif

#ifdef UC_BT_BLE_RPA_TIMEOUT
#define BTM_BLE_PRIVATE_ADDR_INT UC_BT_BLE_RPA_TIMEOUT
#endif

#if (UC_BT_CLASSIC_BQB_ENABLED == TRUE)
#define BT_CLASSIC_BQB_INCLUDED TRUE
#else
#define BT_CLASSIC_BQB_INCLUDED FALSE
#endif

/* This feature is used to enable interleaved scan*/
#ifndef BTA_HOST_INTERLEAVE_SEARCH
#define BTA_HOST_INTERLEAVE_SEARCH FALSE
#endif

#ifndef BT_USE_TRACES
#define BT_USE_TRACES  FALSE
#endif

#ifndef BT_TRACE_BTIF
#define BT_TRACE_BTIF  TRUE
#endif

#ifndef BT_TRACE_VERBOSE
#define BT_TRACE_VERBOSE  FALSE
#endif

#ifndef BTA_DM_SDP_DB_SIZE
#define BTA_DM_SDP_DB_SIZE  8000
#endif

#ifndef HL_INCLUDED
#define HL_INCLUDED  TRUE
#endif

#ifndef AG_VOICE_SETTINGS
#define AG_VOICE_SETTINGS  HCI_DEFAULT_VOICE_SETTINGS
#endif

#ifndef BTIF_DM_OOB_TEST
#define BTIF_DM_OOB_TEST  FALSE
#endif

// How long to wait before activating sniff mode after entering the
// idle state for FTS, OPS connections
#ifndef BTA_FTS_OPS_IDLE_TO_SNIFF_DELAY_MS
#define BTA_FTS_OPS_IDLE_TO_SNIFF_DELAY_MS 7000
#endif
#ifndef BTA_FTC_OPS_IDLE_TO_SNIFF_DELAY_MS
#define BTA_FTC_OPS_IDLE_TO_SNIFF_DELAY_MS 5000
#endif

//------------------End added from bdroid_buildcfg.h---------------------


/******************************************************************************
**
** Buffer Size
**
******************************************************************************/

#ifndef BT_DEFAULT_BUFFER_SIZE
#define BT_DEFAULT_BUFFER_SIZE          (4096 + 16)
#endif

#ifndef BT_SMALL_BUFFER_SIZE
#define BT_SMALL_BUFFER_SIZE            660
#endif

/* Receives HCI events from the lower-layer. */
#ifndef HCI_CMD_BUF_SIZE
#define HCI_CMD_BUF_SIZE                BT_SMALL_BUFFER_SIZE
#endif

/* Sends SDP data packets. */
#ifndef SDP_DATA_BUF_SIZE
#define SDP_DATA_BUF_SIZE               BT_DEFAULT_BUFFER_SIZE
#endif

/* Sends RFCOMM command packets. */
#ifndef RFCOMM_CMD_BUF_SIZE
#define RFCOMM_CMD_BUF_SIZE             BT_SMALL_BUFFER_SIZE
#endif

/* Sends RFCOMM data packets. */
#ifndef RFCOMM_DATA_BUF_SIZE
#define RFCOMM_DATA_BUF_SIZE            BT_DEFAULT_BUFFER_SIZE
#endif

/* Sends L2CAP packets to the peer and HCI messages to the controller. */
#ifndef L2CAP_CMD_BUF_SIZE
#define L2CAP_CMD_BUF_SIZE              BT_SMALL_BUFFER_SIZE
#endif

#ifndef L2CAP_USER_TX_BUF_SIZE
#define L2CAP_USER_TX_BUF_SIZE          BT_DEFAULT_BUFFER_SIZE
#endif

#ifndef L2CAP_USER_RX_BUF_SIZE
#define L2CAP_USER_RX_BUF_SIZE          BT_DEFAULT_BUFFER_SIZE
#endif

/* Sends L2CAP segmented packets in ERTM mode */
#ifndef L2CAP_FCR_TX_BUF_SIZE
#define L2CAP_FCR_TX_BUF_SIZE           BT_DEFAULT_BUFFER_SIZE
#endif

/* Receives L2CAP segmented packets in ERTM mode */
#ifndef L2CAP_FCR_RX_BUF_SIZE
#define L2CAP_FCR_RX_BUF_SIZE           BT_DEFAULT_BUFFER_SIZE
#endif

#ifndef L2CAP_FCR_ERTM_BUF_SIZE
#define L2CAP_FCR_ERTM_BUF_SIZE         (10240 + 24)
#endif

/* Number of ACL buffers to assign to LE
   if the HCI buffer pool is shared with BR/EDR */
#ifndef L2C_DEF_NUM_BLE_BUF_SHARED
#define L2C_DEF_NUM_BLE_BUF_SHARED      1
#endif

/* Used by BTM when it sends HCI commands to the controller. */
#ifndef BTM_CMD_BUF_SIZE
#define BTM_CMD_BUF_SIZE                BT_SMALL_BUFFER_SIZE
#endif

#ifndef OBX_LRG_DATA_BUF_SIZE
#define OBX_LRG_DATA_BUF_SIZE           (8080 + 26)
#endif

/* Used to send data to L2CAP. */
#ifndef GAP_DATA_BUF_SIZE
#define GAP_DATA_BUF_SIZE               BT_DEFAULT_BUFFER_SIZE
#endif

/* BNEP data and protocol messages. */
#ifndef BNEP_BUF_SIZE
#define BNEP_BUF_SIZE                   BT_DEFAULT_BUFFER_SIZE
#endif

/* AVDTP buffer size for protocol messages */
#ifndef AVDT_CMD_BUF_SIZE
#define AVDT_CMD_BUF_SIZE               BT_SMALL_BUFFER_SIZE
#endif

/* AVDTP buffer size for media packets in case of fragmentation */
#ifndef AVDT_DATA_BUF_SIZE
#define AVDT_DATA_BUF_SIZE              BT_DEFAULT_BUFFER_SIZE
#endif

#ifndef PAN_BUF_SIZE
#define PAN_BUF_SIZE                    BT_DEFAULT_BUFFER_SIZE
#endif

/* Maximum number of buffers to allocate for PAN */
#ifndef PAN_BUF_MAX
#define PAN_BUF_MAX                     100
#endif

/* AVCTP buffer size for protocol messages */
#ifndef AVCT_CMD_BUF_SIZE
#define AVCT_CMD_BUF_SIZE               288
#endif

/* AVRCP buffer size for protocol messages */
#ifndef AVRC_CMD_BUF_SIZE
#define AVRC_CMD_BUF_SIZE               288
#endif

/* AVRCP Metadata buffer size for protocol messages */
#ifndef AVRC_META_CMD_BUF_SIZE
#define AVRC_META_CMD_BUF_SIZE          BT_SMALL_BUFFER_SIZE
#endif

#ifndef BTA_HL_LRG_DATA_BUF_SIZE
#define BTA_HL_LRG_DATA_BUF_SIZE        (10240 + 24)
#endif

/* GATT Server Database buffer size */
#ifndef GATT_DB_BUF_SIZE
#define GATT_DB_BUF_SIZE                128
#endif

/* GATT Data sending buffer size */
#ifndef GATT_DATA_BUF_SIZE
#define GATT_DATA_BUF_SIZE              BT_DEFAULT_BUFFER_SIZE
#endif

/******************************************************************************
**
** HCI Services (H4)
**
******************************************************************************/

/* Use 2 second for low-resolution systems, override to 1 for high-resolution systems */
#ifndef BT_1SEC_TIMEOUT
#define BT_1SEC_TIMEOUT             (2)
#endif

/* Quick Timer */
/* if L2CAP_FCR_INCLUDED is TRUE then it should have 100 millisecond resolution */
/* if none of them is included then QUICK_TIMER_TICKS_PER_SEC is set to 0 to exclude quick timer */
#ifndef QUICK_TIMER_TICKS_PER_SEC
#define QUICK_TIMER_TICKS_PER_SEC   10       /* 100ms timer */
#endif

/******************************************************************************
**
** BTM
**
******************************************************************************/

/* Cancel Inquiry on incoming SSP */
#ifndef BTM_NO_SSP_ON_INQUIRY
#define BTM_NO_SSP_ON_INQUIRY       FALSE
#endif

/* Includes SCO if TRUE */
#ifndef BTM_SCO_INCLUDED
#define BTM_SCO_INCLUDED                FALSE       /* TRUE includes SCO code */
#endif

/* Includes SCO if TRUE */
#ifndef BTM_SCO_HCI_INCLUDED
#if UC_BT_HFP_AUDIO_DATA_PATH_HCI
#define BTM_SCO_HCI_INCLUDED            TRUE       /* TRUE includes SCO over HCI code */
#else
#define BTM_SCO_HCI_INCLUDED            FALSE
#endif /* UC_HFP_AUDIO_DATA_PATH_HCI */
#endif

/* Includes WBS if TRUE */
#ifndef BTM_WBS_INCLUDED
#define BTM_WBS_INCLUDED                UC_BT_HFP_WBS_ENABLE    /* TRUE includes WBS code */
#endif

/*  This is used to work around a controller bug that doesn't like Disconnect
**  issued while there is a role switch in progress
*/
#ifndef BTM_DISC_DURING_RS
#define BTM_DISC_DURING_RS TRUE
#endif

/**************************
** Initial SCO TX credit
*************************/
/* max TX SCO data packet size */
#ifndef BTM_SCO_DATA_SIZE_MAX
#define BTM_SCO_DATA_SIZE_MAX       120 //240
#endif

/* max TX eSCO data packet size */
#ifndef BTM_MSBC_FRAME_SIZE
#define BTM_MSBC_FRAME_SIZE      60
#endif

/* TX eSCO data packet size */
#ifndef BTM_MSBC_FRAME_DATA_SIZE
#define BTM_MSBC_FRAME_DATA_SIZE          57
#endif

/* The size in bytes of the BTM inquiry database. 5 As Default */
#ifndef BTM_INQ_DB_SIZE
#define BTM_INQ_DB_SIZE             5
#endif

/* The default scan mode */
#ifndef BTM_DEFAULT_SCAN_TYPE
#define BTM_DEFAULT_SCAN_TYPE       BTM_SCAN_TYPE_INTERLACED
#endif

/* Should connections to unknown devices be allowed when not discoverable? */
#ifndef BTM_ALLOW_CONN_IF_NONDISCOVER
#define BTM_ALLOW_CONN_IF_NONDISCOVER   TRUE
#endif

/* Sets the Page_Scan_Window:  the length of time that the device is performing a page scan. */
#ifndef BTM_DEFAULT_CONN_WINDOW
#define BTM_DEFAULT_CONN_WINDOW     0x0012
#endif

/* Sets the Page_Scan_Activity:  the interval between the start of two consecutive page scans. */
#ifndef BTM_DEFAULT_CONN_INTERVAL
#define BTM_DEFAULT_CONN_INTERVAL   0x0800
#endif

/* When automatic inquiry scan is enabled, this sets the inquiry scan window. */
#ifndef BTM_DEFAULT_DISC_WINDOW
#define BTM_DEFAULT_DISC_WINDOW     0x0012
#endif

/* When automatic inquiry scan is enabled, this sets the inquiry scan interval. */
#ifndef BTM_DEFAULT_DISC_INTERVAL
#define BTM_DEFAULT_DISC_INTERVAL   0x0800
#endif

/*
* {SERVICE_CLASS, MAJOR_CLASS, MINOR_CLASS}
*
* SERVICE_CLASS:0x5A (Bit17 -Networking,Bit19 - Capturing,Bit20 -Object Transfer,Bit22 -Telephony)
* MAJOR_CLASS:0x02 - PHONE
* MINOR_CLASS:0x0C - SMART_PHONE
*
*/
#define BTA_DM_COD_SMARTPHONE {0x5A, 0x02, 0x0C}

/*
* {SERVICE_CLASS, MAJOR_CLASS, MINOR_CLASS}
*
* SERVICE_CLASS:0x2C (Bit21 - Audio, Bit19 - Capturing)
* MAJOR_CLASS:0x04 - Audio/Video
* MINOR_CLASS:0x05 - LoudSpeaker
*/
#define BTA_DM_COD_LOUDSPEAKER {0x2C, 0x04, 0x14}

/*
* {SERVICE_CLASS, MAJOR_CLASS, MINOR_CLASS}
*
* SERVICE_CLASS:0x00 None
* MAJOR_CLASS:0x1f - Uncategorized: device code not specified
* MINOR_CLASS:0x00 - None
*/
#define BTA_DM_COD_UNCLASSIFIED {0x00, 0x1f, 0x00}

/* Default class of device */
#ifndef BTA_DM_COD
#define BTA_DM_COD BTA_DM_COD_UNCLASSIFIED
#endif

/* The number of SCO links. */
#ifndef BTM_MAX_SCO_LINKS
#define BTM_MAX_SCO_LINKS           0	//3
#endif

/* The preferred type of SCO links (2-eSCO, 0-SCO). */
#ifndef BTM_DEFAULT_SCO_MODE
#define BTM_DEFAULT_SCO_MODE        2
#endif

/* The number of security records for peer devices. 15 AS Default*/
#ifndef BTM_SEC_MAX_DEVICE_RECORDS
#define BTM_SEC_MAX_DEVICE_RECORDS  UC_BT_SMP_MAX_BONDS
#endif

/* The number of security records for services. 32 AS Default*/
#ifndef BTM_SEC_MAX_SERVICE_RECORDS
#define BTM_SEC_MAX_SERVICE_RECORDS 8 // 32
#endif

/* If True, force a retrieval of remote device name for each bond in case it's changed */
#ifndef BTM_SEC_FORCE_RNR_FOR_DBOND
#define BTM_SEC_FORCE_RNR_FOR_DBOND  FALSE
#endif

/* Maximum device name length used in btm database. Up to 248*/
#ifndef BTM_MAX_REM_BD_NAME_LEN
#define BTM_MAX_REM_BD_NAME_LEN     64
#endif

/* Maximum local device name length stored btm database.
  '0' disables storage of the local name in BTM */
#if UC_MAX_LOC_BD_NAME_LEN
#define BTM_MAX_LOC_BD_NAME_LEN     UC_MAX_LOC_BD_NAME_LEN
#define BTC_MAX_LOC_BD_NAME_LEN     BTM_MAX_LOC_BD_NAME_LEN
#else
#define BTM_MAX_LOC_BD_NAME_LEN     64
#define BTC_MAX_LOC_BD_NAME_LEN     BTM_MAX_LOC_BD_NAME_LEN
#endif

/* Fixed Default String. When this is defined as null string, the device's
 * product model name is used as the default local name.
 */
#ifndef BTM_DEF_LOCAL_NAME
#define BTM_DEF_LOCAL_NAME      ""
#endif

/* Maximum service name stored with security authorization (0 if not needed) */
#ifndef BTM_SEC_SERVICE_NAME_LEN
#define BTM_SEC_SERVICE_NAME_LEN    BT_MAX_SERVICE_NAME_LEN
#endif

/* Maximum length of the service name. */
#ifndef BT_MAX_SERVICE_NAME_LEN
#define BT_MAX_SERVICE_NAME_LEN     21
#endif

/* ACL buffer size in HCI Host Buffer Size command. */
#ifndef BTM_ACL_BUF_SIZE
#define BTM_ACL_BUF_SIZE            0
#endif

/* The maximum number of clients that can register with the power manager. */
#ifndef BTM_MAX_PM_RECORDS
#define BTM_MAX_PM_RECORDS          2
#endif

/* This is set to show debug trace messages for the power manager. */
#ifndef BTM_PM_DEBUG
#define BTM_PM_DEBUG                FALSE
#endif

/* This is set to TRUE if link is to be unparked due to BTM_CreateSCO API. */
#ifndef BTM_SCO_WAKE_PARKED_LINK
#define BTM_SCO_WAKE_PARKED_LINK    FALSE
#endif

/* If the user does not respond to security process requests within this many seconds,
 * a negative response would be sent automatically.
 * 30 is LMP response timeout value */
#ifndef BTM_SEC_TIMEOUT_VALUE
#define BTM_SEC_TIMEOUT_VALUE           35
#endif

/* Maximum number of callbacks that can be registered using BTM_RegisterForVSEvents */
#ifndef BTM_MAX_VSE_CALLBACKS
#define BTM_MAX_VSE_CALLBACKS           3
#endif

/******************************************
**    Lisbon Features
*******************************************/
/* This is set to TRUE if the FEC is required for EIR packet. */
#ifndef BTM_EIR_DEFAULT_FEC_REQUIRED
#define BTM_EIR_DEFAULT_FEC_REQUIRED    TRUE
#endif

/* The IO capability of the local device (for Simple Pairing) */
#ifndef BTM_LOCAL_IO_CAPS
#define BTM_LOCAL_IO_CAPS               BTM_IO_CAP_NONE
#endif

#ifndef BTM_LOCAL_IO_CAPS_BLE
#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
#define BTM_LOCAL_IO_CAPS_BLE           BTM_IO_CAP_KBDISP
#else
#define BTM_LOCAL_IO_CAPS_BLE           4
#endif  ///BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
#endif

/* The default MITM Protection Requirement (for Simple Pairing)
 * Possible values are BTM_AUTH_SP_YES or BTM_AUTH_SP_NO */
#ifndef BTM_DEFAULT_AUTH_REQ
#define BTM_DEFAULT_AUTH_REQ            BTM_AUTH_SP_NO
#endif

/* The default MITM Protection Requirement for dedicated bonding using Simple Pairing
 * Possible values are BTM_AUTH_AP_YES or BTM_AUTH_AP_NO */
#ifndef BTM_DEFAULT_DD_AUTH_REQ
#define BTM_DEFAULT_DD_AUTH_REQ            BTM_AUTH_AP_YES
#endif

/* Include Out-of-Band implementation for Simple Pairing */
#ifndef BTM_OOB_INCLUDED
#define BTM_OOB_INCLUDED                TRUE
#endif

/* TRUE to include Sniff Subrating */
#if (BTA_DM_PM_INCLUDED == TRUE)
#ifndef BTM_SSR_INCLUDED
#define BTM_SSR_INCLUDED                FALSE
#endif
#endif /* BTA_DM_PM_INCLUDED */

/*************************
** End of Lisbon Features
**************************/

/* 4.1/4.2 secure connections feature */
#ifndef SC_MODE_INCLUDED
// Disable AES-CCM (BT 4.1) for BT Classic to workaround controller AES issue. E0 encryption (BT 4.0) will be used.
#define SC_MODE_INCLUDED                FALSE
#endif

/* Used for conformance testing ONLY */
#ifndef BTM_BLE_CONFORMANCE_TESTING
#define BTM_BLE_CONFORMANCE_TESTING           FALSE
#endif

/******************************************************************************
**
** CONTROLLER TO HOST FLOW CONTROL
**
******************************************************************************/

#define C2H_FLOW_CONTROL_INCLUDED TRUE

/******************************************************************************
**
** L2CAP
**
******************************************************************************/

#ifndef L2CAP_CLIENT_INCLUDED
#define L2CAP_CLIENT_INCLUDED FALSE
#endif

/* The maximum number of simultaneous applications that can register with LE L2CAP. */
#ifndef BLE_MAX_L2CAP_CLIENTS
#define BLE_MAX_L2CAP_CLIENTS           15
#endif

/* Support status of L2CAP connection-oriented dynamic channels over LE transport with dynamic CID */
#ifndef BLE_L2CAP_COC_INCLUDED
#define BLE_L2CAP_COC_INCLUDED          FALSE // LE COC not use by default
#endif

/* Support status of L2CAP connection-oriented dynamic channels over LE or BR/EDR transport with dynamic CID */
#ifndef L2CAP_COC_INCLUDED
#if (CLASSIC_BT_INCLUDED == TRUE || BLE_L2CAP_COC_INCLUDED == TRUE)
#define L2CAP_COC_INCLUDED              TRUE
#else
#define L2CAP_COC_INCLUDED              FALSE
#endif
#endif

/* The maximum number of simultaneous links that L2CAP can support. Up to 7*/
#ifndef MAX_ACL_CONNECTIONS
#define MAX_L2CAP_LINKS             5
#else
#define MAX_L2CAP_LINKS             MAX_ACL_CONNECTIONS
#endif

/* The maximum number of simultaneous channels that L2CAP can support. Up to 16*/
#ifndef MAX_L2CAP_CHANNELS
#if (CLASSIC_BT_INCLUDED == TRUE)
#define MAX_L2CAP_CHANNELS          16
#else
#if (SMP_INCLUDED == FALSE)
#define MAX_L2CAP_CHANNELS          MAX_ACL_CONNECTIONS  //This is used in the BLE client when start connected with the peer device
#else
#define MAX_L2CAP_CHANNELS          (MAX_ACL_CONNECTIONS * 2)  //This is used in the BLE client when start connected with the peer device and in SMP
#endif   ///SMP_INCLUDED == FALSE
#endif   ///CLASSIC_BT_INCLUDED == TRUE
#endif   ///MAX_L2CAP_CHANNELS

/* The maximum number of simultaneous applications that can register with L2CAP. */
#ifndef MAX_L2CAP_CLIENTS
#if (CLASSIC_BT_INCLUDED == TRUE)
#define MAX_L2CAP_CLIENTS           8
#else
#define MAX_L2CAP_CLIENTS           1  //Not support to allocate a channel control block in BLE only mode
#endif  ///CLASSIC_BT_INCLUDED == TRUE
#endif

/* The number of seconds of link inactivity before a link is disconnected. */
#ifndef L2CAP_LINK_INACTIVITY_TOUT
#define L2CAP_LINK_INACTIVITY_TOUT  4
#endif

/* The number of seconds of link inactivity after bonding before a link is disconnected. */
#ifndef L2CAP_BONDING_TIMEOUT
#define L2CAP_BONDING_TIMEOUT       3
#endif

/* The time from the HCI connection complete to disconnect if no channel is established. */
#ifndef L2CAP_LINK_STARTUP_TOUT
#define L2CAP_LINK_STARTUP_TOUT     60
#endif

/* The L2CAP MTU; must be in accord with the HCI ACL pool size. */
#ifndef L2CAP_MTU_SIZE
#define L2CAP_MTU_SIZE              1691
#endif

/* The L2CAP MPS over Bluetooth; must be in accord with the FCR tx pool size and ACL down buffer size. */
#ifndef L2CAP_MPS_OVER_BR_EDR
#define L2CAP_MPS_OVER_BR_EDR       1010
#endif

/* If host flow control enabled, this is the number of buffers the controller can have unacknowledged. */
#ifndef L2CAP_HOST_FC_ACL_BUFS
#define L2CAP_HOST_FC_ACL_BUFS      20
#endif

/* This is set to enable L2CAP to  take the ACL link out of park mode when ACL data is to be sent. */
#ifndef L2CAP_WAKE_PARKED_LINK
#define L2CAP_WAKE_PARKED_LINK      TRUE
#endif

/* Whether link wants to be the master or the slave. */
#ifndef L2CAP_DESIRED_LINK_ROLE
#define L2CAP_DESIRED_LINK_ROLE     HCI_ROLE_SLAVE
#endif

/* Include Non-Flushable Packet Boundary Flag feature of Lisbon */
#ifndef L2CAP_NON_FLUSHABLE_PB_INCLUDED
#define L2CAP_NON_FLUSHABLE_PB_INCLUDED     TRUE
#endif

/* Minimum number of ACL credit for high priority link */
#ifndef L2CAP_HIGH_PRI_MIN_XMIT_QUOTA
#define L2CAP_HIGH_PRI_MIN_XMIT_QUOTA       5
#endif

/* used for monitoring HCI ACL credit management */
#ifndef L2CAP_HCI_FLOW_CONTROL_DEBUG
#define L2CAP_HCI_FLOW_CONTROL_DEBUG        TRUE
#endif

/* Used for calculating transmit buffers off of */
#ifndef L2CAP_NUM_XMIT_BUFFS
#define L2CAP_NUM_XMIT_BUFFS                HCI_ACL_BUF_MAX
#endif

/* Unicast Connectionless Data */
#ifndef L2CAP_UCD_INCLUDED
#define L2CAP_UCD_INCLUDED                  FALSE
#endif

/* Unicast Connectionless Data MTU */
#ifndef L2CAP_UCD_MTU
#define L2CAP_UCD_MTU                       L2CAP_MTU_SIZE
#endif

/* Unicast Connectionless Data Idle Timeout */
#ifndef L2CAP_UCD_IDLE_TIMEOUT
#define L2CAP_UCD_IDLE_TIMEOUT              2
#endif

/* Unicast Connectionless Data Idle Timeout */
#ifndef L2CAP_UCD_CH_PRIORITY
#define L2CAP_UCD_CH_PRIORITY               L2CAP_CHNL_PRIORITY_MEDIUM
#endif

/* Used for features using fixed channels; set to zero if no fixed channels supported (BLE, etc.) */
/* Excluding L2CAP signaling channel and UCD */
#ifndef L2CAP_NUM_FIXED_CHNLS
#if (CLASSIC_BT_INCLUDED == TRUE)
#define L2CAP_NUM_FIXED_CHNLS               32
#else
#define L2CAP_NUM_FIXED_CHNLS               3   //There are just three fix channel in the BLE only mode(gatt,signal,smp)
#endif  ///CLASSIC_BT_INCLUDED == TRUE
#endif

/* First fixed channel supported */
#ifndef L2CAP_FIRST_FIXED_CHNL
#define L2CAP_FIRST_FIXED_CHNL              4
#endif

#ifndef L2CAP_LAST_FIXED_CHNL
#define L2CAP_LAST_FIXED_CHNL           (L2CAP_FIRST_FIXED_CHNL + L2CAP_NUM_FIXED_CHNLS - 1)
#endif

/* Round Robin service channels in link */
#ifndef L2CAP_ROUND_ROBIN_CHANNEL_SERVICE
#define L2CAP_ROUND_ROBIN_CHANNEL_SERVICE   TRUE
#endif

/* Used for calculating transmit buffers off of */
#ifndef L2CAP_NUM_XMIT_BUFFS
#define L2CAP_NUM_XMIT_BUFFS                HCI_ACL_BUF_MAX
#endif

/* used for monitoring eL2CAP data flow */
#ifndef L2CAP_ERTM_STATS
#define L2CAP_ERTM_STATS                    FALSE
#endif

/* Used for conformance testing ONLY:  When TRUE lets scriptwrapper overwrite info response */
#ifndef L2CAP_CONFORMANCE_TESTING
#define L2CAP_CONFORMANCE_TESTING           FALSE
#endif

/*
 * Max bytes per connection to buffer locally before dropping the
 * connection if local client does not receive it  - default is 1MB
 */
#ifndef L2CAP_MAX_RX_BUFFER
#define L2CAP_MAX_RX_BUFFER                 0x100000
#endif


#ifndef TIMER_PARAM_TYPE
#define TIMER_PARAM_TYPE    UINT32
#endif

/******************************************************************************
**
** BLE
**
******************************************************************************/

#ifndef BLE_INCLUDED
#define BLE_INCLUDED            FALSE
#endif

#ifndef BLE_ANDROID_CONTROLLER_SCAN_FILTER
#define BLE_ANDROID_CONTROLLER_SCAN_FILTER            TRUE
#endif

#ifndef LOCAL_BLE_CONTROLLER_ID
#define LOCAL_BLE_CONTROLLER_ID         (1)
#endif

/*
 * Toggles support for general LE privacy features such as remote address
 * resolution, local address rotation etc.
 */
#ifndef BLE_PRIVACY_SPT
#define BLE_PRIVACY_SPT         FALSE
#endif

/*
 * Enables or disables support for local privacy (ex. address rotation)
 */
#ifndef BLE_LOCAL_PRIVACY_ENABLED
#define BLE_LOCAL_PRIVACY_ENABLED         TRUE
#endif

/*
 * Toggles support for vendor specific extensions such as RPA offloading,
 * feature discovery, multi-adv etc.
 */
#ifndef BLE_VND_INCLUDED
#define BLE_VND_INCLUDED        FALSE
#endif

#ifndef BTM_BLE_ADV_TX_POWER
#ifdef CONFIG_IDF_TARGET_ESP32
#define BTM_BLE_ADV_TX_POWER {-12, -9, -6, -3, 0, 3, 6, 9}
#else
#define BTM_BLE_ADV_TX_POWER {-24, -21, -18, -15, -12, -9, -6, -3, 0, 3, 6, 9, 12, 15, 18, 21}
#endif
#endif

#ifndef BTM_TX_POWER
#ifdef CONFIG_IDF_TARGET_ESP32
#define BTM_TX_POWER {-12, -9, -6, -3, 0, 3, 6, 9}
#else
#define BTM_TX_POWER {-24, -21, -18, -15, -12, -9, -6, -3, 0, 3, 6, 9, 12, 15, 18, 21}
#endif
#endif

#ifndef BTM_TX_POWER_LEVEL_MAX
#ifdef CONFIG_IDF_TARGET_ESP32
#define BTM_TX_POWER_LEVEL_MAX 7
#else
#define BTM_TX_POWER_LEVEL_MAX 15
#endif
#endif


#ifndef BLE_BATCH_SCAN_INCLUDED
#define BLE_BATCH_SCAN_INCLUDED  TRUE
#endif

/******************************************************************************
**
** ATT/GATT Protocol/Profile Settings
**
******************************************************************************/
#ifndef GATT_INCLUDED
#if BLE_INCLUDED == TRUE
#define GATT_INCLUDED TRUE
#else
#define GATT_INCLUDED FALSE
#endif
#endif

#ifndef BTA_GATT_INCLUDED
#if BLE_INCLUDED == TRUE
#define BTA_GATT_INCLUDED TRUE
#else
#define BTA_GATT_INCLUDED FALSE
#endif
#endif

#if BTA_GATT_INCLUDED == TRUE && BLE_INCLUDED == FALSE
#error "can't have GATT without BLE"
#endif

#ifndef BLE_LLT_INCLUDED
#define BLE_LLT_INCLUDED    TRUE
#endif

/* Added this marco to fixed the android 7.0 will lead to update connection parameters
   collision when the slave sent the HCI_BLE_UPD_LL_CONN_PARAMS comment to the controller
   request the master to update connection parameters directly. */
#ifndef BLE_SLAVE_UPD_CONN_PARAMS
#define BLE_SLAVE_UPD_CONN_PARAMS FALSE
#endif

#ifndef ATT_INCLUDED
#define ATT_INCLUDED         TRUE
#endif

#ifndef ATT_DEBUG
#define ATT_DEBUG           FALSE
#endif

#ifndef BLE_PERIPHERAL_MODE_SUPPORT
#define BLE_PERIPHERAL_MODE_SUPPORT  TRUE
#endif

#ifndef BLE_DELAY_REQUEST_ENC
/* This flag is to work around IPHONE problem, We need to wait for iPhone ready
   before send encryption request to iPhone */
#define BLE_DELAY_REQUEST_ENC        FALSE
#endif

#ifndef GAP_TRANSPORT_SUPPORTED
#define GAP_TRANSPORT_SUPPORTED      GATT_TRANSPORT_LE_BR_EDR
#endif

#ifndef GATTP_TRANSPORT_SUPPORTED
#define GATTP_TRANSPORT_SUPPORTED    GATT_TRANSPORT_LE_BR_EDR
#endif

#ifndef GATT_MAX_SR_PROFILES
#define GATT_MAX_SR_PROFILES        UC_CONFIG_BT_GATT_MAX_SR_PROFILES
#endif

#ifndef GATT_MAX_APPS
#define GATT_MAX_APPS            8 /* MAX is 32 note: 2 apps used internally GATT and GAP */
#endif

#ifndef GATT_MAX_PHY_CHANNEL
#define GATT_MAX_PHY_CHANNEL        7
#endif

/* Used for conformance testing ONLY */
#ifndef GATT_CONFORMANCE_TESTING
#define GATT_CONFORMANCE_TESTING           FALSE
#endif

/* number of background connection device allowance, ideally to be the same as WL size
*/
#ifndef GATT_MAX_BG_CONN_DEV
#define GATT_MAX_BG_CONN_DEV        8 /*MAX is 32*/
#endif

/******************************************************************************
**
** GATT
**
******************************************************************************/
#ifndef GATTC_INCLUDED
#if BLE_INCLUDED == TRUE
#define GATTC_INCLUDED         FALSE
#else
#define GATTC_INCLUDED         FALSE
#endif
#endif

#ifndef GATTS_INCLUDED
#if BLE_INCLUDED == TRUE
#define GATTS_INCLUDED         TRUE
#else
#define GATTS_INCLUDED         FALSE
#endif
#endif

/******************************************************************************
**
** SMP
**
******************************************************************************/
#ifndef SMP_INCLUDED
#if BLE_INCLUDED == TRUE
#define SMP_INCLUDED         FALSE
#else
#define SMP_INCLUDED         FALSE
#endif
#endif

#ifndef SMP_DEBUG
#define SMP_DEBUG            FALSE
#endif

#ifndef SMP_DEFAULT_AUTH_REQ
#define SMP_DEFAULT_AUTH_REQ    SMP_AUTH_NB_ENC_ONLY
#endif

#ifndef SMP_MAX_ENC_KEY_SIZE
#define SMP_MAX_ENC_KEY_SIZE    16
#endif

#ifndef SMP_MIN_ENC_KEY_SIZE
#define SMP_MIN_ENC_KEY_SIZE    7
#endif

/* minimum link timeout after SMP pairing is done, leave room for key exchange
   and racing condition for the following service connection.
   Prefer greater than 0 second, and no less than default inactivity link idle
   timer(L2CAP_LINK_INACTIVITY_TOUT) in l2cap) */
#ifndef SMP_LINK_TOUT_MIN
#if (L2CAP_LINK_INACTIVITY_TOUT > 0)
#define SMP_LINK_TOUT_MIN               L2CAP_LINK_INACTIVITY_TOUT
#else
#define SMP_LINK_TOUT_MIN               2
#endif
#endif

/******************************************************************************
**
** SDP
**
******************************************************************************/

#ifndef SDP_INCLUDED
#define SDP_INCLUDED                FALSE
#endif

#if (SDP_INCLUDED == TRUE) && (BTA_JV_INCLUDED == TRUE) && (BT_CLASSIC_BQB_INCLUDED == TRUE)
#define BT_SDP_BQB_INCLUDED         TRUE
#else
#define BT_SDP_BQB_INCLUDED         FALSE
#endif

/* This is set to enable SDP server functionality. */
#ifndef SDP_SERVER_ENABLED
#if SDP_INCLUDED == TRUE
#define SDP_SERVER_ENABLED          TRUE
#else
#define SDP_SERVER_ENABLED          FALSE
#endif
#endif

/* This is set to enable SDP client functionality. */
#ifndef SDP_CLIENT_ENABLED
#if SDP_INCLUDED == TRUE
#define SDP_CLIENT_ENABLED          TRUE
#else
#define SDP_CLIENT_ENABLED          FALSE
#endif
#endif

/* The maximum number of SDP records the server can support. */
#ifndef SDP_MAX_RECORDS
#define SDP_MAX_RECORDS             6  /*max is 30*/
#endif

/* The maximum number of attributes in each record. */
#ifndef SDP_MAX_REC_ATTR
#if (defined(HID_DEV_INCLUDED) && (HID_DEV_INCLUDED==TRUE)) || (defined(BTC_SDP_INCLUDED) && (BTC_SDP_INCLUDED==TRUE))
#define SDP_MAX_REC_ATTR            25
#else
#define SDP_MAX_REC_ATTR            8
#endif /* defined(HID_DEV_INCLUDED) && (HID_DEV_INCLUDED==TRUE) */
#endif

#ifndef SDP_MAX_PAD_LEN
#define SDP_MAX_PAD_LEN             300
#endif

/* The maximum length, in bytes, of an attribute. */
#ifndef SDP_MAX_ATTR_LEN
#define SDP_MAX_ATTR_LEN            400
#endif

/* The maximum number of attribute filters supported by SDP databases. */
#ifndef SDP_MAX_ATTR_FILTERS
#define SDP_MAX_ATTR_FILTERS        15
#endif

/* The maximum number of UUID filters supported by SDP databases. */
#ifndef SDP_MAX_UUID_FILTERS
#define SDP_MAX_UUID_FILTERS        3
#endif

/* The maximum number of record handles retrieved in a search. */
#ifndef SDP_MAX_DISC_SERVER_RECS
#define SDP_MAX_DISC_SERVER_RECS    21
#endif

/* The size of a scratchpad buffer, in bytes, for storing the response to an attribute request. */
#ifndef SDP_MAX_LIST_BYTE_COUNT
#define SDP_MAX_LIST_BYTE_COUNT     4096
#endif

/* The maximum number of parameters in an SDP protocol element. */
#ifndef SDP_MAX_PROTOCOL_PARAMS
#define SDP_MAX_PROTOCOL_PARAMS     2
#endif

/* The maximum number of simultaneous client and server connections. */
#ifndef SDP_MAX_CONNECTIONS
#define SDP_MAX_CONNECTIONS         4
#endif

/* The MTU size for the L2CAP configuration. */
#ifndef SDP_MTU_SIZE
#define SDP_MTU_SIZE                672
#endif

/* The flush timeout for the L2CAP configuration. */
#ifndef SDP_FLUSH_TO
#define SDP_FLUSH_TO                0xFFFF
#endif

/* The name for security authorization. */
#ifndef SDP_SERVICE_NAME
#define SDP_SERVICE_NAME            "Service Discovery"
#endif

/* The security level for BTM. */
#ifndef SDP_SECURITY_LEVEL
#define SDP_SECURITY_LEVEL          BTM_SEC_NONE
#endif

/******************************************************************************
**
** RFCOMM
**
******************************************************************************/
#ifndef RFCOMM_INCLUDED
#define RFCOMM_INCLUDED             FALSE
#endif

#if (RFCOMM_INCLUDED == TRUE) && (BT_CLASSIC_BQB_INCLUDED == TRUE)
#define BT_RFCOMM_BQB_INCLUDED      TRUE
#else
#define BT_RFCOMM_BQB_INCLUDED      FALSE
#endif

#ifndef BTA_JV_RFCOMM_INCLUDED
#define BTA_JV_RFCOMM_INCLUDED      FALSE
#endif

/* The maximum number of ports supported. */
#ifndef MAX_RFC_PORTS
#define MAX_RFC_PORTS               8 /*max is 30*/
#endif

/* The maximum simultaneous links to different devices. */
#ifndef MAX_ACL_CONNECTIONS
#define MAX_BD_CONNECTIONS          3 /*max is 7*/
#else
#define MAX_BD_CONNECTIONS          MAX_ACL_CONNECTIONS
#endif

/* The port receive queue low watermark level, in bytes. */
#ifndef PORT_RX_LOW_WM
#define PORT_RX_LOW_WM              (BTA_RFC_MTU_SIZE * PORT_RX_BUF_LOW_WM)
#endif

/* The port receive queue high watermark level, in bytes. */
#ifndef PORT_RX_HIGH_WM
#define PORT_RX_HIGH_WM             (BTA_RFC_MTU_SIZE * PORT_RX_BUF_HIGH_WM)
#endif

/* The port receive queue critical watermark level, in bytes. */
#ifndef PORT_RX_CRITICAL_WM
#define PORT_RX_CRITICAL_WM         (BTA_RFC_MTU_SIZE * PORT_RX_BUF_CRITICAL_WM)
#endif

/* The port receive queue low watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_LOW_WM
#define PORT_RX_BUF_LOW_WM          4
#endif

/* The port receive queue high watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_HIGH_WM
#define PORT_RX_BUF_HIGH_WM         10
#endif

/* The port receive queue critical watermark level, in number of buffers. */
#ifndef PORT_RX_BUF_CRITICAL_WM
#define PORT_RX_BUF_CRITICAL_WM     15
#endif

/* The port transmit queue high watermark level, in bytes. */
#ifndef PORT_TX_HIGH_WM
#define PORT_TX_HIGH_WM             (BTA_RFC_MTU_SIZE * PORT_TX_BUF_HIGH_WM)
#endif

/* The port transmit queue critical watermark level, in bytes. */
#ifndef PORT_TX_CRITICAL_WM
#define PORT_TX_CRITICAL_WM         (BTA_RFC_MTU_SIZE * PORT_TX_BUF_CRITICAL_WM)
#endif

/* The port transmit queue high watermark level, in number of buffers. */
#ifndef PORT_TX_BUF_HIGH_WM
#define PORT_TX_BUF_HIGH_WM         10
#endif

/* The port transmit queue high watermark level, in number of buffers. */
#ifndef PORT_TX_BUF_CRITICAL_WM
#define PORT_TX_BUF_CRITICAL_WM     15
#endif

/* The RFCOMM multiplexer preferred flow control mechanism. */
#ifndef PORT_FC_DEFAULT
#define PORT_FC_DEFAULT             PORT_FC_CREDIT
#endif

/* The maximum number of credits receiver sends to peer when using credit-based flow control. */
#ifndef PORT_CREDIT_RX_MAX
#define PORT_CREDIT_RX_MAX          16
#endif

/* The credit low watermark level. */
#ifndef PORT_CREDIT_RX_LOW
#define PORT_CREDIT_RX_LOW          8
#endif

/* ERTM Tx window size */
#ifndef RFC_FCR_OPT_TX_WINDOW_SIZE
#define RFC_FCR_OPT_TX_WINDOW_SIZE  10
#endif

/* ERTM Maximum transmissions before disconnecting */
#ifndef RFC_FCR_OPT_MAX_TX_B4_DISCNT
#define RFC_FCR_OPT_MAX_TX_B4_DISCNT 20
#endif

/* ERTM Retransmission timeout (2 secs) */
#ifndef RFC_FCR_OPT_RETX_TOUT
#define RFC_FCR_OPT_RETX_TOUT        2000
#endif

/* ERTM Monitor timeout (12 secs) */
#ifndef RFC_FCR_OPT_MONITOR_TOUT
#define RFC_FCR_OPT_MONITOR_TOUT     12000
#endif

/* ERTM ERTM MPS segment size */
#ifndef RFC_FCR_OPT_MAX_PDU_SIZE
#define RFC_FCR_OPT_MAX_PDU_SIZE     1010
#endif

/******************************************************************************
**
** OBEX
**
******************************************************************************/

/*
 * Buffer size to reassemble the SDU.
 * It will allow buffers to be used that are larger than the L2CAP_MAX_MTU.
 */
#ifndef OBX_USER_RX_BUF_SIZE
#define OBX_USER_RX_BUF_SIZE    OBX_LRG_DATA_BUF_SIZE
#endif

/*
 * Buffer size to hold the SDU.
 * It will allow buffers to be used that are larger than the L2CAP_MAX_MTU.
 */
#ifndef OBX_USER_TX_BUF_SIZE
#define OBX_USER_TX_BUF_SIZE    OBX_LRG_DATA_BUF_SIZE
#endif

/* Buffer size used to hold MPS segments during SDU reassembly. */
#ifndef OBX_FCR_RX_BUF_SIZE
#define OBX_FCR_RX_BUF_SIZE     BT_DEFAULT_BUFFER_SIZE
#endif

/*
 * Buffer size used to hold MPS segments used in (re)transmissions.
 * The size of each buffer must be able to hold the maximum MPS segment size
 * passed in L2CA_SetFCROptions plus BT_HDR (8) + HCI preamble (4) +
 * L2CAP_MIN_OFFSET (11 - as of BT 2.1 + EDR Spec).
 */
#ifndef OBX_FCR_TX_BUF_SIZE
#define OBX_FCR_TX_BUF_SIZE     BT_DEFAULT_BUFFER_SIZE
#endif

/*
 * Size of the transmission window when using enhanced retransmission mode. Not used
 * in basic and streaming modes. Range: 1 - 63
 */
#ifndef OBX_FCR_OPT_TX_WINDOW_SIZE_BR_EDR
#define OBX_FCR_OPT_TX_WINDOW_SIZE_BR_EDR       20
#endif

/*
 * Number of transmission attempts for a single I-Frame before taking
 * Down the connection. Used In ERTM mode only. Value is Ignored in basic and
 * Streaming modes.
 * Range: 0, 1-0xFF
 * 0 - infinite retransmissions
 * 1 - single transmission
 */
#ifndef OBX_FCR_OPT_MAX_TX_B4_DISCNT
#define OBX_FCR_OPT_MAX_TX_B4_DISCNT    20
#endif

/*
 * Retransmission Timeout
 * Range: Minimum 2000 (2 secs) on BR/EDR when supporting PBF.
 */
#ifndef OBX_FCR_OPT_RETX_TOUT
#define OBX_FCR_OPT_RETX_TOUT           2000
#endif

/*
 * Monitor Timeout
 * Range: Minimum 12000 (12 secs) on BR/EDR when supporting PBF.
 */
#ifndef OBX_FCR_OPT_MONITOR_TOUT
#define OBX_FCR_OPT_MONITOR_TOUT        12000
#endif

/*
 * Maximum PDU payload size.
 * Suggestion: The maximum amount of data that will fit into a 3-DH5 packet.
 * Range: 2 octets
*/
#ifndef OBX_FCR_OPT_MAX_PDU_SIZE
#define OBX_FCR_OPT_MAX_PDU_SIZE        L2CAP_MPS_OVER_BR_EDR
#endif

/*
 * Pool ID where to reassemble the SDU.
 * This Pool will allow buffers to be used that are larger than
 * the L2CAP_MAX_MTU.
 */
#ifndef OBX_USER_RX_POOL_ID
#define OBX_USER_RX_POOL_ID              4
#endif

/*
 * Pool ID where to hold the SDU.
 * This Pool will allow buffers to be used that are larger than
 * the L2CAP_MAX_MTU.
 */
#ifndef OBX_USER_TX_POOL_ID
#define OBX_USER_TX_POOL_ID              4
#endif

/*
 * GKI Buffer Pool ID used to hold MPS segments during SDU reassembly
 */
#ifndef OBX_FCR_RX_POOL_ID
#define OBX_FCR_RX_POOL_ID                3
#endif

/*
 * Pool ID used to hold MPS segments used in (re)transmissions.
 * L2CAP_DEFAULT_ERM_POOL_ID is specified to use the HCI ACL data pool.
 * Note:  This pool needs to have enough buffers to hold two times the window size negotiated
 * in the L2CA_SetFCROptions (2 * tx_win_size)  to allow for retransmissions.
 * The size of each buffer must be able to hold the maximum MPS segment size passed in
 * L2CA_SetFCROptions plus BT_HDR (8) + HCI preamble (4) + L2CAP_MIN_OFFSET (11 - as of BT 2.1 + EDR Spec).
 */
#ifndef OBX_FCR_TX_POOL_ID
#define OBX_FCR_TX_POOL_ID                3
#endif


/******************************************************************************
**
** BNEP
**
******************************************************************************/

#ifndef BNEP_INCLUDED
#define BNEP_INCLUDED               FALSE
#endif

/* BNEP status API call is used mainly to get the L2CAP handle */
#ifndef BNEP_SUPPORTS_STATUS_API
#define BNEP_SUPPORTS_STATUS_API            FALSE
#endif

/*
** When BNEP connection changes roles after the connection is established
** we will do an authentication check again on the new role
*/
#ifndef BNEP_DO_AUTH_FOR_ROLE_SWITCH
#define BNEP_DO_AUTH_FOR_ROLE_SWITCH        FALSE
#endif


/* Maximum number of protocol filters supported. */
#ifndef BNEP_MAX_PROT_FILTERS
#define BNEP_MAX_PROT_FILTERS       5
#endif

/* Maximum number of multicast filters supported. */
#ifndef BNEP_MAX_MULTI_FILTERS
#define BNEP_MAX_MULTI_FILTERS      5
#endif

/* Minimum MTU size. */
#ifndef BNEP_MIN_MTU_SIZE
#define BNEP_MIN_MTU_SIZE           L2CAP_MTU_SIZE
#endif

/* Preferred MTU size. */
#ifndef BNEP_MTU_SIZE
#define BNEP_MTU_SIZE               BNEP_MIN_MTU_SIZE
#endif

/* Maximum number of buffers allowed in transmit data queue. */
#ifndef BNEP_MAX_XMITQ_DEPTH
#define BNEP_MAX_XMITQ_DEPTH        20
#endif

/* Maximum number BNEP of connections supported. */
#ifndef BNEP_MAX_CONNECTIONS
#define BNEP_MAX_CONNECTIONS        7
#endif


/******************************************************************************
**
** AVDTP
**
******************************************************************************/

#ifndef AVDT_INCLUDED
#define AVDT_INCLUDED               TRUE
#endif

/* Include reporting capability in AVDTP */
#ifndef AVDT_REPORTING
#define AVDT_REPORTING              TRUE
#endif

/* Include multiplexing capability in AVDTP */
#ifndef AVDT_MULTIPLEXING
#define AVDT_MULTIPLEXING           TRUE
#endif

/* Number of simultaneous links to different peer devices. */
#ifndef AVDT_NUM_LINKS
#define AVDT_NUM_LINKS              2
#endif

/* Number of simultaneous stream endpoints. */
#ifndef AVDT_NUM_SEPS
#define AVDT_NUM_SEPS               3
#endif

/* Number of transport channels setup per media stream(audio or video) */
#ifndef AVDT_NUM_CHANNELS

#if AVDT_REPORTING == TRUE
/* signaling, media and reporting channels */
#define AVDT_NUM_CHANNELS   3
#else
/* signaling and media channels */
#define AVDT_NUM_CHANNELS   2
#endif  // AVDT_REPORTING

#endif  // AVDT_NUM_CHANNELS

/* Number of transport channels setup by AVDT for all media streams
 * AVDT_NUM_CHANNELS * Number of simultaneous streams.
 */
#ifndef AVDT_NUM_TC_TBL
#define AVDT_NUM_TC_TBL             6
#endif

/* Maximum size in bytes of the codec capabilities information element. */
#ifndef AVDT_CODEC_SIZE
#define AVDT_CODEC_SIZE             10
#endif

/* Maximum size in bytes of the content protection information element. */
#ifndef AVDT_PROTECT_SIZE
#define AVDT_PROTECT_SIZE           90
#endif

/* Maximum number of GKI buffers in the fragment queue (for video frames).
 * Must be less than the number of buffers in the buffer pool of size AVDT_DATA_POOL_SIZE */
#ifndef AVDT_MAX_FRAG_COUNT
#define AVDT_MAX_FRAG_COUNT         15
#endif

/******************************************************************************
**
** PAN
**
******************************************************************************/

#ifndef PAN_INCLUDED
#define PAN_INCLUDED                     FALSE
#endif

/* This will enable the PANU role */
#ifndef PAN_SUPPORTS_ROLE_PANU
#define PAN_SUPPORTS_ROLE_PANU              FALSE
#endif

/* This will enable the GN role */
#ifndef PAN_SUPPORTS_ROLE_GN
#define PAN_SUPPORTS_ROLE_GN                FALSE
#endif

/* This will enable the NAP role */
#ifndef PAN_SUPPORTS_ROLE_NAP
#define PAN_SUPPORTS_ROLE_NAP               FALSE
#endif

/* This is just for debugging purposes */
#ifndef PAN_SUPPORTS_DEBUG_DUMP
#define PAN_SUPPORTS_DEBUG_DUMP             FALSE
#endif

/* Maximum number of PAN connections allowed */
#ifndef MAX_PAN_CONNS
#define MAX_PAN_CONNS                    7
#endif

/* Default service name for NAP role */
#ifndef PAN_NAP_DEFAULT_SERVICE_NAME
#define PAN_NAP_DEFAULT_SERVICE_NAME    "Network Access Point Service"
#endif

/* Default service name for GN role */
#ifndef PAN_GN_DEFAULT_SERVICE_NAME
#define PAN_GN_DEFAULT_SERVICE_NAME     "Group Network Service"
#endif

/* Default service name for PANU role */
#ifndef PAN_PANU_DEFAULT_SERVICE_NAME
#define PAN_PANU_DEFAULT_SERVICE_NAME   "PAN User Service"
#endif

/* Default description for NAP role service */
#ifndef PAN_NAP_DEFAULT_DESCRIPTION
#define PAN_NAP_DEFAULT_DESCRIPTION     "NAP"
#endif

/* Default description for GN role service */
#ifndef PAN_GN_DEFAULT_DESCRIPTION
#define PAN_GN_DEFAULT_DESCRIPTION      "GN"
#endif

/* Default description for PANU role service */
#ifndef PAN_PANU_DEFAULT_DESCRIPTION
#define PAN_PANU_DEFAULT_DESCRIPTION    "PANU"
#endif

/* Default Security level for PANU role. */
#ifndef PAN_PANU_SECURITY_LEVEL
#define PAN_PANU_SECURITY_LEVEL          0
#endif

/* Default Security level for GN role. */
#ifndef PAN_GN_SECURITY_LEVEL
#define PAN_GN_SECURITY_LEVEL            0
#endif

/* Default Security level for NAP role. */
#ifndef PAN_NAP_SECURITY_LEVEL
#define PAN_NAP_SECURITY_LEVEL           0
#endif

/******************************************************************************
**
** HFP
**
******************************************************************************/

#if (BTC_HF_INCLUDED == TRUE) && (BT_CLASSIC_BQB_INCLUDED == TRUE)
#define BT_HF_AG_BQB_INCLUDED           TRUE
#else
#define BT_HF_AG_BQB_INCLUDED           FALSE
#endif

#if (BTC_HF_CLIENT_INCLUDED == TRUE) && (BT_CLASSIC_BQB_INCLUDED == TRUE)
#define BT_HF_CLIENT_BQB_INCLUDED       TRUE
#else
#define BT_HF_CLIENT_BQB_INCLUDED       FALSE
#endif

/******************************************************************************
**
** GAP
**
******************************************************************************/

#ifndef GAP_INCLUDED
#define GAP_INCLUDED                TRUE
#endif

/* This is set to enable posting event for data write */
#ifndef GAP_CONN_POST_EVT_INCLUDED
#define GAP_CONN_POST_EVT_INCLUDED  FALSE
#endif

/* The maximum number of simultaneous GAP L2CAP connections. */
#ifndef GAP_MAX_CONNECTIONS
#define GAP_MAX_CONNECTIONS         10 // 30
#endif

/* keep the raw data received from SDP server in database. */
#ifndef SDP_RAW_DATA_INCLUDED
#define SDP_RAW_DATA_INCLUDED       TRUE
#endif

/* Inquiry duration in 1.28 second units. */
#ifndef SDP_DEBUG
#define SDP_DEBUG                   TRUE
#endif

/******************************************************************************
**
** HID
**
******************************************************************************/
#ifndef BT_HID_INCLUDED
#define BT_HID_INCLUDED         FALSE
#endif

/* HID Device Role Included */
#ifndef HID_DEV_INCLUDED
#define HID_DEV_INCLUDED   FALSE
#endif

#ifndef HID_DEV_SUBCLASS
#define HID_DEV_SUBCLASS   COD_MINOR_POINTING
#endif

#ifndef HID_CONTROL_BUF_SIZE
#define HID_CONTROL_BUF_SIZE            BT_DEFAULT_BUFFER_SIZE
#endif

#ifndef HID_INTERRUPT_BUF_SIZE
#define HID_INTERRUPT_BUF_SIZE          BT_DEFAULT_BUFFER_SIZE
#endif

#ifndef HID_DEV_MTU_SIZE
#define HID_DEV_MTU_SIZE 64
#endif

#ifndef HID_DEV_FLUSH_TO
#define HID_DEV_FLUSH_TO 0xffff
#endif

#if (BTA_HD_INCLUDED == TRUE) && (HID_DEV_INCLUDED == TRUE) && (BT_CLASSIC_BQB_INCLUDED == TRUE)
#define BT_HID_DEVICE_BQB_INCLUDED      TRUE
#else
#define BT_HID_DEVICE_BQB_INCLUDED      FALSE
#endif

/*************************************************************************
** Definitions for Both HID-Host & Device
*/
#ifndef HID_MAX_SVC_NAME_LEN
#define HID_MAX_SVC_NAME_LEN  32
#endif

#ifndef HID_MAX_SVC_DESCR_LEN
#define HID_MAX_SVC_DESCR_LEN 32
#endif

#ifndef HID_MAX_PROV_NAME_LEN
#define HID_MAX_PROV_NAME_LEN 32
#endif

/*************************************************************************
** Definitions for HID-Host
*/
#ifndef  HID_HOST_INCLUDED
#define HID_HOST_INCLUDED           FALSE
#endif

#ifndef HID_HOST_MAX_DEVICES
#define HID_HOST_MAX_DEVICES        7
#endif

#ifndef HID_HOST_MTU
#define HID_HOST_MTU                640
#endif

#ifndef HID_HOST_FLUSH_TO
#define HID_HOST_FLUSH_TO                 0xffff
#endif

#ifndef HID_HOST_MAX_CONN_RETRY
#define HID_HOST_MAX_CONN_RETRY     (3)
#endif

#ifndef HID_HOST_REPAGE_WIN
#define HID_HOST_REPAGE_WIN          (2)
#endif

/*************************************************************************
 * A2DP Definitions
 */
#ifndef A2D_INCLUDED
#define A2D_INCLUDED            FALSE
#endif

#if (BTC_AV_SRC_INCLUDED == TRUE) && (BT_CLASSIC_BQB_INCLUDED == TRUE)
#define A2D_SRC_BQB_INCLUDED TRUE
#else
#define A2D_SRC_BQB_INCLUDED FALSE
#endif

/******************************************************************************
**
** AVCTP
**
******************************************************************************/

/* Number of simultaneous ACL links to different peer devices. */
#ifndef AVCT_NUM_LINKS
#define AVCT_NUM_LINKS              2
#endif

/* Number of simultaneous AVCTP connections. */
#ifndef AVCT_NUM_CONN
#define AVCT_NUM_CONN               3
#endif

/******************************************************************************
**
** AVRCP
**
******************************************************************************/
#ifndef AVRC_INCLUDED
#define AVRC_INCLUDED               FALSE
#endif

#ifndef AVRC_METADATA_INCLUDED
#if AVRC_INCLUDED == TRUE
#define AVRC_METADATA_INCLUDED      TRUE
#else
#define AVRC_METADATA_INCLUDED      FALSE
#endif
#endif

#ifndef AVRC_ADV_CTRL_INCLUDED
#if AVRC_INCLUDED == TRUE
#define AVRC_ADV_CTRL_INCLUDED      TRUE
#else
#define AVRC_ADV_CTRL_INCLUDED      FALSE
#endif
#endif

#ifndef AVRC_CTLR_INCLUDED
#if AVRC_INCLUDED == TRUE
#define AVRC_CTLR_INCLUDED          TRUE
#else
#define AVRC_CTLR_INCLUDED          FALSE
#endif
#endif

/******************************************************************************
**
** MCAP
**
******************************************************************************/
#ifndef MCA_INCLUDED
#define MCA_INCLUDED                FALSE
#endif

/* The MTU size for the L2CAP configuration on control channel. 48 is the minimal */
#ifndef MCA_CTRL_MTU
#define MCA_CTRL_MTU    60
#endif

/* The maximum number of registered MCAP instances. */
#ifndef MCA_NUM_REGS
#define MCA_NUM_REGS    12
#endif

/* The maximum number of control channels (to difference devices) per registered MCAP instances. */
#ifndef MCA_NUM_LINKS
#define MCA_NUM_LINKS   3
#endif

/* The maximum number of MDEP (including HDP echo) per registered MCAP instances. */
#ifndef MCA_NUM_DEPS
#define MCA_NUM_DEPS    13
#endif

/* The maximum number of MDL link per control channel. */
#ifndef MCA_NUM_MDLS
#define MCA_NUM_MDLS    4
#endif

/* Buffer size to reassemble the SDU. */
#ifndef MCA_USER_RX_BUF_SIZE
#define MCA_USER_RX_BUF_SIZE    BT_DEFAULT_BUFFER_SIZE
#endif

/* Buffer size to hold the SDU. */
#ifndef MCA_USER_TX_BUF_SIZE
#define MCA_USER_TX_BUF_SIZE    BT_DEFAULT_BUFFER_SIZE
#endif

/*
 * Buffer size used to hold MPS segments during SDU reassembly
 */
#ifndef MCA_FCR_RX_BUF_SIZE
#define MCA_FCR_RX_BUF_SIZE     BT_DEFAULT_BUFFER_SIZE
#endif

/*
 * Default buffer size used to hold MPS segments used in (re)transmissions.
 * The size of each buffer must be able to hold the maximum MPS segment size
 * passed in tL2CAP_FCR_OPTIONS plus BT_HDR (8) + HCI preamble (4) +
 * L2CAP_MIN_OFFSET (11 - as of BT 2.1 + EDR Spec).
 */
#ifndef MCA_FCR_TX_BUF_SIZE
#define MCA_FCR_TX_BUF_SIZE     BT_DEFAULT_BUFFER_SIZE
#endif

/* MCAP control channel FCR Option:
Size of the transmission window when using enhanced retransmission mode.
1 is defined by HDP specification for control channel.
*/
#ifndef MCA_FCR_OPT_TX_WINDOW_SIZE
#define MCA_FCR_OPT_TX_WINDOW_SIZE      1
#endif

/* MCAP control channel FCR Option:
Number of transmission attempts for a single I-Frame before taking
Down the connection. Used In ERTM mode only. Value is Ignored in basic and
Streaming modes.
Range: 0, 1-0xFF
0 - infinite retransmissions
1 - single transmission
*/
#ifndef MCA_FCR_OPT_MAX_TX_B4_DISCNT
#define MCA_FCR_OPT_MAX_TX_B4_DISCNT    20
#endif

/* MCAP control channel FCR Option: Retransmission Timeout
The AVRCP specification set a value in the range of 300 - 2000 ms
Timeout (in msecs) to detect Lost I-Frames. Only used in Enhanced retransmission mode.
Range: Minimum 2000 (2 secs) when supporting PBF.
 */
#ifndef MCA_FCR_OPT_RETX_TOUT
#define MCA_FCR_OPT_RETX_TOUT           2000
#endif

/* MCAP control channel FCR Option: Monitor Timeout
The AVRCP specification set a value in the range of 300 - 2000 ms
Timeout (in msecs) to detect Lost S-Frames. Only used in Enhanced retransmission mode.
Range: Minimum 12000 (12 secs) when supporting PBF.
*/
#ifndef MCA_FCR_OPT_MONITOR_TOUT
#define MCA_FCR_OPT_MONITOR_TOUT        12000
#endif

/* MCAP control channel FCR Option: Maximum PDU payload size.
The maximum number of payload octets that the local device can receive in a single PDU.
*/
#ifndef MCA_FCR_OPT_MPS_SIZE
#define MCA_FCR_OPT_MPS_SIZE            1000
#endif

/* Shared transport */
#ifndef NFC_SHARED_TRANSPORT_ENABLED
#define NFC_SHARED_TRANSPORT_ENABLED    FALSE
#endif

/******************************************************************************
**
** Sleep Mode (Low Power Mode)
**
******************************************************************************/

#ifndef HCILP_INCLUDED
#define HCILP_INCLUDED                  FALSE
#endif

/******************************************************************************
**
** APPL - Application Task
**
******************************************************************************/

#define L2CAP_FEATURE_REQ_ID      73
#define L2CAP_FEATURE_RSP_ID     173

/******************************************************************************
**
** BTA
**
******************************************************************************/
/* BTA EIR canned UUID list (default is dynamic) */
#ifndef BTA_EIR_CANNED_UUID_LIST
#define BTA_EIR_CANNED_UUID_LIST FALSE
#endif

/* Number of supported customer UUID in EIR */
#ifndef BTA_EIR_SERVER_NUM_CUSTOM_UUID
#define BTA_EIR_SERVER_NUM_CUSTOM_UUID     8
#endif

/* CHLD override for bluedroid */
#ifndef BTA_AG_CHLD_VAL_ECC
#define BTA_AG_CHLD_VAL_ECC  "(0,1,1x,2,2x,3)"
#endif

#ifndef BTA_AG_CHLD_VAL
#define BTA_AG_CHLD_VAL  "(0,1,2,3)"
#endif

/* Set the CIND to match HFP 1.5 */
#ifndef BTA_AG_CIND_INFO
#define BTA_AG_CIND_INFO "(\"call\",(0,1)),(\"callsetup\",(0-3)),(\"service\",(0-1)),(\"signal\",(0-5)),(\"roam\",(0,1)),(\"battchg\",(0-5)),(\"callheld\",(0-2))"
#endif

#ifndef BTA_DM_AVOID_A2DP_ROLESWITCH_ON_INQUIRY
#define BTA_DM_AVOID_A2DP_ROLESWITCH_ON_INQUIRY FALSE
#endif

#ifndef BTA_GATTC_MAX_CACHE_CHAR
#define BTA_GATTC_MAX_CACHE_CHAR UC_BT_GATTC_MAX_CACHE_CHAR
#endif

/******************************************************************************
**
** Tracing:  Include trace header file here.
**
******************************************************************************/

/* Enable/disable BTSnoop memory logging */
#ifndef BTSNOOP_MEM
#define BTSNOOP_MEM FALSE
#endif

#include "common/bt_trace.h"

#endif /* BT_TARGET_H */
