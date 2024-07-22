/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "crypto.h"
#include "adv.h"
#include "scan.h"
#include "mesh.h"
#include "lpn.h"
#include "friend.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "beacon.h"
#include "settings.h"
#include "prov.h"
#include "proxy_client.h"
#include "proxy_server.h"
#include "provisioner_main.h"

/* Minimum valid Mesh Network PDU length. The Network headers
 * themselves take up 9 bytes. After that there is a minumum of 1 byte
 * payload for both CTL=1 and CTL=0 PDUs (smallest OpCode is 1 byte). CTL=1
 * PDUs must use a 64-bit (8 byte) NetMIC, whereas CTL=0 PDUs have at least
 * a 32-bit (4 byte) NetMIC and AppMIC giving again a total of 8 bytes.
 */
#define BLE_MESH_NET_MIN_PDU_LEN (BLE_MESH_NET_HDR_LEN + 1 + 8)

/* Seq limit after IV Update is triggered */
#define IV_UPDATE_SEQ_LIMIT 8000000

#define IVI(pdu)           ((pdu)[0] >> 7)
#define NID(pdu)           ((pdu)[0] & 0x7f)
#define CTL(pdu)           ((pdu)[1] >> 7)
#define TTL(pdu)           ((pdu)[1] & 0x7f)
#define SEQ(pdu)           (sys_get_be24(&(pdu)[2]))
#define SRC(pdu)           (sys_get_be16(&(pdu)[5]))
#define DST(pdu)           (sys_get_be16(&(pdu)[7]))

/* Determine how many friendship credentials we need */
#if defined(CONFIG_BLE_MESH_FRIEND)
#define FRIEND_CRED_COUNT CONFIG_BLE_MESH_FRIEND_LPN_COUNT
#elif defined(CONFIG_BLE_MESH_LOW_POWER)
#define FRIEND_CRED_COUNT CONFIG_BLE_MESH_SUBNET_COUNT
#else
#define FRIEND_CRED_COUNT 0
#endif

#if FRIEND_CRED_COUNT > 0
static struct friend_cred friend_cred[FRIEND_CRED_COUNT];
#endif

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

#if defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
#define BLE_MESH_MAX_STORED_RELAY_COUNT  (CONFIG_BLE_MESH_RELAY_ADV_BUF_COUNT / 2)
#endif

static bool check_dup(struct net_buf_simple *data)
{
    const uint8_t *tail = net_buf_simple_tail(data);
    uint32_t val = 0U;
    int i;

    val = sys_get_be32(tail - 4) ^ sys_get_be32(tail - 8);

    for (i = 0; i < ARRAY_SIZE(dup_cache); i++) {
        if (dup_cache[i] == val) {
            return true;
        }
    }

    dup_cache[dup_cache_next++] = val;
    dup_cache_next %= ARRAY_SIZE(dup_cache);

    return false;
}

static bool msg_cache_match(struct bt_mesh_net_rx *rx,
                            struct net_buf_simple *pdu)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(msg_cache); i++) {
        if (msg_cache[i].src == SRC(pdu->data) &&
            msg_cache[i].seq == (SEQ(pdu->data) & BIT_MASK(17))) {
            return true;
        }
    }

    return false;
}

static void msg_cache_add(struct bt_mesh_net_rx *rx)
{
    rx->msg_cache_idx = msg_cache_next++;
    msg_cache[rx->msg_cache_idx].src = rx->ctx.addr;
    msg_cache[rx->msg_cache_idx].seq = rx->seq;
    msg_cache_next %= ARRAY_SIZE(msg_cache);
}

#if CONFIG_BLE_MESH_PROVISIONER
void bt_mesh_msg_cache_clear(uint16_t unicast_addr, uint8_t elem_num)
{
    int i;

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
    int i;

    if (net_idx == BLE_MESH_KEY_ANY) {
        return &bt_mesh.sub[0];
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        if (bt_mesh.sub[i].net_idx == net_idx) {
            return &bt_mesh.sub[i];
        }
    }

    return NULL;
}

int bt_mesh_net_keys_create(struct bt_mesh_subnet_keys *keys,
                            const uint8_t key[16])
{
    uint8_t p[] = { 0 };
    uint8_t nid = 0U;
    int err = 0;

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

#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    err = bt_mesh_identity_key(key, keys->identity);
    if (err) {
        BT_ERR("Unable to generate IdentityKey");
        return err;
    }

    BT_DBG("IdentityKey %s", bt_hex(keys->identity, 16));
#endif /* GATT_PROXY */

    err = bt_mesh_beacon_key(key, keys->beacon);
    if (err) {
        BT_ERR("Unable to generate beacon key");
        return err;
    }

    BT_DBG("BeaconKey %s", bt_hex(keys->beacon, 16));

    return 0;
}

#if (defined(CONFIG_BLE_MESH_LOW_POWER) || \
     defined(CONFIG_BLE_MESH_FRIEND))
