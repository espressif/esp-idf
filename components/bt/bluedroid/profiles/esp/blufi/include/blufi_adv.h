#ifndef __BLUFI_ADV_H__
#define __BLUFI_ADV_H__

#include "bta_api.h"
#include "btm_ble_api.h"
#include "esp_bt_defs.h"

extern void BlufiBleConfigadvData(esp_ble_adv_data_cfg_t *adv_data,
												esp_ble_set_adv_data_cmpl_cb_t *p_adv_data_cback);

extern void BlufiBleSetScanRsp(esp_ble_adv_data_cfg_t *scan_rsp_data,
												esp_ble_set_adv_data_cmpl_cb_t *p_scan_rsp_data_cback);

#endif /* __BLUFI_ADV_H__ */
