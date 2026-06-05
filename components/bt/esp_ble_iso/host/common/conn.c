/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2025 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/common/bt_str.h>

#include <../host/keys.h>
#include <../host/conn_internal.h>
#include <../host/iso_internal.h>

#include "common/host.h"
#include "common/app/gap.h"

LOG_MODULE_REGISTER(ISO_CONN, CONFIG_BT_ISO_LOG_LEVEL);

static struct bt_conn acl_conns[CONFIG_BT_MAX_CONN];

extern struct bt_conn iso_conns[CONFIG_BT_ISO_MAX_CHAN];

static sys_slist_t conn_cbs = SYS_SLIST_STATIC_INIT(&conn_cbs);

static sys_slist_t auth_info_cbs = SYS_SLIST_STATIC_INIT(&auth_info_cbs);

_IDF_ONLY
void bt_conn_get_acl_conns(struct bt_conn **conns, uint8_t *count)
{
    LOG_DBG("GetAclConns[%u]", ARRAY_SIZE(acl_conns));

    *conns = acl_conns;
    *count = ARRAY_SIZE(acl_conns);
}

_IDF_ONLY
bool bt_conn_is_peer_addr_le(const struct bt_conn *conn, uint8_t id,
                             const bt_addr_le_t *peer)
{
    /* LOG_DBG("IsLeAddr[%s][%s]", bt_addr_le_str(&conn->le.dst), bt_addr_le_str(peer)); */

    ARG_UNUSED(id);

    assert(conn && peer);

    /* Check against conn dst address as it may be the identity address */
    if (bt_addr_le_eq(peer, &conn->le.dst)) {
        return true;
    }

    return false;
}

_LIB_IDF
struct bt_conn *bt_conn_ref(struct bt_conn *conn)
{
    return conn;
}

_LIB_IDF
void bt_conn_unref(struct bt_conn *conn)
{
    ARG_UNUSED(conn);
}

_LIB_ONLY
void bt_conn_foreach(enum bt_conn_type type,
                     void (*func)(struct bt_conn *conn, void *data),
                     void *data)
{
    assert(type == BT_CONN_TYPE_LE);

    for (size_t i = 0; i < ARRAY_SIZE(acl_conns); i++) {
        struct bt_conn *conn = &acl_conns[i];

        /* Add this to avoid unexpected error log */
        if (conn->type == BT_CONN_TYPE_LE) {
            func(&acl_conns[i], data);
        }
    }
}

_LIB_ONLY
const bt_addr_le_t *bt_conn_get_dst(const struct bt_conn *conn)
{
    assert(conn);

    return &conn->le.dst;
}

_LIB_IDF
uint8_t bt_conn_index(const struct bt_conn *conn)
{
    ptrdiff_t index = 0;

    assert(conn);

    switch (conn->type) {
    case BT_CONN_TYPE_LE:
        index = conn - acl_conns;
        assert(index >= 0 && index < ARRAY_SIZE(acl_conns));
        break;
    case BT_CONN_TYPE_ISO:
        index = conn - iso_conns;
        assert(index >= 0 && index < ARRAY_SIZE(iso_conns));
        break;
    default:
        assert(0);
        break;
    }

    /* LOG_DBG("ConnIdx[%u]", index); */

    return (uint8_t)index;
}

#if CONFIG_BT_SMP
_IDF_ONLY
int bt_conn_set_security(struct bt_conn *conn, bt_security_t sec)
{
    /* This function is used by CIS to set security level,
     * currently always return failure here to make sure
     * that before creating CIS, the ACL connection is
     * encrypted.
     */
    return -ENOTSUP;
}
#endif /* CONFIG_BT_SMP */

static enum bt_conn_state conn_internal_to_public_state(bt_conn_state_t state)
{
    switch (state) {
    case BT_CONN_DISCONNECTED:
    case BT_CONN_DISCONNECT_COMPLETE:
        return BT_CONN_STATE_DISCONNECTED;
    case BT_CONN_INITIATING:
        return BT_CONN_STATE_CONNECTING;
    case BT_CONN_CONNECTED:
        return BT_CONN_STATE_CONNECTED;
    case BT_CONN_DISCONNECTING:
        return BT_CONN_STATE_DISCONNECTING;
    default:
        assert(0);
        return 0;
    }
}

_LIB_ONLY
int bt_conn_get_info(const struct bt_conn *conn, struct bt_conn_info *info)
{
    assert(conn && info);

    info->type = conn->type;
    info->role = conn->role;
    info->id = conn->id;
    info->state = conn_internal_to_public_state(conn->state);
    info->le.dst = &conn->le.dst;
    info->le.interval = conn->le.interval;

    if (conn->encrypt) {
        /* Currently the flags is updated for lib usage.
         * TODO:
         * Update the flags based on the security information.
         */
        info->security.flags = (BT_SECURITY_FLAG_SC | BT_SECURITY_FLAG_OOB);
        info->security.level = conn->sec_level;
        info->security.enc_key_size = BT_ENC_KEY_SIZE_MAX;  /* 16 octets */
    } else {
        info->security.flags = 0;
        info->security.level = BT_SECURITY_L1;
        info->security.enc_key_size = 0;
    }

    /* LOG_DBG("ConnGetInfo[%u][%02x]", info->state, info->security.flags); */

    return 0;
}

_LIB_ONLY
int bt_conn_cb_register(struct bt_conn_cb *cb)
{
    LOG_DBG("ConnCbReg");

    if (cb == NULL) {
        LOG_ERR("ConnCbNull");
        return -EINVAL;
    }

    if (sys_slist_find(&conn_cbs, &cb->_node, NULL)) {
        LOG_WRN("ConnCbExist");
        return -EEXIST;
    }

    sys_slist_append(&conn_cbs, &cb->_node);

    return 0;
}

_IDF_ONLY
int bt_conn_cb_register_safe(struct bt_conn_cb *cb)
{
    int err;
    bt_le_host_lock();
    err = bt_conn_cb_register(cb);
    bt_le_host_unlock();
    return err;
}

_NOT_USED
int bt_conn_cb_unregister(struct bt_conn_cb *cb)
{
    LOG_DBG("ConnCbUnreg");

    if (cb == NULL) {
        LOG_ERR("ConnCbNull");
        return -EINVAL;
    }

    if (!sys_slist_find_and_remove(&conn_cbs, &cb->_node)) {
        LOG_ERR("ConnCbNotFound");
        return -EALREADY;
    }

    return 0;
}

_LIB_ONLY
int bt_conn_auth_info_cb_register(struct bt_conn_auth_info_cb *cb)
{
    LOG_DBG("ConnAuthInfoCbReg");

    if (cb == NULL) {
        LOG_ERR("ConnAuthInfoCbNull");
        return -EINVAL;
    }

    if (sys_slist_find(&auth_info_cbs, &cb->node, NULL)) {
        LOG_WRN("ConnAuthInfoCbExist");
        return -EALREADY;
    }

    sys_slist_append(&auth_info_cbs, &cb->node);

    return 0;
}

_NOT_USED
int bt_conn_auth_info_cb_unregister(struct bt_conn_auth_info_cb *cb)
{
    LOG_DBG("ConnAuthInfoCbUnreg");

    if (cb == NULL) {
        LOG_ERR("ConnAuthInfoCbNull");
        return -EINVAL;
    }

    if (!sys_slist_find_and_remove(&auth_info_cbs, &cb->node)) {
        LOG_ERR("ConnAuthInfoCbNotFound");
        return -EALREADY;
    }

    return 0;
}

_IDF_ONLY
struct bt_conn *bt_conn_new(struct bt_conn *conns, size_t size)
{
    assert(conns);

    for (size_t i = 0; i < size; i++) {
        struct bt_conn *conn = &conns[i];

        if (conn->type == BT_CONN_TYPE_NONE) {
            (void)memset(conn, 0, sizeof(struct bt_conn));
            return conn;
        }
    }

    return NULL;
}

static struct bt_conn *acl_conn_lookup(uint16_t handle)
{
    for (size_t i = 0; i < ARRAY_SIZE(acl_conns); i++) {
        if (acl_conns[i].type == BT_CONN_TYPE_LE &&
                acl_conns[i].handle == handle) {
            return &acl_conns[i];
        }
    }

    LOG_INF("AclConnNotFound[%u]", handle);
    return NULL;
}

static struct bt_conn *iso_conn_lookup(uint16_t handle)
{
    for (size_t i = 0; i < ARRAY_SIZE(iso_conns); i++) {
        if (iso_conns[i].type == BT_CONN_TYPE_ISO &&
                iso_conns[i].handle == handle) {
            return &iso_conns[i];
        }
    }

    LOG_INF("IsoConnNotFound[%u]", handle);
    return NULL;
}

_IDF_ONLY
struct bt_conn *bt_conn_lookup_handle(uint16_t handle, enum bt_conn_type type)
{
    struct bt_conn *conn = NULL;

    assert(type == BT_CONN_TYPE_LE || type == BT_CONN_TYPE_ISO);

    /* LOG_DBG("ConnLookupHdl[%u][%u]", handle, type); */

    if (type == BT_CONN_TYPE_LE) {
        conn = acl_conn_lookup(handle);
    } else {
        conn = iso_conn_lookup(handle);
    }

    return conn;
}

_IDF_ONLY
struct bt_conn *bt_le_acl_conn_find(uint16_t conn_handle)
{
    /* LOG_DBG("AclConnFind[%u]", conn_handle); */

    return bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);
}

_IDF_ONLY
struct bt_conn *bt_le_acl_conn_find_safe(uint16_t conn_handle)
{
    struct bt_conn *conn = NULL;
    /* LOG_DBG("AclConnFind[%u]", conn_handle); */
    bt_le_host_lock();
    conn = bt_conn_lookup_handle(conn_handle, BT_CONN_TYPE_LE);
    bt_le_host_unlock();
    return conn;
}

_IDF_ONLY
int bt_le_acl_conn_new(uint16_t conn_handle,
                       uint8_t role,
                       bt_addr_le_t *dst,
                       uint8_t sec_level)
{
    struct bt_conn *conn;

    assert(dst);

    LOG_DBG("AclNew[%u][%u][%u][%s]",
            conn_handle, role, sec_level, bt_addr_le_str(dst));

    conn = bt_conn_new(acl_conns, ARRAY_SIZE(acl_conns));
    if (conn) {
        conn->handle = conn_handle;
        conn->type = BT_CONN_TYPE_LE;
        conn->role = role;
        conn->state = BT_CONN_CONNECTED;
        conn->sec_level = sec_level;
        memcpy(&conn->le.dst, dst, sizeof(conn->le.dst));
    } else {
        LOG_ERR("NoFreeConn[%u]", conn_handle);
    }

    return (conn ? 0 : -ENOMEM);
}

_IDF_ONLY
int bt_le_acl_conn_delete(uint16_t conn_handle)
{
    struct bt_conn *conn;

    LOG_INF("AclConnDel[%u]", conn_handle);

    /* disconnected_listener already flipped state to DISCONNECTED. */
    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_DISCONNECTED) {
        LOG_ERR("AclConnDelNotDisc[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return -ENOTCONN;
    }

    memset(conn, 0, sizeof(struct bt_conn));

    return 0;
}

_IDF_ONLY
int bt_le_acl_conn_update(uint16_t conn_handle,
                          uint8_t sec_level,
                          bool *encrypted)
{
    struct bt_conn *conn;

    LOG_DBG("AclConnUpd[%u][%u]", conn_handle, sec_level);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("AclConnUpdNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return -ENOTCONN;
    }

    if (encrypted) {
        *encrypted = false;
    }

    if (conn->sec_level < sec_level) {
        /* No encryption to encryption */
        if (conn->encrypt == 0 && conn->sec_level == BT_SECURITY_L1) {
            conn->encrypt = 1;

            if (encrypted) {
                *encrypted = true;
            }
        }

        LOG_DBG("SecLevelUpd[%u][%u]", conn->sec_level, sec_level);
        conn->sec_level = sec_level;
    }

    return 0;
}

static void acl_conn_set_state(struct bt_conn *conn, bt_conn_state_t state)
{
    LOG_DBG("AclConnSetState");

    conn->state = state;

    switch (conn->state) {
    case BT_CONN_CONNECTED:
        break;
    case BT_CONN_INITIATING:
        break;
    case BT_CONN_DISCONNECT_COMPLETE:
        break;
    default:
        break;
    }
}

static void iso_conn_set_state(struct bt_conn *conn, bt_conn_state_t state)
{
    LOG_DBG("IsoConnSetState[%u]", state);

    conn->state = state;

    switch (conn->state) {
    case BT_CONN_CONNECTED:
        bt_iso_connected(conn);
        break;
    case BT_CONN_INITIATING:
        break;
    case BT_CONN_DISCONNECT_COMPLETE:
        break;
    default:
        break;
    }
}

_IDF_ONLY
void bt_conn_set_state(struct bt_conn *conn, bt_conn_state_t state)
{
    bt_conn_state_t old_state;

    assert(conn);
    assert(conn->type == BT_CONN_TYPE_LE || conn->type == BT_CONN_TYPE_ISO);

    LOG_DBG("ConnSetState[%u][%u]", conn->state, state);

    if (conn->state == state) {
        return;
    }

    old_state = conn->state;

    switch (old_state) {
    case BT_CONN_DISCONNECTED:
        /* TODO: check if anything to do */
        break;
    case BT_CONN_INITIATING:
        /* TODO: check if anything to do */
        break;
    default:
        break;
    }

    if (conn->type == BT_CONN_TYPE_LE) {
        acl_conn_set_state(conn, state);
    } else {
        iso_conn_set_state(conn, state);
    }
}

static int iso_disconnect(struct bt_conn *conn, uint8_t reason)
{
    return bt_le_iso_disconnect(conn->handle, reason);
}

_IDF_ONLY
int bt_conn_disconnect(struct bt_conn *conn, uint8_t reason)
{
    assert(conn);

    LOG_DBG("ConnDisconnect[%u][%u][%02x]", conn->state, conn->type, reason);

    switch (conn->state) {
    case BT_CONN_CONNECTED:
        if (conn->type == BT_CONN_TYPE_LE) {
            return -ENOTSUP;
        }

        if (conn->type == BT_CONN_TYPE_ISO) {
            return iso_disconnect(conn, reason);
        }

        assert(0);
    case BT_CONN_DISCONNECTING:
        return 0;
    case BT_CONN_DISCONNECTED:
    default:
        return -ENOTCONN;
    }
}

_IDF_ONLY
int bt_le_acl_conn_connected_listener(uint16_t conn_handle)
{
    struct bt_conn_cb *listener = NULL;
    struct bt_conn *conn;

    LOG_DBG("AclConnConnectedListener[%u]", conn_handle);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("AclConnConnectedListenerNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return -ENOTCONN;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&conn_cbs, listener, _node) {
        if (listener->connected) {
            listener->connected(conn, BT_HCI_ERR_SUCCESS);
        }
    }

    return 0;
}

_IDF_ONLY
int bt_le_acl_conn_disconnected_listener(uint16_t conn_handle, uint8_t reason)
{
    struct bt_conn_cb *listener = NULL;
    struct bt_conn *conn;

    LOG_DBG("AclConnDisconnectedListener[%u][%02x]", conn_handle, reason);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("AclConnDisconnectedListenerNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return -ENOTCONN;
    }

    /* Flip before dispatch — lib disconnect cbs guard late notifies on
     * bt_conn_get_info().state. Otherwise BTA queues the send and the conn
     * is gone by the time BTU drains it ("Unknown connection ID"). */
    conn->state = BT_CONN_DISCONNECTED;

    SYS_SLIST_FOR_EACH_CONTAINER(&conn_cbs, listener, _node) {
        if (listener->disconnected) {
            listener->disconnected(conn, reason);
        }
    }

#if CONFIG_BT_ISO_UNICAST
    bt_le_acl_conn_disconnected_gatt_listener(conn_handle);
#endif /* CONFIG_BT_ISO_UNICAST */

    return 0;
}

_IDF_ONLY
int bt_le_acl_conn_security_changed_listener(uint16_t conn_handle, bt_security_t level)
{
    struct bt_conn_cb *listener = NULL;
    struct bt_conn *conn;

    LOG_DBG("AclConnSecurityChangedListener[%u][%u]", conn_handle, level);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("AclConnSecChgListenerNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return -ENOTCONN;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&conn_cbs, listener, _node) {
        if (listener->security_changed) {
            listener->security_changed(conn, level, BT_SECURITY_ERR_SUCCESS);
        }
    }

    return 0;
}

_IDF_ONLY
int bt_le_acl_conn_identity_resolved_listener(uint16_t conn_handle,
                                              const bt_addr_le_t *rpa,
                                              const bt_addr_le_t *identity)
{
    struct bt_conn_cb *listener = NULL;
    struct bt_conn *conn;

    LOG_DBG("AclConnIdentityResolvedListener[%u][%s]",
            conn_handle, bt_addr_le_str(rpa));
    LOG_DBG("AclConnIdentityResolvedListener[%s]",
            bt_addr_le_str(identity));

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("AclConnIdResolvedListenerNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return -ENOTCONN;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&conn_cbs, listener, _node) {
        if (listener->identity_resolved) {
            listener->identity_resolved(conn, rpa, identity);
        }
    }

    return 0;
}

_IDF_ONLY
int bt_le_acl_conn_pairing_completed_listener(uint16_t conn_handle, bool bonded)
{
    struct bt_conn_auth_info_cb *listener = NULL;
    struct bt_conn *conn;

    LOG_DBG("AclConnPairingCompletedListener[%u][%u]", conn_handle, bonded);

    conn = bt_le_acl_conn_find(conn_handle);
    if (conn == NULL || conn->state != BT_CONN_CONNECTED) {
        LOG_ERR("AclConnPairingCompletedListenerNotConn[%u][%u]", conn_handle, BT_CONN_STATE_GET(conn));
        return -ENOTCONN;
    }

    SYS_SLIST_FOR_EACH_CONTAINER(&auth_info_cbs, listener, node) {
        if (listener->pairing_complete) {
            listener->pairing_complete(conn, bonded);
        }
    }

    return 0;
}

_IDF_ONLY
int bt_le_acl_conn_bond_deleted_listener(uint8_t id, const bt_addr_le_t *peer)
{
    struct bt_conn_auth_info_cb *listener = NULL;

    LOG_DBG("AclConnBondDeletedListener[%02x][%s]", id, bt_addr_le_str(peer));

    SYS_SLIST_FOR_EACH_CONTAINER(&auth_info_cbs, listener, node) {
        if (listener->bond_deleted) {
            listener->bond_deleted(id, peer);
        }
    }

    return 0;
}