int friend_cred_set(struct friend_cred *cred, uint8_t idx, const uint8_t net_key[16])
{
    uint16_t lpn_addr = 0U, frnd_addr = 0U;
    uint8_t p[9] = {0};
    int err = 0;

#if defined(CONFIG_BLE_MESH_LOW_POWER)
    if (cred->addr == bt_mesh.lpn.frnd) {
        lpn_addr = bt_mesh_primary_addr();
        frnd_addr = cred->addr;
    } else {
        lpn_addr = cred->addr;
        frnd_addr = bt_mesh_primary_addr();
    }
#else
    lpn_addr = cred->addr;
    frnd_addr = bt_mesh_primary_addr();
#endif

    BT_DBG("LPNAddress 0x%04x FriendAddress 0x%04x", lpn_addr, frnd_addr);
    BT_DBG("LPNCounter 0x%04x FriendCounter 0x%04x", cred->lpn_counter,
           cred->frnd_counter);

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

    BT_DBG("Friend NID 0x%02x EncKey %s", cred->cred[idx].nid,
           bt_hex(cred->cred[idx].enc, 16));
    BT_DBG("Friend PrivacyKey %s", bt_hex(cred->cred[idx].privacy, 16));

    return 0;
}

void friend_cred_refresh(uint16_t net_idx)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

        if (cred->addr != BLE_MESH_ADDR_UNASSIGNED &&
                cred->net_idx == net_idx) {
            memcpy(&cred->cred[0], &cred->cred[1],
                   sizeof(cred->cred[0]));
        }
    }
}

int friend_cred_update(struct bt_mesh_subnet *sub)
{
    int err = 0, i;

    BT_DBG("net_idx 0x%04x", sub->net_idx);

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

        if (cred->addr == BLE_MESH_ADDR_UNASSIGNED ||
                cred->net_idx != sub->net_idx) {
            continue;
        }

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

    BT_DBG("net_idx 0x%04x addr 0x%04x", sub->net_idx, addr);

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
    cred->net_idx = BLE_MESH_KEY_UNUSED;
    cred->addr = BLE_MESH_ADDR_UNASSIGNED;
    cred->lpn_counter = 0U;
    cred->frnd_counter = 0U;
    (void)memset(cred->cred, 0, sizeof(cred->cred));
}

int friend_cred_del(uint16_t net_idx, uint16_t addr)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

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

    BT_DBG("net_idx 0x%04x addr 0x%04x", sub->net_idx, addr);

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
#else
int friend_cred_get(struct bt_mesh_subnet *sub, uint16_t addr, uint8_t *nid,
                    const uint8_t **enc, const uint8_t **priv)
{
    return -ENOENT;
}
#endif /* FRIEND || LOW_POWER */

uint8_t bt_mesh_net_flags(struct bt_mesh_subnet *sub)
{
    uint8_t flags = 0x00;

    if (sub && sub->kr_flag) {
        flags |= BLE_MESH_NET_FLAG_KR;
    }

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS)) {
        flags |= BLE_MESH_NET_FLAG_IVU;
    }

    return flags;
}

int bt_mesh_net_beacon_update(struct bt_mesh_subnet *sub)
{
    uint8_t flags = bt_mesh_net_flags(sub);
    struct bt_mesh_subnet_keys *keys = NULL;

    if (sub->kr_flag) {
        BT_DBG("NetIndex %u Using new key", sub->net_idx);
        keys = &sub->keys[1];
    } else {
        BT_DBG("NetIndex %u Using current key", sub->net_idx);
        keys = &sub->keys[0];
    }

    BT_DBG("flags 0x%02x, IVI 0x%08x", flags, bt_mesh.iv_index);

    return bt_mesh_beacon_auth(keys->beacon, flags, keys->net_id,
                               bt_mesh.iv_index, sub->auth);
}

int bt_mesh_net_create(uint16_t idx, uint8_t flags, const uint8_t key[16],
                       uint32_t iv_index)
{
    struct bt_mesh_subnet *sub = NULL;
    int err = 0;

    BT_DBG("idx %u flags 0x%02x iv_index %u", idx, flags, iv_index);

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

    sub->net_idx = idx;

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        sub->node_id = BLE_MESH_NODE_IDENTITY_STOPPED;
    } else {
        sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;
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
    bt_mesh_net_beacon_update(sub);

    return 0;
}

void bt_mesh_net_revoke_keys(struct bt_mesh_subnet *sub)
{
    int i;

    BT_DBG("idx 0x%04x", sub->net_idx);

    memcpy(&sub->keys[0], &sub->keys[1], sizeof(sub->keys[0]));

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        BT_DBG("Store updated NetKey persistently");
        bt_mesh_store_subnet(sub);
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        struct bt_mesh_app_key *key = &bt_mesh.app_keys[i];

        if (key->net_idx != sub->net_idx || !key->updated) {
            continue;
        }

        memcpy(&key->keys[0], &key->keys[1], sizeof(key->keys[0]));
        key->updated = false;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            BT_DBG("Store updated AppKey persistently");
            bt_mesh_store_app_key(key);
        }
    }
}

