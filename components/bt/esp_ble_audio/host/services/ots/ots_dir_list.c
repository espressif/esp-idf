/*
 * SPDX-FileCopyrightText: 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/dlist.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/services/ots.h>
#include "ots_internal.h"
#include "ots_obj_manager_internal.h"
#include "ots_dir_list_internal.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(LEA_OTS, CONFIG_BT_ISO_LOG_LEVEL);

static struct bt_ots_dir_list dir_lists[CONFIG_BT_OTS_MAX_INST_CNT];

static size_t dir_list_object_record_size(const struct bt_gatt_ots_object *obj)
{
    uint16_t len;
    size_t obj_name_len;

    /* Record Length */
    len = sizeof(len);

    /* ID */
    len += BT_OTS_OBJ_ID_SIZE;

    /* Name length (single octet is used for the name length) */
    len += sizeof(uint8_t);

    /* Name */
    obj_name_len = strlen(obj->metadata.name);
    __ASSERT(obj_name_len > 0 && obj_name_len <= CONFIG_BT_OTS_OBJ_MAX_NAME_LEN,
             "Dir list object len is incorrect %zu", obj_name_len);
    len += obj_name_len;

    /* Flags */
    len += sizeof(uint8_t);

    /* Object type */
    if (obj->metadata.type.uuid.type == BT_UUID_TYPE_16) {
        len += BT_UUID_SIZE_16;
    } else {
        len += BT_UUID_SIZE_128;
    }

    /* Object Current size */
    len += sizeof(obj->metadata.size.cur);

    /* Object properties */
    len += sizeof(obj->metadata.props);

    __ASSERT(len >= DIR_LIST_OBJ_RECORD_MIN_SIZE,
             "Dir list object len is too small %u", len);
    __ASSERT(len <= DIR_LIST_OBJ_RECORD_MAX_SIZE,
             "Dir list object len is too large %u", len);

    return len;
}

static void dir_list_object_encode(const struct bt_gatt_ots_object *obj,
                                   struct net_buf_simple *net_buf)
{
    uint8_t flags = 0;
    uint8_t *start;
    uint16_t len;
    size_t obj_name_len;

    BT_OTS_DIR_LIST_SET_FLAG_PROPERTIES(flags);
    BT_OTS_DIR_LIST_SET_FLAG_CUR_SIZE(flags);
    if (obj->metadata.type.uuid.type == BT_UUID_TYPE_128) {
        BT_OTS_DIR_LIST_SET_FLAG_TYPE_128(flags);
    }

    /* skip 16bits at the beginning of the record for the record's length */
    start = net_buf_simple_add(net_buf, sizeof(len));

    /* ID */
    net_buf_simple_add_le48(net_buf, obj->id);

    /* Name length */
    obj_name_len = strlen(obj->metadata.name);
    __ASSERT(obj_name_len > 0 && obj_name_len <= CONFIG_BT_OTS_OBJ_MAX_NAME_LEN,
             "Dir list object len is incorrect %zu", obj_name_len);
    net_buf_simple_add_u8(net_buf, obj_name_len);

    /* Name */
    net_buf_simple_add_mem(net_buf, obj->metadata.name, obj_name_len);

    /* Flags */
    net_buf_simple_add_u8(net_buf, flags);

    /* encode Object type */
    if (obj->metadata.type.uuid.type == BT_UUID_TYPE_16) {
        net_buf_simple_add_le16(net_buf, obj->metadata.type.uuid_16.val);
    } else {
        net_buf_simple_add_mem(net_buf, obj->metadata.type.uuid_128.val,
                               BT_UUID_SIZE_128);
    }

    /* encode Object Current size */
    net_buf_simple_add_le32(net_buf, obj->metadata.size.cur);

    /* Object properties */
    net_buf_simple_add_le32(net_buf, obj->metadata.props);

    len = net_buf_simple_tail(net_buf) - start;

    __ASSERT(len >= DIR_LIST_OBJ_RECORD_MIN_SIZE,
             "Dir list object len is too small %u", len);
    __ASSERT(len <= DIR_LIST_OBJ_RECORD_MAX_SIZE,
             "Dir list object len is too large %u", len);

    /* Update the record length at the beginning */
    sys_put_le16(len, start);
}

static void bt_ots_dir_list_reset_anchor(struct bt_ots_dir_list *dir_list, void *obj_manager)
{
    int err;

    /* Reset the dir_list - Ignore any error as we can't do anything about it anyways */
    err = bt_gatt_ots_obj_manager_first_obj_get(obj_manager, &dir_list->anchor_object);
    if (err) {
        return;
    }

    dir_list->anchor_offset = 0;
    return;
}

