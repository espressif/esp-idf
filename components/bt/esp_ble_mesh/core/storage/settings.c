/*
 * SPDX-FileCopyrightText: 2018 Intel Corporation
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <string.h>

#include "mesh.h"
#include "crypto.h"
#include "transport.h"
#include "access.h"
#include "foundation.h"
#include "proxy_server.h"
#include "mesh/cfg_srv.h"
#include "mesh/common.h"
#include "settings_nvs.h"
#include "settings.h"
#include "settings_uid.h"
#include "pvnr_mgmt.h"
#include "prov_pvnr.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif /* CONFIG_BLE_MESH_V11_SUPPORT */

/* BLE Mesh NVS Key and corresponding data struct.
 * Note: The length of nvs key must be <= 15.
 *       "xxxx" (2 octet) means the rpl_src, net_idx, app_idx, model_key, etc.
 *       Model model_key is a combination "elem_idx << 8 | model_idx".
 * key: "mesh/net"    -> write/read to set/get NET data
 * key: "mesh/iv"     -> write/read to set/get IV data
 * key: "mesh/seq"    -> write/read to set/get SEQ data
 * key: "mesh/hb_pub" -> write/read to set/get CFG HB_PUB data
 * key: "mesh/cfg"    -> write/read to set/get CFG data
 * key: "mesh/rpl"    -> write/read to set/get all RPL src.
 *      key: "mesh/rpl/xxxx" -> write/read to set/get the "xxxx" RPL data
 * key: "mesh/netkey" -> write/read to set/get all NetKey Indexes
 *      key: "mesh/nk/xxxx" -> write/read to set/get the "xxxx" NetKey data
 * key: "mesh/appkey" -> write/read to set/get all AppKey Indexes
 *      key: "mesh/ak/xxxx" -> write/read to set/get the "xxxx" AppKey data
 * key: "mesh/sig" -> write/read to set/get all SIG MODEL model_keys.
 *      key: "mesh/s/xxxx/b"  -> write/read to set/get SIG MODEL Bind AppKey List
 *      key: "mesh/s/xxxx/s" -> write/read to set/get SIG MODEL Subscription List
 *      key: "mesh/s/xxxx/p" -> write/read to set/get SIG MODEL Publication
 *      key: "mesh/s/xxxx/d" -> write/read to set/get SIG MODEL Data
 * key: "mesh/vnd" -> write/read to set/get all VENDOR MODEL model_keys.
 *      key: "mesh/v/xxxx/b"  -> write/read to set/get VENDOR MODEL Bind AppKey List
 *      key: "mesh/v/xxxx/s" -> write/read to set/get VENDOR MODEL Subscription List
 *      key: "mesh/v/xxxx/p" -> write/read to set/get VENDOR MODEL Publication
 *      key: "mesh/v/xxxx/d" -> write/read to set/get VENDOR MODEL Data
 * key: "mesh/vaddr" -> write/read to set/get all virtual addresses
 *      key: "mesh/va/xxxx" -> write/read to set/get the "xxxx" virtual address
 * key: "mesh/dkca" -> write/read to set/get Device Key Candidate
 */

#if CONFIG_BLE_MESH_SETTINGS

/* Tracking of what storage changes are pending for App and Net Keys. We
 * track this in a separate array here instead of within the respective
 * bt_mesh_app_key and bt_mesh_subnet structs themselves, since once a key
 * gets deleted its struct becomes invalid and may be reused for other keys.
 */
static struct key_update {
    uint16_t key_idx:12, /* AppKey or NetKey Index */
             valid:1,    /* 1 if this entry is valid, 0 if not */
             app_key:1,  /* 1 if this is an AppKey, 0 if a NetKey */
             clear:1;    /* 1 if key needs clearing, 0 if storing */
} key_updates[CONFIG_BLE_MESH_APP_KEY_COUNT + CONFIG_BLE_MESH_SUBNET_COUNT];

static struct k_delayed_work pending_store;
static void store_pending(struct k_work *work);

/* Mesh network storage information */
struct net_val {
    uint16_t primary_addr;
    uint8_t  dev_key[16];
} __attribute__((packed));

/* Sequence number storage */
struct seq_val {
    uint8_t val[3];
} __attribute__((packed));

/* Heartbeat Publication storage */
struct hb_pub_val {
    uint16_t dst;
    uint8_t  period;
    uint8_t  ttl;
    uint16_t feat;
    uint16_t net_idx:12,
             indefinite:1;
};

/* Miscellaneous configuration server model states */
struct cfg_val {
    uint8_t net_transmit;
    uint8_t relay;
    uint8_t relay_retransmit;
    uint8_t beacon;
    uint8_t gatt_proxy;
    uint8_t frnd;
    uint8_t default_ttl;
};

/* IV Index & IV Update storage */
struct iv_val {
    uint32_t iv_index;
    uint8_t  iv_update:1,
             iv_duration:7;
} __attribute__((packed));

/* Replay Protection List storage */
struct rpl_val {
    uint32_t seq:24,
             old_iv:1;
};

/* NetKey storage information */
struct net_key_val {
    uint8_t kr_flag:1,
            kr_phase:7;
    uint8_t val[2][16];
} __attribute__((packed));

/* AppKey storage information */
struct app_key_val {
    uint16_t net_idx;
    bool     updated;
    uint8_t  val[2][16];
} __attribute__((packed));

struct mod_pub_val {
    uint16_t addr;
    uint16_t key;
    uint8_t  ttl;
    uint8_t  retransmit;
    uint8_t  period;
    uint8_t  period_div:4,
             cred:1;
#if CONFIG_BLE_MESH_DF_SRV
    uint8_t  directed_pub_policy;   /* Directed publish policy */
#endif /* CONFIG_BLE_MESH_DF_SRV */
};

/* Virtual Address information */
struct va_val {
    uint16_t ref;
    uint16_t addr;
    uint8_t  uuid[16];
} __attribute__((packed));

/* We need this so we don't overwrite app-hardcoded values in case FCB
 * contains a history of changes but then has a NULL at the end.
 */
static struct {
    bool valid;
    struct cfg_val cfg;
} stored_cfg;

struct prov_info {
    uint16_t primary_addr;
    uint16_t alloc_addr;
};

struct node_info {
    uint8_t  addr[6];
    uint8_t  addr_type;
    uint8_t  dev_uuid[16];
    uint16_t oob_info;
    uint16_t unicast_addr;
    uint8_t  element_num;
    uint16_t net_idx;
    uint8_t  flags;
    uint32_t iv_index;
    uint8_t  dev_key[16];
} __attribute__((packed));

static bt_mesh_mutex_t settings_lock;

static inline void bt_mesh_settings_mutex_new(void)
{
    if (settings_lock.mutex == NULL) {
        bt_mesh_mutex_create(&settings_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_settings_mutex_free(void)
{
    bt_mesh_mutex_free(&settings_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_settings_lock(void)
{
    bt_mesh_mutex_lock(&settings_lock);
}

void bt_mesh_settings_unlock(void)
{
    bt_mesh_mutex_unlock(&settings_lock);
}

static int role_set(const char *name)
{
    bool exist = false;
    int err = 0;

    BT_DBG("RoleSet, Name %s", name);

    err = bt_mesh_load_core_settings(name, (uint8_t *)bt_mesh.flags, sizeof(bt_mesh.flags), &exist);
    if (err) {
        BT_ERR("Failed to load mesh device role");
        return err;
    }

    if (exist == false) {
#if CONFIG_BLE_MESH_SETTINGS_BACKWARD_COMPATIBILITY
        if (IS_ENABLED(CONFIG_BLE_MESH_NODE) &&
            !IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER)) {
            bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_NODE);
        }
#else /* CONFIG_BLE_MESH_SETTINGS_BACKWARD_COMPATIBILITY */
        return 0;
#endif /* CONFIG_BLE_MESH_SETTINGS_BACKWARD_COMPATIBILITY */
    }

    return 0;
}

static int net_set(const char *name)
{
    struct net_val net = {0};
    bool exist = false;
    int err = 0;

    BT_DBG("NetSet, Name %s", name);

    err = bt_mesh_load_core_settings(name, (uint8_t *)&net, sizeof(net), &exist);
    if (err) {
        BT_ERR("Failed to load node net info");
        memset(bt_mesh.dev_key, 0, sizeof(bt_mesh.dev_key));
        bt_mesh_comp_unprovision();
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    memcpy(bt_mesh.dev_key, net.dev_key, sizeof(bt_mesh.dev_key));
    bt_mesh_comp_provision(net.primary_addr);

    BT_INFO("Restored Primary Address 0x%04x", net.primary_addr);
    BT_INFO("Restored DevKey %s", bt_hex(bt_mesh.dev_key, 16));

    return 0;
}

static int dkca_set(const char *name)
{
    bool exist = false;
    int err = 0;

    BT_DBG("DkcaSet, Name %s", name);

    err = bt_mesh_load_core_settings(name, bt_mesh.dev_key_ca, sizeof(bt_mesh.dev_key_ca), &exist);
    if (err) {
        BT_ERR("Failed to load DevKey Candidate");
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    BT_INFO("Restored DevKey Candidate %s", bt_hex(bt_mesh.dev_key_ca, 16));

    return 0;
}

static int iv_set(const char *name)
{
    struct iv_val iv = {0};
    bool exist = false;
    int err = 0;

    BT_DBG("IVSet, Name %s", name);

    err = bt_mesh_load_core_settings(name, (uint8_t *)&iv, sizeof(iv), &exist);
    if (err) {
        BT_ERR("Failed to load iv_index");
        bt_mesh.iv_index = 0U;
        bt_mesh_atomic_clear_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS);
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    bt_mesh.iv_index = iv.iv_index;
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS, iv.iv_update);
    bt_mesh.ivu_duration = iv.iv_duration;

    BT_INFO("Restored IV Index 0x%08x (IV Update Flag %u) duration %u hours",
            iv.iv_index, iv.iv_update, iv.iv_duration);

    return 0;
}

static int seq_set(const char *name)
{
    struct seq_val seq = {0};
    bool exist = false;
    int err = 0;

    BT_DBG("SeqSet, Name %s", name);

    err = bt_mesh_load_core_settings(name, (uint8_t *)&seq, sizeof(seq), &exist);
    if (err) {
        BT_ERR("Failed to load sequence number");
        bt_mesh.seq = 0U;
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    bt_mesh.seq = sys_get_le24(seq.val);

#if CONFIG_BLE_MESH_SEQ_STORE_RATE > 0
    /* Make sure we have a large enough sequence number. We
     * subtract 1 so that the first transmission causes a write
     * to the settings storage.
     */
    bt_mesh.seq += (CONFIG_BLE_MESH_SEQ_STORE_RATE -
                    (bt_mesh.seq % CONFIG_BLE_MESH_SEQ_STORE_RATE));
    bt_mesh.seq--;
#endif /* CONFIG_BLE_MESH_SEQ_STORE_RATE > 0 */

    BT_INFO("Restored Sequence Number 0x%06x", bt_mesh.seq);

    return 0;
}

static struct bt_mesh_rpl *rpl_find(uint16_t src)
{
    int i;

    BT_DBG("RPLFind, Src 0x%04x", src);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        if (bt_mesh.rpl[i].src == src) {
            return &bt_mesh.rpl[i];
        }
    }

    return NULL;
}

static struct bt_mesh_rpl *rpl_alloc(uint16_t src)
{
    int i;

    BT_DBG("RPLAlloc, Src 0x%04x", src);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        if (bt_mesh.rpl[i].src == BLE_MESH_ADDR_UNASSIGNED) {
            bt_mesh.rpl[i].src = src;
            return &bt_mesh.rpl[i];
        }
    }

    return NULL;
}

static int rpl_set(const char *name)
{
    struct net_buf_simple *buf = NULL;
    struct bt_mesh_rpl *entry = NULL;
    struct rpl_val rpl = {0};
    char get[16] = {'\0'};
    bool exist = false;
    size_t length = 0U;
    int err = 0;
    int i;

    BT_DBG("RPLSet, Name %s", name);

    buf = bt_mesh_get_core_settings_item(name);
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t src = net_buf_simple_pull_le16(buf);

        if (!BLE_MESH_ADDR_IS_UNICAST(src)) {
            BT_ERR("Invalid source address 0x%04x", src);
            continue;
        }

        sprintf(get, "mesh/rpl/%04x", src);

        err = bt_mesh_load_core_settings(get, (uint8_t *)&rpl, sizeof(rpl), &exist);
        if (err) {
            BT_ERR("Failed to load RPL entry 0x%04x", src);
            continue;
        }

        if (exist == false) {
            continue;
        }

        entry = rpl_find(src);
        if (!entry) {
            entry = rpl_alloc(src);
            if (!entry) {
                BT_ERR("No space for a new RPL 0x%04x", src);
                err = -ENOMEM;
                goto free;
            }
        }

        entry->src = src;
        entry->seq = rpl.seq;
        entry->old_iv = rpl.old_iv;

        BT_INFO("Restored RPL entry 0x%04x: seq 0x%06x, old_iv %u", src, rpl.seq, rpl.old_iv);
    }

free:
    bt_mesh_free_buf(buf);
    return err;
}

static struct bt_mesh_subnet *subnet_exist(uint16_t net_idx)
{
    int i;

    BT_DBG("SubnetExist, NetIdx 0x%04x", net_idx);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        if (bt_mesh.sub[i].net_idx == net_idx) {
            return &bt_mesh.sub[i];
        }
    }

    return NULL;
}

static struct bt_mesh_subnet *subnet_alloc(uint16_t net_idx)
{
    int i;

    BT_DBG("SubnetAlloc, NetIdx 0x%04x", net_idx);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        if (bt_mesh.sub[i].net_idx == BLE_MESH_KEY_UNUSED) {
            bt_mesh.sub[i].net_idx = net_idx;
            return &bt_mesh.sub[i];
        }
    }

    return NULL;
}

