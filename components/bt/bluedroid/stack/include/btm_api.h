/******************************************************************************
 *
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

/******************************************************************************
 *
 *  This file contains the Bluetooth Manager (BTM) API function external
 *  definitions.
 *
 ******************************************************************************/
#ifndef BTM_API_H
#define BTM_API_H

#include "bt_defs.h"
#include "bt_target.h"
#include "hcidefs.h"

#if SDP_INCLUDED == TRUE
#include "sdp_api.h"
#endif

#if SMP_INCLUDED == TRUE
#include "smp_api.h"
#endif
/*****************************************************************************
**  DEVICE CONTROL and COMMON
*****************************************************************************/
/*****************************
**  Device Control Constants
******************************/
/* Maximum number of bytes allowed for vendor specific command parameters */
#define BTM_MAX_VENDOR_SPECIFIC_LEN  HCI_COMMAND_SIZE

/* BTM application return status codes */
enum {
    BTM_SUCCESS = 0,                    /* 0  Command succeeded                 */
    BTM_CMD_STARTED,                    /* 1  Command started OK.               */
    BTM_BUSY,                           /* 2  Device busy with another command  */
    BTM_NO_RESOURCES,                   /* 3  No resources to issue command     */
    BTM_MODE_UNSUPPORTED,               /* 4  Request for 1 or more unsupported modes */
    BTM_ILLEGAL_VALUE,                  /* 5  Illegal parameter value           */
    BTM_WRONG_MODE,                     /* 6  Device in wrong mode for request  */
    BTM_UNKNOWN_ADDR,                   /* 7  Unknown remote BD address         */
    BTM_DEVICE_TIMEOUT,                 /* 8  Device timeout                    */
    BTM_BAD_VALUE_RET,                  /* 9  A bad value was received from HCI */
    BTM_ERR_PROCESSING,                 /* 10 Generic error                     */
    BTM_NOT_AUTHORIZED,                 /* 11 Authorization failed              */
    BTM_DEV_RESET,                      /* 12 Device has been reset             */
    BTM_CMD_STORED,                     /* 13 request is stored in control block */
    BTM_ILLEGAL_ACTION,                 /* 14 state machine gets illegal command */
    BTM_DELAY_CHECK,                    /* 15 delay the check on encryption */
    BTM_SCO_BAD_LENGTH,                 /* 16 Bad SCO over HCI data length */
    BTM_SUCCESS_NO_SECURITY,            /* 17 security passed, no security set  */
    BTM_FAILED_ON_SECURITY,             /* 18 security failed                   */
    BTM_REPEATED_ATTEMPTS,              /* 19 repeated attempts for LE security requests */
    BTM_MODE4_LEVEL4_NOT_SUPPORTED      /* 20 Secure Connections Only Mode can't be supported */
};

typedef uint8_t tBTM_STATUS;

#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
typedef enum {
    BTM_BR_ONE,                         /*0 First state or BR/EDR scan 1*/
    BTM_BLE_ONE,                        /*1BLE scan 1*/
    BTM_BR_TWO,                         /*2 BR/EDR scan 2*/
    BTM_BLE_TWO,                        /*3 BLE scan 2*/
    BTM_FINISH,                         /*4 End of Interleave Scan, or normal scan*/
    BTM_NO_INTERLEAVING                 /*5 No Interleaving*/
} btm_inq_state;
#endif



/*************************
**  Device Control Types
**************************/
#define BTM_DEVICE_ROLE_BR      0x01
#define BTM_DEVICE_ROLE_DUAL    0x02
#define BTM_MAX_DEVICE_ROLE     BTM_DEVICE_ROLE_DUAL
typedef UINT8 tBTM_DEVICE_ROLE;

/* Device name of peer (may be truncated to save space in BTM database) */
typedef UINT8 tBTM_BD_NAME[BTM_MAX_REM_BD_NAME_LEN + 1];

/* Structure returned with local version information */
typedef struct {
    UINT8       hci_version;
    UINT16      hci_revision;
    UINT8       lmp_version;
    UINT16      manufacturer;
    UINT16      lmp_subversion;
} tBTM_VERSION_INFO;

/* Structure returned with Vendor Specific Command complete callback */
typedef struct {
    UINT16  opcode;
    UINT16  param_len;
    UINT8   *p_param_buf;
} tBTM_VSC_CMPL;

#define  BTM_VSC_CMPL_DATA_SIZE  (BTM_MAX_VENDOR_SPECIFIC_LEN + sizeof(tBTM_VSC_CMPL))
/**************************************************
**  Device Control and General Callback Functions
***************************************************/
/* Callback function for when device status changes. Appl must poll for
** what the new state is (BTM_IsDeviceUp). The event occurs whenever the stack
** has detected that the controller status has changed. This asynchronous event
** is enabled/disabled by calling BTM_RegisterForDeviceStatusNotif().
*/
enum {
    BTM_DEV_STATUS_UP,
    BTM_DEV_STATUS_DOWN,
    BTM_DEV_STATUS_CMD_TOUT
};

typedef UINT8 tBTM_DEV_STATUS;


typedef void (tBTM_DEV_STATUS_CB) (tBTM_DEV_STATUS status);


/* Callback function for when a vendor specific event occurs. The length and
** array of returned parameter bytes are included. This asynchronous event
** is enabled/disabled by calling BTM_RegisterForVSEvents().
*/
typedef void (tBTM_VS_EVT_CB) (UINT8 len, UINT8 *p);


/* General callback function for notifying an application that a synchronous
** BTM function is complete. The pointer contains the address of any returned data.
*/
typedef void (tBTM_CMPL_CB) (void *p1);

/* VSC callback function for notifying an application that a synchronous
** BTM function is complete. The pointer contains the address of any returned data.
*/
typedef void (tBTM_VSC_CMPL_CB) (tBTM_VSC_CMPL *p1);

/* Callback for apps to check connection and inquiry filters.
** Parameters are the BD Address of remote and the Dev Class of remote.
** If the app returns none zero, the connection or inquiry result will be dropped.
*/
typedef UINT8 (tBTM_FILTER_CB) (BD_ADDR bd_addr, DEV_CLASS dc);

/*****************************************************************************
**  DEVICE DISCOVERY - Inquiry, Remote Name, Discovery, Class of Device
*****************************************************************************/
/*******************************
**  Device Discovery Constants
********************************/
/* Discoverable modes */
#define BTM_NON_DISCOVERABLE        0
#define BTM_LIMITED_DISCOVERABLE    1
#define BTM_GENERAL_DISCOVERABLE    2
#define BTM_DISCOVERABLE_MASK       (BTM_LIMITED_DISCOVERABLE|BTM_GENERAL_DISCOVERABLE)
#define BTM_MAX_DISCOVERABLE        BTM_GENERAL_DISCOVERABLE
/* high byte for BLE Discoverable modes */
#define BTM_BLE_NON_DISCOVERABLE        0x0000
#define BTM_BLE_LIMITED_DISCOVERABLE    0x0100
#define BTM_BLE_GENERAL_DISCOVERABLE    0x0200
#define BTM_BLE_MAX_DISCOVERABLE        BTM_BLE_GENERAL_DISCOVERABLE
#define BTM_BLE_DISCOVERABLE_MASK       (BTM_BLE_NON_DISCOVERABLE|BTM_BLE_LIMITED_DISCOVERABLE|BTM_BLE_GENERAL_DISCOVERABLE)

/* Connectable modes */
#define BTM_NON_CONNECTABLE         0
#define BTM_CONNECTABLE             1
#define BTM_CONNECTABLE_MASK        (BTM_NON_CONNECTABLE | BTM_CONNECTABLE)
/* high byte for BLE Connectable modes */
#define BTM_BLE_NON_CONNECTABLE      0x0000
#define BTM_BLE_CONNECTABLE          0x0100
#define BTM_BLE_MAX_CONNECTABLE      BTM_BLE_CONNECTABLE
#define BTM_BLE_CONNECTABLE_MASK    (BTM_BLE_NON_CONNECTABLE | BTM_BLE_CONNECTABLE)

/* Inquiry modes
 * Note: These modes are associated with the inquiry active values (BTM_*ACTIVE) */
#define BTM_INQUIRY_NONE            0
#define BTM_GENERAL_INQUIRY         0x01
#define BTM_LIMITED_INQUIRY         0x02
#define BTM_BR_INQUIRY_MASK         (BTM_GENERAL_INQUIRY | BTM_LIMITED_INQUIRY)

/* high byte of inquiry mode for BLE inquiry mode */
#define BTM_BLE_INQUIRY_NONE         0x00
#define BTM_BLE_GENERAL_INQUIRY      0x10
#define BTM_BLE_LIMITED_INQUIRY      0x20
#define BTM_BLE_INQUIRY_MASK         (BTM_BLE_GENERAL_INQUIRY|BTM_BLE_LIMITED_INQUIRY)

/* BTM_IsInquiryActive return values (Bit Mask)
 * Note: These bit masks are associated with the inquiry modes (BTM_*_INQUIRY) */
#define BTM_INQUIRY_INACTIVE        0x0     /* no inquiry in progress */
#define BTM_GENERAL_INQUIRY_ACTIVE  BTM_GENERAL_INQUIRY     /* a general inquiry is in progress */
#define BTM_LIMITED_INQUIRY_ACTIVE  BTM_LIMITED_INQUIRY     /* a limited inquiry is in progress */
#define BTM_PERIODIC_INQUIRY_ACTIVE 0x8     /* a periodic inquiry is active */
#define BTM_SSP_INQUIRY_ACTIVE      0x4     /* SSP is active, so inquiry is disallowed (work around for FW bug) */
#define BTM_LE_GENERAL_INQUIRY_ACTIVE  BTM_BLE_GENERAL_INQUIRY     /* a general inquiry is in progress */
#define BTM_LE_LIMITED_INQUIRY_ACTIVE  BTM_BLE_LIMITED_INQUIRY      /* a limited inquiry is in progress */

/* inquiry activity mask */
#define BTM_BR_INQ_ACTIVE_MASK        (BTM_GENERAL_INQUIRY_ACTIVE|BTM_LIMITED_INQUIRY_ACTIVE|BTM_PERIODIC_INQUIRY_ACTIVE) /* BR/EDR inquiry activity mask */
#define BTM_BLE_SCAN_ACTIVE_MASK      0xF0     /* LE scan activity mask */
#define BTM_BLE_INQ_ACTIVE_MASK       (BTM_LE_GENERAL_INQUIRY_ACTIVE|BTM_LE_LIMITED_INQUIRY_ACTIVE) /* LE inquiry activity mask*/
#define BTM_INQUIRY_ACTIVE_MASK       (BTM_BR_INQ_ACTIVE_MASK | BTM_BLE_INQ_ACTIVE_MASK) /* inquiry activity mask */

/* Define scan types */
#define BTM_SCAN_TYPE_STANDARD      0
#define BTM_SCAN_TYPE_INTERLACED    1       /* 1.2 devices only */

/* Define inquiry results mode */
#define BTM_INQ_RESULT_STANDARD     0
#define BTM_INQ_RESULT_WITH_RSSI    1
#define BTM_INQ_RESULT_EXTENDED     2

#define BTM_INQ_RES_IGNORE_RSSI     0x7f    /* RSSI value not supplied (ignore it) */

/* Inquiry Filter Condition types (see tBTM_INQ_PARMS) */
#define BTM_CLR_INQUIRY_FILTER          0                   /* Inquiry Filtering is turned off */
#define BTM_FILTER_COND_DEVICE_CLASS    HCI_FILTER_COND_DEVICE_CLASS /* Filter on device class */
#define BTM_FILTER_COND_BD_ADDR         HCI_FILTER_COND_BD_ADDR /* Filter on device addr */

/* State of the remote name retrieval during inquiry operations.
** Used in the tBTM_INQ_INFO structure, and returned in the
** BTM_InqDbRead, BTM_InqDbFirst, and BTM_InqDbNext functions.
** The name field is valid when the state returned is
** BTM_INQ_RMT_NAME_DONE */
#define BTM_INQ_RMT_NAME_EMPTY      0
#define BTM_INQ_RMT_NAME_PENDING    1
#define BTM_INQ_RMT_NAME_DONE       2
#define BTM_INQ_RMT_NAME_FAILED     3

/*********************************
 *** Class of Device constants ***
 *********************************/
#define BTM_FORMAT_TYPE_1   0x00

/****************************
** minor device class field
*****************************/

/* 0x00 is used as unclassified for all minor device classes */
#define BTM_COD_MINOR_UNCLASSIFIED          0x00

/* minor device class field for Computer Major Class */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */
#define BTM_COD_MINOR_DESKTOP_WORKSTATION   0x04
#define BTM_COD_MINOR_SERVER_COMPUTER       0x08
#define BTM_COD_MINOR_LAPTOP                0x0C
#define BTM_COD_MINOR_HANDHELD_PC_PDA       0x10    /* clam shell */
#define BTM_COD_MINOR_PALM_SIZE_PC_PDA      0x14
#define BTM_COD_MINOR_WEARABLE_COMPUTER     0x18    /* watch sized */

/* minor device class field for Phone Major Class */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */
#define BTM_COD_MINOR_CELLULAR              0x04
#define BTM_COD_MINOR_CORDLESS              0x08
#define BTM_COD_MINOR_SMART_PHONE           0x0C
#define BTM_COD_MINOR_WIRED_MDM_V_GTWY      0x10 /* wired modem or voice gatway */
#define BTM_COD_MINOR_ISDN_ACCESS           0x14

/* minor device class field for LAN Access Point Major Class */
/* Load Factor Field bit 5-7 */
#define BTM_COD_MINOR_FULLY_AVAILABLE       0x00
#define BTM_COD_MINOR_1_17_UTILIZED         0x20
#define BTM_COD_MINOR_17_33_UTILIZED        0x40
#define BTM_COD_MINOR_33_50_UTILIZED        0x60
#define BTM_COD_MINOR_50_67_UTILIZED        0x80
#define BTM_COD_MINOR_67_83_UTILIZED        0xA0
#define BTM_COD_MINOR_83_99_UTILIZED        0xC0
#define BTM_COD_MINOR_NO_SERVICE_AVAILABLE  0xE0
/* sub-Field bit 2-4 */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */

/* minor device class field for Audio/Video Major Class */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */
#define BTM_COD_MINOR_CONFM_HEADSET         0x04
#define BTM_COD_MINOR_CONFM_HANDSFREE       0x08
#define BTM_COD_MINOR_MICROPHONE            0x10
#define BTM_COD_MINOR_LOUDSPEAKER           0x14
#define BTM_COD_MINOR_HEADPHONES            0x18
#define BTM_COD_MINOR_PORTABLE_AUDIO        0x1C
#define BTM_COD_MINOR_CAR_AUDIO             0x20
#define BTM_COD_MINOR_SET_TOP_BOX           0x24
#define BTM_COD_MINOR_HIFI_AUDIO            0x28
#define BTM_COD_MINOR_VCR                   0x2C
#define BTM_COD_MINOR_VIDEO_CAMERA          0x30
#define BTM_COD_MINOR_CAMCORDER             0x34
#define BTM_COD_MINOR_VIDEO_MONITOR         0x38
#define BTM_COD_MINOR_VIDDISP_LDSPKR        0x3C
#define BTM_COD_MINOR_VIDEO_CONFERENCING    0x40
#define BTM_COD_MINOR_GAMING_TOY            0x48

/* minor device class field for Peripheral Major Class */
/* Bits 6-7 independently specify mouse, keyboard, or combo mouse/keyboard */
#define BTM_COD_MINOR_KEYBOARD              0x40
#define BTM_COD_MINOR_POINTING              0x80
#define BTM_COD_MINOR_COMBO                 0xC0
/* Bits 2-5 OR'd with selection from bits 6-7 */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */
#define BTM_COD_MINOR_JOYSTICK              0x04
#define BTM_COD_MINOR_GAMEPAD               0x08
#define BTM_COD_MINOR_REMOTE_CONTROL        0x0C
#define BTM_COD_MINOR_SENSING_DEVICE        0x10
#define BTM_COD_MINOR_DIGITIZING_TABLET     0x14
#define BTM_COD_MINOR_CARD_READER           0x18    /* e.g. SIM card reader */
#define BTM_COD_MINOR_DIGITAL_PAN           0x1C
#define BTM_COD_MINOR_HAND_SCANNER          0x20
#define BTM_COD_MINOR_HAND_GESTURAL_INPUT   0x24

/* minor device class field for Imaging Major Class */
/* Bits 5-7 independently specify display, camera, scanner, or printer */
#define BTM_COD_MINOR_DISPLAY               0x10
#define BTM_COD_MINOR_CAMERA                0x20
#define BTM_COD_MINOR_SCANNER               0x40
#define BTM_COD_MINOR_PRINTER               0x80
/* Bits 2-3 Reserved */
/* #define BTM_COD_MINOR_UNCLASSIFIED       0x00    */

/* minor device class field for Wearable Major Class */
/* Bits 2-7 meaningful    */
#define BTM_COD_MINOR_WRIST_WATCH           0x04
#define BTM_COD_MINOR_PAGER                 0x08
#define BTM_COD_MINOR_JACKET                0x0C
#define BTM_COD_MINOR_HELMET                0x10
#define BTM_COD_MINOR_GLASSES               0x14

/* minor device class field for Toy Major Class */
/* Bits 2-7 meaningful    */
#define BTM_COD_MINOR_ROBOT                 0x04
#define BTM_COD_MINOR_VEHICLE               0x08
#define BTM_COD_MINOR_DOLL_ACTION_FIGURE    0x0C
#define BTM_COD_MINOR_CONTROLLER            0x10
#define BTM_COD_MINOR_GAME                  0x14

/* minor device class field for Health Major Class */
/* Bits 2-7 meaningful    */
#define BTM_COD_MINOR_BLOOD_MONITOR         0x04
#define BTM_COD_MINOR_THERMOMETER           0x08
#define BTM_COD_MINOR_WEIGHING_SCALE        0x0C
#define BTM_COD_MINOR_GLUCOSE_METER         0x10
#define BTM_COD_MINOR_PULSE_OXIMETER        0x14
#define BTM_COD_MINOR_HEART_PULSE_MONITOR   0x18
#define BTM_COD_MINOR_HEALTH_DATA_DISPLAY   0x1C
#define BTM_COD_MINOR_STEP_COUNTER          0x20
#define BTM_COD_MINOR_BODY_COM_ANALYZER     0x24
#define BTM_COD_MINOR_PEAK_FLOW_MONITOR     0x28
#define BTM_COD_MINOR_MEDICATION_MONITOR    0x2C
#define BTM_COD_MINOR_KNEE_PROSTHESIS       0x30
#define BTM_COD_MINOR_ANKLE_PROSTHESIS      0x34


/***************************
** major device class field
****************************/
#define BTM_COD_MAJOR_MISCELLANEOUS         0x00
#define BTM_COD_MAJOR_COMPUTER              0x01
#define BTM_COD_MAJOR_PHONE                 0x02
#define BTM_COD_MAJOR_LAN_ACCESS_PT         0x03
#define BTM_COD_MAJOR_AUDIO                 0x04
#define BTM_COD_MAJOR_PERIPHERAL            0x05
#define BTM_COD_MAJOR_IMAGING               0x06
#define BTM_COD_MAJOR_WEARABLE              0x07
#define BTM_COD_MAJOR_TOY                   0x08
#define BTM_COD_MAJOR_HEALTH                0x09
#define BTM_COD_MAJOR_UNCLASSIFIED          0x1F

/***************************
** service class fields
****************************/
#define BTM_COD_SERVICE_LMTD_DISCOVER       0x0020
#define BTM_COD_SERVICE_POSITIONING         0x0100
#define BTM_COD_SERVICE_NETWORKING          0x0200
#define BTM_COD_SERVICE_RENDERING           0x0400
#define BTM_COD_SERVICE_CAPTURING           0x0800
#define BTM_COD_SERVICE_OBJ_TRANSFER        0x1000
#define BTM_COD_SERVICE_AUDIO               0x2000
#define BTM_COD_SERVICE_TELEPHONY           0x4000
#define BTM_COD_SERVICE_INFORMATION         0x8000

/* class of device field macros */
#define BTM_COD_FORMAT_TYPE(u8, pd)         {u8  = pd[2]&0x03;}
#define BTM_COD_MINOR_CLASS(u8, pd)         {u8  = pd[2]&0xFC;}
#define BTM_COD_MAJOR_CLASS(u8, pd)         {u8  = pd[1]&0x1F;}
#define BTM_COD_SERVICE_CLASS(u16, pd)      {u16 = pd[0]; u16<<=8; u16 += pd[1]&0xE0;}

