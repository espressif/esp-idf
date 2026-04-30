/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/l2cap.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/sys/check.h>

#include <zephyr/bluetooth/services/ots.h>
#include "ots_internal.h"
#include "ots_obj_manager_internal.h"
#include "ots_dir_list_internal.h"

#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(LEA_OTS, CONFIG_BT_ISO_LOG_LEVEL);

#if defined(CONFIG_BT_OTS_OACP_CREATE_SUPPORT)
#define OACP_FEAT_BIT_CREATE BIT(BT_OTS_OACP_FEAT_CREATE)
#else
#define OACP_FEAT_BIT_CREATE 0
#endif

#if defined(CONFIG_BT_OTS_OACP_DELETE_SUPPORT)
#define OACP_FEAT_BIT_DELETE BIT(BT_OTS_OACP_FEAT_DELETE)
#else
#define OACP_FEAT_BIT_DELETE 0
#endif

#if defined(CONFIG_BT_OTS_OACP_CHECKSUM_SUPPORT)
#define OACP_FEAT_BIT_CRC BIT(BT_OTS_OACP_FEAT_CHECKSUM)
#else
#define OACP_FEAT_BIT_CRC 0
#endif

#if defined(CONFIG_BT_OTS_OACP_READ_SUPPORT)
#define OACP_FEAT_BIT_READ BIT(BT_OTS_OACP_FEAT_READ)
#else
#define OACP_FEAT_BIT_READ 0
#endif

#if defined(CONFIG_BT_OTS_OACP_WRITE_SUPPORT)
#define OACP_FEAT_BIT_WRITE BIT(BT_OTS_OACP_FEAT_WRITE)
#else
#define OACP_FEAT_BIT_WRITE 0
#endif

#if defined(CONFIG_BT_OTS_OACP_PATCH_SUPPORT)
#define OACP_FEAT_BIT_PATCH BIT(BT_OTS_OACP_FEAT_PATCH)
#else
#define OACP_FEAT_BIT_PATCH 0
#endif

/* OACP features supported by Kconfig */
#define OACP_FEAT (     \
    OACP_FEAT_BIT_CREATE |  \
    OACP_FEAT_BIT_DELETE |  \
    OACP_FEAT_BIT_CRC |     \
    OACP_FEAT_BIT_READ |    \
    OACP_FEAT_BIT_WRITE |   \
    OACP_FEAT_BIT_PATCH)

#if defined(CONFIG_BT_OTS_OLCP_GO_TO_SUPPORT)
#define OLCP_FEAT_BIT_GOTO BIT(BT_OTS_OLCP_FEAT_GO_TO)
#else
#define OLCP_FEAT_BIT_GOTO 0
#endif

/* OLCP features supported by Kconfig */
#define OLCP_FEAT OLCP_FEAT_BIT_GOTO

static bool ots_obj_validate_prop_against_oacp(uint32_t prop, uint32_t oacp)
{
    if (BT_OTS_OBJ_GET_PROP_DELETE(prop) > 0 && BT_OTS_OACP_GET_FEAT_DELETE(oacp) == 0) {
        return false;
    }

    if (BT_OTS_OBJ_GET_PROP_EXECUTE(prop) > 0 && BT_OTS_OACP_GET_FEAT_EXECUTE(oacp) == 0) {
        return false;
    }

    if (BT_OTS_OBJ_GET_PROP_READ(prop) > 0 && BT_OTS_OACP_GET_FEAT_READ(oacp) == 0) {
        return false;
    }

    if (BT_OTS_OBJ_GET_PROP_WRITE(prop) > 0 && BT_OTS_OACP_GET_FEAT_WRITE(oacp) == 0) {
        return false;
    }

    if (BT_OTS_OBJ_GET_PROP_APPEND(prop) > 0 && BT_OTS_OACP_GET_FEAT_APPEND(oacp) == 0) {
        return false;
    }

    if (BT_OTS_OBJ_GET_PROP_TRUNCATE(prop) > 0 && BT_OTS_OACP_GET_FEAT_TRUNCATE(oacp) == 0) {
        return false;
    }

    if (BT_OTS_OBJ_GET_PROP_PATCH(prop) > 0 && BT_OTS_OACP_GET_FEAT_PATCH(oacp) == 0) {
        return false;
    }

    return true;
}