static struct bt_mesh_app_key *appkey_exist(uint16_t app_idx)
{
    int i;

    BT_DBG("AppKeyExist, AppIdx 0x%04x", app_idx);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        if (bt_mesh.app_keys[i].net_idx != BLE_MESH_KEY_UNUSED &&
            bt_mesh.app_keys[i].app_idx == app_idx) {
            return &bt_mesh.app_keys[i];
        }
    }

    return NULL;
}

static int net_key_set(const char *name)
{
    struct net_buf_simple *buf = NULL;
    struct bt_mesh_subnet *sub = NULL;
    struct net_key_val key = {0};
    char get[16] = {'\0'};
    bool exist = false;
    size_t length = 0U;
    int err = 0;
    int i;

    BT_DBG("NetKeySet, Name %s", name);

    buf = bt_mesh_get_core_settings_item(name);
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t net_idx = net_buf_simple_pull_le16(buf);
        sprintf(get, "mesh/nk/%04x", net_idx);

        err = bt_mesh_load_core_settings(get, (uint8_t *)&key, sizeof(key), &exist);
        if (err) {
            BT_ERR("Failed to load NetKey 0x%03x", net_idx);
            continue;
        }

        if (exist == false) {
            continue;
        }

        sub = subnet_exist(net_idx);
        if (!sub) {
            sub = subnet_alloc(net_idx);
            if (!sub) {
                BT_ERR("No space for a new subnet 0x%03x", net_idx);
                err = -ENOMEM;
                goto free;
            }
        }

        sub->net_idx = net_idx;
        sub->kr_flag = key.kr_flag;
        sub->kr_phase = key.kr_phase;
        memcpy(sub->keys[0].net, &key.val[0], 16);
        memcpy(sub->keys[1].net, &key.val[1], 16);

        BT_INFO("Restored NetKeyIndex 0x%03x", sub->net_idx);
        BT_INFO("Restored NetKey %s", bt_hex(sub->keys[0].net, 16));
    }

free:
    bt_mesh_free_buf(buf);
    return err;
}

static int app_key_set(const char *name)
{
    struct bt_mesh_app_key *app = NULL;
    struct bt_mesh_subnet *sub = NULL;
    struct net_buf_simple *buf = NULL;
    struct app_key_val key = {0};
    char get[16] = {'\0'};
    bool exist = false;
    size_t length = 0U;
    int err = 0;
    int i;

    BT_DBG("AppKeySet, Name %s", name);

    buf = bt_mesh_get_core_settings_item(name);
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t app_idx = net_buf_simple_pull_le16(buf);
        sprintf(get, "mesh/ak/%04x", app_idx);

        err = bt_mesh_load_core_settings(get, (uint8_t *)&key, sizeof(key), &exist);
        if (err) {
            BT_ERR("Failed to load AppKey 0x%03x", app_idx);
            continue;
        }

        if (exist == false) {
            continue;
        }

        sub = subnet_exist(key.net_idx);
        if (!sub) {
            BT_ERR("Failed to find subnet 0x%03x", key.net_idx);
            continue;
        }

        app = appkey_exist(app_idx);
        if (!app) {
            app = bt_mesh_app_key_alloc(app_idx);
            if (!app) {
                err = -ENOMEM;
                goto free;
            }
        }

        app->net_idx = key.net_idx;
        app->app_idx = app_idx;
        app->updated = key.updated;
        memcpy(app->keys[0].val, key.val[0], 16);
        memcpy(app->keys[1].val, key.val[1], 16);
        bt_mesh_app_id(app->keys[0].val, &app->keys[0].id);
        bt_mesh_app_id(app->keys[1].val, &app->keys[1].id);

        BT_INFO("Restored AppKeyIndex 0x%03x, NetKeyIndex 0x%03x",
                app->app_idx, app->net_idx);
        BT_INFO("Restored AppKey %s", bt_hex(app->keys[0].val, 16));
    }

free:
    bt_mesh_free_buf(buf);
    return err;
}

static int hb_pub_set(const char *name)
{
    struct bt_mesh_hb_pub *hb_pub = bt_mesh_hb_pub_get();
    struct hb_pub_val hb_val = {0};
    bool exist = false;
    int err = 0;

    BT_DBG("HbPubSet, Name %s", name);

    if (!hb_pub) {
        BT_ERR("Invalid heartbeat publication");
        return -EINVAL;
    }

    err = bt_mesh_load_core_settings(name, (uint8_t *)&hb_val, sizeof(hb_val), &exist);
    if (err) {
        BT_ERR("Failed to load heartbeat publication");
        hb_pub->dst = BLE_MESH_ADDR_UNASSIGNED;
        hb_pub->count = 0U;
        hb_pub->ttl = 0U;
        hb_pub->period = 0U;
        hb_pub->feat = 0U;
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    hb_pub->dst = hb_val.dst;
    hb_pub->period = hb_val.period;
    hb_pub->ttl = hb_val.ttl;
    hb_pub->feat = hb_val.feat;
    hb_pub->net_idx = hb_val.net_idx;
    if (hb_val.indefinite) {
        hb_pub->count = 0xffff;
    } else {
        hb_pub->count = 0U;
    }

    BT_INFO("Restored Heartbeat Publication, dst 0x%04x, period %d, net_idx 0x%03x",
            hb_pub->dst, hb_pub->period, hb_pub->net_idx);

    return 0;
}

static int cfg_set(const char *name)
{
    struct bt_mesh_cfg_srv *cfg = bt_mesh_cfg_get();
    struct cfg_val val = {0};
    bool exist = false;
    int err = 0;

    BT_DBG("CfgSet, Name %s", name);

    if (!cfg) {
        BT_ERR("Invalid configuration");
        stored_cfg.valid = false;
        return -EINVAL;
    }

    err = bt_mesh_load_core_settings(name, (uint8_t *)&val, sizeof(val), &exist);
    if (err) {
        BT_ERR("Failed to load configuration state");
        stored_cfg.valid = false;
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    memcpy(&stored_cfg.cfg, &val, sizeof(val));
    stored_cfg.valid = true;

    BT_INFO("Restored Configuration, ttl %d, transmit 0x%02x, retransmit 0x%02x",
            val.default_ttl, val.net_transmit, val.relay_retransmit);

    return 0;
}

static int model_set_bind(bool vnd, struct bt_mesh_model *model, uint16_t model_key)
{
    char name[16] = {'\0'};
    bool exist = false;
    int err = 0;
    int i;

    BT_DBG("ModelSetBind, Key 0x%04x Vnd %u", model_key, vnd);

    /* Start with empty array regardless of cleared or set value */
    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        model->keys[i] = BLE_MESH_KEY_UNUSED;
    }

    sprintf(name, "mesh/%s/%04x/b", vnd ? "v" : "s", model_key);
    err = bt_mesh_load_core_settings(name, (uint8_t *)model->keys, sizeof(model->keys), &exist);
    if (err) {
        BT_ERR("Failed to load model bound keys");
        return -EIO;
    }

    if (exist == true) {
        BT_INFO("Restored Model Bound AppKey, index %s", bt_hex(model->keys, sizeof(model->keys)));
    }

    return 0;
}

static int model_set_sub(bool vnd, struct bt_mesh_model *model, uint16_t model_key)
{
    char name[16] = {'\0'};
    bool exist = false;
    int err = 0;
    int i;

    BT_DBG("ModelSetSub, Key 0x%04x Vnd %u", model_key, vnd);

    /* Start with empty array regardless of cleared or set value */
    for (i = 0; i < ARRAY_SIZE(model->groups); i++) {
        model->groups[i] = BLE_MESH_ADDR_UNASSIGNED;
    }

    sprintf(name, "mesh/%s/%04x/s", vnd ? "v" : "s", model_key);
    err = bt_mesh_load_core_settings(name, (uint8_t *)model->groups, sizeof(model->groups), &exist);
    if (err) {
        BT_ERR("Failed to load model subscriptions");
        return -EIO;
    }

    if (exist == true) {
        BT_INFO("Restored Model Subscription, address %s", bt_hex(model->groups, sizeof(model->groups)));
    }

    return 0;
}

static int model_set_pub(bool vnd, struct bt_mesh_model *model, uint16_t model_key)
{
    struct mod_pub_val pub = {0};
    char name[16] = {'\0'};
    bool exist = false;
    int err = 0;

    BT_DBG("ModelSetPub, Key 0x%04x Vnd %u", model_key, vnd);

    if (!model->pub) {
        BT_INFO("Not support publication, model_id 0x%04x, cid 0x%04x",
                vnd ? model->vnd.id : model->id, vnd ? model->vnd.company : 0xFFFF);
        return 0;
    }

    sprintf(name, "mesh/%s/%04x/p", vnd ? "v" : "s", model_key);
    err = bt_mesh_load_core_settings(name, (uint8_t *)&pub, sizeof(pub), &exist);
    if (err) {
        BT_ERR("Failed to load model publication");
        model->pub->addr = BLE_MESH_ADDR_UNASSIGNED;
        model->pub->key = 0U;
        model->pub->cred = 0U;
        model->pub->ttl = 0U;
        model->pub->period = 0U;
        model->pub->retransmit = 0U;
        model->pub->count = 0U;
        return -EIO;
    }

    if (exist == false) {
        return 0;
    }

    model->pub->addr = pub.addr;
    model->pub->key = pub.key;
    model->pub->cred = pub.cred;
    model->pub->ttl = pub.ttl;
    model->pub->period = pub.period;
    model->pub->retransmit = pub.retransmit;
    model->pub->count = 0U;

#if CONFIG_BLE_MESH_DF_SRV
    model->pub->directed_pub_policy = pub.directed_pub_policy; /* Directed publish policy */
    bt_mesh_power_up_create_path_origin_fsm(model);
#endif /* CONFIG_BLE_MESH_DF_SRV */

    BT_INFO("Restored Model Publication, address 0x%04x, app_idx 0x%03x", pub.addr, pub.key);

    return 0;
}

static int model_set(bool vnd, const char *name)
{
    struct bt_mesh_model *model = NULL;
    struct net_buf_simple *buf = NULL;
    uint8_t elem_idx = 0U, model_idx = 0U;
    size_t length = 0U;
    int i;

    BT_DBG("ModelSet, Name %s Vnd %u", name, vnd);

    buf = bt_mesh_get_core_settings_item(name);
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t model_key = net_buf_simple_pull_le16(buf);

        elem_idx = BLE_MESH_GET_ELEM_IDX(model_key);
        model_idx = BLE_MESH_GET_MODEL_IDX(model_key);

        model = bt_mesh_model_get(vnd, elem_idx, model_idx);
        if (!model) {
            BT_ERR("%s model not found, elem_idx %u, model_idx %u",
                   vnd ? "vnd" : "sig", elem_idx, model_idx);
            continue;
        }

        model_set_bind(vnd, model, model_key);
        model_set_sub(vnd, model, model_key);
        model_set_pub(vnd, model, model_key);
    }

    bt_mesh_free_buf(buf);
    return 0;
}

