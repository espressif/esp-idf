#ifndef __ESP_BT_DEFS_H__
#define	__ESP_BT_DEFS_H__


#include "bta_api.h"
#include "btm_ble_api.h"
#include "bta_gatt_api.h"

#define ESP_BLE_CONN_PARAM_UNDEF        0xffff      /* use this value when a specific value not to be overwritten */

#define ESP_BLE_ADV_CHNL_MAP (ESP_BLE_ADV_CHNL_37|ESP_BLE_ADV_CHNL_38|ESP_BLE_ADV_CHNL_39)

/* advertising channel map */
#define ESP_BLE_ADV_CHNL_37    (0x01 << 0)
#define ESP_BLE_ADV_CHNL_38    (0x01 << 1)
#define ESP_BLE_ADV_CHNL_39    (0x01 << 2)

typedef tBTA_GATT_STATUS					esp_gatt_status_t;






typedef	UINT16							gatt_size_t;
typedef	tBT_UUID						esp_bt_uuid_t;
typedef tBTA_GATT_ID					esp_gatt_id_t;

typedef tBTA_BLE_AD_MASK				esp_ble_ad_mask_t;
typedef	tGATT_IF 						esp_gatt_if_t;
typedef tBTA_GATT_SRVC_ID				esp_gatt_srvc_id_t;
typedef tBTA_GATTC_CHAR_ID				esp_gattc_char_id_t;
typedef tBTA_GATTC_CHAR_DESCR_ID		esp_gattc_char_descr_id_t;
typedef tBTA_GATTC_INCL_SVC_ID			esp_gattc_incl_srvc_id_t;
typedef tBTA_GATT_AUTH_REQ				esp_gatt_auth_req_t;
typedef tBTA_GATTC_MULTI				esp_gattc_multi_t;
typedef tBTA_GATTC_WRITE_TYPE			esp_gattc_write_type_t;
typedef tBTA_GATT_UNFMT					esp_gatt_unfmt_t;
typedef tBTA_GATT_PERM					esp_gatt_perm_t;
typedef	tBTA_GATT_CHAR_PROP				esp_gatt_char_prop_t;
typedef tBTA_GATTC						esp_gattc_t;

typedef tBTA_GATTC_EVT					esp_gattc_evt_t;

typedef tBTA_GATTC_IF					esp_gattc_if_t;



/* Client callback function events */
#define ESP_GATTC_REG_EVT           0   /* GATT client is registered. */
#define ESP_GATTC_DEREG_EVT         1   /* GATT client deregistered event */
#define ESP_GATTC_OPEN_EVT          2   /* GATTC open request status  event */
#define ESP_GATTC_READ_CHAR_EVT     3   /* GATT read characteristic event */
#define ESP_GATTC_WRITE_CHAR_EVT    4   /* GATT write characteristic or char descriptor event */
#define ESP_GATTC_CLOSE_EVT         5   /* GATTC  close request status event */
#define ESP_GATTC_SEARCH_CMPL_EVT   6   /* GATT discovery complete event */
#define ESP_GATTC_SEARCH_RES_EVT    7   /* GATT discovery result event */
#define ESP_GATTC_READ_DESCR_EVT    8   /* GATT read characterisitc descriptor event */
#define ESP_GATTC_WRITE_DESCR_EVT   9   /* GATT write characteristic descriptor event */
#define ESP_GATTC_NOTIF_EVT         10  /* GATT attribute notification event */
#define ESP_GATTC_PREP_WRITE_EVT    11  /* GATT prepare write  event */
#define ESP_GATTC_EXEC_EVT          12  /* execute write complete event */
#define ESP_GATTC_ACL_EVT           13  /* ACL up event */
#define ESP_GATTC_CANCEL_OPEN_EVT   14  /* cancel open event */
#define ESP_GATTC_SRVC_CHG_EVT      15  /* service change event */
#define ESP_GATTC_ENC_CMPL_CB_EVT   17  /* encryption complete callback event */
#define ESP_GATTC_CFG_MTU_EVT       18  /* configure MTU complete event */
#define ESP_GATTC_ADV_DATA_EVT      19  /* ADV data event */
#define ESP_GATTC_MULT_ADV_ENB_EVT  20  /* Enable Multi ADV event */
#define ESP_GATTC_MULT_ADV_UPD_EVT  21  /* Update parameter event */
#define ESP_GATTC_MULT_ADV_DATA_EVT 22  /* Multi ADV data event */
#define ESP_GATTC_MULT_ADV_DIS_EVT  23  /* Disable Multi ADV event */
#define ESP_GATTC_CONGEST_EVT       24  /* Congestion event */
#define ESP_GATTC_BTH_SCAN_ENB_EVT  25 /* Enable batch scan event */
#define ESP_GATTC_BTH_SCAN_CFG_EVT  26 /* Config storage event */
#define ESP_GATTC_BTH_SCAN_RD_EVT   27 /* Batch scan reports read event */
#define ESP_GATTC_BTH_SCAN_THR_EVT  28 /* Batch scan threshold event */
#define ESP_GATTC_BTH_SCAN_PARAM_EVT 29 /* Batch scan param event */
#define ESP_GATTC_BTH_SCAN_DIS_EVT  30 /* Disable batch scan event */
#define ESP_GATTC_SCAN_FLT_CFG_EVT  31 /* Scan filter config event */
#define ESP_GATTC_SCAN_FLT_PARAM_EVT 32 /* Param filter event */
#define ESP_GATTC_SCAN_FLT_STATUS_EVT 33 /* Filter status event */
#define ESP_GATTC_ADV_VSC_EVT         34 /* ADV VSC event */


