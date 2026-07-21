/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "host/ble_gap.h"
#include "os/os_mbuf.h"
#include "ble_ots_server_int.h"

static const char *TAG = "ots_filter_changed";

/*****************************************************************************
 * Bond-index mapping: maps peer identity addresses to stable bit indices
 * (0–31) for the per-object `marked` bitmask.
 *****************************************************************************/

/** @brief Maximum number of bond index slots (must be <= 32 for uint32_t bitmask) */
#define OTS_MAX_BOND_SLOTS  32

/** @brief A single bond-index slot */
typedef struct {
    bool     in_use;        /*!< Whether this slot is occupied */
    ble_addr_t peer_addr;   /*!< Peer identity address */
} ots_bond_slot_t;

/**
 * @brief Aggregated module context for the filter/changed sub-module.
 *
 * Holds all of this module's mutable global state in a single heap-allocated
 * block: the bond-index slot table, the reusable filtered list, and the
 * transient sort order used to bridge ble_ots_server_filter_get_list() and the
 * qsort() comparison callback.
 */
typedef struct {
    ots_bond_slot_t          bond_slots[OTS_MAX_BOND_SLOTS]; /*!< Bond-index slot table */
    ble_ots_server_filtered_list_t filtered_list;           /*!< Reusable filtered list */
    ble_ots_list_sort_order_t sort_order;                    /*!< Active sort order for sort_compare */
} ots_server_filter_changed_ctx_t;

static ots_server_filter_changed_ctx_t *s_ots_server_filter_changed;

/**
 * @brief Allocate the filter/changed module context.
 *
 * Called once on the OTS server init path, before any filter/changed API is
 * used.  Idempotent: a no-op if the context is already allocated.
 *
 * @return 0 on success, BLE_HS_ENOMEM on allocation failure
 */
int
ble_ots_server_filter_changed_init(void)
{
    if (s_ots_server_filter_changed) {
        return 0;
    }

    s_ots_server_filter_changed = calloc(1, sizeof(*s_ots_server_filter_changed));
    if (!s_ots_server_filter_changed) {
        ESP_LOGE(TAG, "filter_changed_init: failed to allocate context");
        return BLE_HS_ENOMEM;
    }

    return 0;
}

/**
 * @brief Free the filter/changed module context.
 *
 * Called on the OTS server deinit path.  Safe to call when the context was
 * never allocated.
 */
void
ble_ots_server_filter_changed_deinit(void)
{
    if (s_ots_server_filter_changed) {
        free(s_ots_server_filter_changed);
        s_ots_server_filter_changed = NULL;
    }
}

/**
 * @brief Resolve a transient conn_id to a stable bond index (0–31).
 *
 * Looks up the peer identity address via ble_gap_conn_find(), then searches
 * the bond-slot table for an existing entry.  If none is found, a free slot
 * is allocated.
 *
 * @param conn_id  BLE connection handle
 * @param[out] out_index  Receives the bond index on success
 * @return 0 on success, non-zero on failure (connection not found or table full)
 */
static int
resolve_bond_index(uint16_t conn_id, uint8_t *out_index)
{
    if (!s_ots_server_filter_changed) {
        return BLE_HS_ENOMEM;
    }

    struct ble_gap_conn_desc desc;
    int rc = ble_gap_conn_find(conn_id, &desc);
    if (rc != 0) {
        ESP_LOGW(TAG, "resolve_bond_index: conn_find failed conn_id=%d rc=%d",
                 conn_id, rc);
        return rc;
    }

    const ble_addr_t *peer = &desc.peer_id_addr;
    ots_bond_slot_t *bond_slots = s_ots_server_filter_changed->bond_slots;

    /* Search for an existing slot with the same peer identity address */
    int free_slot = -1;
    for (int i = 0; i < OTS_MAX_BOND_SLOTS; i++) {
        if (bond_slots[i].in_use) {
            if (bond_slots[i].peer_addr.type == peer->type &&
                memcmp(bond_slots[i].peer_addr.val, peer->val, 6) == 0) {
                *out_index = (uint8_t)i;
                return 0;
            }
        } else if (free_slot < 0) {
            free_slot = i;
        }
    }

    /* Allocate a new slot */
    if (free_slot < 0) {
        ESP_LOGW(TAG, "resolve_bond_index: bond slot table full");
        return BLE_HS_ENOMEM;
    }

    bond_slots[free_slot].in_use = true;
    bond_slots[free_slot].peer_addr = *peer;
    *out_index = (uint8_t)free_slot;
    return 0;
}

/*****************************************************************************
 * Helper: Date-Time comparison
 * Returns <0 if a < b, 0 if a == b, >0 if a > b
 *****************************************************************************/