static int sig_mod_set(const char *name)
{
    return model_set(false, name);
}

static int vnd_mod_set(const char *name)
{
    return model_set(true, name);
}

#if CONFIG_BLE_MESH_LABEL_COUNT > 0
static int va_set(const char *name)
{
    struct net_buf_simple *buf = NULL;
    struct va_val va = {0};
    char get[16] = {'\0'};
    struct label *lab = NULL;
    size_t length = 0U;
    bool exist = false;
    int err = 0;
    int i;

    BT_DBG("VaSet, Name %s", name);

    buf = bt_mesh_get_core_settings_item(name);
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t index = net_buf_simple_pull_le16(buf);
        sprintf(get, "mesh/va/%04x", index);

        err = bt_mesh_load_core_settings(get, (uint8_t *)&va, sizeof(va), &exist);
        if (err) {
            BT_ERR("Failed to load virtual address 0x%04x", index);
            continue;
        }

        if (exist == false) {
            continue;
        }

        if (va.ref == 0) {
            BT_DBG("Ignore virtual address %s with ref = 0", get);
            continue;
        }

        lab = get_label(index);
        if (lab == NULL) {
            BT_WARN("Out of labels buffers");
            err = -ENOBUFS;
            goto free;
        }

        memcpy(lab->uuid, va.uuid, 16);
        lab->addr = va.addr;
        lab->ref = va.ref;

        BT_INFO("Restored Virtual Address 0x%04x, ref 0x%04x", index, lab->ref);
    }

free:
    bt_mesh_free_buf(buf);
    return err;
}
#endif /* CONFIG_BLE_MESH_LABEL_COUNT > 0 */

#if CONFIG_BLE_MESH_PROVISIONER
static int p_prov_set(const char *name)
{
    struct prov_info val = {0};
    bool exist = false;
    int err = 0;

    BT_DBG("PvnrProvSet, Name %s", name);

    err = bt_mesh_load_core_settings(name, (uint8_t *)&val, sizeof(val), &exist);
    if (err) {
        BT_ERR("Failed to load next address allocation");
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    bt_mesh_provisioner_restore_prov_info(val.primary_addr, val.alloc_addr);

    BT_INFO("Restored Primary Address 0x%04x, next address alloc 0x%04x",
            val.primary_addr, val.alloc_addr);

    return 0;
}

static int p_net_idx_set(const char *name)
{
    uint16_t net_idx = 0U;
    bool exist = false;
    int err = 0;

    BT_DBG("PvnrNetIdxSet, Name %s", name);

    err = bt_mesh_load_core_settings(name, (uint8_t *)&net_idx, sizeof(net_idx), &exist);
    if (err) {
        BT_ERR("Failed to load next NetKeyIndex alloc");
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    bt_mesh.p_net_idx_next = net_idx;

    BT_INFO("Restored next NetKeyIndex alloc 0x%03x", bt_mesh.p_net_idx_next);

    return 0;
}

static int p_app_idx_set(const char *name)
{
    uint16_t app_idx = 0U;
    bool exist = false;
    int err = 0;

    BT_DBG("PvnrAppIdxSet, Name %s", name);

    err = bt_mesh_load_core_settings(name, (uint8_t *)&app_idx, sizeof(app_idx), &exist);
    if (err) {
        BT_ERR("Failed to load next AppKeyIndex alloc");
        return 0;
    }

    if (exist == false) {
        return 0;
    }

    bt_mesh.p_app_idx_next = app_idx;

    BT_INFO("Restored next AppKeyIndex alloc 0x%03x", bt_mesh.p_app_idx_next);

    return 0;
}

static struct bt_mesh_subnet *p_subnet_exist(uint16_t net_idx)
{
    int i;

    BT_DBG("PvnrSubnetExist, NetIdx 0x%04x", net_idx);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        if (bt_mesh.p_sub[i] &&
            bt_mesh.p_sub[i]->net_idx == net_idx) {
            return bt_mesh.p_sub[i];
        }
    }

    return NULL;
}

static struct bt_mesh_subnet *p_subnet_alloc(void)
{
    int i;

    BT_DBG("PvnrSubnetAlloc");

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        if (bt_mesh.p_sub[i] == NULL) {
            bt_mesh.p_sub[i] = bt_mesh_calloc(sizeof(struct bt_mesh_subnet));
            if (!bt_mesh.p_sub[i]) {
                BT_ERR("%s, Out of memory", __func__);
                return NULL;
            }

            return bt_mesh.p_sub[i];
        }
    }

    return NULL;
}

static struct bt_mesh_app_key *p_appkey_exist(uint16_t app_idx)
{
    int i;

    BT_DBG("PvnrAppKeyExist, AppIdx 0x%04x", app_idx);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        if (bt_mesh.p_app_keys[i] &&
            bt_mesh.p_app_keys[i]->net_idx != BLE_MESH_KEY_UNUSED &&
            bt_mesh.p_app_keys[i]->app_idx == app_idx) {
            return bt_mesh.p_app_keys[i];
        }
    }

    return NULL;
}

static struct bt_mesh_app_key *p_appkey_alloc(void)
{
    int i;

    BT_DBG("PvnrAppKeyAlloc");

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        if (bt_mesh.p_app_keys[i] == NULL) {
            bt_mesh.p_app_keys[i] = bt_mesh_calloc(sizeof(struct bt_mesh_app_key));
            if (!bt_mesh.p_app_keys[i]) {
                BT_ERR("%s, Out of memory", __func__);
                return NULL;
            }

            return bt_mesh.p_app_keys[i];
        }
    }

    return NULL;
}

static int p_net_key_set(const char *name)
{
    struct net_buf_simple *buf = NULL;
    struct bt_mesh_subnet *sub = NULL;
    struct net_key_val key = {0};
    char get[16] = {'\0'};
    size_t length = 0U;
    bool exist = false;
    int err = 0;
    int i;

    BT_DBG("PvnrNetKeySet, Name %s", name);

    buf = bt_mesh_get_core_settings_item(name);
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t net_idx = net_buf_simple_pull_le16(buf);
        sprintf(get, "mesh/pnk/%04x", net_idx);

        err = bt_mesh_load_core_settings(get, (uint8_t *)&key, sizeof(key), &exist);
        if (err) {
            BT_ERR("Failed to load NetKey 0x%03x", net_idx);
            continue;
        }

        if (exist == false) {
            continue;
        }

        sub = p_subnet_exist(net_idx);
        if (!sub) {
            sub = p_subnet_alloc();
            if (!sub) {
                BT_ERR("No space for a new subnet 0x%03x", net_idx);
                err = -ENOMEM;
                goto free;
            }
        }

        sub->net_idx = net_idx;
        sub->kr_flag = key.kr_flag;
        sub->kr_phase = key.kr_phase;
        memcpy(sub->keys[0].net, &key.val[0], 16);
        memcpy(sub->keys[1].net, &key.val[1], 16);

        BT_INFO("Restored NetKeyIndex 0x%03x", sub->net_idx);
        BT_INFO("Restored NetKey %s", bt_hex(sub->keys[0].net, 16));
    }

free:
    bt_mesh_free_buf(buf);
    return err;
}

static int p_app_key_set(const char *name)
{
    struct bt_mesh_app_key *app = NULL;
    struct bt_mesh_subnet *sub = NULL;
    struct net_buf_simple *buf = NULL;
    struct app_key_val key = {0};
    char get[16] = {'\0'};
    size_t length = 0U;
    bool exist = false;
    int err = 0;
    int i;

    BT_DBG("PvnrAppKeySet, Name %s", name);

    buf = bt_mesh_get_core_settings_item(name);
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t app_idx = net_buf_simple_pull_le16(buf);
        sprintf(get, "mesh/pak/%04x", app_idx);

        err = bt_mesh_load_core_settings(get, (uint8_t *)&key, sizeof(key), &exist);
        if (err) {
            BT_ERR("Failed to load AppKey 0x%03x", app_idx);
            continue;
        }

        if (exist == false) {
            continue;
        }

        sub = p_subnet_exist(key.net_idx);
        if (!sub) {
            BT_ERR("Failed to find subnet 0x%03x", key.net_idx);
            continue;
        }

        app = p_appkey_exist(app_idx);
        if (!app) {
            app = p_appkey_alloc();
            if (!app) {
                BT_ERR("No space for a new appkey 0x%03x", app_idx);
                err = -ENOMEM;
                goto free;
            }
        }

        app->net_idx = key.net_idx;
        app->app_idx = app_idx;
        app->updated = key.updated;
        memcpy(app->keys[0].val, key.val[0], 16);
        memcpy(app->keys[1].val, key.val[1], 16);
        bt_mesh_app_id(app->keys[0].val, &app->keys[0].id);
        bt_mesh_app_id(app->keys[1].val, &app->keys[1].id);

        BT_INFO("Restored AppKeyIndex 0x%03x, NetKeyIndex 0x%03x",
                app->app_idx, app->net_idx);
        BT_INFO("Restored AppKey %s", bt_hex(app->keys[0].val, 16));
    }

free:
    bt_mesh_free_buf(buf);
    return err;
}

