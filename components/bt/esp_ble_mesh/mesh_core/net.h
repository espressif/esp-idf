/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _NET_H_
#define _NET_H_

#include "mesh_access.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_NET_FLAG_KR       BIT(0)
#define BLE_MESH_NET_FLAG_IVU      BIT(1)

#define BLE_MESH_KR_NORMAL         0x00
#define BLE_MESH_KR_PHASE_1        0x01
#define BLE_MESH_KR_PHASE_2        0x02
#define BLE_MESH_KR_PHASE_3        0x03

#define BLE_MESH_IV_UPDATE(flags)   ((flags >> 1) & 0x01)
#define BLE_MESH_KEY_REFRESH(flags) (flags & 0x01)

/* How many hours in between updating IVU duration */
#define BLE_MESH_IVU_MIN_HOURS      96
#define BLE_MESH_IVU_HOURS          (BLE_MESH_IVU_MIN_HOURS / CONFIG_BLE_MESH_IVU_DIVIDER)
#define BLE_MESH_IVU_TIMEOUT        K_HOURS(BLE_MESH_IVU_HOURS)

struct bt_mesh_app_key {
    uint16_t net_idx;
    uint16_t app_idx;
    bool     updated;
    struct bt_mesh_app_keys {
        uint8_t id;
        uint8_t val[16];
    } keys[2];
};

struct bt_mesh_subnet {
    uint32_t beacon_sent;       /* Timestamp of last sent beacon */
    uint8_t  beacons_last;      /* Number of beacons during last observation window. */
    uint8_t  beacons_cur;       /* Number of beacons observed during currently ongoing window. */

    uint8_t  beacon_cache[21];  /* Cached last authenticated beacon */

    uint16_t net_idx;           /* NetKeyIndex */

    bool     kr_flag;           /* Key Refresh Flag */
    uint8_t  kr_phase;          /* Key Refresh Phase */

    uint8_t  node_id;           /* Node Identity State */
    uint32_t node_id_start;     /* Node Identity started timestamp */

    uint8_t  auth[8];           /* Beacon Authentication Value */

    struct bt_mesh_subnet_keys {
        uint8_t net[16];        /* NetKey */
        uint8_t nid;            /* NID */
        uint8_t enc[16];        /* EncKey */
        uint8_t net_id[8];      /* Network ID */
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
        uint8_t identity[16];   /* IdentityKey */
#endif
        uint8_t privacy[16];    /* PrivacyKey */
        uint8_t beacon[16];     /* BeaconKey */
    } keys[2];
};

struct bt_mesh_rpl {
    uint16_t src;
    bool     old_iv;
#if defined(CONFIG_BLE_MESH_SETTINGS)
    bool     store;
#endif
    uint32_t seq;
};

#if defined(CONFIG_BLE_MESH_FRIEND)
#define FRIEND_SEG_RX           CONFIG_BLE_MESH_FRIEND_SEG_RX
#define FRIEND_SUB_LIST_SIZE    CONFIG_BLE_MESH_FRIEND_SUB_LIST_SIZE
#else
#define FRIEND_SEG_RX           0
#define FRIEND_SUB_LIST_SIZE    0
#endif

struct bt_mesh_friend {
    uint16_t lpn;
    uint8_t  recv_delay;
    uint8_t  fsn:1,
             send_last:1,
             pending_req:1,
             pending_buf:1,
             valid:1,
             established:1;
    int32_t  poll_to;
    uint8_t  num_elem;
    uint16_t lpn_counter;
    uint16_t counter;

    uint16_t net_idx;

    uint16_t sub_list[FRIEND_SUB_LIST_SIZE];

    struct k_delayed_work timer;

    struct bt_mesh_friend_seg {
        sys_slist_t queue;

        /* The target number of segments, i.e. not necessarily
         * the current number of segments, in the queue. This is
         * used for Friend Queue free space calculations.
         */
        uint8_t seg_count;
    } seg[FRIEND_SEG_RX];

    struct net_buf *last;