static int
datetime_cmp(const ble_ots_date_time_t *a, const ble_ots_date_time_t *b)
{
    if (a->year != b->year) {
        return (int)a->year - (int)b->year;
    }
    if (a->month != b->month) {
        return (int)a->month - (int)b->month;
    }
    if (a->day != b->day) {
        return (int)a->day - (int)b->day;
    }
    if (a->hours != b->hours) {
        return (int)a->hours - (int)b->hours;
    }
    if (a->minutes != b->minutes) {
        return (int)a->minutes - (int)b->minutes;
    }
    if (a->seconds != b->seconds) {
        return (int)a->seconds - (int)b->seconds;
    }
    return 0;
}

/*****************************************************************************
 * Helper: Check if a date-time is all zeros (not valid)
 *****************************************************************************/
static bool
datetime_is_zero(const ble_ots_date_time_t *dt)
{
    return (dt->year == 0 && dt->month == 0 && dt->day == 0 &&
            dt->hours == 0 && dt->minutes == 0 && dt->seconds == 0);
}

/*****************************************************************************
 * Helper: Parse a date-time from raw bytes (7 octets, little-endian)
 *****************************************************************************/
static void
parse_date_time(const uint8_t *data, ble_ots_date_time_t *dt)
{
    dt->year    = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
    dt->month   = data[2];
    dt->day     = data[3];
    dt->hours   = data[4];
    dt->minutes = data[5];
    dt->seconds = data[6];
}

/*****************************************************************************
 * Helper: Serialize a date-time to raw bytes (7 octets, little-endian)
 *****************************************************************************/
static void
serialize_date_time(const ble_ots_date_time_t *dt, uint8_t *buf)
{
    buf[0] = (uint8_t)(dt->year & 0xFF);
    buf[1] = (uint8_t)((dt->year >> 8) & 0xFF);
    buf[2] = dt->month;
    buf[3] = dt->day;
    buf[4] = dt->hours;
    buf[5] = dt->minutes;
    buf[6] = dt->seconds;
}

/*****************************************************************************
 * Helper: Check if a single object passes a single filter
 *****************************************************************************/
static bool
object_passes_filter(const ble_ots_server_obj_t *obj,
                     const ble_ots_list_filter_t *filter,
                     uint16_t conn_id)
{
    switch (filter->filter_type) {
    case BLE_OTS_FILTER_NO_FILTER:
        return true;

    case BLE_OTS_FILTER_NAME_STARTS_WITH: {
        uint8_t plen = filter->param.name.name_len;
        if (obj->name_len < plen) {
            return false;
        }
        return (memcmp(obj->name, filter->param.name.name, plen) == 0);
    }

    case BLE_OTS_FILTER_NAME_ENDS_WITH: {
        uint8_t plen = filter->param.name.name_len;
        if (obj->name_len < plen) {
            return false;
        }
        return (memcmp(obj->name + obj->name_len - plen,
                       filter->param.name.name, plen) == 0);
    }

    case BLE_OTS_FILTER_NAME_CONTAINS: {
        uint8_t plen = filter->param.name.name_len;
        if (plen == 0) {
            return true;
        }
        if (obj->name_len < plen) {
            return false;
        }
        /* Byte-level substring search */
        for (uint8_t i = 0; i <= obj->name_len - plen; i++) {
            if (memcmp(obj->name + i, filter->param.name.name, plen) == 0) {
                return true;
            }
        }
        return false;
    }

    case BLE_OTS_FILTER_NAME_IS_EXACTLY: {
        uint8_t plen = filter->param.name.name_len;
        if (obj->name_len != plen) {
            return false;
        }
        return (memcmp(obj->name, filter->param.name.name, plen) == 0);
    }

    case BLE_OTS_FILTER_OBJECT_TYPE: {
        if (obj->type_uuid_len != filter->param.obj_type.uuid_len) {
            return false;
        }
        return (memcmp(obj->type_uuid, filter->param.obj_type.uuid,
                       obj->type_uuid_len) == 0);
    }

    case BLE_OTS_FILTER_CREATED_BETWEEN: {
        /* Objects with zero/invalid first_created always pass */
        if (datetime_is_zero(&obj->first_created)) {
            return true;
        }
        if (datetime_cmp(&obj->first_created, &filter->param.timestamp_range.timestamp1) < 0) {
            return false;
        }
        if (datetime_cmp(&obj->first_created, &filter->param.timestamp_range.timestamp2) > 0) {
            return false;
        }
        return true;
    }

    case BLE_OTS_FILTER_MODIFIED_BETWEEN: {
        /* Objects with zero/invalid last_modified always pass */
        if (datetime_is_zero(&obj->last_modified)) {
            return true;
        }
        if (datetime_cmp(&obj->last_modified, &filter->param.timestamp_range.timestamp1) < 0) {
            return false;
        }
        if (datetime_cmp(&obj->last_modified, &filter->param.timestamp_range.timestamp2) > 0) {
            return false;
        }
        return true;
    }

    case BLE_OTS_FILTER_CURRENT_SIZE_BETWEEN:
        return (obj->current_size >= filter->param.size_range.size1 &&
                obj->current_size <= filter->param.size_range.size2);

    case BLE_OTS_FILTER_ALLOCATED_SIZE_BETWEEN:
        return (obj->allocated_size >= filter->param.size_range.size1 &&
                obj->allocated_size <= filter->param.size_range.size2);

    case BLE_OTS_FILTER_MARKED_OBJECTS: {
        /* Resolve conn_id to a stable bond index for the per-object marked bitmask */
        uint8_t bond_idx;
        if (resolve_bond_index(conn_id, &bond_idx) != 0) {
            return false;
        }
        return (obj->marked & (1UL << bond_idx)) != 0;
    }

    default:
        return true;
    }
}

/*****************************************************************************
 * Helper: Compare two object names byte-wise, ascending
 * Returns <0 if a < b, 0 if a == b, >0 if a > b
 *****************************************************************************/
static int
obj_name_cmp(const ble_ots_server_obj_t *a, const ble_ots_server_obj_t *b)
{
    int min_len = (a->name_len < b->name_len) ? a->name_len : b->name_len;
    int result = memcmp(a->name, b->name, min_len);
    if (result == 0) {
        result = (int)a->name_len - (int)b->name_len;
    }
    return result;
}

/*****************************************************************************
 * Helper: Compare two Object IDs, ascending
 * Returns <0 if a < b, 0 if a == b, >0 if a > b
 *****************************************************************************/
static int
obj_id_cmp(ble_ots_obj_id_t a, ble_ots_obj_id_t b)
{
    if (a < b) {
        return -1;
    }
    return (a > b) ? 1 : 0;
}

/*****************************************************************************
 * Sort comparison context and callback for qsort
 *****************************************************************************/
static int
sort_compare(const void *a, const void *b)
{
    ble_ots_obj_id_t id_a = *(const ble_ots_obj_id_t *)a;
    ble_ots_obj_id_t id_b = *(const ble_ots_obj_id_t *)b;

    ble_ots_server_obj_t *obj_a = ble_ots_server_obj_db_lookup(id_a);
    ble_ots_server_obj_t *obj_b = ble_ots_server_obj_db_lookup(id_b);

    if (!obj_a || !obj_b) {
        /* Should not happen: the list is built from live objects.  Fall back to
         * Object ID order so the result stays a valid total ordering. */
        return obj_id_cmp(id_a, id_b);
    }

    int result = 0;

    switch (s_ots_server_filter_changed->sort_order) {
    case BLE_OTS_SORT_ORDER_NAME_ASC:
    case BLE_OTS_SORT_ORDER_NAME_DESC:
        result = obj_name_cmp(obj_a, obj_b);
        break;

    case BLE_OTS_SORT_ORDER_TYPE_ASC:
    case BLE_OTS_SORT_ORDER_TYPE_DESC: {
        /* Compare type UUIDs */
        if (obj_a->type_uuid_len != obj_b->type_uuid_len) {
            result = (int)obj_a->type_uuid_len - (int)obj_b->type_uuid_len;
        } else {
            result = memcmp(obj_a->type_uuid, obj_b->type_uuid, obj_a->type_uuid_len);
        }
        break;
    }

    case BLE_OTS_SORT_ORDER_CURRENT_SIZE_ASC:
    case BLE_OTS_SORT_ORDER_CURRENT_SIZE_DESC:
        if (obj_a->current_size < obj_b->current_size) {
            result = -1;
        } else if (obj_a->current_size > obj_b->current_size) {
            result = 1;
        }
        break;

    case BLE_OTS_SORT_ORDER_FIRST_CREATED_ASC:
    case BLE_OTS_SORT_ORDER_FIRST_CREATED_DESC:
        result = datetime_cmp(&obj_a->first_created, &obj_b->first_created);
        break;

    case BLE_OTS_SORT_ORDER_LAST_MODIFIED_ASC:
    case BLE_OTS_SORT_ORDER_LAST_MODIFIED_DESC:
        result = datetime_cmp(&obj_a->last_modified, &obj_b->last_modified);
        break;

    default:
        /* Default: ascending by name */
        result = obj_name_cmp(obj_a, obj_b);
        break;
    }

    /* Reverse for descending orders.  The ascending sort orders are 0x01-0x05
     * and the descending ones 0x11-0x15, so a single >= test against the first
     * descending value selects exactly the descending half. */
    if (s_ots_server_filter_changed->sort_order >= BLE_OTS_SORT_ORDER_NAME_DESC) {
        result = -result;
    }

    /* Objects that compare equal on the sort criterion must still be ordered
     * deterministically: OLCP First/Last/Next/Previous rebuild and re-sort this
     * list on every procedure and locate the current object by its position, so
     * an unstable order (qsort is not a stable sort) would make navigation skip
     * or repeat objects.  Tie-break on name and then Object ID, always
     * ascending — applied after the inversion above so that descending orders
     * do not flip the tie-breaker as well. */
    if (result == 0) {
        result = obj_name_cmp(obj_a, obj_b);
        if (result == 0) {
            result = obj_id_cmp(id_a, id_b);
        }
    }

    return result;
}

