/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "crypto.h"
#include "adv.h"
#include "scan.h"
#include "mesh.h"
#include "access.h"
#include "settings.h"
#include "fast_prov.h"
#include "mesh/common.h"
#include "proxy_common.h"
#include "proxy_client.h"
#include "prov_common.h"
#include "prov_node.h"
#include "prov_pvnr.h"
#include "pvnr_mgmt.h"

#include "mesh_v1.1/utils.h"

#if CONFIG_BLE_MESH_PROVISIONER

_Static_assert(BLE_MESH_MAX_CONN >= CONFIG_BLE_MESH_PBG_SAME_TIME,
               "Too large BLE Mesh PB-GATT count");

/* Number of devices can be provisioned at the same time equals to PB-ADV + PB-GATT */
#define BLE_MESH_PROV_SAME_TIME \
        (CONFIG_BLE_MESH_PBA_SAME_TIME + CONFIG_BLE_MESH_PBG_SAME_TIME)

#define UNICAST_ADDR_LIMIT  0x7FFF

#define PROV_SVC_ADV_RX_CHECK(pre, cur)   ((cur) < (pre) ? ((cur) + (UINT32_MAX - (pre)) >= 200) : ((cur) - (pre) >= 200))

/* Provisioner link structure allocation
 * |--------------------------------------------------------|
 * |            Link(PB-ADV)            |   Link(PB-GATT)   |
 * |--------------------------------------------------------|
 * |<----------------------Total Link---------------------->|
 */
static struct bt_mesh_prov_link prov_links[BLE_MESH_PROV_SAME_TIME];

struct bt_mesh_prov_ctx {
    /* Primary element address of Provisioner */
    uint16_t primary_addr;

    /* Provisioning bearers used by Provisioner */
    bt_mesh_prov_bearer_t bearers;

#if CONFIG_BLE_MESH_PB_ADV
    /* Current number of PB-ADV provisioned devices simultaneously */
    uint8_t  pba_count;
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
    /* Current number of PB-GATT provisioned devices simultaneously */
    uint8_t  pbg_count;
#endif /* CONFIG_BLE_MESH_PB_GATT */

    /* Current unicast address going to allocated */
    uint16_t alloc_addr;

    /* Current net_idx going to be used in provisioning data */
    uint16_t net_idx;

    /* Length of Static OOB value */
    uint8_t  static_oob_len;

    /* Static OOB value */
    uint8_t  static_oob_val[32];

    /* Offset of the device uuid to be matched, based on zero */
    uint8_t  match_offset;

    /* Length of the device uuid to be matched (start from the match_offset) */
    uint8_t  match_length;

    /* Value of the device uuid to be matched */
    uint8_t  match_value[16];

    /* Indicate when received uuid_match adv_pkts, can provision it at once */
    bool     prov_after_match;

#if CONFIG_BLE_MESH_PB_ADV
    /* Mutex used to protect the PB-ADV procedure */
    bt_mesh_mutex_t pb_adv_lock;
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
    /* Mutex used to protect the PB-GATT procedure */
    bt_mesh_mutex_t pb_gatt_lock;
#endif /* CONFIG_BLE_MESH_PB_GATT */

    /* Fast provisioning related information */
    struct {
        bool     enable;
        uint16_t net_idx;
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
} __attribute__((packed)) unprov_dev[CONFIG_BLE_MESH_WAIT_FOR_PROV_MAX_DEV_NUM] = {
    [0 ... (CONFIG_BLE_MESH_WAIT_FOR_PROV_MAX_DEV_NUM - 1)] = {
        .addr.type = 0xff,
        .bearer    = 0,
        .flags     = false,
    },
};

static unprov_adv_pkt_cb_t  notify_unprov_adv_pkt_cb;

#if CONFIG_BLE_MESH_PB_ADV
static void send_link_open(struct bt_mesh_prov_link *link);
extern uint8_t pvnr_next_xact_id(struct bt_mesh_prov_link *link);
#endif

static void prov_gen_dh_key(struct bt_mesh_prov_link *link);
static void send_pub_key(struct bt_mesh_prov_link *link);
static void close_link(struct bt_mesh_prov_link *link, uint8_t reason);
static void send_invite(struct bt_mesh_prov_link *link);

#if CONFIG_BLE_MESH_PB_ADV
static struct prov_rx_buf {
    struct net_buf_simple buf;
} rx_buf[CONFIG_BLE_MESH_PBA_SAME_TIME];

static uint8_t rx_buf_data[PROV_RX_BUF_SIZE * CONFIG_BLE_MESH_PBA_SAME_TIME];
#endif /* CONFIG_BLE_MESH_PB_ADV */

#define PROV_FREE_MEM(_idx, member)             \
{                                               \
    if (prov_links[_idx].member) {              \
        bt_mesh_free(prov_links[_idx].member);  \
        prov_links[_idx].member = NULL;         \
    }                                           \
}

struct bt_mesh_prov_link *bt_mesh_prov_pvnr_get_link(void)
{
    return &prov_links[0];
}

uint8_t bt_mesh_prov_pvnr_get_link_count(void)
{
    return BLE_MESH_PROV_SAME_TIME;
}

void bt_mesh_prov_pvnr_close_link(struct bt_mesh_prov_link *link, uint8_t reason)
{
    close_link(link, reason);
}

void bt_mesh_prov_pvnr_send_invite(struct bt_mesh_prov_link *link)
{
    send_invite(link);
}

#if CONFIG_BLE_MESH_PB_ADV
static inline void bt_mesh_pb_adv_lock(void)
{
    bt_mesh_mutex_lock(&prov_ctx.pb_adv_lock);
}

static inline void bt_mesh_pb_adv_unlock(void)
{
    bt_mesh_mutex_unlock(&prov_ctx.pb_adv_lock);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
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
        if (!memcmp(prov_links[i].addr.val, addr, BLE_MESH_ADDR_LEN)) {
            bt_mesh_atomic_clear_bit(prov_links[i].flags, CONNECTING);
            prov_links[i].conn = NULL;
            prov_links[i].oob_info = 0x0;
            memset(prov_links[i].uuid, 0, 16);
            memset(&prov_links[i].addr, 0, sizeof(bt_mesh_addr_t));
            bt_mesh_atomic_clear_bit(prov_links[i].flags, LINK_ACTIVE);
            k_delayed_work_cancel(&prov_links[i].prot_timer);
            return;
        }
    }

    BT_WARN("Device not found, addr %s", bt_hex(addr, BLE_MESH_ADDR_LEN));
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_SETTINGS
void bt_mesh_provisioner_restore_prov_info(uint16_t primary_addr, uint16_t alloc_addr)
{
    prov_ctx.primary_addr = primary_addr;
    prov_ctx.alloc_addr = alloc_addr;
}
#endif /* CONFIG_BLE_MESH_SETTINGS */

static bool is_unprov_dev_being_provision(const uint8_t uuid[16])
{
    int i;

#if CONFIG_BLE_MESH_FAST_PROV
    /* During Fast Provisioning test, we found that if a device has already being
     * provisioned, there is still a chance that the Provisioner can receive the
     * Unprovisioned Device Beacon from the device (because the device will stop
     * Unprovisioned Device Beacon when Transaction ACK for Provisioning Complete
     * is received). So in Fast Provisioning the Provisioner should ignore this.
     */
    if (bt_mesh_provisioner_get_node_with_uuid(uuid)) {
        BT_WARN("Device has already been provisioned");
        return true;
    }
#endif /* CONFIG_BLE_MESH_FAST_PROV */

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (bt_mesh_atomic_test_bit(prov_links[i].flags, LINK_ACTIVE)
#if CONFIG_BLE_MESH_PB_GATT
            || bt_mesh_atomic_test_bit(prov_links[i].flags, CONNECTING)
#endif
            ) {
            if (!memcmp(prov_links[i].uuid, uuid, 16)) {
                BT_DBG("Device is being provisioning");
                return true;
            }
        }
    }

    return false;
}

static bool is_unprov_dev_uuid_match(const uint8_t uuid[16])
{
    /* Match value is not configured */
    if (prov_ctx.match_length == 0) {
        return true;
    }

    if (memcmp(uuid + prov_ctx.match_offset,
               prov_ctx.match_value, prov_ctx.match_length)) {
        return false;
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

#if CONFIG_BLE_MESH_PB_ADV
    /* Check if the current PB-ADV link is full */
    if ((prov_ctx.bearers & BLE_MESH_PROV_ADV) &&
        (bearer == BLE_MESH_PROV_ADV) &&
        (prov_ctx.pba_count == CONFIG_BLE_MESH_PBA_SAME_TIME)) {
        BT_INFO("Current PB-ADV links reach max limit");
        return -ENOMEM;
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
    /* Check if the current PB-GATT link is full */
    if ((prov_ctx.bearers & BLE_MESH_PROV_GATT) &&
        (bearer == BLE_MESH_PROV_GATT) &&
        (prov_ctx.pbg_count == CONFIG_BLE_MESH_PBG_SAME_TIME)) {
        BT_INFO("Current PB-GATT links reach max limit");
        return -ENOMEM;
    }
#endif /* CONFIG_BLE_MESH_PB_GATT */

    /* Check if the device has already been provisioned */
    if (bt_mesh_provisioner_get_node_with_uuid(uuid)) {
        BT_INFO("Provisioned before, start to provision again");
    }

    return 0;
}

#if CONFIG_BLE_MESH_PB_ADV
static int provisioner_start_prov_pb_adv(const uint8_t uuid[16], const bt_mesh_addr_t *addr,
                                         uint16_t oob_info, uint16_t assign_addr)
{
    int i;

    if (uuid == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_pb_adv_lock();

    /* If the unicast address of the device is allocated internally,
     * then we need to check if there are addresses available.
     */
    if (assign_addr == BLE_MESH_ADDR_UNASSIGNED &&
        prov_ctx.alloc_addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_ERR("No available unicast address to assign");
        bt_mesh_pb_adv_unlock();
        return -EIO;
    }

    if (is_unprov_dev_being_provision(uuid)) {
        bt_mesh_pb_adv_unlock();
        return -EALREADY;
    }

    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        if (!bt_mesh_atomic_test_bit(prov_links[i].flags, LINK_ACTIVE) &&
            !bt_mesh_atomic_test_bit(prov_links[i].flags, LINK_CLOSING)) {
            memcpy(prov_links[i].uuid, uuid, 16);
            prov_links[i].oob_info = oob_info;
            if (addr) {
                prov_links[i].addr.type = addr->type;
                memcpy(prov_links[i].addr.val, addr->val, BLE_MESH_ADDR_LEN);
            }

            send_link_open(&prov_links[i]);

            /* If a specific unicast address is assigned for the device, then
             * Provisioner will use this address in the Provisioning Data PDU.
             */
            if (BLE_MESH_ADDR_IS_UNICAST(assign_addr)) {
                prov_links[i].assign_addr = assign_addr;
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

#if CONFIG_BLE_MESH_PB_GATT
static int provisioner_start_prov_pb_gatt(const uint8_t uuid[16], const bt_mesh_addr_t *addr,
                                          uint16_t oob_info, uint16_t assign_addr)
{
    int i;

    if (uuid == NULL || addr == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    bt_mesh_pb_gatt_lock();

    /* If the unicast address of the device is allocated internally,
     * then we need to check if there are addresses available.
     */
    if (assign_addr == BLE_MESH_ADDR_UNASSIGNED &&
        prov_ctx.alloc_addr == BLE_MESH_ADDR_UNASSIGNED) {
        BT_ERR("No available unicast address to assign");
        bt_mesh_pb_gatt_unlock();
        return -EIO;
    }

    if (is_unprov_dev_being_provision(uuid)) {
        bt_mesh_pb_gatt_unlock();
        return -EALREADY;
    }

    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (!bt_mesh_atomic_test_bit(prov_links[i].flags, CONNECTING) &&
            !bt_mesh_atomic_test_bit(prov_links[i].flags, LINK_ACTIVE)) {
            if (bt_mesh_gattc_conn_create(addr, BLE_MESH_UUID_MESH_PROV_VAL)) {
                bt_mesh_pb_gatt_unlock();
                return -EIO;
            }

            memcpy(prov_links[i].uuid, uuid, 16);
            prov_links[i].oob_info = oob_info;
            prov_links[i].addr.type = addr->type;
            memcpy(prov_links[i].addr.val, addr->val, BLE_MESH_ADDR_LEN);

            /* If the application layer assigned a specific unicast address for the device,
             * then Provisioner will use this address in the Provisioning Data PDU.
             */
            if (BLE_MESH_ADDR_IS_UNICAST(assign_addr)) {
                prov_links[i].assign_addr = assign_addr;
            }

            /* If creating connection successfully, set CONNECTING flag */
            bt_mesh_atomic_set_bit(prov_links[i].flags, CONNECTING);

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
    if (bt_mesh_provisioner_get_node_with_uuid(add_dev->uuid) == NULL &&
        bt_mesh_provisioner_get_node_count() == CONFIG_BLE_MESH_MAX_PROV_NODES) {
        BT_WARN("Current provisioned devices reach max limit");
        return -ENOMEM;
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
        if (bt_mesh_provisioner_get_node_count()
#if CONFIG_BLE_MESH_PB_ADV
            + prov_ctx.pba_count /* Add current PB-ADV link count */
#endif
#if CONFIG_BLE_MESH_PB_GATT
            + prov_ctx.pbg_count /* Add current PB-GATT link count */
#endif
            >= CONFIG_BLE_MESH_MAX_PROV_NODES) {
            BT_WARN("Node count + active link count reach max limit");
            return -EIO;
        }
    }

    if ((err = provisioner_check_unprov_dev_info(add_dev->uuid, add_dev->bearer))) {
        return err;
    }

#if CONFIG_BLE_MESH_PB_ADV
    if (add_dev->bearer == BLE_MESH_PROV_ADV) {
        return provisioner_start_prov_pb_adv(add_dev->uuid, addr_valid ? &add_addr : NULL,
                                             add_dev->oob_info, BLE_MESH_ADDR_UNASSIGNED);
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
    if (add_dev->bearer == BLE_MESH_PROV_GATT) {
        return provisioner_start_prov_pb_gatt(add_dev->uuid, &add_addr, add_dev->oob_info,
                                              BLE_MESH_ADDR_UNASSIGNED);
    }
#endif /* CONFIG_BLE_MESH_PB_GATT */

    /* Shall not reach here. */
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
     * 2. If the node is a re-provisioned device, then the original allocated
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
        if (bt_mesh_provisioner_get_node_count()
#if CONFIG_BLE_MESH_PB_ADV
            + prov_ctx.pba_count /* Add current PB-ADV link count */
#endif
#if CONFIG_BLE_MESH_PB_GATT
            + prov_ctx.pbg_count /* Add current PB-GATT link count */
#endif
            >= CONFIG_BLE_MESH_MAX_PROV_NODES) {
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

#if CONFIG_BLE_MESH_PB_ADV
    if (bearer == BLE_MESH_PROV_ADV) {
        return provisioner_start_prov_pb_adv(uuid, addr ? &dev_addr : NULL, oob_info, unicast_addr);
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
    if (bearer == BLE_MESH_PROV_GATT) {
        return provisioner_start_prov_pb_gatt(uuid, &dev_addr, oob_info, unicast_addr);
    }
#endif /* CONFIG_BLE_MESH_PB_GATT */

    /* Shall not reach here. */
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
    for (i = 0; i < ARRAY_SIZE(prov_links); i++) {
        if (!memcmp(prov_links[i].uuid, del_dev->uuid, 16)) {
            close_link(&prov_links[i], CLOSE_REASON_FAILED);
            break;
        }
    }

    return 0;
}

int bt_mesh_provisioner_set_dev_uuid_match(uint8_t offset, uint8_t length,
                                           const uint8_t *match, bool prov_flag)
{
    if (length && (match == NULL || (offset + length > 16))) {
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

    if (info == NULL || info->flag == 0) {
        return -EINVAL;
    }

    if (info->flag & NET_IDX_FLAG) {
        key = bt_mesh_provisioner_net_key_get(info->net_idx);
        if (!key) {
            BT_ERR("Failed to get NetKey");
            return -EINVAL;
        }

        prov_ctx.net_idx = info->net_idx;
    }

    return 0;
}

static inline uint8_t get_net_flags(uint16_t net_idx)
{
    return bt_mesh_net_flags(bt_mesh_subnet_get(net_idx));
}

int bt_mesh_provisioner_init_prov_info(void)
{
    if (prov_ctx.primary_addr == BLE_MESH_ADDR_UNASSIGNED) {
        /* If unicast address of primary element of Provisioner has not been set
         * before, then the following initialization procedure will be used.
         */
        if (bt_mesh_prov_get() == NULL) {
            BT_ERR("No provisioning context provided");
            return -EINVAL;
        }

        if (!BLE_MESH_ADDR_IS_UNICAST(bt_mesh_prov_get()->prov_unicast_addr) ||
            !BLE_MESH_ADDR_IS_UNICAST(bt_mesh_prov_get()->prov_start_address)) {
            BT_ERR("Invalid address, own 0x%04x, start 0x%04x",
                    bt_mesh_prov_get()->prov_unicast_addr,
                    bt_mesh_prov_get()->prov_start_address);
            return -EINVAL;
        }

        const struct bt_mesh_comp *comp = bt_mesh_comp_get();
        if (!comp) {
            BT_ERR("Invalid composition data");
            return -EINVAL;
        }

        if (bt_mesh_prov_get()->prov_unicast_addr + comp->elem_count >
            bt_mesh_prov_get()->prov_start_address) {
            BT_WARN("Too small start address 0x%04x, update to 0x%04x",
                    bt_mesh_prov_get()->prov_start_address,
                    bt_mesh_prov_get()->prov_unicast_addr + comp->elem_count);

            prov_ctx.alloc_addr = bt_mesh_prov_get()->prov_unicast_addr + comp->elem_count;
        } else {
            prov_ctx.alloc_addr = bt_mesh_prov_get()->prov_start_address;
        }

        /* Update primary element address with the initialized value here. */
        prov_ctx.primary_addr = bt_mesh_prov_get()->prov_unicast_addr;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.alloc_addr);
        }
    }

    prov_ctx.net_idx = BLE_MESH_KEY_PRIMARY;

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

    if (value == NULL || length == 0U || length > BLE_MESH_PROV_STATIC_OOB_MAX_LEN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Make sure Static OOB is not being used. */
    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (prov_links[i].auth_method == AUTH_METHOD_STATIC) {
            BT_ERR("Static OOB is being used");
            return -EINVAL;
        }
    }

    (void)memset(prov_ctx.static_oob_val, 0, BLE_MESH_PROV_STATIC_OOB_MAX_LEN);

    prov_ctx.static_oob_len = MIN(BLE_MESH_PROV_STATIC_OOB_MAX_LEN, length);
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

    /* If the current can-be allocated address is larger than "primary address
     * + element number", then the alloc_addr will not be changed, and only
     * the Provisioner related addresses will be updated.
     */
    if (addr + comp->elem_count > prov_ctx.alloc_addr) {
        prov_ctx.alloc_addr = addr + comp->elem_count;
    }

    BT_INFO("Primary address updated, old 0x%04x, new 0x%04x", prov_ctx.primary_addr, addr);
    prov_ctx.primary_addr = addr;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.alloc_addr);
    }

    bt_mesh_comp_provision(addr);

    return 0;
}

#if CONFIG_BLE_MESH_TEST_AUTO_ENTER_NETWORK
int bt_mesh_test_provisioner_update_alloc_addr(uint16_t unicast_addr, uint16_t element_num)
{
    uint16_t max_addr = FAST_PROV_ENABLE() ? prov_ctx.fast_prov.unicast_addr_max : UNICAST_ADDR_LIMIT;

    if (unicast_addr + element_num > max_addr) {
        BT_WARN("Not enough unicast address to allocate");
        prov_ctx.alloc_addr = BLE_MESH_ADDR_UNASSIGNED;
    } else {
        prov_ctx.alloc_addr = unicast_addr + element_num;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.alloc_addr);
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

    prov_ctx.alloc_addr = prov_ctx.fast_prov.unicast_addr_min;

    return 0x0; /* status: success */
}

static void prov_memory_free(struct bt_mesh_prov_link *link)
{
    /* TODO: memory optimization - free */
}

#if CONFIG_BLE_MESH_PB_ADV
static struct net_buf_simple *get_rx_buf(const uint8_t idx)
{
    struct net_buf_simple *buf = &(rx_buf[idx].buf);

    net_buf_simple_reset(buf);

    return buf;
}

static void reset_adv_link(struct bt_mesh_prov_link *link, uint8_t reason)
{
    bt_mesh_prov_clear_tx(link, true);

    if (bt_mesh_prov_get()->prov_link_close) {
        bt_mesh_prov_get()->prov_link_close(BLE_MESH_PROV_ADV, reason);
    }

    prov_memory_free(link);

    k_delayed_work_cancel(&link->prot_timer);

#if CONFIG_BLE_MESH_PB_GATT
    if (link->conn) {
        bt_mesh_conn_unref(link->conn);
    }
#endif

#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    /* Remove the link id from exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID,
                                    &link->link_id);
#endif

    /* Clear everything except the retransmit delayed work config */
    memset(link, 0, offsetof(struct bt_mesh_prov_link, tx.retransmit));

    link->pending_ack = PROV_XACT_NVAL;

    link->rx.prev_id = PROV_XACT_NVAL;
    link->rx.buf = get_rx_buf(link - prov_links);

    link->next_xact_id = pvnr_next_xact_id;
    link->reset_adv_link = reset_adv_link;
    link->retrans_timeout = close_link;

#if CONFIG_BLE_MESH_FAST_PROV
    link->last_tx_pdu = PROV_DATA;
#endif /* CONFIG_BLE_MESH_FAST_PROV */

    if (prov_ctx.pba_count) {
        prov_ctx.pba_count--;
    }
}

static void send_link_open(struct bt_mesh_prov_link *link)
{
    uint8_t count;
    int i;

    link->link_id = 0;

    while (1) {
        count = 0;

        /* Make sure the generated Link ID is not 0 */
        while(link->link_id == 0) {
            bt_mesh_rand(&link->link_id, sizeof(link->link_id));
            if (count++ > 10) {
                BT_ERR("Link ID error: all zero");
                return;
            }
        }

        /* Check if the generated Link ID is the same with other links */
        for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
            if (bt_mesh_atomic_test_bit(prov_links[i].flags, LINK_ACTIVE) &&
                prov_links[i].link_id == link->link_id) {
                bt_mesh_rand(&link->link_id, sizeof(link->link_id));
                break;
            }
        }

        if (i == CONFIG_BLE_MESH_PBA_SAME_TIME) {
            break;
        }
    }

#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    /* Add the link id into exceptional list */
    bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_ADD,
                                    BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID,
                                    &link->link_id);
#endif

    bt_mesh_prov_bearer_ctl_send(link, LINK_OPEN, link->uuid, 16);

    /* Set LINK_ACTIVE just to be in compatibility with current Zephyr code */
    bt_mesh_atomic_set_bit(link->flags, LINK_ACTIVE);

    if (bt_mesh_prov_get()->prov_link_open) {
        bt_mesh_prov_get()->prov_link_open(BLE_MESH_PROV_ADV);
    }
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
static int prov_send_gatt(struct bt_mesh_prov_link *link, struct net_buf_simple *msg)
{
    int err = 0;

    if (link->conn == NULL) {
        BT_ERR("PB-GATT send, not connected");
        return -ENOTCONN;
    }

    err = bt_mesh_proxy_client_send(link->conn, BLE_MESH_PROXY_PROV, msg);
    if (err) {
        BT_ERR("Failed to send PB-GATT pdu");
        return err;
    }

    k_delayed_work_submit(&link->prot_timer, PROTOCOL_TIMEOUT);

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

static void prov_invite(struct bt_mesh_prov_link *link,
                        struct net_buf_simple *buf)
{}

static void prov_start(struct bt_mesh_prov_link *link,
                       struct net_buf_simple *buf)
{}

static void prov_data(struct bt_mesh_prov_link *link,
                      struct net_buf_simple *buf)
{}

static void send_invite(struct bt_mesh_prov_link *link)
{
    PROV_BUF(buf, 2);

    bt_mesh_prov_buf_init(&buf, PROV_INVITE);

    net_buf_simple_add_u8(&buf, bt_mesh_prov_get()->prov_attention);

    link->conf_inputs[0] = bt_mesh_prov_get()->prov_attention;

    if (bt_mesh_prov_send(link, &buf)) {
        BT_ERR("Failed to send Provisioning Invite");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    link->expect = PROV_CAPABILITIES;
}

static void prov_capabilities(struct bt_mesh_prov_link *link,
                              struct net_buf_simple *buf)
{
    uint16_t output_action = 0U, input_action = 0U;
    uint8_t  output_size = 0U, input_size = 0U;
    uint8_t  oob_type = 0U;
    uint8_t  pub_key_type = 0U;
    uint16_t algorithms = 0U;
    uint8_t *data = buf->data;
    PROV_BUF(prov_buf, 6);

    link->element_num = net_buf_simple_pull_u8(buf);
    BT_INFO("Elements:          0x%02x", link->element_num);
    if (link->element_num == 0) {
        BT_ERR("Invalid element number %d", link->element_num);
        goto fail;
    }

    algorithms = net_buf_simple_pull_be16(buf);
    BT_INFO("Algorithms:        0x%04x", algorithms);
    if (!(algorithms & BIT(PROV_ALG_P256_CMAC_AES128))
#if CONFIG_BLE_MESH_PROV_EPA
    && !(algorithms & BIT(PROV_ALG_P256_HMAC_SHA256))
#endif
        ) {
        BT_ERR("Invalid algorithms 0x%04x", algorithms);
        goto fail;
    }

    pub_key_type = net_buf_simple_pull_u8(buf);
    BT_INFO("Public Key Type:   0x%02x", pub_key_type);
    if (pub_key_type > PROV_OOB_PUB_KEY) {
        BT_ERR("Invalid public key type 0x%02x", pub_key_type);
        goto fail;
    }

    link->public_key = ((bt_mesh_prov_get()->prov_pub_key_oob &&
                         bt_mesh_prov_get()->prov_pub_key_oob_cb) ?
                        pub_key_type : PROV_NO_OOB_PUB_KEY);

    oob_type = net_buf_simple_pull_u8(buf);
    BT_INFO("OOB Type:   0x%02x", oob_type);

    output_size = net_buf_simple_pull_u8(buf);
    BT_INFO("Output OOB Size:   0x%02x", output_size);
    if (output_size > 0x08) {
        BT_ERR("Invalid Output OOB size %d", output_size);
        goto fail;
    }

    output_action = net_buf_simple_pull_be16(buf);
    BT_INFO("Output OOB Action: 0x%04x", output_action);
    if (output_action > 0x1f) {
        BT_ERR("Invalid Output OOB action 0x%04x", output_action);
        goto fail;
    }

    /* Provisioner select output action */
    if (bt_mesh_prov_get()->prov_input_num && output_size) {
        output_action = __builtin_ctz(output_action);
    } else {
        output_size = 0x0;
        output_action = 0x0;
    }

    input_size = net_buf_simple_pull_u8(buf);
    BT_INFO("Input OOB Size:    0x%02x", input_size);
    if (input_size > 0x08) {
        BT_ERR("Invalid Input OOB size %d", input_size);
        goto fail;
    }

    input_action = net_buf_simple_pull_be16(buf);
    BT_INFO("Input OOB Action:  0x%04x", input_action);
    if (input_action > 0x0f) {
        BT_ERR("Invalid Input OOB action 0x%04x", input_action);
        goto fail;
    }

    /* NOTE: If bit 1 of the OOB Type field is set to 1, bit 0 of
     * the Algorithms field shall be set to 0 (case_1)and at least one of
     * the conditions listed below shall be met:
     *   • Bit 0 of the OOB Type field is set to 1.(case_2)
     *   • The Output OOB Size field is not equal to 0x00.(case_3)
     *   • The Input OOB Size field is not equal to 0x00.(case_4)
     * That means:
     *  !(case_1 && (case_2 || case_3 || case_4)) goto fail
     * equals:
     *  !case1 || !(case_2 || case_3 || case_4) goto fail
     */
    if (oob_type & BIT(PROV_ONLY_OOB_AUTH_SUPPORT)) {
        if ((algorithms & BIT(PROV_ALG_P256_CMAC_AES128)) ||
            (!((oob_type & BIT(PROV_STATIC_OOB_AVAILABLE)) == 0x00 ||
               output_size == 0x00 || input_size == 0x00))) {
            goto fail;
        }
    }

    /* Provisioner select input action */
    if (bt_mesh_prov_get()->prov_output_num && input_size) {
        input_action = __builtin_ctz(input_action);
    } else {
        input_size = 0x0;
        input_action = 0x0;
    }

    if (oob_type & BIT(PROV_STATIC_OOB_AVAILABLE)) {
        /* if static oob is valid, just use static oob */
        link->auth_method = AUTH_METHOD_STATIC;
        link->auth_action = 0x00;
        link->auth_size   = 0x00;
    } else {
        if (!output_size && !input_size) {
            link->auth_method = AUTH_METHOD_NO_OOB;
            link->auth_action = 0x00;
            link->auth_size   = 0x00;
        } else if (!output_size && input_size) {
            link->auth_method = AUTH_METHOD_INPUT;
            link->auth_action = (uint8_t)input_action;
            link->auth_size   = input_size;
        } else {
            link->auth_method = AUTH_METHOD_OUTPUT;
            link->auth_action = (uint8_t)output_action;
            link->auth_size   = output_size;
        }
    }

    /* Store provisioning capabilities value in conf_inputs */
    memcpy(&link->conf_inputs[1], data, 11);

    bt_mesh_prov_buf_init(&prov_buf, PROV_START);

#if CONFIG_BLE_MESH_PROV_EPA
    if (algorithms & BIT(PROV_ALG_P256_HMAC_SHA256)) {
        net_buf_simple_add_u8(&prov_buf, PROV_ALG_P256_HMAC_SHA256);
        link->algorithm = PROV_ALG_P256_HMAC_SHA256;
    } else {
        net_buf_simple_add_u8(&prov_buf, PROV_ALG_P256_CMAC_AES128);
        link->algorithm = PROV_ALG_P256_CMAC_AES128;
    }
#else
    net_buf_simple_add_u8(&prov_buf, PROV_ALG_P256_CMAC_AES128);
    link->algorithm = PROV_ALG_P256_CMAC_AES128;
#endif

    net_buf_simple_add_u8(&prov_buf, link->public_key);
    net_buf_simple_add_u8(&prov_buf, link->auth_method);
    net_buf_simple_add_u8(&prov_buf, link->auth_action);
    net_buf_simple_add_u8(&prov_buf, link->auth_size);

    memcpy(&link->conf_inputs[12], &prov_buf.data[1], 5);

    if (bt_mesh_prov_send(link, &prov_buf)) {
        BT_ERR("Failed to send Provisioning Start");
        goto fail;
    }

    /* After prov start is sent, use OOB to get remote public key.
     * And we just follow the procedure in Figure 5.15 of Section
     * 5.4.2.3 of Mesh Profile Spec.
     */
    if (link->public_key == PROV_OOB_PUB_KEY &&
        bt_mesh_prov_get()->prov_pub_key_oob_cb(link - prov_links)) {
        BT_ERR("Failed to notify input OOB Public Key");
        goto fail;
    }

    /* If the link is used by PB-Remote, return here. Need to
     * wait for Remote Provisioning PDU Outbound Report, then
     * send Remote Provisioning PDU Send with Public Key.
     */
    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
        return;
    }

#if CONFIG_BLE_MESH_PB_ADV
    /* If the link is used by PB-ADV, need to wait for the
     * Transaction ACK for Provisioning Start. When ACK is
     * received, we can send Provisioning Public Key.
     */
    if (link - prov_links < CONFIG_BLE_MESH_PBA_SAME_TIME) {
        link->expect_ack_for = PROV_START;
        return;
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

    send_pub_key(link);
    return;

fail:
    close_link(link, CLOSE_REASON_FAILED);
}

static int prov_auth(struct bt_mesh_prov_link *link,
                     uint8_t method, uint8_t action,
                     uint8_t size)
{
    bt_mesh_output_action_t output = 0U;
    bt_mesh_input_action_t input = 0U;
    uint8_t auth_size = PROV_AUTH_SIZE(link);

    switch (method) {
    case AUTH_METHOD_NO_OOB:
        if (action || size) {
            return -EINVAL;
        }

        memset(link->auth, 0, PROV_AUTH_MAX_SIZE);
        return 0;

    case AUTH_METHOD_STATIC:
        if (action || size) {
            return -EINVAL;
        }

        if (prov_ctx.static_oob_len > auth_size) {
            memcpy(link->auth, prov_ctx.static_oob_val, auth_size);
        } else {
            memcpy(link->auth + auth_size - prov_ctx.static_oob_len,
                   prov_ctx.static_oob_val, prov_ctx.static_oob_len);
            memset(link->auth, 0, auth_size - prov_ctx.static_oob_len);
        }

        return 0;

    case AUTH_METHOD_OUTPUT:
        /* Use auth_action to get device output action */
        output = bt_mesh_prov_output_action(action);
        if (!output) {
            return -EINVAL;
        }

        return bt_mesh_prov_get()->prov_input_num(AUTH_METHOD_OUTPUT, output,
                                                  size, link - prov_links);

    case AUTH_METHOD_INPUT:
        /* Use auth_action to get device input action */
        input = bt_mesh_prov_input_action(action);
        if (!input) {
            return -EINVAL;
        }

        /* Provisioner ouput number/string and wait for device's Provisioning Input Complete PDU */
        link->expect = PROV_INPUT_COMPLETE;

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

            memcpy(link->auth, str, size);
            memset(link->auth + size, 0, auth_size - size);

            return bt_mesh_prov_get()->prov_output_num(AUTH_METHOD_INPUT, input, str,
                                                       size, link - prov_links);
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

            sys_put_be32(num, &link->auth[auth_size - 4]);
            memset(link->auth, 0, auth_size - 4);

            return bt_mesh_prov_get()->prov_output_num(AUTH_METHOD_INPUT, input, &num,
                                                       size, link - prov_links);
        }

    default:
        return -EINVAL;
    }
}

static void send_confirm(struct bt_mesh_prov_link *link)
{
    uint8_t *conf = NULL;
    uint8_t conf_salt_size = 0;
    uint8_t conf_key_size = 0;
    uint8_t rand_size = 0;
    uint8_t conf_size = 0;

    conf_salt_size = PROV_CONF_SALT_SIZE(link);
    conf_key_size = PROV_CONF_KEY_SIZE(link);
    rand_size = PROV_RAND_SIZE(link);
    conf_size = PROV_CONF_SIZE(link);

    PROV_BUF(buf, (conf_size + 1));

    if (!bt_mesh_atomic_test_bit(link->flags, HAVE_DHKEY)) {
        BT_WARN("Wait for generating DHKey");
        return;
    }

    BT_DBG("ConfInputs[0]   %s", bt_hex(link->conf_inputs, 64));
    BT_DBG("ConfInputs[64]  %s", bt_hex(link->conf_inputs + 64, 64));
    BT_DBG("ConfInputs[128] %s", bt_hex(link->conf_inputs + 128, 17));

    if (link->algorithm == PROV_ALG_P256_CMAC_AES128) {
        if (bt_mesh_prov_conf_salt(link->conf_inputs, link->conf_salt)) {
            BT_ERR("Failed to generate confirmation salt");
            goto fail;
        }

        if (bt_mesh_prov_conf_key(link->dhkey, link->conf_salt, link->conf_key)) {
            BT_ERR("Failed to generate confirmation key");
            goto fail;
        }
    }
#if CONFIG_BLE_MESH_PROV_EPA
      else {
        if (bt_mesh_prov_conf_salt_epa(link->conf_inputs, link->conf_salt)) {
            BT_ERR("Failed to generate confirmation salt");
            goto fail;
        }

        if (bt_mesh_prov_conf_key_epa(link->dhkey, link->auth, link->conf_salt, link->conf_key)) {
            BT_ERR("Failed to generate confirmation key");
            goto fail;
        }
    }
#endif

    if (bt_mesh_rand(link->rand, rand_size)) {
        BT_ERR("Failed to generate random number");
        goto fail;
    }

    BT_DBG("ConfirmationSalt: %s", bt_hex(link->conf_salt, conf_salt_size));
    BT_DBG("ConfirmationKey: %s", bt_hex(link->conf_key, conf_key_size));
    BT_DBG("LocalRandom: %s", bt_hex(link->rand, rand_size));

    bt_mesh_prov_buf_init(&buf, PROV_CONFIRM);

    conf = net_buf_simple_add(&buf, conf_size);

    if (link->algorithm == PROV_ALG_P256_CMAC_AES128) {
        if (bt_mesh_prov_conf(link->conf_key, link->rand, link->auth, conf)) {
            BT_ERR("Failed to generate confirmation value");
            goto fail;
        }
    }
#if CONFIG_BLE_MESH_PROV_EPA
      else {
        if (bt_mesh_prov_conf_epa(link->conf_key, link->rand, conf)) {
            BT_ERR("Failed to generate confirmation value");
            goto fail;
        }
    }
#endif

    memcpy(link->local_conf, conf, conf_size);

    if (bt_mesh_prov_send(link, &buf)) {
        BT_ERR("Failed to send Provisioning Confirm");
        goto fail;
    }

    link->expect = PROV_CONFIRM;
    return;

fail:
    close_link(link, CLOSE_REASON_FAILED);
}

int bt_mesh_provisioner_set_oob_input_data(const uint8_t idx, const uint8_t *val, bool num_flag)
{
    /* This function should be called in the prov_input_num callback,
     * after the data output by device has been input by Provisioner.
     *
     * "val" is the auth value, for example, if device output data is
     * 12345678(decimal), the data in auth value will be 0xBC614E.
     *
     * "num_flag" is used to indicate whether the value input by
     * Provisioner is number or string.
     */
    struct bt_mesh_prov_link *link = NULL;
    uint8_t auth_size = 0;

    if (idx >= ARRAY_SIZE(prov_links) || val == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    link = &prov_links[idx];
    auth_size = PROV_AUTH_SIZE(link);

    if (!bt_mesh_atomic_test_bit(link->flags, LINK_ACTIVE) ||
        link->auth_method != AUTH_METHOD_OUTPUT) {
        BT_ERR("Not ready for Output OOB, link idx %d", idx);
        return -EIO;
    }

    memset(link->auth, 0, PROV_AUTH_MAX_SIZE);

    if (num_flag) {
        /* Provisioner inputs number */
        sys_memcpy_swap(link->auth + auth_size - 4, val, sizeof(uint32_t));
    } else {
        /* Provisioner inputs string */
        memcpy(link->auth, val, link->auth_size);
    }

    BT_INFO("Output OOB, idx %d, type %s, auth %s", idx,
            num_flag ? "number" : "string", bt_hex(link->auth, auth_size));

    /* If the link is used by PB-Remote, and we are
     * waiting for Remote Provisioning PDU Outbound
     * Report for Public Key, return here.
     */
    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
        if (bt_mesh_atomic_test_bit(link->flags, WAIT_PK_OBR)) {
            BT_INFO("PB-Remote, wait for outbound report for public key");
            /* Use SEND_CONFIRM flag to indicate that Remote
             * Provisioning PDU Send with confirmation could
             * be sent when the Remote Provisioning Outbound
             * Report for Public Key is received.
             */
            bt_mesh_atomic_set_bit(link->flags, SEND_CONFIRM);
            return 0;
        }
    }

    send_confirm(link);

    return 0;
}

int bt_mesh_provisioner_set_oob_output_data(const uint8_t idx, const uint8_t *num,
                                            uint8_t size, bool num_flag)
{
    /* This function should be called in the prov_output_num callback,
     * after the data has been output by Provisioner.
     *
     * "size" is used to indicate the length of "num", for example, if
     * Provisioner output data is 12345678(decimal), the data in auth
     * value will be 0xBC614E.
     *
     * "num_flag" is used to indicate whether the value output by
     * Provisioner is number or string.
     */
    struct bt_mesh_prov_link *link = NULL;
    uint8_t auth_size = 0;

    if (idx >= ARRAY_SIZE(prov_links) || num == NULL ||
        size > BLE_MESH_PROV_INPUT_OOB_MAX_LEN) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    link = &prov_links[idx];
    auth_size = PROV_AUTH_SIZE(link);

    if (!bt_mesh_atomic_test_bit(link->flags, LINK_ACTIVE) ||
        link->auth_method != AUTH_METHOD_INPUT) {
        BT_ERR("Not ready for Input OOB, link idx %d", idx);
        return -EIO;
    }

    memset(link->auth, 0, PROV_AUTH_MAX_SIZE);

    if (num_flag) {
        /* Provisioner output number */
        sys_memcpy_swap(link->auth + auth_size - size, num, size);
    } else {
        /* Provisioner output string */
        memcpy(link->auth, num, size);
    }

    BT_INFO("Input OOB, idx %d, type %s, auth %s", idx,
            num_flag ? "number" : "string", bt_hex(link->auth, auth_size));

    link->expect = PROV_INPUT_COMPLETE;

    return 0;
}

int bt_mesh_provisioner_read_oob_pub_key(const uint8_t idx, const uint8_t pub_key_x[32],
                                         const uint8_t pub_key_y[32])
{
    struct bt_mesh_prov_link *link = NULL;

    if (idx >= ARRAY_SIZE(prov_links) || pub_key_x == NULL || pub_key_y == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    link = &prov_links[idx];

    if (!bt_mesh_atomic_test_bit(link->flags, LINK_ACTIVE) ||
        link->public_key != PROV_OOB_PUB_KEY) {
        BT_ERR("Not ready for OOB Public Key, link idx %d", idx);
        return -EIO;
    }

    BT_INFO("OOB Public Key, idx %d, x %s, y %s", idx,
            bt_hex(pub_key_x, 32), bt_hex(pub_key_y, 32));

    /* Swap X and Y halves independently to big-endian */
    sys_memcpy_swap(&link->conf_inputs[81], pub_key_x, 32);
    sys_memcpy_swap(&link->conf_inputs[81] + 32, pub_key_y, 32);

    bt_mesh_atomic_set_bit(link->flags, REMOTE_PUB_KEY);

    if (bt_mesh_atomic_test_and_clear_bit(link->flags, WAIT_GEN_DHKEY)) {
        prov_gen_dh_key(link);
    }

    return 0;
}

static void prov_gen_dh_key(struct bt_mesh_prov_link *link)
{
    uint8_t pub_key[64] = {0};
    uint8_t dhkey[32] = {0};

    /* Copy public key in little-endian for generating DHKey.
     * X and Y halves are swapped independently.
     */
    sys_memcpy_swap(&pub_key[0], &link->conf_inputs[81], 32);
    sys_memcpy_swap(&pub_key[32], &link->conf_inputs[113], 32);

    if (bt_mesh_dh_key_gen(pub_key, dhkey)) {
        BT_ERR("Failed to generate DHKey");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    sys_memcpy_swap(link->dhkey, dhkey, 32);

    BT_DBG("DHKey: %s", bt_hex(link->dhkey, 32));

    bt_mesh_atomic_set_bit(link->flags, HAVE_DHKEY);

    /* After DHKey is generated, if auth_method is No OOB or
     * Static OOB, Provisioner can start to send confirmation.
     *
     * If output OOB is used by the device, Provisioner needs
     * to watch out the output number and input it as auth_val.
     *
     * If input OOB is used by the device, Provisioner needs
     * to output a value, and wait for prov input complete pdu.
     */
    if (prov_auth(link, link->auth_method,
                  link->auth_action, link->auth_size)) {
        BT_ERR("Failed to authenticate");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    /* If authentication method is Output OOB, wait for the
     * authentication value;
     * If authentication method is Input OOB, wait for the
     * Provisioning Input Complete.
     */
    if (link->auth_method == AUTH_METHOD_OUTPUT ||
        link->auth_method == AUTH_METHOD_INPUT) {
        return;
    }

    /* This function will be invoked when the Device Public
     * Key is received.
     * If the link is used by PB-Remote, we can decide the
     * next move depends on the Public Key type.
     */
    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
        if (bt_mesh_atomic_test_bit(link->flags, WAIT_PK_OBR)) {
            BT_INFO("PB-Remote, wait for outbound report for public key");
            /* Use SEND_CONFIRM flag to indicate that Remote
             * Provisioning PDU Send with confirmation could
             * be sent when the Remote Provisioning Outbound
             * Report for Public Key is received.
             */
            bt_mesh_atomic_set_bit(link->flags, SEND_CONFIRM);
            return;
        }
    }

    /* If the link is not used by PB-Remote, send confirmation here.
     * If the link is used by PB-Remote:
     * a) If No OOB Public Key is used, which means Device Public
     *    Key is received, and WAIT_PK_OBR will not be set, send
     *    Remote Provisioning PDU Send with confirmation here.
     * b) If OOB Public Key is used, which means Device Public Key
     *    has been input through OOB, and WAIT_PK_OBR is cleared
     *    (i.e. Remote Provisioning Outbound Report for Public Key
     *    is received), send Remote Provisioning PDU Send with
     *    confirmation here.
     * Note:
     * No need to check if the next expect pdu is Provisioning
     * Input Complete, because if the authentication method is
     * Output OOB or Input OOB, it will directly return above.
     */
    if (link->expect != PROV_INPUT_COMPLETE) {
        send_confirm(link);
    }
}

static void send_pub_key(struct bt_mesh_prov_link *link)
{
    const uint8_t *key = NULL;
    PROV_BUF(buf, 65);

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("No public key available");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    BT_DBG("Local Public Key: %s", bt_hex(key, 64));

    bt_mesh_prov_buf_init(&buf, PROV_PUB_KEY);

    /* Swap X and Y halves independently to big-endian */
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), key, 32);
    sys_memcpy_swap(net_buf_simple_add(&buf, 32), &key[32], 32);

    /* Store provisioner public key value in conf_inputs */
    memcpy(&link->conf_inputs[17], &buf.data[1], 64);

    if (bt_mesh_prov_send(link, &buf)) {
        BT_ERR("Failed to send Provisioning Public Key");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    if (link->public_key == PROV_NO_OOB_PUB_KEY) {
        link->expect = PROV_PUB_KEY;
        return;
    }

#if CONFIG_BLE_MESH_PB_ADV
    /* If the link is used by PB-ADV and OOB Public Key is
     * used, need to wait for the Transaction ACK for Public
     * Key. When ACK is received, we can start to generate
     * DHKey and send Provisioning Confirm.
     */
    if (!bt_mesh_atomic_test_bit(link->flags, PB_REMOTE) &&
        (link - prov_links < CONFIG_BLE_MESH_PBA_SAME_TIME)) {
        link->expect_ack_for = PROV_PUB_KEY;
        return;
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
        /* If OOB Public Key is used, try to generate DHKey,
         * and we need to wait for the Remote Provisioning
         * Outbound Report for Public Key, then we can send
         * Remote Provisioning PDU Send with confirmation.
         */
        bt_mesh_atomic_set_bit(link->flags, WAIT_PK_OBR);
    }

    /* If OOB Public Key has been read, start to generate
     * DHKey; otherwise need to wait for OOB Public Key.
     */
    if (bt_mesh_atomic_test_bit(link->flags, REMOTE_PUB_KEY)) {
        prov_gen_dh_key(link);
    } else {
        bt_mesh_atomic_set_bit(link->flags, WAIT_GEN_DHKEY);
    }
}

static void prov_pub_key(struct bt_mesh_prov_link *link,
                         struct net_buf_simple *buf)
{
    BT_DBG("Remote Public Key: %s", bt_hex(buf->data, 64));

    /* Check public key using the following rules:
     * 1. X > 0, Y > 0
     * 2. X > 0, Y = 0
     * 3. X = 0, Y = 0
     */
    if (!bt_mesh_check_public_key(buf->data)) {
        BT_ERR("Invalid public key");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    memcpy(&link->conf_inputs[81], buf->data, 64);

    prov_gen_dh_key(link);
}

static void prov_input_complete(struct bt_mesh_prov_link *link,
                                struct net_buf_simple *buf)
{
    /* TODO:
     * Add a flag to make sure authentication value has been set
     * when using Input OOB.
     *
     * Currently the authentication value is generated and set
     * internally, so this should works fine at this moment.
     */

    /* Provisioner receives input complete and send confirm */
    send_confirm(link);
}

static void prov_confirm(struct bt_mesh_prov_link *link,
                         struct net_buf_simple *buf)
{
    uint8_t conf_size = PROV_CONF_SIZE(link);

    PROV_BUF(prov_buf, conf_size + 1);

    BT_DBG("Remote Confirm: %s", bt_hex(buf->data, conf_size));

    /* NOTE: The Bluetooth SIG recommends that potentially vulnerable mesh provisioners
     * restrict the authentication procedure and not accept provisioning random and
     * provisioning confirmation numbers from a remote peer that are the same as those
     * selected by the local device (CVE-2020-26560).
     */
    if (!memcmp(buf->data, link->local_conf, conf_size)) {
        BT_ERR("Confirmation value is identical to ours, rejecting.");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    memcpy(link->conf, buf->data, conf_size);

    if (!bt_mesh_atomic_test_bit(link->flags, HAVE_DHKEY)) {
#if CONFIG_BLE_MESH_PB_ADV
        bt_mesh_prov_clear_tx(link, true);
#endif
        bt_mesh_atomic_set_bit(link->flags, SEND_CONFIRM);
    }

    bt_mesh_prov_buf_init(&prov_buf, PROV_RANDOM);

    net_buf_simple_add_mem(&prov_buf, link->rand, PROV_RAND_SIZE(link));

    if (bt_mesh_prov_send(link, &prov_buf)) {
        BT_ERR("Failed to send Provisioning Random");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    link->expect = PROV_RANDOM;
}

static void send_prov_data(struct bt_mesh_prov_link *link)
{
    uint16_t prev_addr = BLE_MESH_ADDR_UNASSIGNED;
    uint16_t max_addr = BLE_MESH_ADDR_UNASSIGNED;
    struct bt_mesh_node *node = NULL;
    const uint8_t *netkey = NULL;
    uint8_t session_key[16] = {0};
    uint8_t nonce[13] = {0};
    uint8_t pdu[25] = {0};
    PROV_BUF(buf, 34);
    int err = 0;

    err = bt_mesh_session_key(link->dhkey, link->prov_salt, session_key);
    if (err) {
        BT_ERR("Failed to generate session key");
        goto fail;
    }

    BT_DBG("SessionKey: %s", bt_hex(session_key, 16));

    err = bt_mesh_prov_nonce(link->dhkey, link->prov_salt, nonce);
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
        pdu[18] = get_net_flags(prov_ctx.fast_prov.net_idx);
        sys_put_be32(bt_mesh.iv_index, &pdu[19]);
    } else {
        netkey = bt_mesh_provisioner_net_key_get(prov_ctx.net_idx);
        if (!netkey) {
            BT_ERR("No NetKey for provisioning data");
            goto fail;
        }

        memcpy(pdu, netkey, 16);
        sys_put_be16(prov_ctx.net_idx, &pdu[16]);
        pdu[18] = get_net_flags(prov_ctx.net_idx);
        sys_put_be32(bt_mesh.iv_index, &pdu[19]);
    }

    /* The Provisioner must not reuse unicast addresses that have been
     * allocated to a device and sent in a Provisioning Data PDU until
     * the Provisioner receives an Unprovisioned Device beacon or
     * Service Data for the Mesh Provisioning Service from that same
     * device, identified using the Device UUID of the device.
     */

    /* Check if this device is a re-provisioned device */
    node = bt_mesh_provisioner_get_node_with_uuid(link->uuid);
    if (node) {
        if (link->element_num <= node->element_num &&
            link->pb_remote_nppi != NPPI_NODE_ADDR_REFRESH) {
            /* If the device is provisioned before, but the element number of
             * the device is bigger now, then we treat it as a new device.
             */
            prev_addr = node->unicast_addr;
        }

        /* If the link is a normal provisioning link, or it is a PB-Remote one
         * and not used by NPPI, then we could remove the device.
         */
        if (!bt_mesh_atomic_test_bit(link->flags, PB_REMOTE) ||
            link->pb_remote_nppi == NPPI_UNKNOWN) {
            bt_mesh_provisioner_remove_node(link->uuid);
        }
    }

    max_addr = FAST_PROV_ENABLE() ? prov_ctx.fast_prov.unicast_addr_max : UNICAST_ADDR_LIMIT;

    if (BLE_MESH_ADDR_IS_UNICAST(prev_addr)) {
        /* For Node Address Refresh, the previous node address
         * shall not be used.
         */
        sys_put_be16(prev_addr, &pdu[23]);
        link->unicast_addr = prev_addr;
    } else {
        uint16_t alloc_addr = BLE_MESH_ADDR_UNASSIGNED;

        if (BLE_MESH_ADDR_IS_UNICAST(link->assign_addr)) {
            alloc_addr = link->assign_addr;
        } else {
            /* If this device to be provisioned is a new device */
            if (prov_ctx.alloc_addr == BLE_MESH_ADDR_UNASSIGNED) {
                BT_ERR("Not enough unicast address to be allocated");
                goto fail;
            }

            alloc_addr = prov_ctx.alloc_addr;
        }

        if (alloc_addr + link->element_num - 1 > max_addr) {
            BT_ERR("Not enough unicast address for the device");
            goto fail;
        }

        /* Make sure the assigned unicast address is not identical with any unicast
         * address of other nodes. And make sure the address is not identical with
         * any unicast address of Provisioner.
         */
        if (bt_mesh_provisioner_check_is_addr_dup(alloc_addr, link->element_num, true)) {
            BT_ERR("Duplicate assigned address 0x%04x", alloc_addr);
            goto fail;
        }

        sys_put_be16(alloc_addr, &pdu[23]);
        link->unicast_addr = alloc_addr;
    }

    bt_mesh_prov_buf_init(&buf, PROV_DATA);

    err = bt_mesh_prov_encrypt(session_key, nonce, pdu, net_buf_simple_add(&buf, 33));
    if (err) {
        BT_ERR("Failed to encrypt provisioning data");
        goto fail;
    }

    if (bt_mesh_prov_send(link, &buf)) {
        BT_ERR("Failed to send Provisioning Data");
        goto fail;
    }

    /* We update the next unicast address to be allocated here because if
     * Provisioner is provisioning two devices at the same time, we need
     * to assign the unicast address for them correctly. Hence we should
     * not update the prov_ctx.alloc_addr after the proper provisioning
     * complete pdu is received.
     */
    if (!BLE_MESH_ADDR_IS_UNICAST(prev_addr)) {
        if (BLE_MESH_ADDR_IS_UNICAST(link->assign_addr)) {
            /* Even if the unicast address of the node is assigned by the
             * application, we will also update the prov_ctx.alloc_addr
             * here, in case Users use the two methods together (i.e. allocate
             * the unicast address for the node internally and assign the
             * unicast address for the node from application).
             */
            if (prov_ctx.alloc_addr < link->assign_addr + link->element_num) {
                prov_ctx.alloc_addr = link->assign_addr + link->element_num;
            }
        } else {
            prov_ctx.alloc_addr += link->element_num;
            if (prov_ctx.alloc_addr > max_addr) {
                /* No unicast address will be used for further provisioning */
                prov_ctx.alloc_addr = BLE_MESH_ADDR_UNASSIGNED;
            }
        }

        /* Store the available unicast address range to flash */
        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_prov_info(prov_ctx.primary_addr, prov_ctx.alloc_addr);
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && FAST_PROV_ENABLE()) {
        link->kri_flags = get_net_flags(prov_ctx.fast_prov.net_idx);
    } else {
        link->kri_flags = get_net_flags(prov_ctx.net_idx);
    }

    link->expect = PROV_COMPLETE;
    return;

fail:
    close_link(link, CLOSE_REASON_FAILED);
}

static void prov_random(struct bt_mesh_prov_link *link,
                        struct net_buf_simple *buf)
{
    uint8_t conf_verify[32] = {0};
    uint8_t rand_size = 0;
    uint8_t *data = buf->data;

    rand_size = PROV_RAND_SIZE(link);

    BT_DBG("Remote Random: %s", bt_hex(data, rand_size));

    /* NOTE: The Bluetooth SIG recommends that potentially vulnerable mesh provisioners
     * restrict the authentication procedure and not accept provisioning random and
     * provisioning confirmation numbers from a remote peer that are the same as those
     * selected by the local device (CVE-2020-26560).
     */
    if (!memcmp(data, link->rand, rand_size)) {
        BT_ERR("Random value is identical to ours, rejecting.");
        goto fail;
    }

    if (link->algorithm == PROV_ALG_P256_CMAC_AES128) {
        if (bt_mesh_prov_conf(link->conf_key, data, link->auth, conf_verify)) {
            BT_ERR("Failed to calculate confirmation verification");
            goto fail;
        }
    }
#if CONFIG_BLE_MESH_PROV_EPA
      else {
        if (bt_mesh_prov_conf_epa(link->conf_key, data, conf_verify)) {
            BT_ERR("Failed to calculate confirmation verification");
            goto fail;
        }
    }
#endif

    if (memcmp(conf_verify, link->conf, PROV_CONF_SIZE(link))) {
        BT_ERR("Invalid confirmation value");
        BT_ERR("Received:   %s", bt_hex(link->conf, PROV_CONF_SIZE(link)));
        BT_ERR("Calculated: %s",  bt_hex(conf_verify, PROV_CONF_SIZE(link)));
        goto fail;
    }

    /* After Provisioner received Provisioning Random PDU from
     * the device, and successfully checked the confirmation,
     * the following needs to be done:
     * - Calculate prov_salt
     * - Prepare and send Provisioning Data PDU
     */
    if (link->algorithm == PROV_ALG_P256_CMAC_AES128) {
        if (bt_mesh_prov_salt(link->conf_salt, link->rand, data,
                              link->prov_salt)) {
            BT_ERR("Failed to generate ProvisioningSalt");
            goto fail;
        }
    }
#if CONFIG_BLE_MESH_PROV_EPA
      else {
        if (bt_mesh_prov_salt_epa(link->conf_salt, link->rand, data,
                                  link->prov_salt)) {
            BT_ERR("Failed to generate ProvisioningSalt");
            goto fail;
        }
    }
#endif

    BT_DBG("ProvisioningSalt: %s", bt_hex(link->prov_salt, PROV_CONF_SALT_SIZE(link)));

    send_prov_data(link);
    return;

fail:
    close_link(link, CLOSE_REASON_FAILED);
}

static void prov_complete(struct bt_mesh_prov_link *link,
                          struct net_buf_simple *buf)
{
    uint8_t device_key[16] = {0};
    uint16_t net_idx = 0U;
    uint16_t index = 0U;
    bool nppi = false;
    int err = 0;
    int i;

    err = bt_mesh_dev_key(link->dhkey, link->prov_salt, device_key);
    if (err) {
        BT_ERR("Failed to generate device key");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FAST_PROV) && FAST_PROV_ENABLE()) {
        net_idx = prov_ctx.fast_prov.net_idx;
    } else {
        net_idx = prov_ctx.net_idx;
    }

    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE) &&
        link->pb_remote_nppi != NPPI_UNKNOWN) {
        nppi = true;
    }

    err = bt_mesh_provisioner_provision(&link->addr, link->uuid, link->oob_info,
                                        link->unicast_addr, link->element_num,
                                        net_idx, link->kri_flags, bt_mesh.iv_index,
                                        device_key, &index, nppi);
    if (err) {
        BT_ERR("Failed to store node info");
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

#if CONFIG_BLE_MESH_RPR_CLI
    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
        /* TODO:
         * If NPPI info is activated after RP Link Close with
         * a success reason is received, then this could be
         * removed.
         */
        if (link->pb_remote_nppi == NPPI_NODE_ADDR_REFRESH) {
            bt_mesh_rpr_cli_node_addr_update(link);
        }

        /* Notify to the application layer that provisioning
         * is completed successfully.
         */
        bt_mesh_rpr_cli_prov_complete(link, index, net_idx);

        close_link(link, CLOSE_REASON_SUCCESS);
        return;
    }
#endif /* CONFIG_BLE_MESH_RPR_CLI */

    if (bt_mesh_prov_get()->prov_complete) {
        bt_mesh_prov_get()->prov_complete(index, link->uuid, link->unicast_addr,
                                          link->element_num, net_idx);
    }

    /* Find if the device is in the device queue */
    for (i = 0; i < ARRAY_SIZE(unprov_dev); i++) {
        if (!memcmp(unprov_dev[i].uuid, link->uuid, 16) &&
            (unprov_dev[i].flags & RM_AFTER_PROV)) {
            memset(&unprov_dev[i], 0, sizeof(struct unprov_dev_queue));
            break;
        }
    }

    close_link(link, CLOSE_REASON_SUCCESS);
}

static void prov_failed(struct bt_mesh_prov_link *link,
                        struct net_buf_simple *buf)
{
    BT_WARN("Error 0x%02x", buf->data[0]);

    close_link(link, CLOSE_REASON_FAILED);
}

static const struct {
    void (*func)(struct bt_mesh_prov_link *link, struct net_buf_simple *buf);
} prov_handlers[] = {
    { prov_invite,               },
    { prov_capabilities,         },
    { prov_start,                },
    { prov_pub_key,              },
    { prov_input_complete,       },
    { prov_confirm,              },
    { prov_random,               },
    { prov_data,                 },
    { prov_complete,             },
    { prov_failed,               },
#if CONFIG_BLE_MESH_CERT_BASED_PROV
    { bt_mesh_pvnr_record_req,   },
    { bt_mesh_pvnr_record_rsp,   },
    { bt_mesh_pvnr_records_get,  },
    { bt_mesh_pvnr_records_list, },
#endif
};

static void close_link(struct bt_mesh_prov_link *link, uint8_t reason)
{
    if (bt_mesh_atomic_test_bit(link->flags, PB_REMOTE)) {
        if (link->pb_remote_close) {
            link->pb_remote_close(link, reason);
        }
        return;
    }

#if CONFIG_BLE_MESH_PB_GATT
    if (link->conn) {
        bt_mesh_gattc_disconnect(link->conn);
        return;
    }
#endif /* CONFIG_BLE_MESH_PB_GATT */

#if CONFIG_BLE_MESH_PB_ADV
    bt_mesh_prov_bearer_ctl_send(link, LINK_CLOSE, &reason, sizeof(reason));
#endif /* CONFIG_BLE_MESH_PB_ADV */
}

#if CONFIG_BLE_MESH_PB_ADV
static void link_ack(struct bt_mesh_prov_link *link, struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (buf->len) {
        BT_ERR("Invalid Link ACK length %d", buf->len);
        close_link(link, CLOSE_REASON_FAILED);
        return;
    }

    if (link->expect == PROV_CAPABILITIES ||
        link->expect == PROV_REC_LIST || link->expect == PROV_REC_RSP) {
        BT_INFO("Link ACK is already received");
        return;
    }

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    if (PROV_REC_SUPPORT(link->oob_info)) {
        if (bt_mesh_prov_get()->cert_based_prov_start) {
            bt_mesh_prov_get()->cert_based_prov_start(link - prov_links);
        }
    } else
#endif
    {
        send_invite(link);
    }
}

static void link_close(struct bt_mesh_prov_link *link, struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (buf->len != 1) {
        BT_ERR("Invalid Link Close length %d", buf->len);
        return;
    }

    reset_adv_link(link, net_buf_simple_pull_u8(buf));
}

static void gen_prov_ctl(struct bt_mesh_prov_link *link, struct prov_rx *rx, struct net_buf_simple *buf)
{
    BT_DBG("op 0x%02x len %u", BEARER_CTL(rx->gpc), buf->len);

    switch (BEARER_CTL(rx->gpc)) {
    case LINK_OPEN:
        break;

    case LINK_ACK:
        if (!bt_mesh_atomic_test_bit(link->flags, LINK_ACTIVE)) {
            return;
        }

        link_ack(link, rx, buf);
        break;

    case LINK_CLOSE:
        if (!bt_mesh_atomic_test_bit(link->flags, LINK_ACTIVE)) {
            return;
        }

        link_close(link, rx, buf);
        break;

    default:
        BT_ERR("Unknown bearer opcode 0x%02x", BEARER_CTL(rx->gpc));
        return;
    }
}

static void prov_msg_recv(struct bt_mesh_prov_link *link)
{
    uint8_t type = 0;

    /* When Link Close is being sent, any received Provisioning PDU
     * should be ignored.
     */
    if (bt_mesh_atomic_test_bit(link->flags, LINK_CLOSING)) {
        BT_WARN("Link is closing, unexpected msg 0x%02x", type);
        return;
    }

    if (!bt_mesh_fcs_check(link->rx.buf, link->rx.fcs)) {
        BT_ERR("Incorrect FCS");
        return;
    }

    type = net_buf_simple_pull_u8(link->rx.buf);

    BT_DBG("type 0x%02x len %u", type, link->rx.buf->len);

    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        goto fail;
    }

    bt_mesh_gen_prov_ack_send(link, link->rx.id);
    link->rx.prev_id = link->rx.id;
    link->rx.id = 0;

    /* Provisioner first checks information within the received
     * Provisioning PDU. If the check succeeds then check fcs.
     */
    if (type != PROV_FAILED && type != link->expect) {
        BT_ERR("Unexpected msg 0x%02x != 0x%02x", type, link->expect);
        goto fail;
    }

    if (!bt_mesh_prov_pdu_check(type, link->rx.buf->len, NULL)) {
        goto fail;
    }

    k_delayed_work_submit(&link->prot_timer, PROTOCOL_TIMEOUT);

    prov_handlers[type].func(link, link->rx.buf);

    net_buf_simple_reset(link->rx.buf);

    return;

fail:
    /**
     * For the case MESH/PVNR/PROV/BV-10-C and MESH/PVNR/PROV/BI-14-C,
     * provisioner should send transaction ack before closing the link.
     */
    bt_mesh_gen_prov_ack_send(link, link->rx.id);

    close_link(link, CLOSE_REASON_FAILED);
}

static void gen_prov_cont(struct bt_mesh_prov_link *link,
                          struct prov_rx *rx,
                          struct net_buf_simple *buf)
{
    bool close = false;

    if (!bt_mesh_gen_prov_cont(link, buf, rx, &close)) {
        if (close) {
            close_link(link, CLOSE_REASON_FAILED);
        }
        return;
    }

    prov_msg_recv(link);
}

static void gen_prov_ack(struct bt_mesh_prov_link *link,
                         struct prov_rx *rx,
                         struct net_buf_simple *buf)
{
    BT_DBG("len %u", buf->len);

    if (!link->tx.buf[0]) {
        return;
    }

    if (!link->tx.id) {
        return;
    }

    if (rx->xact_id == (link->tx.id - 1)) {
        bt_mesh_prov_clear_tx(link, true);

        switch (link->expect_ack_for) {
        case PROV_START:
            send_pub_key(link);

            uint8_t pub_key_oob = link->conf_inputs[13];
            /* For case MESH/PVNR/PROV/BV-04-C, if using OOB public key,
             * the value of expect_ack_for shall be PROV_PUB_KEY.
             */
            if (pub_key_oob) {
                return;
            }
            break;
        case PROV_PUB_KEY:
            prov_gen_dh_key(link);
            break;
        default:
            break;
        }

        link->expect_ack_for = 0x00;
    }
}

static void gen_prov_start(struct bt_mesh_prov_link *link,
                           struct prov_rx *rx,
                           struct net_buf_simple *buf)
{
    bool close = false;

    if (!bt_mesh_gen_prov_start(link, buf, rx, &close)) {
        if (close) {
            close_link(link, CLOSE_REASON_FAILED);
        }
        return;
    }

    prov_msg_recv(link);
}

static const struct {
    void (*const func)(struct bt_mesh_prov_link *link,
                       struct prov_rx *rx,
                       struct net_buf_simple *buf);
    const uint8_t require_link;
    const uint8_t min_len;
} gen_prov[] = {
    { gen_prov_start, true, 3 },
    { gen_prov_ack,   true, 0 },
    { gen_prov_cont,  true, 0 },
    { gen_prov_ctl,   true, 0 },
};

static void gen_prov_recv(struct bt_mesh_prov_link *link,
                          struct prov_rx *rx,
                          struct net_buf_simple *buf)
{
    if (buf->len < gen_prov[GPCF(rx->gpc)].min_len) {
        BT_ERR("Too short GPC message type %u", GPCF(rx->gpc));
        return;
    }

    if (!bt_mesh_atomic_test_bit(link->flags, LINK_ACTIVE) &&
        gen_prov[GPCF(rx->gpc)].require_link) {
        BT_DBG("Ignoring message that requires active link");
        return;
    }

    gen_prov[GPCF(rx->gpc)].func(link, rx, buf);
}

static struct bt_mesh_prov_link *find_pba_link(uint32_t link_id)
{
    int i;

    /* PB-ADV link is from 0 to CONFIG_BLE_MESH_PBA_SAME_TIME */
    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        if (bt_mesh_atomic_test_bit(prov_links[i].flags, LINK_ACTIVE) &&
            prov_links[i].link_id == link_id) {
            return &prov_links[i];
        }
    }

    return NULL;
}

void bt_mesh_provisioner_pb_adv_recv(struct net_buf_simple *buf)
{
    struct bt_mesh_prov_link *link = NULL;
    struct prov_rx rx = {0};

    if (buf->len < 6) {
        BT_ERR("Too short provisioning packet (len %u)", buf->len);
        return;
    }

    rx.link_id = net_buf_simple_pull_be32(buf);
    rx.xact_id = net_buf_simple_pull_u8(buf);
    rx.gpc = net_buf_simple_pull_u8(buf);

    BT_DBG("link_id 0x%08x xact_id %u", rx.link_id, rx.xact_id);

    link = find_pba_link(rx.link_id);
    if (link == NULL) {
        BT_DBG("Ignoring mesh beacon for unknown link");
        return;
    }

    gen_prov_recv(link, &rx, buf);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
static struct bt_mesh_prov_link *find_pbg_link(struct bt_mesh_conn *conn)
{
    int i;

    /* PB-GATT link is from CONFIG_BLE_MESH_PBA_SAME_TIME to BLE_MESH_PROV_SAME_TIME */
    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (bt_mesh_atomic_test_bit(prov_links[i].flags, LINK_ACTIVE) &&
            prov_links[i].conn == conn) {
            return &prov_links[i];
        }
    }

    return NULL;
}

int bt_mesh_provisioner_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf)
{
    struct bt_mesh_prov_link *link = NULL;
    uint8_t type = 0U;

    BT_DBG("%u bytes: %s", buf->len, bt_hex(buf->data, buf->len));

    link = find_pbg_link(conn);
    if (link == NULL) {
        BT_ERR("Link not found, conn %p", conn);
        return -ENOTCONN;
    }

    if (buf->len < 1) {
        BT_ERR("Too short provisioning packet (len %u)", buf->len);
        return -EINVAL;
    }

    type = net_buf_simple_pull_u8(buf);

    if (type >= ARRAY_SIZE(prov_handlers)) {
        BT_ERR("Unknown provisioning PDU type 0x%02x", type);
        close_link(link, CLOSE_REASON_FAILED);
        return -EINVAL;
    }

    if (type != PROV_FAILED && type != link->expect) {
        BT_ERR("Unexpected msg 0x%02x != 0x%02x", type, link->expect);
        close_link(link, CLOSE_REASON_FAILED);
        return -EINVAL;
    }

    if (!bt_mesh_prov_pdu_check(type, buf->len, NULL)) {
        close_link(link, CLOSE_REASON_FAILED);
        return -EINVAL;
    }

    k_delayed_work_submit(&link->prot_timer, PROTOCOL_TIMEOUT);

    prov_handlers[type].func(link, buf);

    return 0;
}

int bt_mesh_provisioner_pb_gatt_open(struct bt_mesh_conn *conn, const uint8_t addr[6])
{
    struct bt_mesh_prov_link *link = NULL;
    int i;

    if (conn == NULL || addr == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        if (!memcmp(prov_links[i].addr.val, addr, BLE_MESH_ADDR_LEN)) {
            prov_links[i].conn = bt_mesh_conn_ref(conn);
            link = &prov_links[i];
            break;
        }
    }

    if (link == NULL) {
        BT_ERR("Device address %s not found", bt_hex(addr, BLE_MESH_ADDR_LEN));
        return -ENODEV;
    }

    /* Double check if the device is currently being provisioned using PB-ADV. */
#if CONFIG_BLE_MESH_PB_ADV
    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        if (bt_mesh_atomic_test_bit(prov_links[i].flags, LINK_ACTIVE)) {
            if (!memcmp(prov_links[i].uuid, link->uuid, 16)) {
                BT_WARN("Provision using PB-GATT & PB-ADV same time");
                close_link(link, CLOSE_REASON_FAILED);
                return -EALREADY;
            }
        }
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

    bt_mesh_atomic_set_bit(link->flags, LINK_ACTIVE);

    /* May use lcd to indicate starting provisioning each device */
    if (bt_mesh_prov_get()->prov_link_open) {
        bt_mesh_prov_get()->prov_link_open(BLE_MESH_PROV_GATT);
    }

    /* TODO: memory optimization - calloc */

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    if (PROV_REC_SUPPORT(link->oob_info)) {
        if (bt_mesh_prov_get()->cert_based_prov_start) {
            bt_mesh_prov_get()->cert_based_prov_start(link - prov_links);
        }
    } else
#endif
    {
        send_invite(link);
    }

    return 0;
}

int bt_mesh_provisioner_pb_gatt_close(struct bt_mesh_conn *conn, uint8_t reason)
{
    struct bt_mesh_prov_link *link = NULL;

    BT_DBG("conn %p", conn);

    link = find_pbg_link(conn);
    if (link == NULL) {
        BT_ERR("Not connected, conn %p", conn);
        return -ENOTCONN;
    }

    k_delayed_work_cancel(&link->prot_timer);

    if (bt_mesh_prov_get()->prov_link_close) {
        bt_mesh_prov_get()->prov_link_close(BLE_MESH_PROV_GATT, reason);
    }

    prov_memory_free(link);

    memset(link, 0, offsetof(struct bt_mesh_prov_link, prot_timer));

    link->pb_gatt_send = prov_send_gatt;

    return 0;
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

static void protocol_timeout(struct k_work *work)
{
    struct bt_mesh_prov_link *link = work->user_data;

    BT_WARN("Protocol timeout");

    close_link(link, CLOSE_REASON_TIMEOUT);
}

int bt_mesh_provisioner_prov_init(void)
{
    const uint8_t *key = NULL;
    int i;

    if (bt_mesh_prov_get() == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    key = bt_mesh_pub_key_get();
    if (!key) {
        BT_ERR("Failed to generate Public Key");
        return -EIO;
    }

    prov_ctx.primary_addr = BLE_MESH_ADDR_UNASSIGNED;

    if (bt_mesh_prov_get()->prov_static_oob_val &&
        bt_mesh_prov_get()->prov_static_oob_len) {
        prov_ctx.static_oob_len = MIN(BLE_MESH_PROV_STATIC_OOB_MAX_LEN, bt_mesh_prov_get()->prov_static_oob_len);
        memcpy(prov_ctx.static_oob_val,
               bt_mesh_prov_get()->prov_static_oob_val,
               prov_ctx.static_oob_len);
    }

#if CONFIG_BLE_MESH_PB_ADV
    for (i = 0; i < CONFIG_BLE_MESH_PBA_SAME_TIME; i++) {
        rx_buf[i].buf.size = PROV_RX_BUF_SIZE;
        rx_buf[i].buf.__buf = rx_buf_data + (i * PROV_RX_BUF_SIZE);

        prov_links[i].pending_ack = PROV_XACT_NVAL;

        bt_mesh_prov_retransmit_init(&prov_links[i]);

        prov_links[i].rx.prev_id = PROV_XACT_NVAL;
        prov_links[i].rx.buf = get_rx_buf(i);

        prov_links[i].next_xact_id = pvnr_next_xact_id;
        prov_links[i].reset_adv_link = reset_adv_link;
        prov_links[i].retrans_timeout = close_link;

#if CONFIG_BLE_MESH_FAST_PROV
        prov_links[i].last_tx_pdu = PROV_DATA;
#endif /* CONFIG_BLE_MESH_FAST_PROV */

        bt_mesh_mutex_create(&prov_links[i].buf_lock);
    }
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
    for (i = CONFIG_BLE_MESH_PBA_SAME_TIME; i < BLE_MESH_PROV_SAME_TIME; i++) {
        prov_links[i].pb_gatt_send = prov_send_gatt;
    }
#endif /* CONFIG_BLE_MESH_PB_GATT */

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        k_delayed_work_init(&prov_links[i].prot_timer, protocol_timeout);
        prov_links[i].prot_timer.work.user_data = &prov_links[i];
    }

#if CONFIG_BLE_MESH_PB_ADV
    bt_mesh_mutex_create(&prov_ctx.pb_adv_lock);
#endif
#if CONFIG_BLE_MESH_PB_GATT
    bt_mesh_mutex_create(&prov_ctx.pb_gatt_lock);
#endif

    return 0;
}

int bt_mesh_provisioner_prov_reset(bool erase)
{
    int i;

    if (bt_mesh_prov_get() == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
        k_delayed_work_cancel(&prov_links[i].prot_timer);

        prov_memory_free(&prov_links[i]);

        if (i < CONFIG_BLE_MESH_PBA_SAME_TIME) {
#if CONFIG_BLE_MESH_PB_ADV
            bt_mesh_prov_clear_tx(&prov_links[i], true);
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
            bt_mesh_update_exceptional_list(BLE_MESH_EXCEP_LIST_SUB_CODE_REMOVE,
                                            BLE_MESH_EXCEP_LIST_TYPE_MESH_LINK_ID,
                                            &prov_links[i].link_id);
#endif
            memset(&prov_links[i], 0, offsetof(struct bt_mesh_prov_link, tx.retransmit));
            prov_links[i].pending_ack = PROV_XACT_NVAL;
            prov_links[i].rx.prev_id = PROV_XACT_NVAL;
            prov_links[i].rx.buf = get_rx_buf(i);
#endif /* CONFIG_BLE_MESH_PB_ADV */
        } else {
            memset(&prov_links[i], 0, offsetof(struct bt_mesh_prov_link, prot_timer));
        }

#if CONFIG_BLE_MESH_CERT_BASED_PROV
        for (size_t j = 0; j < BLE_MESH_REC_MAX_ID; j++) {
            if (prov_links[i].records[j]) {
                bt_mesh_free(prov_links[i].records[j]);
                prov_links[i].records[j] = NULL;
            }
        }
#endif /* CONFIG_BLE_MESH_CERT_BASED_PROV */
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

    if (bt_mesh_prov_get() == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    bt_mesh_provisioner_prov_reset(erase);

    for (i = 0; i < BLE_MESH_PROV_SAME_TIME; i++) {
#if CONFIG_BLE_MESH_PB_ADV
        if (i < CONFIG_BLE_MESH_PBA_SAME_TIME) {
            k_delayed_work_free(&prov_links[i].tx.retransmit);
            bt_mesh_mutex_free(&prov_links[i].buf_lock);
        }
#endif
        k_delayed_work_free(&prov_links[i].prot_timer);
        memset(&prov_links[i], 0, sizeof(prov_links[i]));
    }

#if CONFIG_BLE_MESH_PB_ADV
    bt_mesh_mutex_free(&prov_ctx.pb_adv_lock);
#endif
#if CONFIG_BLE_MESH_PB_GATT
    bt_mesh_mutex_free(&prov_ctx.pb_gatt_lock);
#endif
    prov_ctx.static_oob_len = 0U;
    memset(prov_ctx.static_oob_val, 0, sizeof(prov_ctx.static_oob_val));

#if CONFIG_BLE_MESH_PB_ADV
    memset(rx_buf, 0, sizeof(rx_buf));
    memset(rx_buf_data, 0, sizeof(rx_buf_data));
#endif

    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static bool bt_mesh_prov_svc_adv_filter(void)
{
    static uint32_t timestamp = 0;
    static uint32_t pre_timestamp = 0;

    timestamp = k_uptime_get_32();

    if (PROV_SVC_ADV_RX_CHECK(pre_timestamp, timestamp)) {
        pre_timestamp = timestamp;
        return false;
    }

    return true;
}

static bool notify_unprov_dev_info(bt_mesh_prov_bearer_t bearer, const uint8_t uuid[16],
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

            if (adv_type == BLE_MESH_ADV_IND && bt_mesh_prov_svc_adv_filter()) {
                return true;
            }

            if (notify_unprov_adv_pkt_cb) {
                notify_unprov_adv_pkt_cb(addr->val, addr->type, adv_type,
                                         uuid, oob_info, bearer, rssi);
            }
            return true;
        }

        if (!(unprov_dev[i].bearer & bearer)) {
            BT_WARN("Device in queue not support PB-%s",
                    (bearer == BLE_MESH_PROV_ADV) ? "ADV" : "GATT");
            if (notify_unprov_adv_pkt_cb) {
                notify_unprov_adv_pkt_cb(addr->val, addr->type, adv_type,
                                         uuid, oob_info, bearer, rssi);
            }
            return true;
        }
    }

    return false;
}

#if CONFIG_BLE_MESH_PB_ADV
void bt_mesh_provisioner_unprov_beacon_recv(struct net_buf_simple *buf, int8_t rssi)
{
    const bt_mesh_addr_t *addr = NULL;
    const uint8_t *uuid = NULL;
    uint16_t oob_info = 0U;

    if (!(prov_ctx.bearers & BLE_MESH_PROV_ADV)) {
        BT_INFO("Not support PB-ADV bearer");
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

    if (notify_unprov_dev_info(BLE_MESH_PROV_ADV, uuid, addr, oob_info, rssi)) {
        return;
    }

    provisioner_start_prov_pb_adv(uuid, addr, oob_info, BLE_MESH_ADDR_UNASSIGNED);
}
#endif /* CONFIG_BLE_MESH_PB_ADV */

#if CONFIG_BLE_MESH_PB_GATT
void bt_mesh_provisioner_prov_adv_recv(struct net_buf_simple *buf,
                                       const bt_mesh_addr_t *addr, int8_t rssi)
{
    const uint8_t *uuid = NULL;
    uint16_t oob_info = 0U;

    if (!(prov_ctx.bearers & BLE_MESH_PROV_GATT)) {
        BT_INFO("Not support PB-GATT bearer");
        return;
    }

    if (bt_mesh_gattc_get_free_conn_count() == 0) {
        BT_INFO("BLE connections for mesh reach max limit");
        return;
    }

    uuid = buf->data;
    net_buf_simple_pull(buf, 16);
    /* According CSS, all the field within adv data shall be little-endian */
    oob_info = net_buf_simple_pull_le16(buf);

    if (provisioner_check_unprov_dev_info(uuid, BLE_MESH_PROV_GATT)) {
        return;
    }

    if (notify_unprov_dev_info(BLE_MESH_PROV_GATT, uuid, addr, oob_info, rssi)) {
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
     * Use CONNECTING flag to prevent if two devices's adv pkts are both received,
     * the previous one info will be replaced by the second one.
     */
    provisioner_start_prov_pb_gatt(uuid, addr, oob_info, BLE_MESH_ADDR_UNASSIGNED);
}
#endif /* CONFIG_BLE_MESH_PB_GATT */

/* This function can be invoked by Remote Provisioning Client
 * to handle received Provisioning PDUs.
 */
int bt_mesh_rpr_cli_pdu_recv(struct bt_mesh_prov_link *link, uint8_t type,
                             struct net_buf_simple *buf)
{
    if (type != link->expect) {
        BT_ERR("PB-Remote, unexpected msg 0x%02x != 0x%02x", type, link->expect);
        return -EINVAL;
    }

    prov_handlers[type].func(link, buf);
    return 0;
}

int bt_mesh_rpr_cli_pdu_send(struct bt_mesh_prov_link *link, uint8_t type)
{
    switch (type) {
    case PROV_INVITE:
        send_invite(link);
        break;
    case PROV_PUB_KEY:
        send_pub_key(link);
        break;
    case PROV_CONFIRM:
        send_confirm(link);
        break;
    default:
        break;
    }

    return 0;
}

/* The behavior of sending confirmation based on the combination
 * of Public Key and authentication:
 *
 * 1. No OOB Public Key + No OOB
 *  - No need to send confirmation manually, it will be sent when
 *    Device Public Key is received.
 *
 * 2. No OOB Public Key + Static OOB
 *  - Same with 1.
 *
 * 3. No OOB Public Key + Output OOB
 *  - Confirmation will be sent after the authentication value is
 *    set with bt_mesh_provisioner_set_oob_input_data().
 *
 * 4. No OOB Public Key + Input OOB
 *  - No need to send confirmation manually, it will be sent when
 *    Input Complete is received.
 *    Note:
 *    Need to make sure the authentication value has been set with
 *    bt_mesh_provisioner_set_oob_output_data() before sending
 *    confirmation.
 *
 * 5. OOB Public Key + No OOB
 *  - If OOB Public Key is set before sending Public Key, we need
 *    to wait for the Remote Provisioning PDU Outbound Report for
 *    Public Key, then send confirmation manually;
 *  - If OOB Public Key is set after sending Public Key, and before
 *    receiving the Remote Provisioning PDU Outbound Report for
 *    Public Key, need to send confirmation manually;
 *  - If OOB Public Key is set after sending Public Key, and after
 *    receiving the Remote Provisioning PDU Outbound Report for
 *    Public Key, no need to sending confirmation manually, it will
 *    be sent after DHKey is generated successfully.
 *
 * 6. OOB Public Key + Static OOB
 *  - Same with 5.
 *
 * 7. OOB Public Key + Output OOB
 *  - If OOB Public Key is set before sending Public Key, DHKey
 *    will be generated after Public Key is sent, and:
 *      - If authentication value is set before receiving Remote
 *        Provisioning PDU Outbound Report, we need to wait for
 *        the Remote Provisioning PDU Outbound Report for Public
 *        Key, then send confirmation manually;
 *      - If authentication value is set after receiving Remote
 *        Provisioning PDU Outbound Report, no need to send
 *        confirmation manually, it will be sent in
 *        bt_mesh_provisioner_set_oob_input_data().
 *  - If OOB Public Key is set after sending Public Key, and the
 *    OOB Public Key should be set before setting authentication
 *    value. After OOB Public Key is set, DHKey will be generated.
 *    And after the authentication value is set:
 *      - If authentication value is set before receiving Remote
 *        Provisioning PDU Outbound Report, we need to wait for
 *        the Remote Provisioning PDU Outbound Report for Public
 *        Key, then send confirmation manually;
 *      - If authentication value is set after receiving Remote
 *        Provisioning PDU Outbound Report, no need to send
 *        confirmation manually, it will be sent in
 *        bt_mesh_provisioner_set_oob_input_data().
 *
 * 8. OOB Public Key + Input OOB
 *  - No need to send confirmation manually, it will be sent
 *    when Input Complete is received.
 *    Note:
 *    Need to make sure the authentication value has been set
 *    with bt_mesh_provisioner_set_oob_output_data() before
 *    sending confirmation.
 *
 * Can change the behavior that after sending Public Key, then
 * notify the application layer to input OOB Public Key, this
 * could simplify the implementation.
 */

/* The following two functions need to be used based on the above
 * considerations.
 */
int bt_mesh_rpr_cli_recv_pub_key_outbound_report(struct bt_mesh_prov_link *link)
{
    bt_mesh_atomic_test_and_clear_bit(link->flags, WAIT_PK_OBR);

    if (bt_mesh_atomic_test_and_clear_bit(link->flags, SEND_CONFIRM)) {
        send_confirm(link);
    }

    return 0;
}

#endif /* CONFIG_BLE_MESH_PROVISIONER */
