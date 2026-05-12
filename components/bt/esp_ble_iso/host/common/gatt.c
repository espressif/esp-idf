/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/autoconf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/common/bt_str.h>

#include <../host/conn_internal.h>

#include "common/host.h"

LOG_MODULE_REGISTER(ISO_GATT, CONFIG_BT_ISO_LOG_LEVEL);

static sys_slist_t gatt_db = SYS_SLIST_STATIC_INIT(&gatt_db);

static struct gattc_sub subscriptions[CONFIG_BT_MAX_CONN];

_LIB_ONLY
int bt_gatt_service_register(struct bt_gatt_service *svc)
{
    const struct bt_uuid_16 *uuid;

    LOG_DBG("GattServiceReg");

    if (svc == NULL) {
        LOG_ERR("GattSvcNull");
        return -EINVAL;
    }

    uuid = svc->attrs[0].user_data;
    assert(uuid);

    LOG_DBG("GattSvc[%04x]", uuid->val);

    if (sys_slist_find(&gatt_db, &svc->node, NULL)) {
        LOG_WRN("GattSvcExist");
        return -EALREADY;
    }

    sys_slist_append(&gatt_db, &svc->node);

    return 0;
}

_IDF_ONLY
int bt_gatt_service_register_safe(struct bt_gatt_service *svc)
{
    int err;
    bt_le_host_lock();
    err = bt_gatt_service_register(svc);
    bt_le_host_unlock();
    return err;
}

_LIB_ONLY
int bt_gatt_service_unregister(struct bt_gatt_service *svc)
{
    LOG_DBG("GattServiceUnreg");

    if (svc == NULL) {
        LOG_ERR("GattSvcNull");
        return -EINVAL;
    }

    if (!sys_slist_find_and_remove(&gatt_db, &svc->node)) {
        LOG_ERR("GattSvcNotFound");
        return -ENOENT;
    }

    return 0;
}

_IDF_ONLY
int bt_gatt_service_unregister_safe(struct bt_gatt_service *svc)
{
    int err;
    bt_le_host_lock();
    err = bt_gatt_service_unregister(svc);
    bt_le_host_unlock();
    return err;
}

_LIB_ONLY
uint16_t bt_gatt_get_mtu(struct bt_conn *conn)
{
    uint16_t mtu;

    assert(conn);

    mtu = bt_le_nimble_gatt_get_mtu(conn);

    LOG_DBG("GattGetMtu[%u]", mtu);

    return mtu;
}

_LIB_ONLY
size_t bt_eatt_count(struct bt_conn *conn)
{
    return 0;
}

_IDF_ONLY
struct bt_gatt_attr *bt_gatts_find_attr_by_handle(uint16_t handle)
{
    struct bt_gatt_service *svc = NULL;
    struct bt_gatt_attr *attr = NULL;

    LOG_DBG("GattsFindAttrByHdl[%u]", handle);

    SYS_SLIST_FOR_EACH_CONTAINER(&gatt_db, svc, node) {
        for (size_t i = 0; i < svc->attr_count; i++) {
            if (svc->attrs[i].handle == handle) {
                attr = &svc->attrs[i];
                break;
            }
        }
        if (attr != NULL) {
            break;
        }
    }

    return attr;
}

_IDF_ONLY
uint16_t bt_gatt_attr_get_handle(const struct bt_gatt_attr *attr)
{
    uint16_t handle = 0;

    LOG_DBG("GattAttrGetHdl");

    if (attr && attr->handle) {
        handle = attr->handle;
    }

    LOG_DBG("Hdl[%u]", handle);
    return handle;
}

_LIB_IDF
uint16_t bt_gatt_attr_value_handle(const struct bt_gatt_attr *attr)
{
    uint16_t handle = 0;

    /* Note:
     * This function is used to get the handle of the
     * characteristic value descriptor.
     * Currently this function is only used by TMAP.
     */

    LOG_DBG("GattAttrValHdl");

    if (attr) {
        if (attr->uuid == NULL) {
            handle = (attr->handle + 1);
        } else {
            LOG_DBG("Uuid[%s]", bt_uuid_str(attr->uuid));

            if (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CHRC) == 0) {
                struct bt_gatt_chrc *chrc = attr->user_data;

                assert(chrc);
                handle = chrc->value_handle;

                LOG_DBG("ValHdl[%u]", handle);

                if (handle == 0) {
                    /* Fall back to Zephyr value handle policy */
                    handle = bt_gatt_attr_get_handle(attr) + 1;
                    LOG_INF("ValHdlNew[%u]", handle);
                }
            }
        }
    }

    LOG_DBG("Hdl[%u]", handle);
    return handle;
}

static uint8_t find_next(const struct bt_gatt_attr *attr,
                         uint16_t handle, void *user_data)
{
    struct bt_gatt_attr **next = user_data;

    LOG_DBG("FindNext");

    *next = (struct bt_gatt_attr *)attr;

    return BT_GATT_ITER_STOP;
}

_IDF_ONLY
struct bt_gatt_attr *bt_gatt_attr_next(const struct bt_gatt_attr *attr)
{
    struct bt_gatt_attr *next = NULL;
    uint16_t handle;

    LOG_DBG("GattAttrNext");

    assert(attr);
    handle = bt_gatt_attr_get_handle(attr);

    bt_gatt_foreach_attr(handle + 1, handle + 1, find_next, &next);

    return next;
}