/* to set the fields (assumes that format type is always 0) */
#define FIELDS_TO_COD(pd, mn, mj, sv) {pd[2] = mn; pd[1] =              \
                                       mj+ ((sv)&BTM_COD_SERVICE_CLASS_LO_B); \
                                       pd[0] = (sv) >> 8;}

/* the COD masks */
#define BTM_COD_FORMAT_TYPE_MASK      0x03
#define BTM_COD_MINOR_CLASS_MASK      0xFC
#define BTM_COD_MAJOR_CLASS_MASK      0x1F
#define BTM_COD_SERVICE_CLASS_LO_B    0x00E0
#define BTM_COD_SERVICE_CLASS_MASK    0xFFE0


/* BTM service definitions
** Used for storing EIR data to bit mask
*/
enum {
    BTM_EIR_UUID_SERVCLASS_SERVICE_DISCOVERY_SERVER,
    /*    BTM_EIR_UUID_SERVCLASS_BROWSE_GROUP_DESCRIPTOR,   */
    /*    BTM_EIR_UUID_SERVCLASS_PUBLIC_BROWSE_GROUP,       */
    BTM_EIR_UUID_SERVCLASS_SERIAL_PORT,
    BTM_EIR_UUID_SERVCLASS_LAN_ACCESS_USING_PPP,
    BTM_EIR_UUID_SERVCLASS_DIALUP_NETWORKING,
    BTM_EIR_UUID_SERVCLASS_IRMC_SYNC,
    BTM_EIR_UUID_SERVCLASS_OBEX_OBJECT_PUSH,
    BTM_EIR_UUID_SERVCLASS_OBEX_FILE_TRANSFER,
    BTM_EIR_UUID_SERVCLASS_IRMC_SYNC_COMMAND,
    BTM_EIR_UUID_SERVCLASS_HEADSET,
    BTM_EIR_UUID_SERVCLASS_CORDLESS_TELEPHONY,
    BTM_EIR_UUID_SERVCLASS_AUDIO_SOURCE,
    BTM_EIR_UUID_SERVCLASS_AUDIO_SINK,
    BTM_EIR_UUID_SERVCLASS_AV_REM_CTRL_TARGET,
    /*    BTM_EIR_UUID_SERVCLASS_ADV_AUDIO_DISTRIBUTION,    */
    BTM_EIR_UUID_SERVCLASS_AV_REMOTE_CONTROL,
    /*    BTM_EIR_UUID_SERVCLASS_VIDEO_CONFERENCING,        */
    BTM_EIR_UUID_SERVCLASS_INTERCOM,
    BTM_EIR_UUID_SERVCLASS_FAX,
    BTM_EIR_UUID_SERVCLASS_HEADSET_AUDIO_GATEWAY,
    /*    BTM_EIR_UUID_SERVCLASS_WAP,                       */
    /*    BTM_EIR_UUID_SERVCLASS_WAP_CLIENT,                */
    BTM_EIR_UUID_SERVCLASS_PANU,
    BTM_EIR_UUID_SERVCLASS_NAP,
    BTM_EIR_UUID_SERVCLASS_GN,
    BTM_EIR_UUID_SERVCLASS_DIRECT_PRINTING,
    /*    BTM_EIR_UUID_SERVCLASS_REFERENCE_PRINTING,        */
    BTM_EIR_UUID_SERVCLASS_IMAGING,
    BTM_EIR_UUID_SERVCLASS_IMAGING_RESPONDER,
    BTM_EIR_UUID_SERVCLASS_IMAGING_AUTO_ARCHIVE,
    BTM_EIR_UUID_SERVCLASS_IMAGING_REF_OBJECTS,
    BTM_EIR_UUID_SERVCLASS_HF_HANDSFREE,
    BTM_EIR_UUID_SERVCLASS_AG_HANDSFREE,
    BTM_EIR_UUID_SERVCLASS_DIR_PRT_REF_OBJ_SERVICE,
    /*    BTM_EIR_UUID_SERVCLASS_REFLECTED_UI,              */
    BTM_EIR_UUID_SERVCLASS_BASIC_PRINTING,
    BTM_EIR_UUID_SERVCLASS_PRINTING_STATUS,
    BTM_EIR_UUID_SERVCLASS_HUMAN_INTERFACE,
    BTM_EIR_UUID_SERVCLASS_CABLE_REPLACEMENT,
    BTM_EIR_UUID_SERVCLASS_HCRP_PRINT,
    BTM_EIR_UUID_SERVCLASS_HCRP_SCAN,
    /*    BTM_EIR_UUID_SERVCLASS_COMMON_ISDN_ACCESS,        */
    /*    BTM_EIR_UUID_SERVCLASS_VIDEO_CONFERENCING_GW,     */
    /*    BTM_EIR_UUID_SERVCLASS_UDI_MT,                    */
    /*    BTM_EIR_UUID_SERVCLASS_UDI_TA,                    */
    /*    BTM_EIR_UUID_SERVCLASS_VCP,                       */
    BTM_EIR_UUID_SERVCLASS_SAP,
    BTM_EIR_UUID_SERVCLASS_PBAP_PCE,
    BTM_EIR_UUID_SERVCLASS_PBAP_PSE,
    /*    BTM_EIR_UUID_SERVCLASS_TE_PHONE_ACCESS,           */
    /*    BTM_EIR_UUID_SERVCLASS_ME_PHONE_ACCESS,           */
    BTM_EIR_UUID_SERVCLASS_PHONE_ACCESS,
    BTM_EIR_UUID_SERVCLASS_HEADSET_HS,
    BTM_EIR_UUID_SERVCLASS_PNP_INFORMATION,
    /*    BTM_EIR_UUID_SERVCLASS_GENERIC_NETWORKING,        */
    /*    BTM_EIR_UUID_SERVCLASS_GENERIC_FILETRANSFER,      */
    /*    BTM_EIR_UUID_SERVCLASS_GENERIC_AUDIO,             */
    /*    BTM_EIR_UUID_SERVCLASS_GENERIC_TELEPHONY,         */
    /*    BTM_EIR_UUID_SERVCLASS_UPNP_SERVICE,              */
    /*    BTM_EIR_UUID_SERVCLASS_UPNP_IP_SERVICE,           */
    /*    BTM_EIR_UUID_SERVCLASS_ESDP_UPNP_IP_PAN,          */
    /*    BTM_EIR_UUID_SERVCLASS_ESDP_UPNP_IP_LAP,          */
    /*    BTM_EIR_UUID_SERVCLASS_ESDP_UPNP_IP_L2CAP,        */
    BTM_EIR_UUID_SERVCLASS_VIDEO_SOURCE,
    BTM_EIR_UUID_SERVCLASS_VIDEO_SINK,
    /*    BTM_EIR_UUID_SERVCLASS_VIDEO_DISTRIBUTION         */
    /*    BTM_EIR_UUID_SERVCLASS_HDP_PROFILE                */
    BTM_EIR_UUID_SERVCLASS_MESSAGE_ACCESS,
    BTM_EIR_UUID_SERVCLASS_MESSAGE_NOTIFICATION,
    BTM_EIR_UUID_SERVCLASS_HDP_SOURCE,
    BTM_EIR_UUID_SERVCLASS_HDP_SINK,
    BTM_EIR_MAX_SERVICES
};

/* search result in EIR of inquiry database */
#define BTM_EIR_FOUND           0
#define BTM_EIR_NOT_FOUND       1
#define BTM_EIR_UNKNOWN         2

typedef UINT8 tBTM_EIR_SEARCH_RESULT;

#define BTM_EIR_FLAGS_TYPE                  HCI_EIR_FLAGS_TYPE                  /* 0x01 */
#define BTM_EIR_MORE_16BITS_UUID_TYPE       HCI_EIR_MORE_16BITS_UUID_TYPE       /* 0x02 */
#define BTM_EIR_COMPLETE_16BITS_UUID_TYPE   HCI_EIR_COMPLETE_16BITS_UUID_TYPE   /* 0x03 */
#define BTM_EIR_MORE_32BITS_UUID_TYPE       HCI_EIR_MORE_32BITS_UUID_TYPE       /* 0x04 */
#define BTM_EIR_COMPLETE_32BITS_UUID_TYPE   HCI_EIR_COMPLETE_32BITS_UUID_TYPE   /* 0x05 */
#define BTM_EIR_MORE_128BITS_UUID_TYPE      HCI_EIR_MORE_128BITS_UUID_TYPE      /* 0x06 */
#define BTM_EIR_COMPLETE_128BITS_UUID_TYPE  HCI_EIR_COMPLETE_128BITS_UUID_TYPE  /* 0x07 */
#define BTM_EIR_SHORTENED_LOCAL_NAME_TYPE   HCI_EIR_SHORTENED_LOCAL_NAME_TYPE   /* 0x08 */
#define BTM_EIR_COMPLETE_LOCAL_NAME_TYPE    HCI_EIR_COMPLETE_LOCAL_NAME_TYPE    /* 0x09 */
#define BTM_EIR_TX_POWER_LEVEL_TYPE         HCI_EIR_TX_POWER_LEVEL_TYPE         /* 0x0A */
#define BTM_EIR_MANUFACTURER_SPECIFIC_TYPE  HCI_EIR_MANUFACTURER_SPECIFIC_TYPE  /* 0xFF */

/* the following EIR tags are defined to OOB, not regular EIR data */
#define BTM_EIR_OOB_BD_ADDR_TYPE            HCI_EIR_OOB_BD_ADDR_TYPE    /* 6 bytes */
#define BTM_EIR_OOB_COD_TYPE                HCI_EIR_OOB_COD_TYPE        /* 3 bytes */
#define BTM_EIR_OOB_SSP_HASH_C_TYPE         HCI_EIR_OOB_SSP_HASH_C_TYPE /* 16 bytes */
#define BTM_EIR_OOB_SSP_RAND_R_TYPE         HCI_EIR_OOB_SSP_RAND_R_TYPE /* 16 bytes */

#define BTM_OOB_MANDATORY_SIZE      8   /* include 2 bytes length & 6 bytes bd_addr */
#define BTM_OOB_DATA_LEN_SIZE       2
#define BTM_OOB_BD_ADDR_SIZE        6
#define BTM_OOB_COD_SIZE            BT_OOB_COD_SIZE
#define BTM_OOB_HASH_C_SIZE         BT_OOB_HASH_C_SIZE
#define BTM_OOB_RAND_R_SIZE         BT_OOB_RAND_R_SIZE


#if BLE_INCLUDED == TRUE
#define BTM_BLE_SEC_NONE                0
#define BTM_BLE_SEC_ENCRYPT             1 /* encrypt the link using current key */
#define BTM_BLE_SEC_ENCRYPT_NO_MITM     2
#define BTM_BLE_SEC_ENCRYPT_MITM        3
typedef UINT8   tBTM_BLE_SEC_ACT;
#endif
/************************************************************************************************
** BTM Services MACROS handle array of UINT32 bits for more than 32 services
*************************************************************************************************/
/* Determine the number of UINT32's necessary for services */
#define BTM_EIR_ARRAY_BITS          32          /* Number of bits in each array element */
#define BTM_EIR_SERVICE_ARRAY_SIZE  (((UINT32)BTM_EIR_MAX_SERVICES / BTM_EIR_ARRAY_BITS) + \
                                    (((UINT32)BTM_EIR_MAX_SERVICES % BTM_EIR_ARRAY_BITS) ? 1 : 0))

/* MACRO to set the service bit mask in a bit stream */
#define BTM_EIR_SET_SERVICE(p, service)  (((UINT32 *)(p))[(((UINT32)(service)) / BTM_EIR_ARRAY_BITS)] |=  \
                                    ((UINT32)1 << (((UINT32)(service)) % BTM_EIR_ARRAY_BITS)))


/* MACRO to clear the service bit mask in a bit stream */
#define BTM_EIR_CLR_SERVICE(p, service)  (((UINT32 *)(p))[(((UINT32)(service)) / BTM_EIR_ARRAY_BITS)] &=  \
                                    ~((UINT32)1 << (((UINT32)(service)) % BTM_EIR_ARRAY_BITS)))

/* MACRO to check the service bit mask in a bit stream */
#define BTM_EIR_HAS_SERVICE(p, service)  ((((UINT32 *)(p))[(((UINT32)(service)) / BTM_EIR_ARRAY_BITS)] &  \
                                    ((UINT32)1 << (((UINT32)(service)) % BTM_EIR_ARRAY_BITS))) >> (((UINT32)(service)) % BTM_EIR_ARRAY_BITS))

/* start of EIR in HCI buffer, 4 bytes = HCI Command(2) + Length(1) + FEC_Req(1) */
#define BTM_HCI_EIR_OFFSET          (BT_HDR_SIZE + 4)

/***************************
**  Device Discovery Types
****************************/
/* Definitions of the parameters passed to BTM_StartInquiry and
** BTM_SetPeriodicInquiryMode.
*/
typedef struct {            /* contains the two device class condition fields */
    DEV_CLASS       dev_class;
    DEV_CLASS       dev_class_mask;
} tBTM_COD_COND;


typedef union {             /* contains the inquiry filter condition */
    BD_ADDR         bdaddr_cond;
    tBTM_COD_COND   cod_cond;
} tBTM_INQ_FILT_COND;


typedef struct {            /* contains the parameters passed to the inquiry functions */
    UINT8   mode;                       /* general or limited */
    UINT8   duration;                   /* duration of the inquiry (1.28 sec increments) */
    UINT8   max_resps;                  /* maximum number of responses to return */
    BOOLEAN report_dup;                 /* report duplicated inquiry response with higher RSSI value */
    UINT8   filter_cond_type;           /* new devices, BD ADDR, COD, or No filtering */
    tBTM_INQ_FILT_COND  filter_cond;    /* filter value based on filter cond type */
#if (defined(BTA_HOST_INTERLEAVE_SEARCH) && BTA_HOST_INTERLEAVE_SEARCH == TRUE)
    UINT8   intl_duration[4];              /*duration array storing the interleave scan's time portions*/
#endif
} tBTM_INQ_PARMS;

#define BTM_INQ_RESULT_BR       0x01
#define BTM_INQ_RESULT_BLE      0x02

#if (BLE_INCLUDED == TRUE)
#define BTM_BLE_EVT_CONN_ADV        0x00
#define BTM_BLE_EVT_CONN_DIR_ADV    0x01
#define BTM_BLE_EVT_DISC_ADV        0x02
#define BTM_BLE_EVT_NON_CONN_ADV    0x03
#define BTM_BLE_EVT_SCAN_RSP        0x04
typedef UINT8 tBTM_BLE_EVT_TYPE;
#endif

/* These are the fields returned in each device's response to the inquiry.  It
** is returned in the results callback if registered.
*/
typedef struct {
    UINT16      clock_offset;
    BD_ADDR     remote_bd_addr;
    DEV_CLASS   dev_class;
    UINT8       page_scan_rep_mode;
    UINT8       page_scan_per_mode;
    UINT8       page_scan_mode;
    INT8        rssi;       /* Set to BTM_INQ_RES_IGNORE_RSSI if  not valid */
    UINT32      eir_uuid[BTM_EIR_SERVICE_ARRAY_SIZE];
    BOOLEAN     eir_complete_list;
#if (BLE_INCLUDED == TRUE)
    tBT_DEVICE_TYPE         device_type;
    UINT8       inq_result_type;
    UINT8       ble_addr_type;
    tBTM_BLE_EVT_TYPE       ble_evt_type;
    UINT8                   flag;
    UINT8       adv_data_len;
    UINT8       scan_rsp_len;
#endif
} tBTM_INQ_RESULTS;


/* This is the inquiry response information held in its database by BTM, and available
** to applications via BTM_InqDbRead, BTM_InqDbFirst, and BTM_InqDbNext.
*/
typedef struct {
    tBTM_INQ_RESULTS    results;

    BOOLEAN             appl_knows_rem_name;    /* set by application if it knows the remote name of the peer device.
                                                   This is later used by application to determine if remote name request is
                                                   required to be done. Having the flag here avoid duplicate store of inquiry results */
#if ( BLE_INCLUDED == TRUE)
    UINT16          remote_name_len;
    tBTM_BD_NAME    remote_name;
    UINT8           remote_name_state;
    UINT8           remote_name_type;
#endif

} tBTM_INQ_INFO;


/* Structure returned with inquiry complete callback */
typedef struct {
    tBTM_STATUS status;
    UINT8       num_resp;       /* Number of results from the current inquiry */
} tBTM_INQUIRY_CMPL;


/* Structure returned with remote name  request */
typedef struct {
    UINT16      status;
    BD_ADDR     bd_addr;
    UINT16      length;
    BD_NAME     remote_bd_name;
} tBTM_REMOTE_DEV_NAME;

typedef struct {
    UINT8   pcm_intf_rate;  /* PCM interface rate: 0: 128kbps, 1: 256 kbps;
                                2:512 bps; 3: 1024kbps; 4: 2048kbps */
    UINT8   frame_type;     /* frame type: 0: short; 1: long */
    UINT8   sync_mode;      /* sync mode: 0: slave; 1: master */
    UINT8   clock_mode;     /* clock mode: 0: slave; 1: master */

} tBTM_SCO_PCM_PARAM;

/****************************************
**  Device Discovery Callback Functions
*****************************************/
/* Callback function for asynchronous notifications when the BTM inquiry DB
** changes. First param is inquiry database, second is if added to or removed
** from the inquiry database.
*/
typedef void (tBTM_INQ_DB_CHANGE_CB) (void *p1, BOOLEAN is_new);

/* Callback function for notifications when the BTM gets inquiry response.
** First param is inquiry results database, second is pointer of EIR.
*/
typedef void (tBTM_INQ_RESULTS_CB) (tBTM_INQ_RESULTS *p_inq_results, UINT8 *p_eir);

/*****************************************************************************
**  ACL CHANNEL MANAGEMENT
*****************************************************************************/
/******************
**  ACL Constants
*******************/

/* ACL modes */
#define BTM_ACL_MODE_NORMAL     HCI_MODE_ACTIVE
#define BTM_ACL_MODE_HOLD       HCI_MODE_HOLD
#define BTM_ACL_MODE_SNIFF      HCI_MODE_SNIFF
#define BTM_ACL_MODE_PARK       HCI_MODE_PARK

/* Returned with structure in role switch callback (tBTM_ROLE_SWITCH_CMPL) */
#define BTM_ROLE_MASTER         HCI_ROLE_MASTER
#define BTM_ROLE_SLAVE          HCI_ROLE_SLAVE
#define BTM_ROLE_UNDEFINED      0xff     /* undefined value (error status) */

/* ACL Packet Types */
#define BTM_ACL_PKT_TYPES_MASK_DM1      HCI_PKT_TYPES_MASK_DM1
#define BTM_ACL_PKT_TYPES_MASK_DH1      HCI_PKT_TYPES_MASK_DH1
#define BTM_ACL_PKT_TYPES_MASK_DM3      HCI_PKT_TYPES_MASK_DM3
#define BTM_ACL_PKT_TYPES_MASK_DH3      HCI_PKT_TYPES_MASK_DH3
#define BTM_ACL_PKT_TYPES_MASK_DM5      HCI_PKT_TYPES_MASK_DM5
#define BTM_ACL_PKT_TYPES_MASK_DH5      HCI_PKT_TYPES_MASK_DH5
#define BTM_ACL_PKT_TYPES_MASK_NO_2_DH1 HCI_PKT_TYPES_MASK_NO_2_DH1
#define BTM_ACL_PKT_TYPES_MASK_NO_3_DH1 HCI_PKT_TYPES_MASK_NO_3_DH1
#define BTM_ACL_PKT_TYPES_MASK_NO_2_DH3 HCI_PKT_TYPES_MASK_NO_2_DH3
#define BTM_ACL_PKT_TYPES_MASK_NO_3_DH3 HCI_PKT_TYPES_MASK_NO_3_DH3
#define BTM_ACL_PKT_TYPES_MASK_NO_2_DH5 HCI_PKT_TYPES_MASK_NO_2_DH5
#define BTM_ACL_PKT_TYPES_MASK_NO_3_DH5 HCI_PKT_TYPES_MASK_NO_3_DH5

/***************
**  ACL Types
****************/