static int bt_ots_dir_list_search_forward(struct bt_ots_dir_list *dir_list, void *obj_manager,
                                          off_t offset)
{
    int err;
    char id_str[BT_OTS_OBJ_ID_STR_LEN];
    struct bt_gatt_ots_object *obj = dir_list->anchor_object;
    size_t rec_len = dir_list_object_record_size(obj);

    bt_ots_obj_id_to_str(obj->id, id_str, sizeof(id_str));
    LOG_DBG("Searching forward for offset %ld starting at %ld with object ID %s",
            (long)offset, (long)dir_list->anchor_offset, id_str);

    while (dir_list->anchor_offset + rec_len <= offset) {

        err = bt_gatt_ots_obj_manager_next_obj_get(obj_manager, obj, &obj);
        if (err) {
            return err;
        }

        dir_list->anchor_offset += rec_len;
        dir_list->anchor_object = obj;

        rec_len = dir_list_object_record_size(obj);
    }

    return 0;
}

static int bt_ots_dir_list_search_backward(struct bt_ots_dir_list *dir_list, void *obj_manager,
                                           off_t offset)
{
    int err;
    char id_str[BT_OTS_OBJ_ID_STR_LEN];
    struct bt_gatt_ots_object *obj = dir_list->anchor_object;

    bt_ots_obj_id_to_str(obj->id, id_str, sizeof(id_str));
    LOG_DBG("Searching backward for offset %ld starting at %ld with object ID %s",
            (long)offset, (long)dir_list->anchor_offset, id_str);

    while (dir_list->anchor_offset > offset) {

        err = bt_gatt_ots_obj_manager_prev_obj_get(obj_manager, obj, &obj);
        if (err) {
            return err;
        }

        dir_list->anchor_offset -= dir_list_object_record_size(obj);
        dir_list->anchor_object = obj;
    }

    return 0;
}

static int bt_ots_dir_list_search(struct bt_ots_dir_list *dir_list, void *obj_manager, off_t offset)
{
    int err = 0;
    char id_str[BT_OTS_OBJ_ID_STR_LEN];

    /* decide start location and direction of movement based on offset, we can only choose
     * current anchor point, beginning, or end as those are the only places where we know
     * the associated object that builds up the record.
     */
    if (offset >= dir_list->anchor_offset) {
        const size_t last = dir_list->dir_list_obj->metadata.size.cur;
        const size_t mid = dir_list->anchor_offset + (last - dir_list->anchor_offset) / 2;

        if (offset < mid) {
            err = bt_ots_dir_list_search_forward(dir_list, obj_manager, offset);
        } else {
            size_t rec_len;

            LOG_DBG("Offset %ld is closer to %zu than %ld, start from end",
                    (long)offset, last, (long)dir_list->anchor_offset);
            err = bt_gatt_ots_obj_manager_last_obj_get(obj_manager, &dir_list->anchor_object);
            if (err) {
                return err;
            }
            rec_len = dir_list_object_record_size(dir_list->anchor_object);
            dir_list->anchor_offset = last - rec_len;
            err = bt_ots_dir_list_search_backward(dir_list, obj_manager, offset);
        }
    } else {
        const size_t mid = dir_list->anchor_offset / 2;

        if (offset < mid) {
            LOG_DBG("Offset %ld is closer to 0 than %ld, start from beginning",
                    (long)offset, (long)dir_list->anchor_offset);
            bt_ots_dir_list_reset_anchor(dir_list, obj_manager);
            err = bt_ots_dir_list_search_forward(dir_list, obj_manager, offset);
        } else {
            err = bt_ots_dir_list_search_backward(dir_list, obj_manager, offset);
        }
    }

    if (err) {
        return err;
    }

    bt_ots_obj_id_to_str(dir_list->anchor_object->id, id_str, sizeof(id_str));
    LOG_DBG("Found offset %ld starting at %ld in object with ID %s",
            (long)offset, (long)dir_list->anchor_offset, id_str);

    return 0;
}

static void dir_list_update_size(struct bt_ots_dir_list *dir_list, void *obj_manager)
{
    struct bt_gatt_ots_object *obj;
    int err;
    size_t len = 0;

    err = bt_gatt_ots_obj_manager_first_obj_get(obj_manager, &obj);

    __ASSERT(err == 0 && obj == dir_list->dir_list_obj,
             "Expecting first object to be the Directory Listing Object");

    do {
        len += dir_list_object_record_size(obj);

        err = bt_gatt_ots_obj_manager_next_obj_get(obj_manager, obj, &obj);
    } while (!err);

    LOG_DBG("Update directory listing current size to 0x%zx", len);
    dir_list->dir_list_obj->metadata.size.cur = len;
}