static uint8_t gatt_foreach_iter(const struct bt_gatt_attr *attr,
                                 uint16_t handle,
                                 uint16_t start_handle,
                                 uint16_t end_handle,
                                 const struct bt_uuid *uuid,
                                 const void *attr_data,
                                 uint16_t *num_matches,
                                 bt_gatt_attr_func_t func,
                                 void *user_data)
{
    uint8_t result;

    /* LOG_DBG("GattForeachIter[%u][%u][%u]", handle, start_handle, end_handle); */

    /* Continue if over the requested range (stop is not suitable for our Hosts) */
    if (handle > end_handle) {
        return BT_GATT_ITER_CONTINUE;
    }

    /* Check if attribute handle is within range */
    if (handle < start_handle) {
        return BT_GATT_ITER_CONTINUE;
    }

    /* Match attribute UUID if set */
    if (uuid && bt_uuid_cmp(uuid, attr->uuid)) {
        /* LOG_DBG("MismatchUuid[%s][%s]", bt_uuid_str(uuid), bt_uuid_str(attr->uuid)); */
        return BT_GATT_ITER_CONTINUE;
    }

    /* Match attribute user_data if set */
    if (attr_data && attr_data != attr->user_data) {
        LOG_DBG("MismatchAttrData");
        return BT_GATT_ITER_CONTINUE;
    }

    *num_matches -= 1;

    result = func(attr, handle, user_data);

    if (*num_matches == 0) {
        LOG_DBG("NumMatchesZero");
        return BT_GATT_ITER_STOP;
    }

    LOG_DBG("GattIter%s", result == BT_GATT_ITER_CONTINUE ? "Cont" : "Stop");
    return result;
}

static void foreach_attr_type_dyndb(uint16_t start_handle,
                                    uint16_t end_handle,
                                    const struct bt_uuid *uuid,
                                    const void *attr_data,
                                    uint16_t num_matches,
                                    bt_gatt_attr_func_t func,
                                    void *user_data)
{
    struct bt_gatt_service *svc;

    SYS_SLIST_FOR_EACH_CONTAINER(&gatt_db, svc, node) {
#if 0
        /* Upstream Zephyr's "skip current service if next->attrs[0].handle
         * <= start_handle" optimization assumes gatt_db is sorted by
         * handle ascending. bt_gatt_service_register() here uses
         * sys_slist_append() (registration order, not handle order), and
         * NimBLE assigns handles internally, so that invariant doesn't
         * hold -- this optimization would skip services that should be
         * searched. Re-enable only after switching service registration
         * to insert by handle.
         */
        struct bt_gatt_service *next;

        next = SYS_SLIST_PEEK_NEXT_CONTAINER(svc, node);
        if (next) {
            /* Skip ahead if start is not within service handles */
            if (next->attrs[0].handle <= start_handle) {
                continue;
            }
        }
#endif

        for (size_t i = 0; i < svc->attr_count; i++) {
            struct bt_gatt_attr *attr = &svc->attrs[i];

            assert(attr->uuid);

            if (gatt_foreach_iter(attr, attr->handle,
                                  start_handle,
                                  end_handle,
                                  uuid, attr_data,
                                  &num_matches,
                                  func, user_data) == BT_GATT_ITER_STOP) {
                return;
            }
        }
    }
}

_LIB_IDF
void bt_gatt_foreach_attr_type(uint16_t start_handle,
                               uint16_t end_handle,
                               const struct bt_uuid *uuid,
                               const void *attr_data,
                               uint16_t num_matches,
                               bt_gatt_attr_func_t func,
                               void *user_data)
{
    LOG_DBG("GattForeachAttrType[%u][%u][%u]", start_handle, end_handle, num_matches);

    if (num_matches == 0) {
        num_matches = UINT16_MAX;
    }

    /* Iterate over dynamic db */
    foreach_attr_type_dyndb(start_handle, end_handle, uuid, attr_data,
                            num_matches, func, user_data);
}

static uint8_t match_uuid(const struct bt_gatt_attr *attr, uint16_t handle, void *user_data)
{
    struct notify_data *data = user_data;

    LOG_DBG("MatchUuid[%u]", handle);

    data->attr = attr;
    data->handle = handle;

    return BT_GATT_ITER_STOP;
}

_IDF_ONLY
bool bt_gatts_find_attr_by_uuid(struct notify_data *found, const struct bt_uuid *uuid)
{
    LOG_DBG("GattsFindAttrByUuid[%u][%s]", found->handle, bt_uuid_str(uuid));

    found->attr = NULL;

    bt_gatt_foreach_attr_type(found->handle, 0xffff, uuid, NULL, 1, match_uuid, found);

    if (found->attr) {
        LOG_DBG("AttrFoundByUuid");
    } else {
        LOG_WRN("AttrNotFoundByUuid");
    }

    return (found->attr ? true : false);
}

_LIB_ONLY
struct bt_gatt_attr *bt_gatt_find_by_uuid(const struct bt_gatt_attr *attr,
                                          uint16_t attr_count,
                                          const struct bt_uuid *uuid)
{
    struct bt_gatt_attr *found = NULL;
    uint16_t start_handle;
    uint16_t end_handle;

    start_handle = bt_gatt_attr_get_handle(attr);
    end_handle = (start_handle && attr_count) ? (start_handle + attr_count) : 0xffff;

    LOG_DBG("GattFindByUuid[%u][%u]", start_handle, end_handle);

    bt_gatt_foreach_attr_type(start_handle, end_handle, uuid,
                              NULL, 1, find_next, &found);

    return found;
}

_LIB_IDF
ssize_t bt_gatt_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t buf_len, uint16_t offset,
                          const void *value, uint16_t value_len)
{
    assert(buf);

    LOG_DBG("GattAttrRd[%u][%u][%u]", buf_len, offset, value_len);

    if (offset > value_len) {
        LOG_ERR("TooLargeOffset[%u][%u]", offset, value_len);
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    return bt_le_nimble_gatts_attr_read(conn, attr, buf, buf_len,
                                        offset, value, value_len);
}

_LIB_ONLY
int bt_gatt_notify_cb(struct bt_conn *conn, struct bt_gatt_notify_params *params)
{
    int err;

    assert(params);

    LOG_DBG("GattNtfCb[%u]", params->len);

    if (conn && conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    err = bt_le_nimble_gatts_notify(conn, params);

    /* gatts_notify is synchronous (mbuf-copy + dispatch on return); fire the
     * caller's completion cb here so state machines like PACS_FLAG_NOTIFY_RDY
     * advance. */
    if (err == 0 && params->func != NULL) {
        params->func(conn, params->user_data);
    }

    return err;
}

_LIB_IDF
int bt_gatt_indicate(struct bt_conn *conn, struct bt_gatt_indicate_params *params)
{
    assert(params);
    assert(params->attr && params->attr->uuid);

    LOG_DBG("GattInd[%s]", bt_uuid_str(params->attr->uuid));

    if (conn && conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    if ((!params->data ^ !params->len)) {
        LOG_ERR("InvData[%p][%u]", params->data, params->len);
        return -EINVAL;
    }

    if (params->attr->uuid->type != BT_UUID_TYPE_16) {
        LOG_ERR("InvUuidType[%u]", params->attr->uuid->type);
        return -ENOTSUP;
    }

    return bt_le_nimble_gatts_indicate(conn, params);
}

_LIB_IDF
ssize_t bt_gatt_attr_read_service(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr,
                                  void *buf, uint16_t len,
                                  uint16_t offset)
{
    struct bt_uuid *uuid = attr->user_data;

    assert(uuid);

    LOG_DBG("GattAttrRdSvc[%s]", bt_uuid_str(uuid));

    if (uuid->type == BT_UUID_TYPE_16) {
        uint16_t uuid16 = sys_cpu_to_le16(BT_UUID_16(uuid)->val);

        return bt_gatt_attr_read(conn, attr, buf, len, offset, &uuid16, 2);
    }

    /* BT_UUID_TYPE_32 is not expected for service UUIDs in LE Audio */
    assert(uuid->type == BT_UUID_TYPE_128);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, BT_UUID_128(uuid)->val, 16);
}

static uint8_t gatts_get_svc_handles(const struct bt_gatt_attr *attr, uint16_t handle, void *user_data)
{
    struct gatt_incl *include = user_data;

    LOG_DBG("GattsGetSvcHdl[%s][%u]", bt_uuid_str(attr->uuid), handle);

    /* Stop if attribute is a service */
    if (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_PRIMARY) == 0 ||
            bt_uuid_cmp(attr->uuid, BT_UUID_GATT_SECONDARY) == 0) {
        LOG_DBG("SvcAttr");
        return BT_GATT_ITER_STOP;
    }

    include->end_handle = sys_cpu_to_le16(handle);

    return BT_GATT_ITER_CONTINUE;
}

_LIB_ONLY
ssize_t bt_gatt_attr_read_included(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, uint16_t len,
                                   uint16_t offset)
{
    struct bt_gatt_attr *incl = NULL;
    struct bt_uuid *uuid = NULL;
    struct gatt_incl pdu = {0};
    uint8_t value_len;
    uint16_t handle;

    incl = attr->user_data;
    assert(incl);
    assert(incl->user_data);

    handle = bt_gatt_attr_get_handle(incl);
    uuid = incl->user_data;

    LOG_DBG("GattAttrRdInc[%u][%u][%u][%s]",
            len, offset, handle, bt_uuid_str(uuid));

    /* first attr points to the start handle */
    pdu.start_handle = sys_cpu_to_le16(handle);
    value_len = sizeof(pdu.start_handle) + sizeof(pdu.end_handle);

    /* The Service UUID shall only be present when the UUID is a
     * 16-bit Bluetooth UUID.
     */
    if (uuid->type == BT_UUID_TYPE_16) {
        pdu.uuid16 = sys_cpu_to_le16(BT_UUID_16(uuid)->val);
        value_len += sizeof(pdu.uuid16);
    }

    /* Lookup for service end handle */
    bt_gatt_foreach_attr(handle + 1, 0xffff, gatts_get_svc_handles, &pdu);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

_LIB_IDF
ssize_t bt_gatt_attr_read_chrc(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len,
                               uint16_t offset)
{
    struct bt_gatt_chrc *chrc = attr->user_data;
    struct gatt_chrc pdu;
    uint8_t value_len;

    assert(chrc);
    assert(chrc->uuid);

    pdu.properties = chrc->properties;
    pdu.value_handle = sys_cpu_to_le16(bt_gatt_attr_value_handle(attr));

    LOG_DBG("GattAttrRdChrc[%u][%u][%02x][%u][%s]",
            len, offset, pdu.properties, pdu.value_handle, bt_uuid_str(chrc->uuid));

    value_len = sizeof(pdu.properties) + sizeof(pdu.value_handle);

    if (chrc->uuid->type == BT_UUID_TYPE_16) {
        pdu.uuid16 = sys_cpu_to_le16(BT_UUID_16(chrc->uuid)->val);
        value_len += 2U;
    } else if (chrc->uuid->type == BT_UUID_TYPE_128) {
        memcpy(pdu.uuid, BT_UUID_128(chrc->uuid)->val, 16);
        value_len += 16U;
    } else {
        LOG_ERR("UnsupportedUuidType[%u]", chrc->uuid->type);
        return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
    }

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &pdu, value_len);
}

static struct bt_gatt_ccc_cfg *gatts_find_ccc_cfg(const struct bt_conn *conn,
                                                  struct bt_gatt_ccc_managed_user_data *ccc)
{
    /* LOG_DBG("GattsFindCccCfg"); */

    for (size_t i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
        struct bt_gatt_ccc_cfg *cfg = &ccc->cfg[i];

        if (conn) {
            if (bt_conn_is_peer_addr_le(conn, cfg->id, &cfg->peer)) {
                /* LOG_DBG("CccCfgFound[%u]", i); */
                return cfg;
            }
        } else if (bt_addr_le_eq(&cfg->peer, BT_ADDR_LE_ANY)) {
            LOG_DBG("CccCfgNew[%u]", i);
            return cfg;
        }
    }

    if (conn) {
        /* LOG_DBG("CccCfgNotFound"); */
    } else {
        LOG_ERR("CccNoFreeCfg");
    }

    return NULL;
}

static void gatts_ccc_changed(const struct bt_gatt_attr *attr,
                              struct bt_gatt_ccc_managed_user_data *ccc)
{
    uint16_t value = 0x0000;

    for (size_t i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
        if (ccc->cfg[i].value > value) {
            value = ccc->cfg[i].value;
        }
    }

    /* LOG_DBG("GattsCccChanged[%04x][%04x]", ccc->value, value); */

    if (ccc->value != value) {
        ccc->value = value;
        if (ccc->cfg_changed) {
            ccc->cfg_changed(attr, value);
        }
    }
}

static void gatts_clear_ccc_cfg(struct bt_gatt_ccc_cfg *cfg)
{
    /* LOG_DBG("GattsClearCccCfg"); */

    bt_addr_le_copy(&cfg->peer, BT_ADDR_LE_ANY);
    cfg->id = 0U;
    cfg->value = 0U;
}

_LIB_IDF
ssize_t bt_gatt_attr_read_ccc(struct bt_conn *conn,
                              const struct bt_gatt_attr *attr,
                              void *buf, uint16_t len,
                              uint16_t offset)
{
    struct bt_gatt_ccc_managed_user_data *ccc;
    const struct bt_gatt_ccc_cfg *cfg;
    uint16_t value;

    assert(attr);

    ccc = attr->user_data;
    assert(ccc);

    LOG_DBG("GattAttrRdCcc[%u][%u]", len, offset);

    cfg = gatts_find_ccc_cfg(conn, ccc);
    if (cfg) {
        value = sys_cpu_to_le16(cfg->value);
    } else {
        /* Default to disable if there is no cfg for the peer */
        value = 0x0000;
    }

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &value, sizeof(value));
}

