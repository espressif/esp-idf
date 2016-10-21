#include "bta_api.h"
#include "btm_ble_api.h"

#define API_BLE_CONN_PARAM_UNDEF        0xffff      /* use this value when a specific value not to be overwritten */


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
    /// Start non connectable advertising
    API_ADV_NON_CONN,
    /// Start undirected connectable advertising
    API_ADV_UNDIRECT,
	/// Start directed connectable advertising
	API_ADV_DIRECT_LOW_DUTY,
    /// Start directed connectable advertising
    API_ADV_DIRECT
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


extern void ESP_AppBleConfigadvData(tESP_BLE_ADV_DATA *adv_data,
												tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

extern void ESP_AppBleSetScanRsp(tESP_BLE_ADV_DATA *scan_rsp_data,
							  tBTA_SET_ADV_DATA_CMPL_CBACK *p_scan_rsp_data_cback);


