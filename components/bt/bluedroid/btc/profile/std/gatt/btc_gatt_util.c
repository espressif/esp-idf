// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "btc_gatt_util.h"

#define GATTC_READ_VALUE_TYPE_VALUE          0x0000  /* Attribute value itself */
#define GATTC_READ_VALUE_TYPE_AGG_FORMAT     0x2905  /* Characteristic Aggregate Format*/

static unsigned char BASE_UUID[16] = {
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
        LOG_ERROR("%s: Unknown UUID length %d!", __FUNCTION__, p_dest->len);
        break;
    }
}

/*******************************************************************************
 * BTC -> BTA conversion functions
 *******************************************************************************/

void btc_to_bta_uuid(tBT_UUID *p_dest, esp_bt_uuid_t *p_src)
{
    p_dest->len = p_src->len;
    if (p_src->len == LEN_UUID_16) {
        p_dest->uu.uuid16 = p_src->uuid.uuid16;
    } else if (p_src->len == LEN_UUID_32) {
        p_dest->uu.uuid32 = p_src->uuid.uuid32;
    } else if (p_src->len == LEN_UUID_128) {
        memcpy(&p_dest->uu.uuid128, p_src->uuid.uuid128, p_dest->len);
    } else {
        LOG_ERROR("%s UUID len is invalid %d\n", __func__, p_dest->len);
    }
}

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
void bta_to_btc_uuid(esp_bt_uuid_t *p_dest, tBT_UUID *p_src)
{
    p_dest->len = p_src->len;
    if (p_src->len == LEN_UUID_16) {
        p_dest->uuid.uuid16 = p_src->uu.uuid16;
    } else if (p_src->len == LEN_UUID_32) {
        p_dest->uuid.uuid32 = p_src->uu.uuid32;
    } else if (p_src->len == LEN_UUID_128) {
        memcpy(&p_dest->uuid.uuid128, p_src->uu.uuid128, p_dest->len);
    } else {
        LOG_ERROR("%s UUID len is invalid %d\n", __func__, p_dest->len);
    }
}

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
uint16_t set_read_value(esp_ble_gattc_cb_param_t *p_dest, tBTA_GATTC_READ *p_src)
{
    uint16_t descr_type = 0;
    uint16_t len = 0;

    p_dest->read.status = p_src->status;
    p_dest->read.conn_id = p_src->conn_id;
    bta_to_btc_srvc_id(&p_dest->read.srvc_id, &p_src->srvc_id);
    bta_to_btc_gatt_id(&p_dest->read.char_id, &p_src->char_id);
    bta_to_btc_gatt_id(&p_dest->read.descr_id, &p_src->descr_type);

    descr_type = get_uuid16(&p_src->descr_type.uuid);

    switch (descr_type) {
    case GATT_UUID_CHAR_AGG_FORMAT:
        /* not supported */
        p_dest->read.value_type = GATTC_READ_VALUE_TYPE_AGG_FORMAT;
        p_dest->read.value_len = 0;
        break;

    default:
        if (( p_src->status == BTA_GATT_OK ) && (p_src->p_value != NULL)) {
            LOG_INFO("%s unformat.len = %d ", __FUNCTION__, p_src->p_value->unformat.len);
            p_dest->read.value_len = p_src->p_value->unformat.len;
            if ( p_src->p_value->unformat.len > 0  && p_src->p_value->unformat.p_value != NULL ) {
                p_dest->read.value = p_src->p_value->unformat.p_value;
            }
            len += p_src->p_value->unformat.len;
        } else {
            p_dest->read.value_len = 0;
        }

        p_dest->read.value_type = GATTC_READ_VALUE_TYPE_VALUE;
        break;
    }

    return len;
}