_LIB_IDF
ssize_t bt_gatt_attr_write_ccc(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len,
                               uint16_t offset, uint8_t flags)
{
    struct bt_gatt_ccc_managed_user_data *ccc;
    struct bt_gatt_ccc_cfg *cfg;
    bool value_changed;
    bool new_entry;
    uint16_t value;

    assert(attr);

    ccc = attr->user_data;
    assert(ccc);

    LOG_DBG("GattAttrWrChrc[%u][%u][%02x]", len, offset, flags);

    if (offset) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    if (len == 0 || len > sizeof(uint16_t)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    if (len < sizeof(uint16_t)) {
        value = *(uint8_t *)buf;
    } else {
        value = sys_get_le16(buf);
    }

    new_entry = false;

    cfg = gatts_find_ccc_cfg(conn, ccc);
    if (cfg == NULL) {
        /* If there's no existing entry, but the new value is zero,
         * we don't need to do anything, since a disabled CCC is
         * behaviorally the same as no written CCC.
         */
        if (value == 0) {
            LOG_INF("CccValZero");
            return len;
        }

        cfg = gatts_find_ccc_cfg(NULL, ccc);
        if (cfg == NULL) {
            return BT_GATT_ERR(BT_ATT_ERR_INSUFFICIENT_RESOURCES);
        }

        bt_addr_le_copy(&cfg->peer, &conn->le.dst);
        cfg->id = conn->id;

        new_entry = true;
    }

    /* Confirm write if cfg is managed by application */
    if (ccc->cfg_write) {
        ssize_t write = ccc->cfg_write(conn, attr, value);

        LOG_INF("CccCfgWr[%d]", write);

        if (write < 0) {
            if (new_entry) {
                gatts_clear_ccc_cfg(cfg);
            }
            return write;
        }

        /* Accept size=1 for backwards compatibility */
        if (write != sizeof(value) && write != 1) {
            if (new_entry) {
                gatts_clear_ccc_cfg(cfg);
            }
            return BT_GATT_ERR(BT_ATT_ERR_UNLIKELY);
        }
    }

    LOG_DBG("CccCfg[%u][%04x][%04x]", attr->handle, cfg->value, value);

    value_changed = (cfg->value != value);
    cfg->value = value;

    /* Update cfg if don't match */
    if (cfg->value != ccc->value) {
        gatts_ccc_changed(attr, ccc);
    }

    if (value_changed) {
        /* TBD: ccc store? */
    }

    /* Disabled CCC is the same as no configured CCC, so clear the entry */
    if (value == 0) {
        gatts_clear_ccc_cfg(cfg);
    }

    return len;
}

_LIB_ONLY
bool bt_gatt_is_subscribed(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr,
                           uint16_t ccc_type)
{
    const struct bt_gatt_ccc_managed_user_data *ccc;

    assert(conn);
    assert(attr && attr->uuid);

    LOG_DBG("GattIsSub[%04x][%s]", ccc_type, bt_uuid_str(attr->uuid));

    if (conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return false;
    }

    /* Check if attribute is a characteristic declaration */
    if (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CHRC) == 0) {
        uint8_t properties;
        ssize_t len;

        if (attr->read == NULL) {
            LOG_WRN("AttrRdCbNull");
            return false;
        }

        /* The characteristic properties is the first byte of the attribute value */
        len = attr->read(NULL, attr, &properties, 1, 0);
        if (len < 0) {
            LOG_ERR("RdAttrFail[%zd]", len);
            return false;
        }

        if (len != 1) {
            LOG_ERR("InvRdLen[%zd]", len);
            return false;
        }

        if (!(properties & (BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_INDICATE))) {
            /* Characteristic doesn't support subscription */
            LOG_WRN("SubNotSupp[%02x]", properties);
            return false;
        }

        attr = bt_gatt_attr_next(attr);
        assert(attr && attr->uuid);

        LOG_DBG("Attr[%s][%d]", bt_uuid_str(attr->uuid), __LINE__);
    }

    /* Check if attribute is a characteristic value */
    if (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC)) {
        attr = bt_gatt_attr_next(attr);
        if (attr == NULL) {
            return false;
        }

        assert(attr->uuid);
        LOG_DBG("Attr[%s][%d]", bt_uuid_str(attr->uuid), __LINE__);
    }

    /* Find the CCC Descriptor */
    while (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC) &&
            /* Also stop if we leave the current characteristic definition */
            bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CHRC) &&
            bt_uuid_cmp(attr->uuid, BT_UUID_GATT_PRIMARY) &&
            bt_uuid_cmp(attr->uuid, BT_UUID_GATT_SECONDARY)) {
        attr = bt_gatt_attr_next(attr);
        if (attr == NULL) {
            return false;
        }
    }

    assert(attr->uuid);

    LOG_DBG("Attr[%s][%d]", bt_uuid_str(attr->uuid), __LINE__);

    if (bt_uuid_cmp(attr->uuid, BT_UUID_GATT_CCC)) {
        return false;
    }

    ccc = attr->user_data;
    assert(ccc);

    /* Check if the connection is subscribed */
    for (size_t i = 0; i < ARRAY_SIZE(ccc->cfg); i++) {
        const struct bt_gatt_ccc_cfg *cfg = &ccc->cfg[i];

        LOG_DBG("CccCfg[%u][%04x]", i, ccc->cfg[i].value);

        if (bt_conn_is_peer_addr_le(conn, cfg->id, &cfg->peer) &&
                (ccc_type & ccc->cfg[i].value)) {
            return true;
        }
    }

    return false;
}

