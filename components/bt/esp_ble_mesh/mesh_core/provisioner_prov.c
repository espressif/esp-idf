// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include <errno.h>

#include "crypto.h"
#include "adv.h"
#include "mesh.h"
#include "access.h"
#include "settings.h"
#include "mesh_common.h"
#include "proxy_client.h"
#include "provisioner_prov.h"
#include "provisioner_main.h"

#if CONFIG_BLE_MESH_PROVISIONER

_Static_assert(BLE_MESH_MAX_CONN >= CONFIG_BLE_MESH_PBG_SAME_TIME,
               "Too large BLE Mesh PB-GATT count");

/* 3 transmissions, 20ms interval */
#define PROV_XMIT              BLE_MESH_TRANSMIT(2, 20)

#define AUTH_METHOD_NO_OOB     0x00
#define AUTH_METHOD_STATIC     0x01
#define AUTH_METHOD_OUTPUT     0x02
#define AUTH_METHOD_INPUT      0x03

#define OUTPUT_OOB_BLINK       0x00
#define OUTPUT_OOB_BEEP        0x01
#define OUTPUT_OOB_VIBRATE     0x02
#define OUTPUT_OOB_NUMBER      0x03
#define OUTPUT_OOB_STRING      0x04

#define INPUT_OOB_PUSH         0x00
#define INPUT_OOB_TWIST        0x01
#define INPUT_OOB_NUMBER       0x02
#define INPUT_OOB_STRING       0x03

#define PROV_ERR_NONE          0x00
#define PROV_ERR_NVAL_PDU      0x01
#define PROV_ERR_NVAL_FMT      0x02
#define PROV_ERR_UNEXP_PDU     0x03
#define PROV_ERR_CFM_FAILED    0x04
#define PROV_ERR_RESOURCES     0x05
#define PROV_ERR_DECRYPT       0x06
#define PROV_ERR_UNEXP_ERR     0x07
#define PROV_ERR_ADDR          0x08

#define PROV_INVITE            0x00
#define PROV_CAPABILITIES      0x01
#define PROV_START             0x02
#define PROV_PUB_KEY           0x03
#define PROV_INPUT_COMPLETE    0x04
#define PROV_CONFIRM           0x05
#define PROV_RANDOM            0x06
#define PROV_DATA              0x07
#define PROV_COMPLETE          0x08
#define PROV_FAILED            0x09

#define PROV_ALG_P256          0x00

#define GPCF(gpc)              (gpc & 0x03)
#define GPC_START(last_seg)    (((last_seg) << 2) | 0x00)
#define GPC_ACK                0x01
#define GPC_CONT(seg_id)       (((seg_id) << 2) | 0x02)
#define GPC_CTL(op)            (((op) << 2) | 0x03)

#define START_PAYLOAD_MAX      20
#define CONT_PAYLOAD_MAX       23

#define START_LAST_SEG(gpc)    (gpc >> 2)
#define CONT_SEG_INDEX(gpc)    (gpc >> 2)

#define BEARER_CTL(gpc)        (gpc >> 2)
#define LINK_OPEN              0x00
#define LINK_ACK               0x01
#define LINK_CLOSE             0x02

#define CLOSE_REASON_SUCCESS   0x00
#define CLOSE_REASON_TIMEOUT   0x01
#define CLOSE_REASON_FAILED    0x02

#define PROV_AUTH_VAL_SIZE     0x10
#define PROV_CONF_SALT_SIZE    0x10
#define PROV_CONF_KEY_SIZE     0x10
#define PROV_DH_KEY_SIZE       0x20
#define PROV_CONFIRM_SIZE      0x10
#define PROV_PROV_SALT_SIZE    0x10
#define PROV_CONF_INPUTS_SIZE  0x91

#define XACT_SEG_DATA(_idx, _seg) (&link[_idx].rx.buf->data[20 + ((_seg - 1) * 23)])
#define XACT_SEG_RECV(_idx, _seg) (link[_idx].rx.seg &= ~(1 << (_seg)))

#define XACT_NVAL              0xff

enum {
    REMOTE_PUB_KEY,        /* Remote key has been received */
    LOCAL_PUB_KEY,         /* Local public key is available */
    LINK_ACTIVE,           /* Link has been opened */
    WAIT_GEN_DHKEY,        /* Waiting for remote public key to generate DHKey */
    HAVE_DHKEY,            /* DHKey has been calcualted */
    SEND_CONFIRM,          /* Waiting to send Confirm value */
    WAIT_NUMBER,           /* Waiting for number input from user */
    WAIT_STRING,           /* Waiting for string input from user */
    TIMEOUT_START,         /* Provision timeout timer has started */
    NUM_FLAGS,
};

/** Provisioner link structure allocation
 * |--------------------------------------------------------|
 * |            Link(PB-ADV)            |   Link(PB-GATT)   |
 * |--------------------------------------------------------|
 * |<----------------------Total Link---------------------->|
 */
struct prov_link {
    BLE_MESH_ATOMIC_DEFINE(flags, NUM_FLAGS);
    u8_t  uuid[16];          /* check if device is being provisioned*/
    u16_t oob_info;          /* oob info of this device */
    u8_t  element_num;       /* element num of device */
    u8_t  ki_flags;          /* Key refresh flag and iv update flag */
    u32_t iv_index;          /* IV Index */
    u8_t  auth_method;       /* choosed authentication method */
    u8_t  auth_action;       /* choosed authentication action */
    u8_t  auth_size;         /* choosed authentication size */
    u16_t assign_addr;       /* Application assigned address for the device */
    u16_t unicast_addr;      /* unicast address allocated for device */
    bt_mesh_addr_t addr;     /* Device address */
#if defined(CONFIG_BLE_MESH_PB_GATT)
    bool   connecting;       /* start connecting with device */
    struct bt_mesh_conn *conn; /* GATT connection */
#endif
    u8_t  expect;            /* Next expected PDU */

    u8_t *dhkey;             /* Calculated DHKey */
    u8_t *auth;              /* Authentication Value */

    u8_t *conf_salt;         /* ConfirmationSalt */
    u8_t *conf_key;          /* ConfirmationKey */
    u8_t *conf_inputs;       /* ConfirmationInputs */

    u8_t *rand;              /* Local Random */
    u8_t *conf;              /* Remote Confirmation */

    u8_t *prov_salt;         /* Provisioning Salt */

#if defined(CONFIG_BLE_MESH_PB_ADV)
    bool  linking;           /* Linking is being establishing */
    u16_t send_link_close;   /* Link close is being sent flag */
    u32_t link_id;           /* Link ID */
    u8_t  pending_ack;       /* Decide which transaction id ack is pending */
    u8_t  expect_ack_for;    /* Transaction ACK expected for provisioning pdu */
    u8_t  tx_pdu_type;       /* The current transmitted Provisioning PDU type */

    struct {
        u8_t  trans_id;      /* Transaction ID */
        u8_t  prev_id;       /* Previous Transaction ID */
        u8_t  seg;           /* Bit-field of unreceived segments */
        u8_t  last_seg;      /* Last segment (to check length) */
        u8_t  fcs;           /* Expected FCS value */
        u8_t  adv_buf_id;    /* index of buf allocated in adv_buf_data */
        struct net_buf_simple *buf;
    } rx;

    struct {
        /* Start timestamp of the transaction */
        s64_t start;

        /* Transaction id*/
        u8_t trans_id;

        /* Pending outgoing buffer(s) */
        struct net_buf *buf[3];

        /* Retransmit timer */
        struct k_delayed_work retransmit;
    } tx;
#endif

    /** Provision timeout timer. Spec P259 says: The provisioning protocol
     *  shall have a minimum timeout of 60 seconds that is reset each time
     *  a provisioning protocol PDU is sent or received.
     */
    struct k_delayed_work timeout;
};

/* Number of devices can be provisioned at the same time equals to PB-ADV + PB-GATT */
#define BLE_MESH_PROV_SAME_TIME \
    (CONFIG_BLE_MESH_PBA_SAME_TIME + CONFIG_BLE_MESH_PBG_SAME_TIME)

#define PROV_MAX_ADDR_TO_ASSIGN    0x7FFF

static struct prov_link link[BLE_MESH_PROV_SAME_TIME];

struct prov_rx {
    u32_t link_id;
    u8_t  xact_id;
    u8_t  gpc;
};

struct bt_mesh_prov_ctx {
    /* Primary element address of Provisioner */
    u16_t primary_addr;

    /* Provisioning bearers used by Provisioner */
    bt_mesh_prov_bearer_t bearers;

    /* If provisioning random have been generated, set BIT0 to 1 */
    u8_t  rand_gen_done;

    /* Provisioner random */
    u8_t  random[16];

    /* Current number of PB-ADV provisioned devices simultaneously */
    u8_t  pba_count;

    /* Current number of PB-GATT provisioned devices simultaneously */
    u8_t  pbg_count;

    /* Current unicast address going to allocated */
    u16_t curr_alloc_addr;

    /* Current net_idx going to be used in provisioning data */
    u16_t curr_net_idx;

    /* Current flags going to be used in provisioning data */
    u16_t curr_flags;

    /* Current iv_index going to be used in provisioning data */
    u16_t curr_iv_index;

    /* Length of Static OOB value */
    u8_t  static_oob_len;

    /* Static OOB value */
    u8_t  static_oob_val[16];

    /* Offset of the device uuid to be matched, based on zero */
    u8_t  match_offset;

    /* Length of the device uuid to be matched (start from the match_offset) */
    u8_t  match_length;

    /* Value of the device uuid to be matched */
    u8_t  match_value[16];

    /* Indicate when received uuid_match adv_pkts, can provision it at once */
    bool prov_after_match;

#if defined(CONFIG_BLE_MESH_PB_ADV)
    /* Mutex used to protect the PB-ADV procedure */
    bt_mesh_mutex_t pb_adv_lock;

    /* Mutex used to protect the adv buf during PB-ADV procedure */
    bt_mesh_mutex_t pb_buf_lock;
#endif

#if defined(CONFIG_BLE_MESH_PB_GATT)
    /* Mutex used to protect the PB-GATT procedure */
    bt_mesh_mutex_t pb_gatt_lock;
#endif

    /* Fast provisioning related information */
    struct {
        bool  enable;
        u16_t net_idx;
        const u8_t *net_key;
        u8_t  flags;
        u32_t iv_index;
        u16_t unicast_addr_min;
        u16_t unicast_addr_max;
    } fast_prov;
};

static struct bt_mesh_prov_ctx prov_ctx;

#define FAST_PROV_ENABLE()  (prov_ctx.fast_prov.enable)

struct unprov_dev_queue {
    bt_mesh_addr_t addr;
    u8_t  uuid[16];
    u16_t oob_info;
    u8_t  bearer;
    u8_t  flags;
} __packed unprov_dev[CONFIG_BLE_MESH_WAIT_FOR_PROV_MAX_DEV_NUM] = {
    [0 ... (CONFIG_BLE_MESH_WAIT_FOR_PROV_MAX_DEV_NUM - 1)] = {
        .addr.type = 0xff,
        .bearer    = 0,
        .flags     = false,
    },
};

static unprov_adv_pkt_cb_t   notify_unprov_adv_pkt_cb;

#define BUF_TIMEOUT          K_MSEC(400)

#if defined(CONFIG_BLE_MESH_FAST_PROV)
#define RETRANSMIT_TIMEOUT   K_MSEC(360)
#define TRANSACTION_TIMEOUT  K_SECONDS(3)
#define PROVISION_TIMEOUT    K_SECONDS(6)
#else
#define RETRANSMIT_TIMEOUT   K_MSEC(500)
#define TRANSACTION_TIMEOUT  K_SECONDS(30)
#define PROVISION_TIMEOUT    K_SECONDS(60)
#endif /* CONFIG_BLE_MESH_FAST_PROV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
#define PROV_BUF_HEADROOM 5
#else
#define PROV_BUF_HEADROOM 0
#endif

#define PROV_BUF(name, len) \
    NET_BUF_SIMPLE_DEFINE(name, PROV_BUF_HEADROOM + len)

static const struct bt_mesh_prov *prov;

#if defined(CONFIG_BLE_MESH_PB_ADV)
static void send_link_open(const u8_t idx);
#endif

static void prov_gen_dh_key(const u8_t idx);

static void send_pub_key(const u8_t idx, u8_t oob);

static void close_link(const u8_t idx, u8_t reason);

#if defined(CONFIG_BLE_MESH_PB_ADV)
#define ADV_BUF_SIZE    65

static struct prov_adv_buf {
    struct net_buf_simple buf;
} adv_buf[CONFIG_BLE_MESH_PBA_SAME_TIME];

static u8_t adv_buf_data[ADV_BUF_SIZE * CONFIG_BLE_MESH_PBA_SAME_TIME];
#endif /* CONFIG_BLE_MESH_PB_ADV */

#define PROV_FREE_MEM(_idx, member)     \
{                                       \
    if (link[_idx].member) {            \
        bt_mesh_free(link[_idx].member);    \
        link[_idx].member = NULL;       \
    }                                   \
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static void bt_mesh_pb_adv_mutex_new(void)
{
    if (!prov_ctx.pb_adv_lock.mutex) {
        bt_mesh_mutex_create(&prov_ctx.pb_adv_lock);
    }
}

static void bt_mesh_pb_adv_mutex_free(void)
{
    bt_mesh_mutex_free(&prov_ctx.pb_adv_lock);
}

static void bt_mesh_pb_adv_lock(void)
{
    bt_mesh_mutex_lock(&prov_ctx.pb_adv_lock);
}

static void bt_mesh_pb_adv_unlock(void)
{
    bt_mesh_mutex_unlock(&prov_ctx.pb_adv_lock);
}

static void bt_mesh_pb_buf_mutex_new(void)
{
    if (!prov_ctx.pb_buf_lock.mutex) {
        bt_mesh_mutex_create(&prov_ctx.pb_buf_lock);
    }
}

static void bt_mesh_pb_buf_mutex_free(void)
{
    bt_mesh_mutex_free(&prov_ctx.pb_buf_lock);
}

static void bt_mesh_pb_buf_lock(void)
{
    bt_mesh_mutex_lock(&prov_ctx.pb_buf_lock);
}

static void bt_mesh_pb_buf_unlock(void)
{
    bt_mesh_mutex_unlock(&prov_ctx.pb_buf_lock);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
static void bt_mesh_pb_gatt_mutex_new(void)
{
    if (!prov_ctx.pb_gatt_lock.mutex) {
        bt_mesh_mutex_create(&prov_ctx.pb_gatt_lock);
    }
}

static void bt_mesh_pb_gatt_mutex_free(void)
{
    bt_mesh_mutex_free(&prov_ctx.pb_gatt_lock);
}

static void bt_mesh_pb_gatt_lock(void)
{
    bt_mesh_mutex_lock(&prov_ctx.pb_gatt_lock);
}

static void bt_mesh_pb_gatt_unlock(void)
{
    bt_mesh_mutex_unlock(&prov_ctx.pb_gatt_lock);
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

void bt_mesh_provisioner_pbg_count_dec(void)
{
    if (prov_ctx.pbg_count) {
        prov_ctx.pbg_count--;
    }
}

static inline void provisioner_pbg_count_inc(void)
{
    prov_ctx.pbg_count++;
}

#if defined(CONFIG_BLE_MESH_PB_GATT)
void bt_mesh_provisioner_clear_link_info(const u8_t addr[6])
{
    int i;

    if (!addr) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("%s, Clear device %s info", __func__, bt_hex(addr, BLE_MESH_ADDR_LEN));

    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (!memcmp(link[i].addr.val, addr, BLE_MESH_ADDR_LEN)) {
            link[i].connecting = false;
            link[i].conn = NULL;
            link[i].oob_info = 0x0;
            memset(link[i].uuid, 0, 16);
            memset(&link[i].addr, 0, sizeof(bt_mesh_addr_t));
            bt_mesh_atomic_test_and_clear_bit(link[i].flags, LINK_ACTIVE);
            if (bt_mesh_atomic_test_and_clear_bit(link[i].flags, TIMEOUT_START)) {
                k_delayed_work_cancel(&link[i].timeout);
            }
            return;
        }
    }

    BT_WARN("Device address %s is not found", bt_hex(addr, BLE_MESH_ADDR_LEN));
    return;
}
#endif

const struct bt_mesh_prov *bt_mesh_provisioner_get_prov_info(void)
{
    return prov;
}

void bt_mesh_provisoner_restore_prov_info(u16_t primary_addr, u16_t alloc_addr)
{
    prov_ctx.primary_addr = primary_addr;
    prov_ctx.curr_alloc_addr = alloc_addr;
}

static int provisioner_dev_find(const bt_mesh_addr_t *addr, const u8_t uuid[16], u16_t *index)
{
    bool uuid_match = false;
    bool addr_match = false;
    u8_t zero[16] = {0};
    u16_t i = 0U, j = 0U;
    int comp = 0;

    if (addr) {
        comp = memcmp(addr->val, zero, BLE_MESH_ADDR_LEN);
    }

    if ((!uuid && (!addr || (comp == 0) || (addr->type > BLE_MESH_ADDR_RANDOM))) || !index) {
        return -EINVAL;
    }

    /** Note: user may add a device into two unprov_dev array elements,
     *        one with device address, address type and another only
     *        with device UUID. We need to take this into consideration.
     */
    if (uuid && memcmp(uuid, zero, 16)) {
        for (i = 0; i < ARRAY_SIZE(unprov_dev); i++) {
            if (!memcmp(unprov_dev[i].uuid, uuid, 16)) {
                uuid_match = true;
                break;
            }
        }
    }

    if (addr && comp && (addr->type <= BLE_MESH_ADDR_RANDOM)) {
        for (j = 0; j < ARRAY_SIZE(unprov_dev); j++) {
            if (!memcmp(unprov_dev[j].addr.val, addr->val, BLE_MESH_ADDR_LEN) &&
                    unprov_dev[j].addr.type == addr->type) {
                addr_match = true;
                break;
            }
        }
    }

    if (!uuid_match && !addr_match) {
        BT_DBG("%s, Device does not exist in queue", __func__);
        return -ENODEV;
    }

    if (uuid_match && addr_match && (i != j)) {
        /**
         * In this situation, copy address & type into device uuid
         * array element, reset another element, rm_flag will be
         * decided by uuid element.
         */
        unprov_dev[i].addr.type = unprov_dev[j].addr.type;
        memcpy(unprov_dev[i].addr.val, unprov_dev[j].addr.val, BLE_MESH_ADDR_LEN);
        unprov_dev[i].bearer |= unprov_dev[j].bearer;
        memset(&unprov_dev[j], 0x0, sizeof(struct unprov_dev_queue));
    }

    *index = uuid_match ? i : j;
    return 0;
}

static bool is_unprov_dev_being_provision(const u8_t uuid[16])
{
    int i;

#if defined(CONFIG_BLE_MESH_FAST_PROV)
    /**
     * During Fast Provisioning test, we found that if a device has already being
     * provisioned, there is still a chance that the Provisioner can receive the
     * Unprovisioned Device Beacon from the device (because the device will stop
     * Unprovisioned Device Beacon when Transaction ACK for Provisioning Complete
     * is received). So in Fast Provisioning the Provisioner should ignore this.
     */
    if (bt_mesh_provisioner_find_node_with_uuid(uuid, false)) {
        BT_WARN("Device has already been provisioned");
        return true;
    }
#endif

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
#if defined(CONFIG_BLE_MESH_PB_ADV) && defined(CONFIG_BLE_MESH_PB_GATT)
        if (link[i].linking || link[i].connecting ||
                bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE)) {
#elif defined(CONFIG_BLE_MESH_PB_ADV) && !defined(CONFIG_BLE_MESH_PB_GATT)
        if (link[i].linking || bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE)) {
#else
        if (link[i].connecting || bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE)) {
#endif
            if (!memcmp(link[i].uuid, uuid, 16)) {
                BT_DBG("%s, Device is being provisioned", __func__);
                return true;
            }
        }
    }

    return false;
}

static bool is_unprov_dev_uuid_match(const u8_t uuid[16])
{
    if (prov_ctx.match_length) {
        if (memcmp(uuid + prov_ctx.match_offset,
                   prov_ctx.match_value, prov_ctx.match_length)) {
            return false;
        }
    }

    return true;
}

static int provisioner_check_unprov_dev_info(const u8_t uuid[16], bt_mesh_prov_bearer_t bearer)
{
    if (!uuid) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Check if the device uuid matches configured value */
    if (is_unprov_dev_uuid_match(uuid) == false) {
        BT_DBG("%s, Device uuid mismatch", __func__);
        return -EIO;
    }

    /* Check if this device is currently being provisioned.
     * According to Zephyr's device code, if we connect with
     * one device and start to provision it, we may still can
     * receive the connectable prov adv pkt from this device.
     * Here we check both PB-GATT and PB-ADV link status.
     */
    if (is_unprov_dev_being_provision(uuid)) {
        return -EALREADY;
    }

    /* Check if the current PB-ADV link is full */
    if (bearer == BLE_MESH_PROV_ADV && prov_ctx.pba_count == CONFIG_BLE_MESH_PBA_SAME_TIME) {
        BT_INFO("Current PB-ADV links reach max limit");
        return -ENOMEM;
    }

    /* Check if the current PB-GATT link is full */
    if (bearer == BLE_MESH_PROV_GATT && prov_ctx.pbg_count == CONFIG_BLE_MESH_PBG_SAME_TIME) {
        BT_INFO("Current PB-GATT links reach max limit");
        return -ENOMEM;
    }

    /* Check if the device has already been provisioned */
    if (bt_mesh_provisioner_find_node_with_uuid(uuid, false)) {
        BT_WARN("Provisioned before, start to provision again");
        return 0;
    }

    /* Check if the provisioned nodes queue is full */
    if (bt_mesh_provisioner_get_prov_node_count() == CONFIG_BLE_MESH_MAX_PROV_NODES) {
        BT_WARN("Current provisioned devices reach max limit");
        return -ENOMEM;
    }

    return 0;
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static int provisioner_start_prov_pb_adv(const u8_t uuid[16], const bt_mesh_addr_t *addr,
                                         u16_t oob_info, u16_t assign_addr)
{
    u8_t zero[6] = {0};
    int addr_cmp = 0;
    int i;

    if (!uuid || !addr) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_pb_adv_lock();

    /* If the unicast address of the node is going to be allocated internally,
     * then we need to check if there are addresses can be allocated.
     */
    if (assign_addr == BLE_MESH_ADDR_UNASSIGNED &&
            prov_ctx.curr_alloc_addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_ERR("No available unicast address to assign");
        return -EIO;
    }

    if (is_unprov_dev_being_provision(uuid)) {
        bt_mesh_pb_adv_unlock();
        return -EALREADY;
    }

    addr_cmp = memcmp(addr->val, zero, BLE_MESH_ADDR_LEN);

    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        if (!bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE) && !link[i].linking) {
            memcpy(link[i].uuid, uuid, 16);
            link[i].oob_info = oob_info;
            if (addr_cmp && (addr->type <= BLE_MESH_ADDR_RANDOM)) {
                link[i].addr.type = addr->type;
                memcpy(link[i].addr.val, addr->val, BLE_MESH_ADDR_LEN);
            }
            send_link_open(i);
            /* If the application layer assigned a specific unicast address for the device,
             * then Provisioner will use this address in the Provisoning Data PDU.
             */
            if (BLE_MESH_ADDR_IS_UNICAST(assign_addr)) {
                link[i].assign_addr = assign_addr;
            }
            bt_mesh_pb_adv_unlock();
            return 0;
        }
    }

    BT_ERR("%s, No PB-ADV link is available", __func__);
    bt_mesh_pb_adv_unlock();

    return -ENOMEM;
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
static int provisioner_start_prov_pb_gatt(const u8_t uuid[16], const bt_mesh_addr_t *addr,
                                          u16_t oob_info, u16_t assign_addr)
{
    u8_t zero[6] = {0};
    int addr_cmp = 0;
    int i;

    if (!uuid || !addr) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_pb_gatt_lock();

    /* If the unicast address of the node is going to be allocated internally,
     * then we need to check if there are addresses can be allocated.
     */
    if (assign_addr == BLE_MESH_ADDR_UNASSIGNED &&
            prov_ctx.curr_alloc_addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_ERR("No available unicast address to assign");
        return -EIO;
    }

    if (is_unprov_dev_being_provision(uuid)) {
        bt_mesh_pb_gatt_unlock();
        return -EALREADY;
    }

    addr_cmp = memcmp(addr->val, zero, BLE_MESH_ADDR_LEN);

    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (!link[i].connecting && !bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE)) {
            memcpy(link[i].uuid, uuid, 16);
            link[i].oob_info = oob_info;
            if (addr_cmp && (addr->type <= BLE_MESH_ADDR_RANDOM)) {
                link[i].addr.type = addr->type;
                memcpy(link[i].addr.val, addr->val, BLE_MESH_ADDR_LEN);
            }
            if (bt_mesh_gattc_conn_create(&link[i].addr, BLE_MESH_UUID_MESH_PROV_VAL) < 0) {
                memset(link[i].uuid, 0, 16);
                link[i].oob_info = 0x0;
                memset(&link[i].addr, 0, sizeof(bt_mesh_addr_t));
                bt_mesh_pb_gatt_unlock();
                return -EIO;
            }
            /* If the application layer assigned a specific unicast address for the device,
             * then Provisioner will use this address in the Provisoning Data PDU.
             */
            if (BLE_MESH_ADDR_IS_UNICAST(assign_addr)) {
                link[i].assign_addr = assign_addr;
            }
            /* If creating connection successfully, set connecting flag to 1 */
            link[i].connecting = true;
            provisioner_pbg_count_inc();
            bt_mesh_pb_gatt_unlock();
            return 0;
        }
    }

    BT_ERR("%s, No PB-GATT link is available", __func__);
    bt_mesh_pb_gatt_unlock();

    return -ENOMEM;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