/* GATT Server Data Structure */
/* Server callback function events */
#define ESP_GATTS_REG_EVT                               0
#define ESP_GATTS_READ_EVT                              GATTS_REQ_TYPE_READ         /* 1 */
#define ESP_GATTS_WRITE_EVT                             GATTS_REQ_TYPE_WRITE        /* 2 */
#define ESP_GATTS_EXEC_WRITE_EVT                        GATTS_REQ_TYPE_WRITE_EXEC   /* 3 */
#define ESP_GATTS_MTU_EVT                               GATTS_REQ_TYPE_MTU          /* 4 */
#define ESP_GATTS_CFM_EVT                               GATTS_REQ_TYPE_CONF         /* 5 */
#define ESP_GATTS_DEREG_EVT                             6
#define ESP_GATTS_CREATE_EVT                            7
#define ESP_GATTS_ADD_INCL_SRVC_EVT                     8
#define ESP_GATTS_ADD_CHAR_EVT                          9
#define ESP_GATTS_ADD_CHAR_DESCR_EVT                    10
#define ESP_GATTS_DELELTE_EVT                           11
#define ESP_GATTS_START_EVT                             12
#define ESP_GATTS_STOP_EVT                              13
#define ESP_GATTS_CONNECT_EVT                           14
#define ESP_GATTS_DISCONNECT_EVT                        15
#define ESP_GATTS_OPEN_EVT                              16
#define ESP_GATTS_CANCEL_OPEN_EVT                       17
#define ESP_GATTS_CLOSE_EVT                             18
#define ESP_GATTS_CONGEST_EVT                           20

/* Attribute permissions
*/
#define ESP_GATT_PERM_READ					BTA_GATT_PERM_READ                    /* bit 0 -  0x0001 */
#define ESP_GATT_PERM_READ_ENCRYPTED		BTA_GATT_PERM_READ_ENCRYPTED          /* bit 1 -  0x0002 */
#define ESP_GATT_PERM_READ_ENC_MITM		 	BTA_GATT_PERM_READ_ENC_MITM           /* bit 2 -  0x0004 */
#define ESP_GATT_PERM_WRITE					BTA_GATT_PERM_WRITE                   /* bit 4 -  0x0010 */
#define ESP_GATT_PERM_WRITE_ENCRYPTED		BTA_GATT_PERM_WRITE_ENCRYPTED      	  /* bit 5 -  0x0020 */
#define ESP_GATT_PERM_WRITE_ENC_MITM		BTA_GATT_PERM_WRITE_ENC_MITM          /* bit 6 -  0x0040 */
#define ESP_GATT_PERM_WRITE_SIGNED			BTA_GATT_PERM_WRITE_SIGNED            /* bit 7 -  0x0080 */
#define ESP_GATT_PERM_WRITE_SIGNED_MITM		BTA_GATT_PERM_WRITE_SIGNED_MITM 	  /* bit 8 -  0x0100 */