bool bt_mesh_kr_update(struct bt_mesh_subnet *sub, uint8_t new_kr, bool new_key)
{
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
                BT_DBG("Storing kr phase persistently");
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
            BT_INFO("KR Phase 0x%02x -> Normal", sub->kr_phase);

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

void bt_mesh_rpl_reset(void)
{
    int i;

    /* Discard "old old" IV Index entries from RPL and flag
     * any other ones (which are valid) as old.
     */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

        if (rpl->src) {
            if (rpl->old_iv) {
                (void)memset(rpl, 0, sizeof(*rpl));
            } else {
                rpl->old_iv = true;
            }

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_rpl(rpl);
            }
        }
    }
}

#if defined(CONFIG_BLE_MESH_IV_UPDATE_TEST)
void bt_mesh_iv_update_test(bool enable)
{
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_TEST, enable);
    /* Reset the duration variable - needed for some PTS tests */
    bt_mesh.ivu_duration = 0U;
}

bool bt_mesh_iv_update(void)
{
    if (!bt_mesh_is_provisioned()) {
        BT_ERR("Not yet provisioned");
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
    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_sec_update(sub ? sub->net_idx : BLE_MESH_KEY_ANY);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
            bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED) {
        bt_mesh_proxy_server_beacon_send(sub);
    }
}

bool bt_mesh_net_iv_update(uint32_t iv_index, bool iv_update)
{
    int i;

    if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS)) {
        /* We're currently in IV Update mode */

        if (iv_index != bt_mesh.iv_index) {
            BT_WARN("IV Index mismatch: 0x%08x != 0x%08x",
                    iv_index, bt_mesh.iv_index);
            return false;
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

        if (iv_index < bt_mesh.iv_index ||
                iv_index > bt_mesh.iv_index + 42) {
            BT_ERR("IV Index out of sync: 0x%08x != 0x%08x",
                   iv_index, bt_mesh.iv_index);
            return false;
        }

        if ((iv_index > bt_mesh.iv_index + 1)
#if CONFIG_BLE_MESH_IVU_RECOVERY_IVI
            || (iv_index == bt_mesh.iv_index + 1 && !iv_update)
#endif
            ) {
            BT_WARN("Performing IV Index Recovery");
            (void)memset(bt_mesh.rpl, 0, sizeof(bt_mesh.rpl));
            bt_mesh.iv_index = iv_index;
            bt_mesh.seq = 0U;
            goto do_update;
        }

#if !CONFIG_BLE_MESH_IVU_RECOVERY_IVI
        if (iv_index == bt_mesh.iv_index + 1 && !iv_update) {
            BT_WARN("Ignoring new index in normal mode");
            return false;
        }
#endif

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

        bt_mesh_rpl_reset();
    } else {
        BT_INFO("Normal mode entered");
        bt_mesh.seq = 0U;
    }

    k_delayed_work_submit(&bt_mesh.ivu_timer, BLE_MESH_IVU_TIMEOUT);

    size_t subnet_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < subnet_size; i++) {
        struct bt_mesh_subnet *sub = bt_mesh_rx_netkey_get(i);
        if (sub && sub->net_idx != BLE_MESH_KEY_UNUSED) {
            bt_mesh_net_beacon_update(sub);
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_iv(false);
    }

    return true;
}

bool bt_mesh_primary_subnet_exist(void)
{
    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) {
        if (bt_mesh_subnet_get(BLE_MESH_KEY_PRIMARY)) {
            return true;
        }
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
        if (bt_mesh_provisioner_subnet_get(BLE_MESH_KEY_PRIMARY)) {
            return true;
        }
    }

    return false;
}

uint32_t bt_mesh_next_seq(void)
{
    uint32_t seq = bt_mesh.seq++;

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
                       bool new_key, const struct bt_mesh_send_cb *cb,
                       void *cb_data)
{
    const uint8_t *enc = NULL, *priv = NULL;
    uint32_t seq = 0U;
    uint16_t dst = 0U;
    int err = 0;

    BT_DBG("net_idx 0x%04x new_key %u len %u", sub->net_idx, new_key,
           buf->len);

    enc = sub->keys[new_key].enc;
    priv = sub->keys[new_key].privacy;

    err = bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_TX, priv);
    if (err) {
        BT_ERR("De-obfuscate failed (err %d)", err);
        return err;
    }

    err = bt_mesh_net_decrypt(enc, &buf->b, BLE_MESH_NET_IVI_TX, false);
    if (err) {
        BT_ERR("Decrypt failed (err %d)", err);
        return err;
    }

    /* Update with a new sequence number */
    seq = bt_mesh_next_seq();
    sys_put_be24(seq, &buf->data[2]);

    /* Get destination, in case it's a proxy client */
    dst = DST(buf->data);

    err = bt_mesh_net_encrypt(enc, &buf->b, BLE_MESH_NET_IVI_TX, false);
    if (err) {
        BT_ERR("Encrypt failed (err %d)", err);
        return err;
    }

    err = bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_TX, priv);
    if (err) {
        BT_ERR("Obfuscate failed (err %d)", err);
        return err;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
        bt_mesh_proxy_server_relay(&buf->b, dst) &&
        BLE_MESH_ADDR_IS_UNICAST(dst)) {
        send_cb_finalize(cb, cb_data);
        return 0;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT) &&
        bt_mesh_proxy_client_relay(&buf->b, dst)) {
        send_cb_finalize(cb, cb_data);
        return 0;
    }

    bt_mesh_adv_send(buf, cb, cb_data);
    return 0;
}

