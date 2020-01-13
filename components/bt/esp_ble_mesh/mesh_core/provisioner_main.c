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

#include "mesh.h"
#include "crypto.h"
#include "adv.h"
#include "access.h"
#include "settings.h"
#include "friend.h"
#include "mesh_common.h"
#include "proxy_client.h"
#include "provisioner_prov.h"
#include "provisioner_main.h"

#if CONFIG_BLE_MESH_PROVISIONER

static struct bt_mesh_node *mesh_nodes[CONFIG_BLE_MESH_MAX_STORED_NODES];
static bt_mesh_mutex_t provisioner_lock;
static u16_t all_node_count;
static u16_t prov_node_count;

static int provisioner_remove_node(u16_t index, bool erase);

static void bt_mesh_provisioner_mutex_new(void)
{
    if (!provisioner_lock.mutex) {
        bt_mesh_mutex_create(&provisioner_lock);
    }
}

static void bt_mesh_provisioner_mutex_free(void)
{
    bt_mesh_mutex_free(&provisioner_lock);
}

static void bt_mesh_provisioner_lock(void)
{
    bt_mesh_mutex_lock(&provisioner_lock);
}

static void bt_mesh_provisioner_unlock(void)
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
    const struct bt_mesh_prov *prov = NULL;
    struct bt_mesh_subnet *sub = NULL;
    u8_t p_key[16] = {0};

    BT_DBG("%s", __func__);

    prov = bt_mesh_provisioner_get_prov_info();
    if (!prov) {
        BT_ERR("%s, NULL provisioning context", __func__);
        return -EINVAL;
    }

    /* If the device only acts as a Provisioner, need to initialize
     * each element's address.
     */
    bt_mesh_comp_provision(bt_mesh_provisioner_get_primary_elem_addr());

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
        BT_ERR("%s, Failed to generate Primary NetKey", __func__);
        return -EIO;
    }

    sub = bt_mesh_calloc(sizeof(struct bt_mesh_subnet));
    if (!sub) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    sub->kr_flag = BLE_MESH_KEY_REFRESH(prov->flags);
    if (sub->kr_flag) {
        if (bt_mesh_net_keys_create(&sub->keys[1], p_key)) {
            BT_ERR("%s, Failed to generate net-related keys", __func__);
            bt_mesh_free(sub);
            return -EIO;
        }
        sub->kr_phase = BLE_MESH_KR_PHASE_2;
    } else {
        /* Currently provisioner only use keys[0] */
        if (bt_mesh_net_keys_create(&sub->keys[0], p_key)) {
            BT_ERR("%s, Failed to create net-related keys", __func__);
            bt_mesh_free(sub);
            return -EIO;
        }
        sub->kr_phase = BLE_MESH_KR_NORMAL;
    }
    sub->net_idx = BLE_MESH_KEY_PRIMARY;
    sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;

    bt_mesh.p_sub[0] = sub;

    /* Dynamically added appkey & netkey will use these key_idx */
    bt_mesh.p_app_idx_next = 0x0000;
    bt_mesh.p_net_idx_next = 0x0001;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_net_idx();
        bt_mesh_store_p_app_idx();
        bt_mesh_store_p_subnet(bt_mesh.p_sub[0]);
    }

    bt_mesh.iv_index = prov->iv_index;
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS,
                              BLE_MESH_IV_UPDATE(prov->flags));

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
    BT_INFO("net_idx 0x%03x, netkey %s, nid 0x%02x",
        sub->net_idx, bt_hex(sub->keys[0].net, 16), sub->keys[0].nid);

    return 0;
}

int bt_mesh_provisioner_deinit(bool erase)
{
    int i;

    for (i = 0; i < CONFIG_BLE_MESH_PROVISIONER_SUBNET_COUNT; i++) {
        if (bt_mesh.p_sub[i]) {
            if (erase && IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_clear_p_subnet(bt_mesh.p_sub[i]);
            }
            bt_mesh_free(bt_mesh.p_sub[i]);
            bt_mesh.p_sub[i] = NULL;
        }
    }

    for (i = 0; i < CONFIG_BLE_MESH_PROVISIONER_APP_KEY_COUNT; i++) {
        if (bt_mesh.p_app_keys[i]) {
            if (erase && IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_clear_p_app_key(bt_mesh.p_app_keys[i]);
            }
            bt_mesh_free(bt_mesh.p_app_keys[i]);
            bt_mesh.p_app_keys[i] = NULL;
        }
    }

    bt_mesh.p_net_idx_next = 0U;
    bt_mesh.p_app_idx_next = 0U;
    if (erase && IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_p_net_idx();
        bt_mesh_clear_p_app_idx();
    }

    for (i = 0; i < CONFIG_BLE_MESH_MAX_STORED_NODES; i++) {
        provisioner_remove_node(i, erase);
    }

    all_node_count = 0U;
    prov_node_count = 0U;

    bt_mesh_provisioner_mutex_free();

    return 0;
}

bool bt_mesh_provisioner_check_is_addr_dup(u16_t addr, u8_t elem_num, bool comp_with_own)
{
    const struct bt_mesh_comp *comp = NULL;
    struct bt_mesh_node *node = NULL;
    u16_t primary_addr = BLE_MESH_ADDR_UNASSIGNED;
    u16_t comp_addr = BLE_MESH_ADDR_UNASSIGNED;
    int i;

    if (comp_with_own) {
        comp = bt_mesh_comp_get();
        if (!comp) {
            BT_ERR("NULL composition data");
            return true;
        }

        primary_addr = bt_mesh_provisioner_get_primary_elem_addr();
        if (!BLE_MESH_ADDR_IS_UNICAST(primary_addr)) {
            BT_ERR("%s, Not a unicast address 0x%04x", __func__, primary_addr);
            return true;
        }
    }

    for (comp_addr = addr; comp_addr < addr + elem_num; comp_addr++) {
        for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
            node = mesh_nodes[i];
            if (node && comp_addr >= node->unicast_addr &&
                    comp_addr < node->unicast_addr + node->element_num) {
                BT_ERR("Duplicate with node address 0x%04x", comp_addr);
                return true;
            }

            if (comp_with_own && comp_addr >= primary_addr &&
                    comp_addr < primary_addr + comp->elem_count) {
                BT_ERR("Duplicate with Provisioner address 0x%04x", comp_addr);
                return true;
            }
        }
    }

    return false;
}

static void provisioner_node_count_inc(bool prov)
{
    all_node_count++;
    if (prov) {
        prov_node_count++;
    }
}

static void provisioner_node_count_dec(bool prov)
{
    if (all_node_count) {
        all_node_count--;
    }
    if (prov) {
        if (prov_node_count) {
            prov_node_count--;
        }
    }
}

u16_t bt_mesh_provisioner_get_prov_node_count(void)
{
    return prov_node_count;
}

u16_t bt_mesh_provisioner_get_all_node_count(void)
{
    return all_node_count;
}

static int provisioner_store_node(struct bt_mesh_node *node, bool prov, bool store, u16_t *index)
{
    u16_t min = 0U, max = 0U;
    size_t i = 0U;

    bt_mesh_provisioner_lock();

    /* Check if the node already exists */
    for (i = 0U; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i] && !memcmp(mesh_nodes[i]->dev_uuid, node->dev_uuid, 16)) {
            BT_WARN("Node already exists, uuid %s", bt_hex(node->dev_uuid, 16));
            bt_mesh_provisioner_unlock();
            return -EEXIST;
        }
    }

    /**
     * 0 ~ (CONFIG_BLE_MESH_MAX_PROV_NODES - 1) are used to store
     * the information of self-provisioned nodes.
     */
    if (prov) {
        min = 0U;
        max = CONFIG_BLE_MESH_MAX_PROV_NODES;
    } else {
        min = CONFIG_BLE_MESH_MAX_PROV_NODES;
        max = ARRAY_SIZE(mesh_nodes);
    }

    for (i = min; i < max; i++) {
        if (mesh_nodes[i] == NULL) {
            mesh_nodes[i] = bt_mesh_calloc(sizeof(struct bt_mesh_node));
            if (!mesh_nodes[i]) {
                BT_ERR("%s, Failed to allocate memory", __func__);
                bt_mesh_provisioner_unlock();
                return -ENOMEM;
            }

            memcpy(mesh_nodes[i], node, sizeof(struct bt_mesh_node));
            provisioner_node_count_inc(prov);
            if (index) {
                *index = i;
            }

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS) && store) {
                bt_mesh_store_node_info(mesh_nodes[i], prov);
            }

            bt_mesh_provisioner_unlock();
            return 0;
        }
    }

    BT_ERR("%s, Node queue is full", __func__);
    bt_mesh_provisioner_unlock();
    return -ENOMEM;
}

int bt_mesh_provisioner_restore_node_info(struct bt_mesh_node *node, bool prov)
{
    if (!node) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    return provisioner_store_node(node, prov, false, NULL);
}

int bt_mesh_provisioner_provision(const bt_mesh_addr_t *addr, const u8_t uuid[16], u16_t oob_info,
                                  u16_t unicast_addr, u8_t element_num, u16_t net_idx, u8_t flags,
                                  u32_t iv_index, const u8_t dev_key[16], u16_t *index)
{
    struct bt_mesh_node node = {0};

    BT_DBG("%s", __func__);

    if (!addr || !uuid || !dev_key || !index) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_INFO("unicast_addr 0x%04x, elem_num %d, net_idx 0x%04x",
        unicast_addr, element_num, net_idx);
    BT_INFO("dev_uuid %s", bt_hex(uuid, 16));
    BT_INFO("dev_key %s", bt_hex(dev_key, 16));

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

    return provisioner_store_node(&node, true, true, index);
}

static int provisioner_remove_node(u16_t index, bool erase)
{
    struct bt_mesh_node *node = NULL;
    struct bt_mesh_rpl *rpl = NULL;
    bool is_prov = false;
    int i;

    BT_DBG("%s, reset node %d", __func__, index);

    bt_mesh_provisioner_lock();

    if (mesh_nodes[index] == NULL) {
        bt_mesh_provisioner_unlock();
        return 0;
    }

    node = mesh_nodes[index];

    /* Reset corresponding network cache when reset the node */
    bt_mesh_msg_cache_clear(node->unicast_addr, node->element_num);

    /* Reset corresponding rpl when removing the node */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        rpl = &bt_mesh.rpl[i];
        if (rpl->src >= node->unicast_addr &&
                rpl->src < node->unicast_addr + node->element_num) {
            memset(rpl, 0, sizeof(struct bt_mesh_rpl));

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_clear_rpl_single(node->unicast_addr);
            }
        }
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_FRIEND)) {
        bt_mesh_friend_remove_lpn(node->unicast_addr);
    }

    is_prov = index < CONFIG_BLE_MESH_MAX_PROV_NODES ? true : false;

    if (erase && IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_clear_node_info(node->unicast_addr, is_prov);
    }

    if (mesh_nodes[index]->comp_data) {
        bt_mesh_free(mesh_nodes[index]->comp_data);
    }
    bt_mesh_free(mesh_nodes[index]);
    mesh_nodes[index] = NULL;

    provisioner_node_count_dec(is_prov);

    bt_mesh_provisioner_unlock();
    return 0;
}

static struct bt_mesh_node *provisioner_find_node_with_uuid(const u8_t uuid[16], u16_t *index)
{
    int i;

    BT_DBG("%s", __func__);

    if (uuid == NULL) {
        BT_ERR("%s, Invalid device uuid", __func__);
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

bool bt_mesh_provisioner_find_node_with_uuid(const u8_t uuid[16], bool reset)
{
    struct bt_mesh_node *node = NULL;
    u16_t index = 0U;

    node = provisioner_find_node_with_uuid(uuid, &index);
    if (!node) {
        return false;
    }

    if (reset) {
        provisioner_remove_node(index, true);
    }
    return true;
}

bool bt_mesh_provisioner_find_node_with_addr(const bt_mesh_addr_t *addr, bool reset)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i]) {
            if (!memcmp(mesh_nodes[i]->addr, addr->val, BLE_MESH_ADDR_LEN) &&
                mesh_nodes[i]->addr_type == addr->type) {
                if (reset) {
                    provisioner_remove_node(i, true);
                }
                return true;
            }
        }
    }

    return false;
}

int bt_mesh_provisioner_remove_node(const u8_t uuid[16])
{
    struct bt_mesh_node *node = NULL;
    u16_t index = 0U;
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
        BT_WARN("Node %s not exist", bt_hex(uuid, 16));
        return -ENODEV;
    }

    provisioner_remove_node(index, true);
    return 0;
}

static struct bt_mesh_node *provisioner_find_node_with_addr(u16_t addr, u16_t *index)
{
    struct bt_mesh_node *node = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("%s, Not a unicast address 0x%04x", __func__, addr);
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

int bt_mesh_provisioner_restore_node_name(u16_t addr, const char *name)
{
    struct bt_mesh_node *node = NULL;

    node = provisioner_find_node_with_addr(addr, NULL);
    if (node == NULL) {
        BT_ERR("%s, Node 0x%04x not exist", __func__, addr);
        return -ENODEV;
    }

    strncpy(node->name, name, BLE_MESH_NODE_NAME_SIZE);
    return 0;
}

int bt_mesh_provisioner_restore_node_comp_data(u16_t addr, const u8_t *data, u16_t length, bool prov)
{
    struct bt_mesh_node *node = NULL;

    if (!data || length == 0U) {
        BT_ERR("%s, Invalid comp data info", __func__);
        return -EINVAL;
    }

    node = provisioner_find_node_with_addr(addr, NULL);
    if (node == NULL) {
        BT_ERR("%s, Node 0x%04x not exist", __func__, addr);
        return -ENODEV;
    }

    node->comp_data = bt_mesh_calloc(length);
    if (!node->comp_data) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    node->comp_length = length;
    memcpy(node->comp_data, data, length);

    return 0;
}

int bt_mesh_provisioner_store_node_info(struct bt_mesh_node *node)
{
    if (!node) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    return provisioner_store_node(node, false, true, NULL);
}

struct bt_mesh_node *bt_mesh_provisioner_get_node_with_uuid(const u8_t uuid[16])
{
    return provisioner_find_node_with_uuid(uuid, NULL);
}

struct bt_mesh_node *bt_mesh_provisioner_get_node_with_addr(u16_t unicast_addr)
{
    return provisioner_find_node_with_addr(unicast_addr, NULL);
}

int bt_mesh_provisioner_delete_node_with_uuid(const u8_t uuid[16])
{
    struct bt_mesh_node *node = NULL;
    u16_t index = 0U;

    node = provisioner_find_node_with_uuid(uuid, &index);
    if (!node) {
        BT_WARN("Node %s not exist", bt_hex(uuid, 16));
        return -ENODEV;
    }

    provisioner_remove_node(index, true);
    return 0;
}

int bt_mesh_provisioner_delete_node_with_addr(u16_t unicast_addr)
{
    struct bt_mesh_node *node = NULL;
    u16_t index = 0U;

    node = provisioner_find_node_with_addr(unicast_addr, &index);
    if (!node) {
        BT_WARN("Node 0x%04x not exist", unicast_addr);
        return -ENODEV;
    }

    provisioner_remove_node(index, true);
    return 0;
}

static int provisioner_check_node_index(u16_t index)
{
    BT_DBG("%s", __func__);

    if (index >= ARRAY_SIZE(mesh_nodes)) {
        BT_ERR("%s, Too big node index %d", __func__, index);
        return -EINVAL;
    }

    if (mesh_nodes[index] == NULL) {
        BT_ERR("%s, Node %d is not found", __func__, index);
        return -ENODEV;
    }

    return 0;
}

int bt_mesh_provisioner_set_node_name(u16_t index, const char *name)
{
    size_t length = 0U, name_len = 0U;
    int i;

    BT_DBG("%s", __func__);

    if (!name) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (provisioner_check_node_index(index)) {
        BT_ERR("%s, Failed to check node index", __func__);
        return -EINVAL;
    }

    BT_DBG("name len is %d, name is %s", strlen(name), name);

    length = (strlen(name) <= BLE_MESH_NODE_NAME_SIZE) ? strlen(name) : BLE_MESH_NODE_NAME_SIZE;
    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i]) {
            name_len = strlen(mesh_nodes[i]->name);
            if (length != name_len) {
                continue;
            }
            if (!strncmp(mesh_nodes[i]->name, name, length)) {
                BT_WARN("Node name %s already exists", name);
                return -EEXIST;
            }
        }
    }

    memset(mesh_nodes[index]->name, 0, BLE_MESH_NODE_NAME_SIZE);

    strncpy(mesh_nodes[index]->name, name, length);
    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_node_name(mesh_nodes[index],
            index < CONFIG_BLE_MESH_MAX_PROV_NODES ? true : false);
    }

    return 0;
}

const char *bt_mesh_provisioner_get_node_name(u16_t index)
{
    BT_DBG("%s", __func__);

    if (provisioner_check_node_index(index)) {
        BT_ERR("%s, Failed to check node index", __func__);
        return NULL;
    }

    return mesh_nodes[index]->name;
}

u16_t bt_mesh_provisioner_get_node_index(const char *name)
{
    size_t length = 0U, name_len = 0U;
    int i;

    BT_DBG("%s", __func__);

    if (!name) {
        BT_ERR("%s, Invalid parameter", __func__);
        return BLE_MESH_INVALID_NODE_INDEX;
    }

    length = (strlen(name) <= BLE_MESH_NODE_NAME_SIZE) ? strlen(name) : BLE_MESH_NODE_NAME_SIZE;
    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (mesh_nodes[i]) {
            name_len = strlen(mesh_nodes[i]->name);
            if (length != name_len) {
                continue;
            }
            if (!strncmp(mesh_nodes[i]->name, name, length)) {
                return (u16_t)i;
            }
        }
    }

    BT_ERR("%s, Node name %s not exist", __func__, name);
    return BLE_MESH_INVALID_NODE_INDEX;
}

int bt_mesh_provisioner_store_node_comp_data(u16_t addr, const u8_t *data, u16_t length)
{
    struct bt_mesh_node *node = NULL;
    u16_t index = 0U;

    if (!BLE_MESH_ADDR_IS_UNICAST(addr) || !data ||
            (length % 2) || length <= 14) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    node = provisioner_find_node_with_addr(addr, &index);
    if (node == NULL) {
        BT_ERR("%s, Node 0x%04x not exist", __func__, addr);
        return -ENODEV;
    }

    node->comp_data = bt_mesh_calloc(length);
    if (node->comp_data == NULL) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    BT_DBG("%s, index %d", __func__, index);

    memcpy(node->comp_data, data, length);
    node->comp_length = length;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_node_comp_data(node,
            index < CONFIG_BLE_MESH_MAX_PROV_NODES ? true : false);
    }

    return 0;
}

/* Provisioner DevKey, NetKey and AppKey related functions */

const u8_t *bt_mesh_provisioner_net_key_get(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            if (sub->kr_flag) {
                return sub->keys[1].net;
            } else {
                return sub->keys[0].net;
            }
        }
    }

    return NULL;
}

struct bt_mesh_subnet *bt_mesh_provisioner_subnet_get(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (net_idx == BLE_MESH_KEY_ANY) {
        return bt_mesh.p_sub[0];
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            return sub;
        }
    }

    return NULL;
}

bool bt_mesh_provisioner_check_msg_dst(u16_t dst)
{
    struct bt_mesh_node *node = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        return true;
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && dst >= node->unicast_addr &&
                dst < node->unicast_addr + node->element_num) {
            return true;
        }
    }

    return false;
}

const u8_t *bt_mesh_provisioner_dev_key_get(u16_t dst)
{
    /* Device key is only used to encrypt configuration messages.
    *  Configuration model shall only be supported by the primary
    *  element which uses the primary unicast address.
    */
    struct bt_mesh_node *node = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        BT_ERR("%s, Not a unicast address 0x%04x", __func__, dst);
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && node->unicast_addr == dst) {
            return node->dev_key;
        }
    }

    return NULL;
}

struct bt_mesh_app_key *bt_mesh_provisioner_app_key_find(u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && key->net_idx != BLE_MESH_KEY_UNUSED &&
                key->app_idx == app_idx) {
            return key;
        }
    }

    return NULL;
}

static int provisioner_check_app_key(const u8_t app_key[16], u16_t *app_idx)
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

static int provisioner_check_app_idx(u16_t app_idx, bool exist)
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

static int provisioner_check_net_key(const u8_t net_key[16], u16_t *net_idx)
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

static int provisioner_check_net_idx(u16_t net_idx, bool exist)
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

int bt_mesh_provisioner_local_app_key_add(const u8_t app_key[16], u16_t net_idx, u16_t *app_idx)
{
    struct bt_mesh_app_keys *keys = NULL;
    struct bt_mesh_app_key *key = NULL;
    u8_t p_key[16] = {0};
    int add = -1;

    if (bt_mesh.p_app_idx_next >= 0x1000) {
        BT_ERR("%s, No AppKey Index available", __func__);
        return -EIO;
    }

    if (!app_idx || (*app_idx != 0xFFFF && *app_idx >= 0x1000)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Check if the same application key already exists */
    if (provisioner_check_app_key(app_key, app_idx)) {
        BT_WARN("AppKey already exists, AppKey Index updated");
        return 0;
    }

    /* Check if the net_idx exists */
    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("%s, NetKey Index does not exist", __func__);
        return -ENODEV;
    }

    /* Check if the same app_idx already exists */
    if (provisioner_check_app_idx(*app_idx, true)) {
        BT_ERR("%s, AppKey Index already exists", __func__);
        return -EEXIST;
    }

    add = provisioner_check_app_key_full();
    if (add < 0) {
        BT_ERR("%s, AppKey queue is full", __func__);
        return -ENOMEM;
    }

    if (!app_key) {
        if (bt_mesh_rand(p_key, 16)) {
            BT_ERR("%s, Failed to generate AppKey", __func__);
            return -EIO;
        }
    } else {
        memcpy(p_key, app_key, 16);
    }

    key = bt_mesh_calloc(sizeof(struct bt_mesh_app_key));
    if (!key) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    keys = &key->keys[0];
    if (bt_mesh_app_id(p_key, &keys->id)) {
        BT_ERR("%s, Failed to generate AID", __func__);
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
                    BT_ERR("%s, No AppKey Index available", __func__);
                    bt_mesh_free(key);
                    return -EIO;
                }
            } else {
                break;
            }
        }
        *app_idx = key->app_idx;
    }
    key->updated = false;

    bt_mesh.p_app_keys[add] = key;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_app_idx();
        bt_mesh_store_p_app_key(key);
    }

    return 0;
}

int bt_mesh_provisioner_local_app_key_update(const u8_t app_key[16], u16_t net_idx, u16_t app_idx)
{
    struct bt_mesh_app_keys *keys = NULL;
    struct bt_mesh_app_key *key = NULL;

    if (app_key == NULL) {
        BT_ERR("%s, Invalid AppKey", __func__);
        return -EINVAL;
    }

    BT_INFO("AppKey %s, net_idx 0x%03x, app_idx 0x%03x", bt_hex(app_key, 16), net_idx, app_idx);

    /* Check if the net_idx exists */
    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("%s, NetKey Index 0x%03x not exist", __func__, net_idx);
        return -ENODEV;
    }

    key = bt_mesh_provisioner_app_key_find(app_idx);
    if (key == NULL) {
        BT_ERR("%s, AppKey 0x%03x not exist", __func__, app_idx);
        return -ENODEV;
    }

    keys = &key->keys[0];
    if (bt_mesh_app_id(app_key, &keys->id)) {
        BT_ERR("%s, Failed to generate AID", __func__);
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

const u8_t *bt_mesh_provisioner_local_app_key_get(u16_t net_idx, u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("%s, NetKey Index does not exist", __func__);
        return NULL;
    }

    if (provisioner_check_app_idx(app_idx, false)) {
        BT_ERR("%s, AppKey Index does not exist", __func__);
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

static void model_pub_clear(struct bt_mesh_model *model)
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

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_mod_pub(model);
    }

    return;
}

static void model_unbind(struct bt_mesh_model *model, u16_t app_idx)
{
    int i;

    BT_DBG("model %p key_idx 0x%03x", model, app_idx);

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] != app_idx) {
            continue;
        }

        model->keys[i] = BLE_MESH_KEY_UNUSED;

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_mod_bind(model);
        }

        model_pub_clear(model);
    }
}

static void _model_unbind(struct bt_mesh_model *mod, struct bt_mesh_elem *elem,
                          bool vnd, bool primary, void *user_data)
{
    u16_t app_idx = *(u16_t *)user_data;

    model_unbind(mod, app_idx);
}

int bt_mesh_provisioner_local_app_key_delete(u16_t net_idx, u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("%s, NetKey Index does not exist", __func__);
        return -ENODEV;
    }

    if (provisioner_check_app_idx(app_idx, false)) {
        BT_ERR("%s, AppKey Index does not exist", __func__);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && key->net_idx == net_idx &&
                key->app_idx == app_idx) {
            /* Remove the AppKey from the models if they are bound with it */
            bt_mesh_model_foreach(_model_unbind, &app_idx);

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_clear_p_app_key(key);
            }

            bt_mesh_free(bt_mesh.p_app_keys[i]);
            bt_mesh.p_app_keys[i] = NULL;
            return 0;
        }
    }

    /* Shall never reach here */
    return -ENODEV;
}

int bt_mesh_provisioner_local_net_key_add(const u8_t net_key[16], u16_t *net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    u8_t p_key[16] = {0};
    int add = -1;

    if (bt_mesh.p_net_idx_next >= 0x1000) {
        BT_ERR("%s, No NetKey Index available", __func__);
        return -EIO;
    }

    if (!net_idx || (*net_idx != 0xFFFF && *net_idx >= 0x1000)) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Check if the same network key already exists */
    if (provisioner_check_net_key(net_key, net_idx)) {
        BT_WARN("NetKey already exists, NetKey Index updated");
        return 0;
    }

    /* Check if the same net_idx already exists */
    if (provisioner_check_net_idx(*net_idx, true)) {
        BT_ERR("%s, NetKey Index already exists", __func__);
        return -EEXIST;
    }

    add = provisioner_check_net_key_full();
    if (add < 0) {
        BT_ERR("%s, NetKey queue is full", __func__);
        return -ENOMEM;
    }

    if (!net_key) {
        if (bt_mesh_rand(p_key, 16)) {
            BT_ERR("%s, Failed to generate NetKey", __func__);
            return -EIO;
        }
    } else {
        memcpy(p_key, net_key, 16);
    }

    sub = bt_mesh_calloc(sizeof(struct bt_mesh_subnet));
    if (!sub) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    if (bt_mesh_net_keys_create(&sub->keys[0], p_key)) {
        BT_ERR("%s, Failed to generate NID", __func__);
        bt_mesh_free(sub);
        return -EIO;
    }

    if (*net_idx != 0xFFFF) {
        sub->net_idx = *net_idx;
    } else {
        sub->net_idx = bt_mesh.p_net_idx_next;
        while (1) {
            if (provisioner_check_net_idx(sub->net_idx, true)) {
                sub->net_idx = (++bt_mesh.p_net_idx_next);
                if (sub->net_idx >= 0x1000) {
                    BT_ERR("%s, No NetKey Index available", __func__);
                    bt_mesh_free(sub);
                    return -EIO;
                }
            } else {
                break;
            }
        }
        *net_idx = sub->net_idx;
    }
    sub->kr_phase = BLE_MESH_KR_NORMAL;
    sub->kr_flag  = false;
    sub->node_id  = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;

    bt_mesh.p_sub[add] = sub;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_net_idx();
        bt_mesh_store_p_subnet(sub);
    }

    return 0;
}

int bt_mesh_provisioner_local_net_key_update(const u8_t net_key[16], u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int err = 0;

    if (net_key == NULL) {
        BT_ERR("%s, Invalid NetKey", __func__);
        return -EINVAL;
    }

    BT_INFO("NetKey %s, net_idx 0x%03x", bt_hex(net_key, 16), net_idx);

    sub = bt_mesh_provisioner_subnet_get(net_idx);
    if (sub == NULL) {
        BT_ERR("%s, NetKey 0x%03x not exist", __func__, net_idx);
        return -ENODEV;
    }

    err = bt_mesh_net_keys_create(&sub->keys[0], net_key);
    if (err) {
        BT_ERR("%s, Failed to generate NID", __func__);
        return -EIO;
    }

    memset(sub->keys[0].net, 0, 16);
    memcpy(sub->keys[0].net, net_key, 16);

    sub->kr_phase = BLE_MESH_KR_NORMAL;
    sub->kr_flag = false;
    sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;

    err = bt_mesh_net_beacon_update(sub);
    if (err) {
        BT_ERR("%s, Failed to update secure beacon", __func__);
        return -EIO;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_p_subnet(sub);
    }

    return 0;
}

const u8_t *bt_mesh_provisioner_local_net_key_get(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("%s, NetKey Index does not exist", __func__);
        return NULL;
    }

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

int bt_mesh_provisioner_local_net_key_delete(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i, j;

    BT_DBG("%s", __func__);

    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("%s, NetKey Index does not exist", __func__);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            /* Delete any app keys bound to this NetKey index */
            for (j = 0; j < ARRAY_SIZE(bt_mesh.p_app_keys); j++) {
                struct bt_mesh_app_key *key = bt_mesh.p_app_keys[j];
                if (key->net_idx == sub->net_idx) {
                    bt_mesh_provisioner_local_app_key_delete(key->net_idx, key->app_idx);
                }
            }

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_clear_p_subnet(sub);
            }

            bt_mesh_free(bt_mesh.p_sub[i]);
            bt_mesh.p_sub[i] = NULL;
            return 0;
        }
    }

    /* Shall never reach here */
    return -ENODEV;
}

int bt_mesh_provisioner_bind_local_model_app_idx(u16_t elem_addr, u16_t mod_id,
        u16_t cid, u16_t app_idx)
{
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_elem *elem = NULL;
    int i;

    elem = bt_mesh_elem_find(elem_addr);
    if (!elem) {
        BT_ERR("%s, No element is found", __func__);
        return -ENODEV;
    }

    if (cid == 0xFFFF) {
        model = bt_mesh_model_find(elem, mod_id);
    } else {
        model = bt_mesh_model_find_vnd(elem, cid, mod_id);
    }
    if (!model) {
        BT_ERR("%s, No model is found", __func__);
        return -ENODEV;
    }

    if (provisioner_check_app_idx(app_idx, false)) {
        BT_ERR("%s, AppKey Index does not exist", __func__);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] == app_idx) {
            BT_WARN("AppKey 0x%03x is already bound to model", app_idx);
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

    BT_ERR("%s, Model AppKey queue is full", __func__);
    return -ENOMEM;
}

int bt_mesh_print_local_composition_data(void)
{
    const struct bt_mesh_comp *comp = NULL;
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_elem *elem = NULL;
    int i, j;

    comp = bt_mesh_comp_get();
    if (!comp) {
        BT_ERR("%s, NULL composition data", __func__);
        return -EINVAL;
    }

    BT_INFO("************************************************");
    BT_INFO("* cid: 0x%04x    pid: 0x%04x    vid: 0x%04x    *", comp->cid, comp->pid, comp->vid);
    BT_INFO("* Element Number: 0x%02x                         *", comp->elem_count);
    for (i = 0; i < comp->elem_count; i++) {
        elem = &comp->elem[i];
        BT_INFO("* Element %d: 0x%04x                            *", i, elem->addr);
        BT_INFO("*     Loc: 0x%04x   NumS: 0x%02x   NumV: 0x%02x    *", elem->loc, elem->model_count, elem->vnd_model_count);
        for (j = 0; j < elem->model_count; j++) {
            model = &elem->models[j];
            BT_INFO("*     sig_model %d: id - 0x%04x                 *", j, model->id);
        }
        for (j = 0; j < elem->vnd_model_count; j++) {
            model = &elem->vnd_models[j];
            BT_INFO("*     vnd_model %d: id - 0x%04x, cid - 0x%04x   *", j, model->vnd.id, model->vnd.company);
        }
    }
    BT_INFO("************************************************");

    ((void) model);

    return 0;
}

#endif /* CONFIG_BLE_MESH_PROVISIONER */

/* The following APIs are for fast provisioning */

#if CONFIG_BLE_MESH_FAST_PROV

const u8_t *bt_mesh_fast_prov_dev_key_get(u16_t dst)
{
    struct bt_mesh_node *node = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        BT_ERR("%s, Not a unicast address 0x%04x", __func__, dst);
        return NULL;
    }

    if (dst == bt_mesh_primary_addr()) {
        return bt_mesh.dev_key;
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && node->unicast_addr == dst) {
            return node->dev_key;
        }
    }

    return NULL;
}

struct bt_mesh_subnet *bt_mesh_fast_prov_subnet_get(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        sub = &bt_mesh.sub[i];
        if (sub->net_idx == net_idx) {
            return sub;
        }
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            return sub;
        }
    }

    return NULL;
}

struct bt_mesh_app_key *bt_mesh_fast_prov_app_key_find(u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        key = &bt_mesh.app_keys[i];
        if (key->net_idx != BLE_MESH_KEY_UNUSED &&
                key->app_idx == app_idx) {
            return key;
        }
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && key->net_idx != BLE_MESH_KEY_UNUSED &&
                key->app_idx == app_idx) {
            return key;
        }
    }

    return NULL;
}

u8_t bt_mesh_set_fast_prov_net_idx(u16_t net_idx)
{
    struct bt_mesh_subnet_keys *key = NULL;
    struct bt_mesh_subnet *sub = NULL;

    sub = bt_mesh_fast_prov_subnet_get(net_idx);
    if (sub) {
        key = BLE_MESH_KEY_REFRESH(sub->kr_flag) ? &sub->keys[1] : &sub->keys[0];
        return bt_mesh_provisioner_set_fast_prov_net_idx(key->net, net_idx);
    }

    /* If net_idx is not found, set net_idx to fast_prov first,
     * and wait for primary provisioner to add net_key */
    return bt_mesh_provisioner_set_fast_prov_net_idx(NULL, net_idx);
}

u8_t bt_mesh_add_fast_prov_net_key(const u8_t net_key[16])
{
    const u8_t *keys = NULL;
    u16_t net_idx = 0U;
    int err = 0;

    net_idx = bt_mesh_provisioner_get_fast_prov_net_idx();
    bt_mesh.p_net_idx_next = net_idx;

    err = bt_mesh_provisioner_local_net_key_add(net_key, &net_idx);
    if (err) {
        return 0x01; /* status: add net_key fail */
    };

    keys = bt_mesh_provisioner_local_net_key_get(net_idx);
    if (!keys) {
        return 0x01; /* status: add net_key fail */
    }

    return bt_mesh_provisioner_set_fast_prov_net_idx(keys, net_idx);
}

const u8_t *bt_mesh_get_fast_prov_net_key(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;

    sub = bt_mesh_fast_prov_subnet_get(net_idx);
    if (!sub) {
        BT_ERR("%s, Failed to get subnet", __func__);
        return NULL;
    }

    return (sub->kr_flag ? sub->keys[1].net : sub->keys[0].net);
}

const u8_t *bt_mesh_get_fast_prov_app_key(u16_t net_idx, u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;

    key = bt_mesh_fast_prov_app_key_find(app_idx);
    if (!key) {
        BT_ERR("%s, Failed to get AppKey", __func__);
        return NULL;
    }

    return (key->updated ? key->keys[1].val : key->keys[0].val);
}

#endif /* CONFIG_BLE_MESH_FAST_PROV */