static ssize_t ots_feature_read(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr, void *buf,
                                uint16_t len, uint16_t offset)
{
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;

    LOG_DBG("OTS Feature GATT Read Operation");

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &ots->features,
                             sizeof(ots->features));
}

static ssize_t ots_obj_name_read(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr, void *buf,
                                 uint16_t len, uint16_t offset)
{
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;

    LOG_DBG("OTS Object Name GATT Read Operation");

    if (!ots->cur_obj) {
        LOG_DBG("No Current Object selected in OTS!");
        return BT_GATT_ERR(BT_GATT_OTS_OBJECT_NOT_SELECTED);
    }

    if (!ots->cur_obj->metadata.name) {
        LOG_DBG("Current Object has no name!");
        return BT_GATT_ERR(BT_GATT_OTS_OBJECT_NOT_SELECTED);
    }

    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             ots->cur_obj->metadata.name,
                             strlen(ots->cur_obj->metadata.name));
}

#if defined(CONFIG_BT_OTS_OBJ_NAME_WRITE_SUPPORT)
ssize_t ots_obj_name_write(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr,
                           const void *buf, uint16_t len,
                           uint16_t offset, uint8_t flags)
{
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;
    struct bt_gatt_ots_object *obj = NULL;
    int rc = 0;
    char name[CONFIG_BT_OTS_OBJ_MAX_NAME_LEN + 1];

    LOG_DBG("OTS Object Name GATT Write Operation");

    if (!ots->cur_obj) {
        LOG_DBG("No Current Object selected in OTS!");
        return BT_GATT_ERR(BT_GATT_OTS_OBJECT_NOT_SELECTED);
    }

    if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) &&
            ots->cur_obj->id == OTS_OBJ_ID_DIR_LIST) {
        LOG_DBG("Rejecting name write for the directory list object.");
        return BT_GATT_ERR(BT_GATT_OTS_WRITE_REQUEST_REJECTED);
    }

    if (offset > 0) {
        LOG_DBG("Rejecting a long write, offset must be 0!");
        return BT_GATT_ERR(BT_GATT_OTS_WRITE_REQUEST_REJECTED);
    }

    if (len == 0 || len > CONFIG_BT_OTS_OBJ_MAX_NAME_LEN) {
        LOG_DBG("Invalid object name length!");
        return BT_GATT_ERR(BT_GATT_OTS_WRITE_REQUEST_REJECTED);
    }

    /* Construct a temporary name for duplication detection */
    memcpy(name, buf, len);
    name[len] = '\0';

    rc = bt_gatt_ots_obj_manager_first_obj_get(ots->obj_manager, &obj);
    while (rc == 0) {
        if (obj != ots->cur_obj && strcmp(name, obj->metadata.name) == 0) {
            LOG_DBG("Object name is duplicated!");
            return BT_GATT_ERR(BT_GATT_OTS_OBJECT_NAME_ALREADY_EXISTS);
        }
        rc = bt_gatt_ots_obj_manager_next_obj_get(ots->obj_manager, obj, &obj);
    }

    /* No duplicate detected, notify application and update real object name */
    if (ots->cb->obj_name_written) {
        ots->cb->obj_name_written(ots, conn, ots->cur_obj->id,
                                  ots->cur_obj->metadata.name, name);
    }

    /* Ensure the name fits; use strncpy or ensure metadata.name buffer
     * is always allocated to CONFIG_BT_OTS_OBJ_MAX_NAME_LEN + 1.
     */
    strncpy(ots->cur_obj->metadata.name, name, CONFIG_BT_OTS_OBJ_MAX_NAME_LEN);
    ots->cur_obj->metadata.name[CONFIG_BT_OTS_OBJ_MAX_NAME_LEN] = '\0';

    return len;
}
#endif

