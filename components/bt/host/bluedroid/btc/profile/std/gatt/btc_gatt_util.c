/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "btc_gatt_util.h"

#define GATTC_READ_VALUE_TYPE_VALUE          0x0000  /* Attribute value itself */
#define GATTC_READ_VALUE_TYPE_AGG_FORMAT     0x2905  /* Characteristic Aggregate Format*/

static const unsigned char BASE_UUID[16] = {
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*******************************************************************************
 * BTIF -> BTA conversion functions
 *******************************************************************************/
int uuidType(unsigned char *p_uuid)
{
    int i = 0;
    int match = 0;
    int all_zero = 1;

    for (i = 0; i != 16; ++i) {
        if (i == 12 || i == 13) {
            continue;
        }

        if (p_uuid[i] == BASE_UUID[i]) {
            ++match;
        }

        if (p_uuid[i] != 0) {
            all_zero = 0;
        }
    }
    if (all_zero) {
        return 0;
    }
    if (match == 12) {
        return LEN_UUID_32;
    }
    if (match == 14) {
        return LEN_UUID_16;
    }
    return LEN_UUID_128;
}

void btc128_to_bta_uuid(tBT_UUID *p_dest, uint8_t *p_src)
{
    int i = 0;

    p_dest->len = uuidType(p_src);

    switch (p_dest->len) {
    case LEN_UUID_16:
        p_dest->uu.uuid16 = (p_src[13] << 8) + p_src[12];
        break;

    case LEN_UUID_32:
        p_dest->uu.uuid32  = (p_src[13] <<  8) + p_src[12];
        p_dest->uu.uuid32 += (p_src[15] << 24) + (p_src[14] << 16);
        break;

    case LEN_UUID_128:
        for (i = 0; i != 16; ++i) {
            p_dest->uu.uuid128[i] = p_src[i];
        }
        break;

    default:
        BTC_TRACE_ERROR("%s: Unknown UUID length %d!", __FUNCTION__, p_dest->len);
        break;
    }
}

/*******************************************************************************
 * BTC -> BTA conversion functions
 *******************************************************************************/
void btc_to_bta_gatt_id(tBTA_GATT_ID *p_dest, esp_gatt_id_t *p_src)
{
    p_dest->inst_id = p_src->inst_id;
    btc_to_bta_uuid(&p_dest->uuid, &p_src->uuid);
}

void btc_to_bta_srvc_id(tBTA_GATT_SRVC_ID *p_dest, esp_gatt_srvc_id_t *p_src)
{
    p_dest->is_primary = p_src->is_primary;
    btc_to_bta_gatt_id(&p_dest->id, &p_src->id);
}


/*******************************************************************************
 * BTA -> BTC conversion functions
 *******************************************************************************/
void bta_to_btc_gatt_id(esp_gatt_id_t *p_dest, tBTA_GATT_ID *p_src)
{
    p_dest->inst_id = p_src->inst_id;
    bta_to_btc_uuid(&p_dest->uuid, &p_src->uuid);
}

void bta_to_btc_srvc_id(esp_gatt_srvc_id_t *p_dest, tBTA_GATT_SRVC_ID *p_src)
{
    p_dest->is_primary = p_src->is_primary;
    bta_to_btc_gatt_id(&p_dest->id, &p_src->id);
}

void btc_to_bta_response(tBTA_GATTS_RSP *p_dest, esp_gatt_rsp_t *p_src)
{
    p_dest->attr_value.auth_req = p_src->attr_value.auth_req;
    p_dest->attr_value.handle   = p_src->attr_value.handle;
    p_dest->attr_value.len      = p_src->attr_value.len;
    p_dest->attr_value.offset   = p_src->attr_value.offset;
    memcpy(p_dest->attr_value.value, p_src->attr_value.value, ESP_GATT_MAX_ATTR_LEN);
}

uint16_t get_uuid16(tBT_UUID *p_uuid)
{
    if (p_uuid->len == LEN_UUID_16) {
        return p_uuid->uu.uuid16;
    } else if (p_uuid->len == LEN_UUID_128) {
        UINT16 u16;
        UINT8 *p = &p_uuid->uu.uuid128[LEN_UUID_128 - 4];
        STREAM_TO_UINT16(u16, p);
        return u16;
    } else { /* p_uuid->len == LEN_UUID_32 */
        return (UINT16) p_uuid->uu.uuid32;
    }
}

uint16_t set_read_value(uint8_t *gattc_if, esp_ble_gattc_cb_param_t *p_dest, tBTA_GATTC_READ *p_src)
{
    uint16_t len = 0;

    p_dest->read.status = p_src->status;
    p_dest->read.conn_id = BTC_GATT_GET_CONN_ID(p_src->conn_id);
    *gattc_if = BTC_GATT_GET_GATT_IF(p_src->conn_id);
    p_dest->read.status = p_src->status;
    p_dest->read.handle = p_src->handle;

    if (( p_src->status == BTA_GATT_OK ) && (p_src->p_value != NULL))
    {
        BTC_TRACE_DEBUG("%s len = %d ", __func__, p_src->p_value->len);
        p_dest->read.value_len = p_src->p_value->len;
        if ( p_src->p_value->len > 0  && p_src->p_value->p_value != NULL ) {
            p_dest->read.value = p_src->p_value->p_value;
        }
        len += p_src->p_value->len;
    } else {
        p_dest->read.value_len = 0;
    }

    return len;
}
