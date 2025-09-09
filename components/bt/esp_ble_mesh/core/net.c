/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "crypto.h"
#include "adv.h"
#include "net.h"
#include "scan.h"
#include "mesh.h"
#include "lpn.h"
#include "rpl.h"
#include "friend.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "beacon.h"
#include "settings.h"
#include "fast_prov.h"
#include "prov_node.h"
#include "test.h"
#include "proxy_client.h"
#include "proxy_server.h"
#include "pvnr_mgmt.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif /* CONFIG_BLE_MESH_V11_SUPPORT */

/* Minimum valid Mesh Network PDU length. The Network headers
 * themselves take up 9 bytes. After that there is a minimum of 1 byte
 * payload for both CTL=1 and CTL=0 PDUs (smallest OpCode is 1 byte). CTL=1
 * PDUs must use a 64-bit (8 byte) NetMIC, whereas CTL=0 PDUs have at least
 * a 32-bit (4 byte) NetMIC and AppMIC giving again a total of 8 bytes.
 */
#define BLE_MESH_NET_MIN_PDU_LEN    (BLE_MESH_NET_HDR_LEN + 1 + 8)

/* Seq limit after IV Update is triggered */
#define IV_UPDATE_SEQ_LIMIT         8000000

/* Determine how many friendship credentials we need */
#if CONFIG_BLE_MESH_FRIEND
#define FRIEND_CRED_COUNT           CONFIG_BLE_MESH_FRIEND_LPN_COUNT
#elif CONFIG_BLE_MESH_LOW_POWER
#define FRIEND_CRED_COUNT           CONFIG_BLE_MESH_SUBNET_COUNT
#else
#define FRIEND_CRED_COUNT           0
#endif

#if CONFIG_BLE_MESH_RELAY_ADV_BUF
#define MAX_STORED_RELAY_COUNT      (CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT / 2)
#endif /* CONFIG_BLE_MESH_RELAY_ADV_BUF */

#if FRIEND_CRED_COUNT > 0
static struct friend_cred friend_cred[FRIEND_CRED_COUNT];
#endif /* FRIEND_CRED_COUNT > 0 */

static struct {
    uint32_t src:15, /* MSB of source address is always 0 */
             seq:17;
} msg_cache[CONFIG_BLE_MESH_MSG_CACHE_SIZE];
static uint16_t msg_cache_next;

/* Singleton network context (the implementation only supports one) */
struct bt_mesh_net bt_mesh = {
    .local_queue = SYS_SLIST_STATIC_INIT(&bt_mesh.local_queue),
    .sub = {
        [0 ... (CONFIG_BLE_MESH_SUBNET_COUNT - 1)] = {
            .net_idx = BLE_MESH_KEY_UNUSED,
        }
    },
    .app_keys = {
        [0 ... (CONFIG_BLE_MESH_APP_KEY_COUNT - 1)] = {
            .net_idx = BLE_MESH_KEY_UNUSED,
        }
    },
};

static uint32_t dup_cache[4];
static int dup_cache_next;

static bool check_dup(struct net_buf_simple *data)
{
    const uint8_t *tail = net_buf_simple_tail(data);
    uint32_t val = 0U;
    int i;

    val = sys_get_be32(tail - 4) ^ sys_get_be32(tail - 8);

    BT_DBG("CheckDup, Val 0x%08lx", val);

    for (i = 0; i < ARRAY_SIZE(dup_cache); i++) {
        if (dup_cache[i] == val) {
            BT_DBG("DupCacheFound");
            return true;
        }
    }

    BT_DBG("DupCacheAdd, CacheNext %ld %d", val, dup_cache_next);

    dup_cache[dup_cache_next++] = val;
    dup_cache_next %= ARRAY_SIZE(dup_cache);

    return false;
}

static bool msg_cache_match(struct bt_mesh_net_rx *rx,
                            struct net_buf_simple *pdu)
{
    int i;

    BT_DBG("MsgCacheMatch");

    for (i = 0; i < ARRAY_SIZE(msg_cache); i++) {
        if (msg_cache[i].src == BLE_MESH_NET_HDR_SRC(pdu->data) &&
            msg_cache[i].seq == (BLE_MESH_NET_HDR_SEQ(pdu->data) & BIT_MASK(17))) {
            BT_DBG("CacheFound, Src 0x%04x Seq 0x%06x",
                   msg_cache[i].src, msg_cache[i].seq);
            return true;
        }
    }

    return false;
}

static void msg_cache_add(struct bt_mesh_net_rx *rx)
{
    BT_DBG("MsgCacheAdd, Src 0x%04x Seq 0x%06x CacheNext %u",
           rx->ctx.addr, rx->seq, msg_cache_next);

    rx->msg_cache_idx = msg_cache_next++;
    msg_cache[rx->msg_cache_idx].src = rx->ctx.addr;
    msg_cache[rx->msg_cache_idx].seq = rx->seq;
    msg_cache_next %= ARRAY_SIZE(msg_cache);
}

#if CONFIG_BLE_MESH_PROVISIONER
void bt_mesh_msg_cache_clear(uint16_t unicast_addr, uint8_t elem_num)
{
    int i;

    BT_DBG("MsgCacheClear, Addr 0x%04x ElemNum %u", unicast_addr, elem_num);

    for (i = 0; i < ARRAY_SIZE(msg_cache); i++) {
        if (msg_cache[i].src >= unicast_addr &&
            msg_cache[i].src < unicast_addr + elem_num) {
            memset(&msg_cache[i], 0, sizeof(msg_cache[i]));
        }
    }
}
#endif /* CONFIG_BLE_MESH_PROVISIONER */

struct bt_mesh_subnet *bt_mesh_subnet_get(uint16_t net_idx)
{
    BT_DBG("SubnetGet, NetIdx 0x%04x", net_idx);

    if (bt_mesh_is_provisioned()) {
#if CONFIG_BLE_MESH_NODE
        if (!IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV)) {
            if (net_idx == BLE_MESH_KEY_ANY) {
                return &bt_mesh.sub[0];
            }

            for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
                if (bt_mesh.sub[i].net_idx == net_idx) {
                    return &bt_mesh.sub[i];
                }
            }
        } else {
            return bt_mesh_fast_prov_subnet_get(net_idx);
        }
#endif /* CONFIG_BLE_MESH_NODE */
    } else if (bt_mesh_is_provisioner_en()) {
#if CONFIG_BLE_MESH_PROVISIONER
        if (net_idx == BLE_MESH_KEY_ANY) {
            return bt_mesh.p_sub[0];
        }

        for (int i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
            if (bt_mesh.p_sub[i] &&
                bt_mesh.p_sub[i]->net_idx == net_idx) {
                return bt_mesh.p_sub[i];
            }
        }
#endif /* CONFIG_BLE_MESH_PROVISIONER */
    }

    return NULL;
}

int bt_mesh_net_keys_create(struct bt_mesh_subnet_keys *keys,
                            const uint8_t key[16])
{
    uint8_t p[] = { 0 };
    uint8_t nid = 0U;
    int err = 0;

    BT_DBG("NetKeysCreate");

    err = bt_mesh_k2(key, p, sizeof(p), &nid, keys->enc, keys->privacy);
    if (err) {
        BT_ERR("Unable to generate NID, EncKey & PrivacyKey");
        return err;
    }

    memcpy(keys->net, key, 16);

    keys->nid = nid;

    BT_DBG("NID 0x%02x EncKey %s", keys->nid, bt_hex(keys->enc, 16));
    BT_DBG("PrivacyKey %s", bt_hex(keys->privacy, 16));

    err = bt_mesh_k3(key, keys->net_id);
    if (err) {
        BT_ERR("Unable to generate Net ID");
        return err;
    }

    BT_DBG("NetID %s", bt_hex(keys->net_id, 8));

#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    err = bt_mesh_identity_key(key, keys->identity);
    if (err) {
        BT_ERR("Unable to generate IdentityKey");
        return err;
    }

    BT_DBG("IdentityKey %s", bt_hex(keys->identity, 16));
#endif /* GATT_PROXY */

    err = bt_mesh_secure_beacon_key(key, keys->beacon);
    if (err) {
        BT_ERR("Unable to generate beacon key");
        return err;
    }

    BT_DBG("BeaconKey %s", bt_hex(keys->beacon, 16));

#if CONFIG_BLE_MESH_PRIVATE_BEACON
    err = bt_mesh_private_beacon_key(key, keys->private_beacon);
    if (err) {
        BT_ERR("Unable to generate private beacon key");
        return err;
    }

    BT_DBG("PrivateBeaconKey %s", bt_hex(keys->private_beacon, 16));
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */

#if CONFIG_BLE_MESH_DF_SRV
    p[0] = 0x02;

    err = bt_mesh_k2(key, p, sizeof(p), &nid, keys->direct_enc, keys->direct_privacy);
    if (err) {
        BT_ERR("Unable to generate directed NID, EncKey & PrivacyKey");
        return err;
    }

    keys->direct_nid = nid;
#endif /* CONFIG_BLE_MESH_DF_SRV */

    return 0;
}

#if (CONFIG_BLE_MESH_LOW_POWER || CONFIG_BLE_MESH_FRIEND)
int friend_cred_set(struct friend_cred *cred, uint8_t idx, const uint8_t net_key[16])
{
    uint16_t lpn_addr = 0U, frnd_addr = 0U;
    uint8_t p[9] = {0};
    int err = 0;

    BT_DBG("FrndCredSet, NetIdx 0x%04x Addr 0x%04x Idx %u",
           cred->net_idx, cred->addr, idx);

#if CONFIG_BLE_MESH_LOW_POWER
    if (cred->addr == bt_mesh.lpn.frnd) {
        lpn_addr = bt_mesh_primary_addr();
        frnd_addr = cred->addr;
    } else {
        lpn_addr = cred->addr;
        frnd_addr = bt_mesh_primary_addr();
    }
#else /* CONFIG_BLE_MESH_LOW_POWER */
    lpn_addr = cred->addr;
    frnd_addr = bt_mesh_primary_addr();
#endif /* CONFIG_BLE_MESH_LOW_POWER */

    BT_DBG("LPN 0x%04x Frnd 0x%04x LPNCounter %u FrndCounter %u",
           lpn_addr, frnd_addr, cred->lpn_counter, cred->frnd_counter);

    p[0] = 0x01;
    sys_put_be16(lpn_addr, p + 1);
    sys_put_be16(frnd_addr, p + 3);
    sys_put_be16(cred->lpn_counter, p + 5);
    sys_put_be16(cred->frnd_counter, p + 7);

    err = bt_mesh_k2(net_key, p, sizeof(p), &cred->cred[idx].nid,
                     cred->cred[idx].enc, cred->cred[idx].privacy);
    if (err) {
        BT_ERR("Unable to generate NID, EncKey & PrivacyKey");
        return err;
    }

    BT_DBG("FrndNID 0x%02x EncKey %s",
           cred->cred[idx].nid, bt_hex(cred->cred[idx].enc, 16));
    BT_DBG("FrndPrivacyKey %s", bt_hex(cred->cred[idx].privacy, 16));

    return 0;
}

void friend_cred_refresh(uint16_t net_idx)
{
    int i;

    BT_DBG("FrndCredRefresh, NetIdx 0x%04x", net_idx);

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

        if (cred->addr != BLE_MESH_ADDR_UNASSIGNED &&
            cred->net_idx == net_idx) {
            BT_DBG("Refreshed, Addr 0x%04x", cred->addr);
            memcpy(&cred->cred[0], &cred->cred[1],
                   sizeof(cred->cred[0]));
        }
    }
}

int friend_cred_update(struct bt_mesh_subnet *sub)
{
    int err = 0, i;

    BT_DBG("FrndCredUpdate, NetIdx 0x%04x", sub->net_idx);

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

        if (cred->addr == BLE_MESH_ADDR_UNASSIGNED ||
            cred->net_idx != sub->net_idx) {
            continue;
        }

        BT_DBG("UpdateFound, Addr 0x%04x", cred->addr);

        err = friend_cred_set(cred, 1, sub->keys[1].net);
        if (err) {
            return err;
        }
    }

    return 0;
}

struct friend_cred *friend_cred_create(struct bt_mesh_subnet *sub, uint16_t addr,
                                       uint16_t lpn_counter, uint16_t frnd_counter)
{
    struct friend_cred *cred = NULL;
    int i, err = 0;

    BT_DBG("FrndCredCreate, NetIdx 0x%04x KrFlag %u Addr 0x%04x",
           sub->net_idx, sub->kr_flag, addr);

    for (cred = NULL, i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        if ((friend_cred[i].addr == BLE_MESH_ADDR_UNASSIGNED) ||
            (friend_cred[i].addr == addr &&
             friend_cred[i].net_idx == sub->net_idx)) {
            cred = &friend_cred[i];
            break;
        }
    }

    if (!cred) {
        BT_WARN("No free friend credential slots");
        return NULL;
    }

    cred->net_idx = sub->net_idx;
    cred->addr = addr;
    cred->lpn_counter = lpn_counter;
    cred->frnd_counter = frnd_counter;

    err = friend_cred_set(cred, 0, sub->keys[0].net);
    if (err) {
        friend_cred_clear(cred);
        return NULL;
    }

    if (sub->kr_flag) {
        err = friend_cred_set(cred, 1, sub->keys[1].net);
        if (err) {
            friend_cred_clear(cred);
            return NULL;
        }
    }

    return cred;
}

void friend_cred_clear(struct friend_cred *cred)
{
    BT_DBG("FrndCredClear, NetIdx 0x%04x Addr 0x%04x", cred->net_idx, cred->addr);

    cred->net_idx = BLE_MESH_KEY_UNUSED;
    cred->addr = BLE_MESH_ADDR_UNASSIGNED;
    cred->lpn_counter = 0U;
    cred->frnd_counter = 0U;
    (void)memset(cred->cred, 0, sizeof(cred->cred));
}

int friend_cred_del(uint16_t net_idx, uint16_t addr)
{
    int i;

    BT_DBG("FrndCredDel, NetIdx 0x%04x Addr 0x%04x", net_idx, addr);

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

        BT_DBG("%u: NetIdx 0x%04x Addr 0x%04x", i, cred->net_idx, cred->addr);

        if (cred->addr == addr && cred->net_idx == net_idx) {
            friend_cred_clear(cred);
            return 0;
        }
    }

    return -ENOENT;
}

int friend_cred_get(struct bt_mesh_subnet *sub, uint16_t addr, uint8_t *nid,
                    const uint8_t **enc, const uint8_t **priv)
{
    int i;

    BT_DBG("FrndCredGet, NetIdx 0x%04x KrFlag %u Addr 0x%04x",
           sub->net_idx, sub->kr_flag, addr);

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

        if (cred->net_idx != sub->net_idx) {
            continue;
        }

        if (addr != BLE_MESH_ADDR_UNASSIGNED && cred->addr != addr) {
            continue;
        }

        if (nid) {
            *nid = cred->cred[sub->kr_flag].nid;
        }

        if (enc) {
            *enc = cred->cred[sub->kr_flag].enc;
        }

        if (priv) {
            *priv = cred->cred[sub->kr_flag].privacy;
        }

        return 0;
    }

    return -ENOENT;
}
#else /* (CONFIG_BLE_MESH_LOW_POWER || CONFIG_BLE_MESH_FRIEND) */
int friend_cred_get(struct bt_mesh_subnet *sub, uint16_t addr, uint8_t *nid,
                    const uint8_t **enc, const uint8_t **priv)
{
    BT_DBG("FrndCredGetNoEnt");

    return -ENOENT;
}
#endif /* (CONFIG_BLE_MESH_LOW_POWER || CONFIG_BLE_MESH_FRIEND) */

uint8_t bt_mesh_net_flags(struct bt_mesh_subnet *sub)
{
    uint8_t flags = 0x00;

    if (sub && sub->kr_flag) {
        flags |= BLE_MESH_NET_FLAG_KR;
    }

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS)) {
        flags |= BLE_MESH_NET_FLAG_IVU;
    }

    BT_DBG("NetFlags 0x%02x", flags);

    return flags;
}

int bt_mesh_net_secure_beacon_update(struct bt_mesh_subnet *sub)
{
    struct bt_mesh_subnet_keys *keys = NULL;
    uint8_t flags = 0;

    BT_DBG("SecureBeaconUpdate, NetIdx 0x%04x KrFlag %u IVIndex %lu",
           sub->net_idx, sub->kr_flag, bt_mesh.iv_index);

    if (sub->kr_flag) {
        keys = &sub->keys[1];
    } else {
        keys = &sub->keys[0];
    }

    flags = bt_mesh_net_flags(sub);

    return bt_mesh_secure_beacon_auth(keys->beacon, flags, keys->net_id,
                                      bt_mesh.iv_index, sub->auth);
}

int bt_mesh_net_create(uint16_t net_idx, uint8_t flags, const uint8_t key[16],
                       uint32_t iv_index)
{
    struct bt_mesh_subnet *sub = NULL;
    int err = 0;

    BT_DBG("NetCreate");
    BT_DBG("NetIdx 0x%04x Flags 0x%02x IVIndex %lu Hours %u",
           net_idx, flags, iv_index, BLE_MESH_IVU_HOURS);
    BT_DBG("NetKey %s", bt_hex(key, 16));

    (void)memset(msg_cache, 0, sizeof(msg_cache));
    msg_cache_next = 0U;

    sub = &bt_mesh.sub[0];

    sub->kr_flag = BLE_MESH_KEY_REFRESH(flags);
    if (sub->kr_flag) {
        err = bt_mesh_net_keys_create(&sub->keys[1], key);
        if (err) {
            return -EIO;
        }

        sub->kr_phase = BLE_MESH_KR_PHASE_2;
    } else {
        err = bt_mesh_net_keys_create(&sub->keys[0], key);
        if (err) {
            return -EIO;
        }
    }

    sub->net_idx = net_idx;

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        sub->node_id = BLE_MESH_NODE_IDENTITY_STOPPED;
    } else {
        sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;
#if CONFIG_BLE_MESH_PRB_SRV
        sub->private_node_id = BLE_MESH_PRIVATE_NODE_IDENTITY_NOT_SUPPORTED;
#endif /* CONFIG_BLE_MESH_PRB_SRV */
    }

    bt_mesh.iv_index = iv_index;
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS,
                              BLE_MESH_IV_UPDATE(flags));

    /* Set minimum required hours, since the 96-hour minimum requirement
     * doesn't apply straight after provisioning (since we can't know how
     * long has actually passed since the network changed its state).
     */
    bt_mesh.ivu_duration = BLE_MESH_IVU_MIN_HOURS;

    /* Make sure we have valid beacon data to be sent */
    bt_mesh_net_secure_beacon_update(sub);

#if CONFIG_BLE_MESH_DF_SRV
    return bt_mesh_directed_forwarding_sub_init(sub);
#else /* CONFIG_BLE_MESH_DF_SRV */
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_net_revoke_keys(struct bt_mesh_subnet *sub)
{
    int i;

    BT_DBG("RevokeKeys, NetIdx 0x%04x", sub->net_idx);

    memcpy(&sub->keys[0], &sub->keys[1], sizeof(sub->keys[0]));

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_subnet(sub);
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];

        BT_DBG("Revoke, NetIdx 0x%04x AppIdx 0x%04x KeyUpdated %u",
               key->net_idx, key->app_idx, key->updated);

        if (key->net_idx != sub->net_idx || !key->updated) {
            continue;
        }

        memcpy(&key->keys[0], &key->keys[1], sizeof(key->keys[0]));
        key->updated = false;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_app_key(key);
        }
    }
}

bool bt_mesh_kr_update(struct bt_mesh_subnet *sub, uint8_t new_kr, bool new_key)
{
    BT_DBG("KrUpdate, NewKr %u NewKey %u KrFlag %u KrPhase %u",
           new_kr, new_key, sub->kr_flag, sub->kr_phase);

    if (new_kr != sub->kr_flag && sub->kr_phase == BLE_MESH_KR_NORMAL) {
        BT_WARN("KR change in normal operation. Are we blacklisted?");
        return false;
    }

    sub->kr_flag = new_kr;

    if (sub->kr_flag) {
        if (sub->kr_phase == BLE_MESH_KR_PHASE_1) {
            BT_INFO("Phase 1 -> Phase 2");

            sub->kr_phase = BLE_MESH_KR_PHASE_2;

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_subnet(sub);
            }

            return true;
        }
    } else {
        switch (sub->kr_phase) {
        case BLE_MESH_KR_PHASE_1:
            if (!new_key) {
                /* Ignore */
                break;
            }
        /* Upon receiving a Secure Network beacon with the KR flag set
         * to 0 using the new NetKey in Phase 1, the node shall
         * immediately transition to Phase 3, which effectively skips
         * Phase 2.
         *
         * Intentional fall-through.
         */
        case BLE_MESH_KR_PHASE_2:
            BT_INFO("KrPhase 0x%02x -> Normal", sub->kr_phase);

#if CONFIG_BLE_MESH_PRB_SRV
            /* In this case, consider that kr_flag has changed, so
             * need to modify the content of the random field.
             */
            bt_mesh_private_beacon_update_random(sub);
#endif /* CONFIG_BLE_MESH_PRB_SRV */

            sub->kr_phase = BLE_MESH_KR_NORMAL;
            bt_mesh_net_revoke_keys(sub);

            if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) ||
                IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
                friend_cred_refresh(sub->net_idx);
            }

            return true;
        }
    }

    return false;
}

#if CONFIG_BLE_MESH_IV_UPDATE_TEST
void bt_mesh_iv_update_test(bool enable)
{
    BT_DBG("IVUpdateTest, Enable %u", enable);

    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_TEST, enable);

    /* Reset the duration variable - needed for some PTS tests */
    bt_mesh.ivu_duration = 0U;
}

bool bt_mesh_iv_update(void)
{
    BT_DBG("IVUpdate, IVIndex %lu InProgress %u", bt_mesh.iv_index,
           bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS));

    if (!bt_mesh_is_provisioned()) {
        BT_ERR("NotProvisioned");
        return false;
    }

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS)) {
        bt_mesh_net_iv_update(bt_mesh.iv_index, false);
    } else {
        bt_mesh_net_iv_update(bt_mesh.iv_index + 1, true);
    }

    bt_mesh_net_sec_update(NULL);

    return bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS);
}
#endif /* CONFIG_BLE_MESH_IV_UPDATE_TEST */

/* Used for sending immediate beacons to Friend queues and GATT clients */
void bt_mesh_net_sec_update(struct bt_mesh_subnet *sub)
{
    BT_DBG("NetSecUpdate, NetIdx 0x%04x", sub ? sub->net_idx : BLE_MESH_KEY_ANY);

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_sec_update(sub ? sub->net_idx : BLE_MESH_KEY_ANY);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
        (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED
#if CONFIG_BLE_MESH_PRB_SRV
         || bt_mesh_private_gatt_proxy_state_get() == BLE_MESH_PRIVATE_GATT_PROXY_ENABLED
#endif /* CONFIG_BLE_MESH_PRB_SRV */
        )) {
        bt_mesh_proxy_server_beacon_send(sub);
    }
}

bool bt_mesh_net_iv_update(uint32_t iv_index, bool iv_update)
{
    int i;

    BT_DBG("NetIVUpdate");
    BT_DBG("IVIndex %lu/%lu IVU %u InProgress %u",
           iv_index, bt_mesh.iv_index, iv_update,
           bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS));

    /* If a node in Normal Operation receives a Secure Network beacon or
     * a Mesh Private beacon with an IV index less than the last known
     * IV Index or greater than the last known IV Index + 42, the Secure
     * Network beacon or the Mesh Private beacon shall be ignored.
     */
    if (iv_index < bt_mesh.iv_index ||
        iv_index > bt_mesh.iv_index + 42) {
        BT_ERR("IVIndex out of sync: 0x%08x != 0x%08x",
                iv_index, bt_mesh.iv_index);
        return false;
    }

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS)) {
        /* We're currently in IV Update mode */
        if (iv_index >= bt_mesh.iv_index + 1) {
            BT_WARN("Performing IV Index Recovery");
            (void)memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));
            bt_mesh.iv_index = iv_index;
            bt_mesh.seq = 0U;
            goto do_update;
        }

        if (iv_update) {
            /* Nothing to do */
            BT_DBG("Already in IV Update in Progress state");
            return false;
        }
    } else {
        /* We're currently in Normal mode */

        if (iv_index == bt_mesh.iv_index) {
            BT_DBG("Same IV Index in normal mode");
            return false;
        }

        /* If a node in Normal Operation receives a Secure Network beacon
         * or a Mesh Private beacon with an IV index greater than the
         * last known IV Index + 1, it may initiate an IV Index Recovery
         * procedure see Section 3.10.6
         */
        if ((iv_index > bt_mesh.iv_index + 1)
#if CONFIG_BLE_MESH_IVU_RECOVERY_IVI
            || (iv_index == bt_mesh.iv_index + 1 && !iv_update)
#endif /* CONFIG_BLE_MESH_IVU_RECOVERY_IVI */
            ) {
            BT_WARN("Performing IV Index Recovery");
            (void)memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));
            bt_mesh.iv_index = iv_index;
            bt_mesh.seq = 0U;
            goto do_update;
        }

        /* If a node in Normal Operation receives a Secure Network beacon
         * or a Mesh Private beacon with an IV index equal to the last known
         * IV index+1 and the IV Update Flag set to 0, the node may update
         * its IV without going to the IV Update in Progress state, or it may
         * initiate an IV Index Recovery procedure(Section 3.10.6), or it may
         * ignore the Secure Network beacon or the Mesh Private beacon.
         */
#if !CONFIG_BLE_MESH_IVU_RECOVERY_IVI
        if (iv_index == bt_mesh.iv_index + 1 && !iv_update) {
            BT_WARN("Ignoring new index in normal mode");
            return false;
        }
#endif /* !CONFIG_BLE_MESH_IVU_RECOVERY_IVI */

        if (!iv_update) {
            /* Nothing to do */
            BT_DBG("Already in Normal state");
            return false;
        }
    }

    if (!(IS_ENABLED(CONFIG_BLE_MESH_IV_UPDATE_TEST) &&
          bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_TEST))) {
        if (bt_mesh.ivu_duration < BLE_MESH_IVU_MIN_HOURS) {
            BT_WARN("IV Update before minimum duration");
            return false;
        }
    }

    /* Defer change to Normal Operation if there are pending acks */
    if (!iv_update && bt_mesh_tx_in_progress()) {
        BT_WARN("IV Update deferred because of pending transfer");
        bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_IVU_PENDING);
        return false;
    }

do_update:
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS, iv_update);
    bt_mesh.ivu_duration = 0U;

    if (iv_update) {
        bt_mesh.iv_index = iv_index;
        BT_INFO("IV Update state entered. New index 0x%08x",
                bt_mesh.iv_index);

        bt_mesh_rpl_update();
    } else {
        BT_INFO("Normal mode entered");
        bt_mesh.seq = 0U;
    }

    k_delayed_work_submit(&bt_mesh.ivu_timer, BLE_MESH_IVU_TIMEOUT);

    size_t subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);
        if (sub && sub->net_idx != BLE_MESH_KEY_UNUSED) {
            bt_mesh_net_secure_beacon_update(sub);
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_iv(false);
    }

    return true;
}

bool bt_mesh_primary_subnet_exist(void)
{
    if (bt_mesh_subnet_get(BLE_MESH_KEY_PRIMARY)) {
        BT_DBG("PrimarySubnetExist");
        return true;
    }

    BT_DBG("PrimarySubnetNotExist");
    return false;
}

uint32_t bt_mesh_next_seq(void)
{
    uint32_t seq = bt_mesh.seq++;

    BT_DBG("NextSeq %lu", seq);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_seq();
    }

    if (!bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS) &&
        bt_mesh.seq > IV_UPDATE_SEQ_LIMIT &&
        bt_mesh_primary_subnet_exist()) {
        bt_mesh_beacon_ivu_initiator(true);
        bt_mesh_net_iv_update(bt_mesh.iv_index + 1, true);
        bt_mesh_net_sec_update(NULL);
    }

    return seq;
}

int bt_mesh_net_resend(struct bt_mesh_subnet *sub, struct net_buf *buf,
                       bool new_key, uint8_t *tx_cred, uint8_t tx_tag,
                       const struct bt_mesh_send_cb *cb, void *cb_data)
{
    const uint8_t *enc = NULL, *priv = NULL;
    uint32_t seq = 0U;
    uint16_t dst = 0U;
    int err = 0;

    /* The variable is not used when proxy server or proxy client is disabled. */
    ARG_UNUSED(dst);

    BT_DBG("NetResend");
    BT_DBG("NetIdx 0x%04x NewKey %u Len %u Tag 0x%02x",
           sub->net_idx, new_key, buf->len, tx_tag);

    /* Previously when resending the segments, only managed flooding
     * security credentials will be used.
     * Now for the segments retransmission, try to get the security
     * credentials based on the parameters "tx_cred" and "tag".
     *
     * Note:
     * Use "tag" here in case the friendship is terminated, which will
     * cause the friendship security credentials are deleted.
     *
     * TODO:
     * Potential issue here, if the subnet is deleted, exception (on
     * Provisioner) will happen.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) &&
        *tx_cred == BLE_MESH_FRIENDSHIP_CRED) {
        err = friend_cred_get(sub, BLE_MESH_ADDR_UNASSIGNED, NULL, &enc, &priv);
        if (err) {
            /* If tagged with immutable-credentials, then no
             * falling back should happen.
             */
            if (bt_mesh_tag_immutable_cred(tx_tag)) {
                BT_ERR("Resend, no friendship credentials found");
                return err;
            }

            BT_WARN("Falling back to managed flooding credentials");

            *tx_cred = BLE_MESH_FLOODING_CRED;
            enc = sub->keys[new_key].enc;
            priv = sub->keys[new_key].privacy;
        }
    }
#if CONFIG_BLE_MESH_DF_SRV
    else if (*tx_cred == BLE_MESH_DIRECTED_CRED) {
        enc = sub->keys[new_key].direct_enc;
        priv = sub->keys[new_key].direct_privacy;
    }
#endif /* CONFIG_BLE_MESH_DF_SRV */
    else {
        enc = sub->keys[new_key].enc;
        priv = sub->keys[new_key].privacy;
    }

    err = bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_TX, priv);
    if (err) {
        return err;
    }

    err = bt_mesh_net_decrypt(enc, &buf->b, BLE_MESH_NET_IVI_TX, false, false);
    if (err) {
        return err;
    }

    /* Update with a new sequence number */
    seq = bt_mesh_next_seq();
    sys_put_be24(seq, &buf->data[2]);

    /* Get destination, in case it's a proxy client */
    dst = BLE_MESH_NET_HDR_DST(buf->data);

    err = bt_mesh_net_encrypt(enc, &buf->b, BLE_MESH_NET_IVI_TX, false, false);
    if (err) {
        return err;
    }

    err = bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_TX, priv);
    if (err) {
        return err;
    }

    /* TODO:
     * Find a way to determine how the message was sent previously
     * during a retransmission, to avoid ineffective advertising.
     */
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    if (bt_mesh_proxy_server_relay(&buf->b, dst) &&
        BLE_MESH_ADDR_IS_UNICAST(dst) &&
        bt_mesh_proxy_server_find_client_by_addr(dst)) {
        send_cb_finalize(cb, cb_data);
        return 0;
    }
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    bt_mesh_proxy_client_relay(&buf->b, dst);
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

    bt_mesh_adv_send(buf, BLE_MESH_ADV(buf)->xmit, cb, cb_data);

    return 0;
}

static void bt_mesh_net_local(void)
{
    struct net_buf *buf = NULL;

    BT_DBG("NetLocal");

    while ((buf = net_buf_slist_get(&bt_mesh.local_queue))) {
        BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));
        bt_mesh_net_recv(&buf->b, 0, BLE_MESH_NET_IF_LOCAL);
        net_buf_unref(buf);
    }
}

int bt_mesh_net_encode(struct bt_mesh_net_tx *tx, struct net_buf_simple *buf,
                       bool proxy)
{
    const bool ctl = (tx->ctx->app_idx == BLE_MESH_KEY_UNUSED);
    const uint8_t *enc = NULL, *priv = NULL;
    uint8_t nid = 0U;
    int err = 0;

    BT_DBG("NetEncode");
    BT_DBG("Src 0x%04x Dst 0x%04x Cred 0x%02x CTL %u Proxy %u",
           tx->src, tx->ctx->addr, tx->ctx->send_cred, ctl, proxy);

    if (ctl && net_buf_simple_tailroom(buf) < BLE_MESH_MIC_LONG) {
        BT_ERR("Insufficient MIC space for CTL PDU");
        return -EINVAL;
    }

    if (net_buf_simple_tailroom(buf) < BLE_MESH_MIC_SHORT) {
        BT_ERR("Insufficient MIC space for PDU");
        return -EINVAL;
    }

    net_buf_simple_push_be16(buf, tx->ctx->addr);
    net_buf_simple_push_be16(buf, tx->src);

    net_buf_simple_push_be24(buf, bt_mesh_next_seq());

    if (ctl) {
        net_buf_simple_push_u8(buf, tx->ctx->send_ttl | 0x80);
    } else {
        net_buf_simple_push_u8(buf, tx->ctx->send_ttl);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) &&
        tx->ctx->send_cred == BLE_MESH_FRIENDSHIP_CRED) {
        err = friend_cred_get(tx->sub, BLE_MESH_ADDR_UNASSIGNED,
                              &nid, &enc, &priv);
        if (err) {
            /* If tagged with immutable-credentials, then no
             * falling back should happen.
             */
            if (bt_mesh_tag_immutable_cred(tx->ctx->send_tag)) {
                BT_ERR("No friendship credentials found");
                return err;
            }

            BT_WARN("Falling back to managed flooding credentials");

            tx->ctx->send_cred = BLE_MESH_FLOODING_CRED;

            nid = tx->sub->keys[tx->sub->kr_flag].nid;
            enc = tx->sub->keys[tx->sub->kr_flag].enc;
            priv = tx->sub->keys[tx->sub->kr_flag].privacy;
        }
    }
#if CONFIG_BLE_MESH_DF_SRV
    else if (tx->ctx->send_cred == BLE_MESH_DIRECTED_CRED) {
        nid = tx->sub->keys[tx->sub->kr_flag].direct_nid;
        enc = tx->sub->keys[tx->sub->kr_flag].direct_enc;
        priv = tx->sub->keys[tx->sub->kr_flag].direct_privacy;
    }
#endif /* CONFIG_BLE_MESH_DF_SRV */
    else {
        tx->ctx->send_cred = BLE_MESH_FLOODING_CRED;
        nid = tx->sub->keys[tx->sub->kr_flag].nid;
        enc = tx->sub->keys[tx->sub->kr_flag].enc;
        priv = tx->sub->keys[tx->sub->kr_flag].privacy;
    }

    net_buf_simple_push_u8(buf, (nid | (BLE_MESH_NET_IVI_TX & 1) << 7));

    err = bt_mesh_net_encrypt(enc, buf, BLE_MESH_NET_IVI_TX, proxy, false);
    if (err) {
        return err;
    }

    return bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_TX, priv);
}

#if !CONFIG_BLE_MESH_V11_SUPPORT
static void bt_mesh_net_adv_xmit_update(struct bt_mesh_net_tx *tx)
{
    /* When transmitting a Network PDU that is tagged as friendship,
     * the Advertising Bearer Network Interface shall transmit the
     * Network PDU over the advertising bearer only once.
     *
     * Note: Currently when transmitting a packet with the friendship
     * credentials used, the message flow will not reach here. It
     * will be enqueued into the friend queue in transport.c, and
     * waited to be sent.
     */
    if (bt_mesh_tag_friendship(tx->ctx->send_tag)) {
        tx->xmit = BLE_MESH_TRANSMIT(0, BLE_MESH_TRANSMIT_INT(bt_mesh_net_transmit_get()));
    } else if (bt_mesh_tag_relay(tx->ctx->send_tag)) {
        tx->xmit = bt_mesh_relay_retransmit_get();
    } else {
        tx->xmit = bt_mesh_net_transmit_get();
    }

    BT_INFO("NetAdvXmitUpdate, Tag 0x%02x Xmit 0x%02x", tx->ctx->send_tag, tx->xmit);
}
#endif /* !CONFIG_BLE_MESH_V11_SUPPORT */

int bt_mesh_net_send(struct bt_mesh_net_tx *tx, struct net_buf *buf,
                     const struct bt_mesh_send_cb *cb, void *cb_data)
{
    const struct bt_mesh_send_cb *send_cb = cb;
    uint8_t bearer = BLE_MESH_ALL_BEARERS;
    int err = 0;

    BT_DBG("NetSend");
    BT_DBG("Src 0x%04x Dst 0x%04x TTL %u Cred %u Tag 0x%02x Seq 0x%06x Room %u/%u",
           tx->src, tx->ctx->addr, tx->ctx->send_ttl, tx->ctx->send_cred,
           tx->ctx->send_tag, bt_mesh.seq, net_buf_headroom(buf), net_buf_tailroom(buf));
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    if (tx->ctx->send_ttl == BLE_MESH_TTL_DEFAULT) {
        tx->ctx->send_ttl = bt_mesh_default_ttl_get();
    }

#if 0
    /* The output filter of the interface connected to advertising
     * or GATT bearers shall drop all messages with the TTL value
     * set to 1 unless they contain a network PDU that is tagged
     * as relay.
     */
    if (tx->ctx->send_ttl == 1U &&
        !bt_mesh_tag_relay(tx->ctx->send_tag)) {
        BT_WARN("Ignore PDU with TTL=1 but not tagged as relay");
        err = -EIO;
        goto done;
    }
#endif

    /* Spec:
     * If the message security material is not set by the network
     * layer or any higher layer, the message shall be secured
     * using the managed flooding security credentials.
     *
     * Note:
     * Currently no need to add any code for this here, because if
     * the security material is not set, the "tx->ctx->send_cred"
     * will be initialized to 0x00, which will be used as managed
     * flooding security credentials.
     */

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_update_net_send_cred(tx, &bearer);

    BT_DBG("TxBearer 0x%02x", bearer);
#endif /* CONFIG_BLE_MESH_DF_SRV */

    err = bt_mesh_net_encode(tx, &buf->b, false);
    if (err) {
        goto done;
    }

#if CONFIG_BLE_MESH_SELF_TEST
    if (net_pdu_test_cb) {
        net_pdu_test_cb(buf->data, buf->len);
        goto done;
    }
#endif /* CONFIG_BLE_MESH_SELF_TEST */

    /* Deliver to GATT Proxy Clients if necessary. Mesh spec 3.4.5.2:
     * "The output filter of the interface connected to advertising or
     * GATT bearers shall drop all messages with the TTL value set to
     * 1 unless they contain a network PDU that is tagged as relay."
     *
     * "The output filter of the interface connected to the GATT bearer
     * shall drop all messages secured using the friendship security
     * credentials."
     */
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    if ((bearer & BLE_MESH_GATT_BEARER) &&
        (tx->ctx->send_ttl != 1U || bt_mesh_tag_relay(tx->ctx->send_tag)) &&
        tx->ctx->send_cred != BLE_MESH_FRIENDSHIP_CRED) {
        if (bt_mesh_proxy_server_relay(&buf->b, tx->ctx->addr) &&
            BLE_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
            /* When the destination address is identified as a proxy client
             * address, the message will be sent only to the proxy client.
             * This action will enhance the efficiency of the proxy server
             * in sending data packets.
             *
             * It should be noted that this approach does not significantly
             * reduce the number of advertising packets in the air, as other
             * proxy clients may receive the message and resend it through
             * a advertising method.
             */
            if (bt_mesh_proxy_server_find_client_by_addr(tx->ctx->addr)) {
                BT_DBG("ProxyClientFound");

                send_cb_finalize(send_cb, cb_data);
                send_cb = NULL;
                goto done;
            }

            /* Finalize transmission if this only went through GATT bearer */
            if ((bearer & (~BLE_MESH_GATT_BEARER)) == 0) {
                send_cb_finalize(send_cb, cb_data);
                send_cb = NULL;

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
                /* This message will not be transmitted by proxy client */
                if (!bt_mesh_proxy_client_get_conn_count()) {
                    BT_DBG("ProxyClientNoConn");
                    goto done;
                }
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
            }
        }
    }
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    if ((bearer & BLE_MESH_GATT_BEARER) &&
        (tx->ctx->send_ttl != 1U || bt_mesh_tag_relay(tx->ctx->send_tag)) &&
        tx->ctx->send_cred != BLE_MESH_FRIENDSHIP_CRED) {
        if (bt_mesh_proxy_client_relay(&buf->b, tx->ctx->addr)) {
            BT_DBG("ProxyClientRelay");

            /* If Proxy Client succeeds to send messages with GATT bearer,
             * we can directly finish here. And if not, which means no
             * connection has been created with Proxy Client, here we will
             * use advertising bearer for the messages.
             */
            if ((bearer & (~BLE_MESH_GATT_BEARER)) == 0) {
                send_cb_finalize(send_cb, cb_data);
                send_cb = NULL;
                goto done;
            }
        }
    }
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

    /* Deliver to local network interface if necessary */
    if (((IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) ||
         (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en())) &&
        (bt_mesh_fixed_group_match(tx->ctx->addr) || bt_mesh_elem_find(tx->ctx->addr))) {
        /* If the target address isn't a unicast address, then the callback
         * function will be called by mesh adv task instead of called here
         * to avoid the callback function being called twice.
         * See BLEMESH24-76 for more details.
         */
        if (BLE_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
            if (send_cb && send_cb->start) {
                send_cb->start(0, 0, cb_data);
            }

            net_buf_slist_put(&bt_mesh.local_queue, net_buf_ref(buf));

            if (send_cb && send_cb->end) {
                send_cb->end(0, cb_data);
            }

            bt_mesh_net_local();
            goto done;
        }

        net_buf_slist_put(&bt_mesh.local_queue, net_buf_ref(buf));

        bt_mesh_net_local();
    }

    if ((bearer & BLE_MESH_ADV_BEARER) &&
        (tx->ctx->send_ttl != 1U || bt_mesh_tag_relay(tx->ctx->send_tag))) {
        /* Deliver to the advertising network interface. Mesh spec
         * 3.4.5.2: "The output filter of the interface connected to
         * advertising or GATT bearers shall drop all messages with
         * TTL value set to 1."
         * Spec v1.1 adds additional condition:
         * "unless they contain a network PDU that is tagged as relay"
         *
         * "tx->xmit" has already been initialized in the access.c
         * (i.e. bt_mesh_model_send() & bt_mesh_model_publish()).
         * Here xmit may needs to be updated according Mesh spec 3.4.6.
         */
        bt_mesh_net_adv_xmit_update(tx);

        bt_mesh_adv_send(buf, tx->xmit, cb, cb_data);
        goto done;
    }

    BT_WARN("NetNotSend");

    err = -EIO;

done:
    net_buf_unref(buf);
    return err;
}

static bool auth_match(struct bt_mesh_subnet_keys *keys,
                       const uint8_t net_id[8], uint8_t flags,
                       uint32_t iv_index, const uint8_t auth[8])
{
    uint8_t net_auth[8] = {0};

    BT_DBG("AuthMatch");

    if (memcmp(net_id, keys->net_id, 8)) {
        BT_DBG("NetID %s != %s", bt_hex(net_id, 8), bt_hex(keys->net_id, 8));
        return false;
    }

    bt_mesh_secure_beacon_auth(keys->beacon, flags, keys->net_id, iv_index,
                               net_auth);

    if (memcmp(auth, net_auth, 8)) {
        BT_WARN("NetAuth %s != %s", bt_hex(auth, 8), bt_hex(net_auth, 8));
        return false;
    }

    return true;
}

struct bt_mesh_subnet *bt_mesh_subnet_find_with_snb(const uint8_t net_id[8], uint8_t flags,
                                                    uint32_t iv_index, const uint8_t auth[8],
                                                    bool *new_key)
{
    size_t subnet_size = 0U;
    int i;

    subnet_size = bt_mesh_rx_netkey_size();

    BT_DBG("SubnetFindWithSnb, Size %lu", subnet_size);

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);

        if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

        if (auth_match(&sub->keys[0], net_id, flags, iv_index, auth)) {
            *new_key = false;
            return sub;
        }

        if (sub->kr_phase == BLE_MESH_KR_NORMAL) {
            continue;
        }

        if (auth_match(&sub->keys[1], net_id, flags, iv_index, auth)) {
            *new_key = true;
            return sub;
        }
    }

    return NULL;
}

int net_decrypt(struct bt_mesh_subnet *sub, const uint8_t *enc,
                const uint8_t *priv, const uint8_t *data,
                size_t data_len, struct bt_mesh_net_rx *rx,
                struct net_buf_simple *buf)
{
    BT_DBG("NetDecrypt");
    BT_DBG("IVI %u NID 0x%02x NetIdx 0x%04x IVIndex %lu NetIf %u",
           BLE_MESH_NET_HDR_IVI(data), BLE_MESH_NET_HDR_NID(data),
           sub->net_idx, bt_mesh.iv_index, rx->net_if);

    rx->old_iv = (BLE_MESH_NET_HDR_IVI(data) != (bt_mesh.iv_index & 0x01));

    net_buf_simple_reset(buf);
    memcpy(net_buf_simple_add(buf, data_len), data, data_len);

    if (bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_RX(rx), priv)) {
        return -ENOENT;
    }

    rx->ctx.addr = BLE_MESH_NET_HDR_SRC(buf->data);
    if (!BLE_MESH_ADDR_IS_UNICAST(rx->ctx.addr)) {
        BT_DBG("Ignoring non-unicast src addr 0x%04x", rx->ctx.addr);
        return -EINVAL;
    }

    if (rx->net_if == BLE_MESH_NET_IF_ADV && msg_cache_match(rx, buf)) {
        BT_DBG("Duplicate found in Network Message Cache");
        return -EALREADY;
    }

    BT_DBG("Src 0x%04x", rx->ctx.addr);

    if (IS_ENABLED(CONFIG_BLE_MESH_PROXY) &&
        rx->net_if == BLE_MESH_NET_IF_PROXY_CFG) {
        return bt_mesh_net_decrypt(enc, buf, BLE_MESH_NET_IVI_RX(rx), true, false);
    }

    return bt_mesh_net_decrypt(enc, buf, BLE_MESH_NET_IVI_RX(rx), false, false);
}

#if (CONFIG_BLE_MESH_LOW_POWER || CONFIG_BLE_MESH_FRIEND)
static int friend_decrypt(struct bt_mesh_subnet *sub, const uint8_t *data,
                          size_t data_len, struct bt_mesh_net_rx *rx,
                          struct net_buf_simple *buf)
{
    int i;

    BT_DBG("FrndDecrypt");
    BT_DBG("NID 0x%02x NetIdx 0x%04x KrPhase %u",
           BLE_MESH_NET_HDR_NID(data), sub->net_idx, sub->kr_phase);

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

        if (cred->net_idx != sub->net_idx) {
            continue;
        }

        if (BLE_MESH_NET_HDR_NID(data) == cred->cred[0].nid &&
            !net_decrypt(sub, cred->cred[0].enc, cred->cred[0].privacy,
                         data, data_len, rx, buf)) {
            BT_DBG("UseOldKey");
            return 0;
        }

        if (sub->kr_phase == BLE_MESH_KR_NORMAL) {
            continue;
        }

        if (BLE_MESH_NET_HDR_NID(data) == cred->cred[1].nid &&
            !net_decrypt(sub, cred->cred[1].enc, cred->cred[1].privacy,
                         data, data_len, rx, buf)) {
            BT_DBG("UseNewKey");
            rx->new_key = 1U;
            return 0;
        }
    }

    return -ENOENT;
}
#endif /* (CONFIG_BLE_MESH_LOW_POWER || CONFIG_BLE_MESH_FRIEND) */

static int flooding_decrypt(struct bt_mesh_subnet *sub, const uint8_t *data,
                            size_t data_len, struct bt_mesh_net_rx *rx,
                            struct net_buf_simple *buf)
{
    BT_DBG("FloodingDecrypt");
    BT_DBG("NID 0x%02x NetIdx 0x%04x KrPhase %u",
           BLE_MESH_NET_HDR_NID(data), sub->net_idx, sub->kr_phase);

    if (BLE_MESH_NET_HDR_NID(data) == sub->keys[0].nid &&
        !net_decrypt(sub, sub->keys[0].enc, sub->keys[0].privacy,
                     data, data_len, rx, buf)) {
        BT_DBG("UseOldKey");
        return 0;
    }

    if (sub->kr_phase == BLE_MESH_KR_NORMAL) {
        return -ENOENT;
    }

    if (BLE_MESH_NET_HDR_NID(data) == sub->keys[1].nid &&
        !net_decrypt(sub, sub->keys[1].enc, sub->keys[1].privacy,
                     data, data_len, rx, buf)) {
        BT_DBG("UseNewKey");
        rx->new_key = 1U;
        return 0;
    }

    return -ENOENT;
}

static bool net_find_and_decrypt(const uint8_t *data, size_t data_len,
                                 struct bt_mesh_net_rx *rx,
                                 struct net_buf_simple *buf)
{
    struct bt_mesh_subnet *sub = NULL;
    size_t array_size = 0U;
    int i;

    array_size = bt_mesh_rx_netkey_size();

    BT_DBG("NetFindAndDecrypt, Size %u", array_size);

    for (i = 0; i < array_size; i++) {
        sub = bt_mesh_rx_netkey_get(i);
        if (!sub) {
            BT_DBG("SubNotFound");
            continue;
        }

        if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

#if CONFIG_BLE_MESH_BRC_SRV
        sub->sbr_net_idx = BLE_MESH_KEY_UNUSED;
#endif /* CONFIG_BLE_MESH_BRC_SRV */

#if (CONFIG_BLE_MESH_LOW_POWER || CONFIG_BLE_MESH_FRIEND)
        if (!friend_decrypt(sub, data, data_len, rx, buf)) {
            rx->ctx.recv_cred = BLE_MESH_FRIENDSHIP_CRED;
            rx->ctx.net_idx = sub->net_idx;
            rx->sub = sub;
            return true;
        }
#endif /* (CONFIG_BLE_MESH_LOW_POWER || CONFIG_BLE_MESH_FRIEND) */

#if CONFIG_BLE_MESH_DF_SRV
        if (!bt_mesh_directed_decrypt(sub, data, data_len, rx, buf)) {
            rx->ctx.recv_cred = BLE_MESH_DIRECTED_CRED;
            rx->ctx.net_idx = sub->net_idx;
            rx->sub = sub;
            return true;
        }
#endif /* CONFIG_BLE_MESH_DF_SRV */

        if (!flooding_decrypt(sub, data, data_len, rx, buf)) {
            rx->ctx.recv_cred = BLE_MESH_FLOODING_CRED;
            rx->ctx.net_idx = sub->net_idx;
            rx->sub = sub;
            return true;
        }
    }

    return false;
}

/* Relaying from advertising to the advertising bearer should only happen
 * if the Relay state is set to enabled. Locally originated packets always
 * get sent to the advertising bearer. If the packet came in through GATT,
 * then we should only relay it if the GATT Proxy state is enabled.
 */
static bool relay_to_adv(enum bt_mesh_net_if net_if)
{
    BT_DBG("RelayToAdv, NetIf %u", net_if);

    switch (net_if) {
    case BLE_MESH_NET_IF_LOCAL:
        return true;
    case BLE_MESH_NET_IF_ADV:
        return (bt_mesh_relay_get() == BLE_MESH_RELAY_ENABLED);
    case BLE_MESH_NET_IF_PROXY:
        return (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED
#if CONFIG_BLE_MESH_PRB_SRV
                || bt_mesh_private_gatt_proxy_state_get() == BLE_MESH_PRIVATE_GATT_PROXY_ENABLED
#endif /* CONFIG_BLE_MESH_PRB_SRV */
                );
    default:
        return false;
    }
}

#if !CONFIG_BLE_MESH_V11_SUPPORT
static uint8_t net_retransmission_adv(struct bt_mesh_net_rx *rx,
                                      uint8_t *cred, uint8_t *tag)
{
    BT_DBG("NetRetransmissionAdv");

    if (rx->ctx.recv_cred == BLE_MESH_FLOODING_CRED) {
        uint8_t bearer = BLE_MESH_NONE_BEARER;

        /* Inbound bearer: ADV;
         * Inbound Security Material: managed flooding;
         */

        /* Condition: Relay is enabled. */
        if (IS_ENABLED(CONFIG_BLE_MESH_RELAY) &&
            bt_mesh_relay_get() == BLE_MESH_RELAY_ENABLED) {
            bearer |= BLE_MESH_ADV_BEARER;
            *cred = BLE_MESH_FLOODING_CRED;
            /* Additional action: Tag as relay */
            *tag |= BLE_MESH_TAG_RELAY;
        }

        /* Condition: Proxy is enabled. */
        if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
            bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED) {
            bearer |= BLE_MESH_GATT_BEARER;
            *cred = BLE_MESH_FLOODING_CRED;
        }

        BT_DBG("FloodingBearer 0x%02x", bearer);

        return bearer;
    }

    if (rx->ctx.recv_cred == BLE_MESH_FRIENDSHIP_CRED &&
        IS_ENABLED(CONFIG_BLE_MESH_FRIEND) &&
        bt_mesh_friend_get() == BLE_MESH_FRIEND_ENABLED) {

        /* Condition: Directed friend is disabled. */
        *cred = BLE_MESH_FLOODING_CRED;

        BT_DBG("FrndBearerAll");

        return BLE_MESH_ALL_BEARERS;
    }

    return BLE_MESH_NONE_BEARER;
}

static uint8_t net_retransmission_gatt(struct bt_mesh_net_rx *rx,
                                       uint8_t *cred, uint8_t *tag)
{
    BT_DBG("NetRetransmissionGatt");

    if (rx->ctx.recv_cred == BLE_MESH_FLOODING_CRED) {
        /* Inbound bearer: GATT;
         * Inbound Security Material: managed flooding;
         * Condition: Proxy is enabled;
         */
        if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
            bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED) {
            /* Condition: Directed proxy is disabled. */
            *cred = BLE_MESH_FLOODING_CRED;

            BT_DBG("FloodingBearerAll");

            return BLE_MESH_ALL_BEARERS;
        }

        return BLE_MESH_NONE_BEARER;
    }

    return BLE_MESH_NONE_BEARER;
}

static uint8_t bt_mesh_net_retrans_match(struct bt_mesh_net_rx *rx,
                                         uint8_t *cred, uint8_t *tag)
{
    if (rx->net_if == BLE_MESH_NET_IF_ADV) {
        return net_retransmission_adv(rx, cred, tag);
    }

    if (rx->net_if == BLE_MESH_NET_IF_PROXY) {
        return net_retransmission_gatt(rx, cred, tag);
    }

    return BLE_MESH_NONE_BEARER;
}
#endif /* !CONFIG_BLE_MESH_V11_SUPPORT */

static void bt_mesh_net_relay(struct net_buf_simple *sbuf,
                              struct bt_mesh_net_rx *rx)
{
    const uint8_t *enc = NULL, *priv = NULL;
    bool netkey_changed = false;
    struct net_buf *buf = NULL;
    uint8_t bearer = 0;
    uint8_t xmit = 0U;
    uint8_t cred = 0;
    uint8_t nid = 0U;
    uint8_t tag = 0;

    BT_DBG("NetRelay");
    BT_DBG("TTL %u CTL %u Dst 0x%04x RecvCred %u NetIf %u",
           rx->ctx.recv_ttl, rx->ctl, rx->ctx.recv_dst,
           rx->ctx.recv_cred, rx->net_if);

    if (rx->net_if == BLE_MESH_NET_IF_LOCAL) {
        /* Locally originated PDUs with TTL=1 will only be delivered
         * to local elements as per Mesh Profile 1.0 section 3.4.5.2:
         * "The output filter of the interface connected to
         * advertising or GATT bearers shall drop all messages with
         * TTL value set to 1."
         */
        /* For local network interface, no outbound bearer will be found.
         * So currently if a PDU is received from local network interface
         * with TTL=1, it will be ignored here.
         */
        if (rx->ctx.recv_ttl == 1U) {
            return;
        }
    } else {
        /* Only PDU with TTL=0 will be ignored here. PDU with TTL=1 will
         * be checked later to see if it could be tagged with RELAY.
         */
        if (rx->ctx.recv_ttl < 1U) {
            return;
        }
    }

#if 0
    /* This could be moved, since in mesh v1.1, two more conditions could
     * cause a message to be relayed, i.e. directed forwarding and subnet
     * bridge.
     * The two conditions will be checked with retransmission requirements.
     */
    if (rx->net_if == BLE_MESH_NET_IF_ADV &&
        bt_mesh_relay_get() != BLE_MESH_RELAY_ENABLED &&
        bt_mesh_gatt_proxy_get() != BLE_MESH_GATT_PROXY_ENABLED &&
        rx->ctx.recv_cred != BLE_MESH_FRIENDSHIP_CRED) {
        return;
    }
#endif

    /* Mesh spec 3.4.6.3:
     * "The Network PDU shall be evaluated against retransmission conditions
     * for incoming Network PDUs as defined in Table 3.10. For a Network PDU,
     * there may be more than one matching row in Table 3.10. If there is no
     * row that matches the retransmission conditions, then the Network PDU
     * shall not be retransmitted."
     */
    bearer = bt_mesh_net_retrans_match(rx, &cred, &tag);
    if (bearer == BLE_MESH_NONE_BEARER) {
        BT_WARN("No outbound bearer found, inbound bearer %d", rx->net_if);
        return;
    }

#if CONFIG_BLE_MESH_BRC_SRV
    if (rx->sbr_rpl) {
        BT_ERR("Bridge RPL attack");
        goto done;
    }
#endif /* CONFIG_BLE_MESH_BRC_SRV */

    if (cred != BLE_MESH_FLOODING_CRED
#if CONFIG_BLE_MESH_DF_SRV
        && cred != BLE_MESH_DIRECTED_CRED
#endif /* CONFIG_BLE_MESH_DF_SRV */
        ) {
        BT_WARN("No outbound security cred found, inbound cred %d", rx->ctx.recv_cred);
        return;
    }

    if (rx->ctx.recv_ttl == 0x01) {
        BT_DBG("Ignore PDU with TTL = 1");
        return;
    }

    if (rx->ctx.recv_ttl == 0x02 && bt_mesh_tag_relay(tag) == false) {
        BT_DBG("Ignore PDU with TTL = 2 but not tagged as relay");
        return;
    }

    /* The Relay Retransmit state is only applied to adv-adv relaying.
     * Anything else (like GATT to adv, or locally originated packets)
     * use the Network Transmit state.
     */
    if (rx->net_if == BLE_MESH_NET_IF_ADV &&
        rx->ctx.recv_cred != BLE_MESH_FRIENDSHIP_CRED) {
        xmit = bt_mesh_relay_retransmit_get();
    } else {
        xmit = bt_mesh_net_transmit_get();
    }

#if 0
    /* TODO: Needs further thinking. */
    xmit = bt_mesh_net_adv_xmit_update(cred, tag, rx->ctl);
    if (xmit == 0) {
        return;
    }
#endif

    /* TODO:
     * When the node tries to relay a Segment ACK message, in this case
     * the corresponding segment packets (if exist) can be removed from
     * the relay queue.
     */

#if !CONFIG_BLE_MESH_RELAY_ADV_BUF
#if CONFIG_BLE_MESH_EXT_ADV
    if (rx->ctx.enh.ext_adv_cfg_used) {
#if CONFIG_BLE_MESH_LONG_PACKET
        if (rx->ctx.enh.long_pkt_cfg) {
            buf = bt_mesh_adv_create(BLE_MESH_ADV_EXT_LONG_RELAY_DATA, K_NO_WAIT);
        } else
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
        {
            buf = bt_mesh_adv_create(BLE_MESH_ADV_EXT_DATA, K_NO_WAIT);
        }
        if (buf) {
            EXT_ADV(buf)->primary_phy = rx->ctx.enh.ext_adv_cfg.primary_phy;
            EXT_ADV(buf)->secondary_phy = rx->ctx.enh.ext_adv_cfg.secondary_phy;
            EXT_ADV(buf)->include_tx_power = rx->ctx.enh.ext_adv_cfg.include_tx_power;
            EXT_ADV(buf)->tx_power = rx->ctx.enh.ext_adv_cfg.tx_power;
        }
    } else
#endif /* CONFIG_BLE_MESH_EXT_ADV */
    {
        buf = bt_mesh_adv_create(BLE_MESH_ADV_DATA, K_NO_WAIT);
    }
#else /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
    /* Check if the number of relay packets in queue is too large, if so
     * use minimum relay retransmit value for later relay packets.
     */
    if (bt_mesh_get_stored_relay_count() >= MAX_STORED_RELAY_COUNT) {
        xmit = BLE_MESH_TRANSMIT(0, 20);
    }
#if CONFIG_BLE_MESH_EXT_ADV
    if (rx->ctx.enh.ext_adv_cfg_used) {
#if CONFIG_BLE_MESH_LONG_PACKET
        if (rx->ctx.enh.long_pkt_cfg) {
            buf = bt_mesh_adv_create(BLE_MESH_ADV_EXT_LONG_RELAY_DATA, K_NO_WAIT);
        } else
#endif /* CONFIG_BLE_MESH_LONG_PACKET */
        {
            buf = bt_mesh_adv_create(BLE_MESH_ADV_EXT_DATA, K_NO_WAIT);
        }
        if (buf) {
            EXT_ADV(buf)->primary_phy = rx->ctx.enh.ext_adv_cfg.primary_phy;
            EXT_ADV(buf)->secondary_phy = rx->ctx.enh.ext_adv_cfg.secondary_phy;
            EXT_ADV(buf)->include_tx_power = rx->ctx.enh.ext_adv_cfg.include_tx_power;
            EXT_ADV(buf)->tx_power = rx->ctx.enh.ext_adv_cfg.tx_power;
        }
    } else
#endif /* CONFIG_BLE_MESH_EXT_ADV */
    {
        buf = bt_mesh_adv_create(BLE_MESH_ADV_RELAY_DATA, K_NO_WAIT);
    }
#endif /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */

    if (!buf) {
        BT_INFO("Out of relay buffers");
        return;
    }

    /* Only decrement TTL for non-locally originated packets */
    if (rx->net_if != BLE_MESH_NET_IF_LOCAL) {
        /* Leave CTL bit intact */
        sbuf->data[1] &= 0x80;
        sbuf->data[1] |= rx->ctx.recv_ttl - 1U;
    }

    net_buf_add_mem(buf, sbuf->data, sbuf->len);

    if (cred == BLE_MESH_FLOODING_CRED) {
        enc = rx->sub->keys[rx->sub->kr_flag].enc;
        priv = rx->sub->keys[rx->sub->kr_flag].privacy;
        nid = rx->sub->keys[rx->sub->kr_flag].nid;
#if CONFIG_BLE_MESH_DF_SRV
    } else {
        enc = rx->sub->keys[rx->sub->kr_flag].direct_enc;
        priv = rx->sub->keys[rx->sub->kr_flag].direct_privacy;
        nid = rx->sub->keys[rx->sub->kr_flag].direct_nid;
#endif /* CONFIG_BLE_MESH_DF_SRV */
    }

#if CONFIG_BLE_MESH_BRC_SRV
    /* Check if subnet bridge is supported & decide which NetKey is used */
    if (bt_mesh_subnet_bridge_state_get() == BLE_MESH_SUBNET_BRIDGE_ENABLED) {
        netkey_changed = bt_mesh_bridge_change_net_key(rx, &enc, &priv, &nid, cred);

        BT_DBG("NetKeyChanged %u", netkey_changed);
    }
#endif /* CONFIG_BLE_MESH_BRC_SRV */

    BT_DBG("Relaying, NewTTL %u", BLE_MESH_NET_HDR_TTL(buf->data));

    /* 1. Update NID if RX or RX was with friend credentials(included by case 3).
     * 2. Update NID if the net_key has changed.
     * 3. Update NID if credential has changed.
     */
    if (netkey_changed || cred != rx->ctx.recv_cred) {
        buf->data[0] &= 0x80; /* Clear everything except IVI */
        buf->data[0] |= nid;
    }

    /* We re-encrypt and obfuscate using the received IVI rather than
     * the normal TX IVI (which may be different) since the transport
     * layer nonce includes the IVI.
     */
    if (bt_mesh_net_encrypt(enc, &buf->b, BLE_MESH_NET_IVI_RX(rx), false, false)) {
        goto done;
    }

    if (bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_RX(rx), priv)) {
        goto done;
    }

    /* Sending to the GATT bearer should only happen if GATT Proxy
     * is enabled or the message originates from the local node.
     *
     * The output filter of the interface connected to the GATT bearer
     * shall drop all messages secured using the friendship security
     * credentials.
     */
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    if ((bearer & BLE_MESH_GATT_BEARER) &&
        ((bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED &&
          cred != BLE_MESH_FRIENDSHIP_CRED) ||
#if CONFIG_BLE_MESH_PRB_SRV
         bt_mesh_private_gatt_proxy_state_get() == BLE_MESH_PRIVATE_GATT_PROXY_ENABLED ||
#endif /* CONFIG_BLE_MESH_PRB_SRV */
         rx->net_if == BLE_MESH_NET_IF_LOCAL ||
         rx->ctx.recv_cred == BLE_MESH_FRIENDSHIP_CRED)) {
        if (bt_mesh_proxy_server_relay(&buf->b, rx->ctx.recv_dst) &&
            BLE_MESH_ADDR_IS_UNICAST(rx->ctx.recv_dst)) {
            /* Not relay if only GATT bearer is chosen or found Proxy Client */
            if (((bearer & (~BLE_MESH_GATT_BEARER)) == 0) ||
                bt_mesh_proxy_server_find_client_by_addr(rx->ctx.recv_dst)) {
                BT_DBG("ProxyNotRelay");
                goto done;
            }
        }
    }
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

    if (((bearer & BLE_MESH_ADV_BEARER) && relay_to_adv(rx->net_if)) ||
        netkey_changed ||
        rx->ctx.recv_cred == BLE_MESH_FRIENDSHIP_CRED) {
#if !CONFIG_BLE_MESH_RELAY_ADV_BUF
        bt_mesh_adv_send(buf, xmit, NULL, NULL);
#else /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
        bt_mesh_relay_adv_send(buf, xmit, rx->ctx.addr, rx->ctx.recv_dst, NULL, NULL);
#endif /* !CONFIG_BLE_MESH_RELAY_ADV_BUF */
    }

done:
    net_buf_unref(buf);
}

void bt_mesh_net_header_parse(struct net_buf_simple *buf,
                              struct bt_mesh_net_rx *rx)
{
    BT_DBG("NetHeaderParse");

    rx->old_iv = (BLE_MESH_NET_HDR_IVI(buf->data) != (bt_mesh.iv_index & 0x01));
    rx->ctl = BLE_MESH_NET_HDR_CTL(buf->data);
    rx->ctx.recv_ttl = BLE_MESH_NET_HDR_TTL(buf->data);
    rx->seq = BLE_MESH_NET_HDR_SEQ(buf->data);
    rx->ctx.addr = BLE_MESH_NET_HDR_SRC(buf->data);
    rx->ctx.recv_dst = BLE_MESH_NET_HDR_DST(buf->data);
}

int bt_mesh_net_decode(struct net_buf_simple *data, enum bt_mesh_net_if net_if,
                       struct bt_mesh_net_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("NetDecode, NetIf %u", net_if);

    if (data->len < BLE_MESH_NET_MIN_PDU_LEN) {
        BT_WARN("Dropping too short mesh packet (len %u)", data->len);
        return -EINVAL;
    }

    if (net_if == BLE_MESH_NET_IF_ADV && check_dup(data)) {
        return -EINVAL;
    }

    BT_DBG("Len %u: %s", data->len, bt_hex(data->data, data->len));

    rx->net_if = net_if;

    if (!net_find_and_decrypt(data->data, data->len, rx, buf)) {
        BT_DBG("Unable to find matching net for packet");
        return -ENOENT;
    }

    /* Spec v1.1, Section 3.4.5.1:
     * The input filter of the interface connected to the GATT
     * bearer shall drop all messages that have been secured
     * using the friendship security credentials.
     */
    if (net_if == BLE_MESH_NET_IF_PROXY &&
        rx->ctx.recv_cred == BLE_MESH_FRIENDSHIP_CRED) {
        BT_WARN("Dropping packet from GATT bearer using friendship creds");
        return -EBADMSG;
    }

    /* Initialize AppIdx to a sane value */
    rx->ctx.app_idx = BLE_MESH_KEY_UNUSED;

    rx->ctx.recv_ttl = BLE_MESH_NET_HDR_TTL(buf->data);

    /* Default to responding with TTL 0 for non-routed messages */
    if (rx->ctx.recv_ttl == 0U) {
        rx->ctx.send_ttl = 0U;
    } else {
        rx->ctx.send_ttl = BLE_MESH_TTL_DEFAULT;
    }

    rx->ctl = BLE_MESH_NET_HDR_CTL(buf->data);
    rx->seq = BLE_MESH_NET_HDR_SEQ(buf->data);
    rx->ctx.recv_dst = BLE_MESH_NET_HDR_DST(buf->data);

    if (net_if != BLE_MESH_NET_IF_PROXY_CFG &&
        rx->ctx.recv_dst == BLE_MESH_ADDR_UNASSIGNED) {
        BT_ERR("Destination address is unassigned; dropping packet");
        return -EBADMSG;
    }

    /* For case MESH/NODE/RLY/BV-01-C, even the DST is RFU, it needs to be forwarded. */
#if !CONFIG_BLE_MESH_BQB_TEST
    if (BLE_MESH_ADDR_IS_RFU(rx->ctx.recv_dst)) {
        BT_ERR("Destination address is RFU; dropping packet 0x%02x", rx->ctx.recv_dst);
        return -EBADMSG;
    }
#endif /* !CONFIG_BLE_MESH_BQB_TEST */

    if (net_if != BLE_MESH_NET_IF_LOCAL && bt_mesh_elem_find(rx->ctx.addr)) {
        BT_DBG("Dropping locally originated packet");
        return -EBADMSG;
    }

    BT_DBG("Src 0x%04x Dst 0x%04x TTL %u", rx->ctx.addr, rx->ctx.recv_dst, rx->ctx.recv_ttl);
    BT_DBG("Len %u: %s", buf->len, bt_hex(buf->data, buf->len));

    msg_cache_add(rx);

    return 0;
}

static bool ready_to_recv(void)
{
    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) {
        BT_DBG("NodeReadyToRecv");
        return true;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
        if (bt_mesh_provisioner_get_node_count()) {
            BT_DBG("PvnrReadyToRecv");
            return true;
        }
    }

    BT_DBG("NotReadyToRecv");
    return false;
}

static bool ignore_net_msg(uint16_t src, uint16_t dst)
{
    BT_DBG("IgnoreNetMsg, Src 0x%04x Dst 0x%04x", src, dst);

    if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV)) {
        /* When fast provisioning is enabled, the node addr
         * message will be sent to the Primary Provisioner,
         * which shall not be ignored here.
         */
        BT_DBG("FastProvNotIgnoreNetMsg");
        return false;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) &&
        bt_mesh_is_provisioner_en()) {
        /* If the destination address of the message is the element
         * address of Provisioner, but Provisioner fails to find the
         * node in its provisioning database, then this message will
         * be ignored.
         * Note: extra check src here to support Provisioner sending
         * messages to itself.
         */
        if (!bt_mesh_provisioner_get_node_with_addr(src) &&
            !bt_mesh_elem_find(src)) {
            BT_INFO("PvnrIgnoreNetMsg");
            return true;
        }
    }

    BT_DBG("NotIgnoreNetMsg");
    return false;
}

void bt_mesh_generic_net_recv(struct net_buf_simple *data,
                              struct bt_mesh_net_rx *rx,
                              enum bt_mesh_net_if net_if)
{
    struct net_buf_simple_state state = {0};
    struct net_buf_simple *buf = NULL;

    if (data->len > (BLE_MESH_GAP_ADV_MAX_LEN - 2)) {
        BT_ERR("Invalid net message length %d", data->len);
        return;
    }

    if (!ready_to_recv()) {
        return;
    }

    buf = bt_mesh_alloc_buf(data->len);
    if (!buf) {
        BT_ERR("Alloc net msg buffer failed");
        return;
    }

    BT_DBG("rssi %d net_if %u", rx->ctx.recv_rssi, net_if);

    if (bt_mesh_net_decode(data, net_if, rx, buf)) {
        BT_DBG("DecodeFailed");
        goto free_net_msg_buf;
    }

    BT_DBG("NetRecv, Src 0x%04x Dst 0x%04x Rssi %d NetIf %u",
        rx->ctx.addr, rx->ctx.recv_dst, rx->ctx.recv_rssi, net_if);

    if (ignore_net_msg(rx->ctx.addr, rx->ctx.recv_dst)) {
        BT_DBG("IgnoreNetMsg");
        goto free_net_msg_buf;
    }

    /* Save the state so the buffer can later be relayed */
    net_buf_simple_save(buf, &state);

    BT_DBG("NetRecv, Src 0x%04x Dst 0x%04x Rssi %d NetIf %u",
           rx->ctx.addr, rx->ctx.recv_dst, rx->ctx.recv_rssi, net_if);

    BT_BQB(BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_NODE | \
           BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_NET,
           "\nNetRecv: ctl: %d, src: %d, dst: %d, ttl: %d, data: 0x%s",
           rx->ctl, rx->ctx.addr, rx->ctx.recv_dst, rx->ctx.recv_ttl,
           bt_hex(buf->data + BLE_MESH_NET_HDR_LEN, buf->len - BLE_MESH_NET_HDR_LEN));

    /* If trying to handle a message with DST set to all-directed-forwarding-nodes,
     * we need to make sure the directed forwarding functionality is enabled in the
     * corresponding subnet.
     */
    rx->local_match = (bt_mesh_fixed_group_match(rx->ctx.recv_dst) ||
                       bt_mesh_fixed_direct_match(rx->sub, rx->ctx.recv_dst) ||
                       bt_mesh_elem_find(rx->ctx.recv_dst));

#if CONFIG_BLE_MESH_LONG_PACKET
    /* It should be noted that if the length of buf
     * is less than or equal to 29, it still may be the
     * last segment for a long packet, But if the bit
     * is set, it must be part of the long packet*/
    rx->ctx.enh.long_pkt_cfg_used = (buf->len >= 29);
    rx->ctx.enh.long_pkt_cfg = BLE_MESH_LONG_PACKET_FORCE;
#endif

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
#if CONFIG_BLE_MESH_PRB_SRV
        bt_mesh_private_gatt_proxy_state_get() != BLE_MESH_PRIVATE_GATT_PROXY_ENABLED &&
#endif /* CONFIG_BLE_MESH_PRB_SRV */
        net_if == BLE_MESH_NET_IF_PROXY) {
        BT_DBG("ProxyServerAddrAdd");

        bt_mesh_proxy_server_addr_add(data, rx->ctx.addr);

        if (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_DISABLED &&
            !rx->local_match) {
            BT_INFO("Proxy is disabled; ignoring message");
            goto free_net_msg_buf;
        }

        /* If the Directed Proxy Server receives a valid Network PDU from the Directed
         * Proxy Client, and the value of the Use_Directed parameter of the connection
         * for the subnet identified by the Network PDU is Enabled, and the source
         * address of the Network PDU is outside the unicast address range defined by
         * the Proxy_Client_Address_Range parameter, then the Directed Proxy Server shall
         * tag the Network PDU with the immutable-credentials tag.
         */
#if CONFIG_BLE_MESH_DF_SRV
        if (rx->sub->directed_proxy == BLE_MESH_DIRECTED_PROXY_ENABLED &&
            rx->sub->use_directed == BLE_MESH_PROXY_USE_DIRECTED_ENABLED &&
            !bt_mesh_addr_in_uar(&rx->sub->proxy_client_uar, rx->ctx.addr) &&
            !bt_mesh_proxy_server_find_client_by_addr(rx->ctx.addr)) {
            BT_DBG("ImmutableCredTag");
            rx->ctx.recv_tag |= BLE_MESH_TAG_IMMUTABLE_CRED;
        }
#endif /* CONFIG_BLE_MESH_DF_SRV */
    }

    /* The transport layer has indicated that it has rejected the message,
    * but would like to see it again if it is received in the future.
    * This can happen if a message is received when the device is in
    * Low Power mode, but the message was not encrypted with the friend
    * credentials. Remove it from the message cache so that we accept
    * it again in the future.
    */
    if (bt_mesh_trans_recv(buf, rx) == -EAGAIN) {
        BT_WARN("Removing rejected message from Network Message Cache");
        msg_cache[rx->msg_cache_idx].src = BLE_MESH_ADDR_UNASSIGNED;
        /* Rewind the next index now that we're not using this entry */
        msg_cache_next = rx->msg_cache_idx;
    }

    /* Relay if this was a group/virtual address, or if the destination
     * was neither a local element nor an LPN we're Friends for.
     */
    if (!BLE_MESH_ADDR_IS_UNICAST(rx->ctx.recv_dst) ||
        (!rx->local_match && !rx->friend_match
#if CONFIG_BLE_MESH_NOT_RELAY_REPLAY_MSG
        && !rx->replay_msg
#endif
        )) {
        net_buf_simple_restore(buf, &state);
        bt_mesh_net_relay(buf, rx);
    }

free_net_msg_buf:
    bt_mesh_free(buf);
}

static void ivu_refresh(struct k_work *work)
{
    bt_mesh.ivu_duration += BLE_MESH_IVU_HOURS;

    BT_INFO("IVURefresh, %s for %u hour%s",
            bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS) ?
            "IVU in Progress" : "IVU Normal mode",
            bt_mesh.ivu_duration, bt_mesh.ivu_duration == 1U ? "" : "s");

    if (bt_mesh.ivu_duration < BLE_MESH_IVU_MIN_HOURS) {
        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_iv(true);
        }

        k_delayed_work_submit(&bt_mesh.ivu_timer, BLE_MESH_IVU_TIMEOUT);
        return;
    }

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS)) {
        bt_mesh_beacon_ivu_initiator(true);
        bt_mesh_net_iv_update(bt_mesh.iv_index, false);
    } else if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_iv(true);
    }
}

void bt_mesh_net_start(void)
{
    BT_DBG("NetStart");

    if (bt_mesh_secure_beacon_get() == BLE_MESH_SECURE_BEACON_ENABLED) {
        bt_mesh_secure_beacon_enable();
    } else {
        bt_mesh_secure_beacon_disable();
    }

#if CONFIG_BLE_MESH_PRB_SRV
    if (bt_mesh_private_beacon_state_get() == BLE_MESH_PRIVATE_BEACON_ENABLED) {
        bt_mesh_private_beacon_enable();
    } else {
        bt_mesh_private_beacon_disable();
    }
#endif /* CONFIG_BLE_MESH_PRB_SRV */

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
        bt_mesh_gatt_proxy_get() != BLE_MESH_GATT_PROXY_NOT_SUPPORTED) {
        bt_mesh_proxy_server_gatt_enable();
        bt_mesh_adv_update();
    }

#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    /* Add Mesh beacon type (Secure Network Beacon) to the exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_BEACON,
                                    NULL);
#endif /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        /* TODO: Enable duplicate scan in Low Power Mode */
        bt_mesh_lpn_init();
    } else {
        bt_mesh_scan_enable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_init();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROV)) {
        uint16_t net_idx = bt_mesh.sub[0].net_idx;
        uint16_t addr = bt_mesh_primary_addr();
        uint32_t iv_index = bt_mesh.iv_index;
        uint8_t flags = (uint8_t)bt_mesh.sub[0].kr_flag;
        const uint8_t *net_key = bt_mesh.sub[0].keys[flags].net;

        if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS)) {
            flags |= BLE_MESH_NET_FLAG_IVU;
        }

        bt_mesh_prov_complete(net_idx, net_key, addr, flags, iv_index);
    }
}

void bt_mesh_net_init(void)
{
    BT_DBG("NetInit");

    k_delayed_work_init(&bt_mesh.ivu_timer, ivu_refresh);
}

void bt_mesh_net_reset(void)
{
    BT_DBG("NetReset");

    k_delayed_work_cancel(&bt_mesh.ivu_timer);

#if FRIEND_CRED_COUNT > 0
    memset(friend_cred, 0, sizeof(friend_cred));
#endif /* FRIEND_CRED_COUNT > 0 */

    memset(msg_cache, 0, sizeof(msg_cache));
    msg_cache_next = 0U;

    memset(dup_cache, 0, sizeof(dup_cache));
    dup_cache_next = 0U;

    bt_mesh.iv_index = 0U;
    bt_mesh.seq = 0U;
}

#if CONFIG_BLE_MESH_DEINIT
void bt_mesh_net_deinit(void)
{
    BT_DBG("NetDeinit");

    bt_mesh_net_reset();

    k_delayed_work_free(&bt_mesh.ivu_timer);

    /* Local queue uses a while loop, currently no need
     * to handle this.
     */
}
#endif /* CONFIG_BLE_MESH_DEINIT */
