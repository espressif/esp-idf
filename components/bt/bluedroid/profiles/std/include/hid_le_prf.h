/**
 ****************************************************************************************
 *
 * @file button_pro.c
 *
 * @brief Application entry point
 *
 * Copyright (C) Espressif 2016
 * Created by Yulong at 2016/9/7
 *
 *
 ****************************************************************************************
 */

#include "prf_defs.h"

#if (HIDD_LE_PROFILE_CFG)
#include "bta_gatts_int.h"
#include "bt_types.h"
#include "bta_api.h"
#include "gatt_api.h"

/// Maximal number of HIDS that can be added in the DB
#ifndef USE_ONE_HIDS_INSTANCE
#define HIDD_LE_NB_HIDS_INST_MAX              (2)
#else
#define HIDD_LE_NB_HIDS_INST_MAX              (1)
#endif

// Number of HID reports defined in the service
#define HID_NUM_REPORTS          9


#define ATT_SVC_HID 	0x1812

/// Maximal number of Report Char. that can be added in the DB for one HIDS - Up to 11
#define HIDD_LE_NB_REPORT_INST_MAX            (5)

/// Maximal length of Report Char. Value
#define HIDD_LE_REPORT_MAX_LEN                (45)
/// Maximal length of Report Map Char. Value
#define HIDD_LE_REPORT_MAP_MAX_LEN            (512)

/// Length of Boot Report Char. Value Maximal Length
#define HIDD_LE_BOOT_REPORT_MAX_LEN           (8)

/// Boot KB Input Report Notification Configuration Bit Mask
#define HIDD_LE_BOOT_KB_IN_NTF_CFG_MASK       (0x40)
/// Boot KB Input Report Notification Configuration Bit Mask
#define HIDD_LE_BOOT_MOUSE_IN_NTF_CFG_MASK    (0x80)
/// Boot Report Notification Configuration Bit Mask
#define HIDD_LE_REPORT_NTF_CFG_MASK           (0x20)


/* HID information flags */
#define HID_FLAGS_REMOTE_WAKE           0x01      // RemoteWake
#define HID_FLAGS_NORMALLY_CONNECTABLE  0x02      // NormallyConnectable

/* Control point commands */
#define HID_CMD_SUSPEND                 0x00      // Suspend
#define HID_CMD_EXIT_SUSPEND            0x01      // Exit Suspend

/* HID protocol mode values */
#define HID_PROTOCOL_MODE_BOOT          0x00      // Boot Protocol Mode
#define HID_PROTOCOL_MODE_REPORT        0x01      // Report Protocol Mode

/* Attribute value lengths */
#define HID_PROTOCOL_MODE_LEN           1         // HID Protocol Mode
#define HID_INFORMATION_LEN             4         // HID Information
#define HID_REPORT_REF_LEN              2         // HID Report Reference Descriptor
#define HID_EXT_REPORT_REF_LEN          2         // External Report Reference Descriptor

// HID feature flags
#define HID_KBD_FLAGS             HID_FLAGS_REMOTE_WAKE


/// HID Service Attributes Indexes
enum
{
    HIDD_LE_IDX_SVC,

    // Included Service
    HIDD_LE_IDX_INCL_SVC,

    // HID Information
    HIDD_LE_IDX_HID_INFO_CHAR,
    HIDD_LE_IDX_HID_INFO_VAL,

    // HID Control Point
    HIDD_LE_IDX_HID_CTNL_PT_CHAR,
    HIDD_LE_IDX_HID_CTNL_PT_VAL,

    // Report Map
    HIDD_LE_IDX_REPORT_MAP_CHAR,
    HIDD_LE_IDX_REPORT_MAP_VAL,
    HIDD_LE_IDX_REPORT_MAP_EXT_REP_REF,

    // Protocol Mode
    HIDD_LE_IDX_PROTO_MODE_CHAR,
    HIDD_LE_IDX_PROTO_MODE_VAL,

    // Boot Keyboard Input Report
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_VAL,
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_NTF_CFG,

    // Boot Keyboard Output Report
    HIDD_LE_IDX_BOOT_KB_OUT_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_KB_OUT_REPORT_VAL,

    // Boot Mouse Input Report
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_VAL,
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_NTF_CFG,

    // Report
    HIDD_LE_IDX_REPORT_CHAR,
    HIDD_LE_IDX_REPORT_VAL,
    HIDD_LE_IDX_REPORT_REP_REF,
    HIDD_LE_IDX_REPORT_NTF_CFG,

    HIDD_LE_IDX_NB,
};


/// Attribute Table Indexes
enum
{
    HIDD_LE_INFO_CHAR,
    HIDD_LE_CTNL_PT_CHAR,
    HIDD_LE_REPORT_MAP_CHAR,
    HIDD_LE_REPORT_CHAR,
    HIDD_LE_PROTO_MODE_CHAR,
    HIDD_LE_BOOT_KB_IN_REPORT_CHAR,
    HIDD_LE_BOOT_KB_OUT_REPORT_CHAR,
    HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR,
    HIDD_LE_CHAR_MAX //= HIDD_LE_REPORT_CHAR + HIDD_LE_NB_REPORT_INST_MAX,
};

