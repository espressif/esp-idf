/******************************************************************************
 *
 *  Copyright (C) 2003-2014 Broadcom Corporation
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

/******************************************************************************
 *
 *  This is the public interface file for BTA, Broadcom's Bluetooth
 *  application layer for mobile phones.
 *
 ******************************************************************************/
#ifndef BTA_API_H
#define BTA_API_H

#include "bt_target.h"
#include "bt_types.h"
#include "btm_api.h"
// #include "uipc_msg.h"
#include "sdp_api.h"

#if BLE_INCLUDED == TRUE
#include "btm_ble_api.h"
#endif

/*****************************************************************************
**  Constants and data types
*****************************************************************************/

/* Status Return Value */
#define BTA_SUCCESS             0           /* Successful operation. */
#define BTA_FAILURE             1           /* Generic failure. */
#define BTA_PENDING             2           /* API cannot be completed right now */
#define BTA_BUSY                3
#define BTA_NO_RESOURCES        4
#define BTA_WRONG_MODE          5

typedef UINT8 tBTA_STATUS;

/*
 * Service ID
 *
 * NOTES: When you add a new Service ID for BTA AND require to change the value of BTA_MAX_SERVICE_ID,
 *        make sure that the correct security ID of the new service from Security service definitions (btm_api.h)
 *        should be added to bta_service_id_to_btm_srv_id_lkup_tbl table in bta_dm_act.c.
 */

#define BTA_RES_SERVICE_ID      0           /* Reserved */
#define BTA_SPP_SERVICE_ID      1           /* Serial port profile. */
#define BTA_DUN_SERVICE_ID      2           /* Dial-up networking profile. */
#define BTA_A2DP_SOURCE_SERVICE_ID      3   /* A2DP Source profile. */
#define BTA_LAP_SERVICE_ID      4           /* LAN access profile. */
#define BTA_HSP_SERVICE_ID      5           /* Headset profile. */
#define BTA_HFP_SERVICE_ID      6           /* Hands-free profile. */
#define BTA_OPP_SERVICE_ID      7           /* Object push  */
#define BTA_FTP_SERVICE_ID      8           /* File transfer */
#define BTA_CTP_SERVICE_ID      9           /* Cordless Terminal */
#define BTA_ICP_SERVICE_ID      10          /* Intercom Terminal */
#define BTA_SYNC_SERVICE_ID     11          /* Synchronization */
#define BTA_BPP_SERVICE_ID      12          /* Basic printing profile */
#define BTA_BIP_SERVICE_ID      13          /* Basic Imaging profile */
#define BTA_PANU_SERVICE_ID     14          /* PAN User */
#define BTA_NAP_SERVICE_ID      15          /* PAN Network access point */
#define BTA_GN_SERVICE_ID       16          /* PAN Group Ad-hoc networks */
#define BTA_SAP_SERVICE_ID      17          /* SIM Access profile */
#define BTA_A2DP_SINK_SERVICE_ID        18  /* A2DP Sink */
#define BTA_AVRCP_SERVICE_ID    19          /* A/V remote control */
#define BTA_HID_SERVICE_ID      20          /* HID */
#define BTA_VDP_SERVICE_ID      21          /* Video distribution */
#define BTA_PBAP_SERVICE_ID     22          /* PhoneBook Access Server*/
#define BTA_HSP_HS_SERVICE_ID   23          /* HFP HS role */
#define BTA_HFP_HS_SERVICE_ID   24          /* HSP HS role */
#define BTA_MAP_SERVICE_ID      25          /* Message Access Profile */
#define BTA_MN_SERVICE_ID       26          /* Message Notification Service */
#define BTA_HDP_SERVICE_ID      27          /* Health Device Profile */
#define BTA_PCE_SERVICE_ID      28          /* PhoneBook Access Client*/
#define BTA_SDP_SERVICE_ID      29          /* SDP Search*/
#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
/* BLE profile service ID */
#define BTA_BLE_SERVICE_ID      30          /* GATT profile */

// btla-specific ++
#define BTA_USER_SERVICE_ID     31          /* User requested UUID */

#define BTA_MAX_SERVICE_ID      32
// btla-specific --
#else
#define BTA_USER_SERVICE_ID     30          /* User requested UUID */
#define BTA_MAX_SERVICE_ID      31
#endif
/* service IDs (BTM_SEC_SERVICE_FIRST_EMPTY + 1) to (BTM_SEC_MAX_SERVICES - 1)
 * are used by BTA JV */
#define BTA_FIRST_JV_SERVICE_ID     (BTM_SEC_SERVICE_FIRST_EMPTY + 1)
#define BTA_LAST_JV_SERVICE_ID      (BTM_SEC_MAX_SERVICES - 1)

typedef UINT8 tBTA_SERVICE_ID;

/* Service ID Mask */
#define BTA_RES_SERVICE_MASK        0x00000001  /* Reserved */
#define BTA_SPP_SERVICE_MASK        0x00000002  /* Serial port profile. */
#define BTA_DUN_SERVICE_MASK        0x00000004  /* Dial-up networking profile. */
#define BTA_FAX_SERVICE_MASK        0x00000008  /* Fax profile. */
#define BTA_LAP_SERVICE_MASK        0x00000010  /* LAN access profile. */
#define BTA_HSP_SERVICE_MASK        0x00000020  /* HSP AG role. */
#define BTA_HFP_SERVICE_MASK        0x00000040  /* HFP AG role */
#define BTA_OPP_SERVICE_MASK        0x00000080  /* Object push  */
#define BTA_FTP_SERVICE_MASK        0x00000100  /* File transfer */
#define BTA_CTP_SERVICE_MASK        0x00000200  /* Cordless Terminal */
#define BTA_ICP_SERVICE_MASK        0x00000400  /* Intercom Terminal */
#define BTA_SYNC_SERVICE_MASK       0x00000800  /* Synchronization */
#define BTA_BPP_SERVICE_MASK        0x00001000  /* Print server */
#define BTA_BIP_SERVICE_MASK        0x00002000  /* Basic Imaging */
#define BTA_PANU_SERVICE_MASK       0x00004000  /* PAN User */
#define BTA_NAP_SERVICE_MASK        0x00008000  /* PAN Network access point */
#define BTA_GN_SERVICE_MASK         0x00010000  /* PAN Group Ad-hoc networks */
#define BTA_SAP_SERVICE_MASK        0x00020000  /* PAN Group Ad-hoc networks */
#define BTA_A2DP_SERVICE_MASK       0x00040000  /* Advanced audio distribution */
#define BTA_AVRCP_SERVICE_MASK      0x00080000  /* A/V remote control */
#define BTA_HID_SERVICE_MASK        0x00100000  /* HID */
#define BTA_VDP_SERVICE_MASK        0x00200000  /* Video distribution */
#define BTA_PBAP_SERVICE_MASK       0x00400000  /* Phone Book Server */
#define BTA_HSP_HS_SERVICE_MASK     0x00800000  /* HFP HS role */
#define BTA_HFP_HS_SERVICE_MASK     0x01000000  /* HSP HS role */
#define BTA_MAS_SERVICE_MASK        0x02000000  /* Message Access Profile */
#define BTA_MN_SERVICE_MASK         0x04000000  /* Message Notification Profile */
#define BTA_HL_SERVICE_MASK         0x08000000  /* Health Device Profile */
#define BTA_PCE_SERVICE_MASK        0x10000000  /* Phone Book Client */

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
#define BTA_BLE_SERVICE_MASK        0x20000000  /* GATT based service */
// btla-specific ++
#define BTA_USER_SERVICE_MASK       0x40000000  /* Message Notification Profile */
// btla-specific --
#else
// btla-specific ++
#define BTA_USER_SERVICE_MASK       0x20000000  /* Message Notification Profile */
// btla-specific --
#endif

#if BLE_INCLUDED == TRUE && BTA_GATT_INCLUDED == TRUE
#define BTA_ALL_SERVICE_MASK        0x3FFFFFFF  /* All services supported by BTA. */
#else
#define BTA_ALL_SERVICE_MASK        0x1FFFFFFF  /* All services supported by BTA. */
#endif

typedef UINT32 tBTA_SERVICE_MASK;

/* extended service mask, including mask with one or more GATT UUID */
typedef struct {
    tBTA_SERVICE_MASK   srvc_mask;
    UINT8               num_uuid;
    tBT_UUID            *p_uuid;
} tBTA_SERVICE_MASK_EXT;

/* Security Setting Mask */
#define BTA_SEC_NONE            BTM_SEC_NONE                                         /* No security. */
#define BTA_SEC_AUTHORIZE       (BTM_SEC_IN_AUTHORIZE )                              /* Authorization required (only needed for out going connection )*/
#define BTA_SEC_AUTHENTICATE    (BTM_SEC_IN_AUTHENTICATE | BTM_SEC_OUT_AUTHENTICATE) /* Authentication required. */
#define BTA_SEC_ENCRYPT         (BTM_SEC_IN_ENCRYPT | BTM_SEC_OUT_ENCRYPT)           /* Encryption required. */
#define BTA_SEC_MODE4_LEVEL4    (BTM_SEC_MODE4_LEVEL4)                               /* Mode 4 level 4 service, i.e. incoming/outgoing MITM and P-256 encryption */
#define BTA_SEC_MITM            (BTM_SEC_IN_MITM | BTM_SEC_OUT_MITM)                 /* Man-In-The_Middle protection */
#define BTA_SEC_IN_16_DIGITS    (BTM_SEC_IN_MIN_16_DIGIT_PIN)                        /* Min 16 digit for pin code */

typedef UINT16 tBTA_SEC;

/* Ignore for Discoverable, Connectable, Pairable and Connectable Paired only device modes */
#define BTA_DM_IGNORE           0x00FF

/* Ignore for Discoverable, Connectable only for LE modes */
#define BTA_DM_LE_IGNORE           0xFF00

#define BTA_APP_ID_PAN_MULTI    0xFE    /* app id for pan multiple connection */
#define BTA_ALL_APP_ID          0xFF

/* Discoverable Modes */
#define BTA_DM_NON_DISC         BTM_NON_DISCOVERABLE        /* Device is not discoverable. */
#define BTA_DM_GENERAL_DISC     BTM_GENERAL_DISCOVERABLE    /* General discoverable. */
#define BTA_DM_LIMITED_DISC     BTM_LIMITED_DISCOVERABLE    /* Limited discoverable. */
#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
#define BTA_DM_BLE_NON_DISCOVERABLE        BTM_BLE_NON_DISCOVERABLE        /* Device is not LE discoverable */
#define BTA_DM_BLE_GENERAL_DISCOVERABLE    BTM_BLE_GENERAL_DISCOVERABLE    /* Device is LE General discoverable */
#define BTA_DM_BLE_LIMITED_DISCOVERABLE    BTM_BLE_LIMITED_DISCOVERABLE    /* Device is LE Limited discoverable */
#endif
typedef UINT16 tBTA_DM_DISC;        /* this discoverability mode is a bit mask among BR mode and LE mode */

/* Connectable Modes */
#define BTA_DM_NON_CONN         BTM_NON_CONNECTABLE         /* Device is not connectable. */
#define BTA_DM_CONN             BTM_CONNECTABLE             /* Device is connectable. */
#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
#define BTA_DM_BLE_NON_CONNECTABLE      BTM_BLE_NON_CONNECTABLE     /* Device is LE non-connectable. */
#define BTA_DM_BLE_CONNECTABLE          BTM_BLE_CONNECTABLE         /* Device is LE connectable. */
#endif

// btla-specific ++
typedef UINT16 tBTA_DM_CONN;

#define BTA_TRANSPORT_UNKNOWN   0
#define BTA_TRANSPORT_BR_EDR    BT_TRANSPORT_BR_EDR
#define BTA_TRANSPORT_LE        BT_TRANSPORT_LE
typedef tBT_TRANSPORT tBTA_TRANSPORT;

/* Pairable Modes */
#define BTA_DM_PAIRABLE         1
#define BTA_DM_NON_PAIRABLE     0

/* Connectable Paired Only Mode */
#define BTA_DM_CONN_ALL         0
#define BTA_DM_CONN_PAIRED      1

/* Inquiry Modes */
#define BTA_DM_INQUIRY_NONE     BTM_INQUIRY_NONE            /*No BR inquiry. */
#define BTA_DM_GENERAL_INQUIRY  BTM_GENERAL_INQUIRY         /* Perform general inquiry. */
#define BTA_DM_LIMITED_INQUIRY  BTM_LIMITED_INQUIRY         /* Perform limited inquiry. */

#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
#define BTA_BLE_INQUIRY_NONE    BTM_BLE_INQUIRY_NONE
#define BTA_BLE_GENERAL_INQUIRY BTM_BLE_GENERAL_INQUIRY      /* Perform LE general inquiry. */
#define BTA_BLE_LIMITED_INQUIRY BTM_BLE_LIMITED_INQUIRY      /* Perform LE limited inquiry. */
#endif
typedef UINT8 tBTA_DM_INQ_MODE;

/* Inquiry Filter Type */
#define BTA_DM_INQ_CLR          BTM_CLR_INQUIRY_FILTER          /* Clear inquiry filter. */
#define BTA_DM_INQ_DEV_CLASS    BTM_FILTER_COND_DEVICE_CLASS    /* Filter on device class. */
#define BTA_DM_INQ_BD_ADDR      BTM_FILTER_COND_BD_ADDR         /* Filter on a specific  BD address. */