/*****************************************************************************
 * ble_ots_server_filter_reset
 *****************************************************************************/
void
ble_ots_server_filter_reset(uint16_t conn_id)
{
    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        ESP_LOGW(TAG, "filter_reset: conn_id=%d not found", conn_id);
        return;
    }

    for (int i = 0; i < BLE_OTS_FILTER_INSTANCE_COUNT; i++) {
        memset(&cs->filter[i], 0, sizeof(ble_ots_list_filter_t));
        cs->filter[i].filter_type = BLE_OTS_FILTER_NO_FILTER;
    }
}

/*****************************************************************************
 * ble_ots_server_filter_write
 *****************************************************************************/
int
ble_ots_server_filter_write(uint16_t conn_id, uint8_t instance_idx,
                            const uint8_t *data, uint16_t length)
{
    if (instance_idx >= BLE_OTS_FILTER_INSTANCE_COUNT) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    if (!ble_ots_server_concurrency_check(conn_id)) {
        return BLE_OTS_APP_ERR_CONCURRENCY_LIMIT_EXCEEDED;
    }

    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    if (length < 1) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    uint8_t filter_type = data[0];
    uint16_t param_len = length - 1;
    const uint8_t *param_data = data + 1;

    /* Validate filter type range */
    if (filter_type > BLE_OTS_FILTER_MARKED_OBJECTS) {
        return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
    }

    /* Validate parameter length per filter type */
    ble_ots_list_filter_t new_filter;
    memset(&new_filter, 0, sizeof(new_filter));
    new_filter.filter_type = filter_type;

    switch (filter_type) {
    case BLE_OTS_FILTER_NO_FILTER:
        if (param_len != 0) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        break;

    case BLE_OTS_FILTER_NAME_STARTS_WITH:
    case BLE_OTS_FILTER_NAME_ENDS_WITH:
    case BLE_OTS_FILTER_NAME_CONTAINS:
        /* At least 1 octet of parameter required for 0x01-0x03 */
        if (param_len < 1 || param_len > BLE_OTS_OBJECT_NAME_MAX_LEN) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        memcpy(new_filter.param.name.name, param_data, param_len);
        new_filter.param.name.name_len = (uint8_t)param_len;
        break;

    case BLE_OTS_FILTER_NAME_IS_EXACTLY:
        /* 0 or more octets for Name Is Exactly */
        if (param_len > BLE_OTS_OBJECT_NAME_MAX_LEN) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        if (param_len > 0) {
            memcpy(new_filter.param.name.name, param_data, param_len);
        }
        new_filter.param.name.name_len = (uint8_t)param_len;
        break;

    case BLE_OTS_FILTER_OBJECT_TYPE:
        if (param_len != 2 && param_len != 16) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        memcpy(new_filter.param.obj_type.uuid, param_data, param_len);
        new_filter.param.obj_type.uuid_len = (uint8_t)param_len;
        break;

    case BLE_OTS_FILTER_CREATED_BETWEEN:
    case BLE_OTS_FILTER_MODIFIED_BETWEEN:
        /* 14 bytes: two date_time (7 each) */
        if (param_len != 14) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        parse_date_time(param_data, &new_filter.param.timestamp_range.timestamp1);
        parse_date_time(param_data + 7, &new_filter.param.timestamp_range.timestamp2);
        /* Validate timestamp1 <= timestamp2 */
        if (datetime_cmp(&new_filter.param.timestamp_range.timestamp1,
                         &new_filter.param.timestamp_range.timestamp2) > 0) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }
        break;

    case BLE_OTS_FILTER_CURRENT_SIZE_BETWEEN:
    case BLE_OTS_FILTER_ALLOCATED_SIZE_BETWEEN:
        /* 8 bytes: two uint32 */
        if (param_len != 8) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        new_filter.param.size_range.size1 = (uint32_t)param_data[0] |
                                            ((uint32_t)param_data[1] << 8) |
                                            ((uint32_t)param_data[2] << 16) |
                                            ((uint32_t)param_data[3] << 24);
        new_filter.param.size_range.size2 = (uint32_t)param_data[4] |
                                            ((uint32_t)param_data[5] << 8) |
                                            ((uint32_t)param_data[6] << 16) |
                                            ((uint32_t)param_data[7] << 24);
        /* Validate size1 <= size2 */
        if (new_filter.param.size_range.size1 > new_filter.param.size_range.size2) {
            return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
        }
        break;

    case BLE_OTS_FILTER_MARKED_OBJECTS:
        if (param_len != 0) {
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }
        break;

    default:
        return BLE_OTS_APP_ERR_WRITE_REQUEST_REJECTED;
    }

    /* Store the new filter */
    memcpy(&cs->filter[instance_idx], &new_filter, sizeof(ble_ots_list_filter_t));

    /* Check if current object still passes all filters; invalidate if not */
    if (cs->current_obj_valid) {
        ble_ots_server_obj_t *cur_obj = ble_ots_server_obj_db_lookup(cs->current_object_id);
        if (cur_obj) {
            bool passes = true;
            for (int i = 0; i < BLE_OTS_FILTER_INSTANCE_COUNT; i++) {
                if (!object_passes_filter(cur_obj, &cs->filter[i], conn_id)) {
                    passes = false;
                    break;
                }
            }
            if (!passes) {
                ble_ots_server_current_obj_set(conn_id, BLE_OTS_OBJ_ID_INVALID);
            }
        }
    }

    return 0;
}