#define ESP_GATTS_INVALID_APP   0xff

#define ESP_GATTS_INVALID_IF    0

/* definition of characteristic properties */
#define ESP_GATT_CHAR_PROP_BIT_BROADCAST	BTA_GATT_CHAR_PROP_BIT_BROADCAST       /* 0x01 */
#define ESP_GATT_CHAR_PROP_BIT_READ			BTA_GATT_CHAR_PROP_BIT_READ            /* 0x02 */
#define ESP_GATT_CHAR_PROP_BIT_WRITE_NR		BTA_GATT_CHAR_PROP_BIT_WRITE_NR        /* 0x04 */
#define ESP_GATT_CHAR_PROP_BIT_WRITE		BTA_GATT_CHAR_PROP_BIT_WRITE           /* 0x08 */
#define ESP_GATT_CHAR_PROP_BIT_NOTIFY		BTA_GATT_CHAR_PROP_BIT_NOTIFY          /* 0x10 */
#define ESP_GATT_CHAR_PROP_BIT_INDICATE		BTA_GATT_CHAR_PROP_BIT_INDICATE        /* 0x20 */
#define ESP_GATT_CHAR_PROP_BIT_AUTH			BTA_GATT_CHAR_PROP_BIT_AUTH            /* 0x40 */
#define ESP_GATT_CHAR_PROP_BIT_EXT_PROP		BTA_GATT_CHAR_PROP_BIT_EXT_PROP        /* 0x80 */

typedef tBTA_GATTS_EVT			esp_gatts_evt_t;
typedef tBTA_GATTS_IF			esp_gatts_if_t;

/* attribute value */
typedef tBTA_GATT_VALUE			esp_gatt_value_t;

/* attribute response data */
typedef tBTA_GATTS_RSP			esp_gatts_rsp_t;

typedef tBTA_GATTS				esp_gatts_t;

/* attribute request data from the client */
#define ESP_GATT_PREP_WRITE_CANCEL   0x00
#define ESP_GATT_PREP_WRITE_EXEC     0x01
typedef tBTA_GATT_EXEC_FLAG		esp_gatt_exec_flag_t;

/* read request always based on UUID */
typedef tBTA_GATT_READ_REQ		esp_gatt_read_req_t;

/* write request data */
typedef tBTA_GATT_WRITE_REQ		esp_gatt_write_req_t;

/* callback data for server access request from client */
typedef tBTA_GATTS_REQ_DATA		esp_gatts_req_data_t;


/* Search callback */
typedef tBTA_DM_SEARCH_CBACK			esp_dm_search_cb_t;

typedef tBLE_SCAN_PARAM_SETUP_CBACK		esp_scan_param_setup_cb_t;


typedef tBTA_SET_ADV_DATA_CMPL_CBACK esp_ble_set_adv_data_cmpl_cb_t;

/* GATTC enable callback function */
typedef void (esp_gattc_enb_cb_t)(esp_gatt_status_t status);

/* Client callback function */
typedef void (esp_gattc_cb_t)(esp_gattc_evt_t event, esp_gattc_t *cb_data);


/* GATTS enable callback function */
typedef void (esp_gatts_enb_cb_t)(esp_gatt_status_t status);

/* Server callback function */
typedef void (esp_gatts_cb_t)(esp_gatts_evt_t event,  esp_gatts_t *cb_data);


