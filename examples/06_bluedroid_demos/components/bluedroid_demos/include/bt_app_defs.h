
#ifndef _BT_APP_DEFS_H__
#define	_BT_APP_DEFS_H__


#include "bta_api.h"
#include "btm_ble_api.h"

#define API_BLE_CONN_PARAM_UNDEF        0xffff      /* use this value when a specific value not to be overwritten */

#define API_BLE_ADV_CHNL_MAP (API_BLE_ADV_CHNL_37|API_BLE_ADV_CHNL_38|API_BLE_ADV_CHNL_39)

/* advertising channel map */
#define API_BLE_ADV_CHNL_37    (0x01 << 0)
#define API_BLE_ADV_CHNL_38    (0x01 << 1)
#define API_BLE_ADV_CHNL_39    (0x01 << 2)


#define API_BLE_ISVALID_PARAM(x, min, max)  (((x) >= (min) && (x) <= (max)) || ((x) == API_BLE_CONN_PARAM_UNDEF))


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
}tESP_BLE_ADV_DATA;

typedef struct
{
	UINT16 adv_int_min;
	UINT16 adv_int_max;
    tBLE_BD_ADDR *p_dir_bda;
}tESP_BLE_ADV_PARAM;

typedef struct
{
	UINT16				  	adv_int_min;
	UINT16					adv_int_max;
	UINT8					adv_type;
	tBLE_ADDR_TYPE			addr_type_own;
	tBTM_BLE_ADV_CHNL_MAP	channel_map;
	tBTM_BLE_AFP			adv_filter_policy;
	tBLE_BD_ADDR			*p_dir_bda;
}tESP_API_BLE_ADV_PARAMS_ALL;

typedef struct
{
	UINT8		scan_type;
	UINT16		scan_intv;
	UINT16		scan_win;
	UINT8		addr_type_own;
	UINT8		scan_fil_policy;
}tESP_BLE_SCAN_PARAMS;


extern void ESP_AppBleConfigadvData(tESP_BLE_ADV_DATA *adv_data,
												tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

extern void ESP_AppBleSetScanRsp(tESP_BLE_ADV_DATA *scan_rsp_data,
							  tBTA_SET_ADV_DATA_CMPL_CBACK *p_scan_rsp_data_cback);




#endif		///_BT_APP_DEFS_H__