/******************************************************************************
 *
 *  Copyright (C) 2005-2012 Broadcom Corporation
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

#ifndef _ESP_BT_HH_API_H_
#define _ESP_BT_HH_API_H_

#include "esp_err.h"
#include "esp_bt_defs.h"

/*****************************************************************************
**  Constants and Type Definitions
*****************************************************************************/
#ifndef BTA_HH_DEBUG
#define BTA_HH_DEBUG    TRUE
#endif

#ifndef BTA_HH_SSR_MAX_LATENCY_DEF
#define BTA_HH_SSR_MAX_LATENCY_DEF  800 /* 500 ms*/
#endif

#ifndef BTA_HH_SSR_MIN_TOUT_DEF
#define BTA_HH_SSR_MIN_TOUT_DEF     2
#endif

/* defined the minimum offset */
#define BTA_HH_MIN_OFFSET       L2CAP_MIN_OFFSET+1

/* HID_HOST_MAX_DEVICES can not exceed 15 for th design of BTA HH */
#define BTA_HH_IDX_INVALID      0xff
#define BTA_HH_MAX_KNOWN        HID_HOST_MAX_DEVICES

/* Security Service Levels [bit mask] (BTM_SetSecurityLevel)
 ** Encryption should not be used without authentication
 */
#define BTM_SEC_NONE               0x0000 /* Nothing required */
#define BTM_SEC_IN_AUTHORIZE       0x0001 /* Inbound call requires authorization */
#define BTM_SEC_IN_AUTHENTICATE    0x0002 /* Inbound call requires authentication */
#define BTM_SEC_IN_ENCRYPT         0x0004 /* Inbound call requires encryption */
#define BTM_SEC_OUT_AUTHORIZE      0x0008 /* Outbound call requires authorization */
#define BTM_SEC_OUT_AUTHENTICATE   0x0010 /* Outbound call requires authentication */
#define BTM_SEC_OUT_ENCRYPT        0x0020 /* Outbound call requires encryption */
#define BTM_SEC_MODE4_LEVEL4       0x0040 /* Secure Connections Only Mode */
#define BTM_SEC_FORCE_MASTER       0x0100 /* Need to switch connection to be master */
#define BTM_SEC_ATTEMPT_MASTER     0x0200 /* Try to switch connection to be master */
#define BTM_SEC_FORCE_SLAVE        0x0400 /* Need to switch connection to be master */
#define BTM_SEC_ATTEMPT_SLAVE      0x0800 /* Try to switch connection to be slave */
#define BTM_SEC_IN_MITM            0x1000 /* inbound Do man in the middle protection */
#define BTM_SEC_OUT_MITM           0x2000 /* outbound Do man in the middle protection */
#define BTM_SEC_IN_MIN_16_DIGIT_PIN 0x4000 /* enforce a minimum of 16 digit for sec mode 2 */

/* Security Setting Mask */
#define BTA_SEC_NONE            BTM_SEC_NONE                                         /* No security. */
#define BTA_SEC_AUTHORIZE       (BTM_SEC_IN_AUTHORIZE )                              /* Authorization required (only needed for out going connection )*/
#define BTA_SEC_AUTHENTICATE    (BTM_SEC_IN_AUTHENTICATE | BTM_SEC_OUT_AUTHENTICATE) /* Authentication required. */
#define BTA_SEC_ENCRYPT         (BTM_SEC_IN_ENCRYPT | BTM_SEC_OUT_ENCRYPT)           /* Encryption required. */
#define BTA_SEC_MODE4_LEVEL4    (BTM_SEC_MODE4_LEVEL4)                               /* Mode 4 level 4 service, i.e. incoming/outgoing MITM and P-256 encryption */
#define BTA_SEC_MITM            (BTM_SEC_IN_MITM | BTM_SEC_OUT_MITM)                 /* Man-In-The_Middle protection */
#define BTA_SEC_IN_16_DIGITS    (BTM_SEC_IN_MIN_16_DIGIT_PIN)                        /* Min 16 digit for pin code */

#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
/* GATT_MAX_PHY_CHANNEL can not exceed 14 for the design of BTA HH */
#define BTA_HH_LE_MAX_KNOWN     GATT_MAX_PHY_CHANNEL
#define BTA_HH_MAX_DEVICE        (HID_HOST_MAX_DEVICES + GATT_MAX_PHY_CHANNEL)
#else
#define BTA_HH_MAX_DEVICE       HID_HOST_MAX_DEVICES
#endif
/* invalid device handle */
#define BTA_HH_INVALID_HANDLE   0xff

#define BTA_HH_SSR_PARAM_INVALID       HID_SSR_PARAM_INVALID

/* id DI is not existing in remote device, vendor_id in tBTA_HH_DEV_DSCP_INFO will be set to 0xffff */
#define BTA_HH_VENDOR_ID_INVALID       0xffff