/* Success code and error codes */
#define  ESP_GATT_OK   						BTA_GATT_OK                       
#define  ESP_GATT_INVALID_HANDLE			BTA_GATT_INVALID_HANDLE            /* 0x0001 */
#define  ESP_GATT_READ_NOT_PERMIT			BTA_GATT_READ_NOT_PERMIT           /* 0x0002 */
#define  ESP_GATT_WRITE_NOT_PERMIT			BTA_GATT_WRITE_NOT_PERMIT          /* 0x0003 */
#define  ESP_GATT_INVALID_PDU				BTA_GATT_INVALID_PDU               /* 0x0004 */
#define  ESP_GATT_INSUF_AUTHENTICATION 		BTA_GATT_INSUF_AUTHENTICATION      /* 0x0005 */
#define  ESP_GATT_REQ_NOT_SUPPORTED		    BTA_GATT_REQ_NOT_SUPPORTED         /* 0x0006 */
#define  ESP_GATT_INVALID_OFFSET			BTA_GATT_INVALID_OFFSET            /* 0x0007 */
#define  ESP_GATT_INSUF_AUTHORIZATION		BTA_GATT_INSUF_AUTHORIZATION       /* 0x0008 */
#define  ESP_GATT_PREPARE_Q_FULL			BTA_GATT_PREPARE_Q_FULL            /* 0x0009 */
#define  ESP_GATT_NOT_FOUND					BTA_GATT_NOT_FOUND                 /* 0x000a */
#define  ESP_GATT_NOT_LONG					BTA_GATT_NOT_LONG                  /* 0x000b */
#define  ESP_GATT_INSUF_KEY_SIZE			BTA_GATT_INSUF_KEY_SIZE            /* 0x000c */
#define  ESP_GATT_INVALID_ATTR_LEN			BTA_GATT_INVALID_ATTR_LEN          /* 0x000d */
#define  ESP_GATT_ERR_UNLIKELY				BTA_GATT_ERR_UNLIKELY              /* 0x000e */
#define  ESP_GATT_INSUF_ENCRYPTION			BTA_GATT_INSUF_ENCRYPTION          /* 0x000f */
#define  ESP_GATT_UNSUPPORT_GRP_TYPE		BTA_GATT_UNSUPPORT_GRP_TYPE        /* 0x0010 */
#define  ESP_GATT_INSUF_RESOURCE			BTA_GATT_INSUF_RESOURCE            /* 0x0011 */


#define  ESP_GATT_NO_RESOURCES				BTA_GATT_NO_RESOURCES              /* 0x80 */
#define  ESP_GATT_INTERNAL_ERROR			BTA_GATT_INTERNAL_ERROR            /* 0x81 */
#define  ESP_GATT_WRONG_STATE				BTA_GATT_WRONG_STATE               /* 0x82 */
#define  ESP_GATT_DB_FULL					BTA_GATT_DB_FULL                   /* 0x83 */
#define  ESP_GATT_BUSY						BTA_GATT_BUSY                      /* 0x84 */
#define  ESP_GATT_ERROR						BTA_GATT_ERROR                     /* 0x85 */
#define  ESP_GATT_CMD_STARTED				BTA_GATT_CMD_STARTED               /* 0x86 */
#define  ESP_GATT_ILLEGAL_PARAMETER			BTA_GATT_ILLEGAL_PARAMETER         /* 0x87 */
#define  ESP_GATT_PENDING					BTA_GATT_PENDING                   /* 0x88 */
#define  ESP_GATT_AUTH_FAIL					BTA_GATT_AUTH_FAIL                 /* 0x89 */
#define  ESP_GATT_MORE						BTA_GATT_MORE                      /* 0x8a */
#define  ESP_GATT_INVALID_CFG				BTA_GATT_INVALID_CFG               /* 0x8b */
#define  ESP_GATT_SERVICE_STARTED			BTA_GATT_SERVICE_STARTED           /* 0x8c */
#define  ESP_GATT_ENCRYPED_MITM				BTA_GATT_ENCRYPED_MITM             /* GATT_SUCCESS */
#define  ESP_GATT_ENCRYPED_NO_MITM			BTA_GATT_ENCRYPED_NO_MITM          /* 0x8d */
#define  ESP_GATT_NOT_ENCRYPTED				BTA_GATT_NOT_ENCRYPTED             /* 0x8e */
#define  ESP_GATT_CONGESTED					BTA_GATT_CONGESTED                 /* 0x8f */