/* Structure returned with Role Switch information (in tBTM_CMPL_CB callback function)
** in response to BTM_SwitchRole call.
*/
typedef struct {
    UINT8   hci_status;     /* HCI status returned with the event */
    UINT8   role;           /* BTM_ROLE_MASTER or BTM_ROLE_SLAVE */
    BD_ADDR remote_bd_addr; /* Remote BD addr involved with the switch */
} tBTM_ROLE_SWITCH_CMPL;

/* Structure returned with QoS information (in tBTM_CMPL_CB callback function)
** in response to BTM_SetQoS call.
*/
typedef struct {
    FLOW_SPEC flow;
    UINT16 handle;
    UINT8 status;
} tBTM_QOS_SETUP_CMPL;


/* Structure returned with read RSSI event (in tBTM_CMPL_CB callback function)
** in response to BTM_ReadRSSI call.
*/
typedef struct {
    tBTM_STATUS status;
    UINT8       hci_status;
    INT8        rssi;
    BD_ADDR     rem_bda;
} tBTM_RSSI_RESULTS;

/* Structure returned with read current TX power event (in tBTM_CMPL_CB callback function)
** in response to BTM_ReadTxPower call.
*/
typedef struct {
    tBTM_STATUS status;
    UINT8       hci_status;
    INT8        tx_power;
    BD_ADDR     rem_bda;
} tBTM_TX_POWER_RESULTS;

/* Structure returned with read link quality event (in tBTM_CMPL_CB callback function)
** in response to BTM_ReadLinkQuality call.
*/
typedef struct {
    tBTM_STATUS status;
    UINT8       hci_status;
    UINT8       link_quality;
    BD_ADDR     rem_bda;
} tBTM_LINK_QUALITY_RESULTS;

/* Structure returned with read inq tx power quality event (in tBTM_CMPL_CB callback function)
** in response to BTM_ReadInquiryRspTxPower call.
*/
typedef struct {
    tBTM_STATUS status;
    UINT8       hci_status;
    INT8        tx_power;
} tBTM_INQ_TXPWR_RESULTS;

enum {
    BTM_BL_CONN_EVT,
    BTM_BL_DISCN_EVT,
    BTM_BL_UPDATE_EVT,
    BTM_BL_ROLE_CHG_EVT,
    BTM_BL_COLLISION_EVT
};
typedef UINT8 tBTM_BL_EVENT;
typedef UINT16 tBTM_BL_EVENT_MASK;

#define BTM_BL_CONN_MASK        0x0001
#define BTM_BL_DISCN_MASK       0x0002
#define BTM_BL_UPDATE_MASK      0x0004
#define BTM_BL_ROLE_CHG_MASK    0x0008

/* Device features mask definitions */
#define BTM_FEATURE_BYTES_PER_PAGE  HCI_FEATURE_BYTES_PER_PAGE
#define BTM_EXT_FEATURES_PAGE_MAX   HCI_EXT_FEATURES_PAGE_MAX

/* the data type associated with BTM_BL_CONN_EVT */
typedef struct {
    tBTM_BL_EVENT   event;      /* The event reported. */
    BD_ADDR_PTR     p_bda;      /* The address of the newly connected device */
    DEV_CLASS_PTR   p_dc;       /* The device class */
    BD_NAME_PTR     p_bdn;      /* The device name */
    UINT8          *p_features; /* pointer to the remote device's features page[0] (supported features page) */
#if BLE_INCLUDED == TRUE
    UINT16          handle;     /* connection handle */
    tBT_TRANSPORT   transport; /* link is LE or not */
#endif
} tBTM_BL_CONN_DATA;

/* the data type associated with BTM_BL_DISCN_EVT */
typedef struct {
    tBTM_BL_EVENT   event;  /* The event reported. */
    BD_ADDR_PTR     p_bda;  /* The address of the disconnected device */
#if BLE_INCLUDED == TRUE
    UINT16          handle; /* disconnected connection handle */
    tBT_TRANSPORT   transport; /* link is LE link or not */
#endif
} tBTM_BL_DISCN_DATA;

/* Busy-Level shall have the inquiry_paging mask set when
 * inquiry/paging is in progress, Else the number of ACL links */
#define BTM_BL_INQUIRY_PAGING_MASK 0x10
#define BTM_BL_INQUIRY_STARTED   (BTM_BL_INQUIRY_PAGING_MASK | 0x1)
#define BTM_BL_INQUIRY_CANCELLED (BTM_BL_INQUIRY_PAGING_MASK | 0x2)
#define BTM_BL_INQUIRY_COMPLETE  (BTM_BL_INQUIRY_PAGING_MASK | 0x3)
#define BTM_BL_PAGING_STARTED    (BTM_BL_INQUIRY_PAGING_MASK | 0x4)
#define BTM_BL_PAGING_COMPLETE   (BTM_BL_INQUIRY_PAGING_MASK | 0x5)
/* the data type associated with BTM_BL_UPDATE_EVT */
typedef struct {
    tBTM_BL_EVENT   event;  /* The event reported. */
    UINT8           busy_level;/* when paging or inquiring, level is 10.
                                * Otherwise, the number of ACL links. */
    UINT8           busy_level_flags; /* Notifies actual inquiry/page activities */
} tBTM_BL_UPDATE_DATA;

/* the data type associated with BTM_BL_ROLE_CHG_EVT */
typedef struct {
    tBTM_BL_EVENT   event;      /* The event reported. */
    BD_ADDR_PTR     p_bda;      /* The address of the peer connected device */
    UINT8           new_role;
    UINT8           hci_status; /* HCI status returned with the event */
} tBTM_BL_ROLE_CHG_DATA;

typedef union {
    tBTM_BL_EVENT           event;  /* The event reported. */
    tBTM_BL_CONN_DATA       conn;   /* The data associated with BTM_BL_CONN_EVT */
    tBTM_BL_DISCN_DATA      discn;  /* The data associated with BTM_BL_DISCN_EVT */
    tBTM_BL_UPDATE_DATA     update; /* The data associated with BTM_BL_UPDATE_EVT */
    tBTM_BL_ROLE_CHG_DATA   role_chg;/*The data associated with BTM_BL_ROLE_CHG_EVT */
} tBTM_BL_EVENT_DATA;

/* Callback function for notifications when the BTM busy level
** changes.
*/
typedef void (tBTM_BL_CHANGE_CB) (tBTM_BL_EVENT_DATA *p_data);

/***************************
**  ACL Callback Functions
****************************/
/* Callback function for notifications when the BTM ACL connection DB
** changes. First param is BD address, second is if added or removed.
** Registered through BTM_AclRegisterForChanges call.
*/
#if BLE_INCLUDED == TRUE
typedef void (tBTM_ACL_DB_CHANGE_CB) (BD_ADDR p_bda, DEV_CLASS p_dc,
                                      BD_NAME p_bdn, UINT8 *features,
                                      BOOLEAN is_new, UINT16 handle,
                                      tBT_TRANSPORT transport);
#else
typedef void (tBTM_ACL_DB_CHANGE_CB) (BD_ADDR p_bda, DEV_CLASS p_dc,
                                      BD_NAME p_bdn, UINT8 *features,
                                      BOOLEAN is_new);
#endif
/*****************************************************************************
**  SCO CHANNEL MANAGEMENT
*****************************************************************************/
/******************
**  SCO Constants
*******************/

/* Define an invalid SCO index and an invalid HCI handle */
#define BTM_INVALID_SCO_INDEX       0xFFFF
#define BTM_INVALID_HCI_HANDLE      0xFFFF

/* Define an invalid SCO disconnect reason */
#define BTM_INVALID_SCO_DISC_REASON 0xFFFF

/* Define first active SCO index */
#define BTM_FIRST_ACTIVE_SCO_INDEX  BTM_MAX_SCO_LINKS

/* Define SCO packet types used in APIs */
#define BTM_SCO_PKT_TYPES_MASK_HV1  HCI_ESCO_PKT_TYPES_MASK_HV1
#define BTM_SCO_PKT_TYPES_MASK_HV2  HCI_ESCO_PKT_TYPES_MASK_HV2
#define BTM_SCO_PKT_TYPES_MASK_HV3  HCI_ESCO_PKT_TYPES_MASK_HV3
#define BTM_SCO_PKT_TYPES_MASK_EV3  HCI_ESCO_PKT_TYPES_MASK_EV3
#define BTM_SCO_PKT_TYPES_MASK_EV4  HCI_ESCO_PKT_TYPES_MASK_EV4
#define BTM_SCO_PKT_TYPES_MASK_EV5  HCI_ESCO_PKT_TYPES_MASK_EV5
#define BTM_SCO_PKT_TYPES_MASK_NO_2_EV3  HCI_ESCO_PKT_TYPES_MASK_NO_2_EV3
#define BTM_SCO_PKT_TYPES_MASK_NO_3_EV3  HCI_ESCO_PKT_TYPES_MASK_NO_3_EV3
#define BTM_SCO_PKT_TYPES_MASK_NO_2_EV5  HCI_ESCO_PKT_TYPES_MASK_NO_2_EV5
#define BTM_SCO_PKT_TYPES_MASK_NO_3_EV5  HCI_ESCO_PKT_TYPES_MASK_NO_3_EV5

#define BTM_SCO_LINK_ONLY_MASK  (BTM_SCO_PKT_TYPES_MASK_HV1 | \
                                 BTM_SCO_PKT_TYPES_MASK_HV2 | \
                                 BTM_SCO_PKT_TYPES_MASK_HV3)

#define BTM_ESCO_LINK_ONLY_MASK (BTM_SCO_PKT_TYPES_MASK_EV3 | \
                                 BTM_SCO_PKT_TYPES_MASK_EV4 | \
                                 BTM_SCO_PKT_TYPES_MASK_EV5)

#define BTM_SCO_LINK_ALL_PKT_MASK   (BTM_SCO_LINK_ONLY_MASK     | \
                                     BTM_ESCO_LINK_ONLY_MASK)

#define BTM_VALID_SCO_ALL_PKT_TYPE HCI_VALID_SCO_ALL_PKT_TYPE

/* Passed in BTM_CreateSco if the packet type parameter should be ignored */
#define BTM_IGNORE_SCO_PKT_TYPE     0

/***************
**  SCO Types
****************/
#define BTM_LINK_TYPE_SCO           HCI_LINK_TYPE_SCO
#define BTM_LINK_TYPE_ESCO          HCI_LINK_TYPE_ESCO
typedef UINT8 tBTM_SCO_TYPE;


/*******************
** SCO Routing Path
********************/
#define BTM_SCO_ROUTE_PCM           HCI_BRCM_SCO_ROUTE_PCM
#define BTM_SCO_ROUTE_HCI           HCI_BRCM_SCO_ROUTE_HCI
typedef UINT8 tBTM_SCO_ROUTE_TYPE;


/*******************
** SCO Codec Types
********************/
// TODO(google) This should use common definitions
// in hci/include/hci_audio.h
#define BTM_SCO_CODEC_NONE          0x0000
#define BTM_SCO_CODEC_CVSD          0x0001
#define BTM_SCO_CODEC_MSBC          0x0002
typedef UINT16 tBTM_SCO_CODEC_TYPE;



/*******************
** SCO Air Mode Types
********************/
#define BTM_SCO_AIR_MODE_U_LAW          0
#define BTM_SCO_AIR_MODE_A_LAW          1
#define BTM_SCO_AIR_MODE_CVSD           2
#define BTM_SCO_AIR_MODE_TRANSPNT       3
typedef UINT8 tBTM_SCO_AIR_MODE_TYPE;

/*******************
** SCO Voice Settings
********************/
#define BTM_VOICE_SETTING_CVSD  ((UINT16)  (HCI_INP_CODING_LINEAR          |   \
                                            HCI_INP_DATA_FMT_2S_COMPLEMENT |   \
                                            HCI_INP_SAMPLE_SIZE_16BIT      |   \
                                            HCI_AIR_CODING_FORMAT_CVSD))

#define BTM_VOICE_SETTING_TRANS ((UINT16)  (HCI_INP_CODING_LINEAR          |   \
                                            HCI_INP_DATA_FMT_2S_COMPLEMENT |   \
                                            HCI_INP_SAMPLE_SIZE_16BIT      |   \
                                            HCI_AIR_CODING_FORMAT_TRANSPNT))

/*******************
** SCO Data Status
********************/
enum {
    BTM_SCO_DATA_CORRECT,
    BTM_SCO_DATA_PAR_ERR,
    BTM_SCO_DATA_NONE,
    BTM_SCO_DATA_PAR_LOST
};
typedef UINT8 tBTM_SCO_DATA_FLAG;

/***************************
**  SCO Callback Functions
****************************/
typedef void (tBTM_SCO_CB) (UINT16 sco_inx);
typedef void (tBTM_SCO_DATA_CB) (UINT16 sco_inx, BT_HDR *p_data, tBTM_SCO_DATA_FLAG status);

/******************
**  eSCO Constants
*******************/
#define BTM_64KBITS_RATE            0x00001f40  /* 64 kbits/sec data rate */

/* Retransmission effort */
#define BTM_ESCO_RETRANS_OFF        0
#define BTM_ESCO_RETRANS_POWER      1
#define BTM_ESCO_RETRANS_QUALITY    2
#define BTM_ESCO_RETRANS_DONTCARE   0xff

/* Max Latency Don't Care */
#define BTM_ESCO_MAX_LAT_DONTCARE   0xffff

/***************
**  eSCO Types
****************/
/* tBTM_ESCO_CBACK event types */
#define BTM_ESCO_CHG_EVT        1
#define BTM_ESCO_CONN_REQ_EVT   2
typedef UINT8 tBTM_ESCO_EVT;

/* Passed into BTM_SetEScoMode() */
typedef struct {
    UINT32 tx_bw;
    UINT32 rx_bw;
    UINT16 max_latency;
    UINT16 voice_contfmt;  /* Voice Settings or Content Format */
    UINT16 packet_types;
    UINT8  retrans_effort;
} tBTM_ESCO_PARAMS;

typedef struct {
    UINT16 max_latency;
    UINT16 packet_types;
    UINT8  retrans_effort;
} tBTM_CHG_ESCO_PARAMS;

/* Returned by BTM_ReadEScoLinkParms() */
typedef struct {
    UINT16  rx_pkt_len;
    UINT16  tx_pkt_len;
    BD_ADDR bd_addr;
    UINT8   link_type;  /* BTM_LINK_TYPE_SCO or BTM_LINK_TYPE_ESCO */
    UINT8   tx_interval;
    UINT8   retrans_window;
    UINT8   air_mode;
} tBTM_ESCO_DATA;

typedef struct {
    UINT16  sco_inx;
    UINT16  rx_pkt_len;
    UINT16  tx_pkt_len;
    BD_ADDR bd_addr;
    UINT8   hci_status;
    UINT8   tx_interval;
    UINT8   retrans_window;
} tBTM_CHG_ESCO_EVT_DATA;

typedef struct {
    UINT16        sco_inx;
    BD_ADDR       bd_addr;
    DEV_CLASS     dev_class;
    tBTM_SCO_TYPE link_type;
} tBTM_ESCO_CONN_REQ_EVT_DATA;

typedef union {
    tBTM_CHG_ESCO_EVT_DATA      chg_evt;
    tBTM_ESCO_CONN_REQ_EVT_DATA conn_evt;
} tBTM_ESCO_EVT_DATA;

/***************************
**  eSCO Callback Functions
****************************/
typedef void (tBTM_ESCO_CBACK) (tBTM_ESCO_EVT event, tBTM_ESCO_EVT_DATA *p_data);


/*****************************************************************************
**  SECURITY MANAGEMENT
*****************************************************************************/
/*******************************
**  Security Manager Constants
********************************/

/* Security Mode (BTM_SetSecurityMode) */
#define BTM_SEC_MODE_UNDEFINED      0
#define BTM_SEC_MODE_NONE           1
#define BTM_SEC_MODE_SERVICE        2
#define BTM_SEC_MODE_LINK           3
#define BTM_SEC_MODE_SP             4
#define BTM_SEC_MODE_SP_DEBUG       5
#define BTM_SEC_MODE_SC             6

/* Maximum Number of BTM Security Modes */
#define BTM_SEC_MODES_MAX           7

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

/* Security Flags [bit mask] (BTM_GetSecurityFlags)
*/
#define BTM_SEC_FLAG_AUTHORIZED     0x01
#define BTM_SEC_FLAG_AUTHENTICATED  0x02
#define BTM_SEC_FLAG_ENCRYPTED      0x04
#define BTM_SEC_FLAG_LKEY_KNOWN     0x10
#define BTM_SEC_FLAG_LKEY_AUTHED    0x20

/* PIN types */
#define BTM_PIN_TYPE_VARIABLE       HCI_PIN_TYPE_VARIABLE
#define BTM_PIN_TYPE_FIXED          HCI_PIN_TYPE_FIXED

/* Link Key types used to generate the new link key.
** returned in link key notification callback function
*/
#define BTM_LKEY_TYPE_COMBINATION   HCI_LKEY_TYPE_COMBINATION
#define BTM_LKEY_TYPE_LOCAL_UNIT    HCI_LKEY_TYPE_LOCAL_UNIT
#define BTM_LKEY_TYPE_REMOTE_UNIT   HCI_LKEY_TYPE_REMOTE_UNIT
#define BTM_LKEY_TYPE_DEBUG_COMB    HCI_LKEY_TYPE_DEBUG_COMB
#define BTM_LKEY_TYPE_UNAUTH_COMB   HCI_LKEY_TYPE_UNAUTH_COMB
#define BTM_LKEY_TYPE_AUTH_COMB     HCI_LKEY_TYPE_AUTH_COMB
#define BTM_LKEY_TYPE_CHANGED_COMB  HCI_LKEY_TYPE_CHANGED_COMB

#define BTM_LKEY_TYPE_UNAUTH_COMB_P_256 HCI_LKEY_TYPE_UNAUTH_COMB_P_256
#define BTM_LKEY_TYPE_AUTH_COMB_P_256   HCI_LKEY_TYPE_AUTH_COMB_P_256

#define BTM_LTK_DERIVED_LKEY_OFFSET 0x20    /* "easy" requirements for LK derived from LTK */
#define BTM_LKEY_TYPE_IGNORE        0xff    /* used when event is response from
                                               hci return link keys request */

typedef UINT8 tBTM_LINK_KEY_TYPE;

/* Protocol level security (BTM_SetSecurityLevel) */
#define BTM_SEC_PROTO_L2CAP         0
#define BTM_SEC_PROTO_SDP           1
#define BTM_SEC_PROTO_TCS           2
#define BTM_SEC_PROTO_RFCOMM        3
#define BTM_SEC_PROTO_OBEX          4
#define BTM_SEC_PROTO_BNEP          5
#define BTM_SEC_PROTO_HID           6   /* HID      */
#define BTM_SEC_PROTO_AVDT          7
#define BTM_SEC_PROTO_MCA           8

/* Determine the number of UINT32's necessary for security services */
#define BTM_SEC_ARRAY_BITS          32          /* Number of bits in each array element */
#define BTM_SEC_SERVICE_ARRAY_SIZE  (((UINT32)BTM_SEC_MAX_SERVICES / BTM_SEC_ARRAY_BITS) + \
                                    (((UINT32)BTM_SEC_MAX_SERVICES % BTM_SEC_ARRAY_BITS) ? 1 : 0))

/* Security service definitions (BTM_SetSecurityLevel)
** Used for Authorization APIs
*/
#define BTM_SEC_SERVICE_SDP_SERVER      0
#define BTM_SEC_SERVICE_SERIAL_PORT     1
#define BTM_SEC_SERVICE_LAN_ACCESS      2
#define BTM_SEC_SERVICE_DUN             3
#define BTM_SEC_SERVICE_IRMC_SYNC       4
#define BTM_SEC_SERVICE_IRMC_SYNC_CMD   5
#define BTM_SEC_SERVICE_OBEX            6
#define BTM_SEC_SERVICE_OBEX_FTP        7
#define BTM_SEC_SERVICE_HEADSET         8
#define BTM_SEC_SERVICE_CORDLESS        9
#define BTM_SEC_SERVICE_INTERCOM        10
#define BTM_SEC_SERVICE_FAX             11
#define BTM_SEC_SERVICE_HEADSET_AG      12
#define BTM_SEC_SERVICE_PNP_INFO        13
#define BTM_SEC_SERVICE_GEN_NET         14
#define BTM_SEC_SERVICE_GEN_FILE        15
#define BTM_SEC_SERVICE_GEN_AUDIO       16
#define BTM_SEC_SERVICE_GEN_TEL         17
#define BTM_SEC_SERVICE_CTP_DATA        18
#define BTM_SEC_SERVICE_HCRP_CTRL       19
#define BTM_SEC_SERVICE_HCRP_DATA       20
#define BTM_SEC_SERVICE_HCRP_NOTIF      21
#define BTM_SEC_SERVICE_BPP_JOB         22
#define BTM_SEC_SERVICE_BPP_STATUS      23
#define BTM_SEC_SERVICE_BPP_REF         24
#define BTM_SEC_SERVICE_BNEP_PANU       25
#define BTM_SEC_SERVICE_BNEP_GN         26
#define BTM_SEC_SERVICE_BNEP_NAP        27
#define BTM_SEC_SERVICE_HF_HANDSFREE    28
#define BTM_SEC_SERVICE_AG_HANDSFREE    29
#define BTM_SEC_SERVICE_TE_PHONE_ACCESS 30
#define BTM_SEC_SERVICE_ME_PHONE_ACCESS 31