/* application ID(none-zero) for each type of device */
#define BTA_HH_APP_ID_MI            1
#define BTA_HH_APP_ID_KB            2
#define BTA_HH_APP_ID_RMC           3
#define BTA_HH_APP_ID_3DSG          4
#define BTA_HH_APP_ID_JOY           5
#define BTA_HH_APP_ID_GPAD          6
#define BTA_HH_APP_ID_LE            0xff

/* type of devices, bit mask */
#define BTA_HH_DEVT_UNKNOWN      0x00
#define BTA_HH_DEVT_JOS          0x01           /* joy stick */
#define BTA_HH_DEVT_GPD          0x02           /* game pad */
#define BTA_HH_DEVT_RMC          0x03           /* remote control */
#define BTA_HH_DEVT_SED          0x04           /* sensing device */
#define BTA_HH_DEVT_DGT          0x05           /* Digitizer tablet */
#define BTA_HH_DEVT_CDR          0x06           /* card reader */
#define BTA_HH_DEVT_KBD          0x10           /* keyboard */
#define BTA_HH_DEVT_MIC          0x20           /* pointing device */
#define BTA_HH_DEVT_COM          0x30           /* Combo keyboard/pointing */
#define BTA_HH_DEVT_OTHER        0x80
typedef uint8_t  tBTA_HH_DEVT;


/* BTA HID Host callback events */
#define BTA_HH_ENABLE_EVT       0       /* HH enabled */
#define BTA_HH_DISABLE_EVT      1       /* HH disabled */
#define BTA_HH_OPEN_EVT         2       /* connection opened */
#define BTA_HH_CLOSE_EVT        3       /* connection closed */
#define BTA_HH_GET_RPT_EVT      4       /* BTA_HhGetReport callback */
#define BTA_HH_SET_RPT_EVT      5       /* BTA_HhSetReport callback */
#define BTA_HH_GET_PROTO_EVT    6       /* BTA_GetProtoMode callback */
#define BTA_HH_SET_PROTO_EVT    7       /* BTA_HhSetProtoMode callback */
#define BTA_HH_GET_IDLE_EVT     8       /* BTA_HhGetIdle comes callback */
#define BTA_HH_SET_IDLE_EVT     9       /* BTA_HhSetIdle finish callback */
#define BTA_HH_GET_DSCP_EVT     10      /* Get report descriptor */
#define BTA_HH_ADD_DEV_EVT      11      /* Add Device callback */
#define BTA_HH_RMV_DEV_EVT      12      /* remove device finished */
#define BTA_HH_VC_UNPLUG_EVT    13      /* virtually unplugged */
#define BTA_HH_DATA_EVT         15
#define BTA_HH_API_ERR_EVT      16      /* API error is caught */
#define BTA_HH_UPDATE_SCPP_EVT  17      /* update scan paramter complete */
typedef uint16_t tBTA_HH_EVT;

/* type of protocol mode */
#define BTA_HH_PROTO_RPT_MODE                   (0x00)
#define BTA_HH_PROTO_BOOT_MODE                  (0x01)
#define BTA_HH_PROTO_UNKNOWN                    (0xff)
typedef uint8_t   tBTA_HH_PROTO_MODE;

#define BTA_HH_VIRTUAL_CABLE           HID_VIRTUAL_CABLE
#define BTA_HH_NORMALLY_CONNECTABLE    HID_NORMALLY_CONNECTABLE
#define BTA_HH_RECONN_INIT             HID_RECONN_INIT
#define BTA_HH_SDP_DISABLE             HID_SDP_DISABLE
#define BTA_HH_BATTERY_POWER           HID_BATTERY_POWER
#define BTA_HH_REMOTE_WAKE             HID_REMOTE_WAKE
#define BTA_HH_SUP_TOUT_AVLBL          HID_SUP_TOUT_AVLBL
#define BTA_HH_SEC_REQUIRED            HID_SEC_REQUIRED
typedef uint16_t tBTA_HH_ATTR_MASK;

enum {
    BTA_HH_OK,
    BTA_HH_HS_HID_NOT_READY,    /* handshake error : device not ready */
    BTA_HH_HS_INVALID_RPT_ID,   /* handshake error : invalid report ID */
    BTA_HH_HS_TRANS_NOT_SPT,    /* handshake error : transaction not spt */
    BTA_HH_HS_INVALID_PARAM,    /* handshake error : invalid paremter */
    BTA_HH_HS_ERROR,            /* handshake error : unspecified HS error */
    BTA_HH_ERR,                 /* general BTA HH error */
    BTA_HH_ERR_SDP,             /* SDP error */
    BTA_HH_ERR_PROTO,           /* SET_Protocol error, only used in BTA_HH_OPEN_EVT callback */
    BTA_HH_ERR_DB_FULL,         /* device database full error, used in BTA_HH_OPEN_EVT/BTA_HH_ADD_DEV_EVT */
    BTA_HH_ERR_TOD_UNSPT,       /* type of device not supported */
    BTA_HH_ERR_NO_RES,          /* out of system resources */
    BTA_HH_ERR_AUTH_FAILED,     /* authentication fail */
    BTA_HH_ERR_HDL,
    BTA_HH_ERR_SEC
};
typedef uint8_t tBTA_HH_STATUS;

enum {
    BTA_HH_RPTT_RESRV,      /* reserved         */
    BTA_HH_RPTT_INPUT,      /* input report     */
    BTA_HH_RPTT_OUTPUT,     /* output report    */
    BTA_HH_RPTT_FEATURE     /* feature report   */
};
typedef uint8_t tBTA_HH_RPT_TYPE;

/* HID_CONTROL operation code used in BTA_HhSendCtrl()
*/
enum {
    BTA_HH_CTRL_NOP         = 0,                       /* mapping from BTE */
    BTA_HH_CTRL_HARD_RESET,                            /* hard reset       */
    BTA_HH_CTRL_SOFT_RESET,                            /* soft reset       */
    BTA_HH_CTRL_SUSPEND,                               /* enter suspend    */
    BTA_HH_CTRL_EXIT_SUSPEND,                          /* exit suspend     */
    BTA_HH_CTRL_VIRTUAL_CABLE_UNPLUG                   /* virtual unplug   */
};
typedef uint8_t tBTA_HH_TRANS_CTRL_TYPE;


typedef struct desc_info {
    uint16_t dl_len;
    uint8_t *dsc_list;
} tBTA_HH_DEV_DESCR;

/* Define the header of each buffer used in the Bluetooth stack. */
typedef struct {
    uint16_t          event;
    uint16_t          len;
    uint16_t          offset;
    uint16_t          layer_specific;
    uint8_t           data[];
} BT_HDR;

#define BT_HDR_SIZE (sizeof (BT_HDR))

/* callback event data for BTA_HH_OPEN_EVT */
typedef struct {
    esp_bd_addr_t       bda;            /* HID device bd address    */
    tBTA_HH_STATUS      status;         /* operation status         */
    uint8_t             handle;         /* device handle            */

} tBTA_HH_CONN;

typedef tBTA_HH_CONN tBTA_HH_DEV_INFO;

/* callback event data */
typedef struct {
    tBTA_HH_STATUS      status;         /* operation status         */
    uint8_t             handle;         /* device handle            */
} tBTA_HH_CBDATA;

/* report descriptor information */
typedef struct {
    uint16_t            vendor_id;      /* vendor ID */
    uint16_t            product_id;     /* product ID */
    uint16_t            version;        /* version */
    uint16_t            ssr_max_latency;/* SSR max latency, BTA_HH_SSR_PARAM_INVALID if unknown */
    uint16_t            ssr_min_tout;   /* SSR min timeout, BTA_HH_SSR_PARAM_INVALID if unknown */
    uint8_t             ctry_code;      /*Country Code.*/
    tBTA_HH_DEV_DESCR   descriptor;
} tBTA_HH_DEV_DSCP_INFO;

/* handshake data */
typedef struct {
    tBTA_HH_STATUS      status;         /* handshake status */
    uint8_t             handle;         /* device handle    */
    union {
        tBTA_HH_PROTO_MODE  proto_mode; /* GET_PROTO_EVT :protocol mode */
        BT_HDR              *p_rpt_data;/* GET_RPT_EVT   : report data  */
        uint8_t             idle_rate;  /* GET_IDLE_EVT  : idle rate    */
    } rsp_data;

} tBTA_HH_HSDATA;

/* union of data associated with HD callback */
typedef union {
    tBTA_HH_DEV_INFO        dev_info;           /* BTA_HH_ADD_DEV_EVT, BTA_HH_RMV_DEV_EVT   */
    tBTA_HH_CONN            conn;               /* BTA_HH_OPEN_EVT      */
    tBTA_HH_CBDATA          dev_status;         /* BTA_HH_CLOSE_EVT,
                                                   BTA_HH_SET_PROTO_EVT
                                                   BTA_HH_SET_RPT_EVT
                                                   BTA_HH_SET_IDLE_EVT
                                                   BTA_HH_UPDATE_SCPP_EVT */

    tBTA_HH_STATUS          status;             /* BTA_HH_ENABLE_EVT */
    tBTA_HH_DEV_DSCP_INFO   dscp_info;          /* BTA_HH_GET_DSCP_EVT */
    tBTA_HH_HSDATA          hs_data;            /* GET_ transaction callback
                                                   BTA_HH_GET_RPT_EVT
                                                   BTA_HH_GET_PROTO_EVT
                                                   BTA_HH_GET_IDLE_EVT */
} tBTA_HH;