typedef UINT8 tBTA_DM_INQ_FILT;

/* Authorize Response */
#define BTA_DM_AUTH_PERM        0      /* Authorized for future connections to the service */
#define BTA_DM_AUTH_TEMP        1      /* Authorized for current connection only */
#define BTA_DM_NOT_AUTH         2      /* Not authorized for the service */

typedef UINT8 tBTA_AUTH_RESP;

/* M/S preferred roles */
#define BTA_ANY_ROLE          0x00
#define BTA_MASTER_ROLE_PREF  0x01
#define BTA_MASTER_ROLE_ONLY  0x02
#define BTA_SLAVE_ROLE_ONLY   0x03     /* Used for PANU only, skip role switch to master */

typedef UINT8 tBTA_PREF_ROLES;

enum {

    BTA_DM_NO_SCATTERNET,        /* Device doesn't support scatternet, it might
                                    support "role switch during connection" for
                                    an incoming connection, when it already has
                                    another connection in master role */
    BTA_DM_PARTIAL_SCATTERNET,   /* Device supports partial scatternet. It can have
                                    simulateous connection in Master and Slave roles
                                    for short period of time */
    BTA_DM_FULL_SCATTERNET       /* Device can have simultaneous connection in master
                                    and slave roles */

};


/* Inquiry filter device class condition */
typedef struct {
    DEV_CLASS       dev_class;        /* device class of interest */
    DEV_CLASS       dev_class_mask;   /* mask to determine the bits of device class of interest */
} tBTA_DM_COD_COND;


/* Inquiry Filter Condition */
typedef union {
    BD_ADDR              bd_addr;            /* BD address of  device to filter. */
    tBTA_DM_COD_COND     dev_class_cond;     /* Device class filter condition */
} tBTA_DM_INQ_COND;

/* Inquiry Parameters */
typedef struct {
    tBTA_DM_INQ_MODE    mode;           /* Inquiry mode, limited or general. */
    UINT8               duration;       /* Inquiry duration in 1.28 sec units. */
    UINT8               max_resps;      /* Maximum inquiry responses.  Set to zero for unlimited responses. */
    BOOLEAN             report_dup;     /* report duplicated inquiry response with higher RSSI value */
    tBTA_DM_INQ_FILT    filter_type;    /* Filter condition type. */
    tBTA_DM_INQ_COND    filter_cond;    /* Filter condition data. */
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
    UINT8               intl_duration[4];/*duration array storing the interleave scan's time portions*/
#endif
} tBTA_DM_INQ;

typedef struct {
    UINT8   bta_dm_eir_min_name_len;        /* minimum length of local name when it is shortened */
#if (BTA_EIR_CANNED_UUID_LIST == TRUE)
    UINT8   bta_dm_eir_uuid16_len;          /* length of 16-bit UUIDs */
    UINT8  *bta_dm_eir_uuid16;              /* 16-bit UUIDs */
#else
    UINT32  uuid_mask[BTM_EIR_SERVICE_ARRAY_SIZE]; /* mask of UUID list in EIR */
#endif
    INT8   *bta_dm_eir_inq_tx_power;        /* Inquiry TX power         */
    UINT8   bta_dm_eir_flag_len;            /* length of flags in bytes */
    UINT8  *bta_dm_eir_flags;               /* flags for EIR */
    UINT8   bta_dm_eir_manufac_spec_len;    /* length of manufacturer specific in bytes */
    UINT8  *bta_dm_eir_manufac_spec;        /* manufacturer specific */
    UINT8   bta_dm_eir_additional_len;      /* length of additional data in bytes */
    UINT8  *bta_dm_eir_additional;          /* additional data */
} tBTA_DM_EIR_CONF;

#if BLE_INCLUDED == TRUE
/* ADV data flag bit definition used for BTM_BLE_AD_TYPE_FLAG */
#define BTA_BLE_LIMIT_DISC_FLAG     BTM_BLE_LIMIT_DISC_FLAG
#define BTA_BLE_GEN_DISC_FLAG       BTM_BLE_GEN_DISC_FLAG
#define BTA_BLE_BREDR_NOT_SPT       BTM_BLE_BREDR_NOT_SPT
#define BTA_BLE_DMT_CONTROLLER_SPT  BTM_BLE_DMT_CONTROLLER_SPT
#define BTA_BLE_DMT_HOST_SPT        BTM_BLE_DMT_HOST_SPT
#define BTA_BLE_NON_LIMIT_DISC_FLAG BTM_BLE_NON_LIMIT_DISC_FLAG
#define BTA_BLE_ADV_FLAG_MASK       BTM_BLE_ADV_FLAG_MASK
#define BTA_BLE_LIMIT_DISC_MASK     BTM_BLE_LIMIT_DISC_MASK

/* ADV data bit mask */
#define BTA_BLE_AD_BIT_DEV_NAME        BTM_BLE_AD_BIT_DEV_NAME
#define BTA_BLE_AD_BIT_FLAGS           BTM_BLE_AD_BIT_FLAGS
#define BTA_BLE_AD_BIT_MANU            BTM_BLE_AD_BIT_MANU
#define BTA_BLE_AD_BIT_TX_PWR          BTM_BLE_AD_BIT_TX_PWR
#define BTA_BLE_AD_BIT_INT_RANGE       BTM_BLE_AD_BIT_INT_RANGE
#define BTA_BLE_AD_BIT_SERVICE         BTM_BLE_AD_BIT_SERVICE
#define BTA_BLE_AD_BIT_APPEARANCE      BTM_BLE_AD_BIT_APPEARANCE
#define BTA_BLE_AD_BIT_PROPRIETARY     BTM_BLE_AD_BIT_PROPRIETARY
#define BTA_DM_BLE_AD_BIT_SERVICE_SOL     BTM_BLE_AD_BIT_SERVICE_SOL
#define BTA_DM_BLE_AD_BIT_SERVICE_DATA    BTM_BLE_AD_BIT_SERVICE_DATA
#define BTA_DM_BLE_AD_BIT_SIGN_DATA       BTM_BLE_AD_BIT_SIGN_DATA
#define BTA_DM_BLE_AD_BIT_SERVICE_128SOL  BTM_BLE_AD_BIT_SERVICE_128SOL
#define BTA_DM_BLE_AD_BIT_PUBLIC_ADDR     BTM_BLE_AD_BIT_PUBLIC_ADDR
#define BTA_DM_BLE_AD_BIT_RANDOM_ADDR     BTM_BLE_AD_BIT_RANDOM_ADDR
#define BTA_DM_BLE_AD_BIT_SERVICE_128     BTM_BLE_AD_BIT_SERVICE_128      /*128-bit Service UUIDs*/

typedef  tBTM_BLE_AD_MASK  tBTA_BLE_AD_MASK;

/* slave preferred connection interval range */
typedef struct {
    UINT16  low;
    UINT16  hi;

} tBTA_BLE_INT_RANGE;

/* Service tag supported in the device */
typedef struct {
    UINT8       num_service;
    BOOLEAN     list_cmpl;
    UINT16      *p_uuid;
} tBTA_BLE_SERVICE;


typedef struct {
    UINT8       len;
    UINT8      *p_val;
} tBTA_BLE_MANU;

typedef struct {
    UINT8       adv_type;
    UINT8       len;
    UINT8       *p_val;     /* number of len byte */
} tBTA_BLE_PROP_ELEM;

/* vendor proprietary adv type */
typedef struct {
    UINT8                   num_elem;
    tBTA_BLE_PROP_ELEM      *p_elem;
} tBTA_BLE_PROPRIETARY;

typedef struct {
    tBT_UUID    service_uuid;
    UINT8       len;
    UINT8      *p_val;
} tBTA_BLE_SERVICE_DATA;

typedef tBTM_BLE_128SERVICE tBTA_BLE_128SERVICE;
typedef tBTM_BLE_32SERVICE  tBTA_BLE_32SERVICE;

typedef struct {
    tBTA_BLE_INT_RANGE      int_range;          /* slave prefered conn interval range */
    tBTA_BLE_MANU           *p_manu;            /* manufacturer data */
    tBTA_BLE_SERVICE        *p_services;        /* 16 bits services */
    tBTA_BLE_128SERVICE     *p_services_128b;   /* 128 bits service */
    tBTA_BLE_32SERVICE      *p_service_32b;     /* 32 bits Service UUID */
    tBTA_BLE_SERVICE        *p_sol_services;    /* 16 bits services Solicitation UUIDs */
    tBTA_BLE_32SERVICE      *p_sol_service_32b; /* List of 32 bit Service Solicitation UUIDs */
    tBTA_BLE_128SERVICE     *p_sol_service_128b;/* List of 128 bit Service Solicitation UUIDs */
    tBTA_BLE_PROPRIETARY    *p_proprietary;     /* proprietary data */
    tBTA_BLE_SERVICE_DATA   *p_service_data;    /* service data */
    UINT16                  appearance;         /* appearance data */
    UINT8                   flag;
    UINT8                   tx_power;
} tBTA_BLE_ADV_DATA;

typedef void (tBTA_SET_ADV_DATA_CMPL_CBACK) (tBTA_STATUS status);

/* advertising channel map */
#define BTA_BLE_ADV_CHNL_37 BTM_BLE_ADV_CHNL_37
#define BTA_BLE_ADV_CHNL_38 BTM_BLE_ADV_CHNL_38
#define BTA_BLE_ADV_CHNL_39 BTM_BLE_ADV_CHNL_39
typedef tBTM_BLE_ADV_CHNL_MAP tBTA_BLE_ADV_CHNL_MAP; /* use as a bit mask */

/* advertising filter policy */
typedef tBTM_BLE_AFP   tBTA_BLE_AFP;

/* adv event type */
#define BTA_BLE_CONNECT_EVT         BTM_BLE_CONNECT_EVT     /* Connectable undirected advertising */
#define BTA_BLE_CONNECT_DIR_EVT     BTM_BLE_CONNECT_DIR_EVT /* Connectable directed advertising */
#define BTA_BLE_DISCOVER_EVT        BTM_BLE_DISCOVER_EVT    /* Scannable undirected advertising */
#define BTA_BLE_NON_CONNECT_EVT     BTM_BLE_NON_CONNECT_EVT /* Non connectable undirected advertising */
typedef UINT8 tBTA_BLE_ADV_EVT;

/* adv tx power level */
#define BTA_BLE_ADV_TX_POWER_MIN        0           /* minimum tx power */
#define BTA_BLE_ADV_TX_POWER_LOW        1           /* low tx power     */
#define BTA_BLE_ADV_TX_POWER_MID        2           /* middle tx power  */
#define BTA_BLE_ADV_TX_POWER_UPPER      3           /* upper tx power   */
#define BTA_BLE_ADV_TX_POWER_MAX        4           /* maximum tx power */
typedef UINT8 tBTA_BLE_ADV_TX_POWER;

/* advertising instance parameters */
typedef struct {
    UINT16                  adv_int_min;            /* minimum adv interval */
    UINT16                  adv_int_max;            /* maximum adv interval */
    tBTA_BLE_ADV_EVT        adv_type;               /* adv event type */
    tBTA_BLE_ADV_CHNL_MAP   channel_map;            /* adv channel map */
    tBTA_BLE_AFP            adv_filter_policy;      /* advertising filter policy */
    tBTA_BLE_ADV_TX_POWER   tx_power;               /* adv tx power */
} tBTA_BLE_ADV_PARAMS;

/* These are the fields returned in each device adv packet.  It
** is returned in the results callback if registered.
*/
typedef struct {
    UINT8               conn_mode;
    tBTA_BLE_AD_MASK    ad_mask;        /* mask of the valid adv data field */
    UINT8               flag;
    UINT8               tx_power_level;
    UINT8               remote_name_len;
    UINT8               *p_remote_name;
    tBTA_BLE_SERVICE    service;
} tBTA_BLE_INQ_DATA;

enum {
    BTA_BLE_BATCH_SCAN_MODE_PASS = 1,
    BTA_BLE_BATCH_SCAN_MODE_ACTI = 2,
    BTA_BLE_BATCH_SCAN_MODE_PASS_ACTI = 3
};
typedef UINT8 tBTA_BLE_BATCH_SCAN_MODE;

enum {
    BTA_BLE_DISCARD_OLD_ITEMS = 0,
    BTA_BLE_DISCARD_LOWER_RSSI_ITEMS = 1
};
typedef UINT8 tBTA_BLE_DISCARD_RULE;

enum {
    BTA_BLE_ADV_SEEN_FIRST_TIME = 0,
    BTA_BLE_ADV_TRACKING_TIMEOUT = 1
};
typedef UINT8 tBTA_BLE_ADV_CHANGE_REASON;

enum {
    BTA_BLE_BATCH_SCAN_ENB_EVT      = 1,
    BTA_BLE_BATCH_SCAN_CFG_STRG_EVT = 2,
    BTA_BLE_BATCH_SCAN_DATA_EVT     = 3,
    BTA_BLE_BATCH_SCAN_THRES_EVT    = 4,
    BTA_BLE_BATCH_SCAN_PARAM_EVT    = 5,
    BTA_BLE_BATCH_SCAN_DIS_EVT      = 6
};
typedef tBTM_BLE_BATCH_SCAN_EVT tBTA_BLE_BATCH_SCAN_EVT;

typedef tBTM_BLE_TRACK_ADV_ACTION tBTA_BLE_TRACK_ADV_ACTION;
#endif