static ssize_t ots_obj_type_read(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr, void *buf,
                                 uint16_t len, uint16_t offset)
{
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;
    struct bt_ots_obj_metadata *obj_meta;

    LOG_DBG("OTS Object Type GATT Read Operation");

    if (!ots->cur_obj) {
        LOG_DBG("No Current Object selected in OTS!");
        return BT_GATT_ERR(BT_GATT_OTS_OBJECT_NOT_SELECTED);
    }

    obj_meta = &ots->cur_obj->metadata;
    switch (obj_meta->type.uuid.type) {
    case BT_UUID_TYPE_16:
        return bt_gatt_attr_read(conn, attr, buf, len, offset,
                                 &obj_meta->type.uuid_16.val,
                                 sizeof(obj_meta->type.uuid_16.val));
    case BT_UUID_TYPE_128:
        return bt_gatt_attr_read(conn, attr, buf, len, offset,
                                 obj_meta->type.uuid_128.val,
                                 sizeof(obj_meta->type.uuid_128.val));
    default:
        return -EINVAL;
    }
}

static ssize_t ots_obj_size_read(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr, void *buf,
                                 uint16_t len, uint16_t offset)
{
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;

    LOG_DBG("OTS Object Size GATT Read Operation");

    if (!ots->cur_obj) {
        LOG_DBG("No Current Object selected in OTS!");
        return BT_GATT_ERR(BT_GATT_OTS_OBJECT_NOT_SELECTED);
    }

    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             &ots->cur_obj->metadata.size,
                             sizeof(ots->cur_obj->metadata.size));
}

static ssize_t ots_obj_id_read(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr, void *buf,
                               uint16_t len, uint16_t offset)
{
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;
    uint8_t id[BT_OTS_OBJ_ID_SIZE];
    char id_str[BT_OTS_OBJ_ID_STR_LEN];

    LOG_DBG("OTS Object ID GATT Read Operation");

    if (!ots->cur_obj) {
        LOG_DBG("No Current Object selected in OTS!");
        return BT_GATT_ERR(BT_GATT_OTS_OBJECT_NOT_SELECTED);
    }

    sys_put_le48(ots->cur_obj->id, id);

    bt_ots_obj_id_to_str(ots->cur_obj->id, id_str,
                         sizeof(id_str));
    LOG_DBG("Current Object ID: %s", id_str);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, id, sizeof(id));
}

static ssize_t ots_obj_prop_read(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr, void *buf,
                                 uint16_t len, uint16_t offset)
{
    struct bt_ots *ots = (struct bt_ots *) attr->user_data;

    LOG_DBG("OTS Object Properties GATT Read Operation");

    if (!ots->cur_obj) {
        LOG_DBG("No Current Object selected in OTS!");
        return BT_GATT_ERR(BT_GATT_OTS_OBJECT_NOT_SELECTED);
    }

    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             &ots->cur_obj->metadata.props,
                             sizeof(ots->cur_obj->metadata.props));
}

int bt_ots_obj_add_internal(struct bt_ots *ots, struct bt_conn *conn,
                            const struct bt_ots_obj_add_param *param,
                            struct bt_gatt_ots_object **obj)
{
    int err;
    struct bt_gatt_ots_object *new_obj;
    struct bt_ots_obj_created_desc created_desc;

    if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) && ots->dir_list &&
            !bt_ots_dir_list_is_idle(ots->dir_list)) {
        LOG_DBG("Directory Listing Object is being read");
        return -EBUSY;
    }

    err = bt_gatt_ots_obj_manager_obj_add(ots->obj_manager, &new_obj);
    if (err) {
        LOG_ERR("No space available in the object manager");
        return err;
    }

    (void)memset(&created_desc, 0, sizeof(created_desc));

    if (ots->cb->obj_created) {
        err = ots->cb->obj_created(ots, conn, new_obj->id, param, &created_desc);

        if (err) {
            (void)bt_gatt_ots_obj_manager_obj_delete(new_obj);

            return err;
        }

        if (!ots_obj_validate_prop_against_oacp(created_desc.props, ots->features.oacp)) {
            LOG_ERR("Object properties (0x%04X) are not a subset of OACP (0x%04X)",
                    created_desc.props, ots->features.oacp);

            (void)bt_ots_obj_delete(ots, new_obj->id);
            return -ECANCELED;
        }

        if (created_desc.name == NULL) {
            LOG_ERR("Object name must be set by application after object creation.");

            (void)bt_ots_obj_delete(ots, new_obj->id);
            return -ECANCELED;
        }

        if (created_desc.size.alloc < param->size) {
            LOG_ERR("Object allocated size must >= requested size.");

            (void)bt_ots_obj_delete(ots, new_obj->id);
            return -ECANCELED;
        }
    } else {
        /* obj_created callback is required to populate the descriptor */
        LOG_ERR("obj_created callback is not set");

        (void)bt_gatt_ots_obj_manager_obj_delete(new_obj);
        return -EINVAL;
    }

    new_obj->metadata.type = param->type;
    new_obj->metadata.name = created_desc.name;
    new_obj->metadata.size = created_desc.size;
    new_obj->metadata.props = created_desc.props;

    if (obj) {
        *obj = new_obj;
    }

    return 0;
}

int bt_ots_obj_add(struct bt_ots *ots, const struct bt_ots_obj_add_param *param)
{
    int err;
    size_t name_len;
    struct bt_gatt_ots_object *obj;

    err = bt_ots_obj_add_internal(ots, NULL, param, &obj);
    if (err) {
        return err;
    }

    if (obj->metadata.name == NULL) {
        LOG_ERR("Object name is NULL");

        (void)bt_ots_obj_delete(ots, obj->id);
        return -ECANCELED;
    }

    name_len = strlen(obj->metadata.name);
    if (name_len == 0 || name_len > CONFIG_BT_OTS_OBJ_MAX_NAME_LEN) {
        LOG_ERR("Invalid name length %zu", name_len);

        (void)bt_ots_obj_delete(ots, obj->id);
        return -ECANCELED;
    }

    if (obj->metadata.size.cur > param->size) {
        LOG_ERR("Object current size must be less than or equal to requested size.");

        (void)bt_ots_obj_delete(ots, obj->id);
        return -ECANCELED;
    }

    return obj->id;
}

int bt_ots_obj_delete(struct bt_ots *ots, uint64_t id)
{
    int err;
    struct bt_gatt_ots_object *obj;

    CHECKIF(!BT_OTS_VALID_OBJ_ID(id)) {
        LOG_DBG("Invalid object ID 0x%016llx", id);

        return -EINVAL;
    }

    err = bt_gatt_ots_obj_manager_obj_get(ots->obj_manager, id, &obj);
    if (err) {
        return err;
    }

    if (obj->state.type != BT_GATT_OTS_OBJECT_IDLE_STATE) {
        return -EBUSY;
    }

    if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) && ots->dir_list &&
            !bt_ots_dir_list_is_idle(ots->dir_list)) {
        LOG_DBG("Directory Listing Object is being read");
        return -EBUSY;
    }

    if (ots->cb->obj_deleted) {
        err = ots->cb->obj_deleted(ots, NULL, obj->id);
        if (err) {
            return err;
        }
    }

    err = bt_gatt_ots_obj_manager_obj_delete(obj);
    if (err) {
        return err;
    }

    if (ots->cur_obj == obj) {
        ots->cur_obj = NULL;
    }

    return 0;
}

#if defined(CONFIG_BT_OTS_SECONDARY_SVC)
void *bt_ots_svc_decl_get(struct bt_ots *ots)
{
    return ots->service->attrs;
}
#endif

static void oacp_indicate_work_handler(struct k_work *work)
{
    struct bt_gatt_ots_indicate *ind = CONTAINER_OF(work, struct bt_gatt_ots_indicate, work);
    struct bt_ots *ots = CONTAINER_OF(ind, struct bt_ots, oacp_ind);

    bt_gatt_indicate(NULL, &ots->oacp_ind.params);
}

static void olcp_indicate_work_handler(struct k_work *work)
{
    struct bt_gatt_ots_indicate *ind = CONTAINER_OF(work, struct bt_gatt_ots_indicate, work);
    struct bt_ots *ots = CONTAINER_OF(ind, struct bt_ots, olcp_ind);

    bt_gatt_indicate(NULL, &ots->olcp_ind.params);
}

int bt_ots_init(struct bt_ots *ots,
                struct bt_ots_init_param *ots_init)
{
    int err;

    if (!ots || !ots_init || !ots_init->cb) {
        return -EINVAL;
    }

    __ASSERT(ots_init->cb->obj_created || !BT_OTS_OACP_GET_FEAT_CREATE(ots_init->features.oacp),
             "Callback for object creation is not set and object creation is enabled");
    __ASSERT(ots_init->cb->obj_deleted || !BT_OTS_OACP_GET_FEAT_DELETE(ots_init->features.oacp),
             "Callback for object deletion is not set and object deletion is enabled");
#if defined(CONFIG_BT_OTS_OACP_CHECKSUM_SUPPORT)
    __ASSERT(ots_init->cb->obj_cal_checksum ||
             !BT_OTS_OACP_GET_FEAT_CHECKSUM(ots_init->features.oacp),
             "Callback for object calculate checksum is not set and checksum is enabled");
#endif
    __ASSERT(ots_init->cb->obj_read || !BT_OTS_OACP_GET_FEAT_READ(ots_init->features.oacp),
             "Callback for object reading is not set and object read is enabled");
    __ASSERT(ots_init->cb->obj_write || !BT_OTS_OACP_GET_FEAT_WRITE(ots_init->features.oacp),
             "Callback for object write is not set and object write is enabled");

    /* Set callback structure. */
    ots->cb = ots_init->cb;

    /* Check OACP supported features against Kconfig. */
    if (ots_init->features.oacp & (~((uint32_t) OACP_FEAT))) {
        return -ENOTSUP;
    }

    __ASSERT(!BT_OTS_OACP_GET_FEAT_CREATE(ots_init->features.oacp) ||
             BT_OTS_OACP_GET_FEAT_WRITE(ots_init->features.oacp),
             "Object creation requires object write to be supported");

    ots->features.oacp = ots_init->features.oacp;
    LOG_DBG("OACP features: 0x%04X", ots->features.oacp);

    /* Check OLCP supported features against Kconfig. */
    if (ots_init->features.olcp & (~((uint32_t) OLCP_FEAT))) {
        return -ENOTSUP;
    }
    ots->features.olcp = ots_init->features.olcp;
    LOG_DBG("OLCP features: 0x%04X", ots->features.olcp);

    /* Register L2CAP context. */
    err = bt_gatt_ots_l2cap_register(&ots->l2cap);
    if (err) {
        return err;
    }

    err = bt_gatt_service_register(ots->service);
    if (err) {
        bt_gatt_ots_l2cap_unregister(&ots->l2cap);

        return err;
    }

    if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ)) {
        bt_ots_dir_list_init(&ots->dir_list, ots->obj_manager);
    }

    k_work_init(&ots->oacp_ind.work, oacp_indicate_work_handler);
    k_work_init(&ots->olcp_ind.work, olcp_indicate_work_handler);

    LOG_DBG("Initialized OTS");

    return 0;
}