static int node_info_set(uint16_t addr, bool *exist)
{
    struct bt_mesh_node node = {0};
    struct node_info info = {0};
    char get[16] = {'\0'};
    int err = 0;

    BT_DBG("NodeInfoSet, Addr 0x%04x", addr);

    sprintf(get, "mesh/pn/%04x/i", addr);
    err = bt_mesh_load_core_settings(get, (uint8_t *)&info, sizeof(info), exist);
    if (err) {
        BT_ERR("Failed to load node 0x%04x info", addr);
        return -EIO;
    }

    if (*exist == false) {
        return 0;
    }

    memcpy(node.addr, info.addr, BLE_MESH_ADDR_LEN);
    node.addr_type = info.addr_type;
    memcpy(node.dev_uuid, info.dev_uuid, 16);
    node.oob_info = info.oob_info;
    node.unicast_addr = info.unicast_addr;
    node.element_num = info.element_num;
    node.net_idx = info.net_idx;
    node.flags = info.flags;
    node.iv_index = info.iv_index;
    memcpy(node.dev_key, info.dev_key, 16);

    err = bt_mesh_provisioner_restore_node_info(&node);
    if (err) {
        BT_ERR("Failed to restore node 0x%04x info", addr);
        return -EIO;
    }

    BT_INFO("Restored Node 0x%04x, UUID %s", addr, bt_hex(node.dev_uuid, 16));

    return 0;
}

static int node_name_set(uint16_t addr)
{
    char name[BLE_MESH_NODE_NAME_SIZE + 1] = {0};
    char get[16] = {'\0'};
    bool exist = false;
    int err = 0;

    BT_DBG("NodeNameSet, Addr 0x%04x", addr);

    sprintf(get, "mesh/pn/%04x/n", addr);
    err = bt_mesh_load_core_settings(get, (uint8_t *)name, BLE_MESH_NODE_NAME_SIZE, &exist);
    if (err) {
        BT_ERR("Failed to load node 0x%04x name", addr);
        return -EIO;
    }

    if (exist == false) {
        return 0;
    }

    err = bt_mesh_provisioner_restore_node_name(addr, name);
    if (err) {
        BT_ERR("Failed to restore node 0x%04x name", addr);
        return -EIO;
    }

    BT_INFO("Restored Node 0x%04x, Name %s", addr, name);

    return 0;
}

static int node_comp_data_set(uint16_t addr)
{
    struct net_buf_simple *buf = NULL;
    char get[16] = {'\0'};
    int err = 0;

    BT_DBG("NodeCompDataSet, Addr 0x%04x", addr);

    sprintf(get, "mesh/pn/%04x/c", addr);
    buf = bt_mesh_get_core_settings_item(get);
    if (!buf) {
        return 0;
    }

    err = bt_mesh_provisioner_restore_node_comp_data(addr, buf->data, buf->len);
    if (err) {
        BT_ERR("Failed to restore node 0x%04x comp data", addr);
    } else {
        BT_INFO("Restored Node 0x%04x, Composition Data %s", addr, bt_hex(buf->data, buf->len));
    }

    bt_mesh_free_buf(buf);
    return err;
}

static int p_node_set(const char *name)
{
    struct net_buf_simple *buf = NULL;
    bool exist = false;
    size_t length = 0U;
    int i;

    BT_DBG("PvnrNodeSet, Name %s", name);

    buf = bt_mesh_get_core_settings_item(name);
    if (!buf) {
        return 0;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        uint16_t addr = net_buf_simple_pull_le16(buf);
        if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
            BT_ERR("Invalid unicast address 0x%04x", addr);
            continue;
        }

        if (node_info_set(addr, &exist)) {
            continue;
        }

        if (exist == false) {
            continue;
        }

        if (node_name_set(addr)) {
            continue;
        }

        if (node_comp_data_set(addr)) {
            continue;
        }
    }

    bt_mesh_free_buf(buf);
    return 0;
}
#endif /* CONFIG_BLE_MESH_PROVISIONER */

const struct bt_mesh_setting {
    const char *name;
    int (*func)(const char *name);
} settings[] = {
    { "mesh/role",     role_set      }, /* For Node & Provisioner */
    { "mesh/net",      net_set       }, /* For Node */
    { "mesh/dkca",     dkca_set      }, /* For Node */
    { "mesh/iv",       iv_set        }, /* For Node & Provisioner */
    { "mesh/seq",      seq_set       }, /* For Node & Provisioner */
    { "mesh/rpl",      rpl_set       }, /* For Node & Provisioner */
    { "mesh/netkey",   net_key_set   }, /* For Node */
    { "mesh/appkey",   app_key_set   }, /* For Node */
    { "mesh/hb_pub",   hb_pub_set    }, /* For Node */
    { "mesh/cfg",      cfg_set       }, /* For Node */
    { "mesh/sig",      sig_mod_set   }, /* For Node & Provisioner */
    { "mesh/vnd",      vnd_mod_set   }, /* For Node & Provisioner */
#if CONFIG_BLE_MESH_LABEL_COUNT > 0
    { "mesh/vaddr",    va_set        }, /* For Node */
#endif /* CONFIG_BLE_MESH_LABEL_COUNT > 0 */
#if CONFIG_BLE_MESH_PROVISIONER
    { "mesh/p_prov",   p_prov_set    }, /* For Provisioner */
    { "mesh/p_netidx", p_net_idx_set }, /* For Provisioner */
    { "mesh/p_appidx", p_app_idx_set }, /* For Provisioner */
    { "mesh/p_netkey", p_net_key_set }, /* For Provisioner */
    { "mesh/p_appkey", p_app_key_set }, /* For Provisioner */
    { "mesh/p_node",   p_node_set    }, /* For Provisioner */
#endif /* CONFIG_BLE_MESH_PROVISIONER */
};

/**
 * For Provisioner, the load operation needs the following actions:
 * role_set:    Need, restore the device role
 * net_set:     Not needed
 * iv_set:      Need, restore the last IV Index status
 * seq_set:     Need, restore the previous sequence number
 * rpl_set:     Need, restore the previous Replay Protection List
 * net_key_set: Not needed
 * app_key_set: Not needed
 * hb_pub_set:  Not needed currently
 * cfg_set:     Not needed currently
 * sig_mod_set: Need, restore SIG models related info (app, sub, pub)
 * vnd_mod_set: Need, restore vendor models related info (app, sub, pub)
 * va_set:      Not needed currently
 */
int settings_core_load(void)
{
    int i;

    BT_DBG("SettingsCoreLoad");

    for (i = 0; i < ARRAY_SIZE(settings); i++) {
        if ((!strcmp(settings[i].name, "mesh/net") ||
            !strcmp(settings[i].name, "mesh/netkey") ||
            !strcmp(settings[i].name, "mesh/appkey") ||
            !strcmp(settings[i].name, "mesh/hb_pub") ||
            !strcmp(settings[i].name, "mesh/cfg") ||
            !strcmp(settings[i].name, "mesh/vaddr")) &&
            (!IS_ENABLED(CONFIG_BLE_MESH_NODE) || bt_mesh_is_provisioner())) {
            BT_DBG("Not restoring %s for Provisioner", settings[i].name);
            continue;
        }

        if ((!strcmp(settings[i].name, "mesh/p_prov") ||
            !strcmp(settings[i].name, "mesh/p_netidx") ||
            !strcmp(settings[i].name, "mesh/p_appidx") ||
            !strcmp(settings[i].name, "mesh/p_netkey") ||
            !strcmp(settings[i].name, "mesh/p_appkey") ||
            !strcmp(settings[i].name, "mesh/p_node")) &&
            (!IS_ENABLED(CONFIG_BLE_MESH_PROVISIONER) || bt_mesh_is_node())) {
            BT_DBG("Not restoring %s for Node", settings[i].name);
            continue;
        }

        settings[i].func(settings[i].name);

        if (!strcmp(settings[i].name, "mesh/role")) {
            uint8_t role = bt_mesh_atomic_get(bt_mesh.flags) & BLE_MESH_SETTINGS_ROLE_BIT_MASK;
            switch (role) {
            case BLE_MESH_SETTINGS_ROLE_NONE:
                BT_INFO("Mesh device just starts up, no restore");
                return 0;
            case BLE_MESH_SETTINGS_ROLE_NODE:
                BT_INFO("Restored mesh device role: Node");
                break;
            case BLE_MESH_SETTINGS_ROLE_PROV:
                BT_INFO("Restored mesh device role: Provisioner");
                break;
            default:
                BT_ERR("Restored mesh device role: Unknown");
                return -EINVAL;
            }
        }
    }

    return 0;
}

static int subnet_init(struct bt_mesh_subnet *sub)
{
    int err = 0;

    BT_DBG("SubnetInit, NetIdx 0x%04x KrPhase %u", sub->net_idx, sub->kr_phase);

    err = bt_mesh_net_keys_create(&sub->keys[0], sub->keys[0].net);
    if (err) {
        BT_ERR("Unable to generate keys for subnet");
        return -EIO;
    }

    if (sub->kr_phase != BLE_MESH_KR_NORMAL) {
        err = bt_mesh_net_keys_create(&sub->keys[1], sub->keys[1].net);
        if (err) {
            BT_ERR("Unable to generate keys for subnet");
            (void)memset(&sub->keys[0], 0, sizeof(sub->keys[0]));
            return -EIO;
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        sub->node_id = BLE_MESH_NODE_IDENTITY_STOPPED;
    } else {
        sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;
    }

    /* Make sure we have valid beacon data to be sent */
    bt_mesh_net_secure_beacon_update(sub);

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_directed_forwarding_sub_init(sub);
    bt_mesh_recovery_directed_forwarding_table(sub);
#endif /* CONFIG_BLE_MESH_DF_SRV */

    return 0;
}

static void commit_model(struct bt_mesh_model *model, struct bt_mesh_elem *elem,
                         bool vnd, bool primary, void *user_data)
{
    BT_DBG("CommitModel, Vnd %u Primary %u", vnd, primary);

    if (model->pub && model->pub->update &&
        model->pub->addr != BLE_MESH_ADDR_UNASSIGNED) {
        int32_t ms = bt_mesh_model_pub_period_get(model);
        if (ms) {
            BT_DBG("Starting publish timer (period %u ms)", ms);
            k_delayed_work_submit(&model->pub->timer, ms);
        }
    }
}

int settings_core_commit(void)
{
    struct bt_mesh_subnet *sub = NULL;
    int err = 0;
    int i;

    BT_DBG("SettingsCoreCommit");

#if CONFIG_BLE_MESH_NODE
    if (bt_mesh_is_node()) {
        if (bt_mesh.sub[0].net_idx == BLE_MESH_KEY_UNUSED) {
            /* Nothing to do since we're not yet provisioned */
            BT_INFO("NodeNotYetProvisioned");
            return 0;
        }

        BT_INFO("Settings commit, sub[0].net_idx 0x%03x", bt_mesh.sub[0].net_idx);

        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
            bt_mesh_proxy_server_prov_disable(true);
        }

        for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
            sub = &bt_mesh.sub[i];

            if (sub->net_idx == BLE_MESH_KEY_UNUSED) {
                continue;
            }

            err = subnet_init(sub);
            if (err) {
                BT_ERR("Failed to init subnet 0x%03x", sub->net_idx);
            }
        }
    }
#endif /* CONFIG_BLE_MESH_NODE */

#if CONFIG_BLE_MESH_PROVISIONER
    if (bt_mesh_is_provisioner()) {
        if (bt_mesh.p_sub[0] == NULL ||
            bt_mesh.p_sub[0]->net_idx == BLE_MESH_KEY_UNUSED) {
            return 0;
        }

        BT_INFO("Settings commit, p_sub[0]->net_idx 0x%03x", bt_mesh.p_sub[0]->net_idx);

        for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
            sub = bt_mesh.p_sub[i];

            if (sub == NULL || sub->net_idx == BLE_MESH_KEY_UNUSED) {
                continue;
            }

            err = subnet_init(sub);
            if (err) {
                BT_ERR("Failed to init subnet 0x%03x", sub->net_idx);
            }
        }
    }
#endif /* CONFIG_BLE_MESH_PROVISIONER */

    if (bt_mesh_is_node() || bt_mesh_is_provisioner()) {
        if (bt_mesh.ivu_duration < BLE_MESH_IVU_MIN_HOURS) {
            k_delayed_work_submit(&bt_mesh.ivu_timer, BLE_MESH_IVU_TIMEOUT);
        }

        bt_mesh_model_foreach(commit_model, NULL);
    }

#if CONFIG_BLE_MESH_NODE
    if (bt_mesh_is_node()) {
        struct bt_mesh_hb_pub *hb_pub = NULL;
        struct bt_mesh_cfg_srv *cfg = NULL;

        bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_VALID);

        hb_pub = bt_mesh_hb_pub_get();
        if (hb_pub && hb_pub->dst != BLE_MESH_ADDR_UNASSIGNED &&
            hb_pub->count && hb_pub->period) {
            BT_DBG("Starting heartbeat publication");
            k_delayed_work_submit(&hb_pub->timer, K_NO_WAIT);
        }

        cfg = bt_mesh_cfg_get();
        if (cfg && stored_cfg.valid) {
            cfg->net_transmit = stored_cfg.cfg.net_transmit;
            cfg->relay = stored_cfg.cfg.relay;
            cfg->relay_retransmit = stored_cfg.cfg.relay_retransmit;
            cfg->beacon = stored_cfg.cfg.beacon;
            cfg->gatt_proxy = stored_cfg.cfg.gatt_proxy;
            cfg->frnd = stored_cfg.cfg.frnd;
            cfg->default_ttl = stored_cfg.cfg.default_ttl;
        }

        bt_mesh_net_start();
    }
#endif /* CONFIG_BLE_MESH_NODE */

    return 0;
}

/* Pending flags that use K_NO_WAIT as the storage timeout */
#define NO_WAIT_PENDING_BITS    (BIT(BLE_MESH_NET_PENDING) | \
                                 BIT(BLE_MESH_IV_PENDING) | \
                                 BIT(BLE_MESH_SEQ_PENDING))

/* Pending flags that use CONFIG_BLE_MESH_STORE_TIMEOUT */
#define GENERIC_PENDING_BITS    (BIT(BLE_MESH_KEYS_PENDING) | \
                                 BIT(BLE_MESH_HB_PUB_PENDING) | \
                                 BIT(BLE_MESH_CFG_PENDING) | \
                                 BIT(BLE_MESH_MOD_PENDING))

static void schedule_store(int flag)
{
    int32_t timeout = 0, remaining = 0;

    BT_DBG("ScheduleStore, Flag %ld", flag);

    bt_mesh_atomic_set_bit(bt_mesh.flags, flag);

    /* When Node is not provisioned OR Provisioner is disabled,
     * we will directly erase the stored information.
     */
    if (!bt_mesh_is_provisioned() && !bt_mesh_is_provisioner_en()) {
        store_pending(NULL);
        return;
    }

    if (bt_mesh_atomic_get(bt_mesh.flags) & NO_WAIT_PENDING_BITS) {
        timeout = K_NO_WAIT;
    } else if (bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_RPL_PENDING) &&
               (!(bt_mesh_atomic_get(bt_mesh.flags) & GENERIC_PENDING_BITS) ||
                (CONFIG_BLE_MESH_RPL_STORE_TIMEOUT < CONFIG_BLE_MESH_STORE_TIMEOUT))) {
        timeout = K_SECONDS(CONFIG_BLE_MESH_RPL_STORE_TIMEOUT);
    } else {
        timeout = K_SECONDS(CONFIG_BLE_MESH_STORE_TIMEOUT);
    }

    remaining = k_delayed_work_remaining_get(&pending_store);
    if (remaining && remaining < timeout) {
        BT_DBG("Not rescheduling due to existing earlier deadline");
        return;
    }

    BT_INFO("Settings store, waiting %d seconds", timeout / MSEC_PER_SEC);

    if (timeout) {
        k_delayed_work_submit(&pending_store, timeout);
    } else {
        k_work_submit(&pending_store.work);
    }
}

static void clear_net(void)
{
    BT_DBG("ClearNet");

    bt_mesh_erase_core_settings("mesh/net");
}

static void store_pending_net(void)
{
    struct net_val net = {0};

    BT_DBG("StorePendingNet");
    BT_DBG("PrimaryAddr 0x%04x DevKey %s",
           bt_mesh_primary_addr(), bt_hex(bt_mesh.dev_key, 16));

    net.primary_addr = bt_mesh_primary_addr();
    memcpy(net.dev_key, bt_mesh.dev_key, 16);

    bt_mesh_save_core_settings("mesh/net", (const uint8_t *)&net, sizeof(net));
}

void bt_mesh_store_role(void)
{
    BT_DBG("StoreRole %lu", bt_mesh_atomic_get(bt_mesh.flags) & BLE_MESH_SETTINGS_ROLE_BIT_MASK);

    bt_mesh_save_core_settings("mesh/role", (const uint8_t *)bt_mesh.flags, sizeof(bt_mesh.flags));
}

void bt_mesh_store_net(void)
{
    BT_DBG("StoreNetSchedule");

    schedule_store(BLE_MESH_NET_PENDING);
}

static void store_pending_iv(void)
{
    struct iv_val iv = {0};

    BT_DBG("StorePendingIV");

    iv.iv_index = bt_mesh.iv_index;
    iv.iv_update = bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS);
    iv.iv_duration = bt_mesh.ivu_duration;

    bt_mesh_save_core_settings("mesh/iv", (const uint8_t *)&iv, sizeof(iv));
}

void bt_mesh_store_iv(bool only_duration)
{
    BT_DBG("StoreIVSchedule, OnlyDuration %u", only_duration);

    schedule_store(BLE_MESH_IV_PENDING);

    if (!only_duration) {
        /* Always update Seq whenever IV changes */
        schedule_store(BLE_MESH_SEQ_PENDING);
    }
}

static void clear_iv(void)
{
    BT_DBG("ClearIV");

    bt_mesh_erase_core_settings("mesh/iv");
}

static void store_pending_seq(void)
{
    struct seq_val seq = {0};

    BT_DBG("StorePendingSeq, Seq 0x%06x", bt_mesh.seq);

    sys_put_le24(bt_mesh.seq, seq.val);

    bt_mesh_save_core_settings("mesh/seq", (const uint8_t *)&seq, sizeof(seq));
}

void bt_mesh_store_seq(void)
{
    BT_DBG("StoreSeqSchedule, Seq 0x%06x", bt_mesh.seq);

    if (CONFIG_BLE_MESH_SEQ_STORE_RATE &&
        (bt_mesh.seq % CONFIG_BLE_MESH_SEQ_STORE_RATE)) {
        return;
    }

    schedule_store(BLE_MESH_SEQ_PENDING);
}

void bt_mesh_clear_seq(void)
{
    BT_DBG("ClearSeq");

    bt_mesh_erase_core_settings("mesh/seq");
}

static void store_rpl(struct bt_mesh_rpl *entry)
{
    struct rpl_val rpl = {0};
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("StoreRPL");
    BT_DBG("Src 0x%04x Seq 0x%06x OldIV %u", entry->src, entry->seq, entry->old_iv);

    rpl.seq = entry->seq;
    rpl.old_iv = entry->old_iv;

    sprintf(name, "mesh/rpl/%04x", entry->src);
    err = bt_mesh_save_core_settings(name, (const uint8_t *)&rpl, sizeof(rpl));
    if (err) {
        BT_ERR("Failed to store RPL entry 0x%04x", entry->src);
        return;
    }

    err = bt_mesh_add_core_settings_item("mesh/rpl", entry->src);
    if (err) {
        BT_ERR("Failed to add 0x%04x to mesh/rpl", entry->src);
    }
}

static void clear_rpl(void)
{
    struct net_buf_simple *buf = NULL;
    char name[16] = {'\0'};
    size_t length = 0U;
    uint16_t src = 0U;
    int i;

    BT_DBG("ClearRPL");

    buf = bt_mesh_get_core_settings_item("mesh/rpl");
    if (!buf) {
        bt_mesh_erase_core_settings("mesh/rpl");
        return;
    }

    length = buf->len;

    for (i = 0; i < length / SETTINGS_ITEM_SIZE; i++) {
        src = net_buf_simple_pull_le16(buf);

        if (!BLE_MESH_ADDR_IS_UNICAST(src)) {
            BT_ERR("Invalid source address 0x%04x", src);
            continue;
        }

        sprintf(name, "mesh/rpl/%04x", src);
        bt_mesh_erase_core_settings(name);
    }

    bt_mesh_erase_core_settings("mesh/rpl");

    bt_mesh_free_buf(buf);
}

static void store_pending_rpl(void)
{
    int i;

    BT_DBG("StorePendingRPL");

    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        struct bt_mesh_rpl *rpl = &bt_mesh.rpl[i];

        BT_DBG("%u: Src 0x%04x Seq 0x%06x OldIV %u Store %u",
               i, rpl->src, rpl->seq, rpl->old_iv, rpl->store);

        if (rpl->store) {
            rpl->store = false;
            store_rpl(rpl);
        }
    }
}

static void store_pending_hb_pub(void)
{
    struct bt_mesh_hb_pub *hb_pub = bt_mesh_hb_pub_get();
    struct hb_pub_val val = {0};

    BT_DBG("StorePendingHbPub");

    if (!hb_pub) {
        BT_ERR("Invalid heartbeat publication");
        return;
    }

    val.indefinite = (hb_pub->count == 0xffff);
    val.dst = hb_pub->dst;
    val.period = hb_pub->period;
    val.ttl = hb_pub->ttl;
    val.feat = hb_pub->feat;
    val.net_idx = hb_pub->net_idx;

    bt_mesh_save_core_settings("mesh/hb_pub", (const uint8_t *)&val, sizeof(val));
}

static void clear_hb_pub(void)
{
    BT_DBG("ClearHbPub");

    bt_mesh_erase_core_settings("mesh/hb_pub");
}

static void store_pending_cfg(void)
{
    struct bt_mesh_cfg_srv *cfg = bt_mesh_cfg_get();
    struct cfg_val val = {0};

    BT_DBG("StorePendingCfg");

    if (!cfg) {
        BT_WARN("NULL configuration state");
        return;
    }

    val.net_transmit = cfg->net_transmit;
    val.relay = cfg->relay;
    val.relay_retransmit = cfg->relay_retransmit;
    val.beacon = cfg->beacon;
    val.gatt_proxy = cfg->gatt_proxy;
    val.frnd = cfg->frnd;
    val.default_ttl = cfg->default_ttl;

    bt_mesh_save_core_settings("mesh/cfg", (const uint8_t *)&val, sizeof(val));
}

static void clear_cfg(void)
{
    BT_DBG("ClearCfg");

    bt_mesh_erase_core_settings("mesh/cfg");
}

static void clear_app_key(uint16_t app_idx)
{
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("ClearAppKey, AppIdx 0x%04x", app_idx);

    sprintf(name, "mesh/ak/%04x", app_idx);
    bt_mesh_erase_core_settings(name);

    err = bt_mesh_remove_core_settings_item("mesh/appkey", app_idx);
    if (err) {
        BT_ERR("Failed to remove 0x%03x from mesh/appkey", app_idx);
    }
}