#define  ESP_GATT_DUP_REG					BTA_GATT_DUP_REG                   /* 0x90 */
#define  ESP_GATT_ALREADY_OPEN				BTA_GATT_ALREADY_OPEN              /* 0x91 */
#define  ESP_GATT_CANCEL					BTA_GATT_CANCEL                    /* 0x92 */

                                             /* 0xE0 ~ 0xFC reserved for future use */
#define  ESP_GATT_CCC_CFG_ERR				BTA_GATT_CCC_CFG_ERR    		  /* 0xFD Client Characteristic Configuration Descriptor Improperly Configured */
#define  ESP_GATT_PRC_IN_PROGRESS			BTA_GATT_PRC_IN_PROGRESS 		  /* 0xFE Procedure Already in progress */
#define  ESP_GATT_OUT_OF_RANGE				BTA_GATT_OUT_OF_RANGE             /* 0xFFAttribute value out of range */



#define API_BLE_ISVALID_PARAM(x, min, max)  (((x) >= (min) && (x) <= (max)) || ((x) == ESP_BLE_CONN_PARAM_UNDEF))


enum
{
	BLE_ADV_DATA_IDX,
	BLE_SCAN_RSP_DATA_IDX,
	ADV_SCAN_IDX_MAX
};

/// Advertising mode
enum api_adv_type
{
    /// Mode in non-discoverable
    API_NON_DISCOVERABLE,
    /// Mode in general discoverable
    API_GEN_DISCOVERABLE,
    /// Mode in limited discoverable
    API_LIM_DISCOVERABLE,
    /// Broadcaster mode which is a non discoverable and non connectable mode.
    API_BROADCASTER_MODE,
};

enum api_adv_filter
{
     ///Allow both scan and connection requests from anyone
    ADV_ALLOW_SCAN_ANY_CON_ANY    = 0x00,
    ///Allow both scan req from White List devices only and connection req from anyone
    ADV_ALLOW_SCAN_WLST_CON_ANY,
    ///Allow both scan req from anyone and connection req from White List devices only
    ADV_ALLOW_SCAN_ANY_CON_WLST,
    ///Allow scan and connection requests from White List devices only
    ADV_ALLOW_SCAN_WLST_CON_WLST,
    ///Enumeration end value for advertising filter policy value check
    ADV_ALLOW_SCAN_END
};

/// Own BD address source of the device
enum api_own_addr_src
{
   /// Public Address
   API_PUBLIC_ADDR,
   /// Provided random address
   API_PROVIDED_RND_ADDR,
   /// Provided static random address
   API_GEN_STATIC_RND_ADDR,
   /// Generated resolvable private random address
   API_GEN_RSLV_ADDR,
   /// Generated non-resolvable private random address
   API_GEN_NON_RSLV_ADDR,
   /// Provided Reconnection address
   API_PROVIDED_RECON_ADDR,
};



typedef struct
{
	char	*adv_name;				//set the device name to be sent on the advertising
	tBTA_BLE_ADV_DATA ble_adv_data;
}esp_ble_adv_data_cfg_t;

typedef struct
{
	UINT16 adv_int_min;
	UINT16 adv_int_max;
    tBLE_BD_ADDR *p_dir_bda;
}esp_ble_adv_params_t;

typedef struct
{
	UINT16				  	adv_int_min;
	UINT16					adv_int_max;
	UINT8					adv_type;
	tBLE_ADDR_TYPE			addr_type_own;
	tBTM_BLE_ADV_CHNL_MAP	channel_map;
	tBTM_BLE_AFP			adv_filter_policy;
	tBLE_BD_ADDR			*p_dir_bda;
}esp_ble_adv_params_all_t;

typedef struct
{
	UINT8		scan_type;
	UINT16		scan_intv;
	UINT16		scan_win;
	UINT8		addr_type_own;
	UINT8		scan_fil_policy;
}esp_ble_scan_params;


extern void ble_config_adv_data(esp_ble_adv_data_cfg_t *adv_data,
												tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

extern void ble_set_scan_rsp(esp_ble_adv_data_cfg_t *scan_rsp_data,
							  tBTA_SET_ADV_DATA_CMPL_CBACK *p_scan_rsp_data_cback);




#endif		///__ESP_BT_DEFS_H__