/*****************************************************************************
 * ble_ots_server_filter_read
 *****************************************************************************/
int
ble_ots_server_filter_read(uint16_t conn_id, uint8_t instance_idx,
                           uint8_t *buf, uint16_t *buf_len)
{
    if (instance_idx >= BLE_OTS_FILTER_INSTANCE_COUNT) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    if (!ble_ots_server_concurrency_check(conn_id)) {
        return BLE_OTS_APP_ERR_CONCURRENCY_LIMIT_EXCEEDED;
    }

    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    const ble_ots_list_filter_t *filter = &cs->filter[instance_idx];
    uint16_t offset = 0;
    uint16_t capacity = *buf_len;

    /* First byte: filter type */
    if (offset < capacity) {
        buf[offset] = filter->filter_type;
    }
    offset++;

    /* Parameter bytes depend on filter type */
    switch (filter->filter_type) {
    case BLE_OTS_FILTER_NO_FILTER:
    case BLE_OTS_FILTER_MARKED_OBJECTS:
        /* No parameter */
        break;

    case BLE_OTS_FILTER_NAME_STARTS_WITH:
    case BLE_OTS_FILTER_NAME_ENDS_WITH:
    case BLE_OTS_FILTER_NAME_CONTAINS:
    case BLE_OTS_FILTER_NAME_IS_EXACTLY: {
        uint8_t nlen = filter->param.name.name_len;
        if (nlen > 0 && (offset + nlen) <= capacity) {
            memcpy(buf + offset, filter->param.name.name, nlen);
        }
        offset += nlen;
        break;
    }

    case BLE_OTS_FILTER_OBJECT_TYPE: {
        uint8_t ulen = filter->param.obj_type.uuid_len;
        if (ulen > 0 && (offset + ulen) <= capacity) {
            memcpy(buf + offset, filter->param.obj_type.uuid, ulen);
        }
        offset += ulen;
        break;
    }

    case BLE_OTS_FILTER_CREATED_BETWEEN:
    case BLE_OTS_FILTER_MODIFIED_BETWEEN:
        if ((offset + 14) <= capacity) {
            serialize_date_time(&filter->param.timestamp_range.timestamp1, buf + offset);
            serialize_date_time(&filter->param.timestamp_range.timestamp2, buf + offset + 7);
        }
        offset += 14;
        break;

    case BLE_OTS_FILTER_CURRENT_SIZE_BETWEEN:
    case BLE_OTS_FILTER_ALLOCATED_SIZE_BETWEEN:
        if ((offset + 8) <= capacity) {
            uint32_t s1 = filter->param.size_range.size1;
            uint32_t s2 = filter->param.size_range.size2;
            buf[offset + 0] = (uint8_t)(s1 & 0xFF);
            buf[offset + 1] = (uint8_t)((s1 >> 8) & 0xFF);
            buf[offset + 2] = (uint8_t)((s1 >> 16) & 0xFF);
            buf[offset + 3] = (uint8_t)((s1 >> 24) & 0xFF);
            buf[offset + 4] = (uint8_t)(s2 & 0xFF);
            buf[offset + 5] = (uint8_t)((s2 >> 8) & 0xFF);
            buf[offset + 6] = (uint8_t)((s2 >> 16) & 0xFF);
            buf[offset + 7] = (uint8_t)((s2 >> 24) & 0xFF);
        }
        offset += 8;
        break;

    default:
        break;
    }

    *buf_len = offset;
    return 0;
}

/*****************************************************************************
 * ble_ots_server_filter_get_list
 *****************************************************************************/