static void clear_net_key(uint16_t net_idx)
{
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("ClearNetKey, NetIdx 0x%04x", net_idx);

    sprintf(name, "mesh/nk/%04x", net_idx);
    bt_mesh_erase_core_settings(name);

    err = bt_mesh_remove_core_settings_item("mesh/netkey", net_idx);
    if (err) {
        BT_ERR("Failed to remove 0x%03x from mesh/netkey", net_idx);
    }

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_clear_directed_forwarding_table_data(net_idx);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

static void store_net_key(struct bt_mesh_subnet *sub)
{
    struct net_key_val key = {0};
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("StoreNetKey, NetIdx 0x%04x KrFlag %u KrPhase %u",
           sub->net_idx, sub->kr_flag, sub->kr_phase);

    memcpy(&key.val[0], sub->keys[0].net, 16);
    memcpy(&key.val[1], sub->keys[1].net, 16);
    key.kr_flag = sub->kr_flag;
    key.kr_phase = sub->kr_phase;

    sprintf(name, "mesh/nk/%04x", sub->net_idx);
    err = bt_mesh_save_core_settings(name, (const uint8_t *)&key, sizeof(key));
    if (err) {
        BT_ERR("Failed to store NetKey 0x%03x", sub->net_idx);
        return;
    }

    err = bt_mesh_add_core_settings_item("mesh/netkey", sub->net_idx);
    if (err) {
        BT_ERR("Failed to add 0x%03x to mesh/netkey", sub->net_idx);
    }
}

static void store_app_key(struct bt_mesh_app_key *app)
{
    struct app_key_val key = {0};
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("StoreAppKey, NetIdx 0x%04x AppIdx 0x%04x Updated %u",
           app->net_idx, app->app_idx, app->updated);

    key.net_idx = app->net_idx;
    key.updated = app->updated;
    memcpy(key.val[0], app->keys[0].val, 16);
    memcpy(key.val[1], app->keys[1].val, 16);

    sprintf(name, "mesh/ak/%04x", app->app_idx);
    err = bt_mesh_save_core_settings(name, (const uint8_t *)&key, sizeof(key));
    if (err) {
        BT_ERR("Failed to store AppKey 0x%03x", app->app_idx);
        return;
    }

    err = bt_mesh_add_core_settings_item("mesh/appkey", app->app_idx);
    if (err) {
        BT_ERR("Failed to add 0x%03x to mesh/appkey", app->app_idx);
    }
}

static void store_pending_keys(void)
{
    int i;

    BT_DBG("StorePendingKeys");

    for (i = 0; i < ARRAY_SIZE(key_updates); i++) {
        struct key_update *update = &key_updates[i];

        BT_DBG("%u: KeyIdx 0x%04x Valid %u AppKey %u Clear %u",
               i, update->key_idx, update->valid, update->app_key, update->clear);

        if (!update->valid) {
            continue;
        }

        if (update->clear) {
            if (update->app_key) {
                clear_app_key(update->key_idx);
            } else {
                clear_net_key(update->key_idx);
            }
        } else {
            if (update->app_key) {
                struct bt_mesh_app_key *key = NULL;

                key = bt_mesh_app_key_get(update->key_idx);
                if (key) {
                    store_app_key(key);
                } else {
                    BT_WARN("AppKeyIndex 0x%03x not found", update->key_idx);
                }
            } else {
                struct bt_mesh_subnet *sub = NULL;

                sub = bt_mesh_subnet_get(update->key_idx);
                if (sub) {
                    store_net_key(sub);
                } else {
                    BT_WARN("NetKeyIndex 0x%03x not found", update->key_idx);
                }
            }
        }

        update->valid = 0U;
    }
}

static void store_pending_mod_bind(struct bt_mesh_model *model, bool vnd)
{
    char name[16] = {'\0'};
    uint16_t model_key = 0U;
    int err = 0;

    BT_DBG("StorePendingModBind, Vnd %u", vnd);

    model_key = BLE_MESH_GET_MODEL_KEY(model->elem_idx, model->model_idx);
    sprintf(name, "mesh/%s/%04x/b", vnd ? "v" : "s", model_key);

    err = bt_mesh_save_core_settings(name, (const uint8_t *)model->keys, sizeof(model->keys));
    if (err) {
        BT_ERR("Failed to store %s", name);
        return;
    }

    err = bt_mesh_add_core_settings_item(vnd ? "mesh/vnd" : "mesh/sig", model_key);
    if (err) {
        BT_ERR("Failed to add bound key to %s, model_key 0x%04x",
               vnd ? "mesh/vnd" : "mesh/sig", model_key);
    }
}

static void store_pending_mod_sub(struct bt_mesh_model *model, bool vnd)
{
    char name[16] = {'\0'};
    uint16_t model_key = 0U;
    int err = 0;

    BT_DBG("StorePendingModSub, Vnd %u", vnd);

    model_key = BLE_MESH_GET_MODEL_KEY(model->elem_idx, model->model_idx);
    sprintf(name, "mesh/%s/%04x/s", vnd ? "v" : "s", model_key);

    err = bt_mesh_save_core_settings(name, (const uint8_t *)model->groups, sizeof(model->groups));
    if (err) {
        BT_ERR("Failed to store %s", name);
        return;
    }

    err = bt_mesh_add_core_settings_item(vnd ? "mesh/vnd" : "mesh/sig", model_key);
    if (err) {
        BT_ERR("Failed to add subscription to %s, model_key 0x%04x",
               vnd ? "mesh/vnd" : "mesh/sig", model_key);
    }
}

static void store_pending_mod_pub(struct bt_mesh_model *model, bool vnd)
{
    struct mod_pub_val pub = {0};
    char name[16] = {'\0'};
    uint16_t model_key = 0U;
    int err = 0;

    BT_DBG("StorePendingModPub, Vnd %u", vnd);

    if (!model->pub) {
        BT_WARN("Model has no publication support");
        return;
    }

    model_key = BLE_MESH_GET_MODEL_KEY(model->elem_idx, model->model_idx);
    sprintf(name, "mesh/%s/%04x/p", vnd ? "v" : "s", model_key);

    pub.addr = model->pub->addr;
    pub.key = model->pub->key;
    pub.ttl = model->pub->ttl;
    pub.retransmit = model->pub->retransmit;
    pub.period = model->pub->period;
    pub.period_div = model->pub->period_div;
    pub.cred = model->pub->cred;

#if CONFIG_BLE_MESH_DF_SRV
    pub.directed_pub_policy = model->pub->directed_pub_policy; /**< Directed publish policy */
#endif /* CONFIG_BLE_MESH_DF_SRV */

    err = bt_mesh_save_core_settings(name, (const uint8_t *)&pub, sizeof(pub));
    if (err) {
        BT_ERR("Failed to store %s", name);
        return;
    }

    err = bt_mesh_add_core_settings_item(vnd ? "mesh/vnd" : "mesh/sig", model_key);
    if (err) {
        BT_ERR("Failed to add publication to %s, model_key 0x%04x",
               vnd ? "mesh/vnd" : "mesh/sig", model_key);
    }
}

static void store_pending_mod(struct bt_mesh_model *model,
                              struct bt_mesh_elem *elem, bool vnd,
                              bool primary, void *user_data)
{
    BT_DBG("StorePendingMod, Flags %u Vnd %u", model->flags, vnd);

    if (!model->flags) {
        return;
    }

    if (model->flags & BLE_MESH_MOD_BIND_PENDING) {
        model->flags &= ~BLE_MESH_MOD_BIND_PENDING;
        store_pending_mod_bind(model, vnd);
    }

    if (model->flags & BLE_MESH_MOD_SUB_PENDING) {
        model->flags &= ~BLE_MESH_MOD_SUB_PENDING;
        store_pending_mod_sub(model, vnd);
    }

    if (model->flags & BLE_MESH_MOD_PUB_PENDING) {
        model->flags &= ~BLE_MESH_MOD_PUB_PENDING;
        store_pending_mod_pub(model, vnd);
    }
}

static void clear_mod_bind(struct bt_mesh_model *model, bool vnd)
{
    char name[16] = {'\0'};
    uint16_t model_key = 0U;

    BT_DBG("ClearModBind, Vnd %u", vnd);

    model_key = BLE_MESH_GET_MODEL_KEY(model->elem_idx, model->model_idx);
    sprintf(name, "mesh/%s/%04x/b", vnd ? "v" : "s", model_key);

    bt_mesh_erase_core_settings(name);
    bt_mesh_remove_core_settings_item(vnd ? "mesh/vnd" : "mesh/sig", model_key);
}

static void clear_mod_sub(struct bt_mesh_model *model, bool vnd)
{
    char name[16] = {'\0'};
    uint16_t model_key = 0U;

    BT_DBG("ClearModSub, Vnd %u", vnd);

    model_key = BLE_MESH_GET_MODEL_KEY(model->elem_idx, model->model_idx);
    sprintf(name, "mesh/%s/%04x/s", vnd ? "v" : "s", model_key);

    bt_mesh_erase_core_settings(name);
    bt_mesh_remove_core_settings_item(vnd ? "mesh/vnd" : "mesh/sig", model_key);
}

static void clear_mod_pub(struct bt_mesh_model *model, bool vnd)
{
    uint16_t model_key = 0U;
    char name[16] = {'\0'};

    BT_DBG("ClearModPub, Vnd %u", vnd);

    model_key = BLE_MESH_GET_MODEL_KEY(model->elem_idx, model->model_idx);
    sprintf(name, "mesh/%s/%04x/p", vnd ? "v" : "s", model_key);

    bt_mesh_erase_core_settings(name);
    bt_mesh_remove_core_settings_item(vnd ? "mesh/vnd" : "mesh/sig", model_key);
}

static void clear_pending_mod(struct bt_mesh_model *model,
                              struct bt_mesh_elem *elem, bool vnd,
                              bool primary, void *user_data)
{
    BT_DBG("ClearPendingMod, Flags 0x%04x Vnd %u", model->flags, vnd);

    if (!model->flags) {
        return;
    }

    if (model->flags & BLE_MESH_MOD_BIND_PENDING) {
        model->flags &= ~BLE_MESH_MOD_BIND_PENDING;
        clear_mod_bind(model, vnd);
    }

    if (model->flags & BLE_MESH_MOD_SUB_PENDING) {
        model->flags &= ~BLE_MESH_MOD_SUB_PENDING;
        clear_mod_sub(model, vnd);
    }

    if (model->flags & BLE_MESH_MOD_PUB_PENDING) {
        model->flags &= ~BLE_MESH_MOD_PUB_PENDING;
        clear_mod_pub(model, vnd);
    }
}

#define IS_VA_DEL(_label)   ((_label)->ref == 0)
static void store_pending_va(void)
{
    struct label *lab = NULL;
    struct va_val va = {0};
    char name[16] = {'\0'};
    uint16_t i = 0U;
    int err = 0;

    BT_DBG("StorePendingVa");

    for (i = 0U; (lab = get_label(i)) != NULL; i++) {
        if (!bt_mesh_atomic_test_and_clear_bit(lab->flags,
            BLE_MESH_VA_CHANGED)) {
            continue;
        }

        sprintf(name, "mesh/va/%04x", i);

        if (IS_VA_DEL(lab)) {
            err = bt_mesh_erase_core_settings(name);
        } else {
            va.ref = lab->ref;
            va.addr = lab->addr;
            memcpy(va.uuid, lab->uuid, 16);
            err = bt_mesh_save_core_settings(name, (const uint8_t *)&va, sizeof(va));
        }
        if (err) {
            BT_ERR("Failed to %s virtual address %s",
                   IS_VA_DEL(lab) ? "delete" : "store", name);
            return;
        }

        if (IS_VA_DEL(lab)) {
            err = bt_mesh_remove_core_settings_item("mesh/vaddr", i);
        } else {
            err = bt_mesh_add_core_settings_item("mesh/vaddr", i);
        }
        if (err) {
            BT_ERR("Failed to %s 0x%04x in mesh/vaddr",
                   IS_VA_DEL(lab) ? "delete" : "store", i);
            return;
        }

        BT_DBG("%s virtual address 0x%04x", IS_VA_DEL(lab) ? "Delete" : "Store", i);
    }
}

static void store_pending(struct k_work *work)
{
    BT_DBG("StorePending");

    if (bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_RPL_PENDING)) {
        if (bt_mesh_is_provisioned() || bt_mesh_is_provisioner_en()) {
            store_pending_rpl();
        } else {
            clear_rpl();
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node() &&
        bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_KEYS_PENDING)) {
        store_pending_keys();
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node() &&
        bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_NET_PENDING)) {
        if (bt_mesh_is_provisioned()) {
            store_pending_net();
        } else {
            clear_net();
        }
    }

    if (bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_IV_PENDING)) {
        if (bt_mesh_is_provisioned() || bt_mesh_is_provisioner_en()) {
            store_pending_iv();
        } else {
            clear_iv();
        }
    }

    if (bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_SEQ_PENDING)) {
        if (bt_mesh_is_provisioned() || bt_mesh_is_provisioner_en()) {
            store_pending_seq();
        } else {
            bt_mesh_clear_seq();
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node() &&
        bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_HB_PUB_PENDING)) {
        if (bt_mesh_is_provisioned()) {
            store_pending_hb_pub();
        } else {
            clear_hb_pub();
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node() &&
        bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_CFG_PENDING)) {
        if (bt_mesh_is_provisioned()) {
            store_pending_cfg();
        } else {
            clear_cfg();
        }
    }

    if (bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_MOD_PENDING)) {
        if (bt_mesh_is_provisioned() || bt_mesh_is_provisioner_en()) {
            bt_mesh_model_foreach(store_pending_mod, NULL);
        } else {
            bt_mesh_model_foreach(clear_pending_mod, NULL);

            bt_mesh_erase_core_settings("mesh/sig");
            bt_mesh_erase_core_settings("mesh/vnd");
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_NODE) && bt_mesh_is_node() &&
        bt_mesh_atomic_test_and_clear_bit(bt_mesh.flags, BLE_MESH_VA_PENDING)) {
        store_pending_va();
    }
}

