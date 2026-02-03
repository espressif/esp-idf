/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "mesh.h"
#include "crypto.h"
#include "adv.h"
#include "rpl.h"
#include "access.h"
#include "settings.h"
#include "friend.h"
#include "transport.h"
#include "foundation.h"
#include "mesh/common.h"
#include "proxy_client.h"
#include "prov_common.h"
#include "prov_pvnr.h"
#include "pvnr_mgmt.h"

#if CONFIG_BLE_MESH_V11_SUPPORT
#include "mesh_v1.1/utils.h"
#endif

#if CONFIG_BLE_MESH_PROVISIONER

static struct bt_mesh_node *mesh_nodes[CONFIG_BLE_MESH_MAX_PROV_NODES];
static bt_mesh_mutex_t provisioner_lock;
static uint16_t node_count;

static int provisioner_remove_node(uint16_t index, bool erase);

static inline void bt_mesh_provisioner_mutex_new(void)
{
    if (!provisioner_lock.mutex) {
        bt_mesh_mutex_create(&provisioner_lock);
    }
}

#if CONFIG_BLE_MESH_DEINIT
static inline void bt_mesh_provisioner_mutex_free(void)
{
    bt_mesh_mutex_free(&provisioner_lock);
}
#endif /* CONFIG_BLE_MESH_DEINIT */

static inline void bt_mesh_provisioner_lock(void)
{
    bt_mesh_mutex_lock(&provisioner_lock);
}

static inline void bt_mesh_provisioner_unlock(void)
{
    bt_mesh_mutex_unlock(&provisioner_lock);
}

int bt_mesh_provisioner_init(void)
{
    bt_mesh_provisioner_mutex_new();

    return 0;
}

/**
 * When a Provisioner tries to create a network, it will check the
 * status of the restored network keys firstly, and try to create
 * one if they are not existed.
 */
int bt_mesh_provisioner_net_create(void)
{
    struct bt_mesh_subnet *sub = NULL;
    uint8_t p_key[16] = {0};

    if (bt_mesh_prov_get() == NULL) {
        BT_ERR("No provisioning context provided");
        return -EINVAL;
    }

    if (bt_mesh.p_sub[0]) {
        /* Provisioner is already enabled (enable -> disable -> enable),
         * or Provisioner is restored from flash.
         */
        BT_INFO("Provisioner already created network");
        sub = bt_mesh.p_sub[0];
        goto done;
    }

    /* Generate the primary netkey */
    if (bt_mesh_rand(p_key, 16)) {
        BT_ERR("Failed to generate Primary NetKey");
        return -EIO;
    }

    sub = bt_mesh_calloc(sizeof(struct bt_mesh_subnet));
    if (!sub) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    sub->kr_flag = BLE_MESH_KEY_REFRESH(bt_mesh_prov_get()->flags);
    if (sub->kr_flag) {
        if (bt_mesh_net_keys_create(&sub->keys[1], p_key)) {
            BT_ERR("Failed to generate net-related keys");
            bt_mesh_free(sub);
            return -EIO;
        }
        sub->kr_phase = BLE_MESH_KR_PHASE_2;
    } else {
        /* Currently provisioner only use keys[0] */
        if (bt_mesh_net_keys_create(&sub->keys[0], p_key)) {
            BT_ERR("Failed to create net-related keys");
            bt_mesh_free(sub);
            return -EIO;
        }
        sub->kr_phase = BLE_MESH_KR_NORMAL;
    }
    sub->net_idx = BLE_MESH_KEY_PRIMARY;
    sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_directed_forwarding_sub_init(sub);
#endif

    bt_mesh.p_sub[0] = sub;

    /* Dynamically added appkey & netkey will use these key_idx */
    bt_mesh.p_app_idx_next = 0x0000;
    bt_mesh.p_net_idx_next = 0x0001;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_net_idx();
        bt_mesh_store_p_app_idx();
        bt_mesh_store_p_subnet(bt_mesh.p_sub[0]);
    }

    bt_mesh.iv_index = bt_mesh_prov_get()->iv_index;
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS,
                              BLE_MESH_IV_UPDATE(bt_mesh_prov_get()->flags));

    /* Set minimum required hours, since the 96-hour minimum requirement
     * doesn't apply straight after provisioning (since we can't know how
     * long has actually passed since the network changed its state).
     * This operation is the same with node initialization.
     */
    bt_mesh.ivu_duration = BLE_MESH_IVU_MIN_HOURS;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_iv(true);
    }

done:
    BT_INFO("NetKeyIndex 0x%03x, NID 0x%02x", sub->net_idx, sub->keys[0].nid);
    BT_INFO("NetKey %s", bt_hex(sub->keys[0].net, 16));

    return 0;
}

void bt_mesh_provisioner_main_reset(bool erase)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        if (bt_mesh.p_sub[i]) {
            bt_mesh_provisioner_local_net_key_del(bt_mesh.p_sub[i]->net_idx, erase);
        }
    }

    /* Clear model state that isn't otherwise cleared. E.g. AppKey
     * binding and model publication is cleared as a consequence
     * of removing all app keys, however model subscription clearing
     * must be taken care of here.
     */
    bt_mesh_mod_sub_reset(erase);

    bt_mesh.p_net_idx_next = 0U;
    bt_mesh.p_app_idx_next = 0U;

    if (erase && IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_p_net_idx();
        bt_mesh_clear_p_app_idx();
    }

    for (i = 0; i < CONFIG_BLE_MESH_MAX_PROV_NODES; i++) {
        provisioner_remove_node(i, erase);
    }

    node_count = 0U;
}

#if CONFIG_BLE_MESH_DEINIT
int bt_mesh_provisioner_deinit(bool erase)
{
    bt_mesh_provisioner_main_reset(erase);
    bt_mesh_provisioner_mutex_free();
    return 0;
}
#endif /* CONFIG_BLE_MESH_DEINIT */

bool bt_mesh_provisioner_check_is_addr_dup(uint16_t addr, uint8_t elem_num, bool comp_with_own)
{
    const struct bt_mesh_comp *comp = NULL;
    struct bt_mesh_node *node = NULL;
    uint16_t primary_addr = BLE_MESH_ADDR_UNASSIGNED;
    uint16_t comp_addr = BLE_MESH_ADDR_UNASSIGNED;
    int i;

    if (comp_with_own) {
        comp = bt_mesh_comp_get();
        if (!comp) {
            BT_ERR("Invalid composition data");
            return true;
        }

        primary_addr = bt_mesh_provisioner_get_primary_elem_addr();
        if (!BLE_MESH_ADDR_IS_UNICAST(primary_addr)) {
            BT_ERR("Invalid unicast address 0x%04x", primary_addr);
            return true;
        }
    }

    bt_mesh_provisioner_lock();

    for (comp_addr = addr; comp_addr < addr + elem_num; comp_addr++) {
        for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
            node = mesh_nodes[i];
            if (node && comp_addr >= node->unicast_addr &&
                    comp_addr < node->unicast_addr + node->element_num) {
                BT_ERR("Duplicate with node address 0x%04x", comp_addr);
                bt_mesh_provisioner_unlock();
                return true;
            }

            if (comp_with_own && comp_addr >= primary_addr &&
                    comp_addr < primary_addr + comp->elem_count) {
                BT_ERR("Duplicate with Provisioner address 0x%04x", comp_addr);
                bt_mesh_provisioner_unlock();
                return true;
            }
        }
    }

    bt_mesh_provisioner_unlock();

    return false;
}

static void provisioner_node_count_inc(void)
{
    node_count++;
}

static void provisioner_node_count_dec(void)
{
    if (node_count) {
        node_count--;
    }
}

uint16_t bt_mesh_provisioner_get_node_count(void)
{
    return node_count;
}

static int provisioner_store_node(struct bt_mesh_node *node, bool store, uint16_t *index)
{
    int i;

    bt_mesh_provisioner_lock();

    /* Check if the node already exists */
    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i] && !memcmp(mesh_nodes[i]->dev_uuid, node->dev_uuid, 16)) {
            BT_WARN("Node already exists, uuid %s", bt_hex(node->dev_uuid, 16));
            bt_mesh_provisioner_unlock();
            return -EEXIST;
        }
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i] == NULL) {
            mesh_nodes[i] = bt_mesh_calloc(sizeof(struct bt_mesh_node));
            if (!mesh_nodes[i]) {
                BT_ERR("%s, Out of memory", __func__);
                bt_mesh_provisioner_unlock();
                return -ENOMEM;
            }

            memcpy(mesh_nodes[i], node, sizeof(struct bt_mesh_node));
            provisioner_node_count_inc();
            if (index) {
                *index = i;
            }

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
                bt_mesh_store_node_info(mesh_nodes[i]);
            }

            bt_mesh_provisioner_unlock();
            return 0;
        }
    }

    BT_ERR("Node is full!");
    bt_mesh_provisioner_unlock();
    return -ENOMEM;
}

int bt_mesh_provisioner_restore_node_info(struct bt_mesh_node *node)
{
    if (!node) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    return provisioner_store_node(node, false, NULL);
}

int bt_mesh_provisioner_provision(const bt_mesh_addr_t *addr, const uint8_t uuid[16],
                                  uint16_t oob_info, uint16_t unicast_addr,
                                  uint8_t element_num, uint16_t net_idx,
                                  uint8_t flags, uint32_t iv_index,
                                  const uint8_t dev_key[16], uint16_t *index, bool nppi)
{
    struct bt_mesh_node node = {0};

    if (!addr || !uuid || !dev_key || !index) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_INFO("Unicast addr 0x%04x, element num %d, NetKeyIndex 0x%04x",
            unicast_addr, element_num, net_idx);
    BT_INFO("UUID %s", bt_hex(uuid, 16));
    BT_INFO("DevKey %s", bt_hex(dev_key, 16));

    if (nppi == true) {
        /* For NPPI, remove the node from database firstly. */
        bt_mesh_provisioner_remove_node(uuid);
    }

    memcpy(node.addr, addr->val, BLE_MESH_ADDR_LEN);
    node.addr_type = addr->type;
    memcpy(node.dev_uuid, uuid, 16);
    node.oob_info = oob_info;
    node.unicast_addr = unicast_addr;
    node.element_num = element_num;
    node.net_idx = net_idx;
    node.flags = flags;
    node.iv_index = iv_index;
    memcpy(node.dev_key, dev_key, 16);

    return provisioner_store_node(&node, true, index);
}

static int provisioner_remove_node(uint16_t index, bool erase)
{
    struct bt_mesh_node *node = NULL;
    int i;

    BT_DBG("Remove node %d", index);

    bt_mesh_provisioner_lock();

    if (mesh_nodes[index] == NULL) {
        bt_mesh_provisioner_unlock();
        return 0;
    }

    node = mesh_nodes[index];

    /* Reset corresponding network cache when reset the node */
    bt_mesh_msg_cache_clear(node->unicast_addr, node->element_num);

    /* Reset corresponding transport info when removing the node */
    for (i = 0; i < node->element_num; i++) {
        bt_mesh_rx_reset_single(node->unicast_addr + i);
    }
    for (i = 0; i < node->element_num; i++) {
        bt_mesh_tx_reset_single(node->unicast_addr + i);
    }
    for (i = 0; i < node->element_num; i++) {
        bt_mesh_rpl_reset_single(node->unicast_addr + i, erase);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_remove_lpn(node->unicast_addr);
    }

    if (erase && IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_node_info(node->unicast_addr);
    }

    if (mesh_nodes[index]->comp_data) {
        bt_mesh_free(mesh_nodes[index]->comp_data);
    }
    bt_mesh_free(mesh_nodes[index]);
    mesh_nodes[index] = NULL;

    provisioner_node_count_dec();

    bt_mesh_provisioner_unlock();
    return 0;
}

static struct bt_mesh_node *provisioner_find_node_with_uuid(const uint8_t uuid[16], uint16_t *index)
{
    int i;

    if (uuid == NULL) {
        BT_ERR("Invalid device uuid");
        return NULL;
    }

    bt_mesh_provisioner_lock();

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i] && !memcmp(mesh_nodes[i]->dev_uuid, uuid, 16)) {
            if (index) {
                *index = i;
            }
            bt_mesh_provisioner_unlock();
            return mesh_nodes[i];
        }
    }

    bt_mesh_provisioner_unlock();
    return NULL;
}

int bt_mesh_provisioner_remove_node(const uint8_t uuid[16])
{
    struct bt_mesh_node *node = NULL;
    uint16_t index = 0U;
    int i;

    if (uuid == NULL) {
        for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
            if (mesh_nodes[i]) {
                provisioner_remove_node(i, true);
            }
        }
        return 0;
    }

    node = provisioner_find_node_with_uuid(uuid, &index);
    if (!node) {
        BT_WARN("Node not found, uuid %s", bt_hex(uuid, 16));
        return -ENODEV;
    }

    provisioner_remove_node(index, true);
    return 0;
}

static struct bt_mesh_node *provisioner_find_node_with_addr(uint16_t addr, uint16_t *index)
{
    struct bt_mesh_node *node = NULL;
    int i;

    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("Invalid unicast address 0x%04x", addr);
        return NULL;
    }

    bt_mesh_provisioner_lock();

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && addr >= node->unicast_addr &&
                addr < (node->unicast_addr + node->element_num)) {
            if (index) {
                *index = i;
            }
            bt_mesh_provisioner_unlock();
            return node;
        }
    }

    bt_mesh_provisioner_unlock();
    return NULL;
}

int bt_mesh_provisioner_restore_node_name(uint16_t addr, const char *name)
{
    struct bt_mesh_node *node = NULL;

    node = provisioner_find_node_with_addr(addr, NULL);
    if (node == NULL) {
        BT_ERR("Node not found, addr 0x%04x", addr);
        return -ENODEV;
    }

    strncpy(node->name, name, BLE_MESH_NODE_NAME_SIZE);
    node->name[BLE_MESH_NODE_NAME_SIZE] = 0;

    return 0;
}

struct bt_mesh_node *bt_mesh_provisioner_get_node_with_uuid(const uint8_t uuid[16])
{
    return provisioner_find_node_with_uuid(uuid, NULL);
}

struct bt_mesh_node *bt_mesh_provisioner_get_node_with_addr(uint16_t unicast_addr)
{
    return provisioner_find_node_with_addr(unicast_addr, NULL);
}

int bt_mesh_provisioner_delete_node_with_uuid(const uint8_t uuid[16])
{
    struct bt_mesh_node *node = NULL;
    uint16_t index = 0U;

    node = provisioner_find_node_with_uuid(uuid, &index);
    if (!node) {
        BT_WARN("Node not found, uuid %s", bt_hex(uuid, 16));
        return -ENODEV;
    }

    provisioner_remove_node(index, true);
    return 0;
}

int bt_mesh_provisioner_delete_node_with_node_addr(uint16_t unicast_addr)
{
    struct bt_mesh_node *node = NULL;
    uint16_t index = 0U;

    node = provisioner_find_node_with_addr(unicast_addr, &index);
    if (!node) {
        BT_WARN("Node not found, addr 0x%04x", unicast_addr);
        return -ENODEV;
    }

    provisioner_remove_node(index, true);
    return 0;
}

int bt_mesh_provisioner_delete_node_with_dev_addr(const bt_mesh_addr_t *addr)
{
    int i;

    bt_mesh_provisioner_lock();

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i] && mesh_nodes[i]->addr_type == addr->type &&
            !memcmp(mesh_nodes[i]->addr, addr->val, BLE_MESH_ADDR_LEN)) {
            bt_mesh_provisioner_unlock();
            return provisioner_remove_node(i, true);
        }
    }

    bt_mesh_provisioner_unlock();

    BT_WARN("Node not exist, device address %s", bt_hex(addr->val, BLE_MESH_ADDR_LEN));
    return -ENODEV;
}

static struct bt_mesh_node **provisioner_find_node_with_name(const char *name)
{
    size_t length = 0U;
    int i;

    BT_DBG("node name %s", name);

    length = MIN(strlen(name), BLE_MESH_NODE_NAME_SIZE);

    bt_mesh_provisioner_lock();

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i]) {
            if (strlen(mesh_nodes[i]->name) != length) {
                continue;
            }
            if (!strncmp(mesh_nodes[i]->name, name, length)) {
                bt_mesh_provisioner_unlock();
                return &mesh_nodes[i];
            }
        }
    }

    bt_mesh_provisioner_unlock();
    return NULL;
}

int bt_mesh_provisioner_set_node_name(uint16_t index, const char *name)
{
    if (index >= ARRAY_SIZE(mesh_nodes)) {
        BT_ERR("Invalid node index %d", index);
        return -EINVAL;
    }

    if (mesh_nodes[index] == NULL) {
        BT_ERR("Node not exists, index %d", index);
        return -EINVAL;
    }

    if (name == NULL) {
        BT_ERR("Invalid node name");
        return -EINVAL;
    }

    if (provisioner_find_node_with_name(name)) {
        BT_WARN("Node name \"%s\" already exists", name);
        return -EEXIST;
    }

    memset(mesh_nodes[index]->name, 0, sizeof(mesh_nodes[index]->name));
    strncpy(mesh_nodes[index]->name, name, BLE_MESH_NODE_NAME_SIZE);

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_node_name(mesh_nodes[index]);
    }

    return 0;
}

const char *bt_mesh_provisioner_get_node_name(uint16_t index)
{
    if (index >= ARRAY_SIZE(mesh_nodes)) {
        BT_ERR("Invalid node index %d", index);
        return NULL;
    }

    if (mesh_nodes[index] == NULL) {
        BT_ERR("Node not exists, index %d", index);
        return NULL;
    }

    return mesh_nodes[index]->name;
}

uint16_t bt_mesh_provisioner_get_node_index(const char *name)
{
    struct bt_mesh_node **node = NULL;

    if (name == NULL) {
        BT_ERR("Invalid node name");
        return BLE_MESH_INVALID_NODE_INDEX;
    }

    node = provisioner_find_node_with_name(name);
    if (node == NULL) {
        BT_ERR("Node name \"%s\" not exists", name);
        return BLE_MESH_INVALID_NODE_INDEX;
    }

    return (node - mesh_nodes);
}

struct bt_mesh_node *bt_mesh_provisioner_get_node_with_name(const char *name)
{
    struct bt_mesh_node **node = NULL;

    if (name == NULL) {
        BT_ERR("Invalid node name");
        return NULL;
    }

    node = provisioner_find_node_with_name(name);
    if (node == NULL) {
        BT_ERR("Node name \"%s\" not exists", name);
        return NULL;
    }

    return *node;
}

const struct bt_mesh_node **bt_mesh_provisioner_get_node_table_entry(void)
{
    return (const struct bt_mesh_node **)mesh_nodes;
}

#define COMP_DATA_PAGE_0_MIN_LEN    16

static int store_node_comp_data(uint16_t addr, const uint8_t *data, uint16_t length, bool store)
{
    struct bt_mesh_node *node = NULL;

    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("Invalid unicast address 0x%04x", addr);
        return -EINVAL;
    }

    if (data == NULL || (length % 2) || length < COMP_DATA_PAGE_0_MIN_LEN) {
        BT_ERR("Invalid composition data");
        return -EINVAL;
    }

    node = provisioner_find_node_with_addr(addr, NULL);
    if (node == NULL) {
        BT_ERR("Node not found, addr 0x%04x", addr);
        return -ENODEV;
    }

    bt_mesh_provisioner_lock();

    /* Free old composition data of the node */
    if (node->comp_data) {
        bt_mesh_free(node->comp_data);
        node->comp_data = NULL;
        node->comp_length = 0;
    }

    node->comp_data = bt_mesh_calloc(length);
    if (node->comp_data == NULL) {
        BT_ERR("%s, Out of memory", __func__);
        bt_mesh_provisioner_unlock();
        return -ENOMEM;
    }

    memcpy(node->comp_data, data, length);
    node->comp_length = length;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
        bt_mesh_store_node_comp_data(node);
    }

    bt_mesh_provisioner_unlock();
    return 0;
}

int bt_mesh_provisioner_store_node_comp_data(uint16_t addr, const uint8_t *data, uint16_t length)
{
    return store_node_comp_data(addr, data, length, true);
}

int bt_mesh_provisioner_restore_node_comp_data(uint16_t addr, const uint8_t *data, uint16_t length)
{
    return store_node_comp_data(addr, data, length, false);
}

/* Provisioner DevKey, NetKey and AppKey related functions */

const uint8_t *bt_mesh_provisioner_net_key_get(uint16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            if (sub->kr_flag) {
                return sub->keys[1].net;
            }

            return sub->keys[0].net;
        }
    }

    return NULL;
}

bool bt_mesh_provisioner_check_msg_dst(uint16_t dst)
{
    struct bt_mesh_node *node = NULL;
    int i;

    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        return true;
    }

    bt_mesh_provisioner_lock();

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && dst >= node->unicast_addr &&
                dst < node->unicast_addr + node->element_num) {
            bt_mesh_provisioner_unlock();
            return true;
        }
    }

    bt_mesh_provisioner_unlock();
    return false;
}

const uint8_t *bt_mesh_provisioner_dev_key_get(uint16_t addr)
{
    /* Device key is only used to encrypt configuration messages.
     * Configuration model shall only be supported by the primary
     * element which uses the primary unicast address.
     */
    int i;

    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("Invalid unicast address 0x%04x", addr);
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        struct bt_mesh_node *node = mesh_nodes[i];

        if (node && node->unicast_addr == addr) {
            return node->dev_key;
        }
    }

    return NULL;
}

static int provisioner_check_app_key(const uint8_t app_key[16], uint16_t *app_idx)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    if (!app_key) {
        return 0;
    }

    /* Check if app_key is already existed */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && (!memcmp(key->keys[0].val, app_key, 16) ||
                    !memcmp(key->keys[1].val, app_key, 16))) {
            *app_idx = key->app_idx;
            return -EEXIST;
        }
    }

    return 0;
}

static int provisioner_check_app_idx(uint16_t app_idx, bool exist)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    if (exist) {
        /* Check if app_idx is already existed */
        for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
            key = bt_mesh.p_app_keys[i];
            if (key && (key->app_idx == app_idx)) {
                return -EEXIST;
            }
        }
        return 0;
    }

    /* Check if app_idx is not existed */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && (key->app_idx == app_idx)) {
            return 0;
        }
    }

    return -ENODEV;
}

static int provisioner_check_app_key_full(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        if (!bt_mesh.p_app_keys[i]) {
            return i;
        }
    }

    return -ENOMEM;
}

static int provisioner_check_net_key(const uint8_t net_key[16], uint16_t *net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    if (!net_key) {
        return 0;
    }

    /* Check if net_key is already existed */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && (!memcmp(sub->keys[0].net, net_key, 16) ||
                    !memcmp(sub->keys[1].net, net_key, 16))) {
            *net_idx = sub->net_idx;
            return -EEXIST;
        }
    }

    return 0;
}

static int provisioner_check_net_idx(uint16_t net_idx, bool exist)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    if (exist) {
        /* Check if net_idx is already existed */
        for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
            sub = bt_mesh.p_sub[i];
            if (sub && (sub->net_idx == net_idx)) {
                return -EEXIST;
            }
        }
        return 0;
    }

    /* Check if net_idx is not existed */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && (sub->net_idx == net_idx)) {
            return 0;
        }
    }

    return -ENODEV;
}

static int provisioner_check_net_key_full(void)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        if (!bt_mesh.p_sub[i]) {
            return i;
        }
    }

    return -ENOMEM;
}

int bt_mesh_provisioner_local_app_key_add(const uint8_t app_key[16],
                                          uint16_t net_idx, uint16_t *app_idx)
{
    struct bt_mesh_app_keys *keys = NULL;
    struct bt_mesh_app_key *key = NULL;
    uint8_t p_key[16] = {0};
    int add = -1;

    if (bt_mesh.p_app_idx_next >= 0x1000) {
        BT_ERR("No AppKeyIndex available");
        return -EIO;
    }

    if (!app_idx || (*app_idx != 0xFFFF && *app_idx >= 0x1000)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Check if the same application key already exists */
    if (provisioner_check_app_key(app_key, app_idx)) {
        BT_WARN("AppKey exists, AppKeyIndex updated");
        return 0;
    }

    /* Check if the net_idx exists */
    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", net_idx);
        return -ENODEV;
    }

    /* Check if the same app_idx already exists */
    if (provisioner_check_app_idx(*app_idx, true)) {
        BT_ERR("Invalid AppKeyIndex 0x%04x", *app_idx);
        return -EEXIST;
    }

    add = provisioner_check_app_key_full();
    if (add < 0) {
        BT_ERR("AppKey is full!");
        return -ENOMEM;
    }

    if (!app_key) {
        if (bt_mesh_rand(p_key, 16)) {
            BT_ERR("Failed to generate AppKey");
            return -EIO;
        }
    } else {
        memcpy(p_key, app_key, 16);
    }

    key = bt_mesh_calloc(sizeof(struct bt_mesh_app_key));
    if (!key) {
        BT_ERR("%s, Out of memory", __func__);
        return -ENOMEM;
    }

    keys = &key->keys[0];
    if (bt_mesh_app_id(p_key, &keys->id)) {
        BT_ERR("Failed to generate AID");
        bt_mesh_free(key);
        return -EIO;
    }

    memcpy(keys->val, p_key, 16);
    key->net_idx = net_idx;
    if (*app_idx != 0xFFFF) {
        key->app_idx = *app_idx;
    } else {
        key->app_idx = bt_mesh.p_app_idx_next;
        while (1) {
            if (provisioner_check_app_idx(key->app_idx, true)) {
                key->app_idx = (++bt_mesh.p_app_idx_next);
                if (key->app_idx >= 0x1000) {
                    BT_ERR("No AppKeyIndex available");
                    bt_mesh_free(key);
                    return -EIO;
                }
            } else {
                break;
            }
        }
        *app_idx = key->app_idx;
        bt_mesh.p_app_idx_next++;
    }
    key->updated = false;

    bt_mesh.p_app_keys[add] = key;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_app_idx();
        bt_mesh_store_p_app_key(key);
    }

    return 0;
}

int bt_mesh_provisioner_local_app_key_update(const uint8_t app_key[16],
                                             uint16_t net_idx, uint16_t app_idx)
{
    struct bt_mesh_app_keys *keys = NULL;
    struct bt_mesh_app_key *key = NULL;

    if (app_key == NULL) {
        BT_ERR("Invalid AppKey");
        return -EINVAL;
    }

    BT_INFO("AppKey %s, net_idx 0x%03x, app_idx 0x%03x", bt_hex(app_key, 16), net_idx, app_idx);

    /* Check if the net_idx exists */
    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", net_idx);
        return -ENODEV;
    }

    key = bt_mesh_app_key_get(app_idx);
    if (key == NULL) {
        BT_ERR("AppKey 0x%04x not found", app_idx);
        return -ENODEV;
    }

    keys = &key->keys[0];
    if (bt_mesh_app_id(app_key, &keys->id)) {
        BT_ERR("Failed to generate AID");
        return -EIO;
    }

    memset(keys->val, 0, 16);
    memcpy(keys->val, app_key, 16);

    key->updated = false;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_app_idx();
        bt_mesh_store_p_app_key(key);
    }

    return 0;
}

const uint8_t *bt_mesh_provisioner_local_app_key_get(uint16_t net_idx, uint16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", net_idx);
        return NULL;
    }

    if (provisioner_check_app_idx(app_idx, false)) {
        BT_ERR("Invalid AppKeyIndex 0x%04x", app_idx);
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && key->net_idx == net_idx &&
                key->app_idx == app_idx) {
            if (key->updated) {
                return key->keys[1].val;
            }
            return key->keys[0].val;
        }
    }

    return NULL;
}

static void model_pub_clear(struct bt_mesh_model *model, bool store)
{
    if (!model->pub) {
        return;
    }

    if (model->pub->addr == BLE_MESH_ADDR_UNASSIGNED) {
        return;
    }

    model->pub->addr = BLE_MESH_ADDR_UNASSIGNED;
    model->pub->key = 0U;
    model->pub->cred = 0U;
    model->pub->ttl = 0U;
    model->pub->period = 0U;
    model->pub->retransmit = 0U;
    model->pub->count = 0U;

    if (model->pub->update) {
        k_delayed_work_cancel(&model->pub->timer);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
        bt_mesh_store_mod_pub(model);
    }
}

static void model_unbind(struct bt_mesh_model *model, uint16_t app_idx, bool store)
{
    int i;

    BT_DBG("model %p app_idx 0x%03x", model, app_idx);

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] != app_idx) {
            continue;
        }

        model->keys[i] = BLE_MESH_KEY_UNUSED;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
            bt_mesh_store_mod_bind(model);
        }

        model_pub_clear(model, store);
    }
}

struct unbind_data {
    uint16_t app_idx;
    bool store;
};

static void _model_unbind(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                          bool vnd, bool primary, void *user_data)
{
    struct unbind_data *data = user_data;

    model_unbind(mod, data->app_idx, data->store);
}

int bt_mesh_provisioner_local_app_key_del(uint16_t net_idx, uint16_t app_idx, bool store)
{
    struct unbind_data data = { .app_idx = app_idx, .store = store };
    struct bt_mesh_app_key *key = NULL;
    int i;

    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", net_idx);
        return -ENODEV;
    }

    if (provisioner_check_app_idx(app_idx, false)) {
        BT_ERR("Invalid AppKeyIndex 0x%04x", app_idx);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && key->net_idx == net_idx &&
                key->app_idx == app_idx) {
            /* Remove the AppKey from the models if they are bound with it */
            bt_mesh_model_foreach(_model_unbind, &data);

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
                bt_mesh_clear_p_app_key(app_idx);
            }

            bt_mesh_free(bt_mesh.p_app_keys[i]);
            bt_mesh.p_app_keys[i] = NULL;
            return 0;
        }
    }

    /* Shall never reach here */
    return -ENODEV;
}

int bt_mesh_provisioner_local_net_key_add(const uint8_t net_key[16], uint16_t *net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    uint8_t p_key[16] = {0};
    int add = -1;
    int err = 0;

    bt_mesh_provisioner_lock();

    if (bt_mesh.p_net_idx_next >= 0x1000) {
        BT_ERR("No NetKeyIndex available");
        err = -EIO;
        goto end;
    }

    if (!net_idx || (*net_idx != 0xFFFF && *net_idx >= 0x1000)) {
        BT_ERR("%s, Invalid parameter", __func__);
        err = -EINVAL;
        goto end;
    }

    /* Check if the same network key already exists */
    if (provisioner_check_net_key(net_key, net_idx)) {
        BT_WARN("NetKey exists, NetKeyIndex updated");
        err = 0;
        goto end;
    }

    /* Check if the same net_idx already exists */
    if (provisioner_check_net_idx(*net_idx, true)) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", *net_idx);
        err = -EEXIST;
        goto end;
    }

    add = provisioner_check_net_key_full();
    if (add < 0) {
        BT_ERR("NetKey is full!");
        err = -ENOMEM;
        goto end;
    }

    if (!net_key) {
        if (bt_mesh_rand(p_key, 16)) {
            BT_ERR("Failed to generate NetKey");
            err = -EIO;
            goto end;
        }
    } else {
        memcpy(p_key, net_key, 16);
    }

    sub = bt_mesh_calloc(sizeof(struct bt_mesh_subnet));
    if (!sub) {
        BT_ERR("%s, Out of memory", __func__);
        err = -ENOMEM;
        goto end;
    }

    if (bt_mesh_net_keys_create(&sub->keys[0], p_key)) {
        BT_ERR("Failed to generate NID");
        bt_mesh_free(sub);
        err = -EIO;
        goto end;
    }

    if (*net_idx != 0xFFFF) {
        sub->net_idx = *net_idx;
    } else {
        sub->net_idx = bt_mesh.p_net_idx_next;
        while (1) {
            if (provisioner_check_net_idx(sub->net_idx, true)) {
                sub->net_idx = (++bt_mesh.p_net_idx_next);
                if (sub->net_idx >= 0x1000) {
                    BT_ERR("No NetKeyIndex available");
                    bt_mesh_free(sub);
                    err = -EIO;
                    goto end;
                }
            } else {
                break;
            }
        }
        *net_idx = sub->net_idx;
        bt_mesh.p_net_idx_next++;
    }
    sub->kr_phase = BLE_MESH_KR_NORMAL;
    sub->kr_flag  = false;
    sub->node_id  = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;

#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_directed_forwarding_sub_init(sub);
#endif

    bt_mesh.p_sub[add] = sub;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_net_idx();
        bt_mesh_store_p_subnet(sub);
    }

end:
    bt_mesh_provisioner_unlock();
    return err;
}

int bt_mesh_provisioner_local_net_key_update(const uint8_t net_key[16], uint16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int err = 0;

    if (net_key == NULL) {
        BT_ERR("Invalid NetKey");
        return -EINVAL;
    }

    BT_INFO("NetKey %s, net_idx 0x%03x", bt_hex(net_key, 16), net_idx);

    sub = bt_mesh_subnet_get(net_idx);
    if (sub == NULL) {
        BT_ERR("NetKey 0x%04x not found", net_idx);
        return -ENODEV;
    }

    err = bt_mesh_net_keys_create(&sub->keys[0], net_key);
    if (err) {
        BT_ERR("Failed to generate NID");
        return -EIO;
    }

    memset(sub->keys[0].net, 0, 16);
    memcpy(sub->keys[0].net, net_key, 16);

    sub->kr_phase = BLE_MESH_KR_NORMAL;
    sub->kr_flag = false;
    sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;

    err = bt_mesh_net_secure_beacon_update(sub);
    if (err) {
        BT_ERR("Failed to update secure beacon");
        return -EIO;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_subnet(sub);
    }

    return 0;
}

int bt_mesh_provisioner_local_net_key_del(uint16_t net_idx, bool store)
{
    struct bt_mesh_subnet *sub = NULL;
    int i, j;

    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", net_idx);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            /* Delete any app keys bound to this NetKey index */
            for (j = 0; j < ARRAY_SIZE(bt_mesh.p_app_keys); j++) {
                struct bt_mesh_app_key *key = bt_mesh.p_app_keys[j];

                if (key && key->net_idx == sub->net_idx) {
                    bt_mesh_provisioner_local_app_key_del(key->net_idx, key->app_idx, store);
                }
            }

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
                bt_mesh_clear_p_subnet(net_idx);
            }

            bt_mesh_free(bt_mesh.p_sub[i]);
            bt_mesh.p_sub[i] = NULL;
            return 0;
        }
    }

    /* Shall never reach here */
    return -ENODEV;
}

int bt_mesh_provisioner_bind_local_model_app_idx(uint16_t elem_addr, uint16_t mod_id,
                                                 uint16_t cid, uint16_t app_idx)
{
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_elem *elem = NULL;
    int i;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        BT_ERR("No element found, addr 0x%04x", elem_addr);
        return -ENODEV;
    }

    if (cid == 0xFFFF) {
        model = bt_mesh_model_find(elem, mod_id);
    } else {
        model = bt_mesh_model_find_vnd(elem, cid, mod_id);
    }
    if (!model) {
        BT_ERR("No model found, model id 0x%04x, cid 0x%04x", mod_id, cid);
        return -ENODEV;
    }

    if (provisioner_check_app_idx(app_idx, false)) {
        BT_ERR("Invalid AppKeyIndex 0x%04x", app_idx);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] == app_idx) {
            BT_INFO("AppKey 0x%03x already bound to model", app_idx);
            return 0;
        }
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] == BLE_MESH_KEY_UNUSED) {
            model->keys[i] = app_idx;

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_mod_bind(model);
            }
            return 0;
        }
    }

    BT_ERR("Model bound is full!");
    return -ENOMEM;
}

#if CONFIG_BLE_MESH_TEST_AUTO_ENTER_NETWORK
int bt_mesh_provisioner_store_node_info(struct bt_mesh_node *node)
{
    int err = 0;

    if (!node) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (!BLE_MESH_ADDR_IS_UNICAST(node->unicast_addr)) {
        BT_ERR("Invalid unicast address 0x%04x", node->unicast_addr);
        return -EINVAL;
    }

    if (node->element_num == 0) {
        BT_ERR("Invalid element count %d", node->element_num);
        return -EINVAL;
    }

    if (bt_mesh_provisioner_check_is_addr_dup(node->unicast_addr, node->element_num, true)) {
        BT_ERR("Duplicate unicast address 0x%04x", node->unicast_addr);
        return -EINVAL;
    }

    if (node->unicast_addr + node->element_num - 1 > 0x7FFF) {
        BT_ERR("Not enough unicast address for the node");
        return -EIO;
    }

    if (bt_mesh_provisioner_net_key_get(node->net_idx) == NULL) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", node->net_idx);
        return -EINVAL;
    }

    err = provisioner_store_node(node, true, NULL);
    if (err) {
        BT_ERR("Failed to store node info");
        return err;
    }

    bt_mesh_test_provisioner_update_alloc_addr(node->unicast_addr, node->element_num);
    return 0;
}
#endif /* CONFIG_BLE_MESH_TEST_AUTO_ENTER_NETWORK */

#endif /* CONFIG_BLE_MESH_PROVISIONER */