///att read event table Indexs
enum
{
	HIDD_LE_READ_INFO_EVT,
	HIDD_LE_READ_CTNL_PT_EVT,
	HIDD_LE_READ_REPORT_MAP_EVT,
	HIDD_LE_READ_REPORT_EVT,
	HIDD_LE_READ_PROTO_MODE_EVT,
	HIDD_LE_BOOT_KB_IN_REPORT_EVT,
	HIDD_LE_BOOT_KB_OUT_REPORT_EVT,
	HIDD_LE_BOOT_MOUSE_IN_REPORT_EVT,

	HID_LE_EVT_MAX
};

/// Client Characteristic Configuration Codes
enum
{
    HIDD_LE_DESC_MASK = 0x10,

    HIDD_LE_BOOT_KB_IN_REPORT_CFG     = HIDD_LE_BOOT_KB_IN_REPORT_CHAR | HIDD_LE_DESC_MASK,
    HIDD_LE_BOOT_MOUSE_IN_REPORT_CFG  = HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR | HIDD_LE_DESC_MASK,
    HIDD_LE_REPORT_CFG                = HIDD_LE_REPORT_CHAR | HIDD_LE_DESC_MASK,
};

/// Features Flag Values
enum
{
    HIDD_LE_CFG_KEYBOARD      = 0x01,
    HIDD_LE_CFG_MOUSE         = 0x02,
    HIDD_LE_CFG_PROTO_MODE    = 0x04,
    HIDD_LE_CFG_MAP_EXT_REF   = 0x08,
    HIDD_LE_CFG_BOOT_KB_WR    = 0x10,
    HIDD_LE_CFG_BOOT_MOUSE_WR = 0x20,
};

/// Report Char. Configuration Flag Values
enum
{
    HIDD_LE_CFG_REPORT_IN     = 0x01,
    HIDD_LE_CFG_REPORT_OUT    = 0x02,
    //HOGPD_CFG_REPORT_FEAT can be used as a mask to check Report type
    HIDD_LE_CFG_REPORT_FEAT   = 0x03,
    HIDD_LE_CFG_REPORT_WR     = 0x10,
};

/// Pointer to the connection clean-up function
#define HIDD_LE_CLEANUP_FNCT        (NULL)

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

 /// HIDD Features structure
 typedef struct 
 {
	 /// Service Features
	 UINT8 svc_features;
	 /// Number of Report Char. instances to add in the database
	 UINT8 report_nb;
	 /// Report Char. Configuration
	 UINT8 report_char_cfg[HIDD_LE_NB_REPORT_INST_MAX];
 }tHIDD_FEATURE;


 typedef struct
 {
	 BOOLEAN		 in_use;
	 BOOLEAN		 congest;
	 UINT16 		 conn_id;
	 BOOLEAN		 connected;
	 BD_ADDR		 remote_bda;
	 UINT32 		 trans_id;
	 UINT8			 cur_srvc_id;
 
 }tHIDD_CLCB;

 // HID report mapping table
typedef struct
{
  UINT16    handle;           // Handle of report characteristic
  UINT16    cccdHandle;       // Handle of CCCD for report characteristic
  UINT8     id;               // Report ID
  UINT8     type;             // Report type
  UINT8     mode;             // Protocol mode (report or boot)
} hidRptMap_t;


 typedef struct
 {
 	/// hidd profile id
	UINT8 app_id;
	 /// Notified handle
    UINT16 ntf_handle;
	///Attribute handle Table
    UINT16 att_tbl[HIDD_LE_CHAR_MAX];
	/// Supported Features
	tHIDD_FEATURE	 hidd_feature[HIDD_LE_NB_HIDS_INST_MAX];
	/// Current Protocol Mode
    UINT8 proto_mode[HIDD_LE_NB_HIDS_INST_MAX];
    /// Number of HIDS added in the database
    UINT8 hids_nb;
 	UINT8 pending_evt;
	UINT16 pending_hal;
 }tHIDD_INST;


 /* service engine control block */
 typedef struct
 {
	 tHIDD_CLCB				 hidd_clcb;			 /* connection link*/
	 tGATT_IF				 gatt_if;
	 BOOLEAN				 enabled;
	 BOOLEAN				 is_primery;
	 tHIDD_INST				 hidd_inst;
	 UINT8					 inst_id;
 }tHIDD_LE_ENV;

 extern tHIDD_LE_ENV hidd_le_env;


 void hidd_le_CreateService(BOOLEAN is_primary);

 void Hidd_Rsp (UINT32 trans_id, UINT16 conn_id, UINT8 app_id,
    tGATT_STATUS status, UINT8 event, tGATTS_DATA *p_rsp);

 void hidd_read_attr_value(tGATTS_DATA *p_data, UINT32 trans_id);

 
 tGATT_STATUS hidd_le_init (void);


#endif	///HIDD_LE_PROFILE_CFG
 