#define BTM_SEC_SERVICE_HIDH_SEC_CTRL   32
#define BTM_SEC_SERVICE_HIDH_NOSEC_CTRL 33
#define BTM_SEC_SERVICE_HIDH_INTR       34
#define BTM_SEC_SERVICE_BIP             35
#define BTM_SEC_SERVICE_BIP_REF         36
#define BTM_SEC_SERVICE_AVDTP           37
#define BTM_SEC_SERVICE_AVDTP_NOSEC     38
#define BTM_SEC_SERVICE_AVCTP           39
#define BTM_SEC_SERVICE_SAP             40
#define BTM_SEC_SERVICE_PBAP            41
#define BTM_SEC_SERVICE_RFC_MUX         42
#define BTM_SEC_SERVICE_AVCTP_BROWSE    43
#define BTM_SEC_SERVICE_MAP             44
#define BTM_SEC_SERVICE_MAP_NOTIF       45
#define BTM_SEC_SERVICE_MCAP_CTRL       46
#define BTM_SEC_SERVICE_MCAP_DATA       47
#define BTM_SEC_SERVICE_HDP_SNK         48
#define BTM_SEC_SERVICE_HDP_SRC         49
#define BTM_SEC_SERVICE_ATT             50

/* Update these as services are added */
#define BTM_SEC_SERVICE_FIRST_EMPTY     51

#ifndef BTM_SEC_MAX_SERVICES
#define BTM_SEC_MAX_SERVICES            65
#endif

/************************************************************************************************
** Security Services MACROS handle array of UINT32 bits for more than 32 trusted services
*************************************************************************************************/
/* MACRO to set the security service bit mask in a bit stream */
#define BTM_SEC_SET_SERVICE(p, service)  (((UINT32 *)(p))[(((UINT32)(service)) / BTM_SEC_ARRAY_BITS)] |=  \
                                    ((UINT32)1 << (((UINT32)(service)) % BTM_SEC_ARRAY_BITS)))


/* MACRO to clear the security service bit mask in a bit stream */
#define BTM_SEC_CLR_SERVICE(p, service)  (((UINT32 *)(p))[(((UINT32)(service)) / BTM_SEC_ARRAY_BITS)] &=  \
                                    ~((UINT32)1 << (((UINT32)(service)) % BTM_SEC_ARRAY_BITS)))

/* MACRO to check the security service bit mask in a bit stream (Returns TRUE or FALSE) */
#define BTM_SEC_IS_SERVICE_TRUSTED(p, service)    (((((UINT32 *)(p))[(((UINT32)(service)) / BTM_SEC_ARRAY_BITS)]) &   \
                                        (UINT32)(((UINT32)1 << (((UINT32)(service)) % BTM_SEC_ARRAY_BITS)))) ? TRUE : FALSE)

/* MACRO to copy two trusted device bitmask */
#define BTM_SEC_COPY_TRUSTED_DEVICE(p_src, p_dst)   {UINT32 trst; for (trst = 0; trst < BTM_SEC_SERVICE_ARRAY_SIZE; trst++) \
                                                        ((UINT32 *)(p_dst))[trst] = ((UINT32 *)(p_src))[trst];}

/* MACRO to clear two trusted device bitmask */
#define BTM_SEC_CLR_TRUSTED_DEVICE(p_dst)   {UINT32 trst; for (trst = 0; trst < BTM_SEC_SERVICE_ARRAY_SIZE; trst++) \
                                                        ((UINT32 *)(p_dst))[trst] = 0;}

/* Following bits can be provided by host in the trusted_mask array */
/* 0..31 bits of mask[0] (Least Significant Word) */
#define BTM_SEC_TRUST_SDP_SERVER        (1 << BTM_SEC_SERVICE_SDP_SERVER)
#define BTM_SEC_TRUST_SERIAL_PORT       (1 << BTM_SEC_SERVICE_SERIAL_PORT)
#define BTM_SEC_TRUST_LAN_ACCESS        (1 << BTM_SEC_SERVICE_LAN_ACCESS)
#define BTM_SEC_TRUST_DUN               (1 << BTM_SEC_SERVICE_DUN)
#define BTM_SEC_TRUST_IRMC_SYNC         (1 << BTM_SEC_SERVICE_IRMC_SYNC)
#define BTM_SEC_TRUST_IRMC_SYNC_CMD     (1 << BTM_SEC_SERVICE_IRMC_SYNC_CMD)
#define BTM_SEC_TRUST_OBEX              (1 << BTM_SEC_SERVICE_OBEX)
#define BTM_SEC_TRUST_OBEX_FTP          (1 << BTM_SEC_SERVICE_OBEX_FTP)
#define BTM_SEC_TRUST_HEADSET           (1 << BTM_SEC_SERVICE_HEADSET)
#define BTM_SEC_TRUST_CORDLESS          (1 << BTM_SEC_SERVICE_CORDLESS)
#define BTM_SEC_TRUST_INTERCOM          (1 << BTM_SEC_SERVICE_INTERCOM)
#define BTM_SEC_TRUST_FAX               (1 << BTM_SEC_SERVICE_FAX)
#define BTM_SEC_TRUST_HEADSET_AG        (1 << BTM_SEC_SERVICE_HEADSET_AG)
#define BTM_SEC_TRUST_PNP_INFO          (1 << BTM_SEC_SERVICE_PNP_INFO)
#define BTM_SEC_TRUST_GEN_NET           (1 << BTM_SEC_SERVICE_GEN_NET)
#define BTM_SEC_TRUST_GEN_FILE          (1 << BTM_SEC_SERVICE_GEN_FILE)
#define BTM_SEC_TRUST_GEN_AUDIO         (1 << BTM_SEC_SERVICE_GEN_AUDIO)
#define BTM_SEC_TRUST_GEN_TEL           (1 << BTM_SEC_SERVICE_GEN_TEL)
#define BTM_SEC_TRUST_CTP_DATA          (1 << BTM_SEC_SERVICE_CTP_DATA)
#define BTM_SEC_TRUST_HCRP_CTRL         (1 << BTM_SEC_SERVICE_HCRP_CTRL)
#define BTM_SEC_TRUST_HCRP_DATA         (1 << BTM_SEC_SERVICE_HCRP_DATA)
#define BTM_SEC_TRUST_HCRP_NOTIF        (1 << BTM_SEC_SERVICE_HCRP_NOTIF)
#define BTM_SEC_TRUST_BPP_JOB           (1 << BTM_SEC_SERVICE_JOB)
#define BTM_SEC_TRUST_BPP_STATUS        (1 << BTM_SEC_SERVICE_STATUS)
#define BTM_SEC_TRUST_BPP_REF           (1 << BTM_SEC_SERVICE_REF)
#define BTM_SEC_TRUST_BNEP_PANU         (1 << BTM_SEC_SERVICE_BNEP_PANU)
#define BTM_SEC_TRUST_BNEP_GN           (1 << BTM_SEC_SERVICE_BNEP_GN)
#define BTM_SEC_TRUST_BNEP_NAP          (1 << BTM_SEC_SERVICE_BNEP_NAP)
#define BTM_SEC_TRUST_HFP_HF            (1 << BTM_SEC_SERVICE_HF_HANDSFREE)
#define BTM_SEC_TRUST_HFP_AG            (1 << BTM_SEC_SERVICE_AG_HANDSFREE)
#define BTM_SEC_TRUST_TE_PHONE_ACCESS   (1 << BTM_SEC_SERVICE_TE_PHONE_ACCESS)
#define BTM_SEC_TRUST_ME_PHONE_ACCESS   (1 << BTM_SEC_SERVICE_ME_PHONE_ACCESS)

/* 0..31 bits of mask[1] (Most Significant Word) */
#define BTM_SEC_TRUST_HIDH_CTRL         (1 << (BTM_SEC_SERVICE_HIDH_SEC_CTRL - 32))
#define BTM_SEC_TRUST_HIDH_NOSEC_CTRL   (1 << (BTM_SEC_SERVICE_HIDH_NOSEC_CTRL - 32))
#define BTM_SEC_TRUST_HIDH_INTR         (1 << (BTM_SEC_SERVICE_HIDH_INTR - 32))
#define BTM_SEC_TRUST_BIP               (1 << (BTM_SEC_SERVICE_BIP - 32))
#define BTM_SEC_TRUST_BIP_REF           (1 << (BTM_SEC_SERVICE_BIP_REF - 32))
#define BTM_SEC_TRUST_AVDTP             (1 << (BTM_SEC_SERVICE_AVDTP - 32))
#define BTM_SEC_TRUST_AVDTP_NOSEC       (1 << (BTM_SEC_SERVICE_AVDTP_NOSEC - 32))
#define BTM_SEC_TRUST_AVCTP             (1 << (BTM_SEC_SERVICE_AVCTP - 32))
#define BTM_SEC_TRUST_SAP               (1 << (BTM_SEC_SERVICE_SAP - 32))
#define BTM_SEC_TRUST_PBAP              (1 << (BTM_SEC_SERVICE_PBAP - 32))
#define BTM_SEC_TRUST_RFC_MUX           (1 << (BTM_SEC_SERVICE_RFC_MUX - 32))
#define BTM_SEC_TRUST_AVCTP_BROWSE      (1 << (BTM_SEC_SERVICE_AVCTP_BROWSE - 32))
#define BTM_SEC_TRUST_MAP               (1 << (BTM_SEC_SERVICE_MAP - 32))
#define BTM_SEC_TRUST_MAP_NOTIF         (1 << (BTM_SEC_SERVICE_MAP_NOTIF - 32))
#define BTM_SEC_TRUST_MCAP_CTRL         (1 << (BTM_SEC_SERVICE_MCAP_CTRL - 32))
#define BTM_SEC_TRUST_MCAP_DATA         (1 << (BTM_SEC_SERVICE_MCAP_DATA - 32))
#define BTM_SEC_TRUST_HDP_SNK           (1 << (BTM_SEC_SERVICE_HDP_SNK - 32))
#define BTM_SEC_TRUST_HDP_SRC           (1 << (BTM_SEC_SERVICE_HDP_SRC - 32))

#define BTM_SEC_TRUST_ALL               0xFFFFFFFF  /* for each array element */

/****************************************
**  Security Manager Callback Functions
*****************************************/
/* Authorize device for service.  Parameters are
**              BD Address of remote
**              Device Class of remote
**              BD Name of remote
**              Service name
**              Service Id (NULL - unknown service or unused
**                                 [BTM_SEC_SERVICE_NAME_LEN set to 0])
**              Is originator of the connection
**              Result of the operation
*/
typedef UINT8 (tBTM_AUTHORIZE_CALLBACK) (BD_ADDR bd_addr, DEV_CLASS dev_class,
        tBTM_BD_NAME bd_name, UINT8 *service_name,
        UINT8 service_id, BOOLEAN is_originator);

/* Get PIN for the connection.  Parameters are
**              BD Address of remote
**              Device Class of remote
**              BD Name of remote
**              Flag indicating the minimum pin code length to be 16 digits
*/
typedef UINT8 (tBTM_PIN_CALLBACK) (BD_ADDR bd_addr, DEV_CLASS dev_class,
                                   tBTM_BD_NAME bd_name, BOOLEAN min_16_digit);

/* New Link Key for the connection.  Parameters are
**              BD Address of remote
**              Link Key
**              Key Type: Combination, Local Unit, or Remote Unit
*/
typedef UINT8 (tBTM_LINK_KEY_CALLBACK) (BD_ADDR bd_addr, DEV_CLASS dev_class,
                                        tBTM_BD_NAME bd_name, UINT8 *key,
                                        UINT8 key_type);


/* Remote Name Resolved.  Parameters are
**              BD Address of remote
**              BD Name of remote
*/
typedef void (tBTM_RMT_NAME_CALLBACK) (BD_ADDR bd_addr, DEV_CLASS dc,
                                       tBTM_BD_NAME bd_name);


/* Authentication complete for the connection.  Parameters are
**              BD Address of remote
**              Device Class of remote
**              BD Name of remote
**
*/
typedef UINT8 (tBTM_AUTH_COMPLETE_CALLBACK) (BD_ADDR bd_addr, DEV_CLASS dev_class,
        tBTM_BD_NAME bd_name, int result);

enum {
    BTM_SP_IO_REQ_EVT,      /* received IO_CAPABILITY_REQUEST event */
    BTM_SP_IO_RSP_EVT,      /* received IO_CAPABILITY_RESPONSE event */
    BTM_SP_CFM_REQ_EVT,     /* received USER_CONFIRMATION_REQUEST event */
    BTM_SP_KEY_NOTIF_EVT,   /* received USER_PASSKEY_NOTIFY event */
    BTM_SP_KEY_REQ_EVT,     /* received USER_PASSKEY_REQUEST event */
    BTM_SP_KEYPRESS_EVT,    /* received KEYPRESS_NOTIFY event */
    BTM_SP_LOC_OOB_EVT,     /* received result for READ_LOCAL_OOB_DATA command */
    BTM_SP_RMT_OOB_EVT,     /* received REMOTE_OOB_DATA_REQUEST event */
    BTM_SP_COMPLT_EVT,      /* received SIMPLE_PAIRING_COMPLETE event */
    BTM_SP_UPGRADE_EVT      /* check if the application wants to upgrade the link key */
};
typedef UINT8 tBTM_SP_EVT;

/* relate to ESP_IO_CAP_xxx in esp_gap_ble_api.h */
#define BTM_IO_CAP_OUT      0   /* DisplayOnly */
#define BTM_IO_CAP_IO       1   /* DisplayYesNo */
#define BTM_IO_CAP_IN       2   /* KeyboardOnly */
#define BTM_IO_CAP_NONE     3   /* NoInputNoOutput */
#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
#define BTM_IO_CAP_KBDISP   4   /* Keyboard display */
#define BTM_IO_CAP_MAX      5
#else
#define BTM_IO_CAP_MAX      4
#endif

typedef UINT8 tBTM_IO_CAP;

#define BTM_MAX_PASSKEY_VAL (999999)
#define BTM_MIN_PASSKEY_VAL (0)

#define BTM_AUTH_SP_NO      0   /* MITM Protection Not Required - Single Profile/non-bonding
                                Numeric comparison with automatic accept allowed */
#define BTM_AUTH_SP_YES     1   /* MITM Protection Required - Single Profile/non-bonding
                                Use IO Capabilities to determine authentication procedure */
#define BTM_AUTH_AP_NO      2   /* MITM Protection Not Required - All Profiles/dedicated bonding
                                Numeric comparison with automatic accept allowed */
#define BTM_AUTH_AP_YES     3   /* MITM Protection Required - All Profiles/dedicated bonding
                                Use IO Capabilities to determine authentication procedure */
#define BTM_AUTH_SPGB_NO    4   /* MITM Protection Not Required - Single Profiles/general bonding
                                Numeric comparison with automatic accept allowed */
#define BTM_AUTH_SPGB_YES   5   /* MITM Protection Required - Single Profiles/general bonding
                                Use IO Capabilities to determine authentication procedure */
#define BTM_AUTH_DD_BOND    2   /* this bit is ORed to the BTM_AUTH_SP_* when IO exchange for dedicated bonding */
#define BTM_AUTH_GB_BIT     4   /* the genernal bonding bit */
#define BTM_AUTH_BONDS      6   /* the general/dedicated bonding bits  */
#define BTM_AUTH_YN_BIT     1   /* this is the Yes or No bit  */

#define BTM_BLE_ENC_KEY_MASK    (1 << 0)
#define BTM_BLE_ID_KEY_MASK     (1 << 1)
#define BTM_BLE_CSR_KEY_MASK    (1 << 2)
#define BTM_BLE_LINK_KEY_MASK   (1 << 3)

#define BTM_BLE_INITIATOR_KEY_SIZE 15
#define BTM_BLE_RESPONDER_KEY_SIZE 15
#define BTM_BLE_MAX_KEY_SIZE       16

typedef UINT8 tBTM_AUTH_REQ;

enum {
    BTM_OOB_NONE,
    BTM_OOB_PRESENT
#if BTM_OOB_INCLUDED == TRUE
    , BTM_OOB_UNKNOWN
#endif
};
typedef UINT8 tBTM_OOB_DATA;

/* data type for BTM_SP_IO_REQ_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    tBTM_IO_CAP     io_cap;         /* local IO capabilities */
    tBTM_OOB_DATA   oob_data;       /* OOB data present (locally) for the peer device */
    tBTM_AUTH_REQ   auth_req;       /* Authentication required (for local device) */
    BOOLEAN         is_orig;        /* TRUE, if local device initiated the SP process */
} tBTM_SP_IO_REQ;

/* data type for BTM_SP_IO_RSP_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    tBTM_IO_CAP     io_cap;         /* peer IO capabilities */
    tBTM_OOB_DATA   oob_data;       /* OOB data present at peer device for the local device */
    tBTM_AUTH_REQ   auth_req;       /* Authentication required for peer device */
} tBTM_SP_IO_RSP;

/* data type for BTM_SP_CFM_REQ_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    tBTM_BD_NAME    bd_name;        /* peer device name */
    UINT32          num_val;        /* the numeric value for comparison. If just_works, do not show this number to UI */
    BOOLEAN         just_works;     /* TRUE, if "Just Works" association model */
    tBTM_AUTH_REQ   loc_auth_req;   /* Authentication required for local device */
    tBTM_AUTH_REQ   rmt_auth_req;   /* Authentication required for peer device */
    tBTM_IO_CAP     loc_io_caps;    /* IO Capabilities of the local device */
    tBTM_IO_CAP     rmt_io_caps;    /* IO Capabilities of the remot device */
} tBTM_SP_CFM_REQ;

/* data type for BTM_SP_KEY_REQ_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    tBTM_BD_NAME    bd_name;        /* peer device name */
} tBTM_SP_KEY_REQ;

/* data type for BTM_SP_KEY_NOTIF_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    tBTM_BD_NAME    bd_name;        /* peer device name */
    UINT32          passkey;        /* passkey */
} tBTM_SP_KEY_NOTIF;

enum {
    BTM_SP_KEY_STARTED,         /* 0 - passkey entry started */
    BTM_SP_KEY_ENTERED,         /* 1 - passkey digit entered */
    BTM_SP_KEY_ERASED,          /* 2 - passkey digit erased */
    BTM_SP_KEY_CLEARED,         /* 3 - passkey cleared */
    BTM_SP_KEY_COMPLT,          /* 4 - passkey entry completed */
    BTM_SP_KEY_OUT_OF_RANGE     /* 5 - out of range */
};
typedef UINT8   tBTM_SP_KEY_TYPE;

/* data type for BTM_SP_KEYPRESS_EVT */
typedef struct {
    BD_ADDR             bd_addr;        /* peer address */
    tBTM_SP_KEY_TYPE   notif_type;
} tBTM_SP_KEYPRESS;

/* data type for BTM_SP_LOC_OOB_EVT */
typedef struct {
    tBTM_STATUS     status;         /* */
    BT_OCTET16      c;              /* Simple Pairing Hash C */
    BT_OCTET16      r;              /* Simple Pairing Randomnizer R */
} tBTM_SP_LOC_OOB;

/* data type for BTM_SP_RMT_OOB_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    tBTM_BD_NAME    bd_name;        /* peer device name */
} tBTM_SP_RMT_OOB;


/* data type for BTM_SP_COMPLT_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    DEV_CLASS       dev_class;      /* peer CoD */
    tBTM_BD_NAME    bd_name;        /* peer device name */
    tBTM_STATUS     status;         /* status of the simple pairing process */
} tBTM_SP_COMPLT;