/* BLE customer specific feature function type definitions */
/* data type used on customer specific feature for RSSI monitoring */
#define BTA_BLE_RSSI_ALERT_HI        0
#define BTA_BLE_RSSI_ALERT_RANGE     1
#define BTA_BLE_RSSI_ALERT_LO        2
typedef UINT8 tBTA_DM_BLE_RSSI_ALERT_TYPE;

#define BTA_BLE_RSSI_ALERT_NONE         BTM_BLE_RSSI_ALERT_NONE         /*    (0) */
#define BTA_BLE_RSSI_ALERT_HI_BIT       BTM_BLE_RSSI_ALERT_HI_BIT       /*    (1) */
#define BTA_BLE_RSSI_ALERT_RANGE_BIT    BTM_BLE_RSSI_ALERT_RANGE_BIT    /*    (1 << 1) */
#define BTA_BLE_RSSI_ALERT_LO_BIT       BTM_BLE_RSSI_ALERT_LO_BIT       /*    (1 << 2) */
typedef UINT8     tBTA_DM_BLE_RSSI_ALERT_MASK;


typedef void (tBTA_DM_BLE_RSSI_CBACK) (BD_ADDR bd_addr, tBTA_DM_BLE_RSSI_ALERT_TYPE alert_type, INT8 rssi);

/* max number of filter spot for different filter type */
#define BTA_DM_BLE_MAX_UUID_FILTER     BTM_BLE_MAX_UUID_FILTER    /* 8 */
#define BTA_DM_BLE_MAX_ADDR_FILTER     BTM_BLE_MAX_ADDR_FILTER    /* 8 */
#define BTA_DM_BLE_PF_STR_COND_MAX     BTM_BLE_PF_STR_COND_MAX    /* 4    apply to manu data , or local name */
#define BTA_DM_BLE_PF_STR_LEN_MAX      BTM_BLE_PF_STR_LEN_MAX     /* match for first 20 bytes */

#define BTA_DM_BLE_PF_LOGIC_OR              0
#define BTA_DM_BLE_PF_LOGIC_AND             1
typedef UINT8 tBTA_DM_BLE_PF_LOGIC_TYPE;

enum {
    BTA_DM_BLE_SCAN_COND_ADD,
    BTA_DM_BLE_SCAN_COND_DELETE,
    BTA_DM_BLE_SCAN_COND_CLEAR = 2
};
typedef UINT8 tBTA_DM_BLE_SCAN_COND_OP;

/* ADV payload filtering vendor specific call event */
enum {
    BTA_BLE_SCAN_PF_ENABLE_EVT = 7,
    BTA_BLE_SCAN_PF_COND_EVT
};

/* filter selection bit index  */
#define BTA_DM_BLE_PF_ADDR_FILTER          BTM_BLE_PF_ADDR_FILTER
#define BTA_DM_BLE_PF_SRVC_DATA            BTM_BLE_PF_SRVC_DATA
#define BTA_DM_BLE_PF_SRVC_UUID            BTM_BLE_PF_SRVC_UUID
#define BTA_DM_BLE_PF_SRVC_SOL_UUID        BTM_BLE_PF_SRVC_SOL_UUID
#define BTA_DM_BLE_PF_LOCAL_NAME           BTM_BLE_PF_LOCAL_NAME
#define BTA_DM_BLE_PF_MANU_DATA            BTM_BLE_PF_MANU_DATA
#define BTA_DM_BLE_PF_SRVC_DATA_PATTERN    BTM_BLE_PF_SRVC_DATA_PATTERN
#define BTA_DM_BLE_PF_TYPE_ALL             BTM_BLE_PF_TYPE_ALL
#define BTA_DM_BLE_PF_TYPE_MAX             BTM_BLE_PF_TYPE_MAX
typedef UINT8   tBTA_DM_BLE_PF_COND_TYPE;

typedef union {
    UINT16              uuid16_mask;
    UINT32              uuid32_mask;
    UINT8               uuid128_mask[LEN_UUID_128];
} tBTA_DM_BLE_PF_COND_MASK;

typedef struct {
    tBLE_BD_ADDR                *p_target_addr;     /* target address, if NULL, generic UUID filter */
    tBT_UUID                    uuid;           /* UUID condition */
    tBTA_DM_BLE_PF_LOGIC_TYPE   cond_logic;    /* AND/OR */
    tBTA_DM_BLE_PF_COND_MASK    *p_uuid_mask;           /* UUID condition mask, if NULL, match exact as UUID condition */
} tBTA_DM_BLE_PF_UUID_COND;

typedef struct {
    UINT8                   data_len;       /* <= 20 bytes */
    UINT8                   *p_data;
} tBTA_DM_BLE_PF_LOCAL_NAME_COND;

typedef struct {
    UINT16                  company_id;     /* company ID */
    UINT8                   data_len;       /* <= 20 bytes */
    UINT8                   *p_pattern;
    UINT16                  company_id_mask; /* UUID value mask */
    UINT8                   *p_pattern_mask; /* Manufacturer data matching mask, same length
                                                as data pattern, set to all 0xff, match exact data */
} tBTA_DM_BLE_PF_MANU_COND;

typedef struct {
    UINT16                  uuid;     /* service ID */
    UINT8                   data_len;       /* <= 20 bytes */
    UINT8                   *p_pattern;
    UINT8                   *p_pattern_mask; /* Service data matching mask, same length
                                                as data pattern, set to all 0xff, match exact data */
} tBTA_DM_BLE_PF_SRVC_PATTERN_COND;

typedef union {
    tBLE_BD_ADDR                            target_addr;
    tBTA_DM_BLE_PF_LOCAL_NAME_COND             local_name; /* lcoal name filtering */
    tBTA_DM_BLE_PF_MANU_COND                   manu_data;  /* manufactuer data filtering */
    tBTA_DM_BLE_PF_UUID_COND                   srvc_uuid;  /* service UUID filtering */
    tBTA_DM_BLE_PF_UUID_COND                   solicitate_uuid;   /* solicitated service UUID filtering */
    tBTA_DM_BLE_PF_SRVC_PATTERN_COND           srvc_data;      /* service data pattern */
} tBTA_DM_BLE_PF_COND_PARAM;

typedef UINT8 tBTA_DM_BLE_PF_FILT_INDEX;
typedef UINT8 tBTA_DM_BLE_PF_AVBL_SPACE;

typedef INT8 tBTA_DM_RSSI_VALUE;
typedef UINT8 tBTA_DM_LINK_QUALITY_VALUE;


typedef UINT8 tBTA_SIG_STRENGTH_MASK;


/* Security Callback Events */
#define BTA_DM_ENABLE_EVT               0       /* Enable Event */
#define BTA_DM_DISABLE_EVT              1       /* Disable Event */
#define BTA_DM_PIN_REQ_EVT              2       /* PIN request. */
#define BTA_DM_AUTH_CMPL_EVT            3       /* Authentication complete indication. */
#define BTA_DM_AUTHORIZE_EVT            4       /* Authorization request. */
#define BTA_DM_LINK_UP_EVT              5       /* Connection UP event */
#define BTA_DM_LINK_DOWN_EVT            6       /* Connection DOWN event */
#define BTA_DM_SIG_STRENGTH_EVT         7       /* Signal strength for bluetooth connection */
#define BTA_DM_BUSY_LEVEL_EVT           8       /* System busy level */
#define BTA_DM_BOND_CANCEL_CMPL_EVT     9       /* Bond cancel complete indication */
#define BTA_DM_SP_CFM_REQ_EVT           10      /* Simple Pairing User Confirmation request. */
#define BTA_DM_SP_KEY_NOTIF_EVT         11      /* Simple Pairing Passkey Notification */
#define BTA_DM_SP_RMT_OOB_EVT           12      /* Simple Pairing Remote OOB Data request. */
#define BTA_DM_SP_KEYPRESS_EVT          13      /* Key press notification event. */
#define BTA_DM_ROLE_CHG_EVT             14      /* Role Change event. */
#define BTA_DM_BLE_KEY_EVT              15      /* BLE SMP key event for peer device keys */
#define BTA_DM_BLE_SEC_REQ_EVT          16      /* BLE SMP security request */
#define BTA_DM_BLE_PASSKEY_NOTIF_EVT    17      /* SMP passkey notification event */
#define BTA_DM_BLE_PASSKEY_REQ_EVT      18      /* SMP passkey request event */
#define BTA_DM_BLE_OOB_REQ_EVT          19      /* SMP OOB request event */
#define BTA_DM_BLE_LOCAL_IR_EVT         20      /* BLE local IR event */
#define BTA_DM_BLE_LOCAL_ER_EVT         21      /* BLE local ER event */
#define BTA_DM_BLE_NC_REQ_EVT           22      /* SMP Numeric Comparison request event */
// btla-specific ++
#define BTA_DM_SP_RMT_OOB_EXT_EVT       23      /* Simple Pairing Remote OOB Extended Data request. */
#define BTA_DM_BLE_AUTH_CMPL_EVT        24      /* BLE Auth complete */
// btla-specific --
#define BTA_DM_DEV_UNPAIRED_EVT         25
#define BTA_DM_HW_ERROR_EVT             26      /* BT Chip H/W error */
#define BTA_DM_LE_FEATURES_READ         27      /* Cotroller specific LE features are read */
#define BTA_DM_ENER_INFO_READ           28      /* Energy info read */
typedef UINT8 tBTA_DM_SEC_EVT;

/* Structure associated with BTA_DM_ENABLE_EVT */
typedef struct {
    tBTA_STATUS    status;
} tBTA_DM_ENABLE;

/* Structure associated with BTA_DM_PIN_REQ_EVT */
typedef struct {
    /* Note: First 3 data members must be, bd_addr, dev_class, and bd_name in order */
    BD_ADDR         bd_addr;            /* BD address peer device. */
    DEV_CLASS       dev_class;          /* Class of Device */
    BD_NAME         bd_name;            /* Name of peer device. */
    BOOLEAN         min_16_digit;       /* TRUE if the pin returned must be at least 16 digits */
} tBTA_DM_PIN_REQ;

/* BLE related definition */

#define BTA_DM_AUTH_FAIL_BASE                   (HCI_ERR_MAX_ERR + 10)
#define BTA_DM_AUTH_CONVERT_SMP_CODE(x)        (BTA_DM_AUTH_FAIL_BASE + (x))
#define BTA_DM_AUTH_SMP_PASSKEY_FAIL             BTA_DM_AUTH_CONVERT_SMP_CODE (SMP_PASSKEY_ENTRY_FAIL)
#define BTA_DM_AUTH_SMP_OOB_FAIL                (BTA_DM_AUTH_FAIL_BASE + SMP_OOB_FAIL)
#define BTA_DM_AUTH_SMP_PAIR_AUTH_FAIL          (BTA_DM_AUTH_FAIL_BASE + SMP_PAIR_AUTH_FAIL)
#define BTA_DM_AUTH_SMP_CONFIRM_VALUE_FAIL      (BTA_DM_AUTH_FAIL_BASE + SMP_CONFIRM_VALUE_ERR)
#define BTA_DM_AUTH_SMP_PAIR_NOT_SUPPORT        (BTA_DM_AUTH_FAIL_BASE + SMP_PAIR_NOT_SUPPORT)
#define BTA_DM_AUTH_SMP_ENC_KEY_SIZE            (BTA_DM_AUTH_FAIL_BASE + SMP_ENC_KEY_SIZE)
#define BTA_DM_AUTH_SMP_INVALID_CMD             (BTA_DM_AUTH_FAIL_BASE + SMP_INVALID_CMD)
#define BTA_DM_AUTH_SMP_UNKNOWN_ERR             (BTA_DM_AUTH_FAIL_BASE + SMP_PAIR_FAIL_UNKNOWN)
#define BTA_DM_AUTH_SMP_REPEATED_ATTEMPT        (BTA_DM_AUTH_FAIL_BASE + SMP_REPEATED_ATTEMPTS)
#define BTA_DM_AUTH_SMP_INVALID_PARAMETERS      (BTA_DM_AUTH_FAIL_BASE + SMP_INVALID_PARAMETERS)
#define BTA_DM_AUTH_SMP_INTERNAL_ERR            (BTA_DM_AUTH_FAIL_BASE + SMP_PAIR_INTERNAL_ERR)
#define BTA_DM_AUTH_SMP_UNKNOWN_IO              (BTA_DM_AUTH_FAIL_BASE + SMP_UNKNOWN_IO_CAP)
#define BTA_DM_AUTH_SMP_INIT_FAIL               (BTA_DM_AUTH_FAIL_BASE + SMP_INIT_FAIL)
#define BTA_DM_AUTH_SMP_CONFIRM_FAIL            (BTA_DM_AUTH_FAIL_BASE + SMP_CONFIRM_FAIL)
#define BTA_DM_AUTH_SMP_BUSY                    (BTA_DM_AUTH_FAIL_BASE + SMP_BUSY)
#define BTA_DM_AUTH_SMP_ENC_FAIL                (BTA_DM_AUTH_FAIL_BASE + SMP_ENC_FAIL)
#define BTA_DM_AUTH_SMP_RSP_TIMEOUT             (BTA_DM_AUTH_FAIL_BASE + SMP_RSP_TIMEOUT)