ble_ots_server_filtered_list_t *
ble_ots_server_filter_get_list(uint16_t conn_id)
{
    if (!s_ots_server_filter_changed) {
        return NULL;
    }

    if (!ble_ots_server_concurrency_check(conn_id)) {
        return NULL;
    }

    ble_ots_server_conn_state_t *cs = ble_ots_server_conn_state_get(conn_id);
    if (!cs) {
        return NULL;
    }

    ble_ots_server_filtered_list_t *filtered_list =
        &s_ots_server_filter_changed->filtered_list;

    memset(filtered_list, 0, sizeof(*filtered_list));

    /* Iterate all objects in the database */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[i];
        if (!obj->in_use) {
            continue;
        }

        /* Check all three filters (AND logic) */
        bool passes = true;
        for (int f = 0; f < BLE_OTS_FILTER_INSTANCE_COUNT; f++) {
            if (!object_passes_filter(obj, &cs->filter[f], conn_id)) {
                passes = false;
                break;
            }
        }

        if (passes && filtered_list->count < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS) {
            filtered_list->object_ids[filtered_list->count] = obj->object_id;
            filtered_list->count++;
        }
    }

    /* Sort the result list according to the connection's sort order */
    s_ots_server_filter_changed->sort_order = cs->sort_order;
    if (filtered_list->count > 1) {
        qsort(filtered_list->object_ids, filtered_list->count,
              sizeof(ble_ots_obj_id_t), sort_compare);
    }

    return filtered_list;
}

/*****************************************************************************
 * ble_ots_server_dispatch_obj_changed
 *****************************************************************************/
void
ble_ots_server_dispatch_obj_changed(ble_ots_obj_id_t object_id, uint8_t flags,
                                    uint16_t source_conn_id)
{
    /* DLO is excluded from Object Changed indications */
    if (object_id == BLE_OTS_OBJ_ID_DIRECTORY_LISTING) {
        return;
    }

    /* Build 7-byte indication payload: Flags (1) + Object ID (6, LE) */
    uint8_t payload[7];

    /* Determine source flag */
    if (source_conn_id != BLE_OTS_CONN_ID_NONE) {
        /* Client-originated: set bit 0 = 1 */
        flags |= BLE_OTS_OBJ_CHANGED_FLAG_SOURCE;
    } else {
        /* Server-originated: clear bit 0 */
        flags &= ~BLE_OTS_OBJ_CHANGED_FLAG_SOURCE;
    }

    payload[0] = flags;
    payload[1] = (uint8_t)(object_id & 0xFF);
    payload[2] = (uint8_t)((object_id >> 8) & 0xFF);
    payload[3] = (uint8_t)((object_id >> 16) & 0xFF);
    payload[4] = (uint8_t)((object_id >> 24) & 0xFF);
    payload[5] = (uint8_t)((object_id >> 32) & 0xFF);
    payload[6] = (uint8_t)((object_id >> 40) & 0xFF);

    /* Iterate all connections in the concurrency pool */
    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY; i++) {
        ble_ots_server_conn_state_t *cs = &ble_ots_server_env.conn_pool[i];
        if (!cs->in_pool) {
            continue;
        }

        /* Skip the source client for client-originated changes */
        if (source_conn_id != BLE_OTS_CONN_ID_NONE &&
            cs->conn_id == source_conn_id) {
            continue;
        }

        /* Send indication via the helper (which checks CCCD internally) */
        ble_ots_server_indicate_response(cs->conn_id,
                                         BLE_OTS_UUID_OBJECT_CHANGED,
                                         payload, sizeof(payload));
    }

    /* Trigger DLO rebuild for changes that affect the object database */
    ble_ots_server_dlo_rebuild();
}

/*****************************************************************************
 * ble_ots_server_dlo_rebuild
 *****************************************************************************/

/**
 * @brief Serialize the whole object database into the DLO content buffer.
 *
 * Caller must hold the OTS server mutex: the object database is read and the
 * shared dlo_content buffer is rewritten from scratch, so the rebuild has to
 * be atomic with respect to the other task.
 */