/* BTA HH callback function */
typedef void (tBTA_HH_CBACK) (tBTA_HH_EVT event, tBTA_HH *p_data);

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/* This function enable HID host and registers HID-Host with lower layers.*/
extern void BTA_HhEnable(uint16_t sec_mask, tBTA_HH_CBACK *p_cback);

/* This function is called when the host is about power down. */
extern void BTA_HhDisable(void);

/* This function is called to start an inquiry and read SDP record of responding devices; connect to a device if only one active HID device is found. */
extern void BTA_HhOpen (esp_bd_addr_t dev_bda, uint8_t bd_type, tBTA_HH_PROTO_MODE mode, uint16_t sec_mask);

/* This function disconnects the device. */
extern void BTA_HhClose(uint8_t dev_handle);

/* This function set the protocol mode at specified HID handle */
extern void BTA_HhSetProtoMode(uint8_t handle, tBTA_HH_PROTO_MODE t_type);

/* This function get the protocol mode of a specified HID device. */
extern void BTA_HhGetProtoMode(uint8_t dev_handle);

/* send SET_REPORT to device. */
extern void BTA_HhSetReport(uint8_t dev_handle, tBTA_HH_RPT_TYPE r_type, BT_HDR *p_data);

/* Send a GET_REPORT to HID device. */
extern void BTA_HhGetReport(uint8_t dev_handle, tBTA_HH_RPT_TYPE r_type, uint8_t rpt_id, uint16_t buf_size);

/* send SET_IDLE to device. */
extern void BTA_HhSetIdle(uint8_t dev_handle, uint16_t idle_rate);

/* Send a GET_IDLE to HID device. */
extern void BTA_HhGetIdle(uint8_t dev_handle);

/* Send HID_CONTROL request to a HID device. */
extern void BTA_HhSendCtrl(uint8_t dev_handle, tBTA_HH_TRANS_CTRL_TYPE c_type);

/* Send DATA transaction to a HID device. */
extern void BTA_HhSendData(uint8_t dev_handle, esp_bd_addr_t dev_bda, BT_HDR  *p_buf);

/* Get report descriptor of the device */
extern void BTA_HhGetDscpInfo(uint8_t dev_handle);

/* Add a virtually cabled device into HID-Host device list to manage and assign a device handle for future API call, host applciation call this API at start-up to initialize its virtually cabled devices. */
extern void BTA_HhAddDev(esp_bd_addr_t bda, tBTA_HH_ATTR_MASK attr_mask, uint8_t sub_class, uint8_t app_id, tBTA_HH_DEV_DSCP_INFO dscp_info);

/* Remove a device from the HID host devices list. */
extern void BTA_HhRemoveDev(uint8_t dev_handle );

enum {
    BTA_HH_MOD_CTRL_KEY,
    BTA_HH_MOD_SHFT_KEY,
    BTA_HH_MOD_ALT_KEY,
    BTA_HH_MOD_GUI_KEY,
    BTA_HH_MOD_MAX_KEY
};

/* parsed boot mode keyboard report */
typedef struct {
    uint8_t          this_char[6];       /* virtual key code     */
    bool             mod_key[BTA_HH_MOD_MAX_KEY];/* ctrl, shift, Alt, GUI */
    bool             caps_lock;          /* is caps locked       */
    bool             num_lock;           /* is Num key pressed   */
} tBTA_HH_KEYBD_RPT;

/* parsed boot mode mouse report */
typedef struct {
    uint8_t               mouse_button;       /* mouse button is clicked   */
    int8_t                delta_x;            /* displacement x            */
    int8_t                delta_y;            /* displacement y            */
} tBTA_HH_MICE_RPT;

enum {
    BTA_HH_KEYBD_RPT_ID  =               1,
    BTA_HH_MOUSE_RPT_ID
};
typedef uint8_t tBTA_HH_BOOT_RPT_ID;

/* parsed Boot report */
typedef struct {
    tBTA_HH_BOOT_RPT_ID dev_type;           /* type of device report */
    union {
        tBTA_HH_KEYBD_RPT   keybd_rpt;      /* keyboard report      */
        tBTA_HH_MICE_RPT    mice_rpt;       /* mouse report         */
    }                   data_rpt;
} tBTA_HH_BOOT_RPT;

/* This utility function parse a boot mode report. */
extern void BTA_HhParseBootRpt(tBTA_HH_BOOT_RPT *p_data, uint8_t *p_report, uint16_t report_len);




#ifdef __cplusplus
}
#endif

#endif /* _ESP_BT_HH_API_H_ */