/* connection parameter boundary value and dummy value */
#define BTA_DM_BLE_SCAN_INT_MIN          BTM_BLE_SCAN_INT_MIN
#define BTA_DM_BLE_SCAN_INT_MAX          BTM_BLE_SCAN_INT_MAX
#define BTA_DM_BLE_SCAN_WIN_MIN          BTM_BLE_SCAN_WIN_MIN
#define BTA_DM_BLE_SCAN_WIN_MAX          BTM_BLE_SCAN_WIN_MAX
#define BTA_DM_BLE_CONN_INT_MIN          BTM_BLE_CONN_INT_MIN
#define BTA_DM_BLE_CONN_INT_MAX          BTM_BLE_CONN_INT_MAX
#define BTA_DM_BLE_CONN_LATENCY_MAX      BTM_BLE_CONN_LATENCY_MAX
#define BTA_DM_BLE_CONN_SUP_TOUT_MIN     BTM_BLE_CONN_SUP_TOUT_MIN
#define BTA_DM_BLE_CONN_SUP_TOUT_MAX     BTM_BLE_CONN_SUP_TOUT_MAX
#define BTA_DM_BLE_CONN_PARAM_UNDEF      BTM_BLE_CONN_PARAM_UNDEF  /* use this value when a specific value not to be overwritten */


#define BTA_LE_KEY_PENC      BTM_LE_KEY_PENC  /* encryption information of peer device */
#define BTA_LE_KEY_PID       BTM_LE_KEY_PID   /* identity key of the peer device */
#define BTA_LE_KEY_PCSRK     BTM_LE_KEY_PCSRK   /* peer SRK */
#define BTA_LE_KEY_LENC      BTM_LE_KEY_LENC        /* master role security information:div */
#define BTA_LE_KEY_LID       BTM_LE_KEY_LID         /* master device ID key */
#define BTA_LE_KEY_LCSRK     BTM_LE_KEY_LCSRK        /* local CSRK has been deliver to peer */
typedef UINT8 tBTA_LE_KEY_TYPE; /* can be used as a bit mask */


typedef tBTM_LE_PENC_KEYS  tBTA_LE_PENC_KEYS ;
typedef tBTM_LE_PCSRK_KEYS tBTA_LE_PCSRK_KEYS;
typedef tBTM_LE_LENC_KEYS  tBTA_LE_LENC_KEYS  ;
typedef tBTM_LE_LCSRK_KEYS tBTA_LE_LCSRK_KEYS ;
typedef tBTM_LE_PID_KEYS   tBTA_LE_PID_KEYS ;

typedef union {
    tBTA_LE_PENC_KEYS   penc_key;       /* received peer encryption key */
    tBTA_LE_PCSRK_KEYS  psrk_key;       /* received peer device SRK */
    tBTA_LE_PID_KEYS    pid_key;        /* peer device ID key */
    tBTA_LE_LENC_KEYS   lenc_key;       /* local encryption reproduction keys LTK = = d1(ER,DIV,0)*/
    tBTA_LE_LCSRK_KEYS  lcsrk_key;      /* local device CSRK = d1(ER,DIV,1)*/
    tBTA_LE_PID_KEYS    lid_key;        /* local device ID key for the particular remote */
} tBTA_LE_KEY_VALUE;

#define BTA_BLE_LOCAL_KEY_TYPE_ID         1
#define BTA_BLE_LOCAL_KEY_TYPE_ER         2
typedef UINT8 tBTA_DM_BLE_LOCAL_KEY_MASK;

typedef struct {
    BT_OCTET16       ir;
    BT_OCTET16       irk;
    BT_OCTET16       dhk;
} tBTA_BLE_LOCAL_ID_KEYS;

#define BTA_DM_SEC_GRANTED              BTA_SUCCESS
#define BTA_DM_SEC_PAIR_NOT_SPT         BTA_DM_AUTH_SMP_PAIR_NOT_SUPPORT
#define BTA_DM_SEC_REP_ATTEMPTS         BTA_DM_AUTH_SMP_REPEATED_ATTEMPT
typedef UINT8 tBTA_DM_BLE_SEC_GRANT;


#define BTA_DM_BLE_ONN_NONE             BTM_BLE_CONN_NONE
#define BTA_DM_BLE_CONN_AUTO            BTM_BLE_CONN_AUTO
#define BTA_DM_BLE_CONN_SELECTIVE       BTM_BLE_CONN_SELECTIVE
typedef UINT8 tBTA_DM_BLE_CONN_TYPE;

typedef BOOLEAN (tBTA_DM_BLE_SEL_CBACK)(BD_ADDR random_bda, UINT8 *p_remote_name);

/* Structure associated with BTA_DM_BLE_SEC_REQ_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    BD_NAME         bd_name;        /* peer device name */
} tBTA_DM_BLE_SEC_REQ;

typedef struct {
    BD_ADDR                 bd_addr;        /* peer address */
    tBTM_LE_KEY_TYPE        key_type;
    tBTM_LE_KEY_VALUE       *p_key_value;
} tBTA_DM_BLE_KEY;

/* Structure associated with BTA_DM_AUTH_CMPL_EVT */
typedef struct {
    BD_ADDR         bd_addr;            /* BD address peer device. */
    BD_NAME         bd_name;            /* Name of peer device. */
    BOOLEAN         key_present;        /* Valid link key value in key element */
    LINK_KEY        key;                /* Link key associated with peer device. */
    UINT8           key_type;           /* The type of Link Key */
    BOOLEAN         success;            /* TRUE of authentication succeeded, FALSE if failed. */
    UINT8           fail_reason;        /* The HCI reason/error code for when success=FALSE */
    tBLE_ADDR_TYPE  addr_type;          /* Peer device address type */
    tBT_DEVICE_TYPE dev_type;
} tBTA_DM_AUTH_CMPL;


/* Structure associated with BTA_DM_AUTHORIZE_EVT */
typedef struct {
    BD_ADDR         bd_addr;            /* BD address peer device. */
    BD_NAME         bd_name;            /* Name of peer device. */
    tBTA_SERVICE_ID service;            /* Service ID to authorize. */
// btla-specific ++
    DEV_CLASS      dev_class;
// btla-specific --
} tBTA_DM_AUTHORIZE;

/* Structure associated with BTA_DM_LINK_UP_EVT */
typedef struct {
    BD_ADDR         bd_addr;            /* BD address peer device. */
#if BLE_INCLUDED == TRUE
    tBTA_TRANSPORT  link_type;
#endif
} tBTA_DM_LINK_UP;

/* Structure associated with BTA_DM_LINK_DOWN_EVT */
typedef struct {
    BD_ADDR         bd_addr;            /* BD address peer device. */
    UINT8           status;             /* connection open/closed */
    BOOLEAN         is_removed;         /* TRUE if device is removed when link is down */
#if BLE_INCLUDED == TRUE
    tBTA_TRANSPORT  link_type;
#endif
} tBTA_DM_LINK_DOWN;

/* Structure associated with BTA_DM_ROLE_CHG_EVT */
typedef struct {
    BD_ADDR         bd_addr;            /* BD address peer device. */
    UINT8           new_role;           /* the new connection role */
} tBTA_DM_ROLE_CHG;

/* Structure associated with BTA_DM_BUSY_LEVEL_EVT */
typedef struct {
    UINT8           level;     /* when paging or inquiring, level is 10.
                                    Otherwise, the number of ACL links */
    UINT8           level_flags; /* indicates individual flags */
} tBTA_DM_BUSY_LEVEL;

#define BTA_IO_CAP_OUT      BTM_IO_CAP_OUT      /* 0 DisplayOnly */
#define BTA_IO_CAP_IO       BTM_IO_CAP_IO       /* 1 DisplayYesNo */
#define BTA_IO_CAP_IN       BTM_IO_CAP_IN       /* 2 KeyboardOnly */
#define BTA_IO_CAP_NONE     BTM_IO_CAP_NONE     /* 3 NoInputNoOutput */
#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
#define BTA_IO_CAP_KBDISP   BTM_IO_CAP_KBDISP   /* 4 Keyboard display */
#endif
typedef tBTM_IO_CAP     tBTA_IO_CAP;

#define BTA_AUTH_SP_NO    BTM_AUTH_SP_NO      /* 0 MITM Protection Not Required - Single Profile/non-bonding
                                                Numeric comparison with automatic accept allowed */
#define BTA_AUTH_SP_YES   BTM_AUTH_SP_YES     /* 1 MITM Protection Required - Single Profile/non-bonding
                                                Use IO Capabilities to determine authentication procedure */
#define BTA_AUTH_AP_NO    BTM_AUTH_AP_NO      /* 2 MITM Protection Not Required - All Profiles/dedicated bonding
                                                Numeric comparison with automatic accept allowed */
#define BTA_AUTH_AP_YES   BTM_AUTH_AP_YES     /* 3 MITM Protection Required - All Profiles/dedicated bonding
                                                Use IO Capabilities to determine authentication procedure */
#define BTA_AUTH_SPGB_NO  BTM_AUTH_SPGB_NO    /* 4 MITM Protection Not Required - Single Profiles/general bonding
                                                Numeric comparison with automatic accept allowed */
#define BTA_AUTH_SPGB_YES BTM_AUTH_SPGB_YES   /* 5 MITM Protection Required - Single Profiles/general bonding
                                                Use IO Capabilities to determine authentication procedure */
typedef tBTM_AUTH_REQ   tBTA_AUTH_REQ;

#define BTA_AUTH_DD_BOND    BTM_AUTH_DD_BOND  /* 2 this bit is set for dedicated bonding */
#define BTA_AUTH_GEN_BOND   BTM_AUTH_SPGB_NO  /* 4 this bit is set for general bonding */
#define BTA_AUTH_BONDS      BTM_AUTH_BONDS    /* 6 the general/dedicated bonding bits  */

#define BTA_LE_AUTH_NO_BOND    BTM_LE_AUTH_REQ_NO_BOND  /* 0*/
#define BTA_LE_AUTH_BOND       BTM_LE_AUTH_REQ_BOND     /* 1 << 0 */
#define BTA_LE_AUTH_REQ_MITM   BTM_LE_AUTH_REQ_MITM    /* 1 << 2 */

#define BTA_LE_AUTH_REQ_SC_ONLY         BTM_LE_AUTH_REQ_SC_ONLY         /* 1 << 3 */
#define BTA_LE_AUTH_REQ_SC_BOND         BTM_LE_AUTH_REQ_SC_BOND      /* 1001 */
#define BTA_LE_AUTH_REQ_SC_MITM         BTM_LE_AUTH_REQ_SC_MITM      /* 1100 */
#define BTA_LE_AUTH_REQ_SC_MITM_BOND    BTM_LE_AUTH_REQ_SC_MITM_BOND /* 1101 */
typedef tBTM_LE_AUTH_REQ       tBTA_LE_AUTH_REQ;       /* combination of the above bit pattern */

#define BTA_OOB_NONE        BTM_OOB_NONE
#define BTA_OOB_PRESENT     BTM_OOB_PRESENT
#if BTM_OOB_INCLUDED == TRUE
#define BTA_OOB_UNKNOWN     BTM_OOB_UNKNOWN
#endif
typedef tBTM_OOB_DATA   tBTA_OOB_DATA;

/* Structure associated with BTA_DM_SP_CFM_REQ_EVT */
typedef struct {
    /* Note: First 3 data members must be, bd_addr, dev_class, and bd_name in order */
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    BD_NAME         bd_name;        /* peer device name */
    UINT32          num_val;        /* the numeric value for comparison. If just_works, do not show this number to UI */
    BOOLEAN         just_works;     /* TRUE, if "Just Works" association model */
    tBTA_AUTH_REQ   loc_auth_req;   /* Authentication required for local device */
    tBTA_AUTH_REQ   rmt_auth_req;   /* Authentication required for peer device */
    tBTA_IO_CAP     loc_io_caps;    /* IO Capabilities of local device */
    tBTA_AUTH_REQ   rmt_io_caps;    /* IO Capabilities of remote device */
} tBTA_DM_SP_CFM_REQ;

enum {
    BTA_SP_KEY_STARTED,         /* passkey entry started */
    BTA_SP_KEY_ENTERED,         /* passkey digit entered */
    BTA_SP_KEY_ERASED,          /* passkey digit erased */
    BTA_SP_KEY_CLEARED,         /* passkey cleared */
    BTA_SP_KEY_COMPLT           /* passkey entry completed */
};
typedef UINT8   tBTA_SP_KEY_TYPE;

/* Structure associated with BTA_DM_SP_KEYPRESS_EVT */
typedef struct {
    BD_ADDR             bd_addr;        /* peer address */
    tBTA_SP_KEY_TYPE   notif_type;
} tBTA_DM_SP_KEY_PRESS;

/* Structure associated with BTA_DM_SP_KEY_NOTIF_EVT */
typedef struct {
    /* Note: First 3 data members must be, bd_addr, dev_class, and bd_name in order */
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    BD_NAME         bd_name;        /* peer device name */
    UINT32          passkey;        /* the numeric value for comparison. If just_works, do not show this number to UI */
} tBTA_DM_SP_KEY_NOTIF;

/* Structure associated with BTA_DM_SP_RMT_OOB_EVT */
typedef struct {
    /* Note: First 3 data members must be, bd_addr, dev_class, and bd_name in order */
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    BD_NAME         bd_name;        /* peer device name */
} tBTA_DM_SP_RMT_OOB;

/* Structure associated with BTA_DM_BOND_CANCEL_CMPL_EVT */
typedef struct {
    tBTA_STATUS     result;    /* TRUE of bond cancel succeeded, FALSE if failed. */
} tBTA_DM_BOND_CANCEL_CMPL;