void bt_ots_dir_list_selected(struct bt_ots_dir_list *dir_list, void *obj_manager,
                              struct bt_gatt_ots_object *cur_obj)
{
    if (dir_list->dir_list_obj != cur_obj) {
        /* We only need to update the object directory listing if it is currently selected,
         * as we otherwise only create it when it is selected.
         */
        return;
    }

    bt_ots_dir_list_reset_anchor(dir_list, obj_manager);
    dir_list_update_size(dir_list, obj_manager);
}

void bt_ots_dir_list_init(struct bt_ots_dir_list **dir_list, void *obj_manager)
{
    struct bt_gatt_ots_object *dir_list_obj;
    int err;
    static char *dir_list_obj_name = CONFIG_BT_OTS_DIR_LIST_OBJ_NAME;

    __ASSERT(*dir_list == NULL, "Already initialized");

    for (size_t i = 0; i < ARRAY_SIZE(dir_lists); i++) {
        if (!dir_lists[i].dir_list_obj) {
            *dir_list = &dir_lists[i];
        }
    }

    __ASSERT(*dir_list, "Could not assign Directory Listing Object");

    __ASSERT(strlen(dir_list_obj_name) <= CONFIG_BT_OTS_OBJ_MAX_NAME_LEN,
             "BT_OTS_DIR_LIST_OBJ_NAME shall be less than or equal to %u octets",
             CONFIG_BT_OTS_OBJ_MAX_NAME_LEN);

    err = bt_gatt_ots_obj_manager_obj_add(obj_manager, &dir_list_obj);

    __ASSERT(!err, "Could not add Directory Listing Object for object manager %p", obj_manager);

    memset(&dir_list_obj->metadata, 0, sizeof(dir_list_obj->metadata));
    dir_list_obj->metadata.name = dir_list_obj_name;
    dir_list_obj->metadata.size.alloc = DIR_LIST_MAX_SIZE;
    dir_list_obj->metadata.type.uuid.type = BT_UUID_TYPE_16;
    dir_list_obj->metadata.type.uuid_16.val = BT_UUID_OTS_DIRECTORY_LISTING_VAL;
    BT_OTS_OBJ_SET_PROP_READ(dir_list_obj->metadata.props);

    (*dir_list)->dir_list_obj = dir_list_obj;

    bt_ots_dir_list_reset_anchor(*dir_list, obj_manager);
    dir_list_update_size(*dir_list, obj_manager);
}

ssize_t bt_ots_dir_list_content_get(struct bt_ots_dir_list *dir_list, void *obj_manager,
                                    void **data, size_t len, off_t offset)
{
    int err;
    size_t last_rec_len;
    size_t rec_len;
    off_t rec_offset;
    struct bt_gatt_ots_object *obj;

    err = bt_ots_dir_list_search(dir_list, obj_manager, offset);
    if (err) {
        return err;
    }

    net_buf_simple_init_with_data(&dir_list->net_buf, dir_list->_content,
                                  sizeof(dir_list->_content));
    net_buf_simple_reset(&dir_list->net_buf);

    obj = dir_list->anchor_object;
    rec_offset = dir_list->anchor_offset;

    last_rec_len = 0;
    rec_len = dir_list_object_record_size(obj);
    while (net_buf_simple_tailroom(&dir_list->net_buf) >= rec_len) {

        dir_list_object_encode(obj, &dir_list->net_buf);

        dir_list->anchor_object = obj;
        dir_list->anchor_offset += last_rec_len;

        if (dir_list->net_buf.len - (offset - rec_offset) >= len) {
            /* we have encoded as much data as the client has asked */
            break;
        }

        err = bt_gatt_ots_obj_manager_next_obj_get(obj_manager, obj, &obj);
        if (err) {
            /* there are no more objects to encode */
            break;
        }

        last_rec_len = rec_len;
        rec_len = dir_list_object_record_size(obj);
    }

    *data = dir_list->net_buf.data + (offset - rec_offset);

    return MIN(len, dir_list->net_buf.len - (offset - rec_offset));
}

bool bt_ots_dir_list_is_idle(const struct bt_ots_dir_list *dir_list)
{
    return dir_list->dir_list_obj->state.type == BT_GATT_OTS_OBJECT_IDLE_STATE;
}