/* data type for BTM_SP_UPGRADE_EVT */
typedef struct {
    BD_ADDR         bd_addr;        /* peer address */
    BOOLEAN         upgrade;        /* TRUE, to upgrade the link key */
} tBTM_SP_UPGRADE;

typedef union {
    tBTM_SP_IO_REQ      io_req;     /* BTM_SP_IO_REQ_EVT      */
    tBTM_SP_IO_RSP      io_rsp;     /* BTM_SP_IO_RSP_EVT      */
    tBTM_SP_CFM_REQ     cfm_req;    /* BTM_SP_CFM_REQ_EVT     */
    tBTM_SP_KEY_NOTIF   key_notif;  /* BTM_SP_KEY_NOTIF_EVT   */
    tBTM_SP_KEY_REQ     key_req;    /* BTM_SP_KEY_REQ_EVT     */
    tBTM_SP_KEYPRESS    key_press;  /* BTM_SP_KEYPRESS_EVT    */
    tBTM_SP_LOC_OOB     loc_oob;    /* BTM_SP_LOC_OOB_EVT     */
    tBTM_SP_RMT_OOB     rmt_oob;    /* BTM_SP_RMT_OOB_EVT     */
    tBTM_SP_COMPLT      complt;     /* BTM_SP_COMPLT_EVT      */
    tBTM_SP_UPGRADE     upgrade;    /* BTM_SP_UPGRADE_EVT      */
} tBTM_SP_EVT_DATA;

/* Simple Pairing Events.  Called by the stack when Simple Pairing related
** events occur.
*/
typedef UINT8 (tBTM_SP_CALLBACK) (tBTM_SP_EVT event, tBTM_SP_EVT_DATA *p_data);


typedef void (tBTM_MKEY_CALLBACK) (BD_ADDR bd_addr, UINT8 status, UINT8 key_flag) ;

/* Encryption enabled/disabled complete: Optionally passed with BTM_SetEncryption.
** Parameters are
**              BD Address of remote
**              optional data passed in by BTM_SetEncryption
**              tBTM_STATUS - result of the operation
*/
typedef void (tBTM_SEC_CBACK) (BD_ADDR bd_addr, tBT_TRANSPORT trasnport,
                               void *p_ref_data, tBTM_STATUS result);

/* Bond Cancel complete. Parameters are
**              Result of the cancel operation
**
*/
typedef void (tBTM_BOND_CANCEL_CMPL_CALLBACK) (tBTM_STATUS result);

/* LE related event and data structure
*/
/* relate to ESP_LE_KEY_xxx in esp_gap_ble_api.h */
#if (SMP_INCLUDED == TRUE)
#define BTM_LE_IO_REQ_EVT       SMP_IO_CAP_REQ_EVT     /* received IO_CAPABILITY_REQUEST event */
#define BTM_LE_SEC_REQUEST_EVT  SMP_SEC_REQUEST_EVT    /* security request event */
#define BTM_LE_KEY_NOTIF_EVT    SMP_PASSKEY_NOTIF_EVT  /* received USER_PASSKEY_NOTIFY event */
#define BTM_LE_KEY_REQ_EVT      SMP_PASSKEY_REQ_EVT    /* received USER_PASSKEY_REQUEST event */
#define BTM_LE_OOB_REQ_EVT      SMP_OOB_REQ_EVT        /* OOB data request event */
#define BTM_LE_NC_REQ_EVT       SMP_NC_REQ_EVT          /* Numeric Comparison request event */
#define BTM_LE_PR_KEYPR_NOT_EVT SMP_PEER_KEYPR_NOT_EVT /* Peer keypress notification recd event */
/* SC OOB request event (both local and peer OOB data) can be expected in response */
#define BTM_LE_SC_OOB_REQ_EVT   SMP_SC_OOB_REQ_EVT
/* SC OOB local data set is created (as result of SMP_CrLocScOobData(...)) */
#define BTM_LE_SC_LOC_OOB_EVT   SMP_SC_LOC_OOB_DATA_UP_EVT
#define BTM_LE_BR_KEYS_REQ_EVT  SMP_BR_KEYS_REQ_EVT     /* SMP over BR keys request event */
#define BTM_LE_COMPLT_EVT       SMP_COMPLT_EVT         /* SMP complete event */
#define BTM_LE_LAST_FROM_SMP    BTM_LE_BR_KEYS_REQ_EVT
#define BTM_LE_KEY_EVT          BTM_LE_LAST_FROM_SMP + 1 /* KEY update event */
#endif  ///SMP_INCLUDED == TRUE
typedef UINT8 tBTM_LE_EVT;

#if (BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE)
#define BTM_LE_KEY_NONE           0
#define BTM_LE_KEY_PENC      SMP_SEC_KEY_TYPE_ENC        /* encryption information of peer device */
#define BTM_LE_KEY_PID       SMP_SEC_KEY_TYPE_ID         /* identity key of the peer device */
#define BTM_LE_KEY_PCSRK     SMP_SEC_KEY_TYPE_CSRK      /* peer SRK */
#define BTM_LE_KEY_PLK       SMP_SEC_KEY_TYPE_LK
#define BTM_LE_KEY_LLK       (SMP_SEC_KEY_TYPE_LK << 4)
#define BTM_LE_KEY_LENC      (SMP_SEC_KEY_TYPE_ENC << 4)  /* master role security information:div */
#define BTM_LE_KEY_LID       (SMP_SEC_KEY_TYPE_ID << 4)   /* master device ID key */
#define BTM_LE_KEY_LCSRK     (SMP_SEC_KEY_TYPE_CSRK << 4) /* local CSRK has been deliver to peer */
#endif  ///BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
typedef UINT8 tBTM_LE_KEY_TYPE;

/* relate to ESP_LE_AUTH_xxx in esp_gap_ble_api.h */
#if (SMP_INCLUDED == TRUE)
#define BTM_LE_AUTH_REQ_NO_BOND SMP_AUTH_NO_BOND   /* 0 */
#define BTM_LE_AUTH_REQ_BOND    SMP_AUTH_GEN_BOND  /* 1 << 0 */
#define BTM_LE_AUTH_REQ_MITM    SMP_AUTH_YN_BIT    /* 1 << 2 */
#endif  ///SMP_INCLUDED == TRUE
typedef UINT8 tBTM_LE_AUTH_REQ;
#if (SMP_INCLUDED == TRUE)
#define BTM_LE_SC_SUPPORT_BIT           SMP_SC_SUPPORT_BIT     /* (1 << 3) */
#define BTM_LE_KP_SUPPORT_BIT           SMP_KP_SUPPORT_BIT     /* (1 << 4) */

#define BTM_LE_AUTH_REQ_SC_ONLY         SMP_AUTH_SC_ENC_ONLY    /* 1 << 3 */
#define BTM_LE_AUTH_REQ_SC_BOND         SMP_AUTH_SC_GB          /* 1001 */
#define BTM_LE_AUTH_REQ_SC_MITM         SMP_AUTH_SC_MITM_NB     /* 1100 */
#define BTM_LE_AUTH_REQ_SC_MITM_BOND    SMP_AUTH_SC_MITM_GB     /* 1101 */
#define BTM_LE_AUTH_REQ_MASK            SMP_AUTH_MASK           /* 0x1D */

/* LE security level */
#define BTM_LE_SEC_NONE             SMP_SEC_NONE
#define BTM_LE_SEC_UNAUTHENTICATE   SMP_SEC_UNAUTHENTICATE      /* 1 */
#define BTM_LE_SEC_AUTHENTICATED    SMP_SEC_AUTHENTICATED       /* 4 */
#endif  ///SMP_INCLUDED == TRUE
typedef UINT8 tBTM_LE_SEC;


typedef struct {
    tBTM_IO_CAP         io_cap;         /* local IO capabilities */
    UINT8               oob_data;       /* OOB data present (locally) for the peer device */
    tBTM_LE_AUTH_REQ    auth_req;       /* Authentication request (for local device) contain bonding and MITM info */
    UINT8               max_key_size;   /* max encryption key size */
    tBTM_LE_KEY_TYPE    init_keys;      /* keys to be distributed, bit mask */
    tBTM_LE_KEY_TYPE    resp_keys;      /* keys to be distributed, bit mask */
} tBTM_LE_IO_REQ;

#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
/* data type for tBTM_LE_COMPLT */
typedef struct {
    UINT8 reason;
    UINT8 sec_level;
    BOOLEAN is_pair_cancel;
    BOOLEAN smp_over_br;
} tBTM_LE_COMPLT;
#endif

/* BLE encryption keys */
typedef struct {
    BT_OCTET16  ltk;
    BT_OCTET8   rand;
    UINT16      ediv;
    UINT8       sec_level;
    UINT8       key_size;
} tBTM_LE_PENC_KEYS;

/* BLE CSRK keys */
typedef struct {
    UINT32          counter;
    BT_OCTET16      csrk;
    UINT8           sec_level;
} tBTM_LE_PCSRK_KEYS;

/* BLE Encryption reproduction keys */
typedef struct {
    BT_OCTET16  ltk;
    UINT16      div;
    UINT8       key_size;
    UINT8       sec_level;
} tBTM_LE_LENC_KEYS;

/* BLE SRK keys */
typedef struct {
    UINT32          counter;
    UINT16          div;
    UINT8           sec_level;
    BT_OCTET16      csrk;
} tBTM_LE_LCSRK_KEYS;

typedef struct {
    BT_OCTET16          irk;
    tBLE_ADDR_TYPE      addr_type;
    BD_ADDR             static_addr;
} tBTM_LE_PID_KEYS;

typedef union {
    tBTM_LE_PENC_KEYS   penc_key;       /* received peer encryption key */
    tBTM_LE_PCSRK_KEYS  pcsrk_key;      /* received peer device SRK */
    tBTM_LE_PID_KEYS    pid_key;        /* peer device ID key */
    tBTM_LE_LENC_KEYS   lenc_key;       /* local encryption reproduction keys LTK = = d1(ER,DIV,0)*/
    tBTM_LE_LCSRK_KEYS   lcsrk_key;     /* local device CSRK = d1(ER,DIV,1)*/
} tBTM_LE_KEY_VALUE;

typedef struct {
    tBTM_LE_KEY_TYPE        key_type;
    tBTM_LE_KEY_VALUE       *p_key_value;
} tBTM_LE_KEY;

typedef union {
    tBTM_LE_IO_REQ      io_req;     /* BTM_LE_IO_REQ_EVT      */
    UINT32              key_notif;  /* BTM_LE_KEY_NOTIF_EVT   */
    /* BTM_LE_NC_REQ_EVT */
    /* no callback data for BTM_LE_KEY_REQ_EVT */
    /* and BTM_LE_OOB_REQ_EVT  */
#if BLE_INCLUDED == TRUE && SMP_INCLUDED == TRUE
    tBTM_LE_COMPLT      complt;     /* BTM_LE_COMPLT_EVT      */
    tSMP_OOB_DATA_TYPE  req_oob_type;
#endif
    tBTM_LE_KEY         key;
} tBTM_LE_EVT_DATA;

/* Simple Pairing Events.  Called by the stack when Simple Pairing related
** events occur.
*/
typedef UINT8 (tBTM_LE_CALLBACK) (tBTM_LE_EVT event, BD_ADDR bda, tBTM_LE_EVT_DATA *p_data);

#define BTM_BLE_KEY_TYPE_ID         1
#define BTM_BLE_KEY_TYPE_ER         2
#define BTM_BLE_KEY_TYPE_COUNTER    3  //tobe obsolete

typedef struct {
    BT_OCTET16       ir;
    BT_OCTET16       irk;
    BT_OCTET16       dhk;

} tBTM_BLE_LOCAL_ID_KEYS;

typedef union {
    tBTM_BLE_LOCAL_ID_KEYS  id_keys;
    BT_OCTET16              er;
} tBTM_BLE_LOCAL_KEYS;


/* New LE identity key for local device.
*/
typedef void (tBTM_LE_KEY_CALLBACK) (UINT8 key_type, tBTM_BLE_LOCAL_KEYS *p_key);


/***************************
**  Security Manager Types
****************************/
/* Structure that applications use to register with BTM_SecRegister */
typedef struct {
    tBTM_AUTHORIZE_CALLBACK     *p_authorize_callback;
    tBTM_PIN_CALLBACK           *p_pin_callback;
    tBTM_LINK_KEY_CALLBACK      *p_link_key_callback;
    tBTM_AUTH_COMPLETE_CALLBACK *p_auth_complete_callback;
    tBTM_BOND_CANCEL_CMPL_CALLBACK *p_bond_cancel_cmpl_callback;
    tBTM_SP_CALLBACK            *p_sp_callback;
#if BLE_INCLUDED == TRUE
#if SMP_INCLUDED == TRUE
    tBTM_LE_CALLBACK            *p_le_callback;
#endif
    tBTM_LE_KEY_CALLBACK        *p_le_key_callback;
#endif
} tBTM_APPL_INFO;

/* Callback function for when a link supervision timeout event occurs.
** This asynchronous event is enabled/disabled by calling BTM_RegForLstoEvt().
*/
typedef void (tBTM_LSTO_CBACK) (BD_ADDR remote_bda, UINT16 timeout);

/*****************************************************************************
**  POWER MANAGEMENT
*****************************************************************************/
/****************************
**  Power Manager Constants
*****************************/
/* BTM Power manager status codes */
enum {
    BTM_PM_STS_ACTIVE = HCI_MODE_ACTIVE,
    BTM_PM_STS_HOLD   = HCI_MODE_HOLD,
    BTM_PM_STS_SNIFF  = HCI_MODE_SNIFF,
    BTM_PM_STS_PARK   = HCI_MODE_PARK,
    BTM_PM_STS_SSR,     /* report the SSR parameters in HCI_SNIFF_SUB_RATE_EVT */
    BTM_PM_STS_PENDING,   /* when waiting for status from controller */
    BTM_PM_STS_ERROR   /* when HCI command status returns error */
};
typedef UINT8 tBTM_PM_STATUS;

/* BTM Power manager modes */
enum {
    BTM_PM_MD_ACTIVE = BTM_PM_STS_ACTIVE,
    BTM_PM_MD_HOLD   = BTM_PM_STS_HOLD,
    BTM_PM_MD_SNIFF  = BTM_PM_STS_SNIFF,
    BTM_PM_MD_PARK   = BTM_PM_STS_PARK,
    BTM_PM_MD_FORCE  = 0x10 /* OR this to force ACL link to a certain mode */
};
typedef UINT8 tBTM_PM_MODE;

#define BTM_PM_SET_ONLY_ID  0x80

/* Operation codes */
#define BTM_PM_REG_SET      1 /* The module wants to set the desired power mode */
#define BTM_PM_REG_NOTIF    2 /* The module wants to receive mode change event */
#define BTM_PM_DEREG        4 /* The module does not want to involve with PM anymore */

/************************
**  Power Manager Types
*************************/
typedef struct {
    UINT16          max;
    UINT16          min;
    UINT16          attempt;
    UINT16          timeout;
    tBTM_PM_MODE    mode;
} tBTM_PM_PWR_MD;

/*************************************
**  Power Manager Callback Functions
**************************************/
typedef void (tBTM_PM_STATUS_CBACK) (BD_ADDR p_bda, tBTM_PM_STATUS status,
                                     UINT16 value, UINT8 hci_status);


/************************
**  Stored Linkkey Types
*************************/
#define BTM_CB_EVT_DELETE_STORED_LINK_KEYS  4

typedef struct {
    UINT8          event;
    UINT8          status;
    UINT16         num_keys;

} tBTM_DELETE_STORED_LINK_KEY_COMPLETE;

/* MIP evnets, callbacks    */
enum {
    BTM_MIP_MODE_CHG_EVT,
    BTM_MIP_DISCONNECT_EVT,
    BTM_MIP_PKTS_COMPL_EVT,
    BTM_MIP_RXDATA_EVT
};
typedef UINT8 tBTM_MIP_EVT;

typedef struct {
    tBTM_MIP_EVT    event;
    BD_ADDR         bd_addr;
    UINT16          mip_id;
} tBTM_MIP_MODE_CHANGE;

typedef struct {
    tBTM_MIP_EVT    event;
    UINT16          mip_id;
    UINT8           disc_reason;
} tBTM_MIP_CONN_TIMEOUT;

#define BTM_MIP_MAX_RX_LEN  17

typedef struct {
    tBTM_MIP_EVT    event;
    UINT16          mip_id;
    UINT8           rx_len;
    UINT8           rx_data[BTM_MIP_MAX_RX_LEN];
} tBTM_MIP_RXDATA;

typedef struct {
    tBTM_MIP_EVT    event;
    BD_ADDR         bd_addr;
    UINT8           data[11];       /* data[0] shows Vender-specific device type */
} tBTM_MIP_EIR_HANDSHAKE;

typedef struct {
    tBTM_MIP_EVT    event;
    UINT16          num_sent;       /* Number of packets completed at the controller */
} tBTM_MIP_PKTS_COMPL;

typedef union {
    tBTM_MIP_EVT            event;
    tBTM_MIP_MODE_CHANGE    mod_chg;
    tBTM_MIP_CONN_TIMEOUT   conn_tmo;
    tBTM_MIP_EIR_HANDSHAKE  eir;
    tBTM_MIP_PKTS_COMPL     completed;
    tBTM_MIP_RXDATA         rxdata;
} tBTM_MIP_EVENT_DATA;

/* MIP event callback function  */
typedef void (tBTM_MIP_EVENTS_CB) (tBTM_MIP_EVT event, tBTM_MIP_EVENT_DATA data);

/* MIP Device query callback function  */
typedef BOOLEAN (tBTM_MIP_QUERY_CB) (BD_ADDR dev_addr, UINT8 *p_mode, LINK_KEY link_key);

#define BTM_CONTRL_ACTIVE  1       /* ACL link on, SCO link ongoing, sniff mode */
#define BTM_CONTRL_SCAN    2       /* Scan state - paging/inquiry/trying to connect*/
#define BTM_CONTRL_IDLE    3       /* Idle state - page scan, LE advt, inquiry scan */

typedef UINT8 tBTM_CONTRL_STATE;

/*****************************************************************************
**  EXTERNAL FUNCTION DECLARATIONS
*****************************************************************************/
/*
#ifdef __cplusplus
extern "C" {
#endif
*/
/*****************************************************************************
**  DEVICE CONTROL and COMMON FUNCTIONS
*****************************************************************************/

/*******************************************************************************
**
** Function         BTM_DeviceReset
**
** Description      This function is called to reset the controller.The Callback function
**                  if provided is called when startup of the device has
**                  completed.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_DeviceReset (tBTM_CMPL_CB *p_cb);


/*******************************************************************************
**
** Function         BTM_IsDeviceUp
**
** Description      This function is called to check if the device is up.
**
** Returns          TRUE if device is up, else FALSE
**
*******************************************************************************/
//extern
BOOLEAN BTM_IsDeviceUp (void);


/*******************************************************************************
**
** Function         BTM_SetLocalDeviceName
**
** Description      This function is called to set the local device name.
**
** Returns          BTM_CMD_STARTED if successful, otherwise an error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetLocalDeviceName (char *p_name);

/*******************************************************************************
**
** Function         BTM_SetDeviceClass
**
** Description      This function is called to set the local device class
**
** Returns          BTM_SUCCESS if successful, otherwise an error
**
*******************************************************************************/
//extern
tBTM_STATUS  BTM_SetDeviceClass (DEV_CLASS dev_class);


/*******************************************************************************
**
** Function         BTM_ReadLocalDeviceName
**
** Description      This function is called to read the local device name.
**
** Returns          status of the operation
**                  If success, BTM_SUCCESS is returned and p_name points stored
**                              local device name
**                  If BTM doesn't store local device name, BTM_NO_RESOURCES is
**                              is returned and p_name is set to NULL
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadLocalDeviceName (char **p_name);

/*******************************************************************************
**
** Function         BTM_ReadLocalDeviceNameFromController
**
** Description      Get local device name from controller. Do not use cached
**                  name (used to get chip-id prior to btm reset complete).
**
** Returns          BTM_CMD_STARTED if successful, otherwise an error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadLocalDeviceNameFromController (tBTM_CMPL_CB *p_rln_cmpl_cback);

/*******************************************************************************
**
** Function         BTM_ReadDeviceClass
**
** Description      This function is called to read the local device class
**
** Returns          pointer to the device class
**
*******************************************************************************/
//extern
UINT8 *BTM_ReadDeviceClass (void);