/* Union of all security callback structures */
typedef union {
    tBTA_DM_ENABLE      enable;         /* BTA enabled */
    tBTA_DM_PIN_REQ     pin_req;        /* PIN request. */
    tBTA_DM_AUTH_CMPL   auth_cmpl;      /* Authentication complete indication. */
    tBTA_DM_AUTHORIZE   authorize;      /* Authorization request. */
    tBTA_DM_LINK_UP     link_up;       /* ACL connection down event */
    tBTA_DM_LINK_DOWN   link_down;       /* ACL connection down event */
    tBTA_DM_BUSY_LEVEL  busy_level;     /* System busy level */
    tBTA_DM_SP_CFM_REQ  cfm_req;        /* user confirm request */
    tBTA_DM_SP_KEY_NOTIF key_notif;     /* passkey notification */
    tBTA_DM_SP_RMT_OOB  rmt_oob;        /* remote oob */
    tBTA_DM_BOND_CANCEL_CMPL bond_cancel_cmpl; /* Bond Cancel Complete indication */
    tBTA_DM_SP_KEY_PRESS   key_press;   /* key press notification event */
    tBTA_DM_ROLE_CHG     role_chg;       /* role change event */
    tBTA_DM_BLE_SEC_REQ  ble_req;        /* BLE SMP related request */
    tBTA_DM_BLE_KEY      ble_key;        /* BLE SMP keys used when pairing */
    tBTA_BLE_LOCAL_ID_KEYS  ble_id_keys;  /* IR event */
    BT_OCTET16              ble_er;       /* ER event data */
} tBTA_DM_SEC;

/* Security callback */
typedef void (tBTA_DM_SEC_CBACK)(tBTA_DM_SEC_EVT event, tBTA_DM_SEC *p_data);

#define BTA_BLE_MULTI_ADV_ILLEGAL 0

/* multi adv callback event */
#define BTA_BLE_MULTI_ADV_ENB_EVT           1
#define BTA_BLE_MULTI_ADV_DISABLE_EVT       2
#define BTA_BLE_MULTI_ADV_PARAM_EVT         3
#define BTA_BLE_MULTI_ADV_DATA_EVT          4

typedef UINT8 tBTA_BLE_MULTI_ADV_EVT;

/* multi adv callback */
typedef void (tBTA_BLE_MULTI_ADV_CBACK)(tBTA_BLE_MULTI_ADV_EVT event,
                                        UINT8 inst_id, void *p_ref, tBTA_STATUS status);
typedef UINT32 tBTA_DM_BLE_REF_VALUE;

#define BTA_DM_BLE_PF_ENABLE_EVT       BTM_BLE_PF_ENABLE
#define BTA_DM_BLE_PF_CONFIG_EVT       BTM_BLE_PF_CONFIG
typedef UINT8 tBTA_DM_BLE_PF_EVT;

#define BTA_DM_BLE_PF_ENABLE       1
#define BTA_DM_BLE_PF_CONFIG       2
typedef UINT8 tBTA_DM_BLE_PF_ACTION;

/* Config callback */
typedef void (tBTA_DM_BLE_PF_CFG_CBACK) (tBTA_DM_BLE_PF_ACTION action,
        tBTA_DM_BLE_PF_COND_TYPE cfg_cond,
        tBTA_DM_BLE_PF_AVBL_SPACE avbl_space, tBTA_STATUS status,
        tBTA_DM_BLE_REF_VALUE ref_value);
/* Param callback */
typedef void (tBTA_DM_BLE_PF_PARAM_CBACK) (UINT8 action_type, tBTA_DM_BLE_PF_AVBL_SPACE avbl_space,
        tBTA_DM_BLE_REF_VALUE ref_value, tBTA_STATUS status);

/* Status callback */
typedef void (tBTA_DM_BLE_PF_STATUS_CBACK) (UINT8 action, tBTA_STATUS status,
        tBTA_DM_BLE_REF_VALUE ref_value);


#define BTA_DM_BLE_PF_BRDCAST_ADDR_FILT  1
#define BTA_DM_BLE_PF_SERV_DATA_CHG_FILT 2
#define BTA_DM_BLE_PF_SERV_UUID          4
#define BTA_DM_BLE_PF_SERV_SOLC_UUID     8
#define BTA_DM_BLE_PF_LOC_NAME_CHECK    16
#define BTA_DM_BLE_PF_MANUF_NAME_CHECK  32
#define BTA_DM_BLE_PF_SERV_DATA_CHECK   64
typedef UINT16 tBTA_DM_BLE_PF_FEAT_SEL;

#define BTA_DM_BLE_PF_LIST_LOGIC_OR   1
#define BTA_DM_BLE_PF_LIST_LOGIC_AND  2
typedef UINT16 tBTA_DM_BLE_PF_LIST_LOGIC_TYPE;

#define BTA_DM_BLE_PF_FILT_LOGIC_OR   0
#define BTA_DM_BLE_PF_FILT_LOGIC_AND  1
typedef UINT16 tBTA_DM_BLE_PF_FILT_LOGIC_TYPE;

typedef UINT8  tBTA_DM_BLE_PF_RSSI_THRESHOLD;
typedef UINT8  tBTA_DM_BLE_PF_DELIVERY_MODE;
typedef UINT16 tBTA_DM_BLE_PF_TIMEOUT;
typedef UINT8  tBTA_DM_BLE_PF_TIMEOUT_CNT;
typedef UINT16 tBTA_DM_BLE_PF_ADV_TRACK_ENTRIES;

typedef struct {
    tBTA_DM_BLE_PF_FEAT_SEL feat_seln;
    tBTA_DM_BLE_PF_LIST_LOGIC_TYPE list_logic_type;
    tBTA_DM_BLE_PF_FILT_LOGIC_TYPE filt_logic_type;
    tBTA_DM_BLE_PF_RSSI_THRESHOLD  rssi_high_thres;
    tBTA_DM_BLE_PF_RSSI_THRESHOLD  rssi_low_thres;
    tBTA_DM_BLE_PF_DELIVERY_MODE dely_mode;
    tBTA_DM_BLE_PF_TIMEOUT found_timeout;
    tBTA_DM_BLE_PF_TIMEOUT lost_timeout;
    tBTA_DM_BLE_PF_TIMEOUT_CNT found_timeout_cnt;
    tBTA_DM_BLE_PF_ADV_TRACK_ENTRIES num_of_tracking_entries;
} tBTA_DM_BLE_PF_FILT_PARAMS;

/* Search callback events */
#define BTA_DM_INQ_RES_EVT              0       /* Inquiry result for a peer device. */
#define BTA_DM_INQ_CMPL_EVT             1       /* Inquiry complete. */
#define BTA_DM_DISC_RES_EVT             2       /* Discovery result for a peer device. */
#define BTA_DM_DISC_BLE_RES_EVT         3       /* Discovery result for BLE GATT based servoce on a peer device. */
#define BTA_DM_DISC_CMPL_EVT            4       /* Discovery complete. */
#define BTA_DM_DI_DISC_CMPL_EVT         5       /* Discovery complete. */
#define BTA_DM_SEARCH_CANCEL_CMPL_EVT   6       /* Search cancelled */

typedef UINT8 tBTA_DM_SEARCH_EVT;

#define BTA_DM_INQ_RES_IGNORE_RSSI      BTM_INQ_RES_IGNORE_RSSI /* 0x7f RSSI value not supplied (ignore it) */

/* Structure associated with BTA_DM_INQ_RES_EVT */
typedef struct {
    BD_ADDR         bd_addr;                /* BD address peer device. */
    DEV_CLASS       dev_class;              /* Device class of peer device. */
    BOOLEAN         remt_name_not_required; /* Application sets this flag if it already knows the name of the device */
    /* If the device name is known to application BTA skips the remote name request */
    BOOLEAN         is_limited;             /* TRUE, if the limited inquiry bit is set in the CoD */
    INT8            rssi;                   /* The rssi value */
    UINT8           *p_eir;                 /* received EIR */
#if (BLE_INCLUDED == TRUE)
    UINT8               inq_result_type;
    UINT8               ble_addr_type;
    tBTM_BLE_EVT_TYPE   ble_evt_type;
    tBT_DEVICE_TYPE     device_type;
    UINT8               flag;
#endif

} tBTA_DM_INQ_RES;

/* Structure associated with BTA_DM_INQ_CMPL_EVT */
typedef struct {
    UINT8           num_resps;          /* Number of inquiry responses. */
} tBTA_DM_INQ_CMPL;

/* Structure associated with BTA_DM_DI_DISC_CMPL_EVT */
typedef struct {
    BD_ADDR             bd_addr;        /* BD address peer device. */
    UINT8               num_record;     /* Number of DI record */
    tBTA_STATUS         result;
} tBTA_DM_DI_DISC_CMPL;

/* Structure associated with BTA_DM_DISC_RES_EVT */
typedef struct {
    BD_ADDR             bd_addr;        /* BD address peer device. */
    BD_NAME             bd_name;        /* Name of peer device. */
    tBTA_SERVICE_MASK   services;       /* Services found on peer device. */
// btla-specific ++
    UINT8              *p_raw_data;     /* Raw data for discovery DB */
    UINT32              raw_data_size;  /* size of raw data */
    tBT_DEVICE_TYPE     device_type;    /* device type in case it is BLE device */
    UINT32              num_uuids;
    UINT8               *p_uuid_list;
// btla-specific --
    tBTA_STATUS         result;
} tBTA_DM_DISC_RES;

/* Structure associated with tBTA_DM_DISC_BLE_RES */
typedef struct {
    BD_ADDR             bd_addr;        /* BD address peer device. */
    BD_NAME             bd_name;        /* Name of peer device. */
    tBT_UUID            service;        /* GATT based Services UUID found on peer device. */
} tBTA_DM_DISC_BLE_RES;


/* Union of all search callback structures */
typedef union {
    tBTA_DM_INQ_RES     inq_res;        /* Inquiry result for a peer device. */
    tBTA_DM_INQ_CMPL    inq_cmpl;       /* Inquiry complete. */
    tBTA_DM_DISC_RES    disc_res;       /* Discovery result for a peer device. */
    tBTA_DM_DISC_BLE_RES    disc_ble_res;   /* discovery result for GATT based service */
    tBTA_DM_DI_DISC_CMPL    di_disc;        /* DI discovery result for a peer device */

} tBTA_DM_SEARCH;

/* Search callback */
typedef void (tBTA_DM_SEARCH_CBACK)(tBTA_DM_SEARCH_EVT event, tBTA_DM_SEARCH *p_data);

/* Execute call back */
typedef void (tBTA_DM_EXEC_CBACK) (void *p_param);

/* Encryption callback*/
typedef void (tBTA_DM_ENCRYPT_CBACK) (BD_ADDR bd_addr, tBTA_TRANSPORT transport, tBTA_STATUS result);

#if BLE_INCLUDED == TRUE
#define BTA_DM_BLE_SEC_NONE         BTM_BLE_SEC_NONE
#define BTA_DM_BLE_SEC_ENCRYPT      BTM_BLE_SEC_ENCRYPT
#define BTA_DM_BLE_SEC_NO_MITM      BTM_BLE_SEC_ENCRYPT_NO_MITM
#define BTA_DM_BLE_SEC_MITM         BTM_BLE_SEC_ENCRYPT_MITM
typedef tBTM_BLE_SEC_ACT            tBTA_DM_BLE_SEC_ACT;

typedef tBTM_BLE_TX_TIME_MS         tBTA_DM_BLE_TX_TIME_MS;
typedef tBTM_BLE_RX_TIME_MS         tBTA_DM_BLE_RX_TIME_MS;
typedef tBTM_BLE_IDLE_TIME_MS       tBTA_DM_BLE_IDLE_TIME_MS;
typedef tBTM_BLE_ENERGY_USED        tBTA_DM_BLE_ENERGY_USED;

#define BTA_DM_CONTRL_UNKNOWN 0       /* Unknown state */
#define BTA_DM_CONTRL_ACTIVE  1       /* ACL link on, SCO link ongoing, sniff mode */
#define BTA_DM_CONTRL_SCAN    2       /* Scan state - paging/inquiry/trying to connect*/
#define BTA_DM_CONTRL_IDLE    3       /* Idle state - page scan, LE advt, inquiry scan */

typedef UINT8 tBTA_DM_CONTRL_STATE;

typedef UINT8 tBTA_DM_BLE_ADV_STATE;
typedef UINT8 tBTA_DM_BLE_ADV_INFO_PRESENT;
typedef UINT8 tBTA_DM_BLE_RSSI_VALUE;
typedef UINT16 tBTA_DM_BLE_ADV_INFO_TIMESTAMP;

typedef tBTM_BLE_TRACK_ADV_DATA tBTA_DM_BLE_TRACK_ADV_DATA;

typedef void (tBTA_BLE_SCAN_THRESHOLD_CBACK)(tBTA_DM_BLE_REF_VALUE ref_value);

typedef void (tBTA_BLE_SCAN_REP_CBACK) (tBTA_DM_BLE_REF_VALUE ref_value, UINT8 report_format,
                                        UINT8 num_records, UINT16 data_len,
                                        UINT8 *p_rep_data, tBTA_STATUS status);

typedef void (tBTA_BLE_SCAN_SETUP_CBACK) (tBTA_BLE_BATCH_SCAN_EVT evt,
        tBTA_DM_BLE_REF_VALUE ref_value,
        tBTA_STATUS status);

