#ifndef __BTC_GATT_UTIL_H__
#define __BTC_GATT_UTIL_H__

#include "bt_types.h"
#include "bta_gatt_api.h"
#include "esp_bt_defs.h"
#include "esp_gatt_defs.h"
#include "esp_gattc_api.h"

int btc128_to_bta_uuid(tBT_UUID *p_dest, uint8_t *p_src);
void btc_to_bta_uuid(tBT_UUID *p_dest, esp_bt_uuid_t *p_src);
void btc_to_bta_gatt_id(tBTA_GATT_ID *p_dest, esp_gatt_id_t *p_src);
void btc_to_bta_srvc_id(tBTA_GATT_SRVC_ID *p_dest, esp_gatt_srvc_id_t *p_src);
void btc_to_bta_response(tBTA_GATTS_RSP *rsp_struct, esp_gatt_rsp_t *p_rsp);

void bta_to_btc_uuid(esp_bt_uuid_t *p_dest, tBT_UUID *p_src);
void bta_to_btc_gatt_id(esp_gatt_id_t *p_dest, tBTA_GATT_ID *p_src);
void bta_to_btc_srvc_id(esp_gatt_srvc_id_t *p_dest, tBTA_GATT_SRVC_ID *p_src);

uint16_t set_read_value(esp_ble_gattc_cb_param_t *p_dest, tBTA_GATTC_READ *p_src);

#endif /* __BTC_GATT_UTIL_H__*/