void bt_mesh_store_rpl(struct bt_mesh_rpl *entry)
{
    BT_DBG("StoreRPLSchedule");
    BT_DBG("Src 0x%04x Seq 0x%06x OldIV %u", entry->src, entry->seq, entry->old_iv);

    entry->store = true;
    schedule_store(BLE_MESH_RPL_PENDING);
}

static struct key_update *key_update_find(bool app_key, uint16_t key_idx,
                                          struct key_update **free_slot)
{
    struct key_update *match = NULL;
    int i;

    *free_slot = NULL;

    BT_DBG("KeyUpdateFind, KeyIdx 0x%04x AppKey %u", key_idx, app_key);

    for (i = 0; i < ARRAY_SIZE(key_updates); i++) {
        struct key_update *update = &key_updates[i];

        if (!update->valid) {
            *free_slot = update;
            continue;
        }

        if (update->app_key != app_key) {
            continue;
        }

        if (update->key_idx == key_idx) {
            match = update;
        }
    }

    return match;
}

void bt_mesh_store_subnet(struct bt_mesh_subnet *sub)
{
    struct key_update *free_slot = NULL;
    struct key_update *update = NULL;

    BT_DBG("StoreSubnetSchedule, NetIdx 0x%04x", sub->net_idx);

    update = key_update_find(false, sub->net_idx, &free_slot);
    if (update) {
        update->clear = 0U;
        schedule_store(BLE_MESH_KEYS_PENDING);
        return;
    }

    if (!free_slot) {
        BT_DBG("NoFreeSlotForSubnet");
        store_net_key(sub);
        return;
    }

    free_slot->valid = 1U;
    free_slot->key_idx = sub->net_idx;
    free_slot->app_key = 0U;
    free_slot->clear = 0U;

    schedule_store(BLE_MESH_KEYS_PENDING);
}

void bt_mesh_store_app_key(struct bt_mesh_app_key *key)
{
    struct key_update *free_slot = NULL;
    struct key_update *update = NULL;

    BT_DBG("StoreAppKeySchedule, AppIdx 0x%04x", key->app_idx);

    update = key_update_find(true, key->app_idx, &free_slot);
    if (update) {
        update->clear = 0U;
        schedule_store(BLE_MESH_KEYS_PENDING);
        return;
    }

    if (!free_slot) {
        BT_DBG("NoFreeSlotForAppKey");
        store_app_key(key);
        return;
    }

    free_slot->valid = 1U;
    free_slot->key_idx = key->app_idx;
    free_slot->app_key = 1U;
    free_slot->clear = 0U;

    schedule_store(BLE_MESH_KEYS_PENDING);
}

void bt_mesh_store_hb_pub(void)
{
    BT_DBG("StoreHbPubSchedule");

    schedule_store(BLE_MESH_HB_PUB_PENDING);
}

void bt_mesh_store_cfg(void)
{
    BT_DBG("StoreCfgSchedule");

    schedule_store(BLE_MESH_CFG_PENDING);
}

void bt_mesh_clear_role(void)
{
    BT_DBG("ClearRole");

    bt_mesh_erase_core_settings("mesh/role");
}

void bt_mesh_clear_net(void)
{
    BT_DBG("ClearNetSchedule");

    schedule_store(BLE_MESH_NET_PENDING);
    schedule_store(BLE_MESH_IV_PENDING);
    schedule_store(BLE_MESH_CFG_PENDING);
}

void bt_mesh_clear_subnet(struct bt_mesh_subnet *sub)
{
    struct key_update *free_slot = NULL;
    struct key_update *update = NULL;

    BT_DBG("ClearSubnetSchedule, NetIdx 0x%04x", sub->net_idx);

    update = key_update_find(false, sub->net_idx, &free_slot);
    if (update) {
        update->clear = 1U;
        schedule_store(BLE_MESH_KEYS_PENDING);
        return;
    }

    if (!free_slot) {
        clear_net_key(sub->net_idx);
        return;
    }

    free_slot->valid = 1U;
    free_slot->key_idx = sub->net_idx;
    free_slot->app_key = 0U;
    free_slot->clear = 1U;

    schedule_store(BLE_MESH_KEYS_PENDING);
}

void bt_mesh_clear_app_key(struct bt_mesh_app_key *key)
{
    struct key_update *free_slot = NULL;
    struct key_update *update = NULL;

    BT_DBG("ClearAppKeySchedule, AppIdx 0x%04x", key->app_idx);

    update = key_update_find(true, key->app_idx, &free_slot);
    if (update) {
        update->clear = 1U;
        schedule_store(BLE_MESH_KEYS_PENDING);
        return;
    }

    if (!free_slot) {
        clear_app_key(key->app_idx);
        return;
    }

    free_slot->valid = 1U;
    free_slot->key_idx = key->app_idx;
    free_slot->app_key = 1U;
    free_slot->clear = 1U;

    schedule_store(BLE_MESH_KEYS_PENDING);
}

void bt_mesh_clear_rpl_single(uint16_t src)
{
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("ClearRPLSingle, Src 0x%04x", src);

    if (!BLE_MESH_ADDR_IS_UNICAST(src)) {
        BT_ERR("Invalid src 0x%04x", src);
        return;
    }

    sprintf(name, "mesh/rpl/%04x", src);
    bt_mesh_erase_core_settings(name);

    err = bt_mesh_remove_core_settings_item("mesh/rpl", src);
    if (err) {
        BT_ERR("Failed to remove 0x%04x from mesh/rpl", src);
    }
}

void bt_mesh_clear_rpl(void)
{
    BT_DBG("ClearRPLSchedule");

    clear_rpl();
}

void bt_mesh_store_mod_bind(struct bt_mesh_model *model)
{
    BT_DBG("StoreModBindSchedule");

    model->flags |= BLE_MESH_MOD_BIND_PENDING;
    schedule_store(BLE_MESH_MOD_PENDING);
}

void bt_mesh_store_mod_sub(struct bt_mesh_model *model)
{
    BT_DBG("StoreModSubSchedule");

    model->flags |= BLE_MESH_MOD_SUB_PENDING;
    schedule_store(BLE_MESH_MOD_PENDING);
}

void bt_mesh_store_mod_pub(struct bt_mesh_model *model)
{
    BT_DBG("StoreModPubSchedule");

    model->flags |= BLE_MESH_MOD_PUB_PENDING;
    schedule_store(BLE_MESH_MOD_PENDING);
}

void bt_mesh_store_label(void)
{
    BT_DBG("StoreLabelSchedule");

    schedule_store(BLE_MESH_VA_PENDING);
}

void bt_mesh_store_dkca(void)
{
    BT_DBG("StoreDkca");

    bt_mesh_save_core_settings("mesh/dkca", bt_mesh.dev_key_ca, sizeof(bt_mesh.dev_key_ca));
}

void bt_mesh_clear_dkca(void)
{
    BT_DBG("ClearDkca");

    bt_mesh_erase_core_settings("mesh/dkca");
}

#if CONFIG_BLE_MESH_PROVISIONER
/**
 * key: "mesh/p_prov"   -> write/read to set/get prov_ctx.curr_addr
 * key: "mesh/p_netidx" -> write/read to set/get bt_mesh.p_net_idx_next
 * key: "mesh/p_appidx" -> write/read to set/get bt_mesh.p_app_idx_next
 * key: "mesh/p_netkey" -> write/read to set/get all Provisioner NetKey Index
 *      key: "mesh/pnk/xxxx" -> write/read to set/get the "xxxx" NetKey
 * key: "mesh/p_appkey" -> write/read to set/get all Provisioner AppKey Index
 *      key: "mesh/pak/xxxx" -> write/read to set/get the "xxxx" AppKey
 * key: "mesh/p_node"   -> write/read to set/get all self-provisioned nodes info
 *      key: "mesh/pn/xxxx/i" -> write/read to set/get the "xxxx" provisioned node info
 *      key: "mesh/pn/xxxx/n" -> write/read to set/get the "xxxx" provisioned node name
 *      key: "mesh/pn/xxxx/c" -> write/read to set/get the "xxxx" provisioned node composition data
 */
void bt_mesh_store_prov_info(uint16_t primary_addr, uint16_t alloc_addr)
{
    struct prov_info val = {0};

    BT_DBG("StoreProvInfo, PrimaryAddr 0x%04x AllocAddr 0x%04x", primary_addr, alloc_addr);

    val.primary_addr = primary_addr;
    val.alloc_addr = alloc_addr;

    bt_mesh_save_core_settings("mesh/p_prov", (const uint8_t *)&val, sizeof(val));
}

void bt_mesh_clear_prov_info(void)
{
    BT_DBG("ClearProvInfo");

    bt_mesh_erase_core_settings("mesh/p_prov");
}