    sys_slist_t queue;
    uint32_t    queue_size;

    /* Friend Clear Procedure */
    struct {
        uint32_t start;               /* Clear Procedure start */
        uint16_t frnd;                /* Previous Friend's address */
        uint16_t repeat_sec;          /* Repeat timeout in seconds */
        struct k_delayed_work timer;  /* Repeat timer */
    } clear;
};

#if defined(CONFIG_BLE_MESH_LOW_POWER)
#define LPN_GROUPS CONFIG_BLE_MESH_LPN_GROUPS
#else
#define LPN_GROUPS 0
#endif

/* Low Power Node state */
struct bt_mesh_lpn {
    enum __packed {
        BLE_MESH_LPN_DISABLED,     /* LPN feature is disabled */
        BLE_MESH_LPN_CLEAR,        /* Clear in progress */
        BLE_MESH_LPN_TIMER,        /* Waiting for auto timer expiry */
        BLE_MESH_LPN_ENABLED,      /* LPN enabled, but no Friend */
        BLE_MESH_LPN_REQ_WAIT,     /* Wait before scanning for offers */
        BLE_MESH_LPN_WAIT_OFFER,   /* Friend Req sent */
        BLE_MESH_LPN_ESTABLISHED,  /* Friendship established */
        BLE_MESH_LPN_RECV_DELAY,   /* Poll sent, waiting ReceiveDelay */
        BLE_MESH_LPN_WAIT_UPDATE,  /* Waiting for Update or message */
        BLE_MESH_LPN_OFFER_RECV,   /* Friend offer received */
    } state;

    /* Transaction Number (used for subscription list) */
    uint8_t xact_next;
    uint8_t xact_pending;
    uint8_t sent_req;

    /* Address of our Friend when we're a LPN. Unassigned if we don't
     * have a friend yet.
     */
    uint16_t frnd;

    /* Value from the friend offer */
    uint8_t  recv_win;

    uint8_t  req_attempts;     /* Number of Request attempts */

    int32_t  poll_timeout;

    uint8_t  groups_changed: 1, /* Friend Subscription List needs updating */
             pending_poll: 1,   /* Poll to be sent after subscription */
             disable: 1,        /* Disable LPN after clearing */
             fsn: 1,            /* Friend Sequence Number */
             established: 1,    /* Friendship established */
             clear_success: 1;  /* Friend Clear Confirm received */

    /* Friend Queue Size */
    uint8_t  queue_size;

    /* LPNCounter */
    uint16_t counter;

    /* Previous Friend of this LPN */
    uint16_t old_friend;

    /* Duration reported for last advertising packet */
    uint16_t adv_duration;

    /* Next LPN related action timer */
    struct k_delayed_work timer;

    /* Subscribed groups */
    uint16_t groups[LPN_GROUPS];

    /* Bit fields for tracking which groups the Friend knows about */
    BLE_MESH_ATOMIC_DEFINE(added, LPN_GROUPS);
    BLE_MESH_ATOMIC_DEFINE(pending, LPN_GROUPS);
    BLE_MESH_ATOMIC_DEFINE(to_remove, LPN_GROUPS);
};

/* bt_mesh_net.flags */
enum {
    BLE_MESH_NODE,            /* Device is a node */
    BLE_MESH_PROVISIONER,     /* Device is a Provisioner */
    BLE_MESH_VALID,           /* We have been provisioned */
    BLE_MESH_VALID_PROV,      /* Provisioner has been enabled */
    BLE_MESH_SUSPENDED,       /* Network is temporarily suspended */
    BLE_MESH_IVU_IN_PROGRESS, /* IV Update in Progress */
    BLE_MESH_IVU_INITIATOR,   /* IV Update initiated by us */
    BLE_MESH_IVU_TEST,        /* IV Update test mode */
    BLE_MESH_IVU_PENDING,     /* Update blocked by SDU in progress */

    /* pending storage actions, must reside within first 32 flags */
    BLE_MESH_RPL_PENDING,
    BLE_MESH_KEYS_PENDING,
    BLE_MESH_NET_PENDING,
    BLE_MESH_IV_PENDING,
    BLE_MESH_SEQ_PENDING,
    BLE_MESH_HB_PUB_PENDING,
    BLE_MESH_CFG_PENDING,
    BLE_MESH_MOD_PENDING,
    BLE_MESH_VA_PENDING,

    /* Don't touch - intentionally last */
    BLE_MESH_FLAG_COUNT,
};

struct bt_mesh_net {
    uint32_t iv_index; /* Current IV Index */
    uint32_t seq;      /* Next outgoing sequence number (24 bits) */

    BLE_MESH_ATOMIC_DEFINE(flags, BLE_MESH_FLAG_COUNT);

    /* Local network interface */
    struct k_work local_work;
    sys_slist_t local_queue;

#if defined(CONFIG_BLE_MESH_FRIEND)
    /* Friend state, unique for each LPN that we're Friends for */
    struct bt_mesh_friend frnd[CONFIG_BLE_MESH_FRIEND_LPN_COUNT];
#endif

#if defined(CONFIG_BLE_MESH_LOW_POWER)
    struct bt_mesh_lpn lpn;  /* Low Power Node state */
#endif

    /* Number of hours in current IV Update state */
    uint8_t ivu_duration;

    /* Timer to track duration in current IV Update state */
    struct k_delayed_work ivu_timer;

    uint8_t dev_key[16];

    struct bt_mesh_app_key app_keys[CONFIG_BLE_MESH_APP_KEY_COUNT];

    struct bt_mesh_subnet sub[CONFIG_BLE_MESH_SUBNET_COUNT];

    struct bt_mesh_rpl rpl[CONFIG_BLE_MESH_CRPL];

#if defined(CONFIG_BLE_MESH_PROVISIONER)
    /* Application keys stored by provisioner */
    struct bt_mesh_app_key *p_app_keys[CONFIG_BLE_MESH_PROVISIONER_APP_KEY_COUNT];
    /* Next app_idx can be assigned */
    uint16_t p_app_idx_next;

    /* Network keys stored by provisioner */
    struct bt_mesh_subnet *p_sub[CONFIG_BLE_MESH_PROVISIONER_SUBNET_COUNT];
    /* Next net_idx can be assigned */
    uint16_t p_net_idx_next;
#endif
};

/* Network interface */
enum bt_mesh_net_if {
    BLE_MESH_NET_IF_ADV,
    BLE_MESH_NET_IF_LOCAL,
    BLE_MESH_NET_IF_PROXY,
    BLE_MESH_NET_IF_PROXY_CFG,
};

/* Decoding context for Network/Transport data */
struct bt_mesh_net_rx {
    struct bt_mesh_subnet *sub;
    struct bt_mesh_msg_ctx ctx;
    uint32_t seq;            /* Sequence Number */
    uint8_t  old_iv:1,       /* iv_index - 1 was used */
             new_key:1,      /* Data was encrypted with updated key */
             friend_cred:1,  /* Data was encrypted with friend cred */
             ctl:1,          /* Network Control */
             net_if:2,       /* Network interface */
             local_match:1,  /* Matched a local element */
             friend_match:1; /* Matched an LPN we're friends for */
    uint16_t msg_cache_idx;  /* Index of entry in message cache */
};

/* Encoding context for Network/Transport data */
struct bt_mesh_net_tx {
    struct bt_mesh_subnet *sub;
    struct bt_mesh_msg_ctx *ctx;
    uint16_t src;
    uint8_t  xmit;
    uint8_t  friend_cred:1,
             aszmic:1,
             aid: 6;
};

extern struct bt_mesh_net bt_mesh;

#define BLE_MESH_NET_IVI_TX (bt_mesh.iv_index - \
                             bt_mesh_atomic_test_bit(bt_mesh.flags, \
                             BLE_MESH_IVU_IN_PROGRESS))