/*******************************************************************************
**
** Function         BTM_ReadLocalFeatures
**
** Description      This function is called to read the local features
**
** Returns          pointer to the local features string
**
*******************************************************************************/
//extern
UINT8 *BTM_ReadLocalFeatures (void);

/*******************************************************************************
**
** Function         BTM_RegisterForDeviceStatusNotif
**
** Description      This function is called to register for device status
**                  change notifications.
**
** Returns          pointer to previous caller's callback function or NULL if first
**                  registration.
**
*******************************************************************************/
//extern
tBTM_DEV_STATUS_CB *BTM_RegisterForDeviceStatusNotif (tBTM_DEV_STATUS_CB *p_cb);


/*******************************************************************************
**
** Function         BTM_RegisterForVSEvents
**
** Description      This function is called to register/deregister for vendor
**                  specific HCI events.
**
**                  If is_register=TRUE, then the function will be registered;
**                  if is_register=FALSE, then the function will be deregistered.
**
** Returns          BTM_SUCCESS if successful,
**                  BTM_BUSY if maximum number of callbacks have already been
**                           registered.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_RegisterForVSEvents (tBTM_VS_EVT_CB *p_cb, BOOLEAN is_register);


/*******************************************************************************
**
** Function         BTM_VendorSpecificCommand
**
** Description      Send a vendor specific HCI command to the controller.
**
** Returns
**      BTM_SUCCESS         Command sent. Does not expect command complete
**                              event. (command cmpl callback param is NULL)
**      BTM_CMD_STARTED     Command sent. Waiting for command cmpl event.
**      BTM_BUSY            Command not sent. Waiting for cmd cmpl event for
**                              prior command.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_VendorSpecificCommand(UINT16 opcode,
                                      UINT8 param_len,
                                      UINT8 *p_param_buf,
                                      tBTM_VSC_CMPL_CB *p_cb);


/*******************************************************************************
**
** Function         BTM_AllocateSCN
**
** Description      Look through the Server Channel Numbers for a free one to be
**                  used with an RFCOMM connection.
**
** Returns          Allocated SCN number or 0 if none.
**
*******************************************************************************/
//extern
#if (CLASSIC_BT_INCLUDED == TRUE)
UINT8 BTM_AllocateSCN(void);

// btla-specific ++
/*******************************************************************************
**
** Function         BTM_TryAllocateSCN
**
** Description      Try to allocate a fixed server channel
**
** Returns          Returns TRUE if server channel was available
**
*******************************************************************************/
//extern
BOOLEAN BTM_TryAllocateSCN(UINT8 scn);
// btla-specific --


/*******************************************************************************
**
** Function         BTM_FreeSCN
**
** Description      Free the specified SCN.
**
** Returns          TRUE if successful, FALSE if SCN is not in use or invalid
**
*******************************************************************************/
//extern
BOOLEAN BTM_FreeSCN(UINT8 scn);
#endif  ///CLASSIC_BT_INCLUDED == TRUE


/*******************************************************************************
**
** Function         BTM_SetTraceLevel
**
** Description      This function sets the trace level for BTM.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
** Returns          The new or current trace level
**
*******************************************************************************/
//extern
UINT8 BTM_SetTraceLevel (UINT8 new_level);


/*******************************************************************************
**
** Function         BTM_WritePageTimeout
**
** Description      Send HCI Wite Page Timeout.
**
** Returns
**      BTM_SUCCESS         Command sent.
**      BTM_NO_RESOURCES    If out of resources to send the command.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_WritePageTimeout(UINT16 timeout);

/*******************************************************************************
**
** Function         BTM_WriteVoiceSettings
**
** Description      Send HCI Write Voice Settings command.
**                  See hcidefs.h for settings bitmask values.
**
** Returns
**      BTM_SUCCESS         Command sent.
**      BTM_NO_RESOURCES     If out of resources to send the command.
**
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_WriteVoiceSettings(UINT16 settings);

/*******************************************************************************
**
** Function         BTM_EnableTestMode
**
** Description      Send HCI the enable device under test command.
**
**                  Note: Controller can only be taken out of this mode by
**                      resetting the controller.
**
** Returns
**      BTM_SUCCESS         Command sent.
**      BTM_NO_RESOURCES    If out of resources to send the command.
**
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_EnableTestMode(void);


/*****************************************************************************
**  DEVICE DISCOVERY FUNCTIONS - Inquiry, Remote Name, Discovery, Class of Device
*****************************************************************************/

/*******************************************************************************
**
** Function         BTM_SetDiscoverability
**
** Description      This function is called to set the device into or out of
**                  discoverable mode. Discoverable mode means inquiry
**                  scans are enabled.  If a value of '0' is entered for window or
**                  interval, the default values are used.
**
** Returns          BTM_SUCCESS if successful
**                  BTM_BUSY if a setting of the filter is already in progress
**                  BTM_NO_RESOURCES if couldn't get a memory pool buffer
**                  BTM_ILLEGAL_VALUE if a bad parameter was detected
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS  BTM_SetDiscoverability (UINT16 inq_mode, UINT16 window,
                                     UINT16 interval);


/*******************************************************************************
**
** Function         BTM_ReadDiscoverability
**
** Description      This function is called to read the current discoverability
**                  mode of the device.
**
** Output Params:   p_window - current inquiry scan duration
**                  p_interval - current inquiry scan interval
**
** Returns          BTM_NON_DISCOVERABLE, BTM_LIMITED_DISCOVERABLE, or
**                  BTM_GENERAL_DISCOVERABLE
**
*******************************************************************************/
//extern
UINT16       BTM_ReadDiscoverability (UINT16 *p_window,
                                      UINT16 *p_interval);


/*******************************************************************************
**
** Function         BTM_SetPeriodicInquiryMode
**
** Description      This function is called to set the device periodic inquiry mode.
**                  If the duration is zero, the periodic inquiry mode is cancelled.
**
** Parameters:      p_inqparms - pointer to the inquiry information
**                      mode - GENERAL or LIMITED inquiry
**                      duration - length in 1.28 sec intervals (If '0', the inquiry is CANCELLED)
**                      max_resps - maximum amount of devices to search for before ending the inquiry
**                      filter_cond_type - BTM_CLR_INQUIRY_FILTER, BTM_FILTER_COND_DEVICE_CLASS, or
**                                         BTM_FILTER_COND_BD_ADDR
**                      filter_cond - value for the filter (based on filter_cond_type)
**
**                  max_delay - maximum amount of time between successive inquiries
**                  min_delay - minimum amount of time between successive inquiries
**                  p_results_cb - callback returning pointer to results (tBTM_INQ_RESULTS)
**
** Returns          BTM_CMD_STARTED if successfully started
**                  BTM_ILLEGAL_VALUE if a bad parameter is detected
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_SUCCESS - if cancelling the periodic inquiry
**                  BTM_BUSY - if an inquiry is already active
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS  BTM_SetPeriodicInquiryMode (tBTM_INQ_PARMS *p_inqparms,
        UINT16 max_delay, UINT16 min_delay,
        tBTM_INQ_RESULTS_CB *p_results_cb);


/*******************************************************************************
**
** Function         BTM_StartInquiry
**
** Description      This function is called to start an inquiry.
**
** Parameters:      p_inqparms - pointer to the inquiry information
**                      mode - GENERAL or LIMITED inquiry
**                      duration - length in 1.28 sec intervals (If '0', the inquiry is CANCELLED)
**                      max_resps - maximum amount of devices to search for before ending the inquiry
**                      filter_cond_type - BTM_CLR_INQUIRY_FILTER, BTM_FILTER_COND_DEVICE_CLASS, or
**                                         BTM_FILTER_COND_BD_ADDR
**                      filter_cond - value for the filter (based on filter_cond_type)
**
**                  p_results_cb   - Pointer to the callback routine which gets called
**                                upon receipt of an inquiry result. If this field is
**                                NULL, the application is not notified.
**
**                  p_cmpl_cb   - Pointer to the callback routine which gets called
**                                upon completion.  If this field is NULL, the
**                                application is not notified when completed.
** Returns          tBTM_STATUS
**                  BTM_CMD_STARTED if successfully initiated
**                  BTM_BUSY if already in progress
**                  BTM_ILLEGAL_VALUE if parameter(s) are out of range
**                  BTM_NO_RESOURCES if could not allocate resources to start the command
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS  BTM_StartInquiry (tBTM_INQ_PARMS *p_inqparms,
                               tBTM_INQ_RESULTS_CB *p_results_cb,
                               tBTM_CMPL_CB *p_cmpl_cb);


/*******************************************************************************
**
** Function         BTM_IsInquiryActive
**
** Description      This function returns a bit mask of the current inquiry state
**
** Returns          BTM_INQUIRY_INACTIVE if inactive (0)
**                  BTM_LIMITED_INQUIRY_ACTIVE if a limted inquiry is active
**                  BTM_GENERAL_INQUIRY_ACTIVE if a general inquiry is active
**                  BTM_PERIODIC_INQUIRY_ACTIVE if a periodic inquiry is active
**
*******************************************************************************/
//extern
UINT16 BTM_IsInquiryActive (void);


/*******************************************************************************
**
** Function         BTM_CancelInquiry
**
** Description      This function cancels an inquiry if active
**
** Returns          BTM_SUCCESS if successful
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_CancelInquiry(void);


/*******************************************************************************
**
** Function         BTM_CancelPeriodicInquiry
**
** Description      This function cancels a periodic inquiry
**
** Returns
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_SUCCESS - if cancelling the periodic inquiry
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_CancelPeriodicInquiry(void);


/*******************************************************************************
**
** Function         BTM_SetConnectability
**
** Description      This function is called to set the device into or out of
**                  connectable mode. Discoverable mode means page scans enabled.
**
** Returns          BTM_SUCCESS if successful
**                  BTM_ILLEGAL_VALUE if a bad parameter is detected
**                  BTM_NO_RESOURCES if could not allocate a message buffer
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetConnectability (UINT16 page_mode, UINT16 window,
                                   UINT16 interval);


/*******************************************************************************
**
** Function         BTM_ReadConnectability
**
** Description      This function is called to read the current discoverability
**                  mode of the device.
** Output Params    p_window - current page scan duration
**                  p_interval - current time between page scans
**
** Returns          BTM_NON_CONNECTABLE or BTM_CONNECTABLE
**
*******************************************************************************/
//extern
UINT16 BTM_ReadConnectability (UINT16 *p_window, UINT16 *p_interval);


/*******************************************************************************
**
** Function         BTM_SetInquiryMode
**
** Description      This function is called to set standard, with RSSI
**                  mode or extended of the inquiry for local device.
**
** Input Params:    BTM_INQ_RESULT_STANDARD, BTM_INQ_RESULT_WITH_RSSI or
**                  BTM_INQ_RESULT_EXTENDED
**
** Returns          BTM_SUCCESS if successful
**                  BTM_NO_RESOURCES if couldn't get a memory pool buffer
**                  BTM_ILLEGAL_VALUE if a bad parameter was detected
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS  BTM_SetInquiryMode (UINT8 mode);

/*******************************************************************************
**
** Function         BTM_SetInquiryScanType
**
** Description      This function is called to set the iquiry scan-type to
**                  standard or interlaced.
**
** Input Params:    BTM_SCAN_TYPE_STANDARD or BTM_SCAN_TYPE_INTERLACED
**
** Returns          BTM_SUCCESS if successful
**                  BTM_MODE_UNSUPPORTED if not a 1.2 device
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetInquiryScanType (UINT16 scan_type);

/*******************************************************************************
**
** Function         BTM_SetPageScanType
**
** Description      This function is called to set the page scan-type to
**                  standard or interlaced.
**
** Input Params:    BTM_SCAN_TYPE_STANDARD or BTM_SCAN_TYPE_INTERLACED
**
** Returns          BTM_SUCCESS if successful
**                  BTM_MODE_UNSUPPORTED if not a 1.2 device
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/

//extern
tBTM_STATUS BTM_SetPageScanType (UINT16 scan_type);

/*******************************************************************************
**
** Function         BTM_ReadRemoteDeviceName
**
** Description      This function initiates a remote device HCI command to the
**                  controller and calls the callback when the process has completed.
**
** Input Params:    remote_bda      - device address of name to retrieve
**                  p_cb            - callback function called when BTM_CMD_STARTED
**                                    is returned.
**                                    A pointer to tBTM_REMOTE_DEV_NAME is passed to the
**                                    callback.
**
** Returns
**                  BTM_CMD_STARTED is returned if the request was successfully sent
**                                  to HCI.
**                  BTM_BUSY if already in progress
**                  BTM_UNKNOWN_ADDR if device address is bad
**                  BTM_NO_RESOURCES if could not allocate resources to start the command
**                  BTM_WRONG_MODE if the device is not up.
**
*******************************************************************************/
//extern
tBTM_STATUS  BTM_ReadRemoteDeviceName (BD_ADDR remote_bda,
                                       tBTM_CMPL_CB *p_cb,
                                       tBT_TRANSPORT transport);


/*******************************************************************************
**
** Function         BTM_CancelRemoteDeviceName
**
** Description      This function initiates the cancel request for the specified
**                  remote device.
**
** Input Params:    None
**
** Returns
**                  BTM_CMD_STARTED is returned if the request was successfully sent
**                                  to HCI.
**                  BTM_NO_RESOURCES if could not allocate resources to start the command
**                  BTM_WRONG_MODE if there is not an active remote name request.
**
*******************************************************************************/
//extern
tBTM_STATUS  BTM_CancelRemoteDeviceName (void);

/*******************************************************************************
**
** Function         BTM_ReadRemoteVersion
**
** Description      This function is called to read a remote device's version
**
** Returns          BTM_SUCCESS if successful, otherwise an error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadRemoteVersion (BD_ADDR addr,
                                   UINT8 *lmp_version,
                                   UINT16 *manufacturer,
                                   UINT16 *lmp_sub_version);

/*******************************************************************************
**
** Function         BTM_ReadRemoteFeatures
**
** Description      This function is called to read a remote device's
**                  supported features mask (features mask located at page 0)
**
**                  Note: The size of device features mask page is
**                  BTM_FEATURE_BYTES_PER_PAGE bytes.
**
** Returns          pointer to the remote supported features mask
**
*******************************************************************************/
//extern
UINT8 *BTM_ReadRemoteFeatures (BD_ADDR addr);

/*******************************************************************************
**
** Function         BTM_ReadRemoteExtendedFeatures
**
** Description      This function is called to read a specific extended features
**                  page of the remote device
**
**                  Note1: The size of device features mask page is
**                  BTM_FEATURE_BYTES_PER_PAGE bytes.
**                  Note2: The valid device features mask page number depends on
**                  the remote device capabilities. It is expected to be in the
**                  range [0 - BTM_EXT_FEATURES_PAGE_MAX].

** Returns          pointer to the remote extended features mask
**                  or NULL if page_number is not valid
**
*******************************************************************************/
//extern
UINT8 *BTM_ReadRemoteExtendedFeatures (BD_ADDR addr, UINT8 page_number);

/*******************************************************************************
**
** Function         BTM_ReadNumberRemoteFeaturesPages
**
** Description      This function is called to retrieve the number of feature pages
**                  read from the remote device
**
** Returns          number of features pages read from the remote device
**
*******************************************************************************/
//extern
UINT8 BTM_ReadNumberRemoteFeaturesPages (BD_ADDR addr);

/*******************************************************************************
**
** Function         BTM_ReadAllRemoteFeatures
**
** Description      This function is called to read all features of the remote device
**
** Returns          pointer to the byte[0] of the page[0] of the remote device
**                  feature mask.
**
** Note:            the function returns the pointer to the array of the size
**                  BTM_FEATURE_BYTES_PER_PAGE * (BTM_EXT_FEATURES_PAGE_MAX + 1).
**
*******************************************************************************/
//extern
UINT8 *BTM_ReadAllRemoteFeatures (BD_ADDR addr);

/*******************************************************************************
**
** Function         BTM_InqDbRead
**
** Description      This function looks through the inquiry database for a match
**                  based on Bluetooth Device Address. This is the application's
**                  interface to get the inquiry details of a specific BD address.
**
** Returns          pointer to entry, or NULL if not found
**
*******************************************************************************/
//extern
tBTM_INQ_INFO *BTM_InqDbRead (BD_ADDR p_bda);


/*******************************************************************************
**
** Function         BTM_InqDbFirst
**
** Description      This function looks through the inquiry database for the first
**                  used entry, and returns that. This is used in conjunction with
**                  BTM_InqDbNext by applications as a way to walk through the
**                  inquiry database.
**
** Returns          pointer to first in-use entry, or NULL if DB is empty
**
*******************************************************************************/
//extern
tBTM_INQ_INFO *BTM_InqDbFirst (void);


/*******************************************************************************
**
** Function         BTM_InqDbNext
**
** Description      This function looks through the inquiry database for the next
**                  used entry, and returns that.  If the input parameter is NULL,
**                  the first entry is returned.
**
** Returns          pointer to next in-use entry, or NULL if no more found.
**
*******************************************************************************/
//extern
tBTM_INQ_INFO *BTM_InqDbNext (tBTM_INQ_INFO *p_cur);


/*******************************************************************************
**
** Function         BTM_ClearInqDb
**
** Description      This function is called to clear out a device or all devices
**                  from the inquiry database.
**
** Parameter        p_bda - (input) BD_ADDR ->  Address of device to clear
**                                              (NULL clears all entries)
**
** Returns          BTM_BUSY if an inquiry, get remote name, or event filter
**                          is active, otherwise BTM_SUCCESS
**
*******************************************************************************/
//extern
tBTM_STATUS  BTM_ClearInqDb (BD_ADDR p_bda);

/*******************************************************************************
**
** Function         BTM_ReadInquiryRspTxPower
**
** Description      This command will read the inquiry Transmit Power level used
**                  to transmit the FHS and EIR data packets.
**                  This can be used directly in the Tx Power Level EIR data type.
**
** Returns          BTM_SUCCESS if successful
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadInquiryRspTxPower (tBTM_CMPL_CB *p_cb);

#if SDP_INCLUDED == TRUE
/*******************************************************************************
**
** Function         BTM_StartDiscovery
**
** Description      This function is called by an application (or profile)
**                  when it wants to trigger an service discovery using the
**                  BTM's discovery database.
**
** Returns          tBTM_STATUS
**                      BTM_CMD_STARTED if the discovery was initiated
**                      BTM_BUSY if one is already in progress
**                      BTM_UNKNOWN_ADDR if no addresses are in the INQ DB
**                      BTM_ERR_PROCESSING if err initiating the command
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_StartDiscovery (tBTM_CMPL_CB *p_cmpl_cb,
                                BD_ADDR_PTR p_rem_addr);


/*******************************************************************************
**
** Function         BTM_FindAttribute
**
** Description      This function is called by an application (or profile)
**                  when it wants to see if an attribute exists in the BTM
**                  discovery database.
**
** Returns          Pointer to matching record, or NULL
**
*******************************************************************************/
//extern
tSDP_DISC_REC *BTM_FindAttribute (UINT16 attr_id,
                                  tSDP_DISC_REC *p_start_rec);


/*******************************************************************************
**
** Function         BTM_FindService
**
** Description      This function is called by an application (or profile)
**                  when it wants to see if a service exists in the BTM
**                  discovery database.
**
** Returns          Pointer to matching record, or NULL
**
*******************************************************************************/
//extern
tSDP_DISC_REC *BTM_FindService (UINT16 service_uuid,
                                tSDP_DISC_REC *p_start_rec);


/*******************************************************************************
**
** Function         BTM_SetDiscoveryParams
**
** Description      This function is called to set the BTM default discovery parameters.
**                  These UUID and attribute filters are used during the call to
**                  BTM_StartDiscovery.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_SetDiscoveryParams (UINT16 num_uuid, tSDP_UUID *p_uuid_list,
                             UINT16 num_attr, UINT16 *p_attr_list);
#endif /*SDP_INCLUDED*/