#if defined(CONFIG_BT_OTS_SECONDARY_SVC)
#define BT_GATT_OTS_SERVICE BT_GATT_SECONDARY_SERVICE
#else
#define BT_GATT_OTS_SERVICE BT_GATT_PRIMARY_SERVICE
#endif

#if defined(CONFIG_BT_OTS_OBJ_NAME_WRITE_SUPPORT)
#define BT_OTS_OBJ_NAME_GATT_CHRC  (BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE)
#define BT_OTS_OBJ_NAME_GATT_PERM  (BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
#define BT_OTS_OBJ_NAME_GATT_WRITE (ots_obj_name_write)
#else
#define BT_OTS_OBJ_NAME_GATT_CHRC  (BT_GATT_CHRC_READ)
#define BT_OTS_OBJ_NAME_GATT_PERM  (BT_GATT_PERM_READ)
#define BT_OTS_OBJ_NAME_GATT_WRITE (NULL)
#endif

#define BT_GATT_OTS_ATTRS(_ots) {                   \
    BT_GATT_OTS_SERVICE(BT_UUID_OTS),               \
    BT_GATT_CHARACTERISTIC(BT_UUID_OTS_FEATURE,         \
        BT_GATT_CHRC_READ, BT_GATT_PERM_READ,           \
        ots_feature_read, NULL, &_ots),             \
    BT_GATT_CHARACTERISTIC(BT_UUID_OTS_NAME,            \
        BT_OTS_OBJ_NAME_GATT_CHRC, BT_OTS_OBJ_NAME_GATT_PERM,   \
        ots_obj_name_read, BT_OTS_OBJ_NAME_GATT_WRITE, &_ots),  \
    BT_GATT_CHARACTERISTIC(BT_UUID_OTS_TYPE,            \
        BT_GATT_CHRC_READ, BT_GATT_PERM_READ,           \
        ots_obj_type_read, NULL, &_ots),            \
    BT_GATT_CHARACTERISTIC(BT_UUID_OTS_SIZE,            \
        BT_GATT_CHRC_READ, BT_GATT_PERM_READ,           \
        ots_obj_size_read, NULL, &_ots),            \
    BT_GATT_CHARACTERISTIC(BT_UUID_OTS_ID,              \
        BT_GATT_CHRC_READ, BT_GATT_PERM_READ,           \
        ots_obj_id_read, NULL, &_ots),              \
    BT_GATT_CHARACTERISTIC(BT_UUID_OTS_PROPERTIES,          \
        BT_GATT_CHRC_READ, BT_GATT_PERM_READ,           \
        ots_obj_prop_read, NULL, &_ots),            \
    BT_GATT_CHARACTERISTIC(BT_UUID_OTS_ACTION_CP,           \
        BT_GATT_CHRC_WRITE | BT_GATT_CHRC_INDICATE,     \
        BT_GATT_PERM_WRITE, NULL,               \
        bt_gatt_ots_oacp_write, &_ots),             \
    BT_GATT_CCC_MANAGED(&_ots.oacp_ind.ccc,             \
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),        \
    BT_GATT_CHARACTERISTIC(BT_UUID_OTS_LIST_CP,         \
        BT_GATT_CHRC_WRITE | BT_GATT_CHRC_INDICATE,     \
        BT_GATT_PERM_WRITE, NULL,               \
        bt_gatt_ots_olcp_write, &_ots),             \
    BT_GATT_CCC_MANAGED(&_ots.olcp_ind.ccc,             \
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)         \
}

#define BT_GATT_OTS_INSTANCE_LIST_SIZE  (ARRAY_SIZE(ots_instances))
#define BT_GATT_OTS_INSTANCE_LIST_START ots_instances
#define BT_GATT_OTS_INSTANCE_LIST_END   \
    (&ots_instances[BT_GATT_OTS_INSTANCE_LIST_SIZE])

#define BT_GATT_OTS_SERVICE_LIST_START  ots_service_list

static struct bt_ots ots_instances[CONFIG_BT_OTS_MAX_INST_CNT];
static uint32_t instance_cnt;
BT_GATT_SERVICE_INSTANCE_DEFINE(ots_service_list, ots_instances,
                                CONFIG_BT_OTS_MAX_INST_CNT,
                                BT_GATT_OTS_ATTRS);

static void ots_delete_empty_name_objects(struct bt_ots *ots, struct bt_conn *conn)
{
    char id_str[BT_OTS_OBJ_ID_STR_LEN];
    struct bt_gatt_ots_object *obj;
    struct bt_gatt_ots_object *next_obj;
    int err;

    err = bt_gatt_ots_obj_manager_first_obj_get(ots->obj_manager, &next_obj);
    while (!err) {
        obj = next_obj;

        /* Get the next object before we potentially delete the current object and
         * no longer can get the next object
         */
        err = bt_gatt_ots_obj_manager_next_obj_get(ots->obj_manager, obj, &next_obj);

        if (strlen(obj->metadata.name) == 0) {
            bt_ots_obj_id_to_str(obj->id, id_str, sizeof(id_str));
            LOG_DBG("Deleting object with %s ID due to empty name", id_str);

            if (ots->cb && ots->cb->obj_deleted) {
                ots->cb->obj_deleted(ots, conn, obj->id);
            }

            if (bt_gatt_ots_obj_manager_obj_delete(obj)) {
                LOG_ERR("Failed to remove object with %s ID from object manager",
                        id_str);
            }
        }
    }
}

static void ots_conn_disconnected(struct bt_conn *conn, uint8_t reason)
{
    uint32_t index;
    struct bt_ots *instance;

    for (instance = BT_GATT_OTS_INSTANCE_LIST_START, index = 0;
            index < instance_cnt;
            instance++, index++) {

        LOG_DBG("Processing disconnect for OTS instance %u", index);

        if (instance->cur_obj != NULL) {
            __ASSERT(instance->cur_obj->state.type == BT_GATT_OTS_OBJECT_IDLE_STATE,
                     "The current object is expected to be in idle state as part "
                     "of cleanup of the L2CAP channel connection close.");
            instance->cur_obj = NULL;
        }

        ots_delete_empty_name_objects(instance, conn);
    }
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .disconnected = ots_conn_disconnected,
};

struct bt_ots *bt_ots_free_instance_get(void)
{
    if (instance_cnt >= BT_GATT_OTS_INSTANCE_LIST_SIZE) {
        return NULL;
    }

    return &BT_GATT_OTS_INSTANCE_LIST_START[instance_cnt++];
}

static int bt_gatt_ots_instances_prepare(void)
{
    uint32_t index;
    struct bt_ots *instance;

    for (instance = BT_GATT_OTS_INSTANCE_LIST_START, index = 0;
            instance != BT_GATT_OTS_INSTANCE_LIST_END;
            instance++, index++) {
        /* Assign an object pool to the OTS instance. */
        instance->obj_manager = bt_gatt_ots_obj_manager_assign();

        if (!instance->obj_manager) {
            LOG_ERR("OTS Object manager instance not available");
            return -ENOMEM;
        }

        /* Assign pointer to the service descriptor. */
        instance->service = &BT_GATT_OTS_SERVICE_LIST_START[index];

        /* Initialize CCC descriptors for characteristics with
         * indication properties.
         */
        instance->oacp_ind.ccc.cfg_changed =
            bt_gatt_ots_oacp_cfg_changed;
        instance->olcp_ind.ccc.cfg_changed =
            bt_gatt_ots_olcp_cfg_changed;
    }

    return 0;
}

SYS_INIT(bt_gatt_ots_instances_prepare, APPLICATION,
         CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

int bt_gatt_ots_conn_cb_register(void)
{
    return bt_conn_cb_register_safe((void *)&bt_conn_cb_conn_callbacks);
}
