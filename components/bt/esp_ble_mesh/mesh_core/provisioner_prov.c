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
#include "scan.h"
#include "mesh.h"
#include "access.h"
#include "settings.h"
#include "fast_prov.h"
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
#define START_LAST_SEG_MAX     2

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
#define PROV_RANDOM_SIZE       0x10
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
    HAVE_DHKEY,            /* DHKey has been calculated */
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
    uint8_t  uuid[16];          /* check if device is being provisioned*/
    uint16_t oob_info;          /* oob info of this device */
    uint8_t  element_num;       /* element num of device */
    uint8_t  ki_flags;          /* Key refresh flag and iv update flag */
    uint32_t iv_index;          /* IV Index */
    uint8_t  auth_method;       /* Choosen authentication method */
    uint8_t  auth_action;       /* Choosen authentication action */
    uint8_t  auth_size;         /* Choosen authentication size */
    uint16_t assign_addr;       /* Application assigned address for the device */
    uint16_t unicast_addr;      /* unicast address allocated for device */
    bt_mesh_addr_t addr;        /* Device address */
#if defined(CONFIG_BLE_MESH_PB_GATT)
    bool   connecting;          /* start connecting with device */
    struct bt_mesh_conn *conn;  /* GATT connection */
#endif
    uint8_t  expect;            /* Next expected PDU */

    uint8_t *dhkey;             /* Calculated DHKey */
    uint8_t *auth;              /* Authentication Value */

    uint8_t *conf_salt;         /* ConfirmationSalt */
    uint8_t *conf_key;          /* ConfirmationKey */
    uint8_t *conf_inputs;       /* ConfirmationInputs */

    uint8_t *rand;              /* Local Random */
    uint8_t *conf;              /* Remote Confirmation */
    uint8_t *local_conf;        /* Local Confirmation */

    uint8_t *prov_salt;         /* Provisioning Salt */

#if defined(CONFIG_BLE_MESH_PB_ADV)
    bool     linking;           /* Linking is being establishing */
    uint16_t send_link_close;   /* Link close is being sent flag */
    uint32_t link_id;           /* Link ID */
    uint8_t  pending_ack;       /* Decide which transaction id ack is pending */
    uint8_t  expect_ack_for;    /* Transaction ACK expected for provisioning pdu */
    uint8_t  tx_pdu_type;       /* The current transmitted Provisioning PDU type */

    struct {
        uint8_t trans_id;       /* Transaction ID */
        uint8_t prev_id;        /* Previous Transaction ID */
        uint8_t seg;            /* Bit-field of unreceived segments */
        uint8_t last_seg;       /* Last segment (to check length) */
        uint8_t fcs;            /* Expected FCS value */
        uint8_t adv_buf_id;     /* index of buf allocated in adv_buf_data */
        struct net_buf_simple *buf;
    } rx;

    struct {
        /* Start timestamp of the transaction */
        int64_t start;

        /* Transaction id*/
        uint8_t trans_id;

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
    uint32_t link_id;
    uint8_t  xact_id;
    uint8_t  gpc;
};

struct bt_mesh_prov_ctx {
    /* Primary element address of Provisioner */
    uint16_t primary_addr;

    /* Provisioning bearers used by Provisioner */
    bt_mesh_prov_bearer_t bearers;

    /* Current number of PB-ADV provisioned devices simultaneously */
    uint8_t  pba_count;

    /* Current number of PB-GATT provisioned devices simultaneously */
    uint8_t  pbg_count;

    /* Current unicast address going to allocated */
    uint16_t curr_alloc_addr;

    /* Current net_idx going to be used in provisioning data */
    uint16_t curr_net_idx;

    /* Current flags going to be used in provisioning data */
    uint8_t  curr_flags;

    /* Current iv_index going to be used in provisioning data */
    uint16_t curr_iv_index;

    /* Length of Static OOB value */
    uint8_t  static_oob_len;

    /* Static OOB value */
    uint8_t  static_oob_val[16];

    /* Offset of the device uuid to be matched, based on zero */
    uint8_t  match_offset;

    /* Length of the device uuid to be matched (start from the match_offset) */
    uint8_t  match_length;

    /* Value of the device uuid to be matched */
    uint8_t  match_value[16];

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
        bool     enable;
        uint16_t net_idx;
        uint8_t  flags;
        uint32_t iv_index;
        uint16_t unicast_addr_min;
        uint16_t unicast_addr_max;
    } fast_prov;
};

static struct bt_mesh_prov_ctx prov_ctx;

#define FAST_PROV_ENABLE()  (prov_ctx.fast_prov.enable)

struct unprov_dev_queue {
    bt_mesh_addr_t addr;
    uint8_t  uuid[16];
    uint16_t oob_info;
    uint8_t  bearer;
    uint8_t  flags;
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
static void send_link_open(const uint8_t idx);
#endif

static void prov_gen_dh_key(const uint8_t idx);

static void send_pub_key(const uint8_t idx, uint8_t oob);

static void close_link(const uint8_t idx, uint8_t reason);

#if defined(CONFIG_BLE_MESH_PB_ADV)
#define ADV_BUF_SIZE    65

static struct prov_adv_buf {
    struct net_buf_simple buf;
} adv_buf[CONFIG_BLE_MESH_PBA_SAME_TIME];

static uint8_t adv_buf_data[ADV_BUF_SIZE * CONFIG_BLE_MESH_PBA_SAME_TIME];
#endif /* CONFIG_BLE_MESH_PB_ADV */

#define PROV_FREE_MEM(_idx, member)     \
{                                       \
    if (link[_idx].member) {            \
        bt_mesh_free(link[_idx].member);    \
        link[_idx].member = NULL;       \
    }                                   \
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static inline void bt_mesh_pb_adv_mutex_new(void)
{
    if (!prov_ctx.pb_adv_lock.mutex) {
        bt_mesh_mutex_create(&prov_ctx.pb_adv_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_pb_adv_mutex_free(void)
{
    bt_mesh_mutex_free(&prov_ctx.pb_adv_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static inline void bt_mesh_pb_adv_lock(void)
{
    bt_mesh_mutex_lock(&prov_ctx.pb_adv_lock);
}

static inline void bt_mesh_pb_adv_unlock(void)
{
    bt_mesh_mutex_unlock(&prov_ctx.pb_adv_lock);
}

static inline void bt_mesh_pb_buf_mutex_new(void)
{
    if (!prov_ctx.pb_buf_lock.mutex) {
        bt_mesh_mutex_create(&prov_ctx.pb_buf_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_pb_buf_mutex_free(void)
{
    bt_mesh_mutex_free(&prov_ctx.pb_buf_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static inline void bt_mesh_pb_buf_lock(void)
{
    bt_mesh_mutex_lock(&prov_ctx.pb_buf_lock);
}

static inline void bt_mesh_pb_buf_unlock(void)
{
    bt_mesh_mutex_unlock(&prov_ctx.pb_buf_lock);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
static inline void bt_mesh_pb_gatt_mutex_new(void)
{
    if (!prov_ctx.pb_gatt_lock.mutex) {
        bt_mesh_mutex_create(&prov_ctx.pb_gatt_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_pb_gatt_mutex_free(void)
{
    bt_mesh_mutex_free(&prov_ctx.pb_gatt_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static inline void bt_mesh_pb_gatt_lock(void)
{
    bt_mesh_mutex_lock(&prov_ctx.pb_gatt_lock);
}

static inline void bt_mesh_pb_gatt_unlock(void)
{
    bt_mesh_mutex_unlock(&prov_ctx.pb_gatt_lock);
}

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

void bt_mesh_provisioner_clear_link_info(const uint8_t addr[6])
{
    int i;

    if (!addr) {
        BT_ERR("%s, Invalid parameter", __func__);
        return;
    }

    BT_DBG("Clear device info, addr %s", bt_hex(addr, BLE_MESH_ADDR_LEN));

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

    BT_WARN("Device not found, addr %s", bt_hex(addr, BLE_MESH_ADDR_LEN));
    return;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

const struct bt_mesh_prov *bt_mesh_provisioner_get_prov_info(void)
{
    return prov;
}

void bt_mesh_provisioner_restore_prov_info(uint16_t primary_addr, uint16_t alloc_addr)
{
    prov_ctx.primary_addr = primary_addr;
    prov_ctx.curr_alloc_addr = alloc_addr;
}

static bool is_unprov_dev_being_provision(const uint8_t uuid[16])
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
    if (bt_mesh_provisioner_get_node_with_uuid(uuid)) {
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
                BT_DBG("Device is being provisioning");
                return true;
            }
        }
    }

    return false;
}

static bool is_unprov_dev_uuid_match(const uint8_t uuid[16])
{
    if (prov_ctx.match_length) {
        if (memcmp(uuid + prov_ctx.match_offset,
                   prov_ctx.match_value, prov_ctx.match_length)) {
            return false;
        }
    }

    return true;
}

static int provisioner_check_unprov_dev_info(const uint8_t uuid[16], bt_mesh_prov_bearer_t bearer)
{
    if (!uuid) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Check if the device uuid matches configured value */
    if (is_unprov_dev_uuid_match(uuid) == false) {
        BT_DBG("Device uuid mismatch");
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
    if (IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) &&
        (prov_ctx.bearers & BLE_MESH_PROV_ADV) &&
        (bearer == BLE_MESH_PROV_ADV) &&
        (prov_ctx.pba_count == CONFIG_BLE_MESH_PBA_SAME_TIME)) {
        BT_INFO("Current PB-ADV links reach max limit");
        return -ENOMEM;
    }

    /* Check if the current PB-GATT link is full */
    if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) &&
        (prov_ctx.bearers & BLE_MESH_PROV_GATT) &&
        (bearer == BLE_MESH_PROV_GATT) &&
        (prov_ctx.pbg_count == CONFIG_BLE_MESH_PBG_SAME_TIME)) {
        BT_INFO("Current PB-GATT links reach max limit");
        return -ENOMEM;
    }

    /* Check if the device has already been provisioned */
    if (bt_mesh_provisioner_get_node_with_uuid(uuid)) {
        BT_INFO("Provisioned before, start to provision again");
    }

    return 0;
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static int provisioner_start_prov_pb_adv(const uint8_t uuid[16], const bt_mesh_addr_t *addr,
                                         uint16_t oob_info, uint16_t assign_addr)
{
    int i;

    if (uuid == NULL) {
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
        bt_mesh_pb_adv_unlock();
        return -EIO;
    }

    if (is_unprov_dev_being_provision(uuid)) {
        bt_mesh_pb_adv_unlock();
        return -EALREADY;
    }

    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        if (!bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE) && !link[i].linking) {
            memcpy(link[i].uuid, uuid, 16);
            link[i].oob_info = oob_info;
            if (addr) {
                link[i].addr.type = addr->type;
                memcpy(link[i].addr.val, addr->val, BLE_MESH_ADDR_LEN);
            }

            send_link_open(i);

            /* If a specific unicast address is assigned for the device, then
             * Provisioner will use this address in the Provisioning Data PDU.
             */
            if (BLE_MESH_ADDR_IS_UNICAST(assign_addr)) {
                link[i].assign_addr = assign_addr;
            }

            /* Increase PB-ADV link count */
            prov_ctx.pba_count++;

            bt_mesh_pb_adv_unlock();
            return 0;
        }
    }

    BT_ERR("No PB-ADV link available");
    bt_mesh_pb_adv_unlock();

    return -ENOMEM;
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if defined(CONFIG_BLE_MESH_PB_GATT)
static int provisioner_start_prov_pb_gatt(const uint8_t uuid[16], const bt_mesh_addr_t *addr,
                                          uint16_t oob_info, uint16_t assign_addr)
{
    int i;

    if (uuid == NULL || addr == NULL) {
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
        bt_mesh_pb_gatt_unlock();
        return -EIO;
    }

    if (is_unprov_dev_being_provision(uuid)) {
        bt_mesh_pb_gatt_unlock();
        return -EALREADY;
    }

    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (!link[i].connecting && !bt_mesh_atomic_test_bit(link[i].flags, LINK_ACTIVE)) {
            if (bt_mesh_gattc_conn_create(addr, BLE_MESH_UUID_MESH_PROV_VAL)) {
                bt_mesh_pb_gatt_unlock();
                return -EIO;
            }

            memcpy(link[i].uuid, uuid, 16);
            link[i].oob_info = oob_info;
            link[i].addr.type = addr->type;
            memcpy(link[i].addr.val, addr->val, BLE_MESH_ADDR_LEN);

            /* If the application layer assigned a specific unicast address for the device,
             * then Provisioner will use this address in the Provisioning Data PDU.
             */
            if (BLE_MESH_ADDR_IS_UNICAST(assign_addr)) {
                link[i].assign_addr = assign_addr;
            }

            /* If creating connection successfully, set connecting flag to 1 */
            link[i].connecting = true;

            /* Increase PB-GATT link count */
            provisioner_pbg_count_inc();

            bt_mesh_pb_gatt_unlock();
            return 0;
        }
    }

    BT_ERR("No PB-GATT link available");
    bt_mesh_pb_gatt_unlock();

    return -ENOMEM;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

int bt_mesh_provisioner_add_unprov_dev(struct bt_mesh_unprov_dev_add *add_dev, uint8_t flags)
{
    bt_mesh_addr_t add_addr = {0};
    bool addr_valid = false;
    uint8_t zero[16] = {0};
    int err = 0;
    int i;

    if (add_dev == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (!memcmp(add_dev->uuid, zero, 16)) {
        BT_ERR("Invalid device uuid to add");
        return -EINVAL;
    }

    if (!(add_dev->bearer & (BLE_MESH_PROV_ADV | BLE_MESH_PROV_GATT))) {
        BT_ERR("Invalid bearer 0x%02x", add_dev->bearer);
        return -EINVAL;
    }

    if ((!IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) ||
        !(prov_ctx.bearers & BLE_MESH_PROV_GATT))
        && (add_dev->bearer & BLE_MESH_PROV_GATT)) {
        BT_ERR("Not support PB-GATT");
        return -EINVAL;
    }

    if ((!IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) ||
        !(prov_ctx.bearers & BLE_MESH_PROV_ADV))
        && (add_dev->bearer & BLE_MESH_PROV_ADV)) {
        BT_ERR("Not support PB-ADV");
        return -EINVAL;
    }

    if (memcmp(add_dev->addr, zero, BLE_MESH_ADDR_LEN)) {
        addr_valid = true;

        add_addr.type = add_dev->addr_type;
        memcpy(add_addr.val, add_dev->addr, BLE_MESH_ADDR_LEN);
    }

    /* Pb-GATT needs device address to create connection */
    if ((add_dev->bearer & BLE_MESH_PROV_GATT) && (addr_valid == false)) {
        BT_ERR("Invalid device address for PB-GATT");
        return -EINVAL;
    }

    /* If start provisioning immediately, only one bearer can be used */
    if ((flags & START_PROV_NOW) &&
        (add_dev->bearer != BLE_MESH_PROV_ADV) &&
        (add_dev->bearer != BLE_MESH_PROV_GATT)) {
        BT_ERR("Can not start PB-ADV & PB-GATT simultaneously");
        return -EINVAL;
    }

    /* Check if the provisioned nodes array is full */
    if (bt_mesh_provisioner_get_node_with_uuid(add_dev->uuid) == NULL) {
        if (bt_mesh_provisioner_get_node_count() == CONFIG_BLE_MESH_MAX_PROV_NODES) {
            BT_WARN("Current provisioned devices reach max limit");
            return -ENOMEM;
        }
    }

    /* Check if the device already exists in queue */
    for (i = 0; i < ARRAY_SIZE(unprov_dev); i++) {
        if (!memcmp(unprov_dev[i].uuid, add_dev->uuid, 16)) {
            if (!(add_dev->bearer & unprov_dev[i].bearer)) {
                BT_WARN("Add device with only bearer updated");
                unprov_dev[i].bearer |= add_dev->bearer;
            } else {
                BT_WARN("Device already exists in queue");
            }
            goto start;
        }
    }

    /* If not exists, try to add the device into queue */
    for (i = 0; i < ARRAY_SIZE(unprov_dev); i++) {
        if (unprov_dev[i].bearer) {
            continue;
        }
        if (addr_valid) {
            unprov_dev[i].addr.type = add_dev->addr_type;
            memcpy(unprov_dev[i].addr.val, add_dev->addr, BLE_MESH_ADDR_LEN);
        }
        memcpy(unprov_dev[i].uuid, add_dev->uuid, 16);
        unprov_dev[i].bearer = add_dev->bearer & BIT_MASK(2);
        unprov_dev[i].flags = flags & BIT_MASK(3);
        goto start;
    }

    /* If queue is full, find flushable device and replace it */
    for (i = 0; i < ARRAY_SIZE(unprov_dev); i++) {
        if (unprov_dev[i].flags & FLUSHABLE_DEV) {
            memset(&unprov_dev[i], 0, sizeof(struct unprov_dev_queue));
            if (addr_valid) {
                unprov_dev[i].addr.type = add_dev->addr_type;
                memcpy(unprov_dev[i].addr.val, add_dev->addr, BLE_MESH_ADDR_LEN);
            }
            memcpy(unprov_dev[i].uuid, add_dev->uuid, 16);
            unprov_dev[i].bearer = add_dev->bearer & BIT_MASK(2);
            unprov_dev[i].flags  = flags & BIT_MASK(3);
            goto start;
        }
    }

    BT_ERR("Unprovisioned device queue is full");
    return -ENOMEM;

start:
    /* If not provisioning immediately, directly return here */
    if (!(flags & START_PROV_NOW)) {
        return 0;
    }

    /* Check if current provisioned node count + active link reach max limit */
    if (bt_mesh_provisioner_get_node_with_uuid(add_dev->uuid) == NULL) {
        if (bt_mesh_provisioner_get_node_count() + prov_ctx.pba_count + \
            prov_ctx.pbg_count >= CONFIG_BLE_MESH_MAX_PROV_NODES) {
            BT_WARN("Node count + active link count reach max limit");
            return -EIO;
        }
    }

    if ((err = provisioner_check_unprov_dev_info(add_dev->uuid, add_dev->bearer))) {
        return err;
    }

    if (add_dev->bearer == BLE_MESH_PROV_ADV) {
#if defined(CONFIG_BLE_MESH_PB_ADV)
        if ((err = provisioner_start_prov_pb_adv(add_dev->uuid, addr_valid ? &add_addr : NULL,
                                                 add_dev->oob_info, BLE_MESH_ADDR_UNASSIGNED))) {
            return err;
        }
#endif
    } else if (add_dev->bearer == BLE_MESH_PROV_GATT) {
#if defined(CONFIG_BLE_MESH_PB_GATT)
        if ((err = provisioner_start_prov_pb_gatt(add_dev->uuid, &add_addr, add_dev->oob_info,
                                                  BLE_MESH_ADDR_UNASSIGNED))) {
            return err;
        }
#endif
    }

    return 0;
}

int bt_mesh_provisioner_prov_device_with_addr(const uint8_t uuid[16], const uint8_t addr[6],
                                              uint8_t addr_type, bt_mesh_prov_bearer_t bearer,
                                              uint16_t oob_info, uint16_t unicast_addr)
{
    bt_mesh_addr_t dev_addr = {0};
    int err = 0;

    if (uuid == NULL) {
        BT_ERR("Invalid device uuid");
        return -EINVAL;
    }

    if (bearer != BLE_MESH_PROV_ADV && bearer != BLE_MESH_PROV_GATT) {
        BT_ERR("Invalid provisioning bearer 0x%02x", bearer);
        return -EINVAL;
    }

    if ((!IS_ENABLED(CONFIG_BLE_MESH_PB_ADV) ||
        !(prov_ctx.bearers & BLE_MESH_PROV_ADV)) &&
        (bearer == BLE_MESH_PROV_ADV)) {
        BT_ERR("Not support PB-ADV");
        return -ENOTSUP;
    }

    if ((!IS_ENABLED(CONFIG_BLE_MESH_PB_GATT) ||
        !(prov_ctx.bearers & BLE_MESH_PROV_GATT)) &&
        (bearer == BLE_MESH_PROV_GATT)) {
        BT_ERR("Not support PB-GATT");
        return -ENOTSUP;
    }

    if (bearer == BLE_MESH_PROV_GATT && addr == NULL) {
        BT_ERR("Invalid device address for PB-GATT");
        return -EINVAL;
    }

    if (!BLE_MESH_ADDR_IS_UNICAST(unicast_addr)) {
        BT_ERR("Invalid unicast address 0x%04x", unicast_addr);
        return -EINVAL;
    }

    /* Here we will not check if the assigned unicast address is overlapped
     * with the unicast addresses of other nodes or Provisioner, because:
     * 1. At this moment, the element number of the device is unknown
     * 2. If the node is a reprovisioned device, then the original allocated
     *    unicast address will be used.
     * 3. Some other devices may be just being provisioning, and currently we
     *    can not know the exactly allocated addresses of them.
     */

    if (bt_mesh_provisioner_get_node_with_uuid(uuid) == NULL) {
        /* Check if the provisioned nodes array is full */
        if (bt_mesh_provisioner_get_node_count() == CONFIG_BLE_MESH_MAX_PROV_NODES) {
            BT_WARN("Current provisioned devices reach max limit");
            return -ENOMEM;
        }

        /* Check if current provisioned node count + active link reach max limit */
        if (bt_mesh_provisioner_get_node_count() + prov_ctx.pba_count + \
                prov_ctx.pbg_count >= CONFIG_BLE_MESH_MAX_PROV_NODES) {
            BT_WARN("Node count + active link count reach max limit");
            return -EIO;
        }
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
        if ((err = provisioner_start_prov_pb_adv(uuid, addr ? &dev_addr : NULL, oob_info, unicast_addr))) {
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
    uint8_t zero[16] = {0};
    int i;

    if (del_dev == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (!memcmp(del_dev->uuid, zero, 16)) {
        BT_ERR("Invalid device uuid to delete");
        return -EINVAL;
    }

    /* Find if the device is in the device queue */
    for (i = 0; i < ARRAY_SIZE(unprov_dev); i++) {
        if (!memcmp(unprov_dev[i].uuid, del_dev->uuid, 16)) {
            memset(&unprov_dev[i], 0, sizeof(struct unprov_dev_queue));
            break;
        }
    }

    /* Find if the device is being provisioned */
    for (i = 0; i < ARRAY_SIZE(link); i++) {
        if (!memcmp(link[i].uuid, del_dev->uuid, 16)) {
            close_link(i, CLOSE_REASON_FAILED);
            break;
        }
    }

    return 0;
}

int bt_mesh_provisioner_set_dev_uuid_match(uint8_t offset, uint8_t length,
                                           const uint8_t *match, bool prov_flag)
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
    const uint8_t *key = NULL;

    if (!info || info->flag == 0) {
        return -EINVAL;
    }

    if (info->flag & NET_IDX_FLAG) {
        key = bt_mesh_provisioner_net_key_get(info->net_idx);
        if (!key) {
            BT_ERR("Failed to get NetKey");
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

int bt_mesh_provisioner_init_prov_info(void)
{
    if (prov_ctx.primary_addr == BLE_MESH_ADDR_UNASSIGNED) {
        /* If unicast address of primary element of Provisioner has not been set
         * before, then the following initialization procedure will be used.
         */
        if (prov == NULL) {
            BT_ERR("No provisioning context provided");
            return -EINVAL;
        }

        if (!BLE_MESH_ADDR_IS_UNICAST(prov->prov_unicast_addr) ||
            !BLE_MESH_ADDR_IS_UNICAST(prov->prov_start_address)) {
            BT_ERR("Invalid address, own 0x%04x, start 0x%04x",
                    prov->prov_unicast_addr, prov->prov_start_address);
            return -EINVAL;
        }

        const struct bt_mesh_comp *comp = bt_mesh_comp_get();
        if (!comp) {
            BT_ERR("Invalid composition data");
            return -EINVAL;
        }

        if (prov->prov_unicast_addr + comp->elem_count > prov->prov_start_address) {
            BT_WARN("Too small start address 0x%04x, update to 0x%04x",
                prov->prov_start_address, prov->prov_unicast_addr + comp->elem_count);
            prov_ctx.curr_alloc_addr = prov->prov_unicast_addr + comp->elem_count;
        } else {
            prov_ctx.curr_alloc_addr = prov->prov_start_address;
        }

        /* Update primary element address with the initialized value here. */
        prov_ctx.primary_addr = prov->prov_unicast_addr;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.curr_alloc_addr);
        }
    }

    prov_ctx.curr_net_idx = BLE_MESH_KEY_PRIMARY;
    struct bt_mesh_subnet *sub = bt_mesh_provisioner_subnet_get(BLE_MESH_KEY_PRIMARY);
    prov_ctx.curr_flags = bt_mesh_net_flags(sub);
    prov_ctx.curr_iv_index = bt_mesh.iv_index;

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

int bt_mesh_provisioner_set_static_oob_value(const uint8_t *value, uint8_t length)
{
    int i;

    if (value == NULL || length == 0U || length > 16U) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Make sure Static OOB is not being used. */
    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (link[i].auth_method == AUTH_METHOD_STATIC) {
            BT_ERR("Static OOB is being used");
            return -EINVAL;
        }
    }

    (void)memset(prov_ctx.static_oob_val, 0, 16);

    prov_ctx.static_oob_len = MIN(16, length);
    memcpy(prov_ctx.static_oob_val, value, prov_ctx.static_oob_len);

    return 0;
}

uint16_t bt_mesh_provisioner_get_primary_elem_addr(void)
{
    return prov_ctx.primary_addr;
}

int bt_mesh_provisioner_set_primary_elem_addr(uint16_t addr)
{
    const struct bt_mesh_comp *comp = NULL;

    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("Invalid primary address 0x%04x", addr);
        return -EINVAL;
    }

    comp = bt_mesh_comp_get();
    if (!comp) {
        BT_ERR("Invalid composition data");
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

    BT_INFO("Primary address updated, old 0x%04x, new 0x%04x", prov_ctx.primary_addr, addr);
    prov_ctx.primary_addr = addr;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.curr_alloc_addr);
    }

    bt_mesh_comp_provision(addr);

    return 0;
}

#if CONFIG_BLE_MESH_TEST_AUTO_ENTER_NETWORK
int bt_mesh_test_provisioner_update_alloc_addr(uint16_t unicast_addr, uint16_t element_num)
{
    uint16_t max_addr = FAST_PROV_ENABLE() ? prov_ctx.fast_prov.unicast_addr_max : PROV_MAX_ADDR_TO_ASSIGN;

    if (unicast_addr + element_num > max_addr) {
        BT_WARN("Not enough unicast address to allocate");
        prov_ctx.curr_alloc_addr = BLE_MESH_ADDR_UNASSIGNED;
    } else {
        prov_ctx.curr_alloc_addr = unicast_addr + element_num;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.curr_alloc_addr);
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_TEST_AUTO_ENTER_NETWORK */

/* The following APIs are for fast provisioning */

void bt_mesh_provisioner_fast_prov_enable(bool enable)
{
    prov_ctx.fast_prov.enable = enable;
}

void bt_mesh_provisioner_set_fast_prov_net_idx(uint16_t net_idx)
{
    prov_ctx.fast_prov.net_idx = net_idx;
}

uint16_t bt_mesh_provisioner_get_fast_prov_net_idx(void)
{
    return prov_ctx.fast_prov.net_idx;
}

uint8_t bt_mesh_set_fast_prov_unicast_addr_range(uint16_t min, uint16_t max)
{
    if (!BLE_MESH_ADDR_IS_UNICAST(min) || !BLE_MESH_ADDR_IS_UNICAST(max)) {
        BT_ERR("Invalid unicast address, min 0x%04x, max 0x%04x", min, max);
        return 0x01; /* status: not a unicast address */
    }

    if (min > max) {
        BT_ERR("Unicast address min is bigger than max");
        return 0x02; /* status: min is bigger than max */
    }

    if (min <= prov_ctx.fast_prov.unicast_addr_max) {
        BT_ERR("Unicast address overlap");
        return 0x03; /* status: address overlaps with current value */
    }

    prov_ctx.fast_prov.unicast_addr_min = min;
    prov_ctx.fast_prov.unicast_addr_max = max;

    prov_ctx.curr_alloc_addr = prov_ctx.fast_prov.unicast_addr_min;

    return 0x0; /* status: success */
}

void bt_mesh_set_fast_prov_flags_iv_index(uint8_t flags, uint32_t iv_index)
{
    /* BIT0: Key Refresh flag, BIT1: IV Update flag */
    prov_ctx.fast_prov.flags = flags & BIT_MASK(2);
    prov_ctx.fast_prov.iv_index = iv_index;
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static struct net_buf_simple *bt_mesh_pba_get_buf(const uint8_t idx)
{
    struct net_buf_simple *buf = &(adv_buf[idx].buf);

    net_buf_simple_reset(buf);

    return buf;
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

static void prov_memory_free(const uint8_t idx)
{
    PROV_FREE_MEM(idx, dhkey);
    PROV_FREE_MEM(idx, auth);
    PROV_FREE_MEM(idx, rand);
    PROV_FREE_MEM(idx, conf);
    PROV_FREE_MEM(idx, local_conf);
    PROV_FREE_MEM(idx, conf_salt);
    PROV_FREE_MEM(idx, conf_key);
    PROV_FREE_MEM(idx, conf_inputs);
    PROV_FREE_MEM(idx, prov_salt);
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static void buf_sent(int err, void *user_data)
{
    uint8_t idx = (int)user_data;

    if (!link[idx].tx.buf[0]) {
        return;
    }

    k_delayed_work_submit(&link[idx].tx.retransmit, RETRANSMIT_TIMEOUT);
}

static struct bt_mesh_send_cb buf_sent_cb = {
    .end = buf_sent,
};

static void free_segments(const uint8_t idx)
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

static void prov_clear_tx(const uint8_t idx)
{
    BT_DBG("%s", __func__);

    k_delayed_work_cancel(&link[idx].tx.retransmit);

    free_segments(idx);
}

static void reset_link(const uint8_t idx, uint8_t reason)
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
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID, &link[idx].link_id);
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

static void ack_complete(uint16_t duration, int err, void *user_data)
{
    uint8_t idx = (int)user_data;

    BT_DBG("xact %u complete", link[idx].pending_ack);

    link[idx].pending_ack = XACT_NVAL;
}

static void gen_prov_ack_send(const uint8_t idx, uint8_t xact_id)
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

static void send_reliable(const uint8_t idx)
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

static int bearer_ctl_send(const uint8_t idx, uint8_t op, void *data, uint8_t data_len)
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
        uint8_t reason = *(uint8_t *)data;
        link[idx].send_link_close = ((reason & BIT_MASK(2)) << 1) | BIT(0);
        link[idx].tx.trans_id = 0;
    }

    return 0;
}

static void send_link_open(const uint8_t idx)
{
    int j;

    /** Generate link ID, and may need to check if this id is
     *  currently being used, which may will not happen ever.
     */
    bt_mesh_rand(&link[idx].link_id, sizeof(uint32_t));
    while (1) {
        for (j = 0; j < CONFIG_BLE_MESH_PBA_SAME_TIME; j++) {
            if (bt_mesh_atomic_test_bit(link[j].flags, LINK_ACTIVE) || link[j].linking) {
                if (link[idx].link_id == link[j].link_id) {
                    bt_mesh_rand(&link[idx].link_id, sizeof(uint32_t));
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
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID, &link[idx].link_id);
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
}

static uint8_t last_seg(uint8_t len)
{
    if (len <= START_PAYLOAD_MAX) {
        return 0;
    }

    len -= START_PAYLOAD_MAX;

    return 1 + (len / CONT_PAYLOAD_MAX);
}

static inline uint8_t next_transaction_id(const uint8_t idx)
{
    if (link[idx].tx.trans_id > 0x7F) {
        link[idx].tx.trans_id = 0x0;
    }
    return link[idx].tx.trans_id++;
}

static int prov_send_adv(const uint8_t idx, struct net_buf_simple *msg)
{
    struct net_buf *start = NULL, *buf = NULL;
    uint8_t seg_len = 0U, seg_id = 0U;
    uint8_t xact_id = 0U;
    int32_t timeout = PROVISION_TIMEOUT;

    BT_DBG("len %u: %s", msg->len, bt_hex(msg->data, msg->len));

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
            BT_ERR("Too big message (seg_id %d)", seg_id);
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
static int prov_send_gatt(const uint8_t idx, struct net_buf_simple *msg)
{
    int err = 0;

    if (!link[idx].conn) {
        return -ENOTCONN;
    }

    err = bt_mesh_proxy_client_send(link[idx].conn, BLE_MESH_PROXY_PROV, msg);
    if (err) {
        BT_ERR("Failed to send PB-GATT pdu");
        return err;
    }

    if (!bt_mesh_atomic_test_and_set_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_submit(&link[idx].timeout, PROVISION_TIMEOUT);
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

static inline int prov_send(const uint8_t idx, struct net_buf_simple *buf)
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

    BT_ERR("Invalid link index %d", idx);
    return -EINVAL;
}

static void prov_buf_init(struct net_buf_simple *buf, uint8_t type)
{
    net_buf_simple_reserve(buf, PROV_BUF_HEADROOM);
    net_buf_simple_add_u8(buf, type);
}

static void prov_invite(const uint8_t idx, const uint8_t *data)
{
    BT_DBG("%s", __func__);
}

static void prov_start(const uint8_t idx, const uint8_t *data)
{
    BT_DBG("%s", __func__);
}

static void prov_data(const uint8_t idx, const uint8_t *data)
{
    BT_DBG("%s", __func__);
}

static void send_invite(const uint8_t idx)
{
    PROV_BUF(buf, 2);

    prov_buf_init(&buf, PROV_INVITE);

    net_buf_simple_add_u8(&buf, prov->prov_attention);

    link[idx].conf_inputs[0] = prov->prov_attention;

    if (prov_send(idx, &buf)) {
        BT_ERR("Failed to send Provisioning Invite");
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    link[idx].expect = PROV_CAPABILITIES;
}

static void prov_capabilities(const uint8_t idx, const uint8_t *data)
{
    PROV_BUF(buf, 6);
    uint16_t algorithms = 0U, output_action = 0U, input_action = 0U;
    uint8_t  element_num = 0U, pub_key_oob = 0U, static_oob = 0U,
             output_size = 0U, input_size = 0U;
    uint8_t  auth_method = 0U, auth_action = 0U, auth_size = 0U;

    element_num = data[0];
    BT_INFO("Elements:          0x%02x", element_num);
    if (!element_num) {
        BT_ERR("Invalid element number %d", element_num);
        goto fail;
    }
    link[idx].element_num = element_num;

    algorithms = sys_get_be16(&data[1]);
    BT_INFO("Algorithms:        0x%04x", algorithms);
    if (algorithms != BIT(PROV_ALG_P256)) {
        BT_ERR("Invalid algorithms 0x%04x", algorithms);
        goto fail;
    }

    pub_key_oob = data[3];
    BT_INFO("Public Key Type:   0x%02x", pub_key_oob);
    if (pub_key_oob > 0x01) {
        BT_ERR("Invalid public key type 0x%02x", pub_key_oob);
        goto fail;
    }
    pub_key_oob = ((prov->prov_pub_key_oob &&
                    prov->prov_pub_key_oob_cb) ? pub_key_oob : 0x00);

    static_oob = data[4];
    BT_INFO("Static OOB Type:   0x%02x", static_oob);
    if (static_oob > 0x01) {
        BT_ERR("Invalid Static OOB type 0x%02x", static_oob);
        goto fail;
    }
    static_oob = (prov_ctx.static_oob_len ? static_oob : 0x00);

    output_size = data[5];
    BT_INFO("Output OOB Size:   0x%02x", output_size);
    if (output_size > 0x08) {
        BT_ERR("Invalid Output OOB size %d", output_size);
        goto fail;
    }

    output_action = sys_get_be16(&data[6]);
    BT_INFO("Output OOB Action: 0x%04x", output_action);
    if (output_action > 0x1f) {
        BT_ERR("Invalid Output OOB action 0x%04x", output_action);
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
        BT_ERR("Invalid Input OOB size %d", input_size);
        goto fail;
    }

    input_action = sys_get_be16(&data[9]);
    BT_INFO("Input OOB Action:  0x%04x", input_action);
    if (input_action > 0x0f) {
        BT_ERR("Invalid Input OOB action 0x%04x", input_action);
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
            auth_action = (uint8_t)input_action;
            auth_size   = input_size;
        } else {
            auth_method = AUTH_METHOD_OUTPUT;
            auth_action = (uint8_t)output_action;
            auth_size   = output_size;
        }
    }

    /* Store provisioning capabilities value in conf_inputs */
    memcpy(&link[idx].conf_inputs[1], data, 11);

    prov_buf_init(&buf, PROV_START);
    net_buf_simple_add_u8(&buf, prov->prov_algorithm);
    net_buf_simple_add_u8(&buf, pub_key_oob);
    net_buf_simple_add_u8(&buf, auth_method);
    net_buf_simple_add_u8(&buf, auth_action);
    net_buf_simple_add_u8(&buf, auth_size);

    memcpy(&link[idx].conf_inputs[12], &buf.data[1], 5);

    if (prov_send(idx, &buf)) {
        BT_ERR("Failed to send Provisioning Start");
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
            BT_ERR("Failed to notify input OOB Public Key");
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

static bt_mesh_output_action_t output_action(uint8_t action)
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

static bt_mesh_input_action_t input_action(uint8_t action)
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

static int prov_auth(const uint8_t idx, uint8_t method, uint8_t action, uint8_t size)
{
    bt_mesh_output_action_t output = 0U;
    bt_mesh_input_action_t input = 0U;

    link[idx].auth = (uint8_t *)bt_mesh_calloc(PROV_AUTH_VAL_SIZE);
    if (!link[idx].auth) {
        BT_ERR("%s, Out of memory", __func__);
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

        /* Provisioner ouput number/string and wait for device's Provisioning Input Complete PDU */
        link[idx].expect = PROV_INPUT_COMPLETE;

        /* NOTE: The Bluetooth SIG recommends that mesh implementations enforce a randomly
         * selected AuthValue using all of the available bits, where permitted by the
         * implementation. A large entropy helps ensure that a brute-force of the AuthValue,
         * even a static AuthValue, cannot normally be completed in a reasonable time (CVE-2020-26557).
         *
         * AuthValues selected using a cryptographically secure random or pseudorandom number
         * generator and having the maximum permitted entropy (128-bits) will be most difficult
         * to brute-force. AuthValues with reduced entropy or generated in a predictable manner
         * will not grant the same level of protection against this vulnerability. Selecting a
         * new AuthValue with each provisioning attempt can also make it more difficult to launch
         * a brute-force attack by requiring the attacker to restart the search with each
         * provisioning attempt (CVE-2020-26556).
         */
        if (input == BLE_MESH_ENTER_STRING) {
            unsigned char str[9] = {'\0'};
            uint8_t j = 0U;

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
            uint32_t div[8] = { 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };
            uint32_t num = 0U;

            bt_mesh_rand(&num, sizeof(num));

            if (input == BLE_MESH_PUSH ||
                input == BLE_MESH_TWIST) {
                /** NOTE: According to the Bluetooth Mesh Profile Specification
                 *  Section 5.4.2.4, push and twist should be a random integer
                 *  between 0 and 10^size.
                 */
                num = (num % (div[size - 1] - 1)) + 1;
            } else {
                num %= div[size - 1];
            }

            sys_put_be32(num, &link[idx].auth[12]);
            memset(link[idx].auth, 0, 12);

            return prov->prov_output_num(AUTH_METHOD_INPUT, input, &num, size, idx);
        }

    default:
        return -EINVAL;
    }
}

static void send_confirm(const uint8_t idx)
{
    PROV_BUF(buf, 17);
    uint8_t *conf = NULL;

    BT_DBG("ConfInputs[0]   %s", bt_hex(link[idx].conf_inputs, 64));
    BT_DBG("ConfInputs[64]  %s", bt_hex(link[idx].conf_inputs + 64, 64));
    BT_DBG("ConfInputs[128] %s", bt_hex(link[idx].conf_inputs + 128, 17));

    link[idx].conf_salt = (uint8_t *)bt_mesh_calloc(PROV_CONF_SALT_SIZE);
    if (!link[idx].conf_salt) {
        BT_ERR("%s, Out of memory", __func__);
        goto fail;
    }

    link[idx].conf_key = (uint8_t *)bt_mesh_calloc(PROV_CONF_KEY_SIZE);
    if (!link[idx].conf_key) {
        BT_ERR("%s, Out of memory", __func__);
        goto fail;
    }

    if (bt_mesh_prov_conf_salt(link[idx].conf_inputs, link[idx].conf_salt)) {
        BT_ERR("Failed to generate confirmation salt");
        goto fail;
    }

    BT_DBG("ConfirmationSalt: %s", bt_hex(link[idx].conf_salt, 16));

    if (bt_mesh_prov_conf_key(link[idx].dhkey, link[idx].conf_salt, link[idx].conf_key)) {
        BT_ERR("Failed to generate confirmation key");
        goto fail;
    }

    BT_DBG("ConfirmationKey: %s", bt_hex(link[idx].conf_key, 16));

    link[idx].rand = bt_mesh_calloc(PROV_RANDOM_SIZE);
    if (!link[idx].rand) {
        BT_ERR("%s, Out of memory", __func__);
        goto fail;
    }

    if (bt_mesh_rand(link[idx].rand, PROV_RANDOM_SIZE)) {
        BT_ERR("Failed to generate random number");
        goto fail;
    }

    BT_DBG("LocalRandom: %s", bt_hex(link[idx].rand, 16));

    prov_buf_init(&buf, PROV_CONFIRM);

    conf = net_buf_simple_add(&buf, 16);

    if (bt_mesh_prov_conf(link[idx].conf_key, link[idx].rand,
                          link[idx].auth, conf)) {
        BT_ERR("Failed to generate confirmation value");
        goto fail;
    }

    link[idx].local_conf = bt_mesh_calloc(PROV_CONFIRM_SIZE);
    if (!link[idx].local_conf) {
        BT_ERR("%s, Out of memory", __func__);
        goto fail;
    }

    memcpy(link[idx].local_conf, conf, PROV_CONFIRM_SIZE);

    if (prov_send(idx, &buf)) {
        BT_ERR("Failed to send Provisioning Confirm");
        goto fail;
    }

    link[idx].expect = PROV_CONFIRM;
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

int bt_mesh_provisioner_set_oob_input_data(const uint8_t idx, const uint8_t *val, bool num_flag)
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
        BT_ERR("Invalid link auth");
        return -EINVAL;
    }

    BT_INFO("Link idx %d, type %s", idx, num_flag ? "number" : "string");

    memset(link[idx].auth, 0, 16);
    if (num_flag) {
        /* Provisioner inputs number */
        sys_memcpy_swap(link[idx].auth + 12, val, sizeof(uint32_t));
    } else {
        /* Provisioner inputs string */
        memcpy(link[idx].auth, val, link[idx].auth_size);
    }

    send_confirm(idx);
    return 0;
}

int bt_mesh_provisioner_set_oob_output_data(const uint8_t idx, const uint8_t *num,
                                            uint8_t size, bool num_flag)
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
    if (num == NULL || size > BLE_MESH_PROV_INPUT_OOB_MAX_LEN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (!link[idx].auth) {
        BT_ERR("Invalid link auth");
        return -EINVAL;
    }

    BT_INFO("Link idx %d, type %s", idx, num_flag ? "number" : "string");

    if (num_flag) {
        /* Provisioner output number */
        memset(link[idx].auth, 0, 16);
        sys_memcpy_swap(link[idx].auth + 16 - size, num, size);
    } else {
        /* Provisioner output string */
        memset(link[idx].auth, 0, 16);
        memcpy(link[idx].auth, num, size);
    }

    link[idx].expect = PROV_INPUT_COMPLETE;

    return 0;
}

int bt_mesh_provisioner_read_oob_pub_key(const uint8_t idx, const uint8_t pub_key_x[32],
                                         const uint8_t pub_key_y[32])
{
    if (!link[idx].conf_inputs) {
        BT_ERR("Invalid link conf_inputs");
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

static void prov_dh_key_cb(const uint8_t key[32], const uint8_t idx)
{
    BT_DBG("%p", key);

    if (!key) {
        BT_ERR("Failed to generate DHKey");
        goto fail;
    }

    link[idx].dhkey = (uint8_t *)bt_mesh_calloc(PROV_DH_KEY_SIZE);
    if (!link[idx].dhkey) {
        BT_ERR("%s, Out of memory", __func__);
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
        BT_ERR("Failed to authenticate");
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

static void prov_gen_dh_key(const uint8_t idx)
{
    uint8_t pub_key[64] = {0};

    /* Copy device public key in little-endian for bt_mesh_dh_key_gen().
     * X and Y halves are swapped independently.
     */
    sys_memcpy_swap(&pub_key[0], &link[idx].conf_inputs[81], 32);
    sys_memcpy_swap(&pub_key[32], &link[idx].conf_inputs[113], 32);

    if (bt_mesh_dh_key_gen(pub_key, prov_dh_key_cb, idx)) {
        BT_ERR("Failed to generate DHKey");
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }
}

static void send_pub_key(const uint8_t idx, uint8_t oob)
{
    PROV_BUF(buf, 65);
    const uint8_t *key = NULL;

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("No public key available");
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
        BT_ERR("Failed to send Provisioning Public Key");
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

static void prov_pub_key(const uint8_t idx, const uint8_t *data)
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

static void prov_input_complete(const uint8_t idx, const uint8_t *data)
{
    /* Make sure received pdu is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    /* Provisioner receives input complete and send confirm */
    send_confirm(idx);
}

static void prov_confirm(const uint8_t idx, const uint8_t *data)
{
    PROV_BUF(buf, 17);

    BT_DBG("Remote Confirm: %s", bt_hex(data, 16));

    /* NOTE: The Bluetooth SIG recommends that potentially vulnerable mesh provisioners
     * restrict the authentication procedure and not accept provisioning random and
     * provisioning confirmation numbers from a remote peer that are the same as those
     * selected by the local device (CVE-2020-26560).
     */
    if (!memcmp(data, link[idx].local_conf, 16)) {
        BT_ERR("Confirmation value is identical to ours, rejecting.");
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    /* Make sure received pdu is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    link[idx].conf = (uint8_t *)bt_mesh_calloc(PROV_CONFIRM_SIZE);
    if (!link[idx].conf) {
        BT_ERR("%s, Out of memory", __func__);
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
        BT_ERR("Failed to send Provisioning Random");
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    link[idx].expect = PROV_RANDOM;
}

static void send_prov_data(const uint8_t idx)
{
    PROV_BUF(buf, 34);
    uint16_t prev_addr = BLE_MESH_ADDR_UNASSIGNED;
    uint16_t max_addr = BLE_MESH_ADDR_UNASSIGNED;
    struct bt_mesh_node *node = NULL;
    const uint8_t *netkey = NULL;
    uint8_t session_key[16] = {0};
    uint8_t nonce[13] = {0};
    uint8_t pdu[25] = {0};
    int err = 0;

    err = bt_mesh_session_key(link[idx].dhkey, link[idx].prov_salt, session_key);
    if (err) {
        BT_ERR("Failed to generate session key");
        goto fail;
    }
    BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

    err = bt_mesh_prov_nonce(link[idx].dhkey, link[idx].prov_salt, nonce);
    if (err) {
        BT_ERR("Failed to generate session nonce");
        goto fail;
    }
    BT_DBG("Nonce: %s", bt_hex(nonce, 13));

    /* Assign provisioning data for the device. Currently all provisioned devices
     * will be added to the primary subnet, and may add an API to choose to which
     * subnet will the device be provisioned later.
     */
    if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && FAST_PROV_ENABLE()) {
        netkey = bt_mesh_fast_prov_net_key_get(prov_ctx.fast_prov.net_idx);
        if (!netkey) {
            BT_ERR("No NetKey for fast provisioning");
            goto fail;
        }
        memcpy(pdu, netkey, 16);
        sys_put_be16(prov_ctx.fast_prov.net_idx, &pdu[16]);
        pdu[18] = prov_ctx.fast_prov.flags;
        sys_put_be32(prov_ctx.fast_prov.iv_index, &pdu[19]);
    } else {
        netkey = bt_mesh_provisioner_net_key_get(prov_ctx.curr_net_idx);
        if (!netkey) {
            BT_ERR("No NetKey for provisioning data");
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
             * the device is bigger now, then we treat it as a new device.
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
        uint16_t alloc_addr = BLE_MESH_ADDR_UNASSIGNED;

        if (BLE_MESH_ADDR_IS_UNICAST(link[idx].assign_addr)) {
            alloc_addr = link[idx].assign_addr;
        } else {
            /* If this device to be provisioned is a new device */
            if (prov_ctx.curr_alloc_addr == BLE_MESH_ADDR_UNASSIGNED) {
                BT_ERR("Not enough unicast address to be allocated");
                goto fail;
            }
            alloc_addr = prov_ctx.curr_alloc_addr;
        }

        if (alloc_addr + link[idx].element_num - 1 > max_addr) {
            BT_ERR("Not enough unicast address for the device");
            goto fail;
        }

        /* Make sure the assigned unicast address is not identical with any unicast
         * address of other nodes. And make sure the address is not identical with
         * any unicast address of Provisioner.
         */
        if (bt_mesh_provisioner_check_is_addr_dup(alloc_addr, link[idx].element_num, true)) {
            BT_ERR("Duplicate assigned address 0x%04x", alloc_addr);
            goto fail;
        }

        sys_put_be16(alloc_addr, &pdu[23]);
        link[idx].unicast_addr = alloc_addr;
    }

    prov_buf_init(&buf, PROV_DATA);

    err = bt_mesh_prov_encrypt(session_key, nonce, pdu, net_buf_simple_add(&buf, 33));
    if (err) {
        BT_ERR("Failed to encrypt provisioning data");
        goto fail;
    }

    if (prov_send(idx, &buf)) {
        BT_ERR("Failed to send Provisioning Data");
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

    if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && FAST_PROV_ENABLE()) {
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

static void prov_random(const uint8_t idx, const uint8_t *data)
{
    uint8_t conf_verify[16] = {0};

    BT_DBG("Remote Random: %s", bt_hex(data, 16));

    /* NOTE: The Bluetooth SIG recommends that potentially vulnerable mesh provisioners
     * restrict the authentication procedure and not accept provisioning random and
     * provisioning confirmation numbers from a remote peer that are the same as those
     * selected by the local device (CVE-2020-26560).
     */
    if (!memcmp(data, link[idx].rand, 16)) {
        BT_ERR("Random value is identical to ours, rejecting.");
        goto fail;
    }

    if (bt_mesh_prov_conf(link[idx].conf_key, data, link[idx].auth, conf_verify)) {
        BT_ERR("Failed to calculate confirmation verification");
        goto fail;
    }

    if (memcmp(conf_verify, link[idx].conf, 16)) {
        BT_ERR("Invalid confirmation value");
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
    link[idx].prov_salt = (uint8_t *)bt_mesh_calloc(PROV_PROV_SALT_SIZE);
    if (!link[idx].prov_salt) {
        BT_ERR("%s, Out of memory", __func__);
        goto fail;
    }

    if (bt_mesh_prov_salt(link[idx].conf_salt, link[idx].rand, data,
                          link[idx].prov_salt)) {
        BT_ERR("Failed to generate ProvisioningSalt");
        goto fail;
    }

    BT_DBG("ProvisioningSalt: %s", bt_hex(link[idx].prov_salt, 16));

    send_prov_data(idx);
    return;

fail:
    close_link(idx, CLOSE_REASON_FAILED);
    return;
}

static void prov_complete(const uint8_t idx, const uint8_t *data)
{
    uint8_t device_key[16] = {0};
    uint16_t net_idx = 0U;
    uint16_t index = 0U;
    int err = 0;
    int i;

    /* Make sure received pdu is ok and cancel the timeout timer */
    if (bt_mesh_atomic_test_and_clear_bit(link[idx].flags, TIMEOUT_START)) {
        k_delayed_work_cancel(&link[idx].timeout);
    }

    err = bt_mesh_dev_key(link[idx].dhkey, link[idx].prov_salt, device_key);
    if (err) {
        BT_ERR("Failed to generate device key");
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && FAST_PROV_ENABLE()) {
        net_idx = prov_ctx.fast_prov.net_idx;
    } else {
        net_idx = prov_ctx.curr_net_idx;
    }
    err = bt_mesh_provisioner_provision(&link[idx].addr, link[idx].uuid, link[idx].oob_info,
                                        link[idx].unicast_addr, link[idx].element_num, net_idx,
                                        link[idx].ki_flags, link[idx].iv_index, device_key, &index);
    if (err) {
        BT_ERR("Failed to store node info");
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (prov->prov_complete) {
        prov->prov_complete(index, link[idx].uuid, link[idx].unicast_addr,
                            link[idx].element_num, net_idx);
    }

    /* Find if the device is in the device queue */
    for (i = 0; i < ARRAY_SIZE(unprov_dev); i++) {
        if (!memcmp(unprov_dev[i].uuid, link[idx].uuid, 16) &&
            (unprov_dev[i].flags & RM_AFTER_PROV)) {
            memset(&unprov_dev[i], 0, sizeof(struct unprov_dev_queue));
            break;
        }
    }

    close_link(idx, CLOSE_REASON_SUCCESS);
}

static void prov_failed(const uint8_t idx, const uint8_t *data)
{
    BT_WARN("Error 0x%02x", data[0]);

    close_link(idx, CLOSE_REASON_FAILED);
}

static const struct {
    void (*func)(const uint8_t idx, const uint8_t *data);
    uint16_t len;
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

static void close_link(const uint8_t idx, uint8_t reason)
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

    BT_ERR("Invalid link idx %d", idx);
    return;
}

static void prov_timeout(struct k_work *work)
{
    uint8_t idx = (uint8_t)work->index;

    BT_WARN("%s", __func__);

    close_link(idx, CLOSE_REASON_TIMEOUT);
}

#if defined(CONFIG_BLE_MESH_PB_ADV)
static void prov_retransmit(struct k_work *work)
{
    int64_t timeout = TRANSACTION_TIMEOUT;
    uint8_t idx = (uint8_t)work->index;
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
        uint8_t reason = (link[idx].send_link_close >> 1) & BIT_MASK(2);
        uint16_t count = (link[idx].send_link_close >> 3);
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

static void link_ack(const uint8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (buf->len) {
        BT_ERR("Invalid Link ACK length %d", buf->len);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (link[idx].expect == PROV_CAPABILITIES) {
        BT_INFO("Link ACK is already received");
        return;
    }

    link[idx].conf_inputs = (uint8_t *)bt_mesh_calloc(PROV_CONF_INPUTS_SIZE);
    if (!link[idx].conf_inputs) {
        BT_ERR("%s, Out of memory", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    send_invite(idx);
}

static void link_close(const uint8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    uint8_t reason = 0U;

    BT_DBG("len %u", buf->len);

    reason = net_buf_simple_pull_u8(buf);

    reset_link(idx, reason);
}

static void gen_prov_ctl(const uint8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
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
        BT_ERR("Unknown bearer opcode 0x%02x", BEARER_CTL(rx->gpc));
        return;
    }
}

static void prov_msg_recv(const uint8_t idx)
{
    uint8_t type = link[idx].rx.buf->data[0];

    BT_DBG("type 0x%02x len %u", type, link[idx].rx.buf->len);

    /**
     * Provisioner first checks information within the received
     * Provisioning PDU. If the check succeeds then check fcs.
     */
    if (type != PROV_FAILED && type != link[idx].expect) {
        BT_ERR("Unexpected msg 0x%02x != 0x%02x", type, link[idx].expect);
        goto fail;
    }

    if (type >= 0x0A) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        goto fail;
    }

    if (1 + prov_handlers[type].len != link[idx].rx.buf->len) {
        BT_ERR("Invalid length %u for type 0x%02x", link[idx].rx.buf->len, type);
        goto fail;
    }

    if (!bt_mesh_fcs_check(link[idx].rx.buf, link[idx].rx.fcs)) {
        BT_ERR("Incorrect FCS");
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

static void gen_prov_cont(const uint8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    uint8_t seg = CONT_SEG_INDEX(rx->gpc);

    BT_DBG("len %u, seg_index %u", buf->len, seg);

    if (!link[idx].rx.seg && link[idx].rx.prev_id == rx->xact_id) {
        BT_INFO("Resending ack");
        gen_prov_ack_send(idx, rx->xact_id);
        return;
    }

    if (rx->xact_id != link[idx].rx.trans_id) {
        BT_WARN("Data for unknown transaction (%u != %u)",
            rx->xact_id, link[idx].rx.trans_id);
        return;
    }

    if (seg > link[idx].rx.last_seg) {
        BT_ERR("Invalid segment index %u", seg);
        goto fail;
    } else if (seg == link[idx].rx.last_seg) {
        uint8_t expect_len = 0U;

        expect_len = (link[idx].rx.buf->len - 20 -
                      (23 * (link[idx].rx.last_seg - 1)));
        if (expect_len != buf->len) {
            BT_ERR("Incorrect last seg len: %u != %u",
                    expect_len, buf->len);
            goto fail;
        }
    }

    if (!(link[idx].rx.seg & BIT(seg))) {
        BT_INFO("Ignore already received segment");
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

static void gen_prov_ack(const uint8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    uint8_t ack_type = 0U, pub_key_oob = 0U;

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
            /* For case MESH/PVNR/PROV/BV-04-C, if using OOB public key,
             * the value of expect_ack_for shall be PROV_PUB_KEY.
             */
            if (pub_key_oob) {
                return;
            }
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

static void gen_prov_start(const uint8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (link[idx].rx.seg) {
        BT_INFO("Get Start while there are unreceived segments");
        return;
    }

    if (link[idx].rx.prev_id == rx->xact_id) {
        BT_INFO("Resending ack");
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
        BT_ERR("Ignoring zero-length provisioning PDU");
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (START_LAST_SEG(rx->gpc) > START_LAST_SEG_MAX) {
        BT_ERR("Invalid SegN 0x%02x", START_LAST_SEG(rx->gpc));
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (link[idx].rx.buf->len > link[idx].rx.buf->size) {
        BT_ERR("Too large provisioning PDU (%u bytes)",
                link[idx].rx.buf->len);
        close_link(idx, CLOSE_REASON_FAILED);
        return;
    }

    if (START_LAST_SEG(rx->gpc) > 0 && link[idx].rx.buf->len <= 20) {
        BT_ERR("Too small total length for multi-segment PDU");
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
    void (*const func)(const uint8_t idx, struct prov_rx *rx, struct net_buf_simple *buf);
    const uint8_t require_link;
    const uint8_t min_len;
} gen_prov[] = {
    { gen_prov_start, true,  3 },
    { gen_prov_ack,   true,  0 },
    { gen_prov_cont,  true,  0 },
    { gen_prov_ctl,   true,  0 },
};

static void gen_prov_recv(const uint8_t idx, struct prov_rx *rx, struct net_buf_simple *buf)
{
    if (buf->len < gen_prov[GPCF(rx->gpc)].min_len) {
        BT_ERR("Too short GPC message type %u", GPCF(rx->gpc));
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

static int find_link(uint32_t link_id, uint8_t *idx)
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
    uint8_t idx = 0U;

    rx.link_id = net_buf_simple_pull_be32(buf);
    if (find_link(rx.link_id, &idx) < 0) {
        BT_DBG("Data for unexpected link");
        return;
    }

    if (buf->len < 2) {
        BT_ERR("Too short provisioning packet (len %u)", buf->len);
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
static struct bt_mesh_conn *find_conn(struct bt_mesh_conn *conn, uint8_t *idx)
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
    uint8_t type = 0U;
    uint8_t idx = 0U;

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    if (!find_conn(conn, &idx)) {
        BT_ERR("Data for unexpected connection");
        return -ENOTCONN;
    }

    if (buf->len < 1) {
        BT_ERR("Too short provisioning packet (len %u)", buf->len);
        goto fail;
    }

    type = net_buf_simple_pull_u8(buf);
    if (type != PROV_FAILED && type != link[idx].expect) {
        BT_ERR("Unexpected msg 0x%02x != 0x%02x", type, link[idx].expect);
        goto fail;
    }

    if (type >= 0x0A) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        goto fail;
    }

    if (prov_handlers[type].len != buf->len) {
        BT_ERR("Invalid length %u for type 0x%02x", buf->len, type);
        goto fail;
    }

    prov_handlers[type].func(idx, buf->data);

    return 0;

fail:
    /* Mesh Spec Section 5.4.4 Provisioning errors */
    close_link(idx, CLOSE_REASON_FAILED);
    return -EINVAL;
}

int bt_mesh_provisioner_set_prov_conn(const uint8_t addr[6], struct bt_mesh_conn *conn)
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

    BT_ERR("Addr %s not found", bt_hex(addr, BLE_MESH_ADDR_LEN));
    return -ENOMEM;
}

int bt_mesh_provisioner_pb_gatt_open(struct bt_mesh_conn *conn, uint8_t *addr)
{
    uint8_t idx = 0U;
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
        BT_ERR("Link not found");
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

    link[idx].conf_inputs = (uint8_t *)bt_mesh_calloc(PROV_CONF_INPUTS_SIZE);
    if (!link[idx].conf_inputs) {
        /* Disconnect this connection, clear corresponding informations */
        BT_ERR("%s, Out of memory", __func__);
        close_link(idx, CLOSE_REASON_FAILED);
        return -ENOMEM;
    }

    send_invite(idx);
    return 0;
}

int bt_mesh_provisioner_pb_gatt_close(struct bt_mesh_conn *conn, uint8_t reason)
{
    uint8_t idx = 0U;

    BT_DBG("conn %p", conn);

    if (!find_conn(conn, &idx)) {
        BT_ERR("Conn %p not found", conn);
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
    const uint8_t *key = NULL;
    int i;

    if (!prov_info) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("Failed to generate Public Key");
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

int bt_mesh_provisioner_prov_reset(bool erase)
{
    int i;

    if (prov == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        k_delayed_work_cancel(&link[i].timeout);

        prov_memory_free(i);

        if (i < CONFIG_BLE_MESH_PBA_SAME_TIME) {
#if CONFIG_BLE_MESH_PB_ADV
            prov_clear_tx(i);
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
            bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID, &link[i].link_id);
#endif
            memset(&link[i], 0, offsetof(struct prov_link, tx.retransmit));
            link[i].pending_ack = XACT_NVAL;
            link[i].rx.prev_id = XACT_NVAL;
            link[i].rx.buf = bt_mesh_pba_get_buf(i);
#endif /* CONFIG_BLE_MESH_PB_ADV */
        } else {
            memset(&link[i], 0, offsetof(struct prov_link, timeout));
        }

        if (bt_mesh_pub_key_get()) {
            bt_mesh_atomic_set_bit(link[i].flags, LOCAL_PUB_KEY);
        }
    }

    /* static_oob_len & static_oob_val are initialized during mesh init.
     * When reset the Provisioner, they should not be reset. Otherwise
     * users need to invoke the corresponding function to set the static
     * oob information before using them.
     */
    memset(&prov_ctx, 0, offsetof(struct bt_mesh_prov_ctx, static_oob_len));
    prov_ctx.match_offset = 0;
    prov_ctx.match_length = 0;
    prov_ctx.prov_after_match = false;
    memset(prov_ctx.match_value, 0, sizeof(prov_ctx.match_value));
    memset(&prov_ctx.fast_prov, 0, sizeof(prov_ctx.fast_prov));

    memset(unprov_dev, 0, sizeof(unprov_dev));

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && erase) {
        bt_mesh_clear_prov_info();
    }

    return 0;
}

#if CONFIG_BLE_MESH_DEINIT
int bt_mesh_provisioner_prov_deinit(bool erase)
{
    int i;

    if (prov == NULL) {
        BT_ERR("%s, No provisioning context provided", __func__);
        return -EINVAL;
    }

    bt_mesh_provisioner_prov_reset(erase);

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
#if defined(CONFIG_BLE_MESH_PB_ADV)
        if (i < CONFIG_BLE_MESH_PBA_SAME_TIME) {
            k_delayed_work_free(&link[i].tx.retransmit);
        }
#endif
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
    prov_ctx.static_oob_len = 0U;
    memset(prov_ctx.static_oob_val, 0, sizeof(prov_ctx.static_oob_val));

#if defined(CONFIG_BLE_MESH_PB_ADV)
    memset(adv_buf, 0, sizeof(adv_buf));
    memset(adv_buf_data, 0, sizeof(adv_buf_data));
#endif

    prov = NULL;

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static bool is_unprov_dev_info_callback_to_app(bt_mesh_prov_bearer_t bearer, const uint8_t uuid[16],
                                               const bt_mesh_addr_t *addr, uint16_t oob_info, int8_t rssi)
{
    int i;

    if (prov_ctx.prov_after_match == false) {
        uint8_t adv_type = (bearer == BLE_MESH_PROV_ADV) ?
                            BLE_MESH_ADV_NONCONN_IND : BLE_MESH_ADV_IND;

        for (i = 0; i < ARRAY_SIZE(unprov_dev); i++) {
            if (!memcmp(unprov_dev[i].uuid, uuid, 16)) {
                break;
            }
        }

        if (i == ARRAY_SIZE(unprov_dev)) {
            BT_DBG("Device not in queue, notify to app layer");
            if (notify_unprov_adv_pkt_cb) {
                notify_unprov_adv_pkt_cb(addr->val, addr->type, adv_type, uuid, oob_info, bearer, rssi);
            }
            return true;
        }

        if (!(unprov_dev[i].bearer & bearer)) {
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

void bt_mesh_provisioner_unprov_beacon_recv(struct net_buf_simple *buf, int8_t rssi)
{
#if defined(CONFIG_BLE_MESH_PB_ADV)
    const bt_mesh_addr_t *addr = NULL;
    const uint8_t *uuid = NULL;
    uint16_t oob_info = 0U;

    if (!(prov_ctx.bearers & BLE_MESH_PROV_ADV)) {
        BT_WARN("Not support PB-ADV bearer");
        return;
    }

    if (buf->len != 0x12 && buf->len != 0x16) {
        BT_ERR("Invalid Unprovisioned Device Beacon length %d", buf->len);
        return;
    }

    addr = bt_mesh_get_unprov_dev_addr();
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

void bt_mesh_provisioner_prov_adv_recv(struct net_buf_simple *buf,
                                       const bt_mesh_addr_t *addr, int8_t rssi)
{
#if defined(CONFIG_BLE_MESH_PB_GATT)
    const uint8_t *uuid = NULL;
    uint16_t oob_info = 0U;

    if (!(prov_ctx.bearers & BLE_MESH_PROV_GATT)) {
        BT_WARN("Not support PB-GATT bearer");
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