/*****************************************************************************
**  ACL CHANNEL MANAGEMENT FUNCTIONS
*****************************************************************************/
/*******************************************************************************
**
** Function         BTM_SetLinkPolicy
**
** Description      Create and send HCI "Write Policy Set" command
**
** Returns          BTM_CMD_STARTED if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetLinkPolicy (BD_ADDR remote_bda,
                               UINT16 *settings);

/*******************************************************************************
**
** Function         BTM_SetDefaultLinkPolicy
**
** Description      Set the default value for HCI "Write Policy Set" command
**                  to use when an ACL link is created.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_SetDefaultLinkPolicy (UINT16 settings);


/*******************************************************************************
**
** Function         BTM_SetDefaultLinkSuperTout
**
** Description      Set the default value for HCI "Write Link Supervision Timeout"
**                  command to use when an ACL link is created.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_SetDefaultLinkSuperTout (UINT16 timeout);


/*******************************************************************************
**
** Function         BTM_SetLinkSuperTout
**
** Description      Create and send HCI "Write Link Supervision Timeout" command
**
** Returns          BTM_CMD_STARTED if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetLinkSuperTout (BD_ADDR remote_bda,
                                  UINT16 timeout);
/*******************************************************************************
**
** Function         BTM_GetLinkSuperTout
**
** Description      Read the link supervision timeout value of the connection
**
** Returns          status of the operation
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_GetLinkSuperTout (BD_ADDR remote_bda,
                                  UINT16 *p_timeout);

/*******************************************************************************
**
** Function         BTM_IsAclConnectionUp
**
** Description      This function is called to check if an ACL connection exists
**                  to a specific remote BD Address.
**
** Returns          TRUE if connection is up, else FALSE.
**
*******************************************************************************/
//extern
BOOLEAN BTM_IsAclConnectionUp (BD_ADDR remote_bda, tBT_TRANSPORT transport);


/*******************************************************************************
**
** Function         BTM_GetRole
**
** Description      This function is called to get the role of the local device
**                  for the ACL connection with the specified remote device
**
** Returns          BTM_SUCCESS if connection exists.
**                  BTM_UNKNOWN_ADDR if no active link with bd addr specified
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_GetRole (BD_ADDR remote_bd_addr, UINT8 *p_role);



/*******************************************************************************
**
** Function         BTM_SwitchRole
**
** Description      This function is called to switch role between master and
**                  slave.  If role is already set it will do nothing.  If the
**                  command was initiated, the callback function is called upon
**                  completion.
**
** Returns          BTM_SUCCESS if already in specified role.
**                  BTM_CMD_STARTED if command issued to controller.
**                  BTM_NO_RESOURCES if couldn't allocate memory to issue command
**                  BTM_UNKNOWN_ADDR if no active link with bd addr specified
**                  BTM_MODE_UNSUPPORTED if local device does not support role switching
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SwitchRole (BD_ADDR remote_bd_addr,
                            UINT8 new_role,
                            tBTM_CMPL_CB *p_cb);

/*******************************************************************************
**
** Function         BTM_ReadRSSI
**
** Description      This function is called to read the link policy settings.
**                  The address of link policy results are returned in the callback.
**                  (tBTM_RSSI_RESULTS)
**
** Returns          BTM_CMD_STARTED if command issued to controller.
**                  BTM_NO_RESOURCES if couldn't allocate memory to issue command
**                  BTM_UNKNOWN_ADDR if no active link with bd addr specified
**                  BTM_BUSY if command is already in progress
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadRSSI (BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);


/*******************************************************************************
**
** Function         BTM_ReadTxPower
**
** Description      This function is called to read the current connection
**                  TX power of the connection. The TX power level results
**                  are returned in the callback.
**                  (tBTM_RSSI_RESULTS)
**
** Returns          BTM_CMD_STARTED if command issued to controller.
**                  BTM_NO_RESOURCES if couldn't allocate memory to issue command
**                  BTM_UNKNOWN_ADDR if no active link with bd addr specified
**                  BTM_BUSY if command is already in progress
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadTxPower (BD_ADDR remote_bda,
                             tBT_TRANSPORT transport, tBTM_CMPL_CB *p_cb);

/*******************************************************************************
**
** Function         BTM_ReadLinkQuality
**
** Description      This function is called to read the link quality.
**                  The value of the link quality is returned in the callback.
**                  (tBTM_LINK_QUALITY_RESULTS)
**
** Returns          BTM_CMD_STARTED if command issued to controller.
**                  BTM_NO_RESOURCES if couldn't allocate memory to issue command
**                  BTM_UNKNOWN_ADDR if no active link with bd addr specified
**                  BTM_BUSY if command is already in progress
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadLinkQuality (BD_ADDR remote_bda, tBTM_CMPL_CB *p_cb);

/*******************************************************************************
**
** Function         BTM_RegBusyLevelNotif
**
** Description      This function is called to register a callback to receive
**                  busy level change events.
**
** Returns          BTM_SUCCESS if successfully registered, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_RegBusyLevelNotif (tBTM_BL_CHANGE_CB *p_cb, UINT8 *p_level,
                                   tBTM_BL_EVENT_MASK evt_mask);

/*******************************************************************************
**
** Function         BTM_AclRegisterForChanges
**
** Description      This function is called to register a callback to receive
**                  ACL database change events, i.e. new connection or removed.
**
** Returns          BTM_SUCCESS if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_AclRegisterForChanges (tBTM_ACL_DB_CHANGE_CB *p_cb);

/*******************************************************************************
**
** Function         BTM_GetNumAclLinks
**
** Description      This function is called to count the number of
**                  ACL links that are active.
**
** Returns          UINT16  Number of active ACL links
**
*******************************************************************************/
//extern
UINT16 BTM_GetNumAclLinks (void);

/*******************************************************************************
**
** Function         BTM_SetQoS
**
** Description      This function is called to setup QoS
**
** Returns          BTM_CMD_STARTED if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetQoS(BD_ADDR bd, FLOW_SPEC *p_flow,
                       tBTM_CMPL_CB *p_cb);


/*****************************************************************************
**  (e)SCO CHANNEL MANAGEMENT FUNCTIONS
*****************************************************************************/
/*******************************************************************************
**
** Function         BTM_CreateSco
**
** Description      This function is called to create an SCO connection. If the
**                  "is_orig" flag is TRUE, the connection will be originated,
**                  otherwise BTM will wait for the other side to connect.
**
** Returns          BTM_UNKNOWN_ADDR if the ACL connection is not up
**                  BTM_BUSY         if another SCO being set up to
**                                   the same BD address
**                  BTM_NO_RESOURCES if the max SCO limit has been reached
**                  BTM_CMD_STARTED  if the connection establishment is started.
**                                   In this case, "*p_sco_inx" is filled in
**                                   with the sco index used for the connection.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_CreateSco (BD_ADDR remote_bda, BOOLEAN is_orig,
                           UINT16 pkt_types, UINT16 *p_sco_inx,
                           tBTM_SCO_CB *p_conn_cb,
                           tBTM_SCO_CB *p_disc_cb);


/*******************************************************************************
**
** Function         BTM_RemoveSco
**
** Description      This function is called to remove a specific SCO connection.
**
** Returns          BTM_CMD_STARTED if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_RemoveSco (UINT16 sco_inx);


/*******************************************************************************
**
** Function         BTM_SetScoPacketTypes
**
** Description      This function is called to set the packet types used for
**                  a specific SCO connection,
**
** Parameters       pkt_types - One or more of the following
**                  BTM_SCO_PKT_TYPES_MASK_HV1
**                  BTM_SCO_PKT_TYPES_MASK_HV2
**                  BTM_SCO_PKT_TYPES_MASK_HV3
**                  BTM_SCO_PKT_TYPES_MASK_EV3
**                  BTM_SCO_PKT_TYPES_MASK_EV4
**                  BTM_SCO_PKT_TYPES_MASK_EV5
**
**                  BTM_SCO_LINK_ALL_MASK   - enables all supported types
**
** Returns          BTM_CMD_STARTED if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetScoPacketTypes (UINT16 sco_inx, UINT16 pkt_types);


/*******************************************************************************
**
** Function         BTM_ReadScoPacketTypes
**
** Description      This function is read the packet types used for a specific
**                  SCO connection.
**
** Returns       One or more of the following (bitmask)
**                  BTM_SCO_PKT_TYPES_MASK_HV1
**                  BTM_SCO_PKT_TYPES_MASK_HV2
**                  BTM_SCO_PKT_TYPES_MASK_HV3
**                  BTM_SCO_PKT_TYPES_MASK_EV3
**                  BTM_SCO_PKT_TYPES_MASK_EV4
**                  BTM_SCO_PKT_TYPES_MASK_EV5
**
** Returns          packet types supported for the connection
**
*******************************************************************************/
//extern
UINT16 BTM_ReadScoPacketTypes (UINT16 sco_inx);


/*******************************************************************************
**
** Function         BTM_ReadDeviceScoPacketTypes
**
** Description      This function is read the SCO packet types that
**                  the device supports.
**
** Returns          packet types supported by the device.
**
*******************************************************************************/
//extern
UINT16 BTM_ReadDeviceScoPacketTypes (void);


/*******************************************************************************
**
** Function         BTM_ReadScoHandle
**
** Description      This function is used to read the HCI handle used for a specific
**                  SCO connection,
**
** Returns          handle for the connection, or 0xFFFF if invalid SCO index.
**
*******************************************************************************/
//extern
UINT16 BTM_ReadScoHandle (UINT16 sco_inx);


/*******************************************************************************
**
** Function         BTM_ReadScoBdAddr
**
** Description      This function is read the remote BD Address for a specific
**                  SCO connection,
**
** Returns          pointer to BD address or NULL if not known
**
*******************************************************************************/
//extern
UINT8 *BTM_ReadScoBdAddr (UINT16 sco_inx);


/*******************************************************************************
**
** Function         BTM_ReadScoDiscReason
**
** Description      This function is returns the reason why an (e)SCO connection
**                  has been removed. It contains the value until read, or until
**                  another (e)SCO connection has disconnected.
**
** Returns          HCI reason or BTM_INVALID_SCO_DISC_REASON if not set.
**
*******************************************************************************/
//extern
UINT16 BTM_ReadScoDiscReason (void);


/*******************************************************************************
**
** Function         BTM_SetEScoMode
**
** Description      This function sets up the negotiated parameters for SCO or
**                  eSCO, and sets as the default mode used for calls to
**                  BTM_CreateSco.  It can be called only when there are no
**                  active (e)SCO links.
**
** Returns          BTM_SUCCESS if the successful.
**                  BTM_BUSY if there are one or more active (e)SCO links.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetEScoMode (tBTM_SCO_TYPE sco_mode,
                             tBTM_ESCO_PARAMS *p_parms);

/*******************************************************************************
**
** Function         BTM_SetWBSCodec
**
** Description      This function sends command to the controller to setup
**                  WBS codec for the upcoming eSCO connection.
**
** Returns          BTM_SUCCESS.
**
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetWBSCodec (tBTM_SCO_CODEC_TYPE codec_type);

/*******************************************************************************
**
** Function         BTM_RegForEScoEvts
**
** Description      This function registers a SCO event callback with the
**                  specified instance.  It should be used to received
**                  connection indication events and change of link parameter
**                  events.
**
** Returns          BTM_SUCCESS if the successful.
**                  BTM_ILLEGAL_VALUE if there is an illegal sco_inx
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_RegForEScoEvts (UINT16 sco_inx,
                                tBTM_ESCO_CBACK *p_esco_cback);

/*******************************************************************************
**
** Function         BTM_ReadEScoLinkParms
**
** Description      This function returns the current eSCO link parameters for
**                  the specified handle.  This can be called anytime a connection
**                  is active, but is typically called after receiving the SCO
**                  opened callback.
**
**                  Note: If called over a 1.1 controller, only the packet types
**                        field has meaning.
**                  Note: If the upper layer doesn't know the current sco index,
**                  BTM_FIRST_ACTIVE_SCO_INDEX can be used as the first parameter to
**                  find the first active SCO index
**
** Returns          BTM_SUCCESS if returned data is valid connection.
**                  BTM_ILLEGAL_VALUE if no connection for specified sco_inx.
**                  BTM_MODE_UNSUPPORTED if local controller does not support
**                      1.2 specification.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadEScoLinkParms (UINT16 sco_inx,
                                   tBTM_ESCO_DATA *p_parms);

/*******************************************************************************
**
** Function         BTM_ChangeEScoLinkParms
**
** Description      This function requests renegotiation of the parameters on
**                  the current eSCO Link.  If any of the changes are accepted
**                  by the controllers, the BTM_ESCO_CHG_EVT event is sent in
**                  the tBTM_ESCO_CBACK function with the current settings of
**                  the link. The callback is registered through the call to
**                  BTM_SetEScoMode.
**
**
** Returns          BTM_CMD_STARTED if command is successfully initiated.
**                  BTM_ILLEGAL_VALUE if no connection for specified sco_inx.
**                  BTM_NO_RESOURCES - not enough resources to initiate command.
**                  BTM_MODE_UNSUPPORTED if local controller does not support
**                      1.2 specification.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ChangeEScoLinkParms (UINT16 sco_inx,
                                     tBTM_CHG_ESCO_PARAMS *p_parms);

/*******************************************************************************
**
** Function         BTM_EScoConnRsp
**
** Description      This function is called upon receipt of an (e)SCO connection
**                  request event (BTM_ESCO_CONN_REQ_EVT) to accept or reject
**                  the request. Parameters used to negotiate eSCO links.
**                  If p_parms is NULL, then values set through BTM_SetEScoMode
**                  are used.
**                  If the link type of the incoming request is SCO, then only
**                  the tx_bw, max_latency, content format, and packet_types are
**                  valid.  The hci_status parameter should be
**                  ([0x0] to accept, [0x0d..0x0f] to reject)
**
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_EScoConnRsp (UINT16 sco_inx, UINT8 hci_status,
                      tBTM_ESCO_PARAMS *p_parms);

/*******************************************************************************
**
** Function         BTM_GetNumScoLinks
**
** Description      This function returns the number of active SCO links.
**
** Returns          UINT8
**
*******************************************************************************/
//extern
UINT8 BTM_GetNumScoLinks (void);

/*****************************************************************************
**  SECURITY MANAGEMENT FUNCTIONS
*****************************************************************************/
/*******************************************************************************
**
** Function         BTM_SecRegister
**
** Description      Application manager calls this function to register for
**                  security services.  There can be one and only one application
**                  saving link keys.  BTM allows only first registration.
**
** Returns          TRUE if registered OK, else FALSE
**
*******************************************************************************/
//extern
BOOLEAN BTM_SecRegister (tBTM_APPL_INFO *p_cb_info);

/*******************************************************************************
**
** Function         BTM_SecRegisterLinkKeyNotificationCallback
**
** Description      Profiles can register to be notified when a new Link Key
**                  is generated per connection.
**
** Returns          TRUE if registered OK, else FALSE
**
*******************************************************************************/
//extern
BOOLEAN BTM_SecRegisterLinkKeyNotificationCallback(
    tBTM_LINK_KEY_CALLBACK *p_callback);

/*******************************************************************************
**
** Function         BTM_SecAddRmtNameNotifyCallback
**
** Description      Profiles can register to be notified when name of the
**                  remote device is resolved (up to BTM_SEC_MAX_RMT_NAME_CALLBACKS).
**
** Returns          TRUE if registered OK, else FALSE
**
*******************************************************************************/
//extern
BOOLEAN BTM_SecAddRmtNameNotifyCallback (tBTM_RMT_NAME_CALLBACK *p_callback);


/*******************************************************************************
**
** Function         BTM_SecDeleteRmtNameNotifyCallback
**
** Description      A profile can deregister notification when a new Link Key
**                  is generated per connection.
**
** Returns          TRUE if OK, else FALSE
**
*******************************************************************************/
//extern
BOOLEAN BTM_SecDeleteRmtNameNotifyCallback (tBTM_RMT_NAME_CALLBACK *p_callback);

/*******************************************************************************
**
** Function         BTM_GetSecurityFlags
**
** Description      Get security flags for the device
**
** Returns          BOOLEAN TRUE or FALSE is device found
**
*******************************************************************************/
//extern
BOOLEAN BTM_GetSecurityFlags (BD_ADDR bd_addr, UINT8 *p_sec_flags);

/*******************************************************************************
**
** Function         BTM_GetSecurityFlagsByTransport
**
** Description      Get security flags for the device on a particular transport
**
** Parameters      bd_addr: BD address of remote device
**                  p_sec_flags : Out parameter to be filled with security flags for the connection
**                  transport :  Physical transport of the connection (BR/EDR or LE)
**
** Returns          BOOLEAN TRUE or FALSE is device found
**
*******************************************************************************/
//extern
BOOLEAN BTM_GetSecurityFlagsByTransport (BD_ADDR bd_addr,
        UINT8 *p_sec_flags, tBT_TRANSPORT transport);