static int gatts_ccc_cfg_update(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                uint16_t value)
{
    struct bt_gatt_ccc_managed_user_data *ccc;
    struct bt_gatt_ccc_cfg *cfg;
    bool value_changed;

    ccc = attr->user_data;
    assert(ccc);

    /* LOG_DBG("GattsCccCfgUpd[%04x]", value); */

    cfg = gatts_find_ccc_cfg(conn, ccc);
    if (cfg == NULL) {
        /* If there's no existing entry, and the new notification value
         * is zero, we don't need to do anything, since a disabled CCC
         * is behaviorally the same as no written CCC.
         */
        if (value == 0) {
            LOG_INF("CccValZero");
            return 0;
        }

        cfg = gatts_find_ccc_cfg(NULL, ccc);
        if (cfg == NULL) {
            return -ENOMEM;
        }

        bt_addr_le_copy(&cfg->peer, &conn->le.dst);
        cfg->id = conn->id;
    }

    /* LOG_DBG("CccCfg[%u][%04x][%04x]", attr->handle, cfg->value, value); */

    value_changed = (cfg->value != value);
    cfg->value = value;

    if (value_changed) {
        /* TBD: ccc store? */
    }

    if (cfg->value != ccc->value) {
        gatts_ccc_changed(attr, ccc);
    }

    /* Disabled CCC is the same as no configured CCC, so clear the entry */
    if (value == 0) {
        gatts_clear_ccc_cfg(cfg);
    }

    return 0;
}

static struct bt_gatt_attr *gatts_ccc_attr_find(uint16_t ccc_handle)
{
    struct bt_gatt_service *svc = NULL;
    struct bt_gatt_attr *attr = NULL;

    /* LOG_DBG("GattsCccAttrFind[%u]", ccc_handle); */

    SYS_SLIST_FOR_EACH_CONTAINER(&gatt_db, svc, node) {
        for (size_t i = 0; i < svc->attr_count; i++) {
            if (svc->attrs[i].uuid->type == BT_UUID_TYPE_16 &&
                    BT_UUID_16(svc->attrs[i].uuid)->val == BT_UUID_GATT_CCC_VAL &&
                    svc->attrs[i].handle == ccc_handle) {
                attr = &svc->attrs[i];
                /* LOG_DBG("CccAttrFound[%u][%u]", svc->attr_count, i); */
                break;
            }
        }
    }

    return attr;
}

_IDF_ONLY
int bt_gatts_sub_changed(uint16_t conn_handle,
                         uint16_t ccc_handle,
                         uint8_t cur_notify,
                         uint8_t cur_indicate,
                         uint8_t reason)
{
    const struct bt_gatt_attr *attr;
    struct bt_conn *conn;

    /* LOG_DBG("GattsSubChanged[%u][%u][%u][%u][%02x]", */
    /*         conn_handle, ccc_handle, cur_notify, cur_indicate, reason); */

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    attr = gatts_ccc_attr_find(ccc_handle);
    if (attr == NULL) {
        /* May not be characteristic value declaration of LE Audio */
        return 0;
    }

    /* TODO:
     * Check the indication support for some characteristics, e.g.,
     * HAS Hearing Aid Preset Control Point characteristic.
     */
    return gatts_ccc_cfg_update(conn, attr, (cur_indicate << 1) | cur_notify);
}

_IDF_ONLY
int bt_gattc_disc_start_safe(uint16_t conn_handle)
{
    int err;
    LOG_DBG("GattcDiscStart[%u]", conn_handle);
    bt_le_host_lock();
    err = bt_le_nimble_gattc_disc_start(conn_handle);
    bt_le_host_unlock();
    return err;
}

_LIB_ONLY
int bt_gatt_discover(struct bt_conn *conn, struct bt_gatt_discover_params *params)
{
    assert(conn);
    assert(params);
    assert(params->start_handle && params->end_handle);
    assert(params->start_handle <= params->end_handle);

    LOG_DBG("GattDisc[%u][%u]", params->start_handle, params->end_handle);

    if (conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    if (params->uuid && params->uuid->type != BT_UUID_TYPE_16) {
        LOG_ERR("InvUuid[%s]", bt_uuid_str(params->uuid));
        return -ENOTSUP;
    }

    return bt_le_nimble_gattc_discover(conn, params);
}

static struct gattc_sub *gattc_sub_find(struct bt_conn *conn)
{
    /* LOG_DBG("GattcSubFind[%p]", conn); */

    for (size_t i = 0; i < ARRAY_SIZE(subscriptions); i++) {
        struct gattc_sub *sub = &subscriptions[i];

        if (conn == NULL) {
            if (bt_addr_le_eq(&sub->peer, BT_ADDR_LE_ANY)) {
                LOG_DBG("SubNew[%u]", i);
                return sub;
            }
        } else if (bt_conn_is_peer_addr_le(conn, sub->id, &sub->peer)) {
            /* LOG_DBG("SubFound[%u]", i); */
            return sub;
        }
    }

    if (conn) {
        /* LOG_DBG("SubNotFound"); */
    } else {
        LOG_WRN("NoFreeSub");
    }

    return NULL;
}

static struct gattc_sub *gattc_sub_add(struct bt_conn *conn)
{
    struct gattc_sub *sub;

    /* LOG_DBG("GattsSubAdd"); */

    sub = gattc_sub_find(conn);
    if (sub == NULL) {
        sub = gattc_sub_find(NULL);
        if (sub) {
            bt_addr_le_copy(&sub->peer, &conn->le.dst);
            sub->id = conn->id;
            LOG_DBG("SubPeer[%s]", bt_addr_le_str(&sub->peer));
        }
    }

    return sub;
}

static void gattc_sub_clear(struct bt_conn *conn)
{
    struct bt_gatt_subscribe_params *tmp = NULL;
    struct gattc_sub *sub = NULL;

    sub = gattc_sub_find(conn);
    if (sub == NULL) {
        return;
    }

    LOG_DBG("GattcSubClear[%s]", bt_addr_le_str(&sub->peer));

    /* Reset each entry's handles and NOTIFY/INDICATE value so any state read
     * from the params struct after disconnect reflects "not subscribed / no
     * valid handles", then drop the nodes from the list. Dropping the nodes
     * is what actually unblocks resubscription: bt_gatt_subscribe() rejects
     * a params pointer that's still linked (tmp == params → -EALREADY), so
     * without the list reset the CCCD write would never be reissued to the
     * peer.
     */
    SYS_SLIST_FOR_EACH_CONTAINER(&sub->list, tmp, node) {
        tmp->value_handle = 0;
        tmp->ccc_handle = 0;
        tmp->end_handle = 0;
        tmp->value = 0;
    }
    sys_slist_init(&sub->list);
}

_IDF_ONLY
struct gattc_sub *bt_gattc_sub_find(struct bt_conn *conn)
{
    return gattc_sub_find(conn);
}

static uint8_t gattc_ccc_discover_cb(struct bt_conn *conn,
                                     const struct bt_gatt_attr *attr,
                                     struct bt_gatt_discover_params *params)
{
    struct bt_gatt_subscribe_params *sub_params = params->sub_params;

    LOG_DBG("GattcCccDiscCb[%u]", params->type);

    if (attr == NULL) {
        memset(params, 0, sizeof(*params));

        /* Notify with NULL data to complete unsubscribe */
        sub_params->notify(conn, sub_params, NULL, 0);

        return BT_GATT_ITER_STOP;
    }

    if (params->type == BT_GATT_DISCOVER_DESCRIPTOR) {
        memset(params, 0, sizeof(*params));

        LOG_DBG("CccHdl[%u]", attr->handle);

        sub_params->ccc_handle = attr->handle;

        return BT_GATT_ITER_STOP;
    }

    return BT_GATT_ITER_CONTINUE;
}

static int gattc_ccc_discover(struct bt_conn *conn, struct bt_gatt_subscribe_params *params)
{
    static struct bt_uuid_16 ccc_uuid = {
        .uuid.type = BT_UUID_TYPE_16,
        .val = BT_UUID_GATT_CCC_VAL,
    };

    assert(params->disc_params);

    LOG_DBG("GattcCccDisc[%u][%u]", params->value_handle, params->end_handle);

    memset(params->disc_params, 0, sizeof(*params->disc_params));

    params->disc_params->sub_params = params;
    params->disc_params->uuid = &ccc_uuid.uuid;
    params->disc_params->type = BT_GATT_DISCOVER_DESCRIPTOR;
    params->disc_params->start_handle = params->value_handle;
    params->disc_params->end_handle = params->end_handle;
    params->disc_params->func = gattc_ccc_discover_cb;

    return bt_gatt_discover(conn, params->disc_params);
}

static int gattc_write_ccc(struct bt_conn *conn, struct bt_gatt_subscribe_params *params)
{
    LOG_DBG("GattcWrCcc[%u][%04x]", params->ccc_handle, params->value);

    return bt_le_nimble_gattc_write_ccc(conn, params);
}

_LIB_ONLY
int bt_gatt_subscribe(struct bt_conn *conn, struct bt_gatt_subscribe_params *params)
{
    struct bt_gatt_subscribe_params *tmp;
    bool has_subscription = false;
    struct gattc_sub *sub;

    assert(conn);
    assert(params);
    assert(params->notify);
    assert(params->value);
    assert(params->ccc_handle == BT_GATT_AUTO_DISCOVER_CCC_HANDLE ||
           (params->end_handle && params->disc_params));

    LOG_DBG("GattcSub[%u][%u][%u][%04x]",
            params->value_handle, params->ccc_handle, params->end_handle, params->value);

    /* Failures that return before sys_slist_append() reset
     * params->value_handle, mirroring gattc_sub_clear() on
     * disconnect, so callers gating retries on `value_handle == 0`
     * can resubscribe.
     */
    if (conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        params->value_handle = 0; /* unlinked: clear retry guard */
        return -ENOTCONN;
    }

    sub = gattc_sub_add(conn);
    if (sub == NULL) {
        params->value_handle = 0; /* unlinked: clear retry guard */
        return -ENOMEM;
    }

    /* Lookup existing subscriptions */
    SYS_SLIST_FOR_EACH_CONTAINER(&sub->list, tmp, node) {
        /* Fail if entry already exists */
        if (tmp == params) {
            LOG_WRN("GattcSubExist[%u][%u][%04x]",
                    params->value_handle, params->ccc_handle, params->value);
            /* live on sub->list: keep value_handle intact */
            return -EALREADY;
        }

        /* Check if another subscription exists, i.e. multiple
         * subscribe to the same characteristic value handle.
         */
        if (tmp->value_handle == params->value_handle &&
                tmp->value >= params->value) {
            LOG_INF("AnotherSubExist[%04x]", tmp->value);
            has_subscription = true;
        }
    }

    LOG_INF("GattcHasSub[%u][%u][%u]",
            params->value_handle, params->ccc_handle, has_subscription);

    /* Skip write if already subscribed */
    if (has_subscription == false) {
        if (params->ccc_handle == BT_GATT_AUTO_DISCOVER_CCC_HANDLE) {
            int err = gattc_ccc_discover(conn, params);
            if (err) {
                LOG_ERR("DiscCccFail[%d]", err);
                params->value_handle = 0; /* unlinked: clear retry guard */
                return err;
            }
        } else {
            /* Note:
             * If the CCC handle is auto-discovered, and during the
             * auto-discovery procedure, the indication/notification
             * has already been enabled, no need to write CCC again.
             */
        }
    }

    /* Add subscription before write complete as some implementation
     * were reported to send notification before reply to CCC write.
     */
    sys_slist_append(&sub->list, &params->node);

    /* Add this here to complete the discovery procedure directly.
     * For Zephyr, this subscribe callback will be invoked after
     * the response of updating CCCD (Write Request) is received.
     */
    if (params->subscribe) {
        params->subscribe(conn, 0, params);
    }

    return 0;
}

_LIB_ONLY
int bt_gatt_unsubscribe(struct bt_conn *conn, struct bt_gatt_subscribe_params *params)
{
    struct bt_gatt_subscribe_params *tmp;
    bool has_subscription = false;
    struct gattc_sub *sub;
    bool found = false;
    int err = 0;

    assert(conn);
    assert(params);

    LOG_DBG("GattcUnsub[%04x][%u][%u][%u]",
            params->value, params->value_handle, params->ccc_handle, params->end_handle);

    if (conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    sub = gattc_sub_find(conn);
    if (sub == NULL) {
        return -ENODEV;
    }

    /* Lookup existing subscriptions */
    SYS_SLIST_FOR_EACH_CONTAINER(&sub->list, tmp, node) {
        if (params == tmp) {
            found = true;
            continue;
        }

        /* Check if there still remains any other subscription.
         * I.e. subscribed with different sub_params but the same
         * characteristic value handle.
         */
        if (tmp->value_handle == params->value_handle) {
            LOG_INF("AnotherSubFound");
            has_subscription = true;
        }
    }

    if (!found) {
        /* Note:
         * For unsubscribe, the -EINVAL will not be treated as error.
         */
        /* LOG_DBG("GattcSubNotFound"); */
        return -EINVAL;
    }

    if (has_subscription == false) {
        params->value = 0x0000;

        err = gattc_write_ccc(conn, params);
        if (err) {
            /* -ENOTCONN is expected on the disconnect race: upper layer saw the
             * conn as still connected but the peer was already gone by the time
             * the write was issued. Not a real failure.
             */
            if (err != -ENOTCONN) {
                LOG_ERR("WrCccFail[%d]", err);
            }
            return err;
        }
    }

    sys_slist_find_and_remove(&sub->list, &params->node);

    if (sys_slist_is_empty(&sub->list)) {
        LOG_DBG("SubListEmpty");
        bt_addr_le_copy(&sub->peer, BT_ADDR_LE_ANY);
    }

    LOG_DBG("GattcUnsub[%u][%d]", has_subscription, err);

    if (has_subscription || err) {
        /* Notify with NULL data to complete unsubscribe */
        params->notify(conn, params, NULL, 0);
    }

    return 0;
}

_LIB_IDF
int bt_gatt_read(struct bt_conn *conn, struct bt_gatt_read_params *params)
{
    assert(conn);
    assert(params);

    LOG_DBG("GattRd[%u]", params->handle_count);

    if (conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    if (params->handle_count > 1) {
        LOG_ERR("NotSuppToRdHdl[%u]", params->handle_count);
        return -ENOTSUP;
    }

    if (params->handle_count == 0) {
        assert(params->by_uuid.uuid);

        if (params->by_uuid.uuid->type != BT_UUID_TYPE_16) {
            LOG_ERR("InvUuid[%s]", bt_uuid_str(params->by_uuid.uuid));
            return -ENOTSUP;
        }
    }

    return bt_le_nimble_gattc_read(conn, params);
}

_LIB_IDF
int bt_gatt_write(struct bt_conn *conn, struct bt_gatt_write_params *params)
{
    assert(conn);
    assert(params);
    assert(params->func);
    assert(params->handle);
    assert((!params->data ^ !params->length) == 0);

    LOG_DBG("GattWr[%u][%u][%u]", params->handle, params->length, params->offset);

    if (conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    if (params->offset != 0) {
        LOG_ERR("PrepWrNotSupp[%u][%u]", params->handle, params->offset);
        return -ENOTSUP;
    }

    return bt_le_nimble_gattc_write(conn, params);
}

_LIB_ONLY
int bt_gatt_write_without_response_cb(struct bt_conn *conn, uint16_t handle,
                                      const void *data, uint16_t length,
                                      bool sign, bt_gatt_complete_func_t func,
                                      void *user_data)
{
    assert(conn);
    assert(handle);
    assert((!data ^ !length) == 0);

    ARG_UNUSED(func);
    ARG_UNUSED(user_data);

    LOG_DBG("GattWrCmd[%u][%u][%u]", handle, length, sign);

    if (conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("NotConn[%d]", __LINE__);
        return -ENOTCONN;
    }

    if (sign) {
        LOG_ERR("WrCmdNotSupp");
        return -ENOTSUP;
    }

    return bt_le_nimble_gattc_write_without_rsp(conn, handle, data, length);
}

_LIB_ONLY
void bt_le_acl_conn_disconnected_gatt_listener(uint16_t conn_handle)
{
    struct bt_conn *conn;

    LOG_DBG("AclConnDisconnectedGattListener[%u]", conn_handle);

    bt_le_nimble_gatt_nrp_clear(conn_handle);

    /* Drop the cached GATT client database so the next connection runs a
     * fresh discovery. This keeps the client resilient when the peer's
     * bonding is lost (e.g., server reboot without persisted bonds) or its
     * GATT structure changes across reconnects.
     *
     * TODO: preserve the cached DB across reconnects to the same bonded
     * peer, and invalidate it only on a Service Changed indication. That
     * saves the rediscovery round-trip but requires honoring CCCD caching
     * and Service Changed handling end-to-end.
     */
    bt_le_nimble_gattc_db_remove(conn_handle);

    /* The cached DB is dropped above, so the app will rediscover and call
     * bt_gatt_subscribe() again on reconnect with the BAP library's reused
     * `bt_gatt_subscribe_params` pointers. Drop those nodes from the
     * tracking list (and zero their value) so the duplicate check in
     * bt_gatt_subscribe() doesn't short-circuit with -EALREADY, which
     * would otherwise skip the CCCD write and starve notifications.
     */
    conn = bt_le_acl_conn_find(conn_handle);
    if (conn) {
        gattc_sub_clear(conn);
    }
}

void bt_le_gatt_handle_event(uint8_t *data, size_t data_len)
{
    assert(data && data_len);

    bt_le_nimble_gatt_handle_event(data, data_len);
}