typedef void (tBTA_BLE_TRACK_ADV_CMPL_CBACK)(int action, tBTA_STATUS status,
        tBTA_DM_BLE_PF_AVBL_SPACE avbl_space,
        tBTA_DM_BLE_REF_VALUE ref_value);

typedef void (tBTA_BLE_TRACK_ADV_CBACK)(tBTA_DM_BLE_TRACK_ADV_DATA *p_adv_data);

typedef void (tBTA_BLE_ENERGY_INFO_CBACK)(tBTA_DM_BLE_TX_TIME_MS tx_time,
        tBTA_DM_BLE_RX_TIME_MS rx_time,
        tBTA_DM_BLE_IDLE_TIME_MS idle_time,
        tBTA_DM_BLE_ENERGY_USED  energy_used,
        tBTA_DM_CONTRL_STATE ctrl_state,
        tBTA_STATUS status);

#else
typedef UINT8                       tBTA_DM_BLE_SEC_ACT;
#endif

/* Maximum service name length */
#define BTA_SERVICE_NAME_LEN    35
#define BTA_SERVICE_DESP_LEN    BTA_SERVICE_NAME_LEN
#define BTA_PROVIDER_NAME_LEN   BTA_SERVICE_NAME_LEN


/* link policy masks  */
#define BTA_DM_LP_SWITCH        HCI_ENABLE_MASTER_SLAVE_SWITCH
#define BTA_DM_LP_HOLD          HCI_ENABLE_HOLD_MODE
#define BTA_DM_LP_SNIFF         HCI_ENABLE_SNIFF_MODE
#define BTA_DM_LP_PARK          HCI_ENABLE_PARK_MODE
typedef UINT16 tBTA_DM_LP_MASK;

/* power mode actions  */
#define BTA_DM_PM_NO_ACTION    0x00       /* no change to the current pm setting */
#define BTA_DM_PM_PARK         0x10       /* prefers park mode */
#define BTA_DM_PM_SNIFF        0x20       /* prefers sniff mode */
#define BTA_DM_PM_SNIFF1       0x21       /* prefers sniff1 mode */
#define BTA_DM_PM_SNIFF2       0x22       /* prefers sniff2 mode */
#define BTA_DM_PM_SNIFF3       0x23       /* prefers sniff3 mode */
#define BTA_DM_PM_SNIFF4       0x24       /* prefers sniff4 mode */
#define BTA_DM_PM_SNIFF5       0x25       /* prefers sniff5 mode */
#define BTA_DM_PM_SNIFF6       0x26       /* prefers sniff6 mode */
#define BTA_DM_PM_SNIFF7       0x27       /* prefers sniff7 mode */
#define BTA_DM_PM_SNIFF_USER0  0x28       /* prefers user-defined sniff0 mode (testtool only) */
#define BTA_DM_PM_SNIFF_USER1  0x29       /* prefers user-defined sniff1 mode (testtool only) */
#define BTA_DM_PM_ACTIVE       0x40       /* prefers active mode */
#define BTA_DM_PM_RETRY        0x80       /* retry power mode based on current settings */
#define BTA_DM_PM_SUSPEND      0x04       /* prefers suspend mode */
#define BTA_DM_PM_NO_PREF      0x01       /* service has no prefernce on power mode setting. eg. connection to service got closed */

typedef UINT8 tBTA_DM_PM_ACTION;

/* index to bta_dm_ssr_spec */
#define BTA_DM_PM_SSR0          0
#define BTA_DM_PM_SSR1          1       /* BTA_DM_PM_SSR1 will be dedicated for
                                        HH SSR setting entry, no other profile can use it */
#define BTA_DM_PM_SSR2          2
#define BTA_DM_PM_SSR3          3
#define BTA_DM_PM_SSR4          4
#define BTA_DM_PM_SSR5          5
#define BTA_DM_PM_SSR6          6

#define BTA_DM_PM_NUM_EVTS      9

#ifndef BTA_DM_PM_PARK_IDX
#define BTA_DM_PM_PARK_IDX      5 /* the actual index to bta_dm_pm_md[] for PARK mode */
#endif

#ifndef BTA_DM_PM_SNIFF_A2DP_IDX
#define BTA_DM_PM_SNIFF_A2DP_IDX      BTA_DM_PM_SNIFF
#endif

#ifndef BTA_DM_PM_SNIFF_HD_IDLE_IDX
#define BTA_DM_PM_SNIFF_HD_IDLE_IDX   BTA_DM_PM_SNIFF2
#endif

#ifndef BTA_DM_PM_SNIFF_SCO_OPEN_IDX
#define BTA_DM_PM_SNIFF_SCO_OPEN_IDX  BTA_DM_PM_SNIFF3
#endif

#ifndef BTA_DM_PM_SNIFF_HD_ACTIVE_IDX
#define BTA_DM_PM_SNIFF_HD_ACTIVE_IDX BTA_DM_PM_SNIFF4
#endif

#ifndef BTA_DM_PM_SNIFF_HH_OPEN_IDX
#define BTA_DM_PM_SNIFF_HH_OPEN_IDX BTA_DM_PM_SNIFF2
#endif

#ifndef BTA_DM_PM_SNIFF_HH_ACTIVE_IDX
#define BTA_DM_PM_SNIFF_HH_ACTIVE_IDX BTA_DM_PM_SNIFF2
#endif

#ifndef BTA_DM_PM_SNIFF_HH_IDLE_IDX
#define BTA_DM_PM_SNIFF_HH_IDLE_IDX BTA_DM_PM_SNIFF2
#endif


#ifndef BTA_DM_PM_HH_OPEN_DELAY
#define BTA_DM_PM_HH_OPEN_DELAY 30000
#endif

#ifndef BTA_DM_PM_HH_ACTIVE_DELAY
#define BTA_DM_PM_HH_ACTIVE_DELAY 30000
#endif

#ifndef BTA_DM_PM_HH_IDLE_DELAY
#define BTA_DM_PM_HH_IDLE_DELAY 30000
#endif

/* The Sniff Parameters defined below must be ordered from highest
 * latency (biggest interval) to lowest latency.  If there is a conflict
 * among the connected services the setting with the lowest latency will
 * be selected.  If a device should override a sniff parameter then it
 * must insure that order is maintained.
 */
#ifndef BTA_DM_PM_SNIFF_MAX
#define BTA_DM_PM_SNIFF_MAX      800
#define BTA_DM_PM_SNIFF_MIN      400
#define BTA_DM_PM_SNIFF_ATTEMPT  4
#define BTA_DM_PM_SNIFF_TIMEOUT  1
#endif

#ifndef BTA_DM_PM_SNIFF1_MAX
#define BTA_DM_PM_SNIFF1_MAX     400
#define BTA_DM_PM_SNIFF1_MIN     200
#define BTA_DM_PM_SNIFF1_ATTEMPT 4
#define BTA_DM_PM_SNIFF1_TIMEOUT 1
#endif

#ifndef BTA_DM_PM_SNIFF2_MAX
#define BTA_DM_PM_SNIFF2_MAX     180
#define BTA_DM_PM_SNIFF2_MIN     150
#define BTA_DM_PM_SNIFF2_ATTEMPT 4
#define BTA_DM_PM_SNIFF2_TIMEOUT 1
#endif

#ifndef BTA_DM_PM_SNIFF3_MAX
#define BTA_DM_PM_SNIFF3_MAX     150
#define BTA_DM_PM_SNIFF3_MIN     50
#define BTA_DM_PM_SNIFF3_ATTEMPT 4
#define BTA_DM_PM_SNIFF3_TIMEOUT 1
#endif

#ifndef BTA_DM_PM_SNIFF4_MAX
#define BTA_DM_PM_SNIFF4_MAX     54
#define BTA_DM_PM_SNIFF4_MIN     30
#define BTA_DM_PM_SNIFF4_ATTEMPT 4
#define BTA_DM_PM_SNIFF4_TIMEOUT 1
#endif

#ifndef BTA_DM_PM_SNIFF5_MAX
#define BTA_DM_PM_SNIFF5_MAX     36
#define BTA_DM_PM_SNIFF5_MIN     30
#define BTA_DM_PM_SNIFF5_ATTEMPT 2
#define BTA_DM_PM_SNIFF5_TIMEOUT 0
#endif

#ifndef BTA_DM_PM_PARK_MAX
#define BTA_DM_PM_PARK_MAX       800
#define BTA_DM_PM_PARK_MIN       400
#define BTA_DM_PM_PARK_ATTEMPT   0
#define BTA_DM_PM_PARK_TIMEOUT   0
#endif


/* Switch callback events */
#define BTA_DM_SWITCH_CMPL_EVT      0       /* Completion of the Switch API */

typedef UINT8 tBTA_DM_SWITCH_EVT;
typedef void (tBTA_DM_SWITCH_CBACK)(tBTA_DM_SWITCH_EVT event, tBTA_STATUS status);

/* Audio routing out configuration */
#define BTA_DM_ROUTE_NONE       0x00    /* No Audio output */
#define BTA_DM_ROUTE_DAC        0x01    /* routing over analog output */
#define BTA_DM_ROUTE_I2S        0x02    /* routing over digital (I2S) output */
#define BTA_DM_ROUTE_BT_MONO    0x04    /* routing over SCO */
#define BTA_DM_ROUTE_BT_STEREO  0x08    /* routing over BT Stereo */
#define BTA_DM_ROUTE_HOST       0x10    /* routing over Host */
#define BTA_DM_ROUTE_FMTX       0x20    /* routing over FMTX */
#define BTA_DM_ROUTE_FMRX       0x40    /* routing over FMRX */
#define BTA_DM_ROUTE_BTSNK      0x80    /* routing over BT SNK */

typedef UINT8 tBTA_DM_ROUTE_PATH;


/* Device Identification (DI) data structure
*/
/* Used to set the DI record */
typedef tSDP_DI_RECORD          tBTA_DI_RECORD;
/* Used to get the DI record */
typedef tSDP_DI_GET_RECORD      tBTA_DI_GET_RECORD;
/* SDP discovery database */
typedef tSDP_DISCOVERY_DB       tBTA_DISCOVERY_DB;

#ifndef         BTA_DI_NUM_MAX
#define         BTA_DI_NUM_MAX       3
#endif

/* Device features mask definitions */
#define BTA_FEATURE_BYTES_PER_PAGE  BTM_FEATURE_BYTES_PER_PAGE
#define BTA_EXT_FEATURES_PAGE_MAX   BTM_EXT_FEATURES_PAGE_MAX
/* ACL type
*/
#define BTA_DM_LINK_TYPE_BR_EDR    0x01
#define BTA_DM_LINK_TYPE_LE        0x02
#define BTA_DM_LINK_TYPE_ALL       0xFF
typedef UINT8 tBTA_DM_LINK_TYPE;

