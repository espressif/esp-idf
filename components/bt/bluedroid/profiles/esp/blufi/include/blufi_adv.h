#ifndef __BLUFI_ADV_H__
#define __BLUFI_ADV_H__

#include "bta_api.h"
#include "btm_ble_api.h"

enum
{
	BLE_ADV_DATA_IDX,
	BLE_SCAN_RSP_DATA_IDX,
	ADV_SCAN_IDX_MAX
};

typedef struct
{
	char	*adv_name;				//set the device name to be sent on the advertising
	tBTA_BLE_ADV_DATA ble_adv_data;
}tBLUFI_BLE_ADV_DATA;

extern void BlufiBleConfigadvData(tBLUFI_BLE_ADV_DATA *adv_data,
												tBTA_SET_ADV_DATA_CMPL_CBACK *p_adv_data_cback);

extern void BlufiBleSetScanRsp(tBLUFI_BLE_ADV_DATA *scan_rsp_data,
							  tBTA_SET_ADV_DATA_CMPL_CBACK *p_scan_rsp_data_cback);

#endif /* __BLUFI_ADV_H__ */