static void bt_mesh_net_local(struct k_work *work)
{
    struct net_buf *buf = NULL;

    while ((buf = net_buf_slist_get(&bt_mesh.local_queue))) {
        BT_DBG("len %u: %s", buf->len, bt_hex(buf->data, buf->len));
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

    if (ctl && net_buf_simple_tailroom(buf) < BLE_MESH_MIC_LONG) {
        BT_ERR("Insufficient MIC space for CTL PDU");
        return -EINVAL;
    } else if (net_buf_simple_tailroom(buf) < BLE_MESH_MIC_SHORT) {
        BT_ERR("Insufficient MIC space for PDU");
        return -EINVAL;
    }

    BT_DBG("src 0x%04x dst 0x%04x ctl %u seq 0x%06x",
           tx->src, tx->ctx->addr, ctl, bt_mesh.seq);

    net_buf_simple_push_be16(buf, tx->ctx->addr);
    net_buf_simple_push_be16(buf, tx->src);

    net_buf_simple_push_be24(buf, bt_mesh_next_seq());

    if (ctl) {
        net_buf_simple_push_u8(buf, tx->ctx->send_ttl | 0x80);
    } else {
        net_buf_simple_push_u8(buf, tx->ctx->send_ttl);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER) && tx->friend_cred) {
        if (friend_cred_get(tx->sub, BLE_MESH_ADDR_UNASSIGNED,
                            &nid, &enc, &priv)) {
            BT_WARN("Falling back to master credentials");

            tx->friend_cred = 0U;

            nid = tx->sub->keys[tx->sub->kr_flag].nid;
            enc = tx->sub->keys[tx->sub->kr_flag].enc;
            priv = tx->sub->keys[tx->sub->kr_flag].privacy;
        }
    } else {
        tx->friend_cred = 0U;
        nid = tx->sub->keys[tx->sub->kr_flag].nid;
        enc = tx->sub->keys[tx->sub->kr_flag].enc;
        priv = tx->sub->keys[tx->sub->kr_flag].privacy;
    }

    net_buf_simple_push_u8(buf, (nid | (BLE_MESH_NET_IVI_TX & 1) << 7));

    err = bt_mesh_net_encrypt(enc, buf, BLE_MESH_NET_IVI_TX, proxy);
    if (err) {
        return err;
    }

    return bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_TX, priv);
}

int bt_mesh_net_send(struct bt_mesh_net_tx *tx, struct net_buf *buf,
                     const struct bt_mesh_send_cb *cb, void *cb_data)
{
    int err = 0;

    BT_DBG("src 0x%04x dst 0x%04x len %u headroom %u tailroom %u",
           tx->src, tx->ctx->addr, buf->len, net_buf_headroom(buf),
           net_buf_tailroom(buf));
    BT_DBG("Payload len %u: %s", buf->len, bt_hex(buf->data, buf->len));
    BT_DBG("Seq 0x%06x", bt_mesh.seq);

    if (tx->ctx->send_ttl == BLE_MESH_TTL_DEFAULT) {
        tx->ctx->send_ttl = bt_mesh_default_ttl_get();
    }

    err = bt_mesh_net_encode(tx, &buf->b, false);
    if (err) {
        goto done;
    }

    /* Deliver to GATT Proxy Clients if necessary. Mesh spec 3.4.5.2:
     * "The output filter of the interface connected to advertising or
     * GATT bearers shall drop all messages with TTL value set to 1."
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
            tx->ctx->send_ttl != 1U) {
        if (bt_mesh_proxy_server_relay(&buf->b, tx->ctx->addr) &&
                BLE_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
            /* Notify completion if this only went
             * through the Mesh Proxy.
             */
            send_cb_finalize(cb, cb_data);

            err = 0;
            goto done;
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT) &&
            tx->ctx->send_ttl != 1U) {
        if (bt_mesh_proxy_client_relay(&buf->b, tx->ctx->addr)) {
            /* If Proxy Client succeeds to send messages with GATT bearer,
             * we can directly finish here. And if not, which means no
             * connection has been created with Proxy Client, here we will
             * use advertising bearer for the messages.
             */
            send_cb_finalize(cb, cb_data);

            err = 0;
            goto done;
        }
    }

    /* Deliver to local network interface if necessary */
    if (((IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) ||
        (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en())) &&
        (bt_mesh_fixed_group_match(tx->ctx->addr) || bt_mesh_elem_find(tx->ctx->addr))) {
        /**
         * If the target address isn't a unicast address, then the callback function
         * will be called by `adv task` in place of here, to avoid the callback function
         * being called twice.
         * See BLEMESH24-76 for more details.
         */
        if (BLE_MESH_ADDR_IS_UNICAST(tx->ctx->addr)) {
            if (cb && cb->start) {
                cb->start(0, 0, cb_data);
            }

            net_buf_slist_put(&bt_mesh.local_queue, net_buf_ref(buf));

            if (cb && cb->end) {
                cb->end(0, cb_data);
            }

            k_work_submit(&bt_mesh.local_work);

            goto done;
        } else {
            net_buf_slist_put(&bt_mesh.local_queue, net_buf_ref(buf));
            k_work_submit(&bt_mesh.local_work);
        }
    }

    if (tx->ctx->send_ttl != 1U) {
        /* Deliver to the advertising network interface. Mesh spec
         * 3.4.5.2: "The output filter of the interface connected to
         * advertising or GATT bearers shall drop all messages with
         * TTL value set to 1."
         */
        bt_mesh_adv_send(buf, cb, cb_data);
    }