#define IMMEDIATE_DELY_MODE  0x00
#define ONFOUND_DELY_MODE    0x01
#define BATCH_DELY_MODE      0x02
#define ALLOW_ALL_FILTER     0x00
#define LOWEST_RSSI_VALUE     129

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         BTA_EnableBluetooth
**
** Description      This function initializes BTA and prepares BTA and the
**                  Bluetooth protocol stack for use.  This function is
**                  typically called at startup or when Bluetooth services
**                  are required by the phone.  This function must be called
**                  before calling any other API function.
**
**
** Returns          BTA_SUCCESS if successful.
**                  BTA_FAIL if internal failure.
**
*******************************************************************************/
extern tBTA_STATUS BTA_EnableBluetooth(tBTA_DM_SEC_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_DisableBluetooth
**
** Description      This function disables BTA and the Bluetooth protocol
**                  stack.  It is called when BTA is no longer being used
**                  by any application in the system.
**
**
** Returns          void
**
*******************************************************************************/
extern tBTA_STATUS BTA_DisableBluetooth(void);

/*******************************************************************************
**
** Function         BTA_EnableTestMode
**
** Description      Enables bluetooth device under test mode
**
**
** Returns          tBTA_STATUS
**
*******************************************************************************/
extern tBTA_STATUS BTA_EnableTestMode(void);

/*******************************************************************************
**
** Function         BTA_DisableTestMode
**
** Description      Disable bluetooth device under test mode
**
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DisableTestMode(void);

/*******************************************************************************
**
** Function         BTA_DmSetDeviceName
**
** Description      This function sets the Bluetooth name of the local device.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSetDeviceName(char *p_name);

/*******************************************************************************
**
** Function         BTA_DmSetVisibility
**
** Description      This function sets the Bluetooth connectable,discoverable,
**                  pairable and conn paired only modesmodes of the local device.
**                  This controls whether other Bluetooth devices can find and connect to
**                  the local device.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSetVisibility(tBTA_DM_DISC disc_mode, tBTA_DM_CONN conn_mode, UINT8 pairable_mode, UINT8 conn_filter);

/*******************************************************************************
**
** Function         BTA_DmSearch
**
** Description      This function searches for peer Bluetooth devices.  It
**                  first performs an inquiry; for each device found from the
**                  inquiry it gets the remote name of the device.  If
**                  parameter services is nonzero, service discovery will be
**                  performed on each device for the services specified.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSearch(tBTA_DM_INQ *p_dm_inq, tBTA_SERVICE_MASK services,
                         tBTA_DM_SEARCH_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_DmSearchCancel
**
** Description      This function cancels a search that has been initiated
**                  by calling BTA_DmSearch().
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSearchCancel(void);

/*******************************************************************************
**
** Function         BTA_DmDiscover
**
** Description      This function performs service discovery for the services
**                  of a particular peer device.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmDiscover(BD_ADDR bd_addr, tBTA_SERVICE_MASK services,
                           tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search);

// btla-specific ++
/*******************************************************************************
**
** Function         BTA_DmDiscoverUUID
**
** Description      This function performs service discovery for the services
**                  of a particular peer device.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmDiscoverUUID(BD_ADDR bd_addr, tSDP_UUID *uuid,
                               tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search);

/*******************************************************************************
**
** Function         BTA_DmGetCachedRemoteName
**
** Description      Retieve cached remote name if available
**
** Returns          BTA_SUCCESS if cached name was retrieved
**                  BTA_FAILURE if cached name is not available
**
*******************************************************************************/
tBTA_STATUS BTA_DmGetCachedRemoteName(BD_ADDR remote_device, UINT8 **pp_cached_name);
// btla-specific --

/*******************************************************************************
**
** Function         BTA_DmBond
**
** Description      This function initiates a bonding procedure with a peer
**                  device.  The bonding procedure enables authentication
**                  and optionally encryption on the Bluetooth link.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBond(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTA_DmBondByTransport
**
** Description      This function initiates a bonding procedure with a peer
**                  device by designated transport.  The bonding procedure enables
**                  authentication and optionally encryption on the Bluetooth link.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBondByTransport(BD_ADDR bd_addr, tBTA_TRANSPORT transport);


/*******************************************************************************
**
** Function         BTA_DmBondCancel
**
** Description      This function cancels a bonding procedure with a peer
**                  device.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBondCancel(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTA_DmPinReply
**
** Description      This function provides a PIN when one is requested by DM
**                  during a bonding procedure.  The application should call
**                  this function after the security callback is called with
**                  a BTA_DM_PIN_REQ_EVT.
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmPinReply(BD_ADDR bd_addr, BOOLEAN accept, UINT8 pin_len,
                           UINT8 *p_pin);

#if (BTM_OOB_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmLocalOob
**
** Description      This function retrieves the OOB data from local controller.
**                  The result is reported by bta_dm_co_loc_oob().
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmLocalOob(void);
#endif /* BTM_OOB_INCLUDED */

/*******************************************************************************
**
** Function         BTA_DmConfirm
**
** Description      This function accepts or rejects the numerical value of the
**                  Simple Pairing process on BTA_DM_SP_CFM_REQ_EVT
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmConfirm(BD_ADDR bd_addr, BOOLEAN accept);

/*******************************************************************************
**
** Function         BTA_DmAddDevice
**
** Description      This function adds a device to the security database list
**                  of peer devices. This function would typically be called
**                  at system startup to initialize the security database with
**                  known peer devices.  This is a direct execution function
**                  that may lock task scheduling on some platforms.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmAddDevice(BD_ADDR bd_addr, DEV_CLASS dev_class,
                            LINK_KEY link_key, tBTA_SERVICE_MASK trusted_mask,
                            BOOLEAN is_trusted, UINT8 key_type,
                            tBTA_IO_CAP io_cap, UINT8 pin_length);

/*******************************************************************************
**
** Function         BTA_DmRemoveDevice
**
** Description      This function removes a device from the security database.
**                  This is a direct execution function that may lock task
**                  scheduling on some platforms.
**
**
** Returns          BTA_SUCCESS if successful.
**                  BTA_FAIL if operation failed.
**
*******************************************************************************/
extern tBTA_STATUS BTA_DmRemoveDevice(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTA_GetEirService
**
** Description      This function is called to get BTA service mask from EIR.
**
** Parameters       p_eir - pointer of EIR significant part
**                  p_services - return the BTA service mask
**
** Returns          None
**
*******************************************************************************/
extern void BTA_GetEirService( UINT8 *p_eir, tBTA_SERVICE_MASK *p_services );

/*******************************************************************************
**
** Function         BTA_DmGetConnectionState
**
** Description      Returns whether the remote device is currently connected.
**
** Returns          0 if the device is NOT connected.
**
*******************************************************************************/
extern UINT16 BTA_DmGetConnectionState( BD_ADDR bd_addr );


/*******************************************************************************
**
** Function         BTA_DmSetLocalDiRecord
**
** Description      This function adds a DI record to the local SDP database.
**
** Returns          BTA_SUCCESS if record set sucessfully, otherwise error code.
**
*******************************************************************************/
extern tBTA_STATUS BTA_DmSetLocalDiRecord( tBTA_DI_RECORD *p_device_info,
        UINT32 *p_handle );

/*******************************************************************************
**
**
** Function         BTA_DmCloseACL
**
** Description      This function force to close an ACL connection and remove the
**                  device from the security database list of known devices.
**
** Parameters:      bd_addr       - Address of the peer device
**                  remove_dev    - remove device or not after link down
**                  transport     - which transport to close

**
** Returns          void.
**
*******************************************************************************/
extern void BTA_DmCloseACL(BD_ADDR bd_addr, BOOLEAN remove_dev, tBTA_TRANSPORT transport);

/*******************************************************************************
**
** Function         bta_dmexecutecallback
**
** Description      This function will request BTA to execute a call back in the context of BTU task
**                  This API was named in lower case because it is only intended
**                  for the internal customers(like BTIF).
**
** Returns          void
**
*******************************************************************************/
extern void bta_dmexecutecallback (tBTA_DM_EXEC_CBACK *p_callback, void *p_param);

#if (BTM_SCO_HCI_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         BTA_DmPcmInitSamples
**
** Description      initialize the down sample converter.
**
**                  src_sps: original samples per second (source audio data)
**                            (ex. 44100, 48000)
**                  bits: number of bits per pcm sample (16)
**                  n_channels: number of channels (i.e. mono(1), stereo(2)...)
**
** Returns          none
**
*******************************************************************************/
extern void BTA_DmPcmInitSamples (UINT32 src_sps, UINT32 bits, UINT32 n_channels);

/**************************************************************************************
** Function         BTA_DmPcmResample
**
** Description      Down sampling utility to convert higher sampling rate into 8K/16bits
**                  PCM samples.
**
** Parameters       p_src: pointer to the buffer where the original sampling PCM
**                              are stored.
**                  in_bytes:  Length of the input PCM sample buffer in byte.
**                  p_dst:      pointer to the buffer which is to be used to store
**                              the converted PCM samples.
**
**
** Returns          INT32: number of samples converted.
**
**************************************************************************************/
extern INT32 BTA_DmPcmResample (void *p_src, UINT32 in_bytes, void *p_dst);
#endif

#if ((defined BLE_INCLUDED) && (BLE_INCLUDED == TRUE))
/* BLE related API functions */
/*******************************************************************************
**
** Function         BTA_DmBleSecurityGrant
**
** Description      Grant security request access.
**
** Parameters:      bd_addr          - BD address of the peer
**                  res              - security grant status.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleSecurityGrant(BD_ADDR bd_addr, tBTA_DM_BLE_SEC_GRANT res);



/*******************************************************************************
**
** Function         BTA_DmBleSetBgConnType
**
** Description      This function is called to set BLE connectable mode for a
**                  peripheral device.
**
** Parameters       bg_conn_type: it can be auto connection, or selective connection.
**                  p_select_cback: callback function when selective connection procedure
**                              is being used.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleSetBgConnType(tBTA_DM_BLE_CONN_TYPE bg_conn_type, tBTA_DM_BLE_SEL_CBACK *p_select_cback);

/*******************************************************************************
**
** Function         BTA_DmBlePasskeyReply
**
** Description      Send BLE SMP passkey reply.
**
** Parameters:      bd_addr          - BD address of the peer
**                  accept           - passkey entry sucessful or declined.
**                  passkey          - passkey value, must be a 6 digit number,
**                                     can be lead by 0.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBlePasskeyReply(BD_ADDR bd_addr, BOOLEAN accept, UINT32 passkey);

/*******************************************************************************
**
** Function         BTA_DmBleConfirmReply
**
** Description      Send BLE SMP SC user confirmation reply.
**
** Parameters:      bd_addr          - BD address of the peer
**                  accept           - numbers to compare are the same or different.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleConfirmReply(BD_ADDR bd_addr, BOOLEAN accept);

/*******************************************************************************
**
** Function         BTA_DmAddBleDevice
**
** Description      Add a BLE device.  This function will be normally called
**                  during host startup to restore all required information
**                  for a LE device stored in the NVRAM.
**
** Parameters:      bd_addr          - BD address of the peer
**                  dev_type         - Remote device's device type.
**                  addr_type        - LE device address type.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmAddBleDevice(BD_ADDR bd_addr, tBLE_ADDR_TYPE addr_type,
                               tBT_DEVICE_TYPE dev_type);


/*******************************************************************************
**
** Function         BTA_DmAddBleKey
**
** Description      Add/modify LE device information.  This function will be
**                  normally called during host startup to restore all required
**                  information stored in the NVRAM.
**
** Parameters:      bd_addr          - BD address of the peer
**                  p_le_key         - LE key values.
**                  key_type         - LE SMP key type.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmAddBleKey (BD_ADDR bd_addr,
                             tBTA_LE_KEY_VALUE *p_le_key,
                             tBTA_LE_KEY_TYPE key_type);

/*******************************************************************************
**
** Function         BTA_DmSetBlePrefConnParams
**
** Description      This function is called to set the preferred connection
**                  parameters when default connection parameter is not desired.
**
** Parameters:      bd_addr          - BD address of the peripheral
**                  min_conn_int     - minimum preferred connection interval
**                  max_conn_int     - maximum preferred connection interval
**                  slave_latency    - preferred slave latency
**                  supervision_tout - preferred supervision timeout
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSetBlePrefConnParams(BD_ADDR bd_addr,
                                       UINT16 min_conn_int, UINT16 max_conn_int,
                                       UINT16 slave_latency, UINT16 supervision_tout );

/*******************************************************************************
**
** Function         BTA_DmSetBleConnScanParams
**
** Description      This function is called to set scan parameters used in
**                  BLE connection request
**
** Parameters:      scan_interval    - scan interval
**                  scan_window      - scan window
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSetBleConnScanParams(UINT32 scan_interval,
                                       UINT32 scan_window);

/*******************************************************************************
**
** Function         BTA_DmSetBleScanParams
**
** Description      This function is called to set scan parameters
**
** Parameters:      client_if - Client IF
**                  scan_interval - scan interval
**                  scan_window - scan window
**                  scan_mode - scan mode
**                  scan_param_setup_status_cback - Set scan param status callback
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSetBleScanParams(tGATT_IF client_if, UINT32 scan_interval,
                                   UINT32 scan_window, tBLE_SCAN_MODE scan_mode,
                                   tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_status_cback);


/*******************************************************************************
**
** Function         BTA_DmSetBleScanFilterParams
**
** Description      This function is called to set scan parameters
**
** Parameters:      client_if - Client IF
**                  scan_interval - scan interval
**                  scan_window - scan window
**                  scan_mode - scan mode
**                  scan_param_setup_status_cback - Set scan param status callback
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSetBleScanFilterParams(tGATT_IF client_if, UINT32 scan_interval,
        UINT32 scan_window, tBLE_SCAN_MODE scan_mode, UINT8 scan_fil_poilcy,
        UINT8 addr_type_own, tBLE_SCAN_PARAM_SETUP_CBACK scan_param_setup_cback);


/*******************************************************************************
**
** Function         BTA_DmSetBleAdvParams
**
** Description      This function sets the advertising parameters BLE functionality.
**                  It is to be called when device act in peripheral or broadcaster
**                  role.
**
** Parameters:      adv_int_min    - adv interval minimum
**                  adv_int_max    - adv interval max
**                  p_dir_bda      - directed adv initator address
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSetBleAdvParams (UINT16 adv_int_min, UINT16 adv_int_max,
                                   tBLE_BD_ADDR *p_dir_bda);

extern void BTA_DmSetBleAdvParamsAll (UINT16 adv_int_min, UINT16 adv_int_max,
                                      UINT8 adv_type, tBLE_ADDR_TYPE addr_type_own,
                                      tBTM_BLE_ADV_CHNL_MAP chnl_map, tBTM_BLE_AFP adv_fil_pol,
                                      tBLE_BD_ADDR *p_dir_bda);


/*******************************************************************************
**
** Function         BTA_DmSearchExt
**
** Description      This function searches for peer Bluetooth devices. It performs
**                  an inquiry and gets the remote name for devices. Service
**                  discovery is done if services is non zero
**
** Parameters       p_dm_inq: inquiry conditions
**                  services: if service is not empty, service discovery will be done.
**                            for all GATT based service condition, put num_uuid, and
**                            p_uuid is the pointer to the list of UUID values.
**                  p_cback: callback functino when search is completed.
**
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmSearchExt(tBTA_DM_INQ *p_dm_inq, tBTA_SERVICE_MASK_EXT *p_services,
                            tBTA_DM_SEARCH_CBACK *p_cback);

/*******************************************************************************
**
** Function         BTA_DmDiscoverExt
**
** Description      This function does service discovery for services of a
**                  peer device. When services.num_uuid is 0, it indicates all
**                  GATT based services are to be searched; other wise a list of
**                  UUID of interested services should be provided through
**                  services.p_uuid.
**
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmDiscoverExt(BD_ADDR bd_addr, tBTA_SERVICE_MASK_EXT *p_services,
                              tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search);

/*******************************************************************************
**
** Function         BTA_DmDiscoverByTransport
**
** Description      This function does service discovery on particular transport
**                  for services of a
**                  peer device. When services.num_uuid is 0, it indicates all
**                  GATT based services are to be searched; other wise a list of
**                  UUID of interested services should be provided through
**                  p_services->p_uuid.
**
**
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmDiscoverByTransport(BD_ADDR bd_addr, tBTA_SERVICE_MASK_EXT *p_services,
                                      tBTA_DM_SEARCH_CBACK *p_cback, BOOLEAN sdp_search,
                                      tBTA_TRANSPORT transport);

/*******************************************************************************
**
** Function         BTA_DmSetEncryption
**
** Description      This function is called to ensure that connection is
**                  encrypted.  Should be called only on an open connection.
**                  Typically only needed for connections that first want to
**                  bring up unencrypted links, then later encrypt them.
**
** Parameters:      bd_addr       - Address of the peer device
**                  transport     - transport of the link to be encruypted
**                  p_callback    - Pointer to callback function to indicat the
**                                  link encryption status
**                  sec_act       - This is the security action to indicate
**                                  what knid of BLE security level is required for
**                                  the BLE link if the BLE is supported
**                                  Note: This parameter is ignored for the BR/EDR link
**                                        or the BLE is not supported
**
** Returns          void
**
**
*******************************************************************************/
extern void BTA_DmSetEncryption(BD_ADDR bd_addr, tBTA_TRANSPORT transport,
                                tBTA_DM_ENCRYPT_CBACK *p_callback,
                                tBTA_DM_BLE_SEC_ACT sec_act);


/*******************************************************************************
**
** Function         BTA_DmBleObserve
**
** Description      This procedure keep the device listening for advertising
**                  events from a broadcast device.
**
** Parameters       start: start or stop observe.
**                  duration : Duration of the scan. Continuous scan if 0 is passed
**                  p_results_cb: Callback to be called with scan results
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleObserve(BOOLEAN start, UINT8 duration,
                             tBTA_DM_SEARCH_CBACK *p_results_cb);

extern void BTA_DmBleStopAdvertising(void);

extern void BTA_DmSetRandAddress(BD_ADDR rand_addr);

#endif

#if BLE_INCLUDED == TRUE
// btla-specific --
/*******************************************************************************
**
** Function         BTA_DmBleConfigLocalPrivacy
**
** Description      Enable/disable privacy on the local device
**
** Parameters:      privacy_enable   - enable/disabe privacy on remote device.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleConfigLocalPrivacy(BOOLEAN privacy_enable);

/*******************************************************************************
**
** Function         BTA_DmBleEnableRemotePrivacy
**
** Description      Enable/disable privacy on a remote device
**
** Parameters:      bd_addr          - BD address of the peer
**                  privacy_enable   - enable/disabe privacy on remote device.
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleEnableRemotePrivacy(BD_ADDR bd_addr, BOOLEAN privacy_enable);


/*******************************************************************************
**
** Function         BTA_DmBleSetAdvConfig
**
** Description      This function is called to override the BTA default ADV parameters.
**
** Parameters       Pointer to User defined ADV data structure
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleSetAdvConfig (tBTA_BLE_AD_MASK data_mask,
                                   tBTA_BLE_ADV_DATA *p_adv_cfg,
                                   tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

/*******************************************************************************
**
** Function         BTA_DmBleSetAdvConfigRaw
**
** Description      This function is called to set raw Advertising data
**
** Parameters       p_raw_adv : raw advertising data.
**                  raw_adv_len : raw advertising data length.
**                  p_adv_data_cback : set adv data complete callback.
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleSetAdvConfigRaw (UINT8 *p_raw_adv, UINT32 raw_adv_len,
                            tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

/*******************************************************************************
**
** Function         BTA_DmBleSetScanRsp
**
** Description      This function is called to override the BTA scan response.
**
** Parameters       Pointer to User defined ADV data structure
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleSetScanRsp (tBTA_BLE_AD_MASK data_mask,
                                 tBTA_BLE_ADV_DATA *p_adv_cfg,
                                 tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

/*******************************************************************************
**
** Function         BTA_DmBleSetScanRspRaw
**
** Description      This function is called to set raw scan response data
**
** Parameters       p_raw_scan_rsp : raw scan_rspertising data.
**                  raw_scan_rsp_len : raw scan_rspertising data length.
**                  p_scan_rsp_data_cback : set scan_rsp data complete callback.
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleSetScanRspRaw (UINT8 *p_raw_scan_rsp, UINT32 raw_scan_rsp_len,
                                    tBTA_SET_ADV_DATA_CMPL_CBACK *p_scan_rsp_data_cback);

/*******************************************************************************
**
** Function         BTA_DmBleBroadcast
**
** Description      This function starts or stops LE broadcasting.
**
** Parameters       start: start or stop broadcast.
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleBroadcast (BOOLEAN start);


/*******************************************************************************
**
** Function         BTA_BleEnableAdvInstance
**
** Description      This function enables the Multi ADV instance feature
**
** Parameters       p_params Pointer to ADV param user defined structure
**                  p_cback  Pointer to Multi ADV callback structure
**                  p_ref - Reference pointer
**
** Returns          None
**
*******************************************************************************/
extern void BTA_BleEnableAdvInstance (tBTA_BLE_ADV_PARAMS *p_params,
                                      tBTA_BLE_MULTI_ADV_CBACK *p_cback, void *p_ref);

/*******************************************************************************
**
** Function         BTA_BleUpdateAdvInstParam
**
** Description      This function updates the Multi ADV instance params
**
** Parameters       inst_id Instance ID
**                  p_params Pointer to ADV param user defined structure
**
** Returns          None
**
*******************************************************************************/
extern void BTA_BleUpdateAdvInstParam (UINT8 inst_id,
                                       tBTA_BLE_ADV_PARAMS *p_params);

/*******************************************************************************
**
** Function         BTA_BleCfgAdvInstData
**
** Description      This function is called to configure the ADV instance data
**
** Parameters       inst_id - Instance ID
**                  is_scan_rsp - Boolean value Scan response
**                  Pointer to User defined ADV data structure
** Returns          None
**
*******************************************************************************/
extern void BTA_BleCfgAdvInstData (UINT8 inst_id, BOOLEAN is_scan_rsp,
                                   tBTA_BLE_AD_MASK data_mask, tBTA_BLE_ADV_DATA *p_data);

/*******************************************************************************
**
** Function         BTA_BleDisableAdvInstance
**
** Description      This function is called to disable the ADV instance
**
** Parameters       inst_id - Instance ID to be disabled
**
** Returns          None
**
*******************************************************************************/
extern void BTA_BleDisableAdvInstance(UINT8 inst_id);

/*******************************************************************************
**
** Function         BTA_DmBleUpdateConnectionParams
**
** Description      Update connection parameters, can only be used when connection is up.
**
** Parameters:      bd_addr   - BD address of the peer
**                  min_int   -     minimum connection interval, [0x0004~ 0x4000]
**                  max_int   -     maximum connection interval, [0x0004~ 0x4000]
**                  latency   -     slave latency [0 ~ 500]
**                  timeout   -     supervision timeout [0x000a ~ 0xc80]
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleUpdateConnectionParams(BD_ADDR bd_addr, UINT16 min_int,
        UINT16 max_int, UINT16 latency, UINT16 timeout);

/*******************************************************************************
**
** Function         BTA_DmBleSetDataLength
**
** Description      This function is to set maximum LE data packet size
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleSetDataLength(BD_ADDR remote_device, UINT16 tx_data_length);

/*******************************************************************************
**
** Function         BTA_DmBleSetStorageParams
**
** Description      This function is called to set the storage parameters
**
** Parameters       batch_scan_full_max -Max storage space (in %) allocated to full scanning
**                  batch_scan_trunc_max -Max storage space (in %) allocated to truncated scanning
**                  batch_scan_notify_threshold - Setup notification level based on total space
**                  consumed by both pools. Setting it to 0 will disable threshold notification
**                  p_setup_cback - Setup callback
**                  p_thres_cback - Threshold callback
**                  p_rep_cback - Reports callback
**                  ref_value - Reference value
**
** Returns           None
**
*******************************************************************************/
extern void BTA_DmBleSetStorageParams(UINT8 batch_scan_full_max,
                                      UINT8 batch_scan_trunc_max,
                                      UINT8 batch_scan_notify_threshold,
                                      tBTA_BLE_SCAN_SETUP_CBACK *p_setup_cback,
                                      tBTA_BLE_SCAN_THRESHOLD_CBACK *p_thres_cback,
                                      tBTA_BLE_SCAN_REP_CBACK *p_rep_cback,
                                      tBTA_DM_BLE_REF_VALUE ref_value);

/*******************************************************************************
**
** Function         BTA_DmBleEnableBatchScan
**
** Description      This function is called to enable the batch scan
**
** Parameters       scan_mode -Batch scan mode
**                  scan_interval - Scan interval
**                  scan_window - Scan window
**                  discard_rule -Discard rules
**                  addr_type - Address type
**                  ref_value - Reference value
**
** Returns           None
**
*******************************************************************************/
extern void BTA_DmBleEnableBatchScan(tBTA_BLE_BATCH_SCAN_MODE scan_mode,
                                     UINT32 scan_interval, UINT32 scan_window,
                                     tBTA_BLE_DISCARD_RULE discard_rule,
                                     tBLE_ADDR_TYPE        addr_type,
                                     tBTA_DM_BLE_REF_VALUE ref_value);

/*******************************************************************************
**
** Function         BTA_DmBleReadScanReports
**
** Description      This function is called to read the batch scan reports
**
** Parameters       scan_mode -Batch scan mode
**                  ref_value - Reference value
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleReadScanReports(tBTA_BLE_BATCH_SCAN_MODE scan_type,
                                     tBTA_DM_BLE_REF_VALUE ref_value);

/*******************************************************************************
**
** Function         BTA_DmBleDisableBatchScan
**
** Description      This function is called to disable the batch scanning
**
** Parameters       ref_value - Reference value
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleDisableBatchScan(tBTA_DM_BLE_REF_VALUE ref_value);

/*******************************************************************************
**
** Function         BTA_DmEnableScanFilter
**
** Description      This function is called to enable the adv data payload filter
**
** Parameters       action - enable or disable the APCF feature
**                  p_cmpl_cback - Command completed callback
**                  ref_value - Reference value
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmEnableScanFilter(UINT8 action,
                                   tBTA_DM_BLE_PF_STATUS_CBACK *p_cmpl_cback,
                                   tBTA_DM_BLE_REF_VALUE ref_value);

/*******************************************************************************
**
** Function         BTA_DmBleScanFilterSetup
**
** Description      This function is called to setup the filter params
**
** Parameters       p_target: enable the filter condition on a target device; if NULL
**                  filt_index - Filter index
**                  p_filt_params -Filter parameters
**                  ref_value - Reference value
**                  action - Add, delete or clear
**                  p_cmpl_back - Command completed callback
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleScanFilterSetup(UINT8 action,
                                     tBTA_DM_BLE_PF_FILT_INDEX filt_index,
                                     tBTA_DM_BLE_PF_FILT_PARAMS *p_filt_params,
                                     tBLE_BD_ADDR *p_target,
                                     tBTA_DM_BLE_PF_PARAM_CBACK *p_cmpl_cback,
                                     tBTA_DM_BLE_REF_VALUE ref_value);

/*******************************************************************************
**
** Function         BTA_DmBleCfgFilterCondition
**
** Description      This function is called to configure the adv data payload filter
**                  condition.
**
** Parameters       action: to read/write/clear
**                  cond_type: filter condition type
**                  filt_index - Filter index
**                  p_cond: filter condition parameter
**                  p_cmpl_back - Command completed callback
**                  ref_value - Reference value
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleCfgFilterCondition(tBTA_DM_BLE_SCAN_COND_OP action,
                                        tBTA_DM_BLE_PF_COND_TYPE cond_type,
                                        tBTA_DM_BLE_PF_FILT_INDEX filt_index,
                                        tBTA_DM_BLE_PF_COND_PARAM *p_cond,
                                        tBTA_DM_BLE_PF_CFG_CBACK *p_cmpl_cback,
                                        tBTA_DM_BLE_REF_VALUE ref_value);


/*******************************************************************************
**
** Function         BTA_DmBleTrackAdvertiser
**
** Description      This function is called to track the advertiser
**
** Parameters    ref_value - Reference value
**               p_track_adv_cback - ADV callback
**
** Returns          None
**
*******************************************************************************/
extern void BTA_DmBleTrackAdvertiser(tBTA_DM_BLE_REF_VALUE ref_value,
                                     tBTA_BLE_TRACK_ADV_CBACK *p_track_adv_cback);

/*******************************************************************************
**
** Function         BTA_DmBleGetEnergyInfo
**
** Description      This function is called to obtain the energy info
**
** Parameters       p_cmpl_cback - Command complete callback
**
** Returns          void
**
*******************************************************************************/
extern void BTA_DmBleGetEnergyInfo(tBTA_BLE_ENERGY_INFO_CBACK *p_cmpl_cback);

/*******************************************************************************
**
** Function         BTA_BrcmInit
**
** Description      This function initializes Broadcom specific VS handler in BTA
**
** Returns          void
**
*******************************************************************************/
extern void BTA_VendorInit  (void);

/*******************************************************************************
**
** Function         BTA_BrcmCleanup
**
** Description      This function frees up Broadcom specific VS specific dynamic memory
**
** Returns          void
**
*******************************************************************************/
extern void BTA_VendorCleanup (void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* BTA_API_H */