static void
dlo_rebuild_locked(void)
{
    if (!ble_ots_server_env.dlo_content) {
        ESP_LOGW(TAG, "dlo_rebuild: no DLO content buffer");
        return;
    }

    /* Collect all in-use object indices for sorting */
    int obj_indices[CONFIG_BLE_OTS_SERVER_MAX_OBJECTS];
    int obj_count = 0;

    for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
        if (ble_ots_server_env.obj_db[i].in_use) {
            obj_indices[obj_count++] = i;
        }
    }

    /* Sort by name ascending (default DLO order) */
    /* Simple insertion sort for small arrays */
    for (int i = 1; i < obj_count; i++) {
        int key = obj_indices[i];
        ble_ots_server_obj_t *key_obj = &ble_ots_server_env.obj_db[key];
        int j = i - 1;
        while (j >= 0) {
            ble_ots_server_obj_t *cmp_obj = &ble_ots_server_env.obj_db[obj_indices[j]];
            int cmp = obj_name_cmp(key_obj, cmp_obj);
            if (cmp == 0) {
                /* Tie-break on ascending Object ID.  Falling back to the obj_db
                 * slot index would not be deterministic, because slots are
                 * reused once an object is deleted. */
                cmp = obj_id_cmp(key_obj->object_id, cmp_obj->object_id);
            }
            if (cmp >= 0) {
                break;
            }
            obj_indices[j + 1] = obj_indices[j];
            j--;
        }
        obj_indices[j + 1] = key;
    }

    /* Serialize all object records into the DLO content buffer */
    uint32_t buf_offset = 0;
    uint8_t *buf = ble_ots_server_env.dlo_content;
    uint32_t buf_max = BLE_OTS_DLO_MAX_BUF_SIZE;

    for (int idx = 0; idx < obj_count; idx++) {
        ble_ots_server_obj_t *obj = &ble_ots_server_env.obj_db[obj_indices[idx]];

        /* Calculate record flags */
        uint8_t flags = 0;
        if (obj->type_uuid_len == 16) {
            flags |= BLE_OTS_DLO_FLAG_TYPE_UUID_128;
        }
        /* Always include current_size and allocated_size */
        flags |= BLE_OTS_DLO_FLAG_CURRENT_SIZE_PRESENT;
        flags |= BLE_OTS_DLO_FLAG_ALLOCATED_SIZE_PRESENT;

        /* Include first_created if not zero */
        if (!datetime_is_zero(&obj->first_created)) {
            flags |= BLE_OTS_DLO_FLAG_FIRST_CREATED_PRESENT;
        }
        /* Include last_modified if not zero */
        if (!datetime_is_zero(&obj->last_modified)) {
            flags |= BLE_OTS_DLO_FLAG_LAST_MODIFIED_PRESENT;
        }
        /* Include properties if non-zero */
        if (obj->properties != 0) {
            flags |= BLE_OTS_DLO_FLAG_PROPERTIES_PRESENT;
        }

        /* Calculate record length (including the 2-byte length field itself) */
        uint16_t record_len = 2;  /* Length field */
        record_len += 6;          /* Object ID (UINT48) */
        record_len += 1;          /* Name length */
        record_len += obj->name_len; /* Name */
        record_len += 1;          /* Flags */
        record_len += (obj->type_uuid_len == 16) ? 16 : 2; /* Type UUID */

        if (flags & BLE_OTS_DLO_FLAG_CURRENT_SIZE_PRESENT) {
            record_len += 4;
        }
        if (flags & BLE_OTS_DLO_FLAG_ALLOCATED_SIZE_PRESENT) {
            record_len += 4;
        }
        if (flags & BLE_OTS_DLO_FLAG_FIRST_CREATED_PRESENT) {
            record_len += 7;
        }
        if (flags & BLE_OTS_DLO_FLAG_LAST_MODIFIED_PRESENT) {
            record_len += 7;
        }
        if (flags & BLE_OTS_DLO_FLAG_PROPERTIES_PRESENT) {
            record_len += 4;
        }

        /* Check buffer space */
        if (buf_offset + record_len > buf_max) {
            ESP_LOGW(TAG, "dlo_rebuild: buffer overflow, truncating");
            break;
        }

        /* Write record length (UINT16, LE) */
        buf[buf_offset++] = (uint8_t)(record_len & 0xFF);
        buf[buf_offset++] = (uint8_t)((record_len >> 8) & 0xFF);

        /* Write Object ID (UINT48, LE) */
        uint64_t oid = obj->object_id;
        buf[buf_offset++] = (uint8_t)(oid & 0xFF);
        buf[buf_offset++] = (uint8_t)((oid >> 8) & 0xFF);
        buf[buf_offset++] = (uint8_t)((oid >> 16) & 0xFF);
        buf[buf_offset++] = (uint8_t)((oid >> 24) & 0xFF);
        buf[buf_offset++] = (uint8_t)((oid >> 32) & 0xFF);
        buf[buf_offset++] = (uint8_t)((oid >> 40) & 0xFF);

        /* Write Name Length (UINT8) */
        buf[buf_offset++] = obj->name_len;

        /* Write Name (UTF-8) */
        if (obj->name_len > 0) {
            memcpy(buf + buf_offset, obj->name, obj->name_len);
            buf_offset += obj->name_len;
        }

        /* Write Flags */
        buf[buf_offset++] = flags;

        /* Write Object Type UUID */
        if (obj->type_uuid_len == 16) {
            memcpy(buf + buf_offset, obj->type_uuid, 16);
            buf_offset += 16;
        } else {
            memcpy(buf + buf_offset, obj->type_uuid, 2);
            buf_offset += 2;
        }

        /* Write optional fields */
        if (flags & BLE_OTS_DLO_FLAG_CURRENT_SIZE_PRESENT) {
            uint32_t val = obj->current_size;
            buf[buf_offset++] = (uint8_t)(val & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 8) & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 16) & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 24) & 0xFF);
        }

        if (flags & BLE_OTS_DLO_FLAG_ALLOCATED_SIZE_PRESENT) {
            uint32_t val = obj->allocated_size;
            buf[buf_offset++] = (uint8_t)(val & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 8) & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 16) & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 24) & 0xFF);
        }

        if (flags & BLE_OTS_DLO_FLAG_FIRST_CREATED_PRESENT) {
            serialize_date_time(&obj->first_created, buf + buf_offset);
            buf_offset += 7;
        }

        if (flags & BLE_OTS_DLO_FLAG_LAST_MODIFIED_PRESENT) {
            serialize_date_time(&obj->last_modified, buf + buf_offset);
            buf_offset += 7;
        }

        if (flags & BLE_OTS_DLO_FLAG_PROPERTIES_PRESENT) {
            uint32_t val = obj->properties;
            buf[buf_offset++] = (uint8_t)(val & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 8) & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 16) & 0xFF);
            buf[buf_offset++] = (uint8_t)((val >> 24) & 0xFF);
        }
    }

    /* Update DLO content size */
    ble_ots_server_env.dlo_content_size = buf_offset;

    /* Update the DLO object's current_size and allocated_size */
    ble_ots_server_obj_t *dlo = ble_ots_server_obj_db_lookup(BLE_OTS_OBJ_ID_DIRECTORY_LISTING);
    if (dlo) {
        dlo->current_size = buf_offset;
        if (dlo->allocated_size < buf_offset) {
            dlo->allocated_size = buf_offset;
        }
        /* Point DLO data to the content buffer */
        dlo->data = ble_ots_server_env.dlo_content;
    }

    ESP_LOGD(TAG, "dlo_rebuild: %lu bytes, %d objects", (unsigned long)buf_offset, obj_count);
}