static void store_p_net_key(struct bt_mesh_subnet *sub)
{
    struct net_key_val key = {0};
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("StorePvnrNetKey, NetIdx 0x%04x KrFlag %u KrPhase %u",
           sub->net_idx, sub->kr_flag, sub->kr_phase);

    memcpy(&key.val[0], sub->keys[0].net, 16);
    memcpy(&key.val[1], sub->keys[1].net, 16);
    key.kr_flag = sub->kr_flag;
    key.kr_phase = sub->kr_phase;

    sprintf(name, "mesh/pnk/%04x", sub->net_idx);
    err = bt_mesh_save_core_settings(name, (const uint8_t *)&key, sizeof(key));
    if (err) {
        BT_ERR("Failed to store NetKey 0x%03x", sub->net_idx);
        return;
    }

    err = bt_mesh_add_core_settings_item("mesh/p_netkey", sub->net_idx);
    if (err) {
        BT_ERR("Failed to add 0x%03x to mesh/p_netkey", sub->net_idx);
    }
}

static void store_p_app_key(struct bt_mesh_app_key *app)
{
    struct app_key_val key = {0};
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("StorePvnrAppKey, NetIdx 0x%04x AppIdx 0x%04x Updated %u",
           app->net_idx, app->app_idx, app->updated);

    key.net_idx = app->net_idx;
    key.updated = app->updated;
    memcpy(key.val[0], app->keys[0].val, 16);
    memcpy(key.val[1], app->keys[1].val, 16);

    sprintf(name, "mesh/pak/%04x", app->app_idx);
    err = bt_mesh_save_core_settings(name, (const uint8_t *)&key, sizeof(key));
    if (err) {
        BT_ERR("Failed to store AppKey 0x%03x", app->app_idx);
        return;
    }

    err = bt_mesh_add_core_settings_item("mesh/p_appkey", app->app_idx);
    if (err) {
        BT_ERR("Failed to add 0x%03x to mesh/p_appkey", app->app_idx);
    }
}

void bt_mesh_store_p_net_idx(void)
{
    BT_DBG("StorePvnrNetIdx, NetIdxNext 0x%04x", bt_mesh.p_net_idx_next);

    bt_mesh_save_core_settings("mesh/p_netidx",
        (const uint8_t *)&bt_mesh.p_net_idx_next, sizeof(bt_mesh.p_net_idx_next));
}

void bt_mesh_clear_p_net_idx(void)
{
    BT_DBG("ClearPvnrNetIdx");

    bt_mesh_erase_core_settings("mesh/p_netidx");
}

void bt_mesh_store_p_app_idx(void)
{
    BT_DBG("StorePvnrAppIdx, AppIdxNext 0x%04x", bt_mesh.p_app_idx_next);

    bt_mesh_save_core_settings("mesh/p_appidx",
        (const uint8_t *)&bt_mesh.p_app_idx_next, sizeof(bt_mesh.p_app_idx_next));
}

void bt_mesh_clear_p_app_idx(void)
{
    BT_DBG("ClearPvnrAppIdx");

    bt_mesh_erase_core_settings("mesh/p_appidx");
}

void bt_mesh_store_p_subnet(struct bt_mesh_subnet *sub)
{
    BT_DBG("StorePvnrSubnet");

    if (sub == NULL) {
        BT_ERR("Invalid subnet");
        return;
    }

    BT_DBG("NetIdx 0x%04x NetKey %s",
           sub->net_idx, bt_hex(sub->keys[0].net, 16));

    store_p_net_key(sub);
}

void bt_mesh_store_p_app_key(struct bt_mesh_app_key *key)
{
    BT_DBG("StorePvnrAppKey");

    if (key == NULL) {
        BT_ERR("Invalid AppKey");
        return;
    }

    BT_DBG("AppIdx 0x%03x AppKey %s",
           key->app_idx, bt_hex(key->keys[0].val, 16));

    store_p_app_key(key);
}

void bt_mesh_clear_p_subnet(uint16_t net_idx)
{
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("ClearPvnrSubnet, NetIdx 0x%04x", net_idx);

    sprintf(name, "mesh/pnk/%04x", net_idx);
    bt_mesh_erase_core_settings(name);

    err = bt_mesh_remove_core_settings_item("mesh/p_netkey", net_idx);
    if (err) {
        BT_ERR("Failed to remove 0x%04x from mesh/p_netkey", net_idx);
    }
}

void bt_mesh_clear_p_app_key(uint16_t app_idx)
{
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("ClearPvnrAppKey, AppIdx 0x%04x", app_idx);

    sprintf(name, "mesh/pak/%04x", app_idx);
    bt_mesh_erase_core_settings(name);

    err = bt_mesh_remove_core_settings_item("mesh/p_appkey", app_idx);
    if (err) {
        BT_ERR("Failed to remove 0x%04x from mesh/p_appkey", app_idx);
    }
}

void bt_mesh_store_node_info(struct bt_mesh_node *node)
{
    struct node_info val = {0};
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("StoreNodeInfo");

    if (node == NULL) {
        BT_ERR("Invalid node info");
        return;
    }

    BT_DBG("UnicastAddr 0x%04x ElemNum %u NetIdx 0x%04x Flags %u IVIndex %lu",
           node->unicast_addr, node->element_num,
           node->net_idx, node->flags, node->iv_index);

    memcpy(val.addr, node->addr, BLE_MESH_ADDR_LEN);
    val.addr_type = node->addr_type;
    memcpy(val.dev_uuid, node->dev_uuid, 16);
    val.oob_info = node->oob_info;
    val.unicast_addr = node->unicast_addr;
    val.element_num = node->element_num;
    val.net_idx = node->net_idx;
    val.flags = node->flags;
    val.iv_index = node->iv_index;
    memcpy(val.dev_key, node->dev_key, 16);

    sprintf(name, "mesh/pn/%04x/i", node->unicast_addr);
    err = bt_mesh_save_core_settings(name, (const uint8_t *)&val, sizeof(val));
    if (err) {
        BT_ERR("Failed to store node 0x%04x info", node->unicast_addr);
        return;
    }

    err = bt_mesh_add_core_settings_item("mesh/p_node", node->unicast_addr);
    if (err) {
        BT_ERR("Failed to add node 0x%04x info", node->unicast_addr);
    }
}

static void clear_node(uint16_t addr)
{
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("ClearNode, Addr 0x%04x", addr);

    /* Clear node information */
    sprintf(name, "mesh/pn/%04x/i", addr);
    bt_mesh_erase_core_settings(name);

    /* Clear node name */
    sprintf(name, "mesh/pn/%04x/n", addr);
    bt_mesh_erase_core_settings(name);

    /* Clear node composition data */
    sprintf(name, "mesh/pn/%04x/c", addr);
    bt_mesh_erase_core_settings(name);

    err = bt_mesh_remove_core_settings_item("mesh/p_node", addr);
    if (err) {
        BT_ERR("Failed to remove node 0x%04x", addr);
    }
}

void bt_mesh_clear_node_info(uint16_t unicast_addr)
{
    BT_DBG("ClearNodeInfo, Addr 0x%04x", unicast_addr);

    if (!BLE_MESH_ADDR_IS_UNICAST(unicast_addr)) {
        BT_ERR("Invalid unicast address 0x%04x", unicast_addr);
        return;
    }

    clear_node(unicast_addr);
}

void bt_mesh_store_node_name(struct bt_mesh_node *node)
{
    char node_name[BLE_MESH_NODE_NAME_SIZE + 1] = {0};
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("StoreNodeName");

    if (node == NULL) {
        BT_ERR("Invalid node info");
        return;
    }

    BT_DBG("UnicastAddr 0x%04x NodeName %s", node->unicast_addr, node->name);

    strncpy(node_name, node->name, BLE_MESH_NODE_NAME_SIZE + 1);

    sprintf(name, "mesh/pn/%04x/n", node->unicast_addr);
    err = bt_mesh_save_core_settings(name, (const uint8_t *)node_name, BLE_MESH_NODE_NAME_SIZE);
    if (err) {
        BT_ERR("Failed to store node 0x%04x name", node->unicast_addr);
    }
}

void bt_mesh_store_node_comp_data(struct bt_mesh_node *node)
{
    char name[16] = {'\0'};
    int err = 0;

    BT_DBG("StoreNodeCompData");

    if (!node || !node->comp_data || node->comp_length == 0U) {
        BT_ERR("Invalid node info");
        return;
    }

    BT_DBG("UnicastAddr 0x%04x CompLength %u", node->unicast_addr, node->comp_length);

    sprintf(name, "mesh/pn/%04x/c", node->unicast_addr);
    err = bt_mesh_save_core_settings(name, (const uint8_t *)node->comp_data, node->comp_length);
    if (err) {
        BT_ERR("Failed to store node 0x%04x comp data", node->unicast_addr);
    }
}
#endif /* CONFIG_BLE_MESH_PROVISIONER */

int settings_core_init(void)
{
    BT_DBG("SettingsCoreInit");

    return k_delayed_work_init(&pending_store, store_pending);
}

int bt_mesh_settings_init(void)
{
    BT_DBG("SettingsInit");

    bt_mesh_settings_mutex_new();
    bt_mesh_settings_init_foreach();
    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
int settings_core_deinit(void)
{
    BT_DBG("SettingsCoreDeinit");

    k_delayed_work_free(&pending_store);
    return 0;
}

int settings_core_erase(void)
{
    BT_DBG("SettingsCoreErase");

    /* Erase here must not use the pending_store timer.
     * This is used for erasing the information which
     * could not be erased during the previous deinit
     * operations.
     */
    bt_mesh_clear_net();
    bt_mesh_clear_seq();
    bt_mesh_clear_role();
    return 0;
}

int bt_mesh_settings_deinit(bool erase)
{
    BT_DBG("SettingsDeinit, Erase %u", erase);

    bt_mesh_settings_deinit_foreach(erase);
    bt_mesh_settings_mutex_free();
    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

void bt_mesh_settings_reset(bool erase)
{
    BT_DBG("SettingsReset, Erase %u", erase);

    k_delayed_work_cancel(&pending_store);
    if (erase) {
        bt_mesh_clear_net();
        bt_mesh_clear_seq();
        bt_mesh_clear_role();
    }
}

#endif /* CONFIG_BLE_MESH_SETTINGS */

#define SETTINGS_MAX_DIR_DEPTH 8

int bt_mesh_model_data_store(const struct bt_mesh_model *mod, bool vnd,
                             const char *name, const void *data,
                             size_t data_len)
{
    int err = 0;

    char path[30] = {'\0'};
    uint16_t model_key = 0U;

    model_key = BLE_MESH_GET_MODEL_KEY(mod->elem_idx, mod->model_idx);
    sprintf(path, "mesh/%s/%04x/d", vnd ? "v" : "s", model_key);
    if (name) {
        strcat(path, "/");
        strncat(path, name, SETTINGS_MAX_DIR_DEPTH);
    }

    if (data_len) {
        err = bt_mesh_save_core_settings(path, data, data_len);
        if (err) {
            BT_ERR("Failed to store %s", path);
            return err;
        }

        err = bt_mesh_add_core_settings_item(vnd ? "mesh/vnd" : "mesh/sig", model_key);
        if (err) {
            BT_ERR("Failed to add model data to %s, model_key 0x%04x",
                vnd ? "mesh/vnd" : "mesh/sig", model_key);
        }
    } else {
        bt_mesh_erase_core_settings(path);
        bt_mesh_remove_core_settings_item(vnd ? "mesh/vnd" : "mesh/sig", model_key);
    }
    return err;
}
