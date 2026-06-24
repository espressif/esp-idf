/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "dfu_slot.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mesh/common.h"
#include "mesh/utils.h"
#include "settings_nvs.h"
#include "settings.h"

#if CONFIG_BLE_MESH_DFU_SLOTS
#define SLOT_ENTRY_BUFLEN 25

#define DFU_SLOT_SETTINGS_PATH "mesh/dfu/s"

#define HEADER_SIZE offsetof(struct slot, slot.fwid)

#define PROP_HEADER "h"
#define PROP_FWID "id"
#define PROP_METADATA "m"
#define SETTINGS_NAME_END '='
#define SETTINGS_NAME_SEPARATOR '/'

static sys_slist_t list;

static struct slot {
    uint32_t idx;
    struct bt_mesh_dfu_slot slot;
    sys_snode_t n;
} slots[CONFIG_BLE_MESH_DFU_SLOT_CNT];

static uint32_t slot_index;

inline int settings_name_next(const char *name, const char **next)
{
    int rc = 0;

    if (next) {
        *next = NULL;
    }

    if (!name) {
        return 0;
    }

    /* name might come from flash directly, in flash the name would end
     * with '=' or '\0' depending how storage is done. Flash reading is
     * limited to what can be read
     */
    while ((*name != '\0') && (*name != SETTINGS_NAME_END) &&
            (*name != SETTINGS_NAME_SEPARATOR)) {
        rc++;
        name++;
    }

    if (*name == SETTINGS_NAME_SEPARATOR) {
        if (next) {
            *next = name + 1;
        }
        return rc;
    }

    return rc;
}

#if CONFIG_BLE_MESH_SETTINGS
static char *slot_entry_encode(uint16_t idx, char buf[SLOT_ENTRY_BUFLEN],
                               const char *property)
{
    snprintf(buf, SLOT_ENTRY_BUFLEN, DFU_SLOT_SETTINGS_PATH "/%x/%s", idx,
             property);

    return buf;
}
#endif

static bool slot_eq(const struct bt_mesh_dfu_slot *slot,
                    const uint8_t *fwid, size_t fwid_len)
{
    return (slot->fwid_len == fwid_len) &&
           !memcmp(fwid, slot->fwid, fwid_len);
}

static bool is_slot_committed(struct slot *slot_to_check)
{
    struct slot *s;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n) {
        if (s == slot_to_check) {
            return true;
        }
    }

    return false;
}

static int slot_store(const struct slot *slot_to_store)
{
    int err = 0;
#if CONFIG_BLE_MESH_SETTINGS
    uint16_t idx = ARRAY_INDEX(slots, slot_to_store);
    char buf[SLOT_ENTRY_BUFLEN];

    err = bt_mesh_save_core_settings(slot_entry_encode(idx, buf, PROP_HEADER),
                                     (uint8_t *)slot_to_store, HEADER_SIZE);
    if (err) {
        return err;
    }

    err = bt_mesh_save_core_settings(slot_entry_encode(idx, buf, PROP_FWID),
                                     slot_to_store->slot.fwid, slot_to_store->slot.fwid_len);
    if (err) {
        return err;
    }

    err = bt_mesh_save_core_settings(slot_entry_encode(idx, buf, PROP_METADATA),
                                     slot_to_store->slot.metadata, slot_to_store->slot.metadata_len);

#endif
    return err;
}

static void slot_erase(struct slot *slot_to_erase)
{
#if CONFIG_BLE_MESH_SETTINGS
    uint16_t idx = ARRAY_INDEX(slots, slot_to_erase);
    char buf[SLOT_ENTRY_BUFLEN];

    bt_mesh_erase_core_settings(slot_entry_encode(idx, buf, PROP_HEADER));
    bt_mesh_erase_core_settings(slot_entry_encode(idx, buf, PROP_FWID));
    bt_mesh_erase_core_settings(slot_entry_encode(idx, buf, PROP_METADATA));
#endif
}

static void slot_index_defrag(void)
{
    slot_index = 0;
    struct slot *s;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n) {
        s->idx = ++slot_index;
        slot_store(s);
    }
}

int bt_mesh_dfu_slot_count(void)
{
    int cnt = 0;
    sys_snode_t *n;

    SYS_SLIST_FOR_EACH_NODE(&list, n) {
        cnt++;
    }

    return cnt;
}

struct bt_mesh_dfu_slot *bt_mesh_dfu_slot_reserve(void)
{
    struct slot *slot = NULL;

    for (int i = 0; i < ARRAY_SIZE(slots); ++i) {
        if (slots[i].idx == 0) {
            slot = &slots[i];
            break;
        }
    }

    if (!slot) {
        BT_WARN("No space");
        return NULL;
    }

    if (slot_index == UINT32_MAX) {
        slot_index_defrag();
    }

    slot->slot.fwid_len = 0;
    slot->slot.metadata_len = 0;
    slot->slot.size = 0;
    slot->idx = ++slot_index;

    BT_DBG("Reserved slot #%u", slot - &slots[0]);

    return &slot->slot;
}

int bt_mesh_dfu_slot_info_set(struct bt_mesh_dfu_slot *dfu_slot, size_t size,
                              const uint8_t *metadata, size_t metadata_len)
{
    struct slot *slot = CONTAINER_OF(dfu_slot, struct slot, slot);

    if (metadata_len > CONFIG_BLE_MESH_DFU_METADATA_MAXLEN) {
        return -EFBIG;
    }

    if (slot->idx == 0 || is_slot_committed(slot)) {
        return -EINVAL;
    }

    slot->slot.size = size;
    slot->slot.metadata_len = metadata_len;
    memcpy(slot->slot.metadata, metadata, metadata_len);
    return 0;
}

int bt_mesh_dfu_slot_fwid_set(struct bt_mesh_dfu_slot *dfu_slot,
                              const uint8_t *fwid, size_t fwid_len)
{
    struct slot *slot = CONTAINER_OF(dfu_slot, struct slot, slot);

    if (fwid_len > CONFIG_BLE_MESH_DFU_FWID_MAXLEN) {
        return -EFBIG;
    }

    if (slot->idx == 0) {
        return -EINVAL;
    }

    if (is_slot_committed(slot)) {
        return -EEXIST;
    }

    for (int i = 0; i < ARRAY_SIZE(slots); i++) {
        if (slots[i].idx != 0 &&
                slot_eq(&slots[i].slot, fwid, fwid_len)) {
            return -EALREADY;
        }
    }

    slot->slot.fwid_len = fwid_len;
    memcpy(slot->slot.fwid, fwid, fwid_len);
    return 0;
}

int bt_mesh_dfu_slot_commit(struct bt_mesh_dfu_slot *dfu_slot)
{
    int err;
    struct slot *slot = CONTAINER_OF(dfu_slot, struct slot, slot);

    if (slot->idx == 0 ||
            slot->slot.fwid_len == 0 ||
            slot->slot.size == 0 ||
            is_slot_committed(slot)) {
        return -EINVAL;
    }

    err = slot_store(slot);
    if (err) {
        BT_WARN("Store failed (err: %d)", err);
        return err;
    }

    sys_slist_append(&list, &slot->n);

    BT_DBG("Stored slot #%u: %s", ARRAY_INDEX(slots, slot),
           bt_hex(slot->slot.fwid, slot->slot.fwid_len));
    return 0;
}

void bt_mesh_dfu_slot_release(const struct bt_mesh_dfu_slot *dfu_slot)
{
    struct slot *slot = CONTAINER_OF(dfu_slot, struct slot, slot);

    if (is_slot_committed(slot)) {
        return;
    }

    slot->idx = 0;
}

int bt_mesh_dfu_slot_del(const struct bt_mesh_dfu_slot *dfu_slot)
{
    struct slot *slot = CONTAINER_OF(dfu_slot, struct slot, slot);

    if (!sys_slist_find_and_remove(&list, &slot->n)) {
        return -EINVAL;
    }

    int idx = ARRAY_INDEX(slots, slot);
    ARG_UNUSED(idx);

    BT_DBG("%u", idx);

    slot_erase(slot);
    slot->idx = 0;

    return 0;
}

void bt_mesh_dfu_slot_del_all(void)
{
    struct slot *s;
    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n) {
        slot_erase(s);
        s->idx = 0;
    }

    sys_slist_init(&list);
}

const struct bt_mesh_dfu_slot *bt_mesh_dfu_slot_at(uint16_t img_idx)
{
    struct slot *s;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n) {
        if (!img_idx--) {
            return &s->slot;
        }
    }

    return NULL;
}

int bt_mesh_dfu_slot_get(const uint8_t *fwid, size_t fwid_len, struct bt_mesh_dfu_slot **slot)
{
    struct slot *s;
    int idx = 0;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n) {
        if (slot_eq(&s->slot, fwid, fwid_len)) {
            if (slot) {
                *slot = &s->slot;
            }
            return idx;
        }
        idx++;
    }

    return -ENOENT;
}

int bt_mesh_dfu_slot_img_idx_get(const struct bt_mesh_dfu_slot *dfu_slot)
{
    struct slot *s;
    int idx = 0;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n) {
        if (&s->slot == dfu_slot) {
            return idx;
        }
        idx++;
    }

    return -ENOENT;
}

size_t bt_mesh_dfu_slot_foreach(bt_mesh_dfu_slot_cb_t cb, void *user_data)
{
    enum bt_mesh_dfu_iter iter;
    size_t cnt = 0;
    struct slot *s;

    SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n) {
        cnt++;

        if (!cb) {
            continue;
        }

        iter = cb(&s->slot, user_data);
        if (iter != BLE_MESH_DFU_ITER_CONTINUE) {
            break;
        }
    }

    return cnt;
}

#if 0
static int slot_data_load(const char *key, size_t len_rd,
                          settings_read_cb read_cb, void *cb_arg)
{
    const char *prop;
    size_t len;
    uint16_t idx;

    idx = strtol(key, NULL, 16);

    if (idx >= ARRAY_SIZE(slots)) {
        return 0;
    }

    len = settings_name_next(key, &prop);

    if (!strncmp(prop, PROP_HEADER, len)) {
        if (read_cb(cb_arg, &slots[idx], HEADER_SIZE) > 0) {
            struct slot *s, *prev = NULL;

            SYS_SLIST_FOR_EACH_CONTAINER(&list, s, n) {
                if (s->idx > slots[idx].idx) {
                    break;
                }

                prev = s;
            }

            if (prev == NULL) {
                sys_slist_prepend(&list, &slots[idx].n);
            } else {
                sys_slist_insert(&list, &prev->n, &slots[idx].n);
            }

            if (slots[idx].idx >= slot_index) {
                slot_index = slots[idx].idx + 1;
            }
        }
        return 0;
    }

    if (!strncmp(prop, PROP_FWID, len)) {
        if (read_cb(cb_arg, &slots[idx].slot.fwid,
                    sizeof(slots[idx].slot.fwid)) < 0) {
            slots[idx].idx = 0;
            sys_slist_find_and_remove(&list, &slots[idx].n);
            return 0;
        }

        slots[idx].slot.fwid_len = len_rd;
        return 0;
    }

    if (!strncmp(prop, PROP_METADATA, len)) {
        if (read_cb(cb_arg, &slots[idx].slot.metadata,
                    sizeof(slots[idx].slot.metadata)) < 0) {
            slots[idx].idx = 0;
            sys_slist_find_and_remove(&list, &slots[idx].n);
            return 0;
        }

        slots[idx].slot.metadata_len = len_rd;
        return 0;
    }

    return 0;
}
#endif
#endif /* CONFIG_BLE_MESH_DFU_SLOTS */