void
ble_ots_server_dlo_rebuild(void)
{
    /* A rebuild is triggered from both the application task (add/remove object,
     * set object data) and the NimBLE host task (metadata writes, OACP/OLCP),
     * and both rewrite the same dlo_content buffer. The mutex is recursive, so
     * nesting inside an already-locked GATT access is safe. */
    ble_ots_server_lock();
    dlo_rebuild_locked();
    ble_ots_server_unlock();
}

/*****************************************************************************
 * ble_ots_server_object_changed (Public API)
 *****************************************************************************/
int
ble_ots_server_object_changed(ble_ots_obj_id_t object_id, uint8_t flags)
{
    if (!BLE_OTS_SERVER_ENV_OK()) {
        return BLE_HS_EINVAL;
    }

    /* DLO is excluded */
    if (object_id == BLE_OTS_OBJ_ID_DIRECTORY_LISTING) {
        return BLE_HS_EINVAL;
    }

    /* Force Source of Change bit 0 = 0 (Server) */
    flags &= ~BLE_OTS_OBJ_CHANGED_FLAG_SOURCE;

    /* Dispatch with CONN_ID_NONE to indicate server-originated change */
    ble_ots_server_dispatch_obj_changed(object_id, flags, BLE_OTS_CONN_ID_NONE);

    return 0;
}

/*****************************************************************************
 * ble_ots_server_mark_object
 *****************************************************************************/
int
ble_ots_server_mark_object(uint16_t conn_id, ble_ots_obj_id_t object_id, bool mark)
{
    ble_ots_server_obj_t *obj = ble_ots_server_obj_db_lookup(object_id);
    if (!obj) {
        return BLE_HS_ENOENT;
    }

    /* Resolve conn_id to a stable bond index for the per-object marked bitmask */
    uint8_t bond_idx;
    int rc = resolve_bond_index(conn_id, &bond_idx);
    if (rc != 0) {
        ESP_LOGW(TAG, "mark_object: failed to resolve bond index for conn_id=%d",
                 conn_id);
        return rc;
    }

    uint32_t bit = (1UL << bond_idx);
    if (mark) {
        obj->marked |= bit;
    } else {
        obj->marked &= ~bit;
    }

    return 0;
}

/*****************************************************************************
 * ble_ots_server_mark_cleanup_on_disconnect
 *****************************************************************************/
void
ble_ots_server_mark_cleanup_on_disconnect(const ble_addr_t *peer_id_addr, bool bonded)
{
    if (!s_ots_server_filter_changed || !peer_id_addr) {
        return;
    }

    /* Bonded peers keep their slot and marked bits so that the marked state
     * persists across reconnections; nothing to do here. */
    if (bonded) {
        return;
    }

    ots_bond_slot_t *bond_slots = s_ots_server_filter_changed->bond_slots;

    /* Find the bond slot for this peer and release it */
    for (int s = 0; s < OTS_MAX_BOND_SLOTS; s++) {
        if (!bond_slots[s].in_use) {
            continue;
        }
        if (bond_slots[s].peer_addr.type != peer_id_addr->type ||
            memcmp(bond_slots[s].peer_addr.val, peer_id_addr->val, 6) != 0) {
            continue;
        }

        uint32_t bit = (1UL << s);
        for (int i = 0; i < CONFIG_BLE_OTS_SERVER_MAX_OBJECTS; i++) {
            ble_ots_server_env.obj_db[i].marked &= ~bit;
        }
        bond_slots[s].in_use = false;
        ESP_LOGD(TAG, "Cleared marked state for non-bonded peer slot=%d", s);
        break;
    }
}
