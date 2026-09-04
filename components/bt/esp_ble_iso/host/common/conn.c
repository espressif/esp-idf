/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2025 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/l2cap.h>
#include <zephyr/bluetooth/common/bt_str.h>

#include <../host/keys.h>
#include <../host/conn_internal.h>
#include <../host/iso_internal.h>

#include "common/host.h"
#include "common/app/gap.h"

LOG_MODULE_REGISTER(ISO_CONN, CONFIG_BT_ISO_LOG_LEVEL);

static BT_ISO_EXT_RAM_BSS_ATTR struct bt_conn acl_conns[CONFIG_BT_MAX_CONN];

/* Per-ACL LTK backing store (indexed in lockstep with acl_conns[]). conn->le.keys is
 * a bare pointer the adapters fill after bonding; point it at the matching slot so the
 * lib's CSIS sirk_encrypt can read conn->le.keys->ltk.val. */
static BT_ISO_EXT_RAM_BSS_ATTR struct bt_keys conn_ltk[CONFIG_BT_MAX_CONN];

extern struct bt_conn iso_conns[CONFIG_BT_ISO_MAX_CHAN];

static BT_ISO_EXT_RAM_BSS_ATTR sys_slist_t conn_cbs;

static BT_ISO_EXT_RAM_BSS_ATTR sys_slist_t auth_info_cbs;

_IDF_ONLY
void bt_conn_get_acl_conns(struct bt_conn **conns, uint8_t *count)
{
    LOG_DBG("GetAclConns[%u]", ARRAY_SIZE(acl_conns));

    *conns = acl_conns;
    *count = ARRAY_SIZE(acl_conns);
}

size_t bt_le_acl_conn_count(void)
{
    size_t count = 0;

    for (size_t i = 0; i < ARRAY_SIZE(acl_conns); i++) {
        if (acl_conns[i].state != BT_CONN_DISCONNECTED) {
            count++;
        }
    }

    return count;
}

void bt_le_conn_reset(void)
{
    LOG_DBG("ConnReset");

    memset(acl_conns, 0, sizeof(acl_conns));
    memset(conn_ltk, 0, sizeof(conn_ltk));
    memset(iso_conns, 0, sizeof(struct bt_conn) * CONFIG_BT_ISO_MAX_CHAN);

    sys_slist_init(&conn_cbs);
    sys_slist_init(&auth_info_cbs);
}

_IDF_ONLY
bool bt_conn_is_peer_addr_le(const struct bt_conn *conn, uint8_t id,
                             const bt_addr_le_t *peer)
{
    /* LOG_DBG("IsLeAddr[%s][%s]", bt_addr_le_str(&conn->le.dst), bt_addr_le_str(peer)); */

    ARG_UNUSED(id);

    BT_LE_ASSERT(conn && peer);

    /* TODO(privacy): also match init/resp or resolved identity; dst may stay RPA. */

    if (bt_addr_le_eq(peer, &conn->le.dst)) {
        return true;
    }

    return false;
}

_LIB_ONLY
void bt_conn_foreach(enum bt_conn_type type,
                     void (*func)(struct bt_conn *conn, void *data),
                     void *data)
{
    BT_LE_ASSERT(type == BT_CONN_TYPE_LE);

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
    BT_LE_ASSERT(conn);

    return &conn->le.dst;
}

_LIB_IDF
uint8_t bt_conn_index(const struct bt_conn *conn)
{
    ptrdiff_t index = 0;

    BT_LE_ASSERT(conn);

    switch (conn->type) {
    case BT_CONN_TYPE_LE:
        index = conn - acl_conns;
        BT_LE_ASSERT(index >= 0 && index < ARRAY_SIZE(acl_conns));
        break;
    case BT_CONN_TYPE_ISO:
        index = conn - iso_conns;
        BT_LE_ASSERT(index >= 0 && index < ARRAY_SIZE(iso_conns));
        break;
    default:
        BT_LE_ASSERT(0);
        break;
    }

    /* LOG_DBG("ConnIdx[%u]", index); */

    return (uint8_t)index;
}

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
        BT_LE_ASSERT(0);
        return 0;
    }
}

_LIB_ONLY
int bt_conn_get_info(const struct bt_conn *conn, struct bt_conn_info *info)
{
    BT_LE_ASSERT(conn && info);

    /* Callers today only pass ACL (BT_CONN_TYPE_LE). ISO would need
     * info->le.* from conn->iso.acl (union); not implemented until needed. */

    info->type = conn->type;
    info->role = conn->role;
    info->id = conn->id;
    info->state = conn_internal_to_public_state(conn->state);
    info->le.dst = &conn->le.dst;
    info->le.interval_us = conn->le.interval_us;

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

_IDF_ONLY
int bt_conn_cb_unregister_safe(struct bt_conn_cb *cb)
{
    int err;
    bt_le_host_lock();
    err = bt_conn_cb_unregister(cb);
    bt_le_host_unlock();
    return err;
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
    BT_LE_ASSERT(conns);

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

    BT_LE_ASSERT(type == BT_CONN_TYPE_LE || type == BT_CONN_TYPE_ISO);

    /* LOG_DBG("ConnLookupHdl[%u][%u]", handle, type); */

    if (type == BT_CONN_TYPE_LE) {
        conn = acl_conn_lookup(handle);
    } else {
        conn = iso_conn_lookup(handle);
    }

    return conn;
}

struct bt_conn *bt_conn_lookup_index(uint8_t index)
{
    struct bt_conn *conn;

    if (index >= ARRAY_SIZE(acl_conns)) {
        return NULL;
    }

    conn = &acl_conns[index];

    /* Free/wiped slots are type NONE after acl_conn_delete. */
    if (conn->type == BT_CONN_TYPE_NONE) {
        return NULL;
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
int bt_le_acl_conn_new(uint16_t conn_handle,
                       uint8_t role,
                       bt_addr_le_t *dst,
                       uint8_t sec_level)
{
    struct bt_conn *conn;

    BT_LE_ASSERT(dst);

    LOG_DBG("AclNew[%u][%u][%u][%s]",
            conn_handle, role, sec_level, bt_addr_le_str(dst));

    conn = bt_conn_new(acl_conns, ARRAY_SIZE(acl_conns));
    if (conn) {
        conn->handle = conn_handle;
        conn->type = BT_CONN_TYPE_LE;
        conn->role = role;
        conn->state = BT_CONN_CONNECTED;
        conn->sec_level = sec_level;
        /* AUTH-before-ACL-slot path may create with L2+ directly. */
        if (sec_level > BT_SECURITY_L1) {
            conn->encrypt = 1;
        }
        memcpy(&conn->le.dst, dst, sizeof(conn->le.dst));
    } else {
        LOG_ERR("NoFreeConn[%u]", conn_handle);
    }

    return (conn ? 0 : -ENOMEM);
}

_IDF_ONLY
int bt_le_acl_conn_new_safe(uint16_t conn_handle, uint8_t role, uint8_t addr_type,
                            const uint8_t *addr, uint8_t sec_level)
{
    bt_addr_le_t dst;
    int err;

    if (addr == NULL) {
        return -EINVAL;
    }

    dst.type = addr_type;
    bt_addr_copy(&dst.a, (const bt_addr_t *)addr);

    bt_le_host_lock();
    err = bt_le_acl_conn_new(conn_handle, role, &dst, sec_level);
    bt_le_host_unlock();

    return err;
}

/* Point conn->le.keys at this ACL connection's LTK slot, filled with the bonded
 * LTK the adapter captured. Used as key K by the lib's CSIS SIRK encryption. */
_IDF_ONLY
void bt_conn_le_set_ltk(struct bt_conn *conn, const uint8_t *ltk)
{
    size_t idx;

    if (conn == NULL || ltk == NULL) {
        LOG_ERR("ConnSetLtkBadArg");
        return;
    }

    /* Only ACL (LE) connections carry an LTK; the pool tracks acl_conns[]. */
    if (conn < acl_conns || conn >= &acl_conns[ARRAY_SIZE(acl_conns)]) {
        LOG_WRN("ConnSetLtkNotAcl");
        return;
    }

    idx = (size_t)(conn - acl_conns);

    memset(&conn_ltk[idx], 0, sizeof(conn_ltk[idx]));
    memcpy(conn_ltk[idx].ltk.val, ltk, sizeof(conn_ltk[idx].ltk.val));
    conn->le.keys = &conn_ltk[idx];

    LOG_INF("ConnSetLtk[%u][%s]", conn->handle, bt_hex(ltk, 16));
}

#if CONFIG_BT_ISO_UNICAST
/* Defer ACL slot wipe while a CIS still holds iso.acl. */
static bool bt_iso_acl_has_cis(const struct bt_conn *acl)
{
    for (size_t i = 0; i < ARRAY_SIZE(iso_conns); i++) {
        if (iso_conns[i].type == BT_CONN_TYPE_ISO && iso_conns[i].iso.acl == acl) {
            return true;
        }
    }

    return false;
}
#endif /* CONFIG_BT_ISO_UNICAST */

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

#if CONFIG_BT_ISO_UNICAST
    if (bt_iso_acl_has_cis(conn)) {
        LOG_INF("AclConnDelDeferred[%u]", conn_handle);
        return 0;
    }
#endif /* CONFIG_BT_ISO_UNICAST */

    /* OTS CoC (and any L2CAP) keep chan->conn on this list. Wipe without
     * detaching leaves dangling pointers if ACL slot is reused. */
    {
        struct bt_l2cap_chan *chan, *tmp;

        SYS_SLIST_FOR_EACH_CONTAINER_SAFE(&conn->channels, chan, tmp, node) {
            sys_slist_find_and_remove(&conn->channels, &chan->node);
            if (chan->ops != NULL && chan->ops->disconnected != NULL) {
                chan->ops->disconnected(chan);
            }
            chan->conn = NULL;
        }
    }

    /* Wipe this connection's LTK slot (key hygiene); the memset below then nulls
     * conn->le.keys. */
    memset(&conn_ltk[conn - acl_conns], 0, sizeof(conn_ltk[0]));

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
        /* Mark encrypted on first step into L2+ (not only from L1). */
        if (conn->encrypt == 0 && sec_level > BT_SECURITY_L1) {
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

    BT_LE_ASSERT(conn);
    BT_LE_ASSERT(conn->type == BT_CONN_TYPE_LE || conn->type == BT_CONN_TYPE_ISO);

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
    BT_LE_ASSERT(conn);

    LOG_DBG("ConnDisconnect[%u][%u][%02x]", conn->state, conn->type, reason);

    switch (conn->state) {
    case BT_CONN_CONNECTED:
        if (conn->type == BT_CONN_TYPE_LE) {
            return -ENOTSUP;
        }

        if (conn->type == BT_CONN_TYPE_ISO) {
            int err = iso_disconnect(conn, reason);
            if (err == 0) {
                bt_conn_set_state(conn, BT_CONN_DISCONNECTING);
            }
            return err;
        }

        BT_LE_ASSERT(0);
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

    /* GATT sub/CCC cleanup is ACL-scoped, not ISO-unicast. */
    bt_le_acl_conn_disconnected_gatt_listener(conn_handle);

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

    /* TODO(privacy): copy identity into conn->le.dst once adapters post resolve. */

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

    /* Profiles above cleared their own per-client state; also drop the peer's
     * retained server CCC cfg, which is only kept while the bond exists. */
    bt_le_acl_conn_bond_deleted_gatt_listener(id, peer);

    return 0;
}