#define BLE_MESH_NET_IVI_RX(rx) (bt_mesh.iv_index - (rx)->old_iv)

#define BLE_MESH_NET_HDR_LEN 9

void bt_mesh_msg_cache_clear(uint16_t unicast_addr, uint8_t elem_num);

int bt_mesh_net_keys_create(struct bt_mesh_subnet_keys *keys,
                            const uint8_t key[16]);

int bt_mesh_net_create(uint16_t idx, uint8_t flags, const uint8_t key[16],
                       uint32_t iv_index);

uint8_t bt_mesh_net_flags(struct bt_mesh_subnet *sub);

bool bt_mesh_kr_update(struct bt_mesh_subnet *sub, uint8_t new_kr, bool new_key);

void bt_mesh_net_revoke_keys(struct bt_mesh_subnet *sub);

int bt_mesh_net_beacon_update(struct bt_mesh_subnet *sub);

void bt_mesh_rpl_reset(void);

bool bt_mesh_net_iv_update(uint32_t iv_index, bool iv_update);

void bt_mesh_net_sec_update(struct bt_mesh_subnet *sub);

struct bt_mesh_subnet *bt_mesh_subnet_get(uint16_t net_idx);

struct bt_mesh_subnet *bt_mesh_subnet_find(const uint8_t net_id[8], uint8_t flags,
                                           uint32_t iv_index, const uint8_t auth[8],
                                           bool *new_key);

int bt_mesh_net_encode(struct bt_mesh_net_tx *tx, struct net_buf_simple *buf,
                       bool proxy);

int bt_mesh_net_send(struct bt_mesh_net_tx *tx, struct net_buf *buf,
                     const struct bt_mesh_send_cb *cb, void *cb_data);

int bt_mesh_net_resend(struct bt_mesh_subnet *sub, struct net_buf *buf,
                       bool new_key, const struct bt_mesh_send_cb *cb,
                       void *cb_data);

int bt_mesh_net_decode(struct net_buf_simple *data, enum bt_mesh_net_if net_if,
                       struct bt_mesh_net_rx *rx, struct net_buf_simple *buf);

void bt_mesh_net_recv(struct net_buf_simple *data, int8_t rssi,
                      enum bt_mesh_net_if net_if);

bool bt_mesh_primary_subnet_exist(void);

uint32_t bt_mesh_next_seq(void);

void bt_mesh_net_start(void);

void bt_mesh_net_init(void);
void bt_mesh_net_reset(void);
void bt_mesh_net_deinit(void);

void bt_mesh_net_header_parse(struct net_buf_simple *buf,
                              struct bt_mesh_net_rx *rx);

/* Friendship Credential Management */
struct friend_cred {
    uint16_t net_idx;
    uint16_t addr;

    uint16_t lpn_counter;
    uint16_t frnd_counter;

    struct {
        uint8_t nid;         /* NID */
        uint8_t enc[16];     /* EncKey */
        uint8_t privacy[16]; /* PrivacyKey */
    } cred[2];
};

int friend_cred_get(struct bt_mesh_subnet *sub, uint16_t addr, uint8_t *nid,
                    const uint8_t **enc, const uint8_t **priv);
int friend_cred_set(struct friend_cred *cred, uint8_t idx, const uint8_t net_key[16]);
void friend_cred_refresh(uint16_t net_idx);
int friend_cred_update(struct bt_mesh_subnet *sub);
struct friend_cred *friend_cred_create(struct bt_mesh_subnet *sub, uint16_t addr,
                                       uint16_t lpn_counter, uint16_t frnd_counter);
void friend_cred_clear(struct friend_cred *cred);
int friend_cred_del(uint16_t net_idx, uint16_t addr);

static inline void send_cb_finalize(const struct bt_mesh_send_cb *cb,
                                    void *cb_data)
{
    if (!cb) {
        return;
    }

    if (cb->start) {
        cb->start(0, 0, cb_data);
    }

    if (cb->end) {
        cb->end(0, cb_data);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _NET_H_ */
