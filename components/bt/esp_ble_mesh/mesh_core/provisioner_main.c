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

#include "sdkconfig.h"
#include "osi/allocator.h"

#include "mesh_util.h"
#include "mesh_main.h"
#include "mesh_trace.h"
#include "mesh_bearer_adapt.h"

#include "mesh.h"
#include "crypto.h"
#include "adv.h"
#include "net.h"
#include "access.h"

#include "provisioner_prov.h"
#include "proxy_client.h"
#include "provisioner_main.h"

#if CONFIG_BLE_MESH_PROVISIONER

static const struct bt_mesh_prov *prov;
static const struct bt_mesh_comp *comp;

static struct bt_mesh_node_t *mesh_nodes[CONFIG_BLE_MESH_MAX_STORED_NODES];
static u32_t mesh_node_count;

static bool prov_upper_init = false;

static int provisioner_index_check(int node_index)
{
    struct bt_mesh_node_t *node = NULL;

    BT_DBG("%s", __func__);

    if (node_index < 0) {
        BT_ERR("%s, Invalid node index %d", __func__, node_index);
        return -EINVAL;
    }

    if (node_index >= ARRAY_SIZE(mesh_nodes)) {
        BT_ERR("%s, Too big node index", __func__);
        return -EINVAL;
    }

    node = mesh_nodes[node_index];
    if (!node) {
        BT_ERR("%s, Node is not found", __func__);
        return -EINVAL;
    }

    return 0;
}

int provisioner_node_provision(int node_index, const u8_t uuid[16], u16_t oob_info,
                               u16_t unicast_addr, u8_t element_num, u16_t net_idx,
                               u8_t flags, u32_t iv_index, const u8_t dev_key[16])
{
    struct bt_mesh_node_t *node = NULL;

    BT_DBG("%s", __func__);

    if (mesh_node_count >= ARRAY_SIZE(mesh_nodes)) {
        BT_ERR("%s, Node queue is full", __func__);
        return -ENOMEM;
    }

    if (node_index >= ARRAY_SIZE(mesh_nodes) || !uuid || !dev_key) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    node = osi_calloc(sizeof(struct bt_mesh_node_t));
    if (!node) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    BT_DBG("node_index: 0x%x, unicast_addr: 0x%x, element_num: 0x%x, net_idx: 0x%x",
           node_index, unicast_addr, element_num, net_idx);
    BT_DBG("dev_uuid: %s", bt_hex(uuid, 16));
    BT_DBG("dev_key:  %s", bt_hex(dev_key, 16));

    mesh_nodes[node_index] = node;

    memcpy(node->dev_uuid, uuid, 16);
    node->oob_info     = oob_info;
    node->unicast_addr = unicast_addr;
    node->element_num  = element_num;
    node->net_idx      = net_idx;
    node->flags        = flags;
    node->iv_index     = iv_index;
    memcpy(node->dev_key, dev_key, 16);

    mesh_node_count++;

    return 0;
}

int provisioner_node_reset(int node_index)
{
    struct bt_mesh_node_t *node = NULL;
    struct bt_mesh_rpl    *rpl  = NULL;
    int i;

    BT_DBG("%s, reset node %d", __func__, node_index);

    if (!mesh_node_count) {
        BT_ERR("%s, Node queue is empty", __func__);
        return -ENODEV;
    }

    if (provisioner_index_check(node_index)) {
        BT_ERR("%s, Failed to check node index", __func__);
        return -EINVAL;
    }

    node = mesh_nodes[node_index];

    /* Reset corresponding rpl when reset the node */
    for (i = 0; i < ARRAY_SIZE(bt_mesh.rpl); i++) {
        rpl = &bt_mesh.rpl[i];
        if (rpl->src >= node->unicast_addr &&
                rpl->src < node->unicast_addr + node->element_num) {
            memset(rpl, 0, sizeof(struct bt_mesh_rpl));
        }
    }

    osi_free(mesh_nodes[node_index]);
    mesh_nodes[node_index] = NULL;

    mesh_node_count--;

    return 0;
}

int provisioner_upper_reset_all_nodes(void)
{
    int i, err;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        err = provisioner_node_reset(i);
        if (err == -ENODEV) {
            return 0;
        }
    }

    return 0;
}

/** For Provisioner, we use the same data structure
 *  (like, struct bt_mesh_subnet, etc.) for netkey
 *  & appkey because if not we need to change a lot
 *  of APIs.
 */
int provisioner_upper_init(void)
{
    struct bt_mesh_subnet *sub = NULL;
    u8_t p_key[16] = {0};

    BT_DBG("%s", __func__);

    if (prov_upper_init) {
        return 0;
    }

    comp = bt_mesh_comp_get();
    if (!comp) {
        BT_ERR("%s, NULL composition data", __func__);
        return -EINVAL;
    }

    prov = provisioner_get_prov_info();
    if (!prov) {
        BT_ERR("%s, NULL provisioning context", __func__);
        return -EINVAL;
    }

    /* If the device only acts as a Provisioner, need to initialize
       each element's address. */
    bt_mesh_comp_provision(prov->prov_unicast_addr);

    /* Generate the primary netkey */
    if (bt_mesh_rand(p_key, 16)) {
        BT_ERR("%s, Failed to generate Primary NetKey", __func__);
        return -EIO;
    }

    sub = osi_calloc(sizeof(struct bt_mesh_subnet));
    if (!sub) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    sub->kr_flag = BLE_MESH_KEY_REFRESH(prov->flags);
    if (sub->kr_flag) {
        if (bt_mesh_net_keys_create(&sub->keys[1], p_key)) {
            BT_ERR("%s, Failed to generate net-related keys", __func__);
            osi_free(sub);
            return -EIO;
        }
        sub->kr_phase = BLE_MESH_KR_PHASE_2;
    } else {
        /* Currently provisioner only use keys[0] */
        if (bt_mesh_net_keys_create(&sub->keys[0], p_key)) {
            BT_ERR("%s, Failed to create net-related keys", __func__);
            osi_free(sub);
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

    /* In this function, we use the values of struct bt_mesh_prov
       which has been initialized in the application layer */
    bt_mesh.iv_index = prov->iv_index;
    bt_mesh_atomic_set_bit_to(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS,
                              BLE_MESH_IV_UPDATE(prov->flags));

    /* Set minimum required hours, since the 96-hour minimum requirement
     * doesn't apply straight after provisioning (since we can't know how
     * long has actually passed since the network changed its state).
     * This operation is the same with node initialization.
     */
    bt_mesh.ivu_duration = BLE_MESH_IVU_MIN_HOURS;

    prov_upper_init = true;

    BT_DBG("kr_flag: %d, kr_phase: %d, net_idx: 0x%02x, node_id %d",
           sub->kr_flag, sub->kr_phase, sub->net_idx, sub->node_id);
    BT_DBG("netkey:     %s, nid: 0x%x", bt_hex(sub->keys[0].net, 16), sub->keys[0].nid);
    BT_DBG("enckey:     %s", bt_hex(sub->keys[0].enc, 16));
    BT_DBG("network id: %s", bt_hex(sub->keys[0].net_id, 8));
#if defined(CONFIG_BLE_MESH_GATT_PROXY_SERVER)
    BT_DBG("identity:   %s", bt_hex(sub->keys[0].identity, 16));
#endif
    BT_DBG("privacy:    %s", bt_hex(sub->keys[0].privacy, 16));
    BT_DBG("beacon:     %s", bt_hex(sub->keys[0].beacon, 16));

    return 0;
}

/* The following APIs are for provisioner upper layers internal use */

const u8_t *provisioner_net_key_get(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (!sub || (sub->net_idx != net_idx)) {
            continue;
        }
        if (sub->kr_flag) {
            return sub->keys[1].net;
        }
        return sub->keys[0].net;
    }

    return NULL;
}

struct bt_mesh_subnet *provisioner_subnet_get(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (net_idx == BLE_MESH_KEY_ANY) {
        return bt_mesh.p_sub[0];
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (!sub || (sub->net_idx != net_idx)) {
            continue;
        }
        return sub;
    }

    return NULL;
}

bool provisioner_check_msg_dst_addr(u16_t dst_addr)
{
    struct bt_mesh_node_t *node = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(dst_addr)) {
        return true;
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && dst_addr >= node->unicast_addr &&
                dst_addr < node->unicast_addr + node->element_num) {
            return true;
        }
    }

    return false;
}

const u8_t *provisioner_dev_key_get(u16_t dst_addr)
{
    /* Device key is only used to encrypt configuration messages.
    *  Configuration model shall only be supported by the primary
    *  element which uses the primary unicast address.
    */
    struct bt_mesh_node_t *node = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(dst_addr)) {
        BT_ERR("%s, Not a unicast address 0x%04x", __func__, dst_addr);
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && node->unicast_addr == dst_addr) {
            return node->dev_key;
        }
    }

    return NULL;
}

struct bt_mesh_app_key *provisioner_app_key_find(u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    BT_DBG("%s", __func__);

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (!key) {
            continue;
        }
        if (key->net_idx != BLE_MESH_KEY_UNUSED &&
                key->app_idx == app_idx) {
            return key;
        }
    }

    return NULL;
}

u32_t provisioner_get_prov_node_count(void)
{
    return mesh_node_count;
}

/* The following APIs are for provisioner application use */

#if 0
static int bt_mesh_provisioner_set_kr_flag(u16_t net_idx, bool kr_flag)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (!sub || (sub->net_idx != net_idx)) {
            continue;
        }
        sub->kr_flag = kr_flag;
        break;
    }
    if (i == ARRAY_SIZE(bt_mesh.p_sub)) {
        return -ENODEV;
    }

    /* TODO: When kr_flag is changed, provisioner may need
     *       to change the netkey of the subnet and update
     *       corresponding appkey.
     */

    return 0;
}

static void bt_mesh_provisioner_set_iv_index(u32_t iv_index)
{
    bt_mesh.iv_index = iv_index;

    /* TODO: When iv_index is changed, provisioner may need to
     *       start iv update procedure. And the ivu_initiator
     *       & iv_update flags may also need to be set.
     */
}
#endif

int bt_mesh_provisioner_store_node_info(struct bt_mesh_node_t *node_info)
{
    struct bt_mesh_node_t *node = NULL;
    int i;

    if (!node_info) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    /* Check if the device uuid already exists */
    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && !memcmp(node->dev_uuid, node_info->dev_uuid, 16)) {
            BT_WARN("%s, Node info already exists", __func__);
            return -EEXIST;
        }
    }

    /* 0 ~ (CONFIG_BLE_MESH_MAX_PROV_NODES-1) are left for self-provisioned nodes */
    for (i = CONFIG_BLE_MESH_MAX_PROV_NODES; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (!node) {
            node = osi_calloc(sizeof(struct bt_mesh_node_t));
            if (!node) {
                BT_ERR("%s, Failed to allocate memory", __func__);
                return -ENOMEM;
            }
            memcpy(node, node_info, sizeof(struct bt_mesh_node_t));
            mesh_nodes[i] = node;
            mesh_node_count++;
            return 0;
        }
    }

    BT_ERR("%s, Node info is full", __func__);
    return -ENOMEM;
}

int bt_mesh_provisioner_get_all_node_unicast_addr(struct net_buf_simple *buf)
{
    struct bt_mesh_node_t *node = NULL;
    int i;

    if (!buf) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (!node || !BLE_MESH_ADDR_IS_UNICAST(node->unicast_addr)) {
            continue;
        }
        net_buf_simple_add_le16(buf, node->unicast_addr);
    }

    return 0;
}

int bt_mesh_provisioner_set_node_name(int node_index, const char *name)
{
    size_t length, name_len;
    int i;

    BT_DBG("%s", __func__);

    if (!name) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (provisioner_index_check(node_index)) {
        BT_ERR("%s, Failed to check node index", __func__);
        return -EINVAL;
    }

    BT_DBG("name len is %d, name is %s", strlen(name), name);

    length = (strlen(name) <= MESH_NAME_SIZE) ? strlen(name) : MESH_NAME_SIZE;
    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (!mesh_nodes[i] || !mesh_nodes[i]->node_name) {
            continue;
        }
        name_len = strlen(mesh_nodes[i]->node_name);
        if (length != name_len) {
            continue;
        }
        if (!strncmp(mesh_nodes[i]->node_name, name, length)) {
            BT_WARN("%s, Name %s already exists", __func__, name);
            return -EEXIST;
        }
    }

    strncpy(mesh_nodes[node_index]->node_name, name, length);

    return 0;
}

const char *bt_mesh_provisioner_get_node_name(int node_index)
{
    BT_DBG("%s", __func__);

    if (provisioner_index_check(node_index)) {
        BT_ERR("%s, Failed to check node index", __func__);
        return NULL;
    }

    return mesh_nodes[node_index]->node_name;
}

int bt_mesh_provisioner_get_node_index(const char *name)
{
    size_t length, name_len;
    int i;

    BT_DBG("%s", __func__);

    if (!name) {
        return -EINVAL;
    }

    length = (strlen(name) <= MESH_NAME_SIZE) ? strlen(name) : MESH_NAME_SIZE;
    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        if (!mesh_nodes[i] || !mesh_nodes[i]->node_name) {
            continue;
        }
        name_len = strlen(mesh_nodes[i]->node_name);
        if (length != name_len) {
            continue;
        }
        if (!strncmp(mesh_nodes[i]->node_name, name, length)) {
            return i;
        }
    }

    return -ENODEV;
}

struct bt_mesh_node_t *bt_mesh_provisioner_get_node_info(u16_t unicast_addr)
{
    struct bt_mesh_node_t *node = NULL;
    int i;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(unicast_addr)) {
        BT_ERR("%s, Not a unicast address 0x%04x", __func__, unicast_addr);
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (!node) {
            continue;
        }
        if (unicast_addr >= node->unicast_addr &&
                unicast_addr < (node->unicast_addr + node->element_num)) {
            return node;
        }
    }

    return NULL;
}

u32_t bt_mesh_provisioner_get_net_key_count(void)
{
    return ARRAY_SIZE(bt_mesh.p_sub);
}

u32_t bt_mesh_provisioner_get_app_key_count(void)
{
    return ARRAY_SIZE(bt_mesh.p_app_keys);
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
    struct bt_mesh_app_key  *key  = NULL;
    struct bt_mesh_app_keys *keys = NULL;
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
        BT_WARN("%s, AppKey already exists, AppKey Index updated", __func__);
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

    key = osi_calloc(sizeof(struct bt_mesh_app_key));
    if (!key) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    keys = &key->keys[0];
    if (bt_mesh_app_id(p_key, &keys->id)) {
        BT_ERR("%s, Failed to generate AID", __func__);
        osi_free(key);
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
                    osi_free(key);
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
            osi_free(bt_mesh.p_app_keys[i]);
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
        BT_WARN("%s, NetKey already exists, NetKey Index updated", __func__);
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

    sub = osi_calloc(sizeof(struct bt_mesh_subnet));
    if (!sub) {
        BT_ERR("%s, Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    if (bt_mesh_net_keys_create(&sub->keys[0], p_key)) {
        BT_ERR("%s, Failed to generate NID", __func__);
        osi_free(sub);
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
                    osi_free(sub);
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
    int i;

    BT_DBG("%s", __func__);

    if (provisioner_check_net_idx(net_idx, false)) {
        BT_ERR("%s, NetKey Index does not exist", __func__);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            osi_free(bt_mesh.p_sub[i]);
            bt_mesh.p_sub[i] = NULL;
            return 0;
        }
    }

    /* Shall never reach here */
    return -ENODEV;
}

int bt_mesh_provisioner_get_own_unicast_addr(u16_t *addr, u8_t *elem_num)
{
    if (!addr || !elem_num || !prov || !comp) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    *addr = prov->prov_unicast_addr;
    *elem_num = comp->elem_count;

    return 0;
}

int bt_mesh_provisioner_bind_local_model_app_idx(u16_t elem_addr, u16_t mod_id,
        u16_t cid, u16_t app_idx)
{
    struct bt_mesh_elem  *elem  = NULL;
    struct bt_mesh_model *model = NULL;
    int i;

    if (!comp) {
        BT_ERR("%s, NULL composition data", __func__);
        return -EINVAL;
    }

    for (i = 0; i < comp->elem_count; i++) {
        elem = &comp->elem[i];
        if (elem->addr == elem_addr) {
            break;
        }
    }
    if (i == comp->elem_count) {
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
            BT_WARN("%s, AppKey Index is already binded with model", __func__);
            return 0;
        }
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] == BLE_MESH_KEY_UNUSED) {
            model->keys[i] = app_idx;
            return 0;
        }
    }

    BT_ERR("%s, Model AppKey queue is full", __func__);
    return -ENOMEM;
}

int bt_mesh_provisioner_bind_local_app_net_idx(u16_t net_idx, u16_t app_idx)
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
        if (!key || (key->app_idx != app_idx)) {
            continue;
        }
        key->net_idx = net_idx;
        return 0;
    }

    return -ENODEV;
}

int bt_mesh_provisioner_print_local_element_info(void)
{
    struct bt_mesh_elem  *elem  = NULL;
    struct bt_mesh_model *model = NULL;
    int i, j;

    if (!comp) {
        BT_ERR("%s, NULL composition data", __func__);
        return -EINVAL;
    }

    BT_WARN("************************************************");
    BT_WARN("* cid: 0x%04x    pid: 0x%04x    vid: 0x%04x    *", comp->cid, comp->pid, comp->vid);
    BT_WARN("* Element Number: 0x%02x                         *", comp->elem_count);
    for (i = 0; i < comp->elem_count; i++) {
        elem = &comp->elem[i];
        BT_WARN("* Element %d: 0x%04x                            *", i, elem->addr);
        BT_WARN("*     Loc: 0x%04x   NumS: 0x%02x   NumV: 0x%02x    *", elem->loc, elem->model_count, elem->vnd_model_count);
        for (j = 0; j < elem->model_count; j++) {
            model = &elem->models[j];
            BT_WARN("*     sig_model %d: id - 0x%04x                 *", j, model->id);
        }
        for (j = 0; j < elem->vnd_model_count; j++) {
            model = &elem->vnd_models[j];
            BT_WARN("*     vnd_model %d: id - 0x%04x, cid - 0x%04x   *", j, model->vnd.id, model->vnd.company);
        }
    }
    BT_WARN("************************************************");

    return 0;
}

#endif /* CONFIG_BLE_MESH_PROVISIONER */

/* The following APIs are for fast provisioning */

#if CONFIG_BLE_MESH_FAST_PROV

const u8_t *fast_prov_dev_key_get(u16_t addr)
{
    struct bt_mesh_node_t *node = NULL;

    BT_DBG("%s", __func__);

    if (!BLE_MESH_ADDR_IS_UNICAST(addr)) {
        BT_ERR("%s, Not a unicast address 0x%04x", __func__, addr);
        return NULL;
    }

    if (addr == bt_mesh_primary_addr()) {
        return bt_mesh.dev_key;
    }

    for (int i = 0; i < ARRAY_SIZE(mesh_nodes); i++) {
        node = mesh_nodes[i];
        if (node && node->unicast_addr == addr) {
            return node->dev_key;
        }
    }

    return NULL;
}

struct bt_mesh_subnet *fast_prov_subnet_get(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;

    BT_DBG("%s", __func__);

    for (int i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        sub = &bt_mesh.sub[i];
        if (sub->net_idx == net_idx) {
            return sub;
        }
    }

    for (int i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            return sub;
        }
    }

    return NULL;
}

struct bt_mesh_app_key *fast_prov_app_key_find(u16_t net_idx, u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;

    BT_DBG("%s", __func__);

    for (int i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        key = &bt_mesh.app_keys[i];
        if (key->net_idx == net_idx && key->app_idx == app_idx) {
            return key;
        }
    }

    for (int i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && key->net_idx == net_idx && key->app_idx == app_idx) {
            return key;
        }
    }

    return NULL;
}

u8_t bt_mesh_set_fast_prov_net_idx(u16_t net_idx)
{
    struct bt_mesh_subnet      *sub = NULL;
    struct bt_mesh_subnet_keys *key = NULL;

    sub = fast_prov_subnet_get(net_idx);
    if (sub) {
        key = BLE_MESH_KEY_REFRESH(sub->kr_flag) ? &sub->keys[1] : &sub->keys[0];
        return provisioner_set_fast_prov_net_idx(key->net, net_idx);
    }

    /* If net_idx is not found, set net_idx to fast_prov first,
     * and wait for primary provisioner to add net_key */
    return provisioner_set_fast_prov_net_idx(NULL, net_idx);
}

u8_t bt_mesh_add_fast_prov_net_key(const u8_t net_key[16])
{
    const u8_t *keys = NULL;
    u16_t net_idx;
    int err;

    net_idx = provisioner_get_fast_prov_net_idx();
    bt_mesh.p_net_idx_next = net_idx;

    err = bt_mesh_provisioner_local_net_key_add(net_key, &net_idx);
    if (err) {
        return 0x01; /* status: add net_key fail */
    };

    keys = bt_mesh_provisioner_local_net_key_get(net_idx);
    if (!keys) {
        return 0x01; /* status: add net_key fail */
    }

    return provisioner_set_fast_prov_net_idx(keys, net_idx);
}

const u8_t *bt_mesh_get_fast_prov_net_key(u16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;

    sub = fast_prov_subnet_get(net_idx);
    if (!sub) {
        BT_ERR("%s, Failed to get subnet", __func__);
        return NULL;
    }

    return (sub->kr_flag ? sub->keys[1].net : sub->keys[0].net);
}

const u8_t *bt_mesh_get_fast_prov_app_key(u16_t net_idx, u16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;

    key = fast_prov_app_key_find(net_idx, app_idx);
    if (!key) {
        BT_ERR("%s, Failed to get AppKey", __func__);
        return NULL;
    }

    return (key->updated ? key->keys[1].val : key->keys[0].val);
}

#endif /* CONFIG_BLE_MESH_FAST_PROV */