int bt_mesh_provisioner_add_unprov_dev(struct bt_mesh_unprov_dev_add *add_dev, u8_t flags)
{
    bt_mesh_addr_t add_addr = {0};
    u8_t zero[16] = {0};
    int addr_cmp = 0;
    int uuid_cmp = 0;
    u16_t i = 0U;
    int err = 0;

    if (!add_dev) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    addr_cmp = memcmp(add_dev->addr, zero, BLE_MESH_ADDR_LEN);
    uuid_cmp = memcmp(add_dev->uuid, zero, 16);

    if (add_dev->bearer == 0x0 || ((uuid_cmp == 0) &&
                                   ((addr_cmp == 0) || add_dev->addr_type > BLE_MESH_ADDR_RANDOM))) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if ((add_dev->bearer & BLE_MESH_PROV_ADV) && (add_dev->bearer & BLE_MESH_PROV_GATT) &&
            (flags & START_PROV_NOW)) {
        BT_ERR("%s, Can not start PB-ADV & PB-GATT simultaneouly", __func__);
        return -EINVAL;
    }

    if ((uuid_cmp == 0) && (flags & START_PROV_NOW)) {
        BT_ERR("%s, Can not start provisioning with zero uuid", __func__);
        return -EINVAL;
    }

    if ((add_dev->bearer & BLE_MESH_PROV_GATT) && (flags & START_PROV_NOW) &&
            ((addr_cmp == 0) || add_dev->addr_type > BLE_MESH_ADDR_RANDOM)) {
        BT_ERR("%s, Invalid device address for PB-GATT", __func__);
        return -EINVAL;
    }

    if (add_dev->bearer & BLE_MESH_PROV_GATT) {
#if !CONFIG_BLE_MESH_PB_GATT
        BT_ERR("%s, Not support PB-GATT", __func__);
        return -EINVAL;
#endif
    }

    if (add_dev->bearer & BLE_MESH_PROV_ADV) {
#if !CONFIG_BLE_MESH_PB_ADV
        BT_ERR("%s, Not support PB-ADV", __func__);
        return -EINVAL;
#endif
    }

    add_addr.type = add_dev->addr_type;
    memcpy(add_addr.val, add_dev->addr, BLE_MESH_ADDR_LEN);

    err = provisioner_dev_find(&add_addr, add_dev->uuid, &i);
    if (err == -EINVAL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return err;
    } else if (err == 0) {
        if (!(add_dev->bearer & unprov_dev[i].bearer)) {
            BT_WARN("Add device with only bearer updated");
            unprov_dev[i].bearer |= add_dev->bearer;
        } else {
            BT_WARN("Device already exists in queue");
        }
        goto start;
    }

    for (i = 0U; i < ARRAY_SIZE(unprov_dev); i++) {
        if (unprov_dev[i].bearer) {
            continue;
        }
        if (addr_cmp && (add_dev->addr_type <= BLE_MESH_ADDR_RANDOM)) {
            unprov_dev[i].addr.type = add_dev->addr_type;
            memcpy(unprov_dev[i].addr.val, add_dev->addr, BLE_MESH_ADDR_LEN);
        }
        if (uuid_cmp) {
            memcpy(unprov_dev[i].uuid, add_dev->uuid, 16);
        }
        unprov_dev[i].bearer = add_dev->bearer & BIT_MASK(2);
        unprov_dev[i].flags = flags & BIT_MASK(3);
        goto start;
    }

    /* If queue is full, find flushable device and replace it */
    for (i = 0U; i < ARRAY_SIZE(unprov_dev); i++) {
        if (unprov_dev[i].flags & FLUSHABLE_DEV) {
            memset(&unprov_dev[i], 0, sizeof(struct unprov_dev_queue));
            if (addr_cmp && (add_dev->addr_type <= BLE_MESH_ADDR_RANDOM)) {
                unprov_dev[i].addr.type = add_dev->addr_type;
                memcpy(unprov_dev[i].addr.val, add_dev->addr, BLE_MESH_ADDR_LEN);
            }
            if (uuid_cmp) {
                memcpy(unprov_dev[i].uuid, add_dev->uuid, 16);
            }
            unprov_dev[i].bearer = add_dev->bearer & BIT_MASK(2);
            unprov_dev[i].flags  = flags & BIT_MASK(3);
            goto start;
        }
    }

    BT_ERR("%s, Unprovisioned device queue is full", __func__);
    return -ENOMEM;

start:
    if (!(flags & START_PROV_NOW)) {
        return 0;
    }

    /* Check if current provisioned node count + active link reach max limit */
    if (bt_mesh_provisioner_get_prov_node_count() + prov_ctx.pba_count + \
            prov_ctx.pbg_count >= CONFIG_BLE_MESH_MAX_PROV_NODES) {
        BT_ERR("Node count + active link count reach max limit");
        return -EIO;
    }

    if ((err = provisioner_check_unprov_dev_info(add_dev->uuid, add_dev->bearer))) {
        return err;
    }

    if (add_dev->bearer == BLE_MESH_PROV_ADV) {
#if defined(CONFIG_BLE_MESH_PB_ADV)
        if ((err = provisioner_start_prov_pb_adv(
                       add_dev->uuid, &add_addr, add_dev->oob_info, BLE_MESH_ADDR_UNASSIGNED))) {
            return err;
        }
#endif
    } else if (add_dev->bearer == BLE_MESH_PROV_GATT) {
#if defined(CONFIG_BLE_MESH_PB_GATT)
        if ((err = provisioner_start_prov_pb_gatt(
                       add_dev->uuid, &add_addr, add_dev->oob_info, BLE_MESH_ADDR_UNASSIGNED))) {
            return err;
        }
#endif
    }

    return 0;
}

int bt_mesh_provisioner_prov_device_with_addr(const u8_t uuid[16], const u8_t addr[6],
                                              u8_t addr_type, bt_mesh_prov_bearer_t bearer,
                                              u16_t oob_info, u16_t unicast_addr)
{
    bt_mesh_addr_t dev_addr = {0};
    int err = 0;

    if (uuid == NULL) {
        BT_ERR("%s, NULL device uuid", __func__);
        return -EINVAL;
    }

    if (bearer != BLE_MESH_PROV_ADV && bearer != BLE_MESH_PROV_GATT) {
        BT_ERR("%s, Invalid provisioning bearer 0x%02x", __func__, bearer);
        return -EINVAL;
    }

    if (!IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) && bearer == BLE_MESH_PROV_ADV) {
        BT_ERR("%s, Not support PB-ADV", __func__);
        return -ENOTSUP;
    }

    if (!IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) && bearer == BLE_MESH_PROV_GATT) {
        BT_ERR("%s, Not support PB-GATT", __func__);
        return -ENOTSUP;
    }

    if (bearer == BLE_MESH_PROV_GATT && (addr == NULL || addr_type > BLE_MESH_ADDR_RANDOM)) {
        BT_ERR("%s, Invalid device address info", __func__);
        return -EINVAL;
    }

    if (!BLE_MESH_ADDR_IS_UNICAST(unicast_addr)) {
        BT_ERR("%s, Invalid unicast address 0x%04x", __func__, unicast_addr);
        return -EINVAL;
    }

    /* Here we will not check if the assigned unicast address is overlapped
     * with the unicast addresses of other nodes or Provisioner, beacuse:
     * 1. At this moment, the element number of the device is unknown
     * 2. If the node is a reprovisioned device, then the original allocated
     *    unicast address will be used.
     * 3. Some other devices may be just being provisioning, and currently we
     *    can not know the exactly allocated addresses of them.
     */

    /* Check if current provisioned node count + active link reach max limit */
    if (bt_mesh_provisioner_get_prov_node_count() + prov_ctx.pba_count + \
            prov_ctx.pbg_count >= CONFIG_BLE_MESH_MAX_PROV_NODES) {
        BT_ERR("Node count + active link count reach max limit");
        return -EIO;
    }

    if ((err = provisioner_check_unprov_dev_info(uuid, bearer))) {
        return err;
    }

    if (addr) {
        dev_addr.type = addr_type;
        memcpy(dev_addr.val, addr, BLE_MESH_ADDR_LEN);
    }

    if (bearer == BLE_MESH_PROV_ADV) {
#if defined(CONFIG_BLE_MESH_PB_ADV)
        if ((err = provisioner_start_prov_pb_adv(uuid, &dev_addr, oob_info, unicast_addr))) {
            return err;
        }
#endif
    } else if (bearer == BLE_MESH_PROV_GATT) {
#if defined(CONFIG_BLE_MESH_PB_GATT)
        if ((err = provisioner_start_prov_pb_gatt(uuid, &dev_addr, oob_info, unicast_addr))) {
            return err;
        }
#endif
    }

    return 0;
}

int bt_mesh_provisioner_delete_device(struct bt_mesh_device_delete *del_dev)
{
    /**
     * Three Situations:
     * 1. device is not being/been provisioned, just remove from device queue.
     * 2. device is being provisioned, need to close link & remove from device queue.
     * 3. device is been provisioned, need to send config_node_reset and may need to
     *    remove from device queue. config _node_reset can be added in function
     *    provisioner_reset_node() in provisioner_main.c.
     */
    bt_mesh_addr_t del_addr = {0};
    u8_t zero[16] = {0};
    bool addr_match = false;
    bool uuid_match = false;
    int addr_cmp = 0;
    int uuid_cmp = 0;
    u16_t i = 0U;
    int err = 0;

    if (!del_dev) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    addr_cmp = memcmp(del_dev->addr, zero, BLE_MESH_ADDR_LEN);
    uuid_cmp = memcmp(del_dev->uuid, zero, 16);

    if ((uuid_cmp == 0) && ((addr_cmp == 0) || del_dev->addr_type > BLE_MESH_ADDR_RANDOM)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    del_addr.type = del_dev->addr_type;
    memcpy(del_addr.val, del_dev->addr, BLE_MESH_ADDR_LEN);

    /* First: find if the device is in the device queue */
    err = provisioner_dev_find(&del_addr, del_dev->uuid, &i);
    if (err) {
        BT_DBG("%s, Device is not in the queue", __func__);
    } else {
        memset(&unprov_dev[i], 0x0, sizeof(struct unprov_dev_queue));
    }

    /* Second: find if the device is being provisioned */
    for (i = 0U; i < ARRAY_SIZE(link); i++) {
        if (addr_cmp && (del_dev->addr_type <= BLE_MESH_ADDR_RANDOM)) {
            if (!memcmp(link[i].addr.val, del_dev->addr, BLE_MESH_ADDR_LEN) &&
                    link[i].addr.type == del_dev->addr_type) {
                addr_match = true;
            }
        }
        if (uuid_cmp) {
            if (!memcmp(link[i].uuid, del_dev->uuid, 16)) {
                uuid_match = true;
            }
        }
        if (addr_match || uuid_match) {
            close_link(i, CLOSE_REASON_FAILED);
            break;
        }
    }

    /* Third: find if the device is been provisioned */
    if (addr_cmp && (del_dev->addr_type <= BLE_MESH_ADDR_RANDOM)) {
        if (bt_mesh_provisioner_find_node_with_addr(&del_addr, true)) {
            return 0;
        }
    }

    if (uuid_cmp) {
        if (bt_mesh_provisioner_find_node_with_uuid(del_dev->uuid, true)) {
            return 0;
        }
    }

    return 0;
}

int bt_mesh_provisioner_set_dev_uuid_match(u8_t offset, u8_t length,
        const u8_t *match, bool prov_flag)
{
    if (length && (!match || (offset + length > 16))) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    (void)memset(prov_ctx.match_value, 0, 16);

    prov_ctx.match_offset = offset;
    prov_ctx.match_length = length;
    if (length) {
        memcpy(prov_ctx.match_value, match, length);
    }
    prov_ctx.prov_after_match = prov_flag;

    return 0;
}

int bt_mesh_provisioner_adv_pkt_cb_register(unprov_adv_pkt_cb_t cb)
{
    if (!cb) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    notify_unprov_adv_pkt_cb = cb;
    return 0;
}

int bt_mesh_provisioner_set_prov_data_info(struct bt_mesh_prov_data_info *info)
{
    const u8_t *key = NULL;

    if (!info || info->flag == 0) {
        return -EINVAL;
    }

    if (info->flag & NET_IDX_FLAG) {
        key = bt_mesh_provisioner_net_key_get(info->net_idx);
        if (!key) {
            BT_ERR("%s, Failed to get NetKey", __func__);
            return -EINVAL;
        }
        prov_ctx.curr_net_idx = info->net_idx;
    } else if (info->flag & FLAGS_FLAG) {
        prov_ctx.curr_flags = info->flags;
    } else if (info->flag & IV_INDEX_FLAG) {
        prov_ctx.curr_iv_index = info->iv_index;
    }

    return 0;
}

int bt_mesh_provisioner_set_prov_info(void)
{
    const struct bt_mesh_comp *comp = NULL;

    if (prov_ctx.primary_addr == BLE_MESH_ADDR_UNASSIGNED) {
        /* If unicast address of primary element of Provisioner has not been set
         * before, then the following initilization procedure will be used.
         */
        if (!BLE_MESH_ADDR_IS_UNICAST(prov->prov_unicast_addr) ||
            !BLE_MESH_ADDR_IS_UNICAST(prov->prov_start_address)) {
            BT_ERR("%s, Invalid address, own 0x%04x, start 0x%04x",
                __func__, prov->prov_unicast_addr, prov->prov_start_address);
            return -EINVAL;
        }

        comp = bt_mesh_comp_get();
        if (!comp) {
            BT_ERR("%s, NULL composition data", __func__);
            return -EINVAL;
        }

        if (prov->prov_unicast_addr + comp->elem_count > prov->prov_start_address) {
            BT_WARN("Too small start address 0x%04x, update to 0x%04x",
                prov->prov_start_address, prov->prov_unicast_addr + comp->elem_count);
            prov_ctx.curr_alloc_addr = prov->prov_unicast_addr + comp->elem_count;
        } else {
            prov_ctx.curr_alloc_addr = prov->prov_start_address;
        }
        prov_ctx.primary_addr = prov->prov_unicast_addr;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.curr_alloc_addr);
        }
    }
    prov_ctx.curr_net_idx = BLE_MESH_KEY_PRIMARY;
    prov_ctx.curr_flags = prov->flags;
    prov_ctx.curr_iv_index = prov->iv_index;

    return 0;
}

void bt_mesh_provisioner_set_prov_bearer(bt_mesh_prov_bearer_t bearers, bool clear)
{
    if (clear == false) {
        prov_ctx.bearers |= bearers;
    } else {
        prov_ctx.bearers &= ~bearers;
    }
}

bt_mesh_prov_bearer_t bt_mesh_provisioner_get_prov_bearer(void)
{
    return prov_ctx.bearers;
}

int bt_mesh_provisioner_set_static_oob_value(const u8_t *value, u8_t length)
{
    int i;

    if (value == NULL || length == 0U || length > 16U) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Make sure Static OOB is not being used. */
    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (link[i].auth_method == AUTH_METHOD_STATIC) {
            BT_ERR("%s, Static OOB is being used", __func__);
            return -EINVAL;
        }
    }

    (void)memset(prov_ctx.static_oob_val, 0, 16);

    prov_ctx.static_oob_len = MIN(16, length);
    memcpy(prov_ctx.static_oob_val, value, prov_ctx.static_oob_len);

    return 0;
}

u16_t bt_mesh_provisioner_get_primary_elem_addr(void)
{
    return prov_ctx.primary_addr;
}

int bt_mesh_provisioner_set_primary_elem_addr(u16_t addr)
{
    const struct bt_mesh_comp *comp = NULL;

    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("Invalid primary address 0x%04x", addr);
        return -EINVAL;
    }

    comp = bt_mesh_comp_get();
    if (!comp) {
        BT_ERR("NULL composition data");
        return -EINVAL;
    }

    /* Make sure Provisioner address is not identical with the addresses of nodes */
    if (bt_mesh_provisioner_check_is_addr_dup(addr, comp->elem_count, false)) {
        BT_ERR("Address 0x%04x is duplicated with node address", addr);
        return -EINVAL;
    }

    /* If the current can-be allocated address is bigger than primary address +
     * element number, then the curr_alloc_addr will not be changed, and only
     * the Provisioner related addresses will be updated.
     */
    if (addr + comp->elem_count > prov_ctx.curr_alloc_addr) {
        prov_ctx.curr_alloc_addr = addr + comp->elem_count;
    }
    BT_INFO("Provisioner primary address updated, old 0x%04x, new 0x%04x", prov_ctx.primary_addr, addr);
    prov_ctx.primary_addr = addr;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.curr_alloc_addr);
    }

    bt_mesh_comp_provision(addr);

    return 0;
}

/* The following APIs are for fast provisioning */

void bt_mesh_provisioner_fast_prov_enable(bool enable)
{
    prov_ctx.fast_prov.enable = enable;
}

u8_t bt_mesh_provisioner_set_fast_prov_net_idx(const u8_t *net_key, u16_t net_idx)
{
    prov_ctx.fast_prov.net_idx = net_idx;
    prov_ctx.fast_prov.net_key = net_key;

    if (!net_key) {
        BT_WARN("Wait for NetKey for fast provisioning");
        return 0x01; /*status: wait for net_key */
    }

    return 0x0; /* status: success */
}

u16_t bt_mesh_provisioner_get_fast_prov_net_idx(void)
{
    return prov_ctx.fast_prov.net_idx;
}

u8_t bt_mesh_set_fast_prov_unicast_addr_range(u16_t min, u16_t max)
{
    if (!BLE_MESH_ADDR_IS_UNICAST(min) || !BLE_MESH_ADDR_IS_UNICAST(max)) {
        BT_ERR("%s, Not a unicast address", __func__);
        return 0x01; /* status: not a unicast address */
    }

    if (min > max) {
        BT_ERR("%s, Min bigger than max", __func__);
        return 0x02; /* status: min is bigger than max */
    }

    if (min <= prov_ctx.fast_prov.unicast_addr_max) {
        BT_ERR("%s, Address overlap", __func__);
        return 0x03; /* status: address overlaps with current value */
    }

    prov_ctx.fast_prov.unicast_addr_min = min;
    prov_ctx.fast_prov.unicast_addr_max = max;

    prov_ctx.curr_alloc_addr = prov_ctx.fast_prov.unicast_addr_min;

    return 0x0; /* status: success */
}

void bt_mesh_set_fast_prov_flags_iv_index(u8_t flags, u32_t iv_index)
{
    /* BIT0: Key Refreash flag, BIT1: IV Update flag */
    prov_ctx.fast_prov.flags = flags & BIT_MASK(2);
    prov_ctx.fast_prov.iv_index = iv_index;
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static struct net_buf_simple *bt_mesh_pba_get_buf(const u8_t idx)
{
    struct net_buf_simple *buf = &(adv_buf[idx].buf);

    net_buf_simple_reset(buf);

    return buf;
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

static void prov_memory_free(const u8_t idx)
{
    PROV_FREE_MEM(idx, dhkey);
    PROV_FREE_MEM(idx, auth);
    PROV_FREE_MEM(idx, conf);
    PROV_FREE_MEM(idx, conf_salt);
    PROV_FREE_MEM(idx, conf_key);
    PROV_FREE_MEM(idx, conf_inputs);
    PROV_FREE_MEM(idx, prov_salt);
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static void buf_sent(int err, void *user_data)
{
    u8_t idx = (int)user_data;

    if (!link[idx].tx.buf[0]) {
        return;
    }

    k_delayed_work_submit(&link[idx].tx.retransmit, RETRANSMIT_TIMEOUT);
}

static struct bt_mesh_send_cb buf_sent_cb = {
    .end = buf_sent,
};

static void free_segments(const u8_t idx)
{
    int i;

    bt_mesh_pb_buf_lock();

    for (i = 0; i < ARRAY_SIZE(link[idx].tx.buf); i++) {
        struct net_buf *buf = link[idx].tx.buf[i];

        if (!buf) {
            break;
        }

        link[idx].tx.buf[i] = NULL;
        bt_mesh_adv_buf_ref_debug(__func__, buf, 3U, BLE_MESH_BUF_REF_SMALL);
        /* Mark as canceled */
        BLE_MESH_ADV(buf)->busy = 0U;
        net_buf_unref(buf);
    }

    bt_mesh_pb_buf_unlock();
}

static void prov_clear_tx(const u8_t idx)
{
    BT_DBG("%s", __func__);

    k_delayed_work_cancel(&link[idx].tx.retransmit);

    free_segments(idx);
}

static void reset_link(const u8_t idx, u8_t reason)
{
    prov_clear_tx(idx);

    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    if (prov->prov_link_close) {
        prov->prov_link_close(BLE_MESH_PROV_ADV, reason);
    }

    prov_memory_free(idx);

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
    /* Remove the link id from exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_REMOVE,
                                    BLE_MESH_EXCEP_INFO_MESH_LINK_ID, &link[idx].link_id);
#endif

    /* Clear everything except the retransmit delayed work config */
    memset(&link[idx], 0, offsetof(struct prov_link, tx.retransmit));

    link[idx].pending_ack = XACT_NVAL;
    link[idx].rx.prev_id  = XACT_NVAL;

    if (bt_mesh_pub_key_get()) {
        bt_mesh_atomic_set_bit(link[idx].flags, LOCAL_PUB_KEY);
    }

    link[idx].rx.buf = bt_mesh_pba_get_buf(idx);

    if (prov_ctx.pba_count) {
        prov_ctx.pba_count--;
    }
}

static struct net_buf *adv_buf_create(void)
{
    struct net_buf *buf = NULL;

    buf = bt_mesh_adv_create(BLE_MESH_ADV_PROV, PROV_XMIT, BUF_TIMEOUT);
    if (!buf) {
        BT_ERR("Out of provisioning buffers");
        return NULL;
    }

    return buf;
}

static void ack_complete(u16_t duration, int err, void *user_data)
{
    u8_t idx = (int)user_data;

    BT_DBG("xact %u complete", link[idx].pending_ack);

    link[idx].pending_ack = XACT_NVAL;
}

static void gen_prov_ack_send(const u8_t idx, u8_t xact_id)
{
    static const struct bt_mesh_send_cb cb = {
        .start = ack_complete,
    };
    const struct bt_mesh_send_cb *complete = NULL;
    struct net_buf *buf = NULL;

    BT_DBG("xact_id %u", xact_id);

    if (link[idx].pending_ack == xact_id) {
        BT_DBG("Not sending duplicate ack");
        return;
    }

    buf = adv_buf_create();
    if (!buf) {
        return;
    }

    if (link[idx].pending_ack == XACT_NVAL) {
        link[idx].pending_ack = xact_id;
        complete = &cb;
    } else {
        complete = NULL;
    }

    net_buf_add_be32(buf, link[idx].link_id);
    net_buf_add_u8(buf, xact_id);
    net_buf_add_u8(buf, GPC_ACK);

    bt_mesh_adv_send(buf, complete, (void *)(int)idx);
    net_buf_unref(buf);
}

static void send_reliable(const u8_t idx)
{
    int i;

    link[idx].tx.start = k_uptime_get();

    for (i = 0; i < ARRAY_SIZE(link[idx].tx.buf); i++) {
        struct net_buf *buf = link[idx].tx.buf[i];

        if (!buf) {
            break;
        }

        if (i + 1 < ARRAY_SIZE(link[idx].tx.buf) && link[idx].tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, &buf_sent_cb, (void *)(int)idx);
        }
    }
}

static int bearer_ctl_send(const u8_t idx, u8_t op, void *data, u8_t data_len)
{
    struct net_buf *buf = NULL;

    BT_DBG("op 0x%02x data_len %u", op, data_len);

    prov_clear_tx(idx);

    buf = adv_buf_create();
    if (!buf) {
        return -ENOBUFS;
    }

    net_buf_add_be32(buf, link[idx].link_id);
    /* Transaction ID, always 0 for Bearer messages */
    net_buf_add_u8(buf, 0x00);
    net_buf_add_u8(buf, GPC_CTL(op));
    net_buf_add_mem(buf, data, data_len);

    link[idx].tx.buf[0] = buf;
    send_reliable(idx);

    /** We can also use buf->ref and a flag to decide that
     *  link close has been sent 3 times.
     *  Here we use another way: use retransmit timer and need
     *  to make sure the timer is not cancelled during sending
     *  link close pdu, so we add link[i].tx.id = 0
     */
    if (op == LINK_CLOSE) {
        u8_t reason = *(u8_t *)data;
        link[idx].send_link_close = ((reason & BIT_MASK(2)) << 1) | BIT(0);
        link[idx].tx.trans_id = 0;
    }

    return 0;
}

static void send_link_open(const u8_t idx)
{
    int j;

    /** Generate link ID, and may need to check if this id is
     *  currently being used, which may will not happen ever.
     */
    bt_mesh_rand(&link[idx].link_id, sizeof(u32_t));
    while (1) {
        for (j = 0; j < CONFIG_BLE_MESH_PBA_SAME_TIME; j++) {
            if (bt_mesh_atomic_test_bit(link[j].flags, LINK_ACTIVE) || link[j].linking) {
                if (link[idx].link_id == link[j].link_id) {
                    bt_mesh_rand(&link[idx].link_id, sizeof(u32_t));
                    break;
                }
            }
        }
        if (j == CONFIG_BLE_MESH_PBA_SAME_TIME) {
            break;
        }
    }

#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
    /* Add the link id into exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_ADD,
                                    BLE_MESH_EXCEP_INFO_MESH_LINK_ID, &link[idx].link_id);
#endif

    bearer_ctl_send(idx, LINK_OPEN, link[idx].uuid, 16);

    /* If Provisioner sets LINK_ACTIVE flag once Link Open is sent, we have
     * no need to use linking flag (like PB-GATT connecting) to prevent the
     * stored device info (UUID, oob_info) being replaced by other received
     * unprovisioned device beacons.
     * But if Provisioner sets LINK_ACTIVE flag after Link ACK is received,
     * we need to use linking flag to prevent device info being replaced.
     * Currently we set LINK_ACTIVE flag after sending Link Open.
     */
    link[idx].linking = true;

    /* Set LINK_ACTIVE just to be in compatibility with current Zephyr code */
    bt_mesh_atomic_set_bit(link[idx].flags, LINK_ACTIVE);

    if (prov->prov_link_open) {
        prov->prov_link_open(BLE_MESH_PROV_ADV);
    }

    prov_ctx.pba_count++;
}

static u8_t last_seg(u8_t len)
{
    if (len <= START_PAYLOAD_MAX) {
        return 0;
    }

    len -= START_PAYLOAD_MAX;

    return 1 + (len / CONT_PAYLOAD_MAX);
}

static inline u8_t next_transaction_id(const u8_t idx)
{
    if (link[idx].tx.trans_id > 0x7F) {
        link[idx].tx.trans_id = 0x0;
    }
    return link[idx].tx.trans_id++;
}

static int prov_send_adv(const u8_t idx, struct net_buf_simple *msg)
{
    struct net_buf *start = NULL, *buf = NULL;
    u8_t seg_len = 0U, seg_id = 0U;
    u8_t xact_id = 0U;
    s32_t timeout = PROVISION_TIMEOUT;

    BT_DBG("%s, len %u: %s", __func__, msg->len, bt_hex(msg->data, msg->len));

    prov_clear_tx(idx);

    start = adv_buf_create();
    if (!start) {
        return -ENOBUFS;
    }

    xact_id = next_transaction_id(idx);
    net_buf_add_be32(start, link[idx].link_id);
    net_buf_add_u8(start, xact_id);

    net_buf_add_u8(start, GPC_START(last_seg(msg->len)));
    net_buf_add_be16(start, msg->len);
    net_buf_add_u8(start, bt_mesh_fcs_calc(msg->data, msg->len));

    link[idx].tx.buf[0] = start;
    /* Changed by Espressif, get message type */
    link[idx].tx_pdu_type = msg->data[0];

    seg_len = MIN(msg->len, START_PAYLOAD_MAX);
    BT_DBG("seg 0 len %u: %s", seg_len, bt_hex(msg->data, seg_len));
    net_buf_add_mem(start, msg->data, seg_len);
    net_buf_simple_pull(msg, seg_len);

    buf = start;
    for (seg_id = 1; msg->len > 0; seg_id++) {
        if (seg_id >= ARRAY_SIZE(link[idx].tx.buf)) {
            BT_ERR("%s, Too big message", __func__);
            free_segments(idx);
            return -E2BIG;
        }

        buf = adv_buf_create();
        if (!buf) {
            free_segments(idx);
            return -ENOBUFS;
        }

        link[idx].tx.buf[seg_id] = buf;

        seg_len = MIN(msg->len, CONT_PAYLOAD_MAX);

        BT_DBG("seg_id %u len %u: %s", seg_id, seg_len,
               bt_hex(msg->data, seg_len));

        net_buf_add_be32(buf, link[idx].link_id);
        net_buf_add_u8(buf, xact_id);
        net_buf_add_u8(buf, GPC_CONT(seg_id));
        net_buf_add_mem(buf, msg->data, seg_len);
        net_buf_simple_pull(msg, seg_len);
    }

    send_reliable(idx);

#if defined(CONFIG_BLE_MESH_FAST_PROV)
    if (link[idx].tx_pdu_type >= PROV_DATA) {
        timeout = K_SECONDS(60);
    }
#endif
    if (!bt_mesh_atomic_test_and_set_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_submit(&link[idx].timeout, timeout);
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
static int prov_send_gatt(const u8_t idx, struct net_buf_simple *msg)
{
    int err = 0;

    if (!link[idx].conn) {
        return -ENOTCONN;
    }

    err = bt_mesh_proxy_prov_client_send(link[idx].conn, BLE_MESH_PROXY_PROV, msg);
    if (err) {
        BT_ERR("%s, Failed to send PB-GATT pdu", __func__);
        return err;
    }

    if (!bt_mesh_atomic_test_and_set_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_submit(&link[idx].timeout, PROVISION_TIMEOUT);
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

static inline int prov_send(const u8_t idx, struct net_buf_simple *buf)
{
#if defined(CONFIG_BLE_MESH_PB_ADV)
    if (idx < CONFIG_BLE_MESH_PBA_SAME_TIME) {
        return prov_send_adv(idx, buf);
    }
#endif

#if defined(CONFIG_BLE_MESH_PB_GATT)
    if (idx < BLE_MESH_PROV_SAME_TIME
#if defined(CONFIG_BLE_MESH_PB_ADV)
            && idx >= CONFIG_BLE_MESH_PBA_SAME_TIME
#endif
       ) {
        return prov_send_gatt(idx, buf);
    }
#endif

    BT_ERR("%s, Invalid link index %d", __func__, idx);
    return -EINVAL;
}

static void prov_buf_init(struct net_buf_simple *buf, u8_t type)
{
    net_buf_simple_reserve(buf, PROV_BUF_HEADROOM);
    net_buf_simple_add_u8(buf, type);
}

static void prov_invite(const u8_t idx, const u8_t *data)
{
    BT_DBG("%s", __func__);
}

static void prov_start(const u8_t idx, const u8_t *data)
{
    BT_DBG("%s", __func__);
}

static void prov_data(const u8_t idx, const u8_t *data)
{
    BT_DBG("%s", __func__);
}

static void send_invite(const u8_t idx)
{
    PROV_BUF(buf, 2);

    prov_buf_init(&buf, PROV_INVITE);

    net_buf_simple_add_u8(&buf, prov->prov_attention);

    link[idx].conf_inputs[0] = prov->prov_attention;

    if (prov_send(idx, &buf)) {
        BT_ERR("%s, Failed to send Provisioning Invite", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    link[idx].expect = PROV_CAPABILITIES;
}

static void prov_capabilities(const u8_t idx, const u8_t *data)
{
    PROV_BUF(buf, 6);
    u16_t algorithms = 0U, output_action = 0U, input_action = 0U;
    u8_t  element_num = 0U, pub_key_oob = 0U, static_oob = 0U,
          output_size = 0U, input_size = 0U;
    u8_t  auth_method = 0U, auth_action = 0U, auth_size = 0U;

    element_num = data[0];
    BT_INFO("Elements:          0x%02x", element_num);
    if (!element_num) {
        BT_ERR("%s, Invalid element number", __func__);
        goto fail;
    }
    link[idx].element_num = element_num;

    algorithms = sys_get_be16(&data[1]);
    BT_INFO("Algorithms:        0x%04x", algorithms);
    if (algorithms != BIT(PROV_ALG_P256)) {
        BT_ERR("%s, Invalid algorithms", __func__);
        goto fail;
    }

    pub_key_oob = data[3];
    BT_INFO("Public Key Type:   0x%02x", pub_key_oob);
    if (pub_key_oob > 0x01) {
        BT_ERR("%s, Invalid public key type", __func__);
        goto fail;
    }
    pub_key_oob = ((prov->prov_pub_key_oob &&
                    prov->prov_pub_key_oob_cb) ? pub_key_oob : 0x00);

    static_oob = data[4];
    BT_INFO("Static OOB Type:   0x%02x", static_oob);
    if (static_oob > 0x01) {
        BT_ERR("%s, Invalid Static OOB type", __func__);
        goto fail;
    }
    static_oob = (prov_ctx.static_oob_len ? static_oob : 0x00);

    output_size = data[5];
    BT_INFO("Output OOB Size:   0x%02x", output_size);
    if (output_size > 0x08) {
        BT_ERR("%s, Invalid Output OOB size", __func__);
        goto fail;
    }

    output_action = sys_get_be16(&data[6]);
    BT_INFO("Output OOB Action: 0x%04x", output_action);
    if (output_action > 0x1f) {
        BT_ERR("%s, Invalid Output OOB action", __func__);
        goto fail;
    }

    /* Provisioner select output action */
    if (prov->prov_input_num && output_size) {
        output_action = __builtin_ctz(output_action);
    } else {
        output_size = 0x0;
        output_action = 0x0;
    }

    input_size = data[8];
    BT_INFO("Input OOB Size:    0x%02x", input_size);
    if (input_size > 0x08) {
        BT_ERR("%s, Invalid Input OOB size", __func__);
        goto fail;
    }

    input_action = sys_get_be16(&data[9]);
    BT_INFO("Input OOB Action:  0x%04x", input_action);
    if (input_action > 0x0f) {
        BT_ERR("%s, Invalid Input OOB action", __func__);
        goto fail;
    }

    /* Make sure received pdu is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    /* Provisioner select input action */
    if (prov->prov_output_num && input_size) {
        input_action = __builtin_ctz(input_action);
    } else {
        input_size = 0x0;
        input_action = 0x0;
    }

    if (static_oob) {
        /* if static oob is valid, just use static oob */
        auth_method = AUTH_METHOD_STATIC;
        auth_action = 0x00;
        auth_size   = 0x00;
    } else {
        if (!output_size && !input_size) {
            auth_method = AUTH_METHOD_NO_OOB;
            auth_action = 0x00;
            auth_size   = 0x00;
        } else if (!output_size && input_size) {
            auth_method = AUTH_METHOD_INPUT;
            auth_action = (u8_t)input_action;
            auth_size   = input_size;
        } else {
            auth_method = AUTH_METHOD_OUTPUT;
            auth_action = (u8_t)output_action;
            auth_size   = output_size;
        }
    }

    /* Store provisioning capbilities value in conf_inputs */
    memcpy(&link[idx].conf_inputs[1], data, 11);

    prov_buf_init(&buf, PROV_START);
    net_buf_simple_add_u8(&buf, prov->prov_algorithm);
    net_buf_simple_add_u8(&buf, pub_key_oob);
    net_buf_simple_add_u8(&buf, auth_method);
    net_buf_simple_add_u8(&buf, auth_action);
    net_buf_simple_add_u8(&buf, auth_size);

    memcpy(&link[idx].conf_inputs[12], &buf.data[1], 5);

    if (prov_send(idx, &buf)) {
        BT_ERR("%s, Failed to send Provisioning Start", __func__);
        goto fail;
    }

    link[idx].auth_method = auth_method;
    link[idx].auth_action = auth_action;
    link[idx].auth_size   = auth_size;

    /** After prov start sent, use OOB to get remote public key.
     *  And we just follow the procedure in Figure 5.15 of Section
     *  5.4.2.3 of Mesh Profile Spec.
     */
    if (pub_key_oob) {
        if (prov->prov_pub_key_oob_cb(idx)) {
            BT_ERR("%s, Failed to notify input OOB Public Key", __func__);
            goto fail;
        }
    }

    /** If using PB-ADV, need to listen for transaction ack,
     *  after ack is received, provisioner can send public key.
     */
#if defined(CONFIG_BLE_MESH_PB_ADV)
    if (idx < CONFIG_BLE_MESH_PBA_SAME_TIME) {
        link[idx].expect_ack_for = PROV_START;
        return;
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

    send_pub_key(idx, pub_key_oob);
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

static bt_mesh_output_action_t output_action(u8_t action)
{
    switch (action) {
    case OUTPUT_OOB_BLINK:
        return BLE_MESH_BLINK;
    case OUTPUT_OOB_BEEP:
        return BLE_MESH_BEEP;
    case OUTPUT_OOB_VIBRATE:
        return BLE_MESH_VIBRATE;
    case OUTPUT_OOB_NUMBER:
        return BLE_MESH_DISPLAY_NUMBER;
    case OUTPUT_OOB_STRING:
        return BLE_MESH_DISPLAY_STRING;
    default:
        return BLE_MESH_NO_OUTPUT;
    }
}

static bt_mesh_input_action_t input_action(u8_t action)
{
    switch (action) {
    case INPUT_OOB_PUSH:
        return BLE_MESH_PUSH;
    case INPUT_OOB_TWIST:
        return BLE_MESH_TWIST;
    case INPUT_OOB_NUMBER:
        return BLE_MESH_ENTER_NUMBER;
    case INPUT_OOB_STRING:
        return BLE_MESH_ENTER_STRING;
    default:
        return BLE_MESH_NO_INPUT;
    }
}

static int prov_auth(const u8_t idx, u8_t method, u8_t action, u8_t size)
{
    bt_mesh_output_action_t output = 0U;
    bt_mesh_input_action_t input = 0U;

    link[idx].auth = (u8_t *)bt_mesh_calloc(PROV_AUTH_VAL_SIZE);
    if (!link[idx].auth) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return -ENOMEM;
    }

    switch (method) {
    case AUTH_METHOD_NO_OOB:
        if (action || size) {
            return -EINVAL;
        }
        memset(link[idx].auth, 0, 16);
        return 0;

    case AUTH_METHOD_STATIC:
        if (action || size) {
            return -EINVAL;
        }
        memcpy(link[idx].auth + 16 - prov_ctx.static_oob_len,
               prov_ctx.static_oob_val, prov_ctx.static_oob_len);
        memset(link[idx].auth, 0, 16 - prov_ctx.static_oob_len);
        return 0;

    case AUTH_METHOD_OUTPUT:
        /* Use auth_action to get device output action */
        output = output_action(action);
        if (!output) {
            return -EINVAL;
        }
        return prov->prov_input_num(AUTH_METHOD_OUTPUT, output, size, idx);

    case AUTH_METHOD_INPUT:
        /* Use auth_action to get device input action */
        input = input_action(action);
        if (!input) {
            return -EINVAL;
        }

        /* Provisioner ouputs number/string and wait for device's Provisioning Input Complete PDU */
        link[idx].expect = PROV_INPUT_COMPLETE;

        if (input == BLE_MESH_ENTER_STRING) {
            unsigned char str[9] = {'\0'};
            u8_t j = 0U;

            bt_mesh_rand(str, size);

            /* Normalize to '0' .. '9' & 'A' .. 'Z' */
            for (j = 0U; j < size; j++) {
                str[j] %= 36;
                if (str[j] < 10) {
                    str[j] += '0';
                } else {
                    str[j] += 'A' - 10;
                }
            }
            str[size] = '\0';

            memcpy(link[idx].auth, str, size);
            memset(link[idx].auth + size, 0, PROV_AUTH_VAL_SIZE - size);

            return prov->prov_output_num(AUTH_METHOD_INPUT, input, str, size, idx);
        } else {
            u32_t div[8] = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };
            u32_t num = 0U;

            bt_mesh_rand(&num, sizeof(num));
            num %= div[size - 1];

            sys_put_be32(num, &link[idx].auth[12]);
            memset(link[idx].auth, 0, 12);

            return prov->prov_output_num(AUTH_METHOD_INPUT, input, &num, size, idx);
        }

    default:
        return -EINVAL;
    }
}

static void send_confirm(const u8_t idx)
{
    PROV_BUF(buf, 17);

    BT_DBG("ConfInputs[0]   %s", bt_hex(link[idx].conf_inputs, 64));
    BT_DBG("ConfInputs[64]  %s", bt_hex(link[idx].conf_inputs + 64, 64));
    BT_DBG("ConfInputs[128] %s", bt_hex(link[idx].conf_inputs + 128, 17));

    link[idx].conf_salt = (u8_t *)bt_mesh_calloc(PROV_CONF_SALT_SIZE);
    if (!link[idx].conf_salt) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        goto fail;
    }

    link[idx].conf_key = (u8_t *)bt_mesh_calloc(PROV_CONF_KEY_SIZE);
    if (!link[idx].conf_key) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        goto fail;
    }

    if (bt_mesh_prov_conf_salt(link[idx].conf_inputs, link[idx].conf_salt)) {
        BT_ERR("%s, Failed to generate confirmation salt", __func__);
        goto fail;
    }

    BT_DBG("ConfirmationSalt: %s", bt_hex(link[idx].conf_salt, 16));

    if (bt_mesh_prov_conf_key(link[idx].dhkey, link[idx].conf_salt, link[idx].conf_key)) {
        BT_ERR("%s, Failed to generate confirmation key", __func__);
        goto fail;
    }

    BT_DBG("ConfirmationKey: %s", bt_hex(link[idx].conf_key, 16));

    /** Provisioner use the same random number for each provisioning
     *  device, if different random need to be used, here provisioner
     *  should allocate memory for rand and call bt_mesh_rand() every time.
     */
    if (!(prov_ctx.rand_gen_done & BIT(0))) {
        if (bt_mesh_rand(prov_ctx.random, 16)) {
            BT_ERR("%s, Failed to generate random number", __func__);
            goto fail;
        }
        link[idx].rand = prov_ctx.random;
        prov_ctx.rand_gen_done |= BIT(0);
    } else {
        /* Provisioner random has already been generated. */
        link[idx].rand = prov_ctx.random;
    }

    BT_DBG("LocalRandom: %s", bt_hex(link[idx].rand, 16));

    prov_buf_init(&buf, PROV_CONFIRM);

    if (bt_mesh_prov_conf(link[idx].conf_key, link[idx].rand, link[idx].auth,
                          net_buf_simple_add(&buf, 16))) {
        BT_ERR("%s, Failed to generate confirmation value", __func__);
        goto fail;
    }

    if (prov_send(idx, &buf)) {
        BT_ERR("%s, Failed to send Provisioning Confirm", __func__);
        goto fail;
    }

    link[idx].expect = PROV_CONFIRM;
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

int bt_mesh_provisioner_set_oob_input_data(const u8_t idx, const u8_t *val, bool num_flag)
{
    /** This function should be called in the prov_input_num
     *  callback, after the data output by device has been
     *  input by provisioner.
     *  Paramter size is used to indicate the length of data
     *  indicated by Pointer val, for example, if device output
     *  data is 12345678(decimal), the data in auth value will
     *  be 0xBC614E.
     *  Parameter num_flag is used to indicate whether the value
     *  input by provisioner is number or string.
     */
    if (!link[idx].auth) {
        BT_ERR("%s, Link auth is NULL", __func__);
        return -EINVAL;
    }

    BT_INFO("Link idx %d, type %s", idx, num_flag ? "number" : "string");

    memset(link[idx].auth, 0, 16);
    if (num_flag) {
        /* Provisioner inputs number */
        memcpy(link[idx].auth + 12, val, sizeof(u32_t));
    } else {
        /* Provisioner inputs string */
        memcpy(link[idx].auth, val, link[idx].auth_size);
    }

    send_confirm(idx);
    return 0;
}

int bt_mesh_provisioner_set_oob_output_data(const u8_t idx, const u8_t *num, u8_t size, bool num_flag)
{
    /** This function should be called in the prov_output_num
     *  callback, after the data has been output by provisioner.
     *  Parameter size is used to indicate the length of data
     *  indicated by Pointer num, for example, if provisioner
     *  output data is 12345678(decimal), the data in auth value
     *  will be 0xBC614E.
     *  Parameter num_flag is used to indicate whether the value
     *  output by provisioner is number or string.
     */
    if (!link[idx].auth) {
        BT_ERR("%s, link auth is NULL", __func__);
        return -EINVAL;
    }

    BT_INFO("Link idx %d, type %s", idx, num_flag ? "number" : "string");

    if (num_flag) {
        /* Provisioner output number */
        memset(link[idx].auth, 0, 16);
        memcpy(link[idx].auth + 16 - size, num, size);
    } else {
        /* Provisioner output string */
        memset(link[idx].auth, 0, 16);
        memcpy(link[idx].auth, num, size);
    }

    link[idx].expect = PROV_INPUT_COMPLETE;

    return 0;
}

int bt_mesh_provisioner_read_oob_pub_key(const u8_t idx, const u8_t pub_key_x[32], const u8_t pub_key_y[32])
{
    if (!link[idx].conf_inputs) {
        BT_ERR("%s, Link conf_inputs is NULL", __func__);
        return -EINVAL;
    }

    /* Swap X and Y halves independently to big-endian */
    sys_memcpy_swap(&link[idx].conf_inputs[81], pub_key_x, 32);
    sys_memcpy_swap(&link[idx].conf_inputs[81] + 32, pub_key_y, 32);

    bt_mesh_atomic_set_bit(link[idx].flags, REMOTE_PUB_KEY);

    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, WAIT_GEN_DHKEY)) {
        prov_gen_dh_key(idx);
    }

    return 0;
}

static void prov_dh_key_cb(const u8_t key[32], const u8_t idx)
{
    BT_DBG("%p", key);

    if (!key) {
        BT_ERR("%s, Failed to generate DHKey", __func__);
        goto fail;
    }

    link[idx].dhkey = (u8_t *)bt_mesh_calloc(PROV_DH_KEY_SIZE);
    if (!link[idx].dhkey) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        goto fail;
    }
    sys_memcpy_swap(link[idx].dhkey, key, 32);

    BT_DBG("DHkey: %s", bt_hex(link[idx].dhkey, 32));

    bt_mesh_atomic_set_bit(link[idx].flags, HAVE_DHKEY);

    /** After dhkey is generated, if auth_method is No OOB or
     *  Static OOB, provisioner can start to send confirmation.
     *  If output OOB is used by the device, provisioner need
     *  to watch out the output number and input it as auth_val.
     *  If input OOB is used by the device, provisioner need
     *  to output a value, and wait for prov input complete pdu.
     */
    if (prov_auth(idx, link[idx].auth_method,
                  link[idx].auth_action, link[idx].auth_size) < 0) {
        BT_ERR("%s, Failed to authenticate", __func__);
        goto fail;
    }
    if (link[idx].auth_method == AUTH_METHOD_OUTPUT ||
            link[idx].auth_method == AUTH_METHOD_INPUT) {
        return;
    }

    if (link[idx].expect != PROV_INPUT_COMPLETE) {
        send_confirm(idx);
    }
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

static void prov_gen_dh_key(const u8_t idx)
{
    u8_t pub_key[64] = {0};

    /* Copy device public key in little-endian for bt_mesh_dh_key_gen().
     * X and Y halves are swapped independently.
     */
    sys_memcpy_swap(&pub_key[0], &link[idx].conf_inputs[81], 32);
    sys_memcpy_swap(&pub_key[32], &link[idx].conf_inputs[113], 32);

    if (bt_mesh_dh_key_gen(pub_key, prov_dh_key_cb, idx)) {
        BT_ERR("%s, Failed to generate DHKey", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }
}

static void send_pub_key(const u8_t idx, u8_t oob)
{
    PROV_BUF(buf, 65);
    const u8_t *key = NULL;

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("%s, No public key available", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("Local Public Key: %s", bt_hex(key, 64));

    bt_mesh_atomic_set_bit(link[idx].flags, LOCAL_PUB_KEY);

    prov_buf_init(&buf, PROV_PUB_KEY);

    /* Swap X and Y halves independently to big-endian */
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), key, 32);
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), &key[32], 32);

    /* Store provisioner public key value in conf_inputs */
    memcpy(&link[idx].conf_inputs[17], &buf.data[1], 64);

    if (prov_send(idx, &buf)) {
        BT_ERR("%s, Failed to send Provisioning Public Key", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (!oob) {
        link[idx].expect = PROV_PUB_KEY;
    } else {
        /** Have already got device public key. If next is to
         *  send confirm(not wait for input complete), need to
         *  wait for transactiona ack for public key then send
         *  provisioning confirm pdu.
         */
#if defined(CONFIG_BLE_MESH_PB_ADV)
        if (idx < CONFIG_BLE_MESH_PBA_SAME_TIME) {
            link[idx].expect_ack_for = PROV_PUB_KEY;
            return;
        }
#endif /* CONFIG_BLE_MESH_PB_ADV */

        /* If remote public key has been read, then start to generate DHkey,
         * otherwise wait for device oob public key.
         */
        if (bt_mesh_atomic_test_bit(link[idx].flags, REMOTE_PUB_KEY)) {
            prov_gen_dh_key(idx);
        } else {
            bt_mesh_atomic_set_bit(link[idx].flags, WAIT_GEN_DHKEY);
        }
    }
}

static void prov_pub_key(const u8_t idx, const u8_t *data)
{
    BT_DBG("Remote Public Key: %s", bt_hex(data, 64));

    /* Make sure received pdu is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    memcpy(&link[idx].conf_inputs[81], data, 64);

    if (!bt_mesh_atomic_test_bit(link[idx].flags, LOCAL_PUB_KEY)) {
        /* Clear retransmit timer */
#if defined(CONFIG_BLE_MESH_PB_ADV)
        prov_clear_tx(idx);
#endif
        bt_mesh_atomic_set_bit(link[idx].flags, REMOTE_PUB_KEY);
        BT_WARN("Waiting for local public key");
        return;
    }

    prov_gen_dh_key(idx);
}

static void prov_input_complete(const u8_t idx, const u8_t *data)
{
    /* Make sure received pdu is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    /* Provisioner receives input complete and send confirm */
    send_confirm(idx);
}

static void prov_confirm(const u8_t idx, const u8_t *data)
{
    /**
     * Zephyr uses PROV_BUF(16). Currently test with PROV_BUF(16)
     * and PROV_BUF(17) on branch feature/btdm_ble_mesh_debug both
     * work fine.
     */
    PROV_BUF(buf, 17);

    BT_DBG("Remote Confirm: %s", bt_hex(data, 16));

    /* Make sure received pdu is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    link[idx].conf = (u8_t *)bt_mesh_calloc(PROV_CONFIRM_SIZE);
    if (!link[idx].conf) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    memcpy(link[idx].conf, data, 16);

    if (!bt_mesh_atomic_test_bit(link[idx].flags, HAVE_DHKEY)) {
#if defined(CONFIG_BLE_MESH_PB_ADV)
        prov_clear_tx(idx);
#endif
        bt_mesh_atomic_set_bit(link[idx].flags, SEND_CONFIRM);
    }

    prov_buf_init(&buf, PROV_RANDOM);

    net_buf_simple_add_mem(&buf, link[idx].rand, 16);

    if (prov_send(idx, &buf)) {
        BT_ERR("%s, Failed to send Provisioning Random", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    link[idx].expect = PROV_RANDOM;
}

static void send_prov_data(const u8_t idx)
{
    PROV_BUF(buf, 34);
    u16_t prev_addr = BLE_MESH_ADDR_UNASSIGNED;
    u16_t max_addr = BLE_MESH_ADDR_UNASSIGNED;
    struct bt_mesh_node *node = NULL;
    const u8_t *netkey = NULL;
    u8_t session_key[16] = {0};
    u8_t nonce[13] = {0};
    u8_t pdu[25] = {0};
    int err = 0;

    err = bt_mesh_session_key(link[idx].dhkey, link[idx].prov_salt, session_key);
    if (err) {
        BT_ERR("%s, Failed to generate session key", __func__);
        goto fail;
    }
    BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

    err = bt_mesh_prov_nonce(link[idx].dhkey, link[idx].prov_salt, nonce);
    if (err) {
        BT_ERR("%s, Failed to generate session nonce", __func__);
        goto fail;
    }
    BT_DBG("Nonce: %s", bt_hex(nonce, 13));

    /* Assign provisioning data for the device. Currently all provisioned devices
     * will be added to the primary subnet, and may add an API to choose to which
     * subnet will the device be provisioned later.
     */
    if (FAST_PROV_ENABLE()) {
        netkey = prov_ctx.fast_prov.net_key;
        if (!netkey) {
            BT_ERR("%s, Failed to get NetKey for fast provisioning", __func__);
            goto fail;
        }
        memcpy(pdu, netkey, 16);
        sys_put_be16(prov_ctx.fast_prov.net_idx, &pdu[16]);
        pdu[18] = prov_ctx.fast_prov.flags;
        sys_put_be32(prov_ctx.fast_prov.iv_index, &pdu[19]);
    } else {
        netkey = bt_mesh_provisioner_net_key_get(prov_ctx.curr_net_idx);
        if (!netkey) {
            BT_ERR("%s, Failed to get NetKey for provisioning data", __func__);
            goto fail;
        }
        memcpy(pdu, netkey, 16);
        sys_put_be16(prov_ctx.curr_net_idx, &pdu[16]);
        pdu[18] = prov_ctx.curr_flags;
        sys_put_be32(prov_ctx.curr_iv_index, &pdu[19]);
    }

    /**
     * The Provisioner must not reuse unicast addresses that have been
     * allocated to a device and sent in a Provisioning Data PDU until
     * the Provisioner receives an Unprovisioned Device beacon or
     * Service Data for the Mesh Provisioning Service from that same
     * device, identified using the Device UUID of the device.
     */

    /* Check if this device is a re-provisioned device */
    node = bt_mesh_provisioner_get_node_with_uuid(link[idx].uuid);
    if (node) {
        if (link[idx].element_num <= node->element_num) {
            /**
             * If the device is provisioned before, but the element number of
             * the device is bigger now, then we treate it as a new device.
             */
            prev_addr = node->unicast_addr;
        }
        bt_mesh_provisioner_remove_node(link[idx].uuid);
    }

    max_addr = FAST_PROV_ENABLE() ? prov_ctx.fast_prov.unicast_addr_max : PROV_MAX_ADDR_TO_ASSIGN;

    if (BLE_MESH_ADDR_IS_UNICAST(prev_addr)) {
        sys_put_be16(prev_addr, &pdu[23]);
        link[idx].unicast_addr = prev_addr;
    } else {
        if (BLE_MESH_ADDR_IS_UNICAST(link[idx].assign_addr)) {
            if (link[idx].assign_addr + link[idx].element_num - 1 > max_addr) {
                BT_ERR("%s, Too large assigned address for the device", __func__);
                goto fail;
            }

            /* Make sure the assigned unicast address is not identical with any unicast address
             * of other nodes. Also need to make sure the address is not identical with any
             * address of Provisioner.
             */
            if (bt_mesh_provisioner_check_is_addr_dup(link[idx].assign_addr, link[idx].element_num, true)) {
                BT_ERR("%s, Assigned address 0x%04x is duplicated", __func__, link[idx].assign_addr);
                goto fail;
            }

            sys_put_be16(link[idx].assign_addr, &pdu[23]);
            link[idx].unicast_addr = link[idx].assign_addr;
        } else {
            /* If this device to be provisioned is a new device */
            if (prov_ctx.curr_alloc_addr == BLE_MESH_ADDR_UNASSIGNED) {
                BT_ERR("%s, No unicast address can be allocated", __func__);
                goto fail;
            }

            if (prov_ctx.curr_alloc_addr + link[idx].element_num - 1 > max_addr) {
                BT_ERR("%s, Not enough unicast address for the device", __func__);
                goto fail;
            }

            sys_put_be16(prov_ctx.curr_alloc_addr, &pdu[23]);
            link[idx].unicast_addr = prov_ctx.curr_alloc_addr;
        }
    }

    prov_buf_init(&buf, PROV_DATA);

    err = bt_mesh_prov_encrypt(session_key, nonce, pdu, net_buf_simple_add(&buf, 33));
    if (err) {
        BT_ERR("%s, Failed to encrypt provisioning data", __func__);
        goto fail;
    }

    if (prov_send(idx, &buf)) {
        BT_ERR("%s, Failed to send Provisioning Data", __func__);
        goto fail;
    }

    /**
     * We update the next unicast address to be allocated here because if
     * Provisioner is provisioning two devices at the same time, we need
     * to assign the unicast address for them correctly. Hence we should
     * not update the prov_ctx.curr_alloc_addr after the proper provisioning
     * complete pdu is received.
     */
    if (!BLE_MESH_ADDR_IS_UNICAST(prev_addr)) {
        if (BLE_MESH_ADDR_IS_UNICAST(link[idx].assign_addr)) {
            /* Even if the unicast address of the node is assigned by the
             * application, we will also update the prov_ctx.curr_alloc_addr
             * here, in case Users use the two methods together (i.e. allocate
             * the unicast address for the node internally and assign the
             * unicast address for the node from application).
             */
            if (prov_ctx.curr_alloc_addr < link[idx].assign_addr + link[idx].element_num) {
                prov_ctx.curr_alloc_addr = link[idx].assign_addr + link[idx].element_num;
            }
        } else {
            prov_ctx.curr_alloc_addr += link[idx].element_num;
            if (prov_ctx.curr_alloc_addr > max_addr) {
                /* No unicast address will be used for further provisioning */
                prov_ctx.curr_alloc_addr = BLE_MESH_ADDR_UNASSIGNED;
            }
        }
        /* Store the available unicast address range to flash */
        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.curr_alloc_addr);
        }
    }

    if (FAST_PROV_ENABLE()) {
        link[idx].ki_flags = prov_ctx.fast_prov.flags;
        link[idx].iv_index = prov_ctx.fast_prov.iv_index;
    } else {
        link[idx].ki_flags = prov_ctx.curr_flags;
        link[idx].iv_index = prov_ctx.curr_iv_index;
    }

    link[idx].expect = PROV_COMPLETE;
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

static void prov_random(const u8_t idx, const u8_t *data)
{
    u8_t conf_verify[16] = {0};

    BT_DBG("Remote Random: %s", bt_hex(data, 16));

    if (bt_mesh_prov_conf(link[idx].conf_key, data, link[idx].auth, conf_verify)) {
        BT_ERR("%s, Failed to calculate confirmation verification", __func__);
        goto fail;
    }

    if (memcmp(conf_verify, link[idx].conf, 16)) {
        BT_ERR("%s, Invalid confirmation value", __func__);
        BT_DBG("Received:   %s", bt_hex(link[idx].conf, 16));
        BT_DBG("Calculated: %s",  bt_hex(conf_verify, 16));
        goto fail;
    }

    /*Verify received confirm is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    /** After provisioner receives provisioning random from device,
     *  and successfully check the confirmation, the following
     *  should be done:
     *  1. bt_mesh_calloc memory for prov_salt
     *  2. calculate prov_salt
     *  3. prepare provisioning data and send
     */
    link[idx].prov_salt = (u8_t *)bt_mesh_calloc(PROV_PROV_SALT_SIZE);
    if (!link[idx].prov_salt) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        goto fail;
    }

    if (bt_mesh_prov_salt(link[idx].conf_salt, link[idx].rand, data,
                          link[idx].prov_salt)) {
        BT_ERR("%s, Failed to generate ProvisioningSalt", __func__);
        goto fail;
    }

    BT_DBG("ProvisioningSalt: %s", bt_hex(link[idx].prov_salt, 16));

    send_prov_data(idx);
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

static void prov_complete(const u8_t idx, const u8_t *data)
{
    u8_t device_key[16] = {0};
    u16_t net_idx = 0U;
    u16_t index = 0U;
    u16_t rm = 0U;
    int err = 0;

    /* Make sure received pdu is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    err = bt_mesh_dev_key(link[idx].dhkey, link[idx].prov_salt, device_key);
    if (err) {
        BT_ERR("%s, Failed to generate device key", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (FAST_PROV_ENABLE()) {
        net_idx = prov_ctx.fast_prov.net_idx;
    } else {
        net_idx = prov_ctx.curr_net_idx;
    }
    err = bt_mesh_provisioner_provision(&link[idx].addr, link[idx].uuid, link[idx].oob_info,
                                        link[idx].unicast_addr, link[idx].element_num, net_idx,
                                        link[idx].ki_flags, link[idx].iv_index, device_key, &index);
    if (err) {
        BT_ERR("%s, Failed to store node info", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (prov->prov_complete) {
        prov->prov_complete(index, link[idx].uuid, link[idx].unicast_addr,
                            link[idx].element_num, net_idx);
    }

    err = provisioner_dev_find(&link[idx].addr, link[idx].uuid, &rm);
    if (!err) {
        if (unprov_dev[rm].flags & RM_AFTER_PROV) {
            memset(&unprov_dev[rm], 0, sizeof(struct unprov_dev_queue));
        }
    } else if (err == -ENODEV) {
        BT_DBG("Device is not found in queue");
    } else {
        BT_ERR("Failed to remove device from queue");
    }

    close_link(idx, CLOSE_REASON_SUCCESS);
}

static void prov_failed(const u8_t idx, const u8_t *data)
{
    BT_WARN("%s, Error 0x%02x", __func__, data[0]);

    close_link(idx, CLOSE_REASON_FAILED);
}

static const struct {
    void (*func)(const u8_t idx, const u8_t *data);
    u16_t len;
} prov_handlers[] = {
    { prov_invite,         1  },
    { prov_capabilities,   11 },
    { prov_start,          5  },
    { prov_pub_key,        64 },
    { prov_input_complete, 0  },
    { prov_confirm,        16 },
    { prov_random,         16 },
    { prov_data,           33 },
    { prov_complete,       0  },
    { prov_failed,         1  },
};

static void close_link(const u8_t idx, u8_t reason)
{
#if defined(CONFIG_BLE_MESH_PB_ADV)
    if (idx < CONFIG_BLE_MESH_PBA_SAME_TIME) {
        bearer_ctl_send(idx, LINK_CLOSE, &reason, sizeof(reason));
        return;
    }
#endif

#if defined(CONFIG_BLE_MESH_PB_GATT)
    if (idx < BLE_MESH_PROV_SAME_TIME
#if defined(CONFIG_BLE_MESH_PB_ADV)
            && idx >= CONFIG_BLE_MESH_PBA_SAME_TIME
#endif
       ) {
        if (link[idx].conn) {
            bt_mesh_gattc_disconnect(link[idx].conn);
        }
        return;
    }
#endif

    BT_ERR("%s, Invalid link index %d", __func__, idx);
    return;
}

static void prov_timeout(struct k_work *work)
{
    u8_t idx = (u8_t)work->index;

    BT_WARN("%s", __func__);

    close_link(idx, CLOSE_REASON_TIMEOUT);
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static void prov_retransmit(struct k_work *work)
{
    s64_t timeout = TRANSACTION_TIMEOUT;
    u8_t idx = (u8_t)work->index;
    int i;

    BT_DBG("%s", __func__);

    if (!bt_mesh_atomic_test_bit(link[idx].flags, LINK_ACTIVE)) {
        BT_WARN("Link is not active");
        return;
    }

#if defined(CONFIG_BLE_MESH_FAST_PROV)
    if (link[idx].tx_pdu_type >= PROV_DATA) {
        timeout = K_SECONDS(30);
    }
#endif
    if (k_uptime_get() - link[idx].tx.start > timeout) {
        BT_WARN("Provisioner timeout, giving up transaction");
        reset_link(idx, CLOSE_REASON_TIMEOUT);
        return;
    }

    if (link[idx].send_link_close & BIT(0)) {
        u8_t reason = (link[idx].send_link_close >> 1) & BIT_MASK(2);
        u16_t count = (link[idx].send_link_close >> 3);
        if (count >= 2) {
            reset_link(idx, reason);
            return;
        }
        link[idx].send_link_close += BIT(3);
    }

    bt_mesh_pb_buf_lock();

    for (i = 0; i < ARRAY_SIZE(link[idx].tx.buf); i++) {
        struct net_buf *buf = link[idx].tx.buf[i];

        if (!buf) {
            break;
        }

        if (BLE_MESH_ADV(buf)->busy) {
            continue;
        }

        BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

        if (i + 1 < ARRAY_SIZE(link[idx].tx.buf) && link[idx].tx.buf[i + 1]) {
            bt_mesh_adv_send(buf, NULL, NULL);
        } else {
            bt_mesh_adv_send(buf, &buf_sent_cb, (void *)(int)idx);
        }
    }

    bt_mesh_pb_buf_unlock();
}

static void link_ack(const u8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (buf->len) {
        BT_ERR("%s, Invalid Link ACK length", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (link[idx].expect == PROV_CAPABILITIES) {
        BT_INFO("%s, Link ACK is already received", __func__);
        return;
    }

    link[idx].conf_inputs = (u8_t *)bt_mesh_calloc(PROV_CONF_INPUTS_SIZE);
    if (!link[idx].conf_inputs) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    send_invite(idx);
}

static void link_close(const u8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    u8_t reason = 0U;

    BT_DBG("len %u", buf->len);

    reason = net_buf_simple_pull_u8(buf);

    reset_link(idx, reason);
}

static void gen_prov_ctl(const u8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("op 0x%02x len %u", BEARER_CTL(rx->gpc), buf->len);

    switch (BEARER_CTL(rx->gpc)) {
    case LINK_OPEN:
        break;

    case LINK_ACK:
        if (!bt_mesh_atomic_test_bit(link[idx].flags, LINK_ACTIVE)) {
            return;
        }
        link_ack(idx, rx, buf);
        break;

    case LINK_CLOSE:
        if (!bt_mesh_atomic_test_bit(link[idx].flags, LINK_ACTIVE)) {
            return;
        }
        link_close(idx, rx, buf);
        break;

    default:
        BT_ERR("%s, Unknown bearer opcode 0x%02x", __func__, BEARER_CTL(rx->gpc));
        return;
    }
}

static void prov_msg_recv(const u8_t idx)
{
    u8_t type = link[idx].rx.buf->data[0];

    BT_DBG("type 0x%02x len %u", type, link[idx].rx.buf->len);

    /**
     * Provisioner first checks information within the received
     * Provisioning PDU. If the check succeeds then check fcs.
     */
    if (type != PROV_FAILED && type != link[idx].expect) {
        BT_ERR("%s, Unexpected msg 0x%02x != 0x%02x", __func__, type, link[idx].expect);
        goto fail;
    }

    if (type >= 0x0A) {
        BT_ERR("%s, Unknown provisioning PDU type 0x%02x", __func__, type);
        goto fail;
    }

    if (1 + prov_handlers[type].len != link[idx].rx.buf->len) {
        BT_ERR("%s, Invalid length %u for type 0x%02x", __func__, link[idx].rx.buf->len, type);
        goto fail;
    }

    if (!bt_mesh_fcs_check(link[idx].rx.buf, link[idx].rx.fcs)) {
        BT_ERR("%s, Incorrect FCS", __func__);
        goto fail;
    }

    gen_prov_ack_send(idx, link[idx].rx.trans_id);
    link[idx].rx.prev_id = link[idx].rx.trans_id;
    link[idx].rx.trans_id = 0;

    prov_handlers[type].func(idx, &link[idx].rx.buf->data[1]);
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

static void gen_prov_cont(const u8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    u8_t seg = CONT_SEG_INDEX(rx->gpc);

    BT_DBG("len %u, seg_index %u", buf->len, seg);

    if (!link[idx].rx.seg && link[idx].rx.prev_id == rx->xact_id) {
        BT_INFO("%s, Resending ack", __func__);
        gen_prov_ack_send(idx, rx->xact_id);
        return;
    }

    if (rx->xact_id != link[idx].rx.trans_id) {
        BT_WARN("Data for unknown transaction (%u != %u)",
            rx->xact_id, link[idx].rx.trans_id);
        return;
    }

    if (seg > link[idx].rx.last_seg) {
        BT_ERR("%s, Invalid segment index %u", __func__, seg);
        goto fail;
    } else if (seg == link[idx].rx.last_seg) {
        u8_t expect_len = 0U;

        expect_len = (link[idx].rx.buf->len - 20 -
                      (23 * (link[idx].rx.last_seg - 1)));
        if (expect_len != buf->len) {
            BT_ERR("%s, Incorrect last seg len: %u != %u",
                   __func__, expect_len, buf->len);
            goto fail;
        }
    }

    if (!(link[idx].rx.seg & BIT(seg))) {
        BT_INFO("%s, Ignore already received segment", __func__);
        return;
    }

    memcpy(XACT_SEG_DATA(idx, seg), buf->data, buf->len);
    XACT_SEG_RECV(idx, seg);

    if (!link[idx].rx.seg) {
        prov_msg_recv(idx);
    }
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

static void gen_prov_ack(const u8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    u8_t ack_type = 0U, pub_key_oob = 0U;

    BT_DBG("len %u", buf->len);

    if (!link[idx].tx.buf[0]) {
        return;
    }

    if (!link[idx].tx.trans_id) {
        return;
    }

    if (rx->xact_id == (link[idx].tx.trans_id - 1)) {
        prov_clear_tx(idx);

        ack_type = link[idx].expect_ack_for;
        switch (ack_type) {
        case PROV_START:
            pub_key_oob = link[idx].conf_inputs[13];
            send_pub_key(idx, pub_key_oob);
            break;
        case PROV_PUB_KEY:
            prov_gen_dh_key(idx);
            break;
        default:
            break;
        }
        link[idx].expect_ack_for = 0x00;
    }
}

static void gen_prov_start(const u8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (link[idx].rx.seg) {
        BT_INFO("%s, Get Start while there are unreceived segments", __func__);
        return;
    }

    if (link[idx].rx.prev_id == rx->xact_id) {
        BT_INFO("%s, Resending ack", __func__);
        gen_prov_ack_send(idx, rx->xact_id);
        return;
    }

    link[idx].rx.buf->len = net_buf_simple_pull_be16(buf);
    link[idx].rx.trans_id = rx->xact_id;
    link[idx].rx.fcs = net_buf_simple_pull_u8(buf);

    BT_DBG("len %u last_seg %u total_len %u fcs 0x%02x", buf->len,
           START_LAST_SEG(rx->gpc), link[idx].rx.buf->len, link[idx].rx.fcs);

    /* Provisioner can not receive zero-length provisioning pdu */
    if (link[idx].rx.buf->len < 1) {
        BT_ERR("%s, Ignoring zero-length provisioning PDU", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (link[idx].rx.buf->len > link[idx].rx.buf->size) {
        BT_ERR("%s, Too large provisioning PDU (%u bytes)",
               __func__, link[idx].rx.buf->len);
        // close_link(i, CLOSE_REASON_FAILED);
        return;
    }

    if (START_LAST_SEG(rx->gpc) > 0 && link[idx].rx.buf->len <= 20) {
        BT_ERR("%s, Too small total length for multi-segment PDU", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    link[idx].rx.seg = (1 << (START_LAST_SEG(rx->gpc) + 1)) - 1;
    link[idx].rx.last_seg = START_LAST_SEG(rx->gpc);
    memcpy(link[idx].rx.buf->data, buf->data, buf->len);
    XACT_SEG_RECV(idx, 0);

    if (!link[idx].rx.seg) {
        prov_msg_recv(idx);
    }
}

static const struct {
    void (*const func)(const u8_t idx, struct prov_rx *rx, struct net_buf_simple *buf);
    const u8_t require_link;
    const u8_t min_len;
} gen_prov[] = {
    { gen_prov_start, true,  3 },
    { gen_prov_ack,   true,  0 },
    { gen_prov_cont,  true,  0 },
    { gen_prov_ctl,   true,  0 },
};

static void gen_prov_recv(const u8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (buf->len < gen_prov[GPCF(rx->gpc)].min_len) {
        BT_ERR("%s, Too short GPC message type %u", __func__, GPCF(rx->gpc));
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    /**
     * require_link can be used combining with link[].linking flag to
     * set LINK_ACTIVE status after Link ACK is received. In this case
     * there is no need to check LINK_ACTIVE status in find_link().
     */
    if (!bt_mesh_atomic_test_bit(link[idx].flags, LINK_ACTIVE) &&
            gen_prov[GPCF(rx->gpc)].require_link) {
        BT_DBG("Ignoring message that requires active link");
        return;
    }

    gen_prov[GPCF(rx->gpc)].func(idx, rx, buf);
}

static int find_link(u32_t link_id, u8_t *idx)
{
    int i;

    /* link for PB-ADV is from 0 to CONFIG_BLE_MESH_PBA_SAME_TIME */
    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        if (bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE)) {
            if (link[i].link_id == link_id) {
                if (idx) {
                    *idx = i;
                }
                return 0;
            }
        }
    }

    return -1;
}

void bt_mesh_provisioner_pb_adv_recv(struct net_buf_simple *buf)
{
    struct prov_rx rx = {0};
    u8_t idx = 0U;

    rx.link_id = net_buf_simple_pull_be32(buf);
    if (find_link(rx.link_id, &idx) < 0) {
        BT_DBG("%s, Data for unexpected link", __func__);
        return;
    }

    if (buf->len < 2) {
        BT_ERR("%s, Too short provisioning packet (len %u)", __func__, buf->len);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    rx.xact_id = net_buf_simple_pull_u8(buf);
    rx.gpc = net_buf_simple_pull_u8(buf);

    BT_DBG("link_id 0x%08x xact_id %u", rx.link_id, rx.xact_id);

    gen_prov_recv(idx, &rx, buf);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
static struct bt_mesh_conn *find_conn(struct bt_mesh_conn *conn, u8_t *idx)
{
    int i;

    /* link for PB-GATT is from CONFIG_BLE_MESH_PBA_SAME_TIME to BLE_MESH_PROV_SAME_TIME */
    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE)) {
            if (link[i].conn == conn) {
                if (idx) {
                    *idx = i;
                }
                return conn;
            }
        }
    }

    return NULL;
}

int bt_mesh_provisioner_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf)
{
    u8_t type = 0U;
    u8_t idx = 0U;

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    if (!find_conn(conn, &idx)) {
        BT_ERR("%s, Data for unexpected connection", __func__);
        return -ENOTCONN;
    }

    if (buf->len < 1) {
        BT_ERR("%s, Too short provisioning packet (len %u)", __func__, buf->len);
        goto fail;
    }

    type = net_buf_simple_pull_u8(buf);
    if (type != PROV_FAILED && type != link[idx].expect) {
        BT_ERR("%s, Unexpected msg 0x%02x != 0x%02x", __func__, type, link[idx].expect);
        goto fail;
    }

    if (type >= 0x0A) {
        BT_ERR("%s, Unknown provisioning PDU type 0x%02x", __func__, type);
        goto fail;
    }

    if (prov_handlers[type].len != buf->len) {
        BT_ERR("%s, Invalid length %u for type 0x%02x", __func__, buf->len, type);
        goto fail;
    }

    prov_handlers[type].func(idx, buf->data);

    return 0;

fail:
    /* Mesh Spec Section 5.4.4 Provisioning errors */
    close_link(idx, CLOSE_REASON_FAILED);
    return -EINVAL;
}

int bt_mesh_provisioner_set_prov_conn(const u8_t addr[6], struct bt_mesh_conn *conn)
{
    int i;

    if (!addr || !conn) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (!memcmp(link[i].addr.val, addr, BLE_MESH_ADDR_LEN)) {
            link[i].conn = bt_mesh_conn_ref(conn);
            return 0;
        }
    }

    BT_ERR("%s, Address %s is not found", __func__, bt_hex(addr, BLE_MESH_ADDR_LEN));
    return -ENOMEM;
}

int bt_mesh_provisioner_pb_gatt_open(struct bt_mesh_conn *conn, u8_t *addr)
{
    u8_t idx = 0U;
    int i;

    BT_DBG("conn %p", conn);

    /**
     * Double check if the device is currently being provisioned using PB-ADV.
     * Provisioner binds conn with proper device when proxy_prov_connected()
     * is invoked, and here after proper GATT procedures are completed, we just
     * check if this conn already exists in the proxy servers array.
     */
    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (link[i].conn == conn) {
            idx = i;
            break;
        }
    }

    if (i == BLE_MESH_PROV_SAME_TIME) {
        BT_ERR("%s, Link is not found", __func__);
        return -ENOTCONN;
    }

#if defined(CONFIG_BLE_MESH_PB_ADV)
    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        if (bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE)) {
            if (!memcmp(link[i].uuid, link[idx].uuid, 16)) {
                BT_WARN("Provision using PB-GATT & PB-ADV same time");
                close_link(idx, CLOSE_REASON_FAILED);
                return -EALREADY;
            }
        }
    }
#endif

    bt_mesh_atomic_set_bit(link[idx].flags, LINK_ACTIVE);
    link[idx].conn = bt_mesh_conn_ref(conn);

    /* May use lcd to indicate starting provisioning each device */
    if (prov->prov_link_open) {
        prov->prov_link_open(BLE_MESH_PROV_GATT);
    }

    link[idx].conf_inputs = (u8_t *)bt_mesh_calloc(PROV_CONF_INPUTS_SIZE);
    if (!link[idx].conf_inputs) {
        /* Disconnect this connection, clear corresponding informations */
        BT_ERR("%s, Failed to allocate memory", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return -ENOMEM;
    }

    send_invite(idx);
    return 0;
}

int bt_mesh_provisioner_pb_gatt_close(struct bt_mesh_conn *conn, u8_t reason)
{
    u8_t idx = 0U;

    BT_DBG("conn %p", conn);

    if (!find_conn(conn, &idx)) {
        BT_ERR("%s, Conn %p is not found", __func__, conn);
        return -ENOTCONN;
    }

    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    if (prov->prov_link_close) {
        prov->prov_link_close(BLE_MESH_PROV_GATT, reason);
    }

    prov_memory_free(idx);

    memset(&link[idx], 0, offsetof(struct prov_link, timeout));

    if (bt_mesh_pub_key_get()) {
        bt_mesh_atomic_set_bit(link[idx].flags, LOCAL_PUB_KEY);
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

int bt_mesh_provisioner_prov_init(const struct bt_mesh_prov *prov_info)
{
    const u8_t *key = NULL;
    int i;

    if (!prov_info) {
        BT_ERR("%s, No provisioning context provided", __func__);
        return -EINVAL;
    }

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("%s, Failed to generate Public Key", __func__);
        return -EIO;
    }

    prov = prov_info;

    prov_ctx.primary_addr = BLE_MESH_ADDR_UNASSIGNED;

    if (prov->prov_static_oob_val && prov->prov_static_oob_len) {
        prov_ctx.static_oob_len = MIN(16, prov->prov_static_oob_len);
        memcpy(prov_ctx.static_oob_val, prov->prov_static_oob_val, prov_ctx.static_oob_len);
    }

#if defined(CONFIG_BLE_MESH_PB_ADV)
    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        struct prov_adv_buf *adv = &adv_buf[i];
        adv->buf.size = ADV_BUF_SIZE;
        adv->buf.__buf = adv_buf_data + (i * ADV_BUF_SIZE);

        link[i].pending_ack = XACT_NVAL;
        k_delayed_work_init(&link[i].tx.retransmit, prov_retransmit);
        link[i].tx.retransmit.work.index = (int)i;
        link[i].rx.prev_id = XACT_NVAL;
        link[i].rx.buf = bt_mesh_pba_get_buf(i);
    }
#endif

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        k_delayed_work_init(&link[i].timeout, prov_timeout);
        link[i].timeout.work.index = (int)i;
    }

#if defined(CONFIG_BLE_MESH_PB_ADV)
    bt_mesh_pb_adv_mutex_new();
    bt_mesh_pb_buf_mutex_new();
#endif
#if defined(CONFIG_BLE_MESH_PB_GATT)
    bt_mesh_pb_gatt_mutex_new();
#endif

    return 0;
}

int bt_mesh_provisioner_prov_deinit(bool erase)
{
    int i;

    if (prov == NULL) {
        BT_ERR("%s, No provisioning context provided", __func__);
        return -EINVAL;
    }

#if defined(CONFIG_BLE_MESH_PB_ADV)
    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        prov_clear_tx(i);
        k_delayed_work_free(&link[i].tx.retransmit);
#if defined(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN)
        /* Remove the link id from exceptional list */
        bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_REMOVE,
                                        BLE_MESH_EXCEP_INFO_MESH_LINK_ID, &link[i].link_id);
#endif /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        prov_memory_free(i);
        k_delayed_work_free(&link[i].timeout);
        memset(&link[i], 0, sizeof(link[i]));
    }

#if defined(CONFIG_BLE_MESH_PB_ADV)
    bt_mesh_pb_adv_mutex_free();
    bt_mesh_pb_buf_mutex_free();
#endif
#if defined(CONFIG_BLE_MESH_PB_GATT)
    bt_mesh_pb_gatt_mutex_free();
#endif
    memset(&prov_ctx, 0, sizeof(prov_ctx));

#if defined(CONFIG_BLE_MESH_PB_ADV)
    memset(adv_buf, 0, sizeof(adv_buf));
    memset(adv_buf_data, 0, sizeof(adv_buf_data));
#endif
    memset(unprov_dev, 0, sizeof(unprov_dev));

    if (erase && IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_prov_info();
    }

    prov = NULL;

    return 0;
}

static bool is_unprov_dev_info_callback_to_app(bt_mesh_prov_bearer_t bearer,
        const u8_t uuid[16], const bt_mesh_addr_t *addr, u16_t oob_info, s8_t rssi)
{
    u16_t index = 0U;

    if (prov_ctx.prov_after_match == false) {
        u8_t adv_type = (bearer == BLE_MESH_PROV_ADV) ?
                        BLE_MESH_ADV_NONCONN_IND : BLE_MESH_ADV_IND;

        if (provisioner_dev_find(addr, uuid, &index)) {
            BT_DBG("%s, Device is not in queue, notify to upper layer", __func__);
            if (notify_unprov_adv_pkt_cb) {
                notify_unprov_adv_pkt_cb(addr->val, addr->type, adv_type, uuid, oob_info, bearer, rssi);
            }
            return true;
        }

        if (!(unprov_dev[index].bearer & bearer)) {
            BT_WARN("Device in queue not support PB-%s",
                    (bearer == BLE_MESH_PROV_ADV) ? "ADV" : "GATT");
            if (notify_unprov_adv_pkt_cb) {
                notify_unprov_adv_pkt_cb(addr->val, addr->type, adv_type, uuid, oob_info, bearer, rssi);
            }
            return true;
        }
    }

    return false;
}

void bt_mesh_provisioner_unprov_beacon_recv(struct net_buf_simple *buf, s8_t rssi)
{
#if defined(CONFIG_BLE_MESH_PB_ADV)
    const bt_mesh_addr_t *addr = NULL;
    const u8_t *uuid = NULL;
    u16_t oob_info = 0U;

    if (!(prov_ctx.bearers & BLE_MESH_PROV_ADV)) {
        BT_WARN("Provisioner not support PB-ADV bearer");
        return;
    }

    if (buf->len != 0x12 && buf->len != 0x16) {
        BT_ERR("%s, Invalid Unprovisioned Device Beacon length", __func__);
        return;
    }

    addr = bt_mesh_pba_get_addr();
    uuid = buf->data;
    net_buf_simple_pull(buf, 16);
    /* Mesh beacon uses big-endian to send beacon data */
    oob_info = net_buf_simple_pull_be16(buf);

    if (provisioner_check_unprov_dev_info(uuid, BLE_MESH_PROV_ADV)) {
        return;
    }

    if (is_unprov_dev_info_callback_to_app(
                BLE_MESH_PROV_ADV, uuid, addr, oob_info, rssi)) {
        return;
    }

    provisioner_start_prov_pb_adv(uuid, addr, oob_info, BLE_MESH_ADDR_UNASSIGNED);
#endif /* CONFIG_BLE_MESH_PB_ADV */
}

void bt_mesh_provisioner_prov_adv_ind_recv(struct net_buf_simple *buf, const bt_mesh_addr_t *addr, s8_t rssi)
{
#if defined(CONFIG_BLE_MESH_PB_GATT)
    const u8_t *uuid = NULL;
    u16_t oob_info = 0U;

    if (!(prov_ctx.bearers & BLE_MESH_PROV_GATT)) {
        BT_WARN("Provisioner not support PB-GATT bearer");
        return;
    }

    if (bt_mesh_gattc_get_free_conn_count() == 0) {
        BT_INFO("BLE connections for mesh reach max limit");
        return;
    }

    uuid = buf->data;
    net_buf_simple_pull(buf, 16);
    /* Mesh beacon uses big-endian to send beacon data */
    oob_info = net_buf_simple_pull_be16(buf);

    if (provisioner_check_unprov_dev_info(uuid, BLE_MESH_PROV_GATT)) {
        return;
    }

    if (is_unprov_dev_info_callback_to_app(
                BLE_MESH_PROV_GATT, uuid, addr, oob_info, rssi)) {
        return;
    }

    /* Provisioner will copy the device uuid, oob info, etc. into an unused link
     * struct, and at this moment the link has not been activated. Even if we
     * receive an Unprovisioned Device Beacon and a Connectable Provisioning adv
     * pkt from the same device, and store the device info received within each
     * adv pkt into two link structs which will has no impact on the provisioning
     * of this device, because no matter which link among PB-GATT and PB-ADV is
     * activated first, the other one will be dropped finally and the link struct
     * occupied by the dropped link will be used by other devices (because the link
     * is not activated).
     * Use connecting flag to prevent if two devices's adv pkts are both received,
     * the previous one info will be replaced by the second one.
     */
    provisioner_start_prov_pb_gatt(uuid, addr, oob_info, BLE_MESH_ADDR_UNASSIGNED);
#endif /* CONFIG_BLE_MESH_PB_GATT */
}

#endif /* CONFIG_BLE_MESH_PROVISIONER */
