#include "common/bt_target.h"
#include "osi/allocator.h"

#include <string.h>
#include "gatt_int.h"
#include "stack/l2c_api.h"
#include "l2c_int.h"
#include "smp_int.h"

#if (BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)
static void attr_uuid_to_bt_uuid(void *p_attr, tBT_UUID *p_uuid)
{
    tGATT_ATTR16 *p_attr16 = (tGATT_ATTR16 *)p_attr;

    if (p_attr16->uuid_type == GATT_ATTR_UUID_TYPE_16) {
        p_uuid->len = LEN_UUID_16;
        p_uuid->uu.uuid16 = p_attr16->uuid;
    } else if (p_attr16->uuid_type == GATT_ATTR_UUID_TYPE_32) {
        tGATT_ATTR32 *p_attr32 = (tGATT_ATTR32 *)p_attr;
        p_uuid->len = LEN_UUID_32;
        p_uuid->uu.uuid32 = p_attr32->uuid;
    } else if (p_attr16->uuid_type == GATT_ATTR_UUID_TYPE_128) {
        tGATT_ATTR128 *p_attr128 = (tGATT_ATTR128 *)p_attr;
        p_uuid->len = LEN_UUID_128;
        memcpy(p_uuid->uu.uuid128, p_attr128->uuid, LEN_UUID_128);
    }
}

static size_t calculate_database_info_size(void)
{
    UINT8 i;
    tGATT_SVC_DB *p_db;
    tGATT_ATTR16 *p_attr;
    size_t len = 0;

    for (i = 0; i < GATT_MAX_SR_PROFILES; i++) {
        p_db = gatt_cb.sr_reg[i].p_db;
        if (p_db && p_db->p_attr_list) {
            p_attr = (tGATT_ATTR16 *)p_db->p_attr_list;
            while (p_attr) {
                if (p_attr->uuid == GATT_UUID_PRI_SERVICE ||
                    p_attr->uuid == GATT_UUID_SEC_SERVICE) {
                    // Service declaration
                    len += 4 + p_attr->p_value->uuid.len;
                } else if (p_attr->uuid == GATT_UUID_INCLUDE_SERVICE) {
                    // Included service declaration
                    len += 8 + p_attr->p_value->incl_handle.service_type.len;
                } else if (p_attr->uuid == GATT_UUID_CHAR_DECLARE) {
                    tBT_UUID char_uuid;
                    // Characteristic declaration
                    p_attr = (tGATT_ATTR16 *)p_attr->p_next;
                    attr_uuid_to_bt_uuid((void *)p_attr, &char_uuid);
                    // Increment 1 to fetch characteristic uuid from value declaration attribute
                    len += 7 + char_uuid.len;
                } else if (p_attr->uuid == GATT_UUID_CHAR_DESCRIPTION ||
                    p_attr->uuid == GATT_UUID_CHAR_CLIENT_CONFIG ||
                    p_attr->uuid == GATT_UUID_CHAR_SRVR_CONFIG ||
                    p_attr->uuid == GATT_UUID_CHAR_PRESENT_FORMAT ||
                    p_attr->uuid == GATT_UUID_CHAR_AGG_FORMAT) {
                    // Descriptor
                    len += 4;
                } else if (p_attr->uuid == GATT_UUID_CHAR_EXT_PROP) {
                    // Descriptor
                    len += 6;
                }
                p_attr = (tGATT_ATTR16 *) p_attr->p_next;
            }
        }
    }

    return len;
}