/*******************************************************************************
**
** Function         BTM_ReadTrustedMask
**
** Description      Get trusted mask for the device
**
** Returns          NULL, if the device record is not found.
**                  otherwise, the trusted mask
**
*******************************************************************************/
//extern
UINT32 *BTM_ReadTrustedMask (BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTM_SetPinType
**
** Description      Set PIN type for the device.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_SetPinType (UINT8 pin_type, PIN_CODE pin_code, UINT8 pin_code_len);


/*******************************************************************************
**
** Function         BTM_SetPairableMode
**
** Description      Enable or disable pairing
**
** Parameters       allow_pairing - (TRUE or FALSE) whether or not the device
**                      allows pairing.
**                  connect_only_paired - (TRUE or FALSE) whether or not to
**                      only allow paired devices to connect.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_SetPairableMode (BOOLEAN allow_pairing, BOOLEAN connect_only_paired);

/*******************************************************************************
**
** Function         BTM_SetSecureConnectionsOnly
**
** Description      Enable or disable default treatment for Mode 4 Level 0 services
**
** Parameter        secure_connections_only_mode - (TRUE or FALSE)
**                  TRUE means that the device should treat Mode 4 Level 0 services as
**                  services of other levels.
**                  FALSE means that the device should provide default treatment for
**                  Mode 4 Level 0 services.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_SetSecureConnectionsOnly (BOOLEAN secure_connections_only_mode);

/*******************************************************************************
**
** Function         BTM_SetSecurityLevel
**
** Description      Register service security level with Security Manager.  Each
**                  service must register its requirements regardless of the
**                  security level that is used.  This API is called once for originators
**                  nad again for acceptors of connections.
**
** Returns          TRUE if registered OK, else FALSE
**
*******************************************************************************/
//extern
BOOLEAN BTM_SetSecurityLevel (BOOLEAN is_originator, char *p_name,
                              UINT8 service_id, UINT16 sec_level,
                              UINT16 psm, UINT32 mx_proto_id,
                              UINT32 mx_chan_id);

/*******************************************************************************
**
** Function         BTM_SetOutService
**
** Description      This function is called to set the service for
**                  outgoing connection.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_SetOutService(BD_ADDR bd_addr, UINT8 service_id, UINT32 mx_chan_id);

/*******************************************************************************
**
** Function         BTM_SecClrService
**
** Description      Removes specified service record(s) from the security database.
**                  All service records with the specified name are removed.
**                  Typically used only by devices with limited RAM so that it can
**                  reuse an old security service record.
**                          records (except SDP).
**
** Returns          Number of records that were freed.
**
*******************************************************************************/
//extern
UINT8 BTM_SecClrService (UINT8 service_id);

/*******************************************************************************
**
** Function         BTM_SecAddDevice
**
** Description      Add/modify device.  This function will be normally called
**                  during host startup to restore all required information
**                  stored in the NVRAM.
**                  dev_class, bd_name, link_key, and features are NULL if unknown
**
** Returns          TRUE if added OK, else FALSE
**
*******************************************************************************/
//extern
BOOLEAN BTM_SecAddDevice (BD_ADDR bd_addr, DEV_CLASS dev_class,
                          BD_NAME bd_name, UINT8 *features,
                          UINT32 trusted_mask[], LINK_KEY link_key,
                          UINT8 key_type, tBTM_IO_CAP io_cap, UINT8 pin_length);


/*******************************************************************************
**
** Function         BTM_SecDeleteDevice
**
** Description      Free resources associated with the device.
**
** Returns          TRUE if rmoved OK, FALSE if not found
**
*******************************************************************************/
//extern
BOOLEAN BTM_SecDeleteDevice (BD_ADDR bd_addr);


/*******************************************************************************
**
** Function         BTM_SecGetDeviceLinkKey
**
** Description      This function is called to obtain link key for the device
**                  it returns BTM_SUCCESS if link key is available, or
**                  BTM_UNKNOWN_ADDR if Security Manager does not know about
**                  the device or device record does not contain link key info
**
** Returns          BTM_SUCCESS if successful, otherwise error code
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SecGetDeviceLinkKey (BD_ADDR bd_addr,
                                     LINK_KEY link_key);


/*******************************************************************************
**
** Function         BTM_SecGetDeviceLinkKeyType
**
** Description      This function is called to obtain link key type for the
**                  device.
**                  it returns BTM_SUCCESS if link key is available, or
**                  BTM_UNKNOWN_ADDR if Security Manager does not know about
**                  the device or device record does not contain link key info
**
** Returns          BTM_LKEY_TYPE_IGNORE if link key is unknown, link type
**                  otherwise.
**
*******************************************************************************/
//extern
tBTM_LINK_KEY_TYPE BTM_SecGetDeviceLinkKeyType (BD_ADDR bd_addr);


/*******************************************************************************
**
** Function         BTM_PINCodeReply
**
** Description      This function is called after Security Manager submitted
**                  PIN code request to the UI.
**
** Parameters:      bd_addr      - Address of the device for which PIN was requested
**                  res          - result of the operation BTM_SUCCESS if success
**                  pin_len      - length in bytes of the PIN Code
**                  p_pin        - pointer to array with the PIN Code
**                  trusted_mask - bitwise OR of trusted services (array of UINT32)
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_PINCodeReply (BD_ADDR bd_addr, UINT8 res, UINT8 pin_len,
                       UINT8 *p_pin, UINT32 trusted_mask[]);


/*******************************************************************************
**
** Function         BTM_SecBond
**
** Description      This function is called to perform bonding with peer device.
**
** Parameters:      bd_addr      - Address of the device to bond
**                  pin_len      - length in bytes of the PIN Code
**                  p_pin        - pointer to array with the PIN Code
**                  trusted_mask - bitwise OR of trusted services (array of UINT32)

** Returns          BTM_CMD_STARTED if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SecBond (BD_ADDR bd_addr,
                         UINT8 pin_len, UINT8 *p_pin,
                         UINT32 trusted_mask[]);

/*******************************************************************************
**
** Function         BTM_SecBondByTransport
**
** Description      This function is called to perform bonding by designated transport
**
** Parameters:      bd_addr      - Address of the device to bond
**                  pin_len      - length in bytes of the PIN Code
**                  p_pin        - pointer to array with the PIN Code
**                  trusted_mask - bitwise OR of trusted services (array of UINT32)
**                  transport :  Physical transport to use for bonding (BR/EDR or LE)
**
** Returns          BTM_CMD_STARTED if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SecBondByTransport (BD_ADDR bd_addr,
                                    tBT_TRANSPORT transport,
                                    UINT8 pin_len, UINT8 *p_pin,
                                    UINT32 trusted_mask[]);

/*******************************************************************************
**
** Function         BTM_SecBondCancel
**
** Description      This function is called to cancel ongoing bonding process
**                  with peer device.
**
** Returns          BTM_CMD_STARTED if successfully initiated, otherwise error
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SecBondCancel (BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTM_SetEncryption
**
** Description      This function is called to ensure that connection is
**                  encrypted.  Should be called only on an open connection.
**                  Typically only needed for connections that first want to
**                  bring up unencrypted links, then later encrypt them.
**
** Parameters:      bd_addr       - Address of the peer device
**                  p_callback    - Pointer to callback function called if
**                                  this function returns PENDING after required
**                                  procedures are completed.  Can be set to NULL
**                                  if status is not desired.
**                  p_ref_data    - pointer to any data the caller wishes to receive
**                                  in the callback function upon completion.
*                                   can be set to NULL if not used.
**
** Returns          BTM_SUCCESS   - already encrypted
**                  BTM_PENDING   - command will be returned in the callback
**                  BTM_WRONG_MODE- connection not up.
**                  BTM_BUSY      - security procedures are currently active
**                  BTM_MODE_UNSUPPORTED - if security manager not linked in.
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetEncryption (BD_ADDR bd_addr, tBT_TRANSPORT transport,
                               tBTM_SEC_CBACK *p_callback, void *p_ref_data);

/*******************************************************************************
**
** Function         BTM_ConfirmReqReply
**
** Description      This function is called to confirm the numeric value for
**                  Simple Pairing in response to BTM_SP_CFM_REQ_EVT
**
** Parameters:      res           - result of the operation BTM_SUCCESS if success
**                  bd_addr       - Address of the peer device
**
*******************************************************************************/
//extern
void BTM_ConfirmReqReply(tBTM_STATUS res, BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTM_PasskeyReqReply
**
** Description      This function is called to provide the passkey for
**                  Simple Pairing in response to BTM_SP_KEY_REQ_EVT
**
** Parameters:      res           - result of the operation BTM_SUCCESS if success
**                  bd_addr       - Address of the peer device
**                  passkey       - numeric value in the range of 0 - 999999(0xF423F).
**
*******************************************************************************/
//extern
void BTM_PasskeyReqReply(tBTM_STATUS res, BD_ADDR bd_addr, UINT32 passkey);

/*******************************************************************************
**
** Function         BTM_SendKeypressNotif
**
** Description      This function is used during the passkey entry model
**                  by a device with KeyboardOnly IO capabilities
**                  (very likely to be a HID Device).
**                  It is called by a HID Device to inform the remote device when
**                  a key has been entered or erased.
**
** Parameters:      bd_addr - Address of the peer device
**                  type - notification type
**
*******************************************************************************/
//extern
void BTM_SendKeypressNotif(BD_ADDR bd_addr, tBTM_SP_KEY_TYPE type);

/*******************************************************************************
**
** Function         BTM_IoCapRsp
**
** Description      This function is called in response to BTM_SP_IO_REQ_EVT
**                  When the event data io_req.oob_data is set to BTM_OOB_UNKNOWN
**                  by the tBTM_SP_CALLBACK implementation, this function is
**                  called to provide the actual response
**
** Parameters:      bd_addr - Address of the peer device
**                  io_cap  - The IO capability of local device.
**                  oob     - BTM_OOB_NONE or BTM_OOB_PRESENT.
**                  auth_req- MITM protection required or not.
**
*******************************************************************************/
//extern
void BTM_IoCapRsp(BD_ADDR bd_addr, tBTM_IO_CAP io_cap,
                  tBTM_OOB_DATA oob, tBTM_AUTH_REQ auth_req);

/*******************************************************************************
**
** Function         BTM_ReadLocalOobData
**
** Description      This function is called to read the local OOB data from
**                  LM
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadLocalOobData(void);

/*******************************************************************************
**
** Function         BTM_RemoteOobDataReply
**
** Description      This function is called to provide the remote OOB data for
**                  Simple Pairing in response to BTM_SP_RMT_OOB_EVT
**
** Parameters:      bd_addr     - Address of the peer device
**                  c           - simple pairing Hash C.
**                  r           - simple pairing Randomizer  C.
**
*******************************************************************************/
//extern
void BTM_RemoteOobDataReply(tBTM_STATUS res, BD_ADDR bd_addr,
                            BT_OCTET16 c, BT_OCTET16 r);

/*******************************************************************************
**
** Function         BTM_BuildOobData
**
** Description      This function is called to build the OOB data payload to
**                  be sent over OOB (non-Bluetooth) link
**
** Parameters:      p_data  - the location for OOB data
**                  max_len - p_data size.
**                  c       - simple pairing Hash C.
**                  r       - simple pairing Randomizer  C.
**                  name_len- 0, local device name would not be included.
**                            otherwise, the local device name is included for
**                            up to this specified length
**
** Returns          Number of bytes in p_data.
**
*******************************************************************************/
//extern
UINT16 BTM_BuildOobData(UINT8 *p_data, UINT16 max_len, BT_OCTET16 c,
                        BT_OCTET16 r, UINT8 name_len);

/*******************************************************************************
**
** Function         BTM_BothEndsSupportSecureConnections
**
** Description      This function is called to check if both the local device and the peer device
**                   specified by bd_addr support BR/EDR Secure Connections.
**
** Parameters:      bd_addr - address of the peer
**
** Returns          TRUE if BR/EDR Secure Connections are supported by both local
**                  and the remote device.
**                  else FALSE.
**
*******************************************************************************/
//extern
BOOLEAN BTM_BothEndsSupportSecureConnections(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTM_PeerSupportsSecureConnections
**
** Description      This function is called to check if the peer supports
**                  BR/EDR Secure Connections.
**
** Parameters:      bd_addr - address of the peer
**
** Returns          TRUE if BR/EDR Secure Connections are supported by the peer,
**                  else FALSE.
**
*******************************************************************************/
//extern
BOOLEAN BTM_PeerSupportsSecureConnections(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTM_ReadOobData
**
** Description      This function is called to parse the OOB data payload
**                  received over OOB (non-Bluetooth) link
**
** Parameters:      p_data  - the location for OOB data
**                  eir_tag - The associated EIR tag to read the data.
**                  *p_len(output) - the length of the data with the given tag.
**
** Returns          the beginning of the data with the given tag.
**                  NULL, if the tag is not found.
**
*******************************************************************************/
//extern
UINT8 *BTM_ReadOobData(UINT8 *p_data, UINT8 eir_tag, UINT8 *p_len);

/*******************************************************************************
**
** Function         BTM_SecReadDevName
**
** Description      Looks for the device name in the security database for the
**                  specified BD address.
**
** Returns          Pointer to the name or NULL
**
*******************************************************************************/
//extern
char *BTM_SecReadDevName (BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         BTM_SecClearSecurityFlags
**
** Description      Reset the security flags (mark as not-paired) for a given
**                  remove device.
**
*******************************************************************************/
extern void BTM_SecClearSecurityFlags (BD_ADDR bd_addr);



/*****************************************************************************
**  POWER MANAGEMENT FUNCTIONS
*****************************************************************************/
/*******************************************************************************
**
** Function         BTM_PmRegister
**
** Description      register or deregister with power manager
**
** Returns          BTM_SUCCESS if successful,
**                  BTM_NO_RESOURCES if no room to hold registration
**                  BTM_ILLEGAL_VALUE
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_PmRegister (UINT8 mask, UINT8 *p_pm_id,
                            tBTM_PM_STATUS_CBACK *p_cb);


/*******************************************************************************
**
** Function         BTM_SetPowerMode
**
** Description      store the mode in control block or
**                  alter ACL connection behavior.
**
** Returns          BTM_SUCCESS if successful,
**                  BTM_UNKNOWN_ADDR if bd addr is not active or bad
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetPowerMode (UINT8 pm_id, BD_ADDR remote_bda,
                              tBTM_PM_PWR_MD *p_mode);


/*******************************************************************************
**
** Function         BTM_ReadPowerMode
**
** Description      This returns the current mode for a specific
**                  ACL connection.
**
** Input Param      remote_bda - device address of desired ACL connection
**
** Output Param     p_mode - address where the current mode is copied into.
**                          BTM_ACL_MODE_NORMAL
**                          BTM_ACL_MODE_HOLD
**                          BTM_ACL_MODE_SNIFF
**                          BTM_ACL_MODE_PARK
**                          (valid only if return code is BTM_SUCCESS)
**
** Returns          BTM_SUCCESS if successful,
**                  BTM_UNKNOWN_ADDR if bd addr is not active or bad
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ReadPowerMode (BD_ADDR remote_bda,
                               tBTM_PM_MODE *p_mode);

/*******************************************************************************
**
** Function         BTM_SetSsrParams
**
** Description      This sends the given SSR parameters for the given ACL
**                  connection if it is in ACTIVE mode.
**
** Input Param      remote_bda - device address of desired ACL connection
**                  max_lat    - maximum latency (in 0.625ms)(0-0xFFFE)
**                  min_rmt_to - minimum remote timeout
**                  min_loc_to - minimum local timeout
**
**
** Returns          BTM_SUCCESS if the HCI command is issued successful,
**                  BTM_UNKNOWN_ADDR if bd addr is not active or bad
**                  BTM_CMD_STORED if the command is stored
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_SetSsrParams (BD_ADDR remote_bda, UINT16 max_lat,
                              UINT16 min_rmt_to, UINT16 min_loc_to);

/*******************************************************************************
**
** Function         BTM_GetHCIConnHandle
**
** Description      This function is called to get the handle for an ACL connection
**                  to a specific remote BD Address.
**
** Returns          the handle of the connection, or 0xFFFF if none.
**
*******************************************************************************/
//extern
UINT16 BTM_GetHCIConnHandle (BD_ADDR remote_bda, tBT_TRANSPORT transport);

/*******************************************************************************
**
** Function         BTM_DeleteStoredLinkKey
**
** Description      This function is called to delete link key for the specified
**                  device addresses from the NVRAM storage attached to the Bluetooth
**                  controller.
**
** Parameters:      bd_addr      - Addresses of the devices
**                  p_cb         - Call back function to be called to return
**                                 the results
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_DeleteStoredLinkKey(BD_ADDR bd_addr, tBTM_CMPL_CB *p_cb);

/*******************************************************************************
**
** Function         BTM_WriteEIR
**
** Description      This function is called to write EIR data to controller.
**
** Parameters       p_buff - allocated HCI command buffer including extended
**                           inquriry response
**
** Returns          BTM_SUCCESS  - if successful
**                  BTM_MODE_UNSUPPORTED - if local device cannot support it
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_WriteEIR( BT_HDR *p_buff );

/*******************************************************************************
**
** Function         BTM_CheckEirData
**
** Description      This function is called to get EIR data from significant part.
**
** Parameters       p_eir - pointer of EIR significant part
**                  type   - finding EIR data type
**                  p_length - return the length of EIR data
**
** Returns          pointer of EIR data
**
*******************************************************************************/
//extern
UINT8 *BTM_CheckEirData( UINT8 *p_eir, UINT8 type, UINT8 *p_length );

/*******************************************************************************
**
** Function         BTM_HasEirService
**
** Description      This function is called to know if UUID in bit map of UUID.
**
** Parameters       p_eir_uuid - bit map of UUID list
**                  uuid16 - UUID 16-bit
**
** Returns          TRUE - if found
**                  FALSE - if not found
**
*******************************************************************************/
//extern
BOOLEAN BTM_HasEirService( UINT32 *p_eir_uuid, UINT16 uuid16 );

/*******************************************************************************
**
** Function         BTM_HasInquiryEirService
**
** Description      This function is called to know if UUID in bit map of UUID list.
**
** Parameters       p_results - inquiry results
**                  uuid16 - UUID 16-bit
**
** Returns          BTM_EIR_FOUND - if found
**                  BTM_EIR_NOT_FOUND - if not found and it is complete list
**                  BTM_EIR_UNKNOWN - if not found and it is not complete list
**
*******************************************************************************/
//extern
tBTM_EIR_SEARCH_RESULT BTM_HasInquiryEirService( tBTM_INQ_RESULTS *p_results,
        UINT16 uuid16 );

/*******************************************************************************
**
** Function         BTM_AddEirService
**
** Description      This function is called to add a service in bit map of UUID list.
**
** Parameters       p_eir_uuid - bit mask of UUID list for EIR
**                  uuid16 - UUID 16-bit
**
** Returns          None
**
*******************************************************************************/
//extern
void BTM_AddEirService( UINT32 *p_eir_uuid, UINT16 uuid16 );

/*******************************************************************************
**
** Function         BTM_RemoveEirService
**
** Description      This function is called to remove a service in bit map of UUID list.
**
** Parameters       p_eir_uuid - bit mask of UUID list for EIR
**                  uuid16 - UUID 16-bit
**
** Returns          None
**
*******************************************************************************/
//extern
void BTM_RemoveEirService( UINT32 *p_eir_uuid, UINT16 uuid16 );

/*******************************************************************************
**
** Function         BTM_GetEirSupportedServices
**
** Description      This function is called to get UUID list from bit map of UUID list.
**
** Parameters       p_eir_uuid - bit mask of UUID list for EIR
**                  p - reference of current pointer of EIR
**                  max_num_uuid16 - max number of UUID can be written in EIR
**                  num_uuid16 - number of UUID have been written in EIR
**
** Returns          BTM_EIR_MORE_16BITS_UUID_TYPE, if it has more than max
**                  BTM_EIR_COMPLETE_16BITS_UUID_TYPE, otherwise
**
*******************************************************************************/
//extern
UINT8 BTM_GetEirSupportedServices( UINT32 *p_eir_uuid,    UINT8 **p,
                                   UINT8  max_num_uuid16, UINT8 *p_num_uuid16);

/*******************************************************************************
**
** Function         BTM_GetEirUuidList
**
** Description      This function parses EIR and returns UUID list.
**
** Parameters       p_eir - EIR
**                  uuid_size - LEN_UUID_16, LEN_UUID_32, LEN_UUID_128
**                  p_num_uuid - return number of UUID in found list
**                  p_uuid_list - return UUID 16-bit list
**                  max_num_uuid - maximum number of UUID to be returned
**
** Returns          0 - if not found
**                  BTM_EIR_COMPLETE_16BITS_UUID_TYPE
**                  BTM_EIR_MORE_16BITS_UUID_TYPE
**                  BTM_EIR_COMPLETE_32BITS_UUID_TYPE
**                  BTM_EIR_MORE_32BITS_UUID_TYPE
**                  BTM_EIR_COMPLETE_128BITS_UUID_TYPE
**                  BTM_EIR_MORE_128BITS_UUID_TYPE
**
*******************************************************************************/
//extern
UINT8 BTM_GetEirUuidList( UINT8 *p_eir, UINT8 uuid_size, UINT8 *p_num_uuid,
                          UINT8 *p_uuid_list, UINT8 max_num_uuid);

/*****************************************************************************
**  SCO OVER HCI
*****************************************************************************/
/*******************************************************************************
**
** Function         BTM_ConfigScoPath
**
** Description      This function enable/disable SCO over HCI and registers SCO
**                  data callback if SCO over HCI is enabled.
**
** Parameter        path: SCO or HCI
**                  p_sco_data_cb: callback function or SCO data if path is set
**                                 to transport.
**                  p_pcm_param: pointer to the PCM interface parameter. If a NULL
**                               pointer is used, PCM parameter maintained in
**                               the control block will be used; otherwise update
**                               control block value.
**                  err_data_rpt: Lisbon feature to enable the erronous data report
**                                or not.
**
** Returns          BTM_SUCCESS if the successful.
**                  BTM_NO_RESOURCES: no rsource to start the command.
**                  BTM_ILLEGAL_VALUE: invalid callback function pointer.
**                  BTM_CMD_STARTED :Command sent. Waiting for command cmpl event.
**
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_ConfigScoPath (tBTM_SCO_ROUTE_TYPE path,
                               tBTM_SCO_DATA_CB *p_sco_data_cb,
                               tBTM_SCO_PCM_PARAM *p_pcm_param,
                               BOOLEAN err_data_rpt);

/*******************************************************************************
**
** Function         BTM_WriteScoData
**
** Description      This function write SCO data to a specified instance. The data
**                  to be written p_buf needs to carry an offset of
**                  HCI_SCO_PREAMBLE_SIZE bytes, and the data length can not
**                  exceed BTM_SCO_DATA_SIZE_MAX bytes, whose default value is set
**                  to 60 and is configurable. Data longer than the maximum bytes
**                  will be truncated.
**
** Returns          BTM_SUCCESS: data write is successful
**                  BTM_ILLEGAL_VALUE: SCO data contains illegal offset value.
**                  BTM_SCO_BAD_LENGTH: SCO data length exceeds the max SCO packet
**                                      size.
**                  BTM_NO_RESOURCES: no resources.
**                  BTM_UNKNOWN_ADDR: unknown SCO connection handle, or SCO is not
**                                    routed via HCI.
**
**
*******************************************************************************/
//extern
tBTM_STATUS BTM_WriteScoData (UINT16 sco_inx, BT_HDR *p_buf);

/*******************************************************************************
**
** Function         BTM_SetARCMode
**
** Description      Send Audio Routing Control command.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_SetARCMode (UINT8 iface, UINT8 arc_mode, tBTM_VSC_CMPL_CB *p_arc_cb);


/*******************************************************************************
**
** Function         BTM_PCM2Setup_Write
**
** Description      Send PCM2_Setup write command.
**
** Returns          void
**
*******************************************************************************/
//extern
void BTM_PCM2Setup_Write (BOOLEAN clk_master, tBTM_VSC_CMPL_CB *p_arc_cb);


/*******************************************************************************
**
** Function         BTM_PM_ReadControllerState
**
** Description      This function is called to obtain the controller state
**
** Returns          Controller state (BTM_CONTRL_ACTIVE, BTM_CONTRL_SCAN, and BTM_CONTRL_IDLE)
**
*******************************************************************************/
//extern
tBTM_CONTRL_STATE BTM_PM_ReadControllerState(void);
/*
#ifdef __cplusplus
}
#endif
*/

#endif /* BTM_API_H */