done:
    net_buf_unref(buf);
    return err;
}

static bool auth_match(struct bt_mesh_subnet_keys *keys,
                       const uint8_t net_id[8], uint8_t flags,
                       uint32_t iv_index, const uint8_t auth[8])
{
    uint8_t net_auth[8] = {0};

    if (memcmp(net_id, keys->net_id, 8)) {
        return false;
    }

    bt_mesh_beacon_auth(keys->beacon, flags, keys->net_id, iv_index,
                        net_auth);

    if (memcmp(auth, net_auth, 8)) {
        BT_WARN("Authentication Value %s != %s",
                bt_hex(auth, 8), bt_hex(net_auth, 8));
        return false;
    }

    return true;
}

struct bt_mesh_subnet *bt_mesh_subnet_find(const uint8_t net_id[8], uint8_t flags,
                                           uint32_t iv_index, const uint8_t auth[8],
                                           bool *new_key)
{
    size_t subnet_size = 0U;
    int i;

    subnet_size = bt_mesh_rx_netkey_size();

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

static int net_decrypt(struct bt_mesh_subnet *sub, const uint8_t *enc,
                       const uint8_t *priv, const uint8_t *data,
                       size_t data_len, struct bt_mesh_net_rx *rx,
                       struct net_buf_simple *buf)
{
    BT_DBG("NID 0x%02x net_idx 0x%04x", NID(data), sub->net_idx);
    BT_DBG("IVI %u net->iv_index 0x%08x", IVI(data), bt_mesh.iv_index);

    rx->old_iv = (IVI(data) != (bt_mesh.iv_index & 0x01));

    net_buf_simple_reset(buf);
    memcpy(net_buf_simple_add(buf, data_len), data, data_len);

    if (bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_RX(rx), priv)) {
        return -ENOENT;
    }

    rx->ctx.addr = SRC(buf->data);
    if (!BLE_MESH_ADDR_IS_UNICAST(rx->ctx.addr)) {
        BT_INFO("Ignoring non-unicast src addr 0x%04x", rx->ctx.addr);
        return -EINVAL;
    }

    if (rx->net_if == BLE_MESH_NET_IF_ADV && msg_cache_match(rx, buf)) {
        BT_DBG("Duplicate found in Network Message Cache");
        return -EALREADY;
    }

    BT_DBG("src 0x%04x", rx->ctx.addr);

    if (IS_ENABLED(CONFIG_BLE_MESH_PROXY) &&
            rx->net_if == BLE_MESH_NET_IF_PROXY_CFG) {
        return bt_mesh_net_decrypt(enc, buf, BLE_MESH_NET_IVI_RX(rx),
                                   true);
    }

    return bt_mesh_net_decrypt(enc, buf, BLE_MESH_NET_IVI_RX(rx), false);
}

#if (defined(CONFIG_BLE_MESH_LOW_POWER) || \
     defined(CONFIG_BLE_MESH_FRIEND))
static int friend_decrypt(struct bt_mesh_subnet *sub, const uint8_t *data,
                          size_t data_len, struct bt_mesh_net_rx *rx,
                          struct net_buf_simple *buf)
{
    int i;

    BT_DBG("NID 0x%02x net_idx 0x%04x", NID(data), sub->net_idx);

    for (i = 0; i < ARRAY_SIZE(friend_cred); i++) {
        struct friend_cred *cred = &friend_cred[i];

        if (cred->net_idx != sub->net_idx) {
            continue;
        }

        if (NID(data) == cred->cred[0].nid &&
                !net_decrypt(sub, cred->cred[0].enc, cred->cred[0].privacy,
                             data, data_len, rx, buf)) {
            return 0;
        }

        if (sub->kr_phase == BLE_MESH_KR_NORMAL) {
            continue;
        }

        if (NID(data) == cred->cred[1].nid &&
                !net_decrypt(sub, cred->cred[1].enc, cred->cred[1].privacy,
                             data, data_len, rx, buf)) {
            rx->new_key = 1U;
            return 0;
        }
    }

    return -ENOENT;
}
#endif

static bool net_find_and_decrypt(const uint8_t *data, size_t data_len,
                                 struct bt_mesh_net_rx *rx,
                                 struct net_buf_simple *buf)
{
    struct bt_mesh_subnet *sub = NULL;
    size_t array_size = 0U;
    int i;

    BT_DBG("%s", __func__);

    array_size = bt_mesh_rx_netkey_size();

    for (i = 0; i < array_size; i++) {
        sub = bt_mesh_rx_netkey_get(i);
        if (!sub) {
            BT_DBG("Subnet not found");
            continue;
        }

        if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
            continue;
        }

#if (defined(CONFIG_BLE_MESH_LOW_POWER) || defined(CONFIG_BLE_MESH_FRIEND))
        if (!friend_decrypt(sub, data, data_len, rx, buf)) {
            rx->friend_cred = 1;
            rx->ctx.net_idx = sub->net_idx;
            rx->sub = sub;
            return true;
        }
#endif

        if (NID(data) == sub->keys[0].nid &&
                !net_decrypt(sub, sub->keys[0].enc, sub->keys[0].privacy,
                             data, data_len, rx, buf)) {
            rx->ctx.net_idx = sub->net_idx;
            rx->sub = sub;
            return true;
        }

        if (sub->kr_phase == BLE_MESH_KR_NORMAL) {
            continue;
        }

        if (NID(data) == sub->keys[1].nid &&
                !net_decrypt(sub, sub->keys[1].enc, sub->keys[1].privacy,
                             data, data_len, rx, buf)) {
            rx->new_key = 1U;
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
    switch (net_if) {
    case BLE_MESH_NET_IF_LOCAL:
        return true;
    case BLE_MESH_NET_IF_ADV:
        return (bt_mesh_relay_get() == BLE_MESH_RELAY_ENABLED);
    case BLE_MESH_NET_IF_PROXY:
        return (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED);
    default:
        return false;
    }
}

static void bt_mesh_net_relay(struct net_buf_simple *sbuf,
                              struct bt_mesh_net_rx *rx)
{
    const uint8_t *enc = NULL, *priv = NULL;
    struct net_buf *buf = NULL;
    uint8_t nid = 0U, transmit = 0U;

    if (rx->net_if == BLE_MESH_NET_IF_LOCAL) {
        /* Locally originated PDUs with TTL=1 will only be delivered
         * to local elements as per Mesh Profile 1.0 section 3.4.5.2:
         * "The output filter of the interface connected to
         * advertising or GATT bearers shall drop all messages with
         * TTL value set to 1."
         */
        if (rx->ctx.recv_ttl == 1U) {
            return;
        }
    } else {
        if (rx->ctx.recv_ttl <= 1U) {
            return;
        }
    }

    if (rx->net_if == BLE_MESH_NET_IF_ADV &&
            !rx->friend_cred &&
            bt_mesh_relay_get() != BLE_MESH_RELAY_ENABLED &&
            bt_mesh_gatt_proxy_get() != BLE_MESH_GATT_PROXY_ENABLED) {
        return;
    }

    BT_DBG("TTL %u CTL %u dst 0x%04x", rx->ctx.recv_ttl, rx->ctl,
           rx->ctx.recv_dst);

    /* The Relay Retransmit state is only applied to adv-adv relaying.
     * Anything else (like GATT to adv, or locally originated packets)
     * use the Network Transmit state.
     */
    if (rx->net_if == BLE_MESH_NET_IF_ADV && !rx->friend_cred) {
        transmit = bt_mesh_relay_retransmit_get();
    } else {
        transmit = bt_mesh_net_transmit_get();
    }

    /**
     * When the node tries to relay a Segment ACK message, in this case
     * the corresponding segment packets (if exist) can be removed from
     * the relay queue.
     */

#if !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
    buf = bt_mesh_adv_create(BLE_MESH_ADV_DATA, transmit, K_NO_WAIT);
#else
    /**
     * Check if the number of relay packets in queue is too large, if so
     * use minimum relay retransmit value for later relay packets.
     */
    if (bt_mesh_get_stored_relay_count() >= BLE_MESH_MAX_STORED_RELAY_COUNT) {
        transmit = BLE_MESH_TRANSMIT(0, 20);
    }
    buf = bt_mesh_relay_adv_create(BLE_MESH_ADV_DATA, transmit, K_NO_WAIT);
#endif

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

    enc = rx->sub->keys[rx->sub->kr_flag].enc;
    priv = rx->sub->keys[rx->sub->kr_flag].privacy;
    nid = rx->sub->keys[rx->sub->kr_flag].nid;

    BT_DBG("Relaying packet. TTL is now %u", TTL(buf->data));

    /* Update NID if RX or RX was with friend credentials */
    if (rx->friend_cred) {
        buf->data[0] &= 0x80; /* Clear everything except IVI */
        buf->data[0] |= nid;
    }

    /* We re-encrypt and obfuscate using the received IVI rather than
     * the normal TX IVI (which may be different) since the transport
     * layer nonce includes the IVI.
     */
    if (bt_mesh_net_encrypt(enc, &buf->b, BLE_MESH_NET_IVI_RX(rx), false)) {
        BT_ERR("Re-encrypting failed");
        goto done;
    }

    if (bt_mesh_net_obfuscate(buf->data, BLE_MESH_NET_IVI_RX(rx), priv)) {
        BT_ERR("Re-obfuscating failed");
        goto done;
    }

    /* Sending to the GATT bearer should only happen if GATT Proxy
     * is enabled or the message originates from the local node.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
            (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_ENABLED ||
             rx->friend_cred ||
             rx->net_if == BLE_MESH_NET_IF_LOCAL)) {
        if (bt_mesh_proxy_server_relay(&buf->b, rx->ctx.recv_dst) &&
                BLE_MESH_ADDR_IS_UNICAST(rx->ctx.recv_dst)) {
            goto done;
        }
    }

    if (relay_to_adv(rx->net_if) || rx->friend_cred) {
#if !defined(CONFIG_BLE_MESH_RELAY_ADV_BUF)
        bt_mesh_adv_send(buf, NULL, NULL);
#else
        bt_mesh_relay_adv_send(buf, NULL, NULL, rx->ctx.addr, rx->ctx.recv_dst);
#endif
    }

done:
    net_buf_unref(buf);
}

void bt_mesh_net_header_parse(struct net_buf_simple *buf,
                              struct bt_mesh_net_rx *rx)
{
    rx->old_iv = (IVI(buf->data) != (bt_mesh.iv_index & 0x01));
    rx->ctl = CTL(buf->data);
    rx->ctx.recv_ttl = TTL(buf->data);
    rx->seq = SEQ(buf->data);
    rx->ctx.addr = SRC(buf->data);
    rx->ctx.recv_dst = DST(buf->data);
}

int bt_mesh_net_decode(struct net_buf_simple *data, enum bt_mesh_net_if net_if,
                       struct bt_mesh_net_rx *rx, struct net_buf_simple *buf)
{
    if (data->len < BLE_MESH_NET_MIN_PDU_LEN) {
        BT_WARN("Dropping too short mesh packet (len %u)", data->len);
        BT_WARN("%s", bt_hex(data->data, data->len));
        return -EINVAL;
    }

    if (net_if == BLE_MESH_NET_IF_ADV && check_dup(data)) {
        return -EINVAL;
    }

    BT_DBG("%u bytes: %s", data->len, bt_hex(data->data, data->len));

    rx->net_if = net_if;

    if (!net_find_and_decrypt(data->data, data->len, rx, buf)) {
        BT_DBG("Unable to find matching net for packet");
        return -ENOENT;
    }

    /* Initialize AppIdx to a sane value */
    rx->ctx.app_idx = BLE_MESH_KEY_UNUSED;

    rx->ctx.recv_ttl = TTL(buf->data);

    /* Default to responding with TTL 0 for non-routed messages */
    if (rx->ctx.recv_ttl == 0U) {
        rx->ctx.send_ttl = 0U;
    } else {
        rx->ctx.send_ttl = BLE_MESH_TTL_DEFAULT;
    }

    rx->ctl = CTL(buf->data);
    rx->seq = SEQ(buf->data);
    rx->ctx.recv_dst = DST(buf->data);

    BT_DBG("Decryption successful. Payload len %u", buf->len);

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
#endif

    if (net_if != BLE_MESH_NET_IF_LOCAL && bt_mesh_elem_find(rx->ctx.addr)) {
        BT_DBG("Dropping locally originated packet");
        return -EBADMSG;
    }

    BT_DBG("src 0x%04x dst 0x%04x ttl %u", rx->ctx.addr, rx->ctx.recv_dst,
           rx->ctx.recv_ttl);
    BT_DBG("PDU: %s", bt_hex(buf->data, buf->len));

    msg_cache_add(rx);

    return 0;
}

static bool ready_to_recv(void)
{
    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_provisioned()) {
        return true;
    } else if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) && bt_mesh_is_provisioner_en()) {
        if (bt_mesh_provisioner_get_node_count()) {
            return true;
        }
    }

    return false;
}

static bool ignore_net_msg(uint16_t src, uint16_t dst)
{
    if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV)) {
        /* When fast provisioning is enabled, the node addr
         * message will be sent to the Primary Provisioner,
         * which shall not be ignored here.
         */
        return false;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) &&
        bt_mesh_is_provisioner_en()) {
        /* If the destination address of the message is the element
         * address of Provisioner, but Provisioner fails to find the
         * node in its provisioning database, then this message will
         * be ignored.
         */
        if (!bt_mesh_provisioner_get_node_with_addr(src)) {
            BT_INFO("Not found node address 0x%04x", src);
            return true;
        }
    }
    return false;
}

void bt_mesh_net_recv(struct net_buf_simple *data, int8_t rssi,
                      enum bt_mesh_net_if net_if)
{
    NET_BUF_SIMPLE_DEFINE(buf, 29);
    struct bt_mesh_net_rx rx = { .ctx.recv_rssi = rssi };
    struct net_buf_simple_state state = {0};

    BT_DBG("rssi %d net_if %u", rssi, net_if);

    if (!ready_to_recv()) {
        return;
    }

    if (bt_mesh_net_decode(data, net_if, &rx, &buf)) {
        return;
    }

    if (ignore_net_msg(rx.ctx.addr, rx.ctx.recv_dst)) {
        return;
    }

    /* Save the state so the buffer can later be relayed */
    net_buf_simple_save(&buf, &state);

    BT_BQB(BLE_MESH_BQB_TEST_LOG_LEVEL_PRIMARY_ID_NODE | BLE_MESH_BQB_TEST_LOG_LEVEL_SUB_ID_NET,
           "\nNetRecv: ctl: %d, src: %d, dst: %d, ttl: %d, data: 0x%s",
           CTL(buf.data), SRC(buf.data), DST(buf.data), TTL(buf.data),
           bt_hex(buf.data + BLE_MESH_NET_HDR_LEN, buf.len - BLE_MESH_NET_HDR_LEN));

    rx.local_match = (bt_mesh_fixed_group_match(rx.ctx.recv_dst) ||
                      bt_mesh_elem_find(rx.ctx.recv_dst));

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
            net_if == BLE_MESH_NET_IF_PROXY) {
        bt_mesh_proxy_server_addr_add(data, rx.ctx.addr);

        if (bt_mesh_gatt_proxy_get() == BLE_MESH_GATT_PROXY_DISABLED &&
            !rx.local_match) {
            BT_INFO("Proxy is disabled; ignoring message");
            return;
        }
    }

    /* The transport layer has indicated that it has rejected the message,
    * but would like to see it again if it is received in the future.
    * This can happen if a message is received when the device is in
    * Low Power mode, but the message was not encrypted with the friend
    * credentials. Remove it from the message cache so that we accept
    * it again in the future.
    */
    if (bt_mesh_trans_recv(&buf, &rx) == -EAGAIN) {
        BT_WARN("Removing rejected message from Network Message Cache");
        msg_cache[rx.msg_cache_idx].src = BLE_MESH_ADDR_UNASSIGNED;
        /* Rewind the next index now that we're not using this entry */
        msg_cache_next = rx.msg_cache_idx;
    }

    /* Relay if this was a group/virtual address, or if the destination
     * was neither a local element nor an LPN we're Friends for.
     */
    if (!BLE_MESH_ADDR_IS_UNICAST(rx.ctx.recv_dst) ||
        (!rx.local_match && !rx.friend_match
#if CONFIG_BLE_MESH_NOT_RELAY_REPLAY_MSG
        && !rx.replay_msg
#endif
        )) {
        net_buf_simple_restore(&buf, &state);
        bt_mesh_net_relay(&buf, &rx);
    }
}

static void ivu_refresh(struct k_work *work)
{
    bt_mesh.ivu_duration += BLE_MESH_IVU_HOURS;

    BT_INFO("%s for %u hour%s",
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
    if (bt_mesh_beacon_get() == BLE_MESH_BEACON_ENABLED) {
        bt_mesh_beacon_enable();
    } else {
        bt_mesh_beacon_disable();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER) &&
            bt_mesh_gatt_proxy_get() != BLE_MESH_GATT_PROXY_NOT_SUPPORTED) {
        bt_mesh_proxy_server_gatt_enable();
        bt_mesh_adv_update();
    }

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
    /* Add Mesh beacon type (Secure Network Beacon) to the exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_BEACON, NULL);
#endif

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
    k_delayed_work_init(&bt_mesh.ivu_timer, ivu_refresh);

    k_work_init(&bt_mesh.local_work, bt_mesh_net_local);
}

void bt_mesh_net_reset(void)
{
    k_delayed_work_cancel(&bt_mesh.ivu_timer);

#if FRIEND_CRED_COUNT > 0
    memset(friend_cred, 0, sizeof(friend_cred));
#endif

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
    bt_mesh_net_reset();

    k_delayed_work_free(&bt_mesh.ivu_timer);

    k_work_init(&bt_mesh.local_work, NULL);

    /* Local queue uses a while loop, currently no need
     * to handle this.
     */
}
#endif /* CONFIG_BLE_MESH_DEINIT */