static void fill_database_info(UINT8 *p_data)
{
    UINT8 i;
    tGATT_SVC_DB *p_db;
    tGATT_ATTR16 *p_attr;

    for (i = 0; i < GATT_MAX_SR_PROFILES; i++) {
        p_db = gatt_cb.sr_reg[i].p_db;
        if (p_db && p_db->p_attr_list) {
            p_attr = (tGATT_ATTR16 *)p_db->p_attr_list;
            while (p_attr) {
                if (p_attr->uuid == GATT_UUID_PRI_SERVICE ||
                    p_attr->uuid == GATT_UUID_SEC_SERVICE) {
                    // Service declaration
                    UINT16_TO_STREAM(p_data, p_attr->handle);
                    UINT16_TO_STREAM(p_data, p_attr->uuid);
                    gatt_build_uuid_to_stream(&p_data, p_attr->p_value->uuid);
                } else if (p_attr->uuid == GATT_UUID_INCLUDE_SERVICE) {
                    // Included service declaration
                    UINT16_TO_STREAM(p_data, p_attr->handle);
                    UINT16_TO_STREAM(p_data, GATT_UUID_INCLUDE_SERVICE);
                    UINT16_TO_STREAM(p_data, p_attr->p_value->incl_handle.s_handle);
                    UINT16_TO_STREAM(p_data, p_attr->p_value->incl_handle.e_handle);
                    gatt_build_uuid_to_stream(&p_data, p_attr->p_value->incl_handle.service_type);
                } else if (p_attr->uuid == GATT_UUID_CHAR_DECLARE) {
                    tBT_UUID char_uuid;
                    // Characteristic declaration
                    UINT16_TO_STREAM(p_data, p_attr->handle);
                    UINT16_TO_STREAM(p_data, GATT_UUID_CHAR_DECLARE);
                    UINT8_TO_STREAM(p_data, p_attr->p_value->char_decl.property);
                    UINT16_TO_STREAM(p_data, p_attr->p_value->char_decl.char_val_handle);
                    p_attr = (tGATT_ATTR16 *)p_attr->p_next;
                    attr_uuid_to_bt_uuid((void *)p_attr, &char_uuid);
                    // Increment 1 to fetch characteristic uuid from value declaration attribute
                    gatt_build_uuid_to_stream(&p_data, char_uuid);
                } else if (p_attr->uuid == GATT_UUID_CHAR_DESCRIPTION ||
                    p_attr->uuid == GATT_UUID_CHAR_CLIENT_CONFIG ||
                    p_attr->uuid == GATT_UUID_CHAR_SRVR_CONFIG ||
                    p_attr->uuid == GATT_UUID_CHAR_PRESENT_FORMAT ||
                    p_attr->uuid == GATT_UUID_CHAR_AGG_FORMAT) {
                    // Descriptor
                    UINT16_TO_STREAM(p_data, p_attr->handle);
                    UINT16_TO_STREAM(p_data, p_attr->uuid);
                } else if (p_attr->uuid == GATT_UUID_CHAR_EXT_PROP) {
                    // Descriptor
                    UINT16_TO_STREAM(p_data, p_attr->handle);
                    UINT16_TO_STREAM(p_data, p_attr->uuid);
                    // TODO: process extended properties descriptor
                    if (p_attr->p_value->attr_val.attr_len == 2) {
                        memcpy(p_data, p_attr->p_value->attr_val.attr_val, 2);
                    } else {
                        UINT16_TO_STREAM(p_data, 0x0000);
                    }
                }
                p_attr = (tGATT_ATTR16 *) p_attr->p_next;
            }
        }
    }
}

tGATT_STATUS gatts_calculate_datebase_hash(BT_OCTET16 hash)
{
    UINT8 tmp;
    UINT16 i;
    UINT16 j;
    size_t len;
    UINT8 *data_buf = NULL;

    len = calculate_database_info_size();

    data_buf = (UINT8 *)osi_malloc(len);
    if (data_buf == NULL) {
        GATT_TRACE_ERROR ("%s failed to allocate buffer (%u)\n", __func__, len);
        return GATT_NO_RESOURCES;
    }

    fill_database_info(data_buf);

    // reverse database info
    for (i = 0, j = len-1; i < j; i++, j--) {
        tmp = data_buf[i];
        data_buf[i] = data_buf[j];
        data_buf[j] = tmp;
    }

#if SMP_INCLUDED == TRUE
    BT_OCTET16 key = {0};
    aes_cipher_msg_auth_code(key, data_buf, len, 16, hash);
    //ESP_LOG_BUFFER_HEX("db hash", hash, BT_OCTET16_LEN);
#endif

    osi_free(data_buf);
    return GATT_SUCCESS;
}
#endif  /* BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE */
