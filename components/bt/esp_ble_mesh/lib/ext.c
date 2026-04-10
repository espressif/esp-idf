/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "esp_log.h"

#if CONFIG_BT_BLUEDROID_ENABLED
#include "bta/bta_api.h"
#endif

#if CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE
#include "log_compression/utils.h"
#endif

#include "btc_ble_mesh_agg_model.h"
#include "btc_ble_mesh_brc_model.h"
#include "btc_ble_mesh_df_model.h"
#include "btc_ble_mesh_lcd_model.h"
#include "btc_ble_mesh_mbt_model.h"
#include "btc_ble_mesh_odp_model.h"
#include "btc_ble_mesh_prb_model.h"
#include "btc_ble_mesh_rpr_model.h"
#include "btc_ble_mesh_sar_model.h"
#include "btc_ble_mesh_srpl_model.h"

#include "adv.h"
#include "net.h"
#include "scan.h"
#include "crypto.h"
#include "access.h"
#include "beacon.h"
#include "friend.h"
#include "lpn.h"
#include "rpl.h"
#include "foundation.h"
#include "mesh/buf.h"
#include "mesh/slist.h"
#include "mesh/config.h"
#include "mesh/adapter.h"
#include "mesh/main.h"
#include "mesh/timer.h"
#include "mesh/mutex.h"
#include "mesh/common.h"
#include "mesh/access.h"
#include "prov_common.h"
#include "prov_node.h"
#include "prov_pvnr.h"
#include "pvnr_mgmt.h"
#include "transport.h"
#include "proxy_client.h"
#include "proxy_server.h"
#include "settings.h"
#include "settings_nvs.h"
#include "mesh/model_common.h"
#include "mesh/client_common.h"

#include "mesh_v1.1/utils.h"

#define NET_BUF(a)      ((struct net_buf *)(a))
#define k_WORK(a)       ((struct k_work *)(a))
#define COMP(a)         ((const struct bt_mesh_comp *)(a))
#define ELEM(a)         ((struct bt_mesh_elem *)(a))
#define MODEL(a)        ((struct bt_mesh_model *)(a))
#define APP_KEY(a)      ((struct bt_mesh_app_key *)(a))
#define NODE(a)         ((struct bt_mesh_node *)(a))
#define LINK(a)         ((struct bt_mesh_prov_link *)(a))
#define PROXY_CLI(a)    ((struct bt_mesh_proxy_client *)(a))
#define PROXY_CFG(a)    ((struct bt_mesh_proxy_cfg_pdu *)(a))
#define FRIEND(a)       ((struct bt_mesh_friend *)(a))
#define SUBNET(a)       ((struct bt_mesh_subnet *)(a))
#define NET_TX(a)       ((struct bt_mesh_net_tx *)(a))
#define NET_RX(a)       ((struct bt_mesh_net_rx *)(a))
#define MSG_CTX(a)      ((struct bt_mesh_msg_ctx *)(a))
#define CLI_PARAM(a)    ((bt_mesh_client_common_param_t *)(a))
#define CLI_NODE(a)     ((bt_mesh_client_node_t *)(a))
#define ADV_DATA(a)     ((const struct bt_mesh_adv_data *)(a))
#define RPL(a)          ((struct bt_mesh_rpl *)(a))
#define VOID(a)         ((void *)(a))

/* Declare Lib Variables */
uint8_t __meshlib_var_BLE_MESH_ADV_PROV = BLE_MESH_ADV_PROV;
uint8_t __meshlib_var_BLE_MESH_ADV_DATA = BLE_MESH_ADV_DATA;
#if CONFIG_BLE_MESH_FRIEND
uint8_t __meshlib_var_BLE_MESH_ADV_FRIEND = BLE_MESH_ADV_FRIEND;
#endif
#if CONFIG_BLE_MESH_RELAY_ADV_BUF
uint8_t __meshlib_var_BLE_MESH_ADV_RELAY_DATA = BLE_MESH_ADV_RELAY_DATA;
#endif
uint8_t __meshlib_var_BLE_MESH_ADV_BEACON = BLE_MESH_ADV_BEACON;
uint8_t __meshlib_var_BLE_MESH_ADV_URI = BLE_MESH_ADV_URI;
#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
uint8_t __meshlib_var_BLE_MESH_ADV_PROXY_SOLIC = BLE_MESH_ADV_PROXY_SOLIC;
#endif
#if CONFIG_BLE_MESH_SUPPORT_BLE_ADV
uint8_t __meshlib_var_BLE_MESH_ADV_BLE = BLE_MESH_ADV_BLE;
#endif
uint8_t __meshlib_var_BLE_MESH_ADV_TYPES_NUM = BLE_MESH_ADV_TYPES_NUM;

/* Sys utilities */
void bt_mesh_ext_put_be16(uint16_t val, uint8_t dst[2])
{
    sys_put_be16(val, dst);
}

void bt_mesh_ext_put_be24(uint32_t val, uint8_t dst[3])
{
    sys_put_be24(val, dst);
}

void bt_mesh_ext_put_be32(uint32_t val, uint8_t dst[4])
{
    sys_put_be32(val, dst);
}

void bt_mesh_ext_put_be48(uint64_t val, uint8_t dst[6])
{
    sys_put_be48(val, dst);
}

void bt_mesh_ext_put_be64(uint64_t val, uint8_t dst[8])
{
    sys_put_be64(val, dst);
}

void bt_mesh_ext_put_le16(uint16_t val, uint8_t dst[2])
{
    sys_put_le16(val, dst);
}

void bt_mesh_ext_put_le24(uint32_t val, uint8_t dst[3])
{
    sys_put_le24(val, dst);
}

void bt_mesh_ext_put_le32(uint32_t val, uint8_t dst[4])
{
    sys_put_le32(val, dst);
}

void bt_mesh_ext_put_le48(uint64_t val, uint8_t dst[6])
{
    sys_put_le48(val, dst);
}

void bt_mesh_ext_put_le64(uint64_t val, uint8_t dst[8])
{
    sys_put_le64(val, dst);
}

uint16_t bt_mesh_ext_get_be16(const uint8_t src[2])
{
    return sys_get_be16(src);
}

uint32_t bt_mesh_ext_get_be24(const uint8_t src[3])
{
    return sys_get_be24(src);
}

uint32_t bt_mesh_ext_get_be32(const uint8_t src[4])
{
    return sys_get_be32(src);
}

uint64_t bt_mesh_ext_get_be48(const uint8_t src[6])
{
    return sys_get_be48(src);
}

uint64_t bt_mesh_ext_get_be64(const uint8_t src[8])
{
    return sys_get_be64(src);
}

uint16_t bt_mesh_ext_get_le16(const uint8_t src[2])
{
    return sys_get_le16(src);
}

uint32_t bt_mesh_ext_get_le24(const uint8_t src[3])
{
    return sys_get_le24(src);
}

uint32_t bt_mesh_ext_get_le32(const uint8_t src[4])
{
    return sys_get_le32(src);
}

uint64_t bt_mesh_ext_get_le48(const uint8_t src[6])
{
    return sys_get_le48(src);
}

uint64_t bt_mesh_ext_get_le64(const uint8_t src[8])
{
    return sys_get_le64(src);
}

void bt_mesh_ext_memcpy_swap(void *dst, const void *src, size_t length)
{
    sys_memcpy_swap(dst, src, length);
}

void bt_mesh_ext_mem_swap(void *buf, size_t length)
{
    sys_mem_swap(buf, length);
}

/* Net buf */
void bt_mesh_ext_buf_simple_init(struct net_buf_simple *buf, size_t reserve_head)
{
    net_buf_simple_init(buf, reserve_head);
}

void bt_mesh_ext_buf_simple_init_with_data(struct net_buf_simple *buf, void *data, size_t size)
{
    net_buf_simple_init_with_data(buf, data, size);
}

void bt_mesh_ext_buf_simple_reset(struct net_buf_simple *buf)
{
    net_buf_simple_reset(buf);
}

void bt_mesh_ext_buf_simple_clone(const struct net_buf_simple *original, struct net_buf_simple *clone)
{
    net_buf_simple_clone(original, clone);
}

void *bt_mesh_ext_buf_simple_add(struct net_buf_simple *buf, size_t len)
{
    return net_buf_simple_add(buf, len);
}

void *bt_mesh_ext_buf_simple_add_mem(struct net_buf_simple *buf, const void *mem, size_t len)
{
    return net_buf_simple_add_mem(buf, mem, len);
}

uint8_t *bt_mesh_ext_buf_simple_add_u8(struct net_buf_simple *buf, uint8_t val)
{
    return net_buf_simple_add_u8(buf, val);
}

void bt_mesh_ext_buf_simple_add_le16(struct net_buf_simple *buf, uint16_t val)
{
    net_buf_simple_add_le16(buf, val);
}

void bt_mesh_ext_buf_simple_add_be16(struct net_buf_simple *buf, uint16_t val)
{
    net_buf_simple_add_be16(buf, val);
}

void bt_mesh_ext_buf_simple_add_le24(struct net_buf_simple *buf, uint32_t val)
{
    net_buf_simple_add_le24(buf, val);
}

void bt_mesh_ext_buf_simple_add_be24(struct net_buf_simple *buf, uint32_t val)
{
    net_buf_simple_add_be24(buf, val);
}

void bt_mesh_ext_buf_simple_add_le32(struct net_buf_simple *buf, uint32_t val)
{
    net_buf_simple_add_le32(buf, val);
}

void bt_mesh_ext_buf_simple_add_be32(struct net_buf_simple *buf, uint32_t val)
{
    net_buf_simple_add_be32(buf, val);
}

void bt_mesh_ext_buf_simple_add_le48(struct net_buf_simple *buf, uint64_t val)
{
    net_buf_simple_add_le48(buf, val);
}

void bt_mesh_ext_buf_simple_add_be48(struct net_buf_simple *buf, uint64_t val)
{
    net_buf_simple_add_be48(buf, val);
}

void bt_mesh_ext_buf_simple_add_le64(struct net_buf_simple *buf, uint64_t val)
{
    net_buf_simple_add_le64(buf, val);
}

void bt_mesh_ext_buf_simple_add_be64(struct net_buf_simple *buf, uint64_t val)
{
    net_buf_simple_add_be64(buf, val);
}

void *bt_mesh_ext_buf_simple_push(struct net_buf_simple *buf, size_t len)
{
    return net_buf_simple_push(buf, len);
}

void bt_mesh_ext_buf_simple_push_le16(struct net_buf_simple *buf, uint16_t val)
{
    net_buf_simple_push_le16(buf, val);
}

void bt_mesh_ext_buf_simple_push_be16(struct net_buf_simple *buf, uint16_t val)
{
    net_buf_simple_push_be16(buf, val);
}

void bt_mesh_ext_buf_simple_push_u8(struct net_buf_simple *buf, uint8_t val)
{
    net_buf_simple_push_u8(buf, val);
}

void bt_mesh_ext_buf_simple_push_le24(struct net_buf_simple *buf, uint32_t val)
{
    net_buf_simple_push_le24(buf, val);
}

void bt_mesh_ext_buf_simple_push_be24(struct net_buf_simple *buf, uint32_t val)
{
    net_buf_simple_push_be24(buf, val);
}

void bt_mesh_ext_buf_simple_push_le32(struct net_buf_simple *buf, uint32_t val)
{
    net_buf_simple_push_le32(buf, val);
}

void bt_mesh_ext_buf_simple_push_be32(struct net_buf_simple *buf, uint32_t val)
{
    net_buf_simple_push_be32(buf, val);
}

void bt_mesh_ext_buf_simple_push_le48(struct net_buf_simple *buf, uint64_t val)
{
    net_buf_simple_push_le48(buf, val);
}

void bt_mesh_ext_buf_simple_push_be48(struct net_buf_simple *buf, uint64_t val)
{
    net_buf_simple_push_be48(buf, val);
}

void bt_mesh_ext_buf_simple_push_le64(struct net_buf_simple *buf, uint64_t val)
{
    net_buf_simple_push_le64(buf, val);
}

void bt_mesh_ext_buf_simple_push_be64(struct net_buf_simple *buf, uint64_t val)
{
    net_buf_simple_push_be64(buf, val);
}

void *bt_mesh_ext_buf_simple_pull(struct net_buf_simple *buf, size_t len)
{
    return net_buf_simple_pull(buf, len);
}

void *bt_mesh_ext_buf_simple_pull_mem(struct net_buf_simple *buf, size_t len)
{
    return net_buf_simple_pull_mem(buf, len);
}

uint8_t bt_mesh_ext_buf_simple_pull_u8(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_u8(buf);
}

uint16_t bt_mesh_ext_buf_simple_pull_le16(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_le16(buf);
}

uint16_t bt_mesh_ext_buf_simple_pull_be16(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_be16(buf);
}

uint32_t bt_mesh_ext_buf_simple_pull_le24(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_le24(buf);
}

uint32_t bt_mesh_ext_buf_simple_pull_be24(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_be24(buf);
}

uint32_t bt_mesh_ext_buf_simple_pull_le32(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_le32(buf);
}

uint32_t bt_mesh_ext_buf_simple_pull_be32(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_be32(buf);
}

uint64_t bt_mesh_ext_buf_simple_pull_le48(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_le48(buf);
}

uint64_t bt_mesh_ext_buf_simple_pull_be48(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_be48(buf);
}

uint64_t bt_mesh_ext_buf_simple_pull_le64(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_le64(buf);
}

uint64_t bt_mesh_ext_buf_simple_pull_be64(struct net_buf_simple *buf)
{
    return net_buf_simple_pull_be64(buf);
}

uint8_t *bt_mesh_ext_buf_simple_tail(struct net_buf_simple *buf)
{
    return net_buf_simple_tail(buf);
}

size_t bt_mesh_ext_buf_simple_headroom(struct net_buf_simple *buf)
{
    return net_buf_simple_headroom(buf);
}

size_t bt_mesh_ext_buf_simple_tailroom(struct net_buf_simple *buf)
{
    return net_buf_simple_tailroom(buf);
}

void bt_mesh_ext_buf_simple_save(struct net_buf_simple *buf, struct net_buf_simple_state *state)
{
    net_buf_simple_save(buf, state);
}

void bt_mesh_ext_buf_simple_restore(struct net_buf_simple *buf, struct net_buf_simple_state *state)
{
    net_buf_simple_restore(buf, state);
}

void bt_mesh_ext_buf_simple_reserve(struct net_buf_simple *buf, size_t reserve)
{
    net_buf_simple_reserve(buf, reserve);
}

struct net_buf_simple *bt_mesh_ext_buf_get_simple(void *buf)
{
    return &NET_BUF(buf)->b;
}

void *bt_mesh_ext_buf_add_mem(void *buf, const void *mem, size_t len)
{
    return net_buf_simple_add_mem(&NET_BUF(buf)->b, mem, len);
}

uint8_t *bt_mesh_ext_buf_add_u8(void *buf, uint8_t val)
{
    return net_buf_simple_add_u8(&NET_BUF(buf)->b, val);
}

void bt_mesh_ext_buf_add_le16(void *buf, uint16_t val)
{
    net_buf_simple_add_le16(&NET_BUF(buf)->b, val);
}

void bt_mesh_ext_buf_unref(void *buf)
{
    net_buf_unref(buf);
}

/* Memory */
void *bt_mesh_ext_malloc(size_t size)
{
    return bt_mesh_malloc(size);
}

void *bt_mesh_ext_calloc(size_t size)
{
    return bt_mesh_calloc(size);
}

void bt_mesh_ext_free(void *ptr)
{
    bt_mesh_free(ptr);
}

struct net_buf_simple *bt_mesh_ext_alloc_buf(uint16_t size)
{
    return bt_mesh_alloc_buf(size);
}

void bt_mesh_ext_free_buf(struct net_buf_simple *buf)
{
    bt_mesh_free_buf(buf);
}

/* Utilities */
int bt_mesh_ext_rand(void *buf, size_t len)
{
    return bt_mesh_rand(buf, len);
}

int32_t bt_mesh_ext_ceil(float num)
{
    return bt_mesh_ceil(num);
}

float bt_mesh_ext_log2(float num)
{
    return bt_mesh_log2(num);
}

const char *bt_mesh_ext_hex(const void *buf, size_t len)
{
    return bt_hex(buf, len);
}

/* Crypto */
bool bt_mesh_ext_s1(const char *m, uint8_t salt[16])
{
    return bt_mesh_s1(m, salt);
}

int bt_mesh_ext_aes_cmac(const uint8_t key[16], void *sg, size_t sg_len, uint8_t mac[16])
{
    return bt_mesh_aes_cmac(key, sg, sg_len, mac);
}

int bt_mesh_ext_id128(const uint8_t n[16], const char *s, uint8_t out[16])
{
    return bt_mesh_id128(n, s, out);
}

int bt_mesh_ext_encrypt_be(const uint8_t key[16], const uint8_t plaintext[16], uint8_t enc_data[16])
{
    return bt_mesh_encrypt_be(key, plaintext, enc_data);
}

int bt_mesh_ext_net_obfuscate(uint8_t *pdu, uint32_t iv_index, const uint8_t privacy_key[16])
{
    return bt_mesh_net_obfuscate(pdu, iv_index, privacy_key);
}

int bt_mesh_ext_net_encrypt(const uint8_t key[16], struct net_buf_simple *buf,
                            uint32_t iv_index, bool proxy, bool proxy_solic)
{
    return bt_mesh_net_encrypt(key, buf, iv_index, proxy, proxy_solic);
}

int bt_mesh_ext_net_decrypt(const uint8_t key[16], struct net_buf_simple *buf,
                            uint32_t iv_index, bool proxy, bool proxy_solic)
{
    return bt_mesh_net_decrypt(key, buf, iv_index, proxy, proxy_solic);
}

int bt_mesh_ext_hmac_sha_256(const uint8_t key[32], struct bt_mesh_sg *sg, size_t sg_len, uint8_t mac[32])
{
    return bt_mesh_sha256_hmac_raw_key(key, sg, sg_len, mac);
}

/* Mutex */
void bt_mesh_ext_mutex_create(void *mutex)
{
    bt_mesh_mutex_create(mutex);
}

void bt_mesh_ext_mutex_free(void *mutex)
{
    bt_mesh_mutex_free(mutex);
}

void bt_mesh_ext_mutex_lock(void *mutex)
{
    bt_mesh_mutex_lock(mutex);
}

void bt_mesh_ext_mutex_unlock(void *mutex)
{
    bt_mesh_mutex_unlock(mutex);
}

void bt_mesh_ext_r_mutex_create(void *mutex)
{
    bt_mesh_r_mutex_create(mutex);
}

void bt_mesh_ext_r_mutex_free(void *mutex)
{
    bt_mesh_r_mutex_free(mutex);
}

void bt_mesh_ext_r_mutex_lock(void *mutex)
{
    bt_mesh_r_mutex_lock(mutex);
}

void bt_mesh_ext_r_mutex_unlock(void *mutex)
{
    bt_mesh_r_mutex_unlock(mutex);
}

/* Timer */
int bt_mesh_ext_timer_init(void *work, void (*timeout_cb)(void *work))
{
    return k_delayed_work_init(work, VOID(timeout_cb));
}

int bt_mesh_ext_timer_free(void *work)
{
    return k_delayed_work_free(work);
}

int bt_mesh_ext_timer_submit(void *work, int32_t delay)
{
    return k_delayed_work_submit(work, delay);
}

int bt_mesh_ext_timer_cancel(void *work)
{
    return k_delayed_work_cancel(work);
}

int32_t bt_mesh_ext_timer_remaining_get(void *work)
{
    return k_delayed_work_remaining_get(work);
}

uint32_t bt_mesh_ext_timer_get_32(void)
{
    return k_uptime_get_32();
}

void *bt_mesh_ext_timer_get_with_work(void *work)
{
    return CONTAINER_OF(work, struct k_delayed_work, work);
}

void *bt_mesh_ext_timer_get_user_data(void *work)
{
    return k_WORK(work)->user_data;
}

void bt_mesh_ext_timer_set_user_data(void *work, void *user_data)
{
    k_WORK(work)->user_data = user_data;
}

/* Atomic */
int bt_mesh_ext_atomic_inc(int *target)
{
    return bt_mesh_atomic_inc(target);
}

int bt_mesh_ext_atomic_dec(int *target)
{
    return bt_mesh_atomic_dec(target);
}

int bt_mesh_ext_atomic_get(const int *target)
{
    return bt_mesh_atomic_get(target);
}

int bt_mesh_ext_atomic_set(int *target, int value)
{
    return bt_mesh_atomic_set(target, value);
}

int bt_mesh_ext_atomic_or(int *target, int value)
{
    return bt_mesh_atomic_or(target, value);
}

int bt_mesh_ext_atomic_and(int *target, int value)
{
    return bt_mesh_atomic_and(target, value);
}

int bt_mesh_ext_atomic_test_bit(const int *target, int bit)
{
    return bt_mesh_atomic_test_bit(target, bit);
}

int bt_mesh_ext_atomic_test_and_clear_bit(int *target, int bit)
{
    return bt_mesh_atomic_test_and_clear_bit(target, bit);
}

int bt_mesh_ext_atomic_test_and_set_bit(int *target, int bit)
{
    return bt_mesh_atomic_test_and_set_bit(target, bit);
}

void bt_mesh_ext_atomic_clear_bit(int *target, int bit)
{
    return bt_mesh_atomic_clear_bit(target, bit);
}

void bt_mesh_ext_atomic_set_bit(int *target, int bit)
{
    return bt_mesh_atomic_set_bit(target, bit);
}

void bt_mesh_ext_atomic_set_bit_to(int *target, int bit, bool val)
{
    return bt_mesh_atomic_set_bit_to(target, bit, val);
}

/* Access */
void bt_mesh_ext_comp_provision(uint16_t addr)
{
    bt_mesh_comp_provision(addr);
}

int bt_mesh_ext_get_opcode(struct net_buf_simple *buf, uint32_t *opcode, bool pull_buf)
{
    return bt_mesh_get_opcode(buf, opcode, pull_buf);
}

size_t bt_mesh_ext_rx_netkey_size(void)
{
    return bt_mesh_rx_netkey_size();
}

void *bt_mesh_ext_rx_netkey_get(size_t index)
{
    return bt_mesh_rx_netkey_get(index);
}

uint16_t bt_mesh_ext_primary_addr(void)
{
    return bt_mesh_primary_addr();
}

const void *bt_mesh_ext_comp_get(void)
{
    return bt_mesh_comp_get();
}

size_t bt_mesh_ext_comp_get_elem_count(const void *comp)
{
    return COMP(comp)->elem_count;
}

void *bt_mesh_ext_comp_get_elem_s(const void *comp)
{
    return COMP(comp)->elem;
}

void *bt_mesh_ext_comp_get_elem(const void *comp, uint8_t index)
{
    return &COMP(comp)->elem[index];
}

uint16_t bt_mesh_ext_comp_get_cid(const void *comp)
{
    return COMP(comp)->cid;
}

uint16_t bt_mesh_ext_comp_get_pid(const void *comp)
{
    return COMP(comp)->pid;
}

uint16_t bt_mesh_ext_comp_get_vid(const void *comp)
{
    return COMP(comp)->vid;
}

void *bt_mesh_ext_elem_find(uint16_t addr)
{
    return bt_mesh_elem_find(addr);
}

uint8_t bt_mesh_ext_elem_count(void)
{
    return bt_mesh_elem_count();
}

uint16_t bt_mesh_ext_elem_get_loc(void *elem)
{
    return ELEM(elem)->loc;
}

uint8_t bt_mesh_ext_elem_get_model_count(void *elem)
{
    return ELEM(elem)->model_count;
}

void *bt_mesh_ext_elem_get_model(void *elem, uint8_t index)
{
    return &ELEM(elem)->models[index];
}

uint8_t bt_mesh_ext_elem_get_vnd_model_count(void *elem)
{
    return ELEM(elem)->vnd_model_count;
}

void *bt_mesh_ext_elem_get_vnd_model(void *elem, uint8_t index)
{
    return &ELEM(elem)->vnd_models[index];
}

bool bt_mesh_ext_model_in_primary(const void *model)
{
    return bt_mesh_model_in_primary(model);
}

void *bt_mesh_ext_model_elem(void *mod)
{
    return bt_mesh_model_elem(mod);
}

void *bt_mesh_ext_model_find(void *elem, uint16_t id)
{
    return bt_mesh_model_find(elem, id);
}

void *bt_mesh_ext_model_find_vnd(void *elem, uint16_t company, uint16_t id)
{
    return bt_mesh_model_find_vnd(elem, company, id);
}

void bt_mesh_ext_model_msg_init(struct net_buf_simple *msg, uint32_t opcode)
{
    bt_mesh_model_msg_init(msg, opcode);
}

int bt_mesh_ext_model_send(void *model, void *ctx, struct net_buf_simple *msg,
                           const struct bt_mesh_send_cb *cb, void *cb_data)
{
    return bt_mesh_model_send(model, ctx, msg, cb, cb_data);
}

uint16_t bt_mesh_ext_model_get_id(void *model)
{
    return MODEL(model)->id;
}

uint16_t bt_mesh_ext_model_get_vnd_id(void *model)
{
    return MODEL(model)->vnd.id;
}

uint16_t bt_mesh_ext_model_get_vnd_cid(void *model)
{
    return MODEL(model)->vnd.company;
}

void *bt_mesh_ext_model_get_pub(void *model)
{
    return MODEL(model)->pub;
}

uint16_t bt_mesh_ext_model_get_pub_addr(void *model)
{
    return MODEL(model)->pub->addr;
}

uint16_t bt_mesh_ext_model_get_pub_key(void *model)
{
    return MODEL(model)->pub->key;
}

uint8_t bt_mesh_ext_model_get_pub_directed_pub_policy(void *model)
{
#if CONFIG_BLE_MESH_DF_SRV
    return MODEL(model)->pub->directed_pub_policy;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_model_set_pub_directed_pub_policy(void *model, uint8_t directed_pub_policy)
{
#if CONFIG_BLE_MESH_DF_SRV
    MODEL(model)->pub->directed_pub_policy = directed_pub_policy;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void *bt_mesh_ext_model_get_pub_msg(void *model)
{
    return MODEL(model)->pub->msg;
}

uint8_t bt_mesh_ext_model_get_keys_count(void *model)
{
    return ARRAY_SIZE(MODEL(model)->keys);
}

uint16_t bt_mesh_ext_model_get_keys_size(void *model)
{
    return sizeof(MODEL(model)->keys);
}

uint16_t bt_mesh_ext_model_get_key(void *model, uint8_t index)
{
    return MODEL(model)->keys[index];
}

void bt_mesh_ext_model_set_key(void *model, uint8_t index, uint16_t key)
{
    MODEL(model)->keys[index] = key;
}

uint8_t bt_mesh_ext_model_get_groups_count(void *model)
{
    return ARRAY_SIZE(MODEL(model)->groups);
}

uint16_t bt_mesh_ext_model_get_groups_size(void *model)
{
    return sizeof(MODEL(model)->groups);
}

uint16_t bt_mesh_ext_model_get_group(void *model, uint8_t index)
{
    return MODEL(model)->groups[index];
}

void bt_mesh_ext_model_set_group(void *model, uint8_t index, uint16_t group)
{
    MODEL(model)->groups[index] = group;
}

void *bt_mesh_ext_model_get_user_data(void *model)
{
    return MODEL(model)->user_data;
}

void bt_mesh_ext_model_set_user_data(void *model, void *user_data)
{
    MODEL(model)->user_data = user_data;
}

void bt_mesh_ext_model_free_user_data(void *model)
{
    assert(MODEL(model)->user_data);
    bt_mesh_free(MODEL(model)->user_data);
    MODEL(model)->user_data = NULL;
}

void bt_mesh_ext_model_set_user_data_model(void *model, void *mod)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    client->model = mod;
}

void bt_mesh_ext_model_set_user_data_op_pair_size(void *model, uint32_t op_pair_size)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    client->op_pair_size = op_pair_size;
}

void bt_mesh_ext_model_set_user_data_op_pair(void *model, const void *op_pair)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    client->op_pair = op_pair;
}

void *bt_mesh_ext_model_get_user_data_internal_data(void *model)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    return client->internal_data;
}

void bt_mesh_ext_model_set_user_data_internal_data(void *model, void *internal_data)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    client->internal_data = internal_data;
}

void bt_mesh_ext_model_free_user_data_internal_data(void *model)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    assert(client->internal_data);
    bt_mesh_free(client->internal_data);
    client->internal_data = NULL;
}

void *bt_mesh_ext_model_get_user_data_internal_data_queue(void *model)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    bt_mesh_client_internal_data_t *internal = client->internal_data;
    assert(internal);
    return &internal->queue;
}

void *bt_mesh_ext_model_get_user_data_vendor_data(void *model)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    return client->vendor_data;
}

void bt_mesh_ext_model_set_user_data_vendor_data(void *model, void *vendor_data)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    client->vendor_data = vendor_data;
}

void bt_mesh_ext_model_free_user_data_vendor_data(void *model)
{
    bt_mesh_client_user_data_t *client = MODEL(model)->user_data;
    assert(client);
    assert(client->vendor_data);
    bt_mesh_free(client->vendor_data);
    client->vendor_data = NULL;
}

bool bt_mesh_ext_model_is_opcode_belongs(void *models, uint8_t model_count, uint32_t opcode)
{
    const struct bt_mesh_model_op *op = NULL;
    struct bt_mesh_model *model = NULL;

    for (size_t i = 0; i < model_count; i++) {
        model = models + i;
        for (op = model->op; op->func; op++) {
            if (op->opcode == opcode) {
                return true;
            }
        }
    }

    return false;
}

/* Transport */
void *bt_mesh_ext_app_key_get(uint16_t app_idx)
{
    return bt_mesh_app_key_get(app_idx);
}

uint16_t bt_mesh_ext_app_key_get_net_idx(void *key)
{
    return APP_KEY(key)->net_idx;
}

int bt_mesh_ext_ctl_send(void *tx, uint8_t ctl_op, void *data, size_t data_len,
                         const struct bt_mesh_send_cb *cb, void *cb_data)
{
    return bt_mesh_ctl_send(tx, ctl_op, data, data_len, cb, cb_data);
}

uint16_t bt_mesh_ext_get_tx_sdu_max(void)
{
    return BLE_MESH_TX_SDU_MAX;
}

void bt_mesh_ext_tx_reset(void)
{
    bt_mesh_tx_reset();
}

void bt_mesh_ext_rx_reset(void)
{
    bt_mesh_rx_reset();
}

/* Net */
void *bt_mesh_ext_subnet_get(uint16_t net_idx)
{
    return bt_mesh_subnet_get(net_idx);
}

int bt_mesh_ext_net_encode(void *tx, struct net_buf_simple *buf, bool proxy)
{
    return bt_mesh_net_encode(tx, buf, proxy);
}

uint8_t bt_mesh_ext_net_flags(void *sub)
{
    return bt_mesh_net_flags(sub);
}

bool bt_mesh_ext_net_iv_update(uint32_t iv_index, bool iv_update)
{
    return bt_mesh_net_iv_update(iv_index, iv_update);
}

bool bt_mesh_ext_kr_update(void *sub, uint8_t new_kr, bool new_key)
{
    return bt_mesh_kr_update(sub, new_kr, new_key);
}

void bt_mesh_ext_net_sec_update(void *sub)
{
    bt_mesh_net_sec_update(sub);
}

int bt_mesh_ext_net_pdu_decrypt(void *sub, const uint8_t *enc,
                                const uint8_t *priv,
                                const uint8_t *data,
                                size_t data_len, void *rx,
                                struct net_buf_simple *buf)
{
    extern int net_decrypt(struct bt_mesh_subnet *sub,
                           const uint8_t *enc, const uint8_t *priv,
                           const uint8_t *data, size_t data_len,
                           void *rx,
                           struct net_buf_simple *buf);
    return net_decrypt(sub, enc, priv, data, data_len, rx, buf);
}

uint16_t bt_mesh_ext_net_get_sub_net_idx(uint8_t index)
{
    return bt_mesh.sub[index].net_idx;
}

uint8_t bt_mesh_ext_net_get_sub_count(void)
{
    return ARRAY_SIZE(bt_mesh.sub);
}

void *bt_mesh_ext_net_get_sub(uint8_t index)
{
    return &bt_mesh.sub[index];
}

uint32_t bt_mesh_ext_net_get_iv_index(void)
{
    return bt_mesh.iv_index;
}

void bt_mesh_ext_net_set_seq(uint32_t seq)
{
    bt_mesh.seq = seq;
}

uint8_t *bt_mesh_ext_net_get_dev_key(void)
{
    return bt_mesh.dev_key;
}

uint8_t *bt_mesh_ext_net_get_dev_key_ca(void)
{
    return bt_mesh.dev_key_ca;
}

uint16_t bt_mesh_ext_net_get_rpl_count(void)
{
    return ARRAY_SIZE(bt_mesh.rpl);
}

uint16_t bt_mesh_ext_net_get_rpl_src(uint16_t index)
{
    return bt_mesh.rpl[index].src;
}

void bt_mesh_ext_net_reset_rpl(uint16_t index)
{
    memset(&bt_mesh.rpl[index], 0, sizeof(bt_mesh.rpl[index]));
}

int bt_mesh_ext_net_is_ivu_initiator(void)
{
    return bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_INITIATOR);
}

int bt_mesh_ext_net_is_ivu_in_progress(void)
{
    return bt_mesh_atomic_test_bit(bt_mesh.flags, BLE_MESH_IVU_IN_PROGRESS);
}

bool bt_mesh_ext_primary_subnet_exist(void)
{
    return bt_mesh_primary_subnet_exist();
}

/* Beacon */
int bt_mesh_ext_net_secure_beacon_update(void *sub)
{
    return bt_mesh_net_secure_beacon_update(sub);
}

uint8_t *bt_mesh_ext_net_get_sub_mpb_rand(uint8_t index)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return bt_mesh.sub[index].mpb_random;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

void *bt_mesh_ext_beacon_cache_check(uint8_t data[21], bool private_beacon)
{
    extern struct bt_mesh_subnet *cache_check(uint8_t data[21],
                                              bool private_beacon);
    return cache_check(data, private_beacon);
}

void bt_mesh_ext_beacon_cache_add(uint8_t data[21], void *sub, bool private_beacon)
{
    extern void cache_add(uint8_t data[21],
                          struct bt_mesh_subnet *sub,
                          bool private_beacon);
    cache_add(data, sub, private_beacon);
}

void bt_mesh_ext_beacon_update_observation(bool private_beacon)
{
    extern void update_beacon_observation(bool private_beacon);
    update_beacon_observation(private_beacon);
}

void bt_mesh_ext_beacon_ivu_initiator(bool enable)
{
    bt_mesh_beacon_ivu_initiator(enable);
}

/* Foundation */
uint8_t bt_mesh_ext_net_transmit_get(void)
{
    return bt_mesh_net_transmit_get();
}

uint8_t bt_mesh_ext_relay_get(void)
{
    return bt_mesh_relay_get();
}

uint8_t bt_mesh_ext_friend_get(void)
{
    return bt_mesh_friend_get();
}

uint8_t bt_mesh_ext_relay_retransmit_get(void)
{
    return bt_mesh_relay_retransmit_get();
}

uint8_t bt_mesh_ext_secure_beacon_get(void)
{
    return bt_mesh_secure_beacon_get();
}

uint8_t bt_mesh_ext_gatt_proxy_get(void)
{
    return bt_mesh_gatt_proxy_get();
}

uint8_t bt_mesh_ext_default_ttl_get(void)
{
    return bt_mesh_default_ttl_get();
}

void bt_mesh_ext_key_idx_pack(struct net_buf_simple *buf,
                              uint16_t idx1, uint16_t idx2)
{
    return key_idx_pack(buf, idx1, idx2);
}

void bt_mesh_ext_key_idx_unpack(struct net_buf_simple *buf,
                                uint16_t *idx1, uint16_t *idx2)
{
    return key_idx_unpack(buf, idx1, idx2);
}

/* Provisioning */
const void *bt_mesh_ext_prov_get(void)
{
    return bt_mesh_prov_get();
}

uint8_t bt_mesh_ext_prov_buf_headroom(void)
{
    return PROV_BUF_HEADROOM;
}

void bt_mesh_ext_prov_buf_init(struct net_buf_simple *buf, uint8_t type)
{
    bt_mesh_prov_buf_init(buf, type);
}

bool bt_mesh_ext_prov_pdu_check(uint8_t type, uint16_t length, uint8_t *reason)
{
    return bt_mesh_prov_pdu_check(type, length, reason);
}

int bt_mesh_ext_prov_send(void *link, struct net_buf_simple *buf)
{
    return bt_mesh_prov_send(link, buf);
}

int bt_mesh_ext_prov_send_adv(void *link, struct net_buf_simple *msg)
{
    return bt_mesh_prov_send_adv(link, msg);
}

int bt_mesh_ext_prov_bearer_ctl_send(void *link, uint8_t op,
                                     void *data, uint8_t data_len)
{
    return bt_mesh_prov_bearer_ctl_send(link, op, data, data_len);
}

void bt_mesh_ext_prov_clear_tx(void *link, bool cancel)
{
    bt_mesh_prov_clear_tx(link, cancel);
}

uint8_t bt_mesh_ext_prov_node_next_xact_id(void *link)
{
#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_ADV
    extern uint8_t node_next_xact_id(struct bt_mesh_prov_link *link);
    return node_next_xact_id(link);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_ADV */
}

void *bt_mesh_ext_prov_node_get_link(void)
{
#if CONFIG_BLE_MESH_NODE
    extern struct bt_mesh_prov_link *bt_mesh_prov_node_get_link(void);
    return bt_mesh_prov_node_get_link();
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_NODE */
}

void bt_mesh_ext_prov_node_close_link(uint8_t reason)
{
#if CONFIG_BLE_MESH_NODE
    extern void bt_mesh_prov_node_close_link(uint8_t reason);
    return bt_mesh_prov_node_close_link(reason);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_NODE */
}

const uint8_t *bt_mesh_ext_prov_node_get_uuid(void)
{
#if CONFIG_BLE_MESH_NODE
    assert(bt_mesh_prov_get());
    return bt_mesh_prov_get()->uuid;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_NODE */
}

uint16_t bt_mesh_ext_prov_node_get_oob_info(void)
{
#if CONFIG_BLE_MESH_NODE
    assert(bt_mesh_prov_get());
    return bt_mesh_prov_get()->oob_info;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_NODE */
}

const char *bt_mesh_ext_prov_node_get_uri(void)
{
#if CONFIG_BLE_MESH_NODE
    assert(bt_mesh_prov_get());
    return bt_mesh_prov_get()->uri;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_NODE */
}

uint8_t bt_mesh_ext_prov_pvnr_next_xact_id(void *link)
{
#if CONFIG_BLE_MESH_PB_ADV
    extern uint8_t pvnr_next_xact_id(struct bt_mesh_prov_link *link);
    return pvnr_next_xact_id(link);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PB_ADV */
}

void *bt_mesh_ext_prov_pvnr_get_link(void)
{
#if CONFIG_BLE_MESH_PROVISIONER
    extern struct bt_mesh_prov_link *bt_mesh_prov_pvnr_get_link(void);
    return bt_mesh_prov_pvnr_get_link();
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

uint8_t bt_mesh_ext_prov_pvnr_get_link_count(void)
{
#if CONFIG_BLE_MESH_PROVISIONER
    extern uint8_t bt_mesh_prov_pvnr_get_link_count(void);
    return bt_mesh_prov_pvnr_get_link_count();
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

void bt_mesh_ext_prov_pvnr_send_invite(void *link)
{
#if CONFIG_BLE_MESH_PROVISIONER
    extern void bt_mesh_prov_pvnr_send_invite(struct bt_mesh_prov_link *link);
    bt_mesh_prov_pvnr_send_invite(link);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

void bt_mesh_ext_prov_pvnr_close_link(void *link, uint8_t reason)
{
#if CONFIG_BLE_MESH_PROVISIONER
    extern void bt_mesh_prov_pvnr_close_link(struct bt_mesh_prov_link *link, uint8_t reason);
    bt_mesh_prov_pvnr_close_link(link, reason);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

void *bt_mesh_ext_pvnr_get_node_with_addr(uint16_t unicast_addr)
{
#if CONFIG_BLE_MESH_PROVISIONER
    return bt_mesh_provisioner_get_node_with_addr(unicast_addr);
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

void bt_mesh_ext_pvnr_records_list_get_cb(uint16_t link_idx, struct net_buf_simple *data)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    assert(bt_mesh_prov_get());
    if (bt_mesh_prov_get()->records_list_get) {
        bt_mesh_prov_get()->records_list_get(link_idx, data);
    }
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

void bt_mesh_ext_pvnr_records_recv_comp_cb(uint8_t status, uint16_t link_idx,
                                           uint16_t record_id, uint16_t frag_offset,
                                           uint16_t total_len, uint8_t *record)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    assert(bt_mesh_prov_get());
    if (bt_mesh_prov_get()->prov_record_recv_comp) {
        bt_mesh_prov_get()->prov_record_recv_comp(status, link_idx, record_id,
                                                  frag_offset, total_len, record);
    }
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

uint8_t *bt_mesh_ext_pvnr_get_node_uuid(void *node)
{
#if CONFIG_BLE_MESH_PROVISIONER
    return NODE(node)->dev_uuid;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

int bt_mesh_ext_prov_link_test_bit(void *link, int bit)
{
    return bt_mesh_atomic_test_bit(LINK(link)->flags, bit);
}

void bt_mesh_ext_prov_link_clear_bit(void *link, int bit)
{
    bt_mesh_atomic_clear_bit(LINK(link)->flags, bit);
}

void bt_mesh_ext_prov_link_set_bit(void *link, int bit)
{
    bt_mesh_atomic_set_bit(LINK(link)->flags, bit);
}

void bt_mesh_ext_prov_link_set_expect(void *link, uint8_t val)
{
    LINK(link)->expect = val;
}

uint8_t bt_mesh_ext_prov_link_get_expect(void *link)
{
    return LINK(link)->expect;
}

uint8_t bt_mesh_ext_prov_link_get_pub_key_type(void *link)
{
    return LINK(link)->public_key;
}

uint8_t bt_mesh_ext_prov_link_get_auth_method(void *link)
{
    return LINK(link)->auth_method;
}

void *bt_mesh_ext_prov_link_get_addr(void *link)
{
    return &LINK(link)->addr;
}

uint8_t *bt_mesh_ext_prov_link_get_uuid(void *link)
{
#if CONFIG_BLE_MESH_PROVISIONER
    return LINK(link)->uuid;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

uint8_t bt_mesh_ext_prov_link_get_elem_num(void *link)
{
#if CONFIG_BLE_MESH_PROVISIONER
    return LINK(link)->element_num;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

uint16_t bt_mesh_ext_prov_link_get_unicast_addr(void *link)
{
#if CONFIG_BLE_MESH_PROVISIONER
    return LINK(link)->unicast_addr;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
}

uint16_t bt_mesh_ext_prov_link_get_record_id_expect(void *link)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    return LINK(link)->record_id_expect;
#else
    assert(0);
    return 0;
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

void bt_mesh_ext_prov_link_set_record_id_expect(void *link, uint16_t record_id_expect)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    LINK(link)->record_id_expect = record_id_expect;
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

uint16_t bt_mesh_ext_prov_link_get_offset_expect(void *link)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    return LINK(link)->offset_expect;
#else
    assert(0);
    return 0;
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

void bt_mesh_ext_prov_link_set_offset_expect(void *link, uint16_t offset_expect)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    LINK(link)->offset_expect = offset_expect;
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

uint16_t bt_mesh_ext_prov_link_get_max_size(void *link)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    return LINK(link)->max_size;
#else
    assert(0);
    return 0;
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

void bt_mesh_ext_prov_link_set_max_size(void *link, uint16_t max_size)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    LINK(link)->max_size = max_size;
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

void *bt_mesh_ext_prov_link_get_conn(void *link)
{
#if CONFIG_BLE_MESH_PB_GATT
    return LINK(link)->conn;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PB_GATT */
}

uint32_t *bt_mesh_ext_prov_link_get_link_id(void *link)
{
#if CONFIG_BLE_MESH_PB_ADV
    return &LINK(link)->link_id;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PB_ADV */
}

uint8_t *bt_mesh_ext_prov_link_get_pb_remote_uuid(void *link)
{
    return LINK(link)->pb_remote_uuid;
}

void *bt_mesh_ext_prov_link_get_prot_timer(void *link)
{
    return &(LINK(link)->prot_timer);
}

void *bt_mesh_ext_prov_link_get_with_work(void *work)
{
    return CONTAINER_OF(work, struct bt_mesh_prov_link, prot_timer.work);
}

uint8_t bt_mesh_ext_prov_link_get_pb_remote_timeout(void *link)
{
    return LINK(link)->pb_remote_timeout;
}

void bt_mesh_ext_prov_link_set_pb_remote_timeout(void *link, uint8_t pb_remote_timeout)
{
    LINK(link)->pb_remote_timeout = pb_remote_timeout;
}

uint8_t bt_mesh_ext_prov_link_get_pb_remote_nppi(void *link)
{
    return LINK(link)->pb_remote_nppi;
}

void bt_mesh_ext_prov_link_set_pb_remote_nppi(void *link, uint8_t pb_remote_nppi)
{
    LINK(link)->pb_remote_nppi = pb_remote_nppi;
}

uint8_t bt_mesh_ext_prov_link_get_pb_remote_pub_key(void *link)
{
    return LINK(link)->pb_remote_pub_key;
}

void bt_mesh_ext_prov_link_set_pb_remote_pub_key(void *link, uint8_t pb_remote_pub_key)
{
    LINK(link)->pb_remote_pub_key = pb_remote_pub_key;
}

uint8_t bt_mesh_ext_prov_link_get_pb_remote_cbd(void *link)
{
    return LINK(link)->pb_remote_cbd;
}

void bt_mesh_ext_prov_link_set_pb_remote_cbd(void *link, uint8_t pb_remote_cbd)
{
    LINK(link)->pb_remote_cbd = pb_remote_cbd;
}

uint8_t bt_mesh_ext_prov_link_get_pb_remote_csp(void *link)
{
    return LINK(link)->pb_remote_csp;
}

void bt_mesh_ext_prov_link_set_pb_remote_csp(void *link, uint8_t pb_remote_csp)
{
    LINK(link)->pb_remote_csp = pb_remote_csp;
}

uint8_t bt_mesh_ext_prov_link_get_pb_remote_reset(void *link)
{
    return LINK(link)->pb_remote_reset;
}

void bt_mesh_ext_prov_link_set_pb_remote_reset(void *link, uint8_t pb_remote_reset)
{
    LINK(link)->pb_remote_reset = pb_remote_reset;
}

void *bt_mesh_ext_prov_link_get_pb_remote_data(void *link)
{
    return LINK(link)->pb_remote_data;
}

void bt_mesh_ext_prov_link_set_pb_remote_data(void *link, void *data)
{
    LINK(link)->pb_remote_data = data;
}

void bt_mesh_ext_prov_link_free_pb_remote_data(void *link)
{
    assert(LINK(link)->pb_remote_data);
    bt_mesh_free(LINK(link)->pb_remote_data);
    LINK(link)->pb_remote_data = NULL;
}

uint8_t *bt_mesh_ext_prov_link_get_record(void *link, uint16_t id)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    return LINK(link)->records[id];
#else
    assert(0);
    return NULL;
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

uint8_t *bt_mesh_ext_prov_link_alloc_record(void *link, uint16_t id, uint16_t len)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV)
    if (id >= BLE_MESH_REC_MAX_ID) {
        return NULL;
    }
    LINK(link)->records[id] = bt_mesh_calloc(len * sizeof(uint8_t));
    return LINK(link)->records[id];
#else
    assert(0);
    return NULL;
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_CERT_BASED_PROV) */
}

bool bt_mesh_ext_prov_link_pb_gatt_exist(void *link)
{
#if CONFIG_BLE_MESH_PB_GATT
    return !!LINK(link)->pb_gatt_send;
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_PB_GATT */
}

int bt_mesh_ext_prov_link_pb_gatt_send(void *link, struct net_buf_simple *msg)
{
#if CONFIG_BLE_MESH_PB_GATT
    assert(LINK(link)->pb_gatt_send);
    return LINK(link)->pb_gatt_send(link, msg);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PB_GATT */
}

void bt_mesh_ext_prov_link_set_pb_remote_close(void *link, void (*pb_remote_close)(void *link, uint8_t reason))
{
    LINK(link)->pb_remote_close = VOID(pb_remote_close);
}

void bt_mesh_ext_prov_link_pb_remote_close(void *link, uint8_t reason)
{
    if (LINK(link)->pb_remote_close) {
        LINK(link)->pb_remote_close(link, reason);
    }
}

void bt_mesh_ext_prov_link_set_pb_remote_send(void *link, int (*pb_remote_send)(void *link, struct net_buf_simple *buf))
{
    LINK(link)->pb_remote_send = VOID(pb_remote_send);
}

void bt_mesh_ext_prov_link_set_next_xact_id(void *link, uint8_t (*next_xact_id)(void *link))
{
#if CONFIG_BLE_MESH_PB_ADV
    LINK(link)->next_xact_id = VOID(next_xact_id);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PB_ADV */
}

/* Proxy Server */
uint8_t bt_mesh_ext_proxy_server_get_all_client_type(void)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return bt_mesh_proxy_server_get_all_client_type();
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

int bt_mesh_ext_proxy_server_segment_send(void *conn, uint8_t type,
                                          struct net_buf_simple *msg)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return bt_mesh_proxy_server_segment_send(conn, type, msg);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

bool bt_mesh_ext_proxy_server_find_client_by_addr(uint16_t addr)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return bt_mesh_proxy_server_find_client_by_addr(addr);
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

void bt_mesh_ext_proxy_server_update_net_id_rand_stop(void)
{
#if (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV)
    bt_mesh_proxy_server_update_net_id_rand_stop();
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV) */
}

void *bt_mesh_ext_proxy_server_get_client(uint8_t index)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    extern struct bt_mesh_proxy_client *bt_mesh_proxy_server_get_client(uint8_t index);
    return bt_mesh_proxy_server_get_client(index);
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

uint8_t bt_mesh_ext_proxy_server_get_client_count(void)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    extern uint8_t bt_mesh_proxy_server_get_client_count(void);
    return bt_mesh_proxy_server_get_client_count();
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

void bt_mesh_ext_proxy_server_update_net_id_rand(void)
{
#if (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV)
    bt_mesh_proxy_server_update_net_id_rand();
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV) */
}

bool bt_mesh_ext_proxy_server_is_node_id_enable(void)
{
#if (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV)
    return bt_mesh_proxy_server_is_node_id_enable();
#else
    assert(0);
    return false;
#endif /* (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV) */
}

void bt_mesh_ext_proxy_server_private_identity_start(void *sub)
{
#if (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV)
    bt_mesh_proxy_server_private_identity_start(sub);
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV) */
}

void bt_mesh_ext_proxy_server_private_identity_stop(void *sub)
{
#if (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV)
    bt_mesh_proxy_server_private_identity_stop(sub);
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_GATT_PROXY_SERVER && CONFIG_BLE_MESH_PRB_SRV) */
}

uint8_t bt_mesh_ext_proxy_server_get_filter_type(void *client)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return PROXY_CLI(client)->filter_type;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

uint8_t bt_mesh_ext_proxy_server_get_filter_count(void *client)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return ARRAY_SIZE(PROXY_CLI(client)->filter);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

uint16_t bt_mesh_ext_proxy_server_get_filter_size(void *client)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return sizeof(PROXY_CLI(client)->filter);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

uint16_t bt_mesh_ext_proxy_server_get_filter_addr(void *client, uint8_t index)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return PROXY_CLI(client)->filter[index].addr;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

bool bt_mesh_ext_proxy_server_filter_is_client(void *client, uint8_t index)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return PROXY_CLI(client)->filter[index].proxy_client;
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

uint8_t bt_mesh_ext_proxy_server_get_client_type(void *client)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return PROXY_CLI(client)->proxy_client_type;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

bool bt_mesh_ext_proxy_server_is_proxy_msg_recv(void *client)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return PROXY_CLI(client)->proxy_msg_recv;
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

void *bt_mesh_ext_proxy_server_get_conn(void *client)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return PROXY_CLI(client)->conn;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

void bt_mesh_ext_proxy_server_set_client_type(void *client, uint8_t type)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    PROXY_CLI(client)->proxy_client_type = type;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

void bt_mesh_ext_proxy_server_set_msg_recv(void *client, uint8_t val)
{
#if CONFIG_BLE_MESH_GATT_PROXY_SERVER
    PROXY_CLI(client)->proxy_msg_recv = !!val;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

/* Proxy Client */
bool bt_mesh_ext_proxy_client_beacon_send(void *sub, bool private)
{
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    return bt_mesh_proxy_client_beacon_send(sub, private);
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
}

uint8_t bt_mesh_ext_proxy_client_get_opcode(void *cfg)
{
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    return PROXY_CFG(cfg)->opcode;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
}

uint8_t bt_mesh_ext_proxy_client_get_use_directed(void *cfg)
{
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    return PROXY_CFG(cfg)->direct_proxy_ctrl.use_directed;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
}

void *bt_mesh_ext_proxy_client_get_client_uar(void *cfg)
{
#if CONFIG_BLE_MESH_GATT_PROXY_CLIENT
    return &PROXY_CFG(cfg)->direct_proxy_ctrl.proxy_client_uar;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
}

/* Remote Provisioning */
int bt_mesh_ext_rpr_cli_pdu_send(void *link, uint8_t type)
{
#if CONFIG_BLE_MESH_RPR_CLI
    return bt_mesh_rpr_cli_pdu_send(link, type);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_RPR_CLI */
}

int bt_mesh_ext_rpr_cli_recv_pub_key_outbound_report(void *link)
{
#if CONFIG_BLE_MESH_RPR_CLI
    return bt_mesh_rpr_cli_recv_pub_key_outbound_report(link);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_RPR_CLI */
}

int bt_mesh_ext_rpr_cli_pdu_recv(void *link, uint8_t type, struct net_buf_simple *buf)
{
#if CONFIG_BLE_MESH_RPR_CLI
    return bt_mesh_rpr_cli_pdu_recv(link, type, buf);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_RPR_CLI */
}

#if CONFIG_BLE_MESH_RPR_CLI
struct bt_mesh_prov_link rpr_links[CONFIG_BLE_MESH_RPR_CLI_PROV_SAME_TIME];
#endif /* CONFIG_BLE_MESH_RPR_CLI */

bool bt_mesh_ext_bt_mesh_is_unprov_dev_being_prov(void *uuid)
{
#if CONFIG_BLE_MESH_RPR_CLI
    return bt_mesh_is_unprov_dev_being_prov(uuid);
#else
    assert(0);
    return 0;
#endif
}

void *bt_mesh_ext_rpr_cli_get_rpr_link(uint8_t index)
{
#if CONFIG_BLE_MESH_RPR_CLI
    return &rpr_links[index];
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_RPR_CLI */
}

void bt_mesh_ext_rpr_srv_reset_prov_link(void *link, uint8_t reason)
{
#if CONFIG_BLE_MESH_RPR_SRV
    bt_mesh_rpr_srv_reset_prov_link(link, reason);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_RPR_SRV */
}

int bt_mesh_ext_rpr_srv_nppi_pdu_recv(uint8_t type, const uint8_t *data)
{
#if CONFIG_BLE_MESH_RPR_SRV
    return bt_mesh_rpr_srv_nppi_pdu_recv(type, data);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_RPR_SRV */
}

int bt_mesh_ext_rpr_srv_set_waiting_prov_link(void* link, bt_mesh_addr_t *addr)
{
#if (CONFIG_BLE_MESH_PB_GATT && CONFIG_BLE_MESH_RPR_SRV)
    return bt_mesh_rpr_srv_set_waiting_prov_link(link, addr);
#else
    assert(0);
    return 0;
#endif /*  CONFIG_BLE_MESH_PB_GATT && CONFIG_BLE_MESH_RPR_SRV) */
}

/* Friend */
uint16_t bt_mesh_ext_frnd_get_lpn(void *frnd)
{
#if CONFIG_BLE_MESH_FRIEND
    return FRIEND(frnd)->lpn;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

uint8_t bt_mesh_ext_frnd_get_num_elem(void *frnd)
{
#if CONFIG_BLE_MESH_FRIEND
    return FRIEND(frnd)->num_elem;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

bool bt_mesh_ext_frnd_is_valid(void *frnd)
{
#if CONFIG_BLE_MESH_FRIEND
    return FRIEND(frnd)->valid;
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

bool bt_mesh_ext_frnd_is_established(void *frnd)
{
#if CONFIG_BLE_MESH_FRIEND
    return FRIEND(frnd)->established;
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

uint16_t bt_mesh_ext_frnd_get_net_idx(void *frnd)
{
#if CONFIG_BLE_MESH_FRIEND
    return FRIEND(frnd)->net_idx;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

uint8_t bt_mesh_ext_frnd_get_sub_list_count(void *frnd)
{
#if CONFIG_BLE_MESH_FRIEND
    return ARRAY_SIZE(FRIEND(frnd)->sub_list);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

uint16_t bt_mesh_ext_frnd_get_sub_list_size(void *frnd)
{
#if CONFIG_BLE_MESH_FRIEND
    return sizeof(FRIEND(frnd)->sub_list);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

uint16_t bt_mesh_ext_frnd_get_sub_addr(void *frnd, uint8_t index)
{
#if CONFIG_BLE_MESH_FRIEND
    return FRIEND(frnd)->sub_list[index];
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

uint8_t bt_mesh_ext_net_get_frnd_count(void)
{
#if CONFIG_BLE_MESH_FRIEND
    return ARRAY_SIZE(bt_mesh.frnd);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

void *bt_mesh_ext_net_get_frnd(uint8_t index)
{
#if CONFIG_BLE_MESH_FRIEND
    return &bt_mesh.frnd[index];
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

bool bt_mesh_ext_friend_match(uint16_t net_idx, uint16_t addr)
{
#if CONFIG_BLE_MESH_FRIEND
    return bt_mesh_friend_match(net_idx, addr);
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

bool bt_mesh_ext_friend_unicast_match(uint16_t net_idx, uint16_t addr, uint8_t *selem)
{
#if CONFIG_BLE_MESH_FRIEND
    return bt_mesh_friend_unicast_match(net_idx, addr, selem);
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

void *bt_mesh_ext_friend_find(uint16_t net_idx, uint16_t lpn_addr, bool valid, bool established)
{
#if CONFIG_BLE_MESH_FRIEND
    return bt_mesh_friend_find(net_idx, lpn_addr, valid, established);
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_FRIEND */
}

void bt_mesh_ext_friend_clear_net_idx(uint16_t net_idx)
{
#if CONFIG_BLE_MESH_FRIEND
    bt_mesh_friend_clear_net_idx(net_idx);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_FRIEND */
}

/* Low Power */
bool bt_mesh_ext_lpn_match(uint16_t addr)
{
#if CONFIG_BLE_MESH_LOW_POWER
    return bt_mesh_lpn_match(addr);
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_LOW_POWER */
}

uint16_t bt_mesh_ext_lpn_frnd(void)
{
#if CONFIG_BLE_MESH_LOW_POWER
    return bt_mesh.lpn.frnd;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_LOW_POWER */
}

/* RPL */
uint16_t bt_mesh_ext_rpl_get_src(void *rpl)
{
#if CONFIG_BLE_MESH_LOW_POWER
    return RPL(rpl)->src;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_LOW_POWER */
}

bool bt_mesh_ext_rpl_get_old_iv(void *rpl)
{
#if CONFIG_BLE_MESH_LOW_POWER
    return RPL(rpl)->old_iv;
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_LOW_POWER */
}

uint32_t bt_mesh_ext_rpl_get_seq(void *rpl)
{
#if CONFIG_BLE_MESH_LOW_POWER
    return RPL(rpl)->seq;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_LOW_POWER */
}

void bt_mesh_ext_update_rpl(void *rpl, void *rx)
{
#if CONFIG_BLE_MESH_LOW_POWER
    bt_mesh_update_rpl(RPL(rpl), NET_RX(rx));
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_LOW_POWER */
}

/* Adv */
uint8_t bt_mesh_ext_adv_data_get_type(const void *ad)
{
    return ADV_DATA(ad)->type;
}

const uint8_t *bt_mesh_ext_adv_data_get_data(const void *ad)
{
    return ADV_DATA(ad)->data;
}

void bt_mesh_ext_adv_update(void)
{
    bt_mesh_adv_update();
}

void *bt_mesh_ext_adv_create(enum bt_mesh_adv_type type, int32_t timeout)
{
    return bt_mesh_adv_create(type, timeout);
}

void bt_mesh_ext_adv_send(void *buf, uint8_t xmit,
                          const struct bt_mesh_send_cb *cb,
                          void *cb_data)
{
    bt_mesh_adv_send(buf, xmit, cb, cb_data);
}

uint8_t bt_mesh_ext_set_random_addr(uint8_t random_bda[6])
{
#if CONFIG_BT_BLUEDROID_ENABLED
    extern tBTM_STATUS btm_ble_set_random_addr(BD_ADDR random_bda);
    return btm_ble_set_random_addr(random_bda);
#else
    return 0;
#endif
}

void bt_mesh_ext_set_static_addr(uint8_t rand_addr[6])
{
#if CONFIG_BT_BLUEDROID_ENABLED
    extern void BTM_BleSetStaticAddr(BD_ADDR rand_addr);
    BTM_BleSetStaticAddr(rand_addr);
#else
#endif
}

const char *bt_mesh_ext_get_device_name(void)
{
#if (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_SERVER
    return bt_mesh_get_device_name();
#else
    assert(0);
    return NULL;
#endif /* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
           CONFIG_BLE_MESH_GATT_PROXY_SERVER */
}

/* SCAN */
int bt_mesh_ext_update_exceptional_list(uint8_t sub_code, uint32_t type, void *info)
{
#if CONFIG_BLE_MESH_USE_DUPLICATE_SCAN
    return bt_mesh_update_exceptional_list(sub_code, type, info);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
}

int bt_mesh_ext_unprov_dev_info_query(uint8_t uuid[16], uint8_t addr[6],
                                      uint8_t *adv_type, uint8_t query_type)
{
#if (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV)
    return bt_mesh_unprov_dev_info_query(uuid, addr, adv_type, query_type);
#else
    assert(0);
    return 0;
#endif /* (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) */
}

/* GATT */
int bt_mesh_ext_gattc_conn_create(const void *addr, uint16_t service_uuid)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
    return bt_mesh_gattc_conn_create(addr, service_uuid);
#else
    assert(0);
    return 0;
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
            CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
           (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT) */
}

void bt_mesh_ext_gattc_disconnect(void *conn)
{
#if (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
     CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
    (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT)
    bt_mesh_gattc_disconnect(conn);
#else
    assert(0);
#endif /* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
            CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
           (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT) */
}

/* Settings */
void bt_mesh_ext_store_dkca(void)
{
#if CONFIG_BLE_MESH_SETTINGS
    bt_mesh_store_dkca();
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

void bt_mesh_ext_clear_dkca(void)
{
#if CONFIG_BLE_MESH_SETTINGS
    bt_mesh_clear_dkca();
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

void bt_mesh_ext_clear_rpl_single(uint16_t src)
{
#if CONFIG_BLE_MESH_SETTINGS
    bt_mesh_clear_rpl_single(src);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

void bt_mesh_ext_store_net(void)
{
#if CONFIG_BLE_MESH_SETTINGS
    bt_mesh_store_net();
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

void bt_mesh_ext_store_seq(void)
{
#if CONFIG_BLE_MESH_SETTINGS
    bt_mesh_store_seq();
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

struct net_buf_simple *bt_mesh_ext_get_core_settings_item(const char *key)
{
#if CONFIG_BLE_MESH_SETTINGS
    return bt_mesh_get_core_settings_item(key);
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

bool bt_mesh_ext_is_settings_item_exist(struct net_buf_simple *buf, const uint16_t val)
{
#if CONFIG_BLE_MESH_SETTINGS
    return bt_mesh_is_settings_item_exist(buf, val);
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

int bt_mesh_ext_add_core_settings_item(const char *key, const uint16_t val)
{
#if CONFIG_BLE_MESH_SETTINGS
    return bt_mesh_add_core_settings_item(key, val);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

int bt_mesh_ext_remove_core_settings_item(const char *key, const uint16_t val)
{
#if CONFIG_BLE_MESH_SETTINGS
    return bt_mesh_remove_core_settings_item(key, val);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

int bt_mesh_ext_save_core_settings(const char *key, const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_SETTINGS
    return bt_mesh_save_core_settings(key, val, len);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

int bt_mesh_ext_erase_core_settings(const char *key)
{
#if CONFIG_BLE_MESH_SETTINGS
    return bt_mesh_erase_core_settings(key);
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_SETTINGS */
}

/* struct bt_mesh_subnet */
uint32_t bt_mesh_ext_sub_get_mpb_sent(void *sub)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return SUBNET(sub)->mpb_sent;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

void bt_mesh_ext_sub_set_mpb_sent(void *sub, uint32_t mpb_sent)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    SUBNET(sub)->mpb_sent = mpb_sent;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

uint8_t bt_mesh_ext_sub_get_mpb_last(void *sub)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return SUBNET(sub)->mpb_last;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

void bt_mesh_ext_sub_set_mpb_last(void *sub, uint8_t mpb_last)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    SUBNET(sub)->mpb_last = mpb_last;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

uint8_t bt_mesh_ext_sub_get_mpb_cur(void *sub)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return SUBNET(sub)->mpb_cur;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

void bt_mesh_ext_sub_set_mpb_cur(void *sub, uint8_t mpb_cur)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    SUBNET(sub)->mpb_cur = mpb_cur;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

void bt_mesh_ext_sub_inc_mpb_cur(void *sub)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    SUBNET(sub)->mpb_cur++;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

uint8_t bt_mesh_ext_sub_get_mpb_flags_last(void *sub)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return SUBNET(sub)->mpb_flags_last;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

void bt_mesh_ext_sub_set_mpb_flags_last(void *sub, uint8_t mpb_flags_last)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    SUBNET(sub)->mpb_flags_last = mpb_flags_last;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

uint8_t bt_mesh_ext_sub_get_mpb_ivi_last(void *sub)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return SUBNET(sub)->mpb_ivi_last;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

void bt_mesh_ext_sub_set_mpb_ivi_last(void *sub, uint8_t mpb_ivi_last)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    SUBNET(sub)->mpb_ivi_last = !!mpb_ivi_last;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

uint8_t *bt_mesh_ext_sub_get_mpb_random(void *sub)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return SUBNET(sub)->mpb_random;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

uint8_t bt_mesh_ext_sub_get_private_node_id(void *sub)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return SUBNET(sub)->private_node_id;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

uint16_t bt_mesh_ext_sub_get_net_idx(void *sub)
{
    return SUBNET(sub)->net_idx;
}

uint16_t bt_mesh_ext_sub_get_sbr_net_idx(void *sub)
{
#if CONFIG_BLE_MESH_BRC_SRV
    return SUBNET(sub)->sbr_net_idx;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_BRC_SRV */
}

void bt_mesh_ext_sub_set_sbr_net_idx(void *sub, uint16_t sbr_net_idx)
{
#if CONFIG_BLE_MESH_BRC_SRV
    SUBNET(sub)->sbr_net_idx = sbr_net_idx;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_BRC_SRV */
}

bool bt_mesh_ext_sub_get_kr_flag(void *sub)
{
    return SUBNET(sub)->kr_flag;
}

uint8_t bt_mesh_ext_sub_get_kr_phase(void *sub)
{
    return SUBNET(sub)->kr_phase;
}

uint8_t *bt_mesh_ext_sub_get_keys_net(void *sub, uint8_t index)
{
    return SUBNET(sub)->keys[index].net;
}

uint8_t bt_mesh_ext_sub_get_keys_nid(void *sub, uint8_t index)
{
    return SUBNET(sub)->keys[index].nid;
}

uint8_t *bt_mesh_ext_sub_get_keys_enc(void *sub, uint8_t index)
{
    return SUBNET(sub)->keys[index].enc;
}

uint8_t *bt_mesh_ext_sub_get_keys_privacy(void *sub, uint8_t index)
{
    return SUBNET(sub)->keys[index].privacy;
}

uint8_t *bt_mesh_ext_sub_get_keys_private_beacon(void *sub, uint8_t index)
{
#if CONFIG_BLE_MESH_PRIVATE_BEACON
    return SUBNET(sub)->keys[index].private_beacon;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_PRIVATE_BEACON */
}

uint8_t bt_mesh_ext_sub_get_keys_direct_nid(void *sub, uint8_t index)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->keys[index].direct_nid;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t *bt_mesh_ext_sub_get_keys_direct_enc(void *sub, uint8_t index)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->keys[index].direct_enc;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t *bt_mesh_ext_sub_get_keys_direct_privacy(void *sub, uint8_t index)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->keys[index].direct_privacy;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_proxy_privacy(void *sub)
{
    return SUBNET(sub)->proxy_privacy;
}

void bt_mesh_ext_sub_set_proxy_privacy(void *sub, uint8_t proxy_privacy)
{
    SUBNET(sub)->proxy_privacy = proxy_privacy;
}

uint8_t bt_mesh_ext_sub_get_directed_forwarding(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->directed_forwarding;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_directed_forwarding(void *sub, uint8_t directed_forwarding)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->directed_forwarding = directed_forwarding;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_directed_relay(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->directed_relay;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_directed_relay(void *sub, uint8_t directed_relay)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->directed_relay = directed_relay;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_directed_proxy(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->directed_proxy;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_directed_proxy(void *sub, uint8_t directed_proxy)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->directed_proxy = directed_proxy;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_directed_proxy_use_default(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->directed_proxy_use_default;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_directed_proxy_use_default(void *sub, uint8_t directed_proxy_use_default)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->directed_proxy_use_default = directed_proxy_use_default;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_directed_friend(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->directed_friend;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_directed_friend(void *sub, uint8_t directed_friend)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->directed_friend = directed_friend;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_use_directed(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->use_directed;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_use_directed(void *sub, uint8_t use_directed)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->use_directed = use_directed;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void *bt_mesh_ext_sub_get_proxy_client_uar(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return &SUBNET(sub)->proxy_client_uar;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

bool bt_mesh_ext_sub_get_proxy_client_uar_len_present(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->proxy_client_uar.len_present;
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_proxy_client_uar_len_present(void *sub, bool len_present)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->proxy_client_uar.len_present = len_present;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint16_t bt_mesh_ext_sub_get_proxy_client_uar_range_start(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->proxy_client_uar.range_start;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_proxy_client_uar_range_start(void *sub, uint16_t range_start)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->proxy_client_uar.range_start = range_start;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_proxy_client_uar_range_length(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->proxy_client_uar.range_length;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_proxy_client_uar_range_length(void *sub, uint8_t range_length)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->proxy_client_uar.range_length = range_length;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_path_metric_type(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->path_metric_type;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_path_metric_type(void *sub, uint8_t path_metric_type)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->path_metric_type = path_metric_type;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_path_lifetime_type(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->path_lifetime_type;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_path_lifetime_type(void *sub, uint8_t path_lifetime_type)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->path_lifetime_type = path_lifetime_type;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

bool bt_mesh_ext_sub_get_two_way_path(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->two_way_path;
#else
    assert(0);
    return false;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_two_way_path(void *sub, bool two_way_path)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->two_way_path = two_way_path;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_forward_number(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->forward_number;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_forward_number(void *sub, uint8_t forward_number)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->forward_number = forward_number;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_disc_table_max_disc_entries(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->discovery_table.max_disc_entries;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_disc_table_max_disc_entries(void *sub, uint8_t max_disc_entries)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->discovery_table.max_disc_entries = max_disc_entries;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_disc_table_max_concurr_init(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->discovery_table.max_concurr_init;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_disc_table_max_concurr_init(void *sub, uint8_t max_concurr_init)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->discovery_table.max_concurr_init = max_concurr_init;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_disc_table_concurr_init(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->discovery_table.concurr_init;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_disc_table_concurr_init(void *sub, uint8_t concurr_init)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->discovery_table.concurr_init = concurr_init;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_inc_disc_table_concurr_init(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->discovery_table.concurr_init++;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_dec_disc_table_concurr_init(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->discovery_table.concurr_init--;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void *bt_mesh_ext_sub_get_disc_table_mutex(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return &SUBNET(sub)->discovery_table.mutex;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void *bt_mesh_ext_sub_get_disc_table_entries(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return &SUBNET(sub)->discovery_table.entries;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_fwd_table_max_ford_entries(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->forward_table.max_ford_entries;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_fwd_table_max_ford_entries(void *sub, uint8_t max_ford_entries)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->forward_table.max_ford_entries = max_ford_entries;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_fwd_table_max_deps_nodes(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->forward_table.max_deps_nodes;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_fwd_table_max_deps_nodes(void *sub, uint8_t max_deps_nodes)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->forward_table.max_deps_nodes = max_deps_nodes;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint16_t bt_mesh_ext_sub_get_fwd_table_update_id(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->forward_table.update_id;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_fwd_table_update_id(void *sub, uint16_t update_id)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->forward_table.update_id = update_id;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_inc_fwd_table_update_id(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->forward_table.update_id++;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void *bt_mesh_ext_sub_get_fwd_table_mutex(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return &SUBNET(sub)->forward_table.mutex;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void *bt_mesh_ext_sub_get_fwd_table_entries(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return &SUBNET(sub)->forward_table.entries;
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_wanted_lanes(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->wanted_lanes;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_wanted_lanes(void *sub, uint8_t wanted_lanes)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->wanted_lanes = wanted_lanes;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_unicast_echo_interval(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->unicast_echo_interval;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_unicast_echo_interval(void *sub, uint8_t unicast_echo_interval)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->unicast_echo_interval = unicast_echo_interval;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

uint8_t bt_mesh_ext_sub_get_multicast_echo_interval(void *sub)
{
#if CONFIG_BLE_MESH_DF_SRV
    return SUBNET(sub)->multicast_echo_interval;
#else
    assert(0);
    return 0;
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_sub_set_multicast_echo_interval(void *sub, uint8_t multicast_echo_interval)
{
#if CONFIG_BLE_MESH_DF_SRV
    SUBNET(sub)->multicast_echo_interval = multicast_echo_interval;
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

/* struct bt_mesh_net_tx */
void *bt_mesh_ext_net_tx_get_sub(void *tx)
{
    return NET_TX(tx)->sub;
}

void bt_mesh_ext_net_tx_set_sub(void *tx, void *sub)
{
    NET_TX(tx)->sub = sub;
}

void bt_mesh_ext_net_tx_set_ctx(void *tx, void *ctx)
{
    NET_TX(tx)->ctx = ctx;
}

uint16_t bt_mesh_ext_net_tx_get_net_idx(void *tx)
{
    return NET_TX(tx)->ctx->net_idx;
}

uint16_t bt_mesh_ext_net_tx_get_app_idx(void *tx)
{
    return NET_TX(tx)->ctx->app_idx;
}

uint16_t bt_mesh_ext_net_tx_get_addr(void *tx)
{
    return NET_TX(tx)->ctx->addr;
}

uint8_t bt_mesh_ext_net_tx_get_send_ttl(void *tx)
{
    return NET_TX(tx)->ctx->send_ttl;
}

uint8_t bt_mesh_ext_net_tx_get_send_cred(void *tx)
{
    return NET_TX(tx)->ctx->send_cred;
}

void bt_mesh_ext_net_tx_set_send_cred(void *tx, uint8_t send_cred)
{
    NET_TX(tx)->ctx->send_cred = send_cred;
}

uint8_t bt_mesh_ext_net_tx_get_send_tag(void *tx)
{
    return NET_TX(tx)->ctx->send_tag;
}

void bt_mesh_ext_net_tx_set_send_tag(void *tx, uint8_t tag)
{
    NET_TX(tx)->ctx->send_tag = tag;
}

uint16_t bt_mesh_ext_net_tx_get_src(void *tx)
{
    return NET_TX(tx)->src;
}

void bt_mesh_ext_net_tx_set_src(void *tx, uint16_t src)
{
    NET_TX(tx)->src = src;
}

void bt_mesh_ext_net_tx_set_xmit(void *tx, uint8_t xmit)
{
    NET_TX(tx)->xmit = xmit;
}

/* struct bt_mesh_net_rx */
void *bt_mesh_ext_net_rx_get_sub(void *rx)
{
    return NET_RX(rx)->sub;
}

void bt_mesh_ext_net_rx_set_sub(void *rx, void *sub)
{
    NET_RX(rx)->sub = sub;
}

uint16_t bt_mesh_ext_net_rx_get_net_idx(void *rx)
{
    return NET_RX(rx)->ctx.net_idx;
}

void bt_mesh_ext_net_rx_set_net_idx(void *rx, uint16_t net_idx)
{
    NET_RX(rx)->ctx.net_idx = net_idx;
}

uint16_t bt_mesh_ext_net_rx_get_app_idx(void *rx)
{
    return NET_RX(rx)->ctx.app_idx;
}

void bt_mesh_ext_net_rx_set_app_idx(void *rx, uint16_t app_idx)
{
    NET_RX(rx)->ctx.app_idx = app_idx;
}

uint16_t bt_mesh_ext_net_rx_get_addr(void *rx)
{
    return NET_RX(rx)->ctx.addr;
}

void bt_mesh_ext_net_rx_set_addr(void *rx, uint16_t addr)
{
    NET_RX(rx)->ctx.addr = addr;
}

uint16_t bt_mesh_ext_net_rx_get_recv_dst(void *rx)
{
    return NET_RX(rx)->ctx.recv_dst;
}

void bt_mesh_ext_net_rx_set_recv_dst(void *rx, uint16_t recv_dst)
{
    NET_RX(rx)->ctx.recv_dst = recv_dst;
}

int8_t bt_mesh_ext_net_rx_get_recv_rssi(void *rx)
{
    return NET_RX(rx)->ctx.recv_rssi;
}

void bt_mesh_ext_net_rx_set_recv_rssi(void *rx, int8_t recv_rssi)
{
    NET_RX(rx)->ctx.recv_rssi = recv_rssi;
}

uint32_t bt_mesh_ext_net_rx_get_recv_op(void *rx)
{
    return NET_RX(rx)->ctx.recv_op;
}

void bt_mesh_ext_net_rx_set_recv_op(void *rx, uint32_t recv_op)
{
    NET_RX(rx)->ctx.recv_op = recv_op;
}

uint8_t bt_mesh_ext_net_rx_get_recv_ttl(void *rx)
{
    return NET_RX(rx)->ctx.recv_ttl;
}

void bt_mesh_ext_net_rx_set_recv_ttl(void *rx, uint8_t recv_ttl)
{
    NET_RX(rx)->ctx.recv_ttl = recv_ttl;
}

uint8_t bt_mesh_ext_net_rx_get_recv_cred(void *rx)
{
    return NET_RX(rx)->ctx.recv_cred;
}

void bt_mesh_ext_net_rx_set_recv_cred(void *rx, uint8_t recv_cred)
{
    NET_RX(rx)->ctx.recv_cred = recv_cred;
}

uint8_t bt_mesh_ext_net_rx_get_recv_tag(void *rx)
{
    return NET_RX(rx)->ctx.recv_tag;
}

void bt_mesh_ext_net_rx_set_recv_tag(void *rx, uint8_t recv_tag)
{
    NET_RX(rx)->ctx.recv_tag = recv_tag;
}

uint8_t bt_mesh_ext_net_rx_get_send_ttl(void *rx)
{
    return NET_RX(rx)->ctx.send_ttl;
}

void bt_mesh_ext_net_rx_set_send_ttl(void *rx, uint8_t send_ttl)
{
    NET_RX(rx)->ctx.send_ttl = send_ttl;
}

uint8_t bt_mesh_ext_net_rx_get_send_cred(void *rx)
{
    return NET_RX(rx)->ctx.send_cred;
}

void bt_mesh_ext_net_rx_set_send_cred(void *rx, uint8_t send_cred)
{
    NET_RX(rx)->ctx.send_cred = send_cred;
}

uint8_t bt_mesh_ext_net_rx_get_send_tag(void *rx)
{
    return NET_RX(rx)->ctx.send_tag;
}

void bt_mesh_ext_net_rx_set_send_tag(void *rx, uint8_t send_tag)
{
    NET_RX(rx)->ctx.send_tag = send_tag;
}

uint32_t bt_mesh_ext_net_rx_get_seq(void *rx)
{
    return NET_RX(rx)->seq;
}

void bt_mesh_ext_net_rx_set_seq(void *rx, uint32_t seq)
{
    NET_RX(rx)->seq = seq;
}

bool bt_mesh_ext_net_rx_get_new_key(void *rx)
{
    return NET_RX(rx)->new_key;
}

void bt_mesh_ext_net_rx_set_new_key(void *rx, bool new_key)
{
    NET_RX(rx)->new_key = new_key;
}

bool bt_mesh_ext_net_rx_get_ctl(void *rx)
{
    return NET_RX(rx)->ctl;
}

void bt_mesh_ext_net_rx_set_ctl(void *rx, bool ctl)
{
    NET_RX(rx)->ctl = ctl;
}

uint8_t bt_mesh_ext_net_rx_get_net_if(void *rx)
{
    return NET_RX(rx)->net_if;
}

void bt_mesh_ext_net_rx_set_net_if(void *rx, uint8_t net_if)
{
    NET_RX(rx)->net_if = net_if;
}

bool bt_mesh_ext_net_rx_get_old_iv(void *rx)
{
    return NET_RX(rx)->old_iv;
}

bool bt_mesh_ext_net_rx_get_sbr_rpl(void *rx)
{
    return NET_RX(rx)->sbr_rpl;
}

void bt_mesh_ext_net_rx_set_sbr_rpl(void *rx, bool sbr_rpl)
{
    NET_RX(rx)->sbr_rpl = sbr_rpl;
}

/* struct bt_mesh_msg_ctx */
uint16_t bt_mesh_ext_msg_ctx_get_net_idx(void *ctx)
{
    return MSG_CTX(ctx)->net_idx;
}

void bt_mesh_ext_msg_ctx_set_net_idx(void *ctx, uint16_t net_idx)
{
    MSG_CTX(ctx)->net_idx = net_idx;
}

uint16_t bt_mesh_ext_msg_ctx_get_app_idx(void *ctx)
{
    return MSG_CTX(ctx)->app_idx;
}

void bt_mesh_ext_msg_ctx_set_app_idx(void *ctx, uint16_t app_idx)
{
    MSG_CTX(ctx)->app_idx = app_idx;
}

uint16_t bt_mesh_ext_msg_ctx_get_addr(void *ctx)
{
    return MSG_CTX(ctx)->addr;
}

void bt_mesh_ext_msg_ctx_set_addr(void *ctx, uint16_t addr)
{
    MSG_CTX(ctx)->addr = addr;
}

uint16_t bt_mesh_ext_msg_ctx_get_recv_dst(void *ctx)
{
    return MSG_CTX(ctx)->recv_dst;
}

void bt_mesh_ext_msg_ctx_set_recv_dst(void *ctx, uint16_t recv_dst)
{
    MSG_CTX(ctx)->recv_dst = recv_dst;
}

int8_t bt_mesh_ext_msg_ctx_get_recv_rssi(void *ctx)
{
    return MSG_CTX(ctx)->recv_rssi;
}

void bt_mesh_ext_msg_ctx_set_recv_rssi(void *ctx, int8_t recv_rssi)
{
    MSG_CTX(ctx)->recv_rssi = recv_rssi;
}

uint32_t bt_mesh_ext_msg_ctx_get_recv_op(void *ctx)
{
    return MSG_CTX(ctx)->recv_op;
}

void bt_mesh_ext_msg_ctx_set_recv_op(void *ctx, uint32_t recv_op)
{
    MSG_CTX(ctx)->recv_op = recv_op;
}

uint8_t bt_mesh_ext_msg_ctx_get_recv_ttl(void *ctx)
{
    return MSG_CTX(ctx)->recv_ttl;
}

void bt_mesh_ext_msg_ctx_set_recv_ttl(void *ctx, uint8_t recv_ttl)
{
    MSG_CTX(ctx)->recv_ttl = recv_ttl;
}

uint8_t bt_mesh_ext_msg_ctx_get_recv_cred(void *ctx)
{
    return MSG_CTX(ctx)->recv_cred;
}

void bt_mesh_ext_msg_ctx_set_recv_cred(void *ctx, uint8_t recv_cred)
{
    MSG_CTX(ctx)->recv_cred = recv_cred;
}

uint8_t bt_mesh_ext_msg_ctx_get_recv_tag(void *ctx)
{
    return MSG_CTX(ctx)->recv_tag;
}

void bt_mesh_ext_msg_ctx_set_recv_tag(void *ctx, uint8_t recv_tag)
{
    MSG_CTX(ctx)->recv_tag = recv_tag;
}

uint8_t bt_mesh_ext_msg_ctx_get_send_szmic(void *ctx)
{
    return MSG_CTX(ctx)->send_szmic;
}

void bt_mesh_ext_msg_ctx_set_send_szmic(void *ctx, uint8_t send_szmic)
{
    MSG_CTX(ctx)->send_szmic = send_szmic;
}

uint8_t bt_mesh_ext_msg_ctx_get_send_ttl(void *ctx)
{
    return MSG_CTX(ctx)->send_ttl;
}

void bt_mesh_ext_msg_ctx_set_send_ttl(void *ctx, uint8_t send_ttl)
{
    MSG_CTX(ctx)->send_ttl = send_ttl;
}

uint8_t bt_mesh_ext_msg_ctx_get_send_cred(void *ctx)
{
    return MSG_CTX(ctx)->send_cred;
}

void bt_mesh_ext_msg_ctx_set_send_cred(void *ctx, uint8_t send_cred)
{
    MSG_CTX(ctx)->send_cred = send_cred;
}

uint8_t bt_mesh_ext_msg_ctx_get_send_tag(void *ctx)
{
    return MSG_CTX(ctx)->send_tag;
}

void bt_mesh_ext_msg_ctx_set_send_tag(void *ctx, uint8_t send_tag)
{
    MSG_CTX(ctx)->send_tag = send_tag;
}

/* Client */
int bt_mesh_ext_client_clear_list(void *data)
{
    return bt_mesh_client_clear_list(data);
}

int bt_mesh_ext_client_free_node(void *node)
{
    return bt_mesh_client_free_node(node);
}

uint32_t bt_mesh_ext_client_common_get_opcode(void *param)
{
    return CLI_PARAM(param)->opcode;
}

void *bt_mesh_ext_client_common_get_model(void *param)
{
    return CLI_PARAM(param)->model;
}

void bt_mesh_ext_client_common_set_opcode(void *param, uint32_t opcode)
{
    CLI_PARAM(param)->opcode = opcode;
}

void bt_mesh_ext_client_common_set_model(void *param, void *model)
{
    CLI_PARAM(param)->model = model;
}

void *bt_mesh_ext_client_common_get_ctx(void *param)
{
    return &CLI_PARAM(param)->ctx;
}

uint16_t bt_mesh_ext_client_common_get_net_idx(void *param)
{
    return CLI_PARAM(param)->ctx.net_idx;
}

void bt_mesh_ext_client_common_set_net_idx(void *param, uint16_t net_idx)
{
    CLI_PARAM(param)->ctx.net_idx = net_idx;
}

uint16_t bt_mesh_ext_client_common_get_app_idx(void *param)
{
    return CLI_PARAM(param)->ctx.app_idx;
}

void bt_mesh_ext_client_common_set_app_idx(void *param, uint16_t app_idx)
{
    CLI_PARAM(param)->ctx.app_idx = app_idx;
}

uint16_t bt_mesh_ext_client_common_get_addr(void *param)
{
    return CLI_PARAM(param)->ctx.addr;
}

void bt_mesh_ext_client_common_set_addr(void *param, uint16_t addr)
{
    CLI_PARAM(param)->ctx.addr = addr;
}

uint8_t bt_mesh_ext_client_common_get_send_ttl(void *param)
{
    return CLI_PARAM(param)->ctx.send_ttl;
}

void bt_mesh_ext_client_common_set_send_ttl(void *param, uint8_t send_ttl)
{
    CLI_PARAM(param)->ctx.send_ttl = send_ttl;
}

void bt_mesh_ext_client_common_set_cb(void *param, void *cb)
{
    CLI_PARAM(param)->cb = cb;
}

void bt_mesh_ext_client_common_set_cb_data(void *param, void *cb_data)
{
    CLI_PARAM(param)->cb_data = cb_data;
}

void *bt_mesh_ext_client_node_get_with_work(void *work)
{
    return CONTAINER_OF(work, bt_mesh_client_node_t, timer.work);
}

uint32_t bt_mesh_ext_client_node_get_opcode(void *node)
{
    return CLI_NODE(node)->opcode;
}

void *bt_mesh_ext_client_node_get_model(void *node)
{
    return CLI_NODE(node)->model;
}

void *bt_mesh_ext_client_node_get_timer(void *node)
{
    return &CLI_NODE(node)->timer;
}

void *bt_mesh_ext_client_node_get_ctx(void *node)
{
    return &CLI_NODE(node)->ctx;
}

void *bt_mesh_ext_is_client_recv_publish_msg(void *model, void *ctx,
                                             struct net_buf_simple *buf,
                                             bool need_pub)
{
    return bt_mesh_is_client_recv_publish_msg(model, ctx, buf, need_pub);
}

int bt_mesh_ext_client_send_msg(void *param, struct net_buf_simple *msg,
                                bool need_ack, void (*timeout_cb)(void *work))
{
    return bt_mesh_client_send_msg(param, msg, need_ack, VOID(timeout_cb));
}

#if CONFIG_BLE_MESH_BRC_SRV
struct bt_mesh_subnet_bridge_table {
    uint8_t  bridge_direction;
    uint8_t  bridge_net_idx[3];
    uint16_t bridge_addr_1;
    uint16_t bridge_addr_2;
};

struct bt_mesh_bridge_cfg_srv {
    struct bt_mesh_model *model;

    uint8_t  subnet_bridge;
    uint16_t bridging_table_size;
    struct bt_mesh_subnet_bridge_table bridge_table[CONFIG_BLE_MESH_MAX_BRIDGING_TABLE_ENTRY_COUNT];
};

static struct bt_mesh_rpl bridge_rpl[CONFIG_BLE_MESH_BRIDGE_CRPL];
#endif /* CONFIG_BLE_MESH_BRC_SRV */

void *bt_mesh_ext_brc_srv_get_bridge_table_entry(void *srv, uint8_t index)
{
#if CONFIG_BLE_MESH_BRC_SRV
    return &((struct bt_mesh_bridge_cfg_srv *)srv)->bridge_table[index];
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_BRC_SRV */
}

void *bt_mesh_ext_brc_srv_get_bridge_rpl(uint16_t index)
{
#if CONFIG_BLE_MESH_BRC_SRV
    return &bridge_rpl[index];
#else
    assert(0);
    return NULL;
#endif /* CONFIG_BLE_MESH_BRC_SRV */
}

/* BTC */
void bt_mesh_ext_agg_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                          void *model, void *ctx,
                                          const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_AGG_CLI
    bt_mesh_agg_client_cb_evt_to_btc(opcode, event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_AGG_CLI */
}

void bt_mesh_ext_agg_server_cb_evt_to_btc(uint8_t event, void *model, void *ctx,
                                          const void *val, size_t len)
{
#if CONFIG_BLE_MESH_AGG_SRV
    bt_mesh_agg_server_cb_evt_to_btc(event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_AGG_SRV */
}

void bt_mesh_ext_brc_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                          void *model, void *ctx,
                                          const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_BRC_CLI
    bt_mesh_brc_client_cb_evt_to_btc(opcode, event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_BRC_CLI */
}

void bt_mesh_ext_brc_server_cb_evt_to_btc(uint8_t event, void *model, void *ctx,
                                          const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_BRC_SRV
    bt_mesh_brc_server_cb_evt_to_btc(event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_BRC_SRV */
}

void bt_mesh_ext_df_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                         void *model, void *ctx,
                                         const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_DF_CLI
    bt_mesh_df_client_cb_evt_to_btc(opcode, event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_CLI */
}

void bt_mesh_ext_df_server_cb_evt_to_btc(uint8_t event, void *model, void *ctx,
                                         const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_DF_SRV
    bt_mesh_df_server_cb_evt_to_btc(event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_DF_SRV */
}

void bt_mesh_ext_lcd_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                          void *model, void *ctx,
                                          const void *val, size_t len)
{
#if CONFIG_BLE_MESH_LCD_CLI
    bt_mesh_lcd_client_cb_evt_to_btc(opcode, event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_LCD_CLI */
}

void bt_mesh_ext_odp_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                          void *model, void *ctx,
                                          const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_ODP_CLI
    bt_mesh_odp_client_cb_evt_to_btc(opcode, event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_ODP_CLI */
}

void bt_mesh_ext_prb_client_cb_evt_to_btc(uint32_t opcode, uint8_t evt_type,
                                          void *model, void *ctx,
                                          const uint8_t *val, uint16_t len)
{
#if CONFIG_BLE_MESH_PRB_CLI
    bt_mesh_prb_client_cb_evt_to_btc(opcode, evt_type, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_PRB_CLI */
}

void btc_ble_mesh_ext_rpr_client_link_close_cb(void *model, uint16_t rpr_srv_addr, uint8_t reason)
{
#if CONFIG_BLE_MESH_RPR_CLI
    btc_ble_mesh_rpr_client_link_close_cb(model, rpr_srv_addr, reason);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_RPR_CLI */
}

void btc_ble_mesh_ext_rpr_client_prov_comp_cb(void *model, uint16_t rpr_srv_addr,
                                              uint8_t nppi, uint16_t index,
                                              uint8_t uuid[16], uint16_t unicast_addr,
                                              uint8_t element_num, uint16_t net_idx)
{
#if CONFIG_BLE_MESH_RPR_CLI
    btc_ble_mesh_rpr_client_prov_comp_cb(model, rpr_srv_addr, nppi, index, uuid,
                                         unicast_addr, element_num, net_idx);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_RPR_CLI */
}

void bt_mesh_ext_rpr_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                          void *model, void *ctx,
                                          const void *val, size_t len)
{
#if CONFIG_BLE_MESH_RPR_CLI
    bt_mesh_rpr_client_cb_evt_to_btc(opcode, event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_RPR_CLI */
}

void bt_mesh_ext_rpr_server_cb_evt_to_btc(uint8_t event, const void *val, size_t len)
{
#if CONFIG_BLE_MESH_RPR_SRV
    bt_mesh_rpr_server_cb_evt_to_btc(event, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_RPR_SRV */
}

void bt_mesh_ext_sar_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                          void *model, void *ctx,
                                          const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_SAR_CLI
    bt_mesh_sar_client_cb_evt_to_btc(opcode, event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_SAR_CLI */
}

void bt_mesh_ext_srpl_client_cb_evt_to_btc(uint32_t opcode, uint8_t event,
                                           void *model, void *ctx,
                                           const uint8_t *val, size_t len)
{
#if CONFIG_BLE_MESH_SRPL_CLI
    bt_mesh_srpl_client_cb_evt_to_btc(opcode, event, model, ctx, val, len);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_SRPL_CLI */
}

void bt_mesh_ext_mbt_client_cb_evt_to_btc(uint8_t event, uint8_t result, void *model)
{
#if CONFIG_BLE_MESH_MBT_CLI
    bt_mesh_mbt_client_cb_evt_to_btc(event, result, model);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_MBT_CLI */
}

void bt_mesh_ext_mbt_server_cb_evt_to_btc(uint8_t event, void *model, void *ctx)
{
#if CONFIG_BLE_MESH_MBT_SRV
    bt_mesh_mbt_server_cb_evt_to_btc(event, model, ctx);
#else
    assert(0);
#endif /* CONFIG_BLE_MESH_MBT_SRV */
}

typedef struct {
    uint64_t config_ble_mesh_use_ble_50: 1;
    uint64_t config_ble_mesh_use_duplicate_scan : 1;
    uint64_t config_ble_mesh_pb_adv : 1;
    uint64_t config_ble_mesh_pb_gatt : 1;
    uint64_t config_ble_mesh_prov_epa : 1;
    uint64_t config_ble_mesh_cert_based_prov : 1;
    uint64_t config_ble_mesh_gatt_proxy_client : 1;
    uint64_t config_ble_mesh_gatt_proxy_server : 1;
    uint64_t config_ble_mesh_proxy_solic_pdu_rx : 1;
    uint64_t config_ble_mesh_proxy_solic_pdu_tx : 1;
    uint64_t config_ble_mesh_proxy_solic : 1;
    uint64_t config_ble_mesh_support_directed_proxy : 1;
    uint64_t config_ble_mesh_relay : 1;
    uint64_t config_ble_mesh_settings : 1;
    uint64_t config_ble_mesh_friend : 1;
    uint64_t config_ble_mesh_low_power : 1;
    uint64_t config_ble_mesh_agg_cli : 1;
    uint64_t config_ble_mesh_agg_srv : 1;
    uint64_t config_ble_mesh_brc_cli : 1;
    uint64_t config_ble_mesh_brc_srv : 1;
    uint64_t config_ble_mesh_df_cli : 1;
    uint64_t config_ble_mesh_df_srv : 1;
    uint64_t config_ble_mesh_path_monitor_test : 1;
    uint64_t config_ble_mesh_lcd_cli : 1;
    uint64_t config_ble_mesh_lcd_srv : 1;
    uint64_t config_ble_mesh_comp_data_1 : 1;
    uint64_t config_ble_mesh_comp_data_128 : 1;
    uint64_t config_ble_mesh_models_metadata_0 : 1;
    uint64_t config_ble_mesh_models_metadata_128 : 1;
    uint64_t config_ble_mesh_mbt_cli : 1;
    uint64_t config_ble_mesh_mbt_srv : 1;
    uint64_t config_ble_mesh_odp_cli : 1;
    uint64_t config_ble_mesh_odp_srv : 1;
    uint64_t config_ble_mesh_prb_cli : 1;
    uint64_t config_ble_mesh_prb_srv : 1;
    uint64_t config_ble_mesh_private_beacon : 1;
    uint64_t config_ble_mesh_rpr_cli : 1;
    uint64_t config_ble_mesh_rpr_srv : 1;
    uint64_t config_ble_mesh_rpr_srv_active_scan : 1;
    uint64_t config_ble_mesh_sar_cli : 1;
    uint64_t config_ble_mesh_sar_srv : 1;
    uint64_t config_ble_mesh_srpl_cli : 1;
    uint64_t config_ble_mesh_srpl_srv : 1;

    uint32_t config_ble_mesh_prov_protocol_timeout;
    uint16_t config_ble_mesh_record_frag_max_size;
    uint16_t config_ble_mesh_crpl;
    uint16_t config_ble_mesh_proxy_solic_rx_crpl;
    uint16_t config_ble_mesh_proxy_solic_tx_src_count;
    uint16_t config_ble_mesh_max_bridging_table_entry_count;
    uint16_t config_ble_mesh_bridge_crpl;
    uint16_t config_ble_mesh_max_disc_table_entry_count;
    uint16_t config_ble_mesh_max_forward_table_entry_count;
    uint16_t config_ble_mesh_max_deps_nodes_per_path;
    uint16_t config_ble_mesh_rpr_cli_prov_same_time;
    uint16_t config_ble_mesh_rpr_srv_max_scanned_items;
    uint16_t config_ble_mesh_rpr_srv_max_ext_scan;
    uint16_t config_ble_mesh_max_blob_receivers;

    uint8_t struct_net_tx_size;
    uint8_t struct_net_rx_size;
    uint8_t struct_client_common_size;
    uint8_t struct_client_internal_data_size;
    uint8_t struct_msg_ctx_size;
    uint8_t struct_mutex_size;
    uint8_t struct_timer_size;
    uint8_t struct_snode_size;
    uint8_t struct_slist_size;
    uint8_t struct_slist_off_tail;
    uint8_t struct_net_buf_simple_size;
    uint8_t struct_net_buf_simple_off_len;
    uint8_t struct_net_buf_simple_off_size;
    uint8_t struct_net_buf_simple_off_buf;
    uint8_t struct_net_buf_simple_state_size;
    uint8_t struct_net_buf_simple_state_off_len;
    uint8_t struct_client_op_pair_size;
    uint8_t struct_client_op_pair_off_status_op;
    uint8_t struct_model_op_size;
    uint8_t struct_model_op_off_min_len;
    uint8_t struct_model_op_off_func;
    uint8_t struct_model_cb_size;
    uint8_t struct_model_cb_off_deinit;
    uint8_t struct_send_cb_size;
    uint8_t struct_send_cb_off_end;
    uint8_t struct_addr_size;
    uint8_t struct_addr_off_val;
    uint8_t struct_sg_size;
    uint8_t struct_sg_off_len;
    uint8_t struct_tc_sha256_state;
    uint8_t struct_tc_sha256_off_bits_hashed;
    uint8_t struct_tc_sha256_off_leftover;
    uint8_t struct_tc_sha256_off_leftover_offset;
    uint8_t struct_tc_hmac_state_size;
    uint8_t struct_tc_hmac_state_off_key;

    uint8_t btc_ble_mesh_evt_agg_client_send_timeout;
    uint8_t btc_ble_mesh_evt_agg_client_recv_rsp;
    uint8_t btc_ble_mesh_evt_agg_client_recv_pub;
    uint8_t btc_ble_mesh_evt_agg_server_recv_msg;
    uint8_t btc_ble_mesh_evt_brc_client_recv_rsp;
    uint8_t btc_ble_mesh_evt_brc_client_recv_pub;
    uint8_t btc_ble_mesh_evt_brc_client_send_timeout;
    uint8_t btc_ble_mesh_evt_brc_server_state_change;
    uint8_t btc_ble_mesh_evt_df_client_send_timeout;
    uint8_t btc_ble_mesh_evt_df_client_recv_get_rsp;
    uint8_t btc_ble_mesh_evt_df_client_recv_set_rsp;
    uint8_t btc_ble_mesh_evt_df_client_recv_pub;
    uint8_t btc_ble_mesh_evt_df_server_state_change;
    uint8_t btc_ble_mesh_evt_df_server_table_change;
    uint8_t btc_ble_mesh_evt_lcd_client_send_timeout;
    uint8_t btc_ble_mesh_evt_lcd_client_recv_rsp;
    uint8_t btc_ble_mesh_evt_lcd_client_recv_pub;
    uint8_t btc_ble_mesh_evt_mbt_client_retrieve_capabilities_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_transfer_blob_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_send_block_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_send_data_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_determine_block_status_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_determine_transfer_status_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_cancel_transfer_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_set_transfer_ttl_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_clear_transfer_ttl_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_set_app_idx_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_clear_app_idx_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_set_multicast_addr_comp;
    uint8_t btc_ble_mesh_evt_mbt_client_clear_multicast_addr_comp;
    uint8_t btc_ble_mesh_mbt_client_result_complete;
    uint8_t btc_ble_mesh_mbt_client_result_fail;
    uint8_t btc_ble_mesh_evt_mbt_server_initialize_blob_receive_comp;
    uint8_t btc_ble_mesh_evt_mbt_server_cancel_blob_receive_comp;
    uint8_t btc_ble_mesh_evt_mbt_server_set_blob_capabilities_comp;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_transfer_get;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_transfer_start;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_transfer_cancel;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_block_get;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_block_start;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_chunk_transfer;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_information_get;
    uint8_t btc_ble_mesh_evt_mbt_server_block_receive_comp;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_receive_comp;
    uint8_t btc_ble_mesh_evt_mbt_server_blob_receive_timeout;
    uint8_t btc_ble_mesh_evt_odp_client_send_timeout;
    uint8_t btc_ble_mesh_evt_odp_client_recv_rsp;
    uint8_t btc_ble_mesh_evt_odp_client_recv_pub;
    uint8_t btc_ble_mesh_evt_prb_client_recv_rsp;
    uint8_t btc_ble_mesh_evt_prb_client_recv_pub;
    uint8_t btc_ble_mesh_evt_prb_client_send_timeout;
    uint8_t btc_ble_mesh_evt_rpr_client_send_timeout;
    uint8_t btc_ble_mesh_evt_rpr_client_recv_rsp;
    uint8_t btc_ble_mesh_evt_rpr_client_recv_pub;
    uint8_t btc_ble_mesh_evt_rpr_server_scan_start;
    uint8_t btc_ble_mesh_evt_rpr_server_scan_stop;
    uint8_t btc_ble_mesh_evt_rpr_server_ext_scan_start;
    uint8_t btc_ble_mesh_evt_rpr_server_ext_scan_stop;
    uint8_t btc_ble_mesh_evt_rpr_server_link_open;
    uint8_t btc_ble_mesh_evt_rpr_server_link_close;
    uint8_t btc_ble_mesh_evt_sar_client_send_timeout;
    uint8_t btc_ble_mesh_evt_sar_client_recv_rsp;
    uint8_t btc_ble_mesh_evt_sar_client_recv_pub;
    uint8_t btc_ble_mesh_evt_srpl_client_send_timeout;
    uint8_t btc_ble_mesh_evt_srpl_client_recv_rsp;
    uint8_t btc_ble_mesh_evt_srpl_client_recv_pub;
} bt_mesh_ext_config_t;

static const bt_mesh_ext_config_t bt_mesh_ext_cfg = {
    .config_ble_mesh_use_ble_50                     = IS_ENABLED(CONFIG_BLE_MESH_USE_BLE_50),
    .config_ble_mesh_use_duplicate_scan             = IS_ENABLED(CONFIG_BLE_MESH_USE_DUPLICATE_SCAN),
    .config_ble_mesh_pb_adv                         = IS_ENABLED(CONFIG_BLE_MESH_PB_ADV),
    .config_ble_mesh_pb_gatt                        = IS_ENABLED(CONFIG_BLE_MESH_PB_GATT),
    .config_ble_mesh_prov_epa                       = IS_ENABLED(CONFIG_BLE_MESH_PROV_EPA),
    .config_ble_mesh_cert_based_prov                = IS_ENABLED(CONFIG_BLE_MESH_CERT_BASED_PROV),
    .config_ble_mesh_gatt_proxy_client              = IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_CLIENT),
    .config_ble_mesh_gatt_proxy_server              = IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER),
    .config_ble_mesh_proxy_solic_pdu_rx             = IS_ENABLED(CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX),
    .config_ble_mesh_proxy_solic_pdu_tx             = IS_ENABLED(CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX),
    .config_ble_mesh_proxy_solic                    = (IS_ENABLED(CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX) | \
                                                       IS_ENABLED(CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX)),
    .config_ble_mesh_support_directed_proxy         = IS_ENABLED(CONFIG_BLE_MESH_SUPPORT_DIRECTED_PROXY),
    .config_ble_mesh_relay                          = IS_ENABLED(CONFIG_BLE_MESH_RELAY),
    .config_ble_mesh_settings                       = IS_ENABLED(CONFIG_BLE_MESH_SETTINGS),
    .config_ble_mesh_friend                         = IS_ENABLED(CONFIG_BLE_MESH_FRIEND),
    .config_ble_mesh_low_power                      = IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER),
    .config_ble_mesh_agg_cli                        = IS_ENABLED(CONFIG_BLE_MESH_AGG_CLI),
    .config_ble_mesh_agg_srv                        = IS_ENABLED(CONFIG_BLE_MESH_AGG_SRV),
    .config_ble_mesh_brc_cli                        = IS_ENABLED(CONFIG_BLE_MESH_BRC_CLI),
    .config_ble_mesh_brc_srv                        = IS_ENABLED(CONFIG_BLE_MESH_BRC_SRV),
    .config_ble_mesh_df_cli                         = IS_ENABLED(CONFIG_BLE_MESH_DF_CLI),
    .config_ble_mesh_df_srv                         = IS_ENABLED(CONFIG_BLE_MESH_DF_SRV),
    .config_ble_mesh_path_monitor_test              = IS_ENABLED(CONFIG_BLE_MESH_PATH_MONITOR_TEST),
    .config_ble_mesh_lcd_cli                        = IS_ENABLED(CONFIG_BLE_MESH_LCD_CLI),
    .config_ble_mesh_lcd_srv                        = IS_ENABLED(CONFIG_BLE_MESH_LCD_SRV),
    .config_ble_mesh_comp_data_1                    = IS_ENABLED(CONFIG_BLE_MESH_COMP_DATA_1),
    .config_ble_mesh_comp_data_128                  = IS_ENABLED(CONFIG_BLE_MESH_COMP_DATA_128),
    .config_ble_mesh_models_metadata_0              = IS_ENABLED(CONFIG_BLE_MESH_MODELS_METADATA_0),
    .config_ble_mesh_models_metadata_128            = IS_ENABLED(CONFIG_BLE_MESH_MODELS_METADATA_128),
    .config_ble_mesh_mbt_cli                        = IS_ENABLED(CONFIG_BLE_MESH_MBT_CLI),
    .config_ble_mesh_mbt_srv                        = IS_ENABLED(CONFIG_BLE_MESH_MBT_SRV),
    .config_ble_mesh_odp_cli                        = IS_ENABLED(CONFIG_BLE_MESH_ODP_CLI),
    .config_ble_mesh_odp_srv                        = IS_ENABLED(CONFIG_BLE_MESH_ODP_SRV),
    .config_ble_mesh_prb_cli                        = IS_ENABLED(CONFIG_BLE_MESH_PRB_CLI),
    .config_ble_mesh_prb_srv                        = IS_ENABLED(CONFIG_BLE_MESH_PRB_SRV),
    .config_ble_mesh_private_beacon                 = (IS_ENABLED(CONFIG_BLE_MESH_PRB_SRV) | \
                                                       IS_ENABLED(CONFIG_BLE_MESH_PRB_SRV)),
    .config_ble_mesh_rpr_cli                        = IS_ENABLED(CONFIG_BLE_MESH_RPR_CLI),
    .config_ble_mesh_rpr_srv                        = IS_ENABLED(CONFIG_BLE_MESH_RPR_SRV),
    .config_ble_mesh_rpr_srv_active_scan            = IS_ENABLED(CONFIG_BLE_MESH_RPR_SRV_ACTIVE_SCAN),
    .config_ble_mesh_sar_cli                        = IS_ENABLED(CONFIG_BLE_MESH_SAR_CLI),
    .config_ble_mesh_sar_srv                        = IS_ENABLED(CONFIG_BLE_MESH_SAR_SRV),
    .config_ble_mesh_srpl_cli                       = IS_ENABLED(CONFIG_BLE_MESH_SRPL_CLI),
    .config_ble_mesh_srpl_srv                       = IS_ENABLED(CONFIG_BLE_MESH_SRPL_SRV),
    .config_ble_mesh_prov_protocol_timeout          = PROTOCOL_TIMEOUT,

#if CONFIG_BLE_MESH_CERT_BASED_PROV
    .config_ble_mesh_record_frag_max_size           = CONFIG_BLE_MESH_RECORD_FRAG_MAX_SIZE,
#endif /* CONFIG_BLE_MESH_CERT_BASED_PROV */
    .config_ble_mesh_crpl                           = CONFIG_BLE_MESH_CRPL,
#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX
    .config_ble_mesh_proxy_solic_rx_crpl            = CONFIG_BLE_MESH_PROXY_SOLIC_RX_CRPL,
#endif /* CONFIG_BLE_MESH_PROXY_SOLIC_PDU_RX */
#if CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX
    .config_ble_mesh_proxy_solic_tx_src_count       = CONFIG_BLE_MESH_PROXY_SOLIC_TX_SRC_COUNT,
#endif /* CONFIG_BLE_MESH_PROXY_SOLIC_PDU_TX */
#if CONFIG_BLE_MESH_BRC_SRV
    .config_ble_mesh_max_bridging_table_entry_count = CONFIG_BLE_MESH_MAX_BRIDGING_TABLE_ENTRY_COUNT,
    .config_ble_mesh_bridge_crpl                    = CONFIG_BLE_MESH_BRIDGE_CRPL,
#endif /* CONFIG_BLE_MESH_BRC_SRV */
#if CONFIG_BLE_MESH_DF_SRV
    .config_ble_mesh_max_disc_table_entry_count     = CONFIG_BLE_MESH_MAX_DISC_TABLE_ENTRY_COUNT,
    .config_ble_mesh_max_forward_table_entry_count  = CONFIG_BLE_MESH_MAX_FORWARD_TABLE_ENTRY_COUNT,
    .config_ble_mesh_max_deps_nodes_per_path        = CONFIG_BLE_MESH_MAX_DEPS_NODES_PER_PATH,
#endif /* CONFIG_BLE_MESH_DF_SRV */
#if CONFIG_BLE_MESH_RPR_CLI
    .config_ble_mesh_rpr_cli_prov_same_time         = CONFIG_BLE_MESH_RPR_CLI_PROV_SAME_TIME,
#endif /* CONFIG_BLE_MESH_RPR_CLI */
#if CONFIG_BLE_MESH_RPR_SRV
    .config_ble_mesh_rpr_srv_max_scanned_items      = CONFIG_BLE_MESH_RPR_SRV_MAX_SCANNED_ITEMS,
    .config_ble_mesh_rpr_srv_max_ext_scan           = CONFIG_BLE_MESH_RPR_SRV_MAX_EXT_SCAN,
#endif /* CONFIG_BLE_MESH_RPR_SRV */
#if CONFIG_BLE_MESH_MBT_CLI
    .config_ble_mesh_max_blob_receivers             = CONFIG_BLE_MESH_MAX_BLOB_RECEIVERS,
#endif /* CONFIG_BLE_MESH_MBT_CLI */

    .struct_net_tx_size                             = sizeof(struct bt_mesh_net_tx),
    .struct_net_rx_size                             = sizeof(struct bt_mesh_net_rx),
    .struct_client_common_size                      = sizeof(bt_mesh_client_common_param_t),
    .struct_client_internal_data_size               = sizeof(bt_mesh_client_internal_data_t),
    .struct_msg_ctx_size                            = sizeof(struct bt_mesh_msg_ctx),
    .struct_mutex_size                              = sizeof(bt_mesh_mutex_t),
    .struct_timer_size                              = sizeof(struct k_delayed_work),
    .struct_snode_size                              = sizeof(struct _snode),
    .struct_slist_size                              = sizeof(struct _slist),
    .struct_slist_off_tail                          = offsetof(struct _slist, tail),
    .struct_net_buf_simple_size                     = sizeof(struct net_buf_simple),
    .struct_net_buf_simple_off_len                  = offsetof(struct net_buf_simple, len),
    .struct_net_buf_simple_off_size                 = offsetof(struct net_buf_simple, size),
    .struct_net_buf_simple_off_buf                  = offsetof(struct net_buf_simple, __buf),
    .struct_net_buf_simple_state_size               = sizeof(struct net_buf_simple_state),
    .struct_net_buf_simple_state_off_len            = offsetof(struct net_buf_simple_state, len),
    .struct_client_op_pair_size                     = sizeof(bt_mesh_client_op_pair_t),
    .struct_client_op_pair_off_status_op            = offsetof(bt_mesh_client_op_pair_t, status_op),
    .struct_model_op_size                           = sizeof(struct bt_mesh_model_op),
    .struct_model_op_off_min_len                    = offsetof(struct bt_mesh_model_op, min_len),
    .struct_model_op_off_func                       = offsetof(struct bt_mesh_model_op, func),

    .struct_model_cb_size                           = sizeof(struct bt_mesh_model_cb),
#if CONFIG_BLE_MESH_DEINIT
    .struct_model_cb_off_deinit                     = offsetof(struct bt_mesh_model_cb, deinit),
#endif
    .struct_send_cb_size                            = sizeof(struct bt_mesh_send_cb),
    .struct_send_cb_off_end                         = offsetof(struct bt_mesh_send_cb, end),
    .struct_addr_size                               = sizeof(bt_mesh_addr_t),
    .struct_addr_off_val                            = offsetof(bt_mesh_addr_t, val),
    .struct_sg_size                                 = sizeof(struct bt_mesh_sg),
    .struct_sg_off_len                              = offsetof(struct bt_mesh_sg, len),

    .btc_ble_mesh_evt_agg_client_send_timeout                   = BTC_BLE_MESH_EVT_AGG_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_agg_client_recv_rsp                       = BTC_BLE_MESH_EVT_AGG_CLIENT_RECV_RSP,
    .btc_ble_mesh_evt_agg_client_recv_pub                       = BTC_BLE_MESH_EVT_AGG_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_agg_server_recv_msg                       = BTC_BLE_MESH_EVT_AGG_SERVER_RECV_MSG,
    .btc_ble_mesh_evt_brc_client_recv_rsp                       = BTC_BLE_MESH_EVT_BRC_CLIENT_RECV_RSP,
    .btc_ble_mesh_evt_brc_client_recv_pub                       = BTC_BLE_MESH_EVT_BRC_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_brc_client_send_timeout                   = BTC_BLE_MESH_EVT_BRC_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_brc_server_state_change                   = BTC_BLE_MESH_EVT_BRC_SERVER_STATE_CHANGE,
    .btc_ble_mesh_evt_df_client_send_timeout                    = BTC_BLE_MESH_EVT_DF_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_df_client_recv_get_rsp                    = BTC_BLE_MESH_EVT_DF_CLIENT_RECV_GET_RSP,
    .btc_ble_mesh_evt_df_client_recv_set_rsp                    = BTC_BLE_MESH_EVT_DF_CLIENT_RECV_SET_RSP,
    .btc_ble_mesh_evt_df_client_recv_pub                        = BTC_BLE_MESH_EVT_DF_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_df_server_state_change                    = BTC_BLE_MESH_EVT_DF_SERVER_STATE_CHANGE,
    .btc_ble_mesh_evt_df_server_table_change                    = BTC_BLE_MESH_EVT_DF_SERVER_TABLE_CHANGE,
    .btc_ble_mesh_evt_lcd_client_send_timeout                   = BTC_BLE_MESH_EVT_LCD_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_lcd_client_recv_rsp                       = BTC_BLE_MESH_EVT_LCD_CLIENT_RECV_RSP,
    .btc_ble_mesh_evt_lcd_client_recv_pub                       = BTC_BLE_MESH_EVT_LCD_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_mbt_client_retrieve_capabilities_comp     = BTC_BLE_MESH_EVT_MBT_CLIENT_RETRIEVE_CAPABILITIES_COMP,
    .btc_ble_mesh_evt_mbt_client_transfer_blob_comp             = BTC_BLE_MESH_EVT_MBT_CLIENT_TRANSFER_BLOB_COMP,
    .btc_ble_mesh_evt_mbt_client_send_block_comp                = BTC_BLE_MESH_EVT_MBT_CLIENT_SEND_BLOCK_COMP,
    .btc_ble_mesh_evt_mbt_client_send_data_comp                 = BTC_BLE_MESH_EVT_MBT_CLIENT_SEND_DATA_COMP,
    .btc_ble_mesh_evt_mbt_client_determine_block_status_comp    = BTC_BLE_MESH_EVT_MBT_CLIENT_DETERMINE_BLOCK_STATUS_COMP,
    .btc_ble_mesh_evt_mbt_client_determine_transfer_status_comp = BTC_BLE_MESH_EVT_MBT_CLIENT_DETERMINE_TRANSFER_STATUS_COMP,
    .btc_ble_mesh_evt_mbt_client_cancel_transfer_comp           = BTC_BLE_MESH_EVT_MBT_CLIENT_CANCEL_TRANSFER_COMP,
    .btc_ble_mesh_evt_mbt_client_set_transfer_ttl_comp          = BTC_BLE_MESH_EVT_MBT_CLIENT_SET_TRANSFER_TTL_COMP,
    .btc_ble_mesh_evt_mbt_client_clear_transfer_ttl_comp        = BTC_BLE_MESH_EVT_MBT_CLIENT_CLEAR_TRANSFER_TTL_COMP,
    .btc_ble_mesh_evt_mbt_client_set_app_idx_comp               = BTC_BLE_MESH_EVT_MBT_CLIENT_SET_APP_IDX_COMP,
    .btc_ble_mesh_evt_mbt_client_clear_app_idx_comp             = BTC_BLE_MESH_EVT_MBT_CLIENT_CLEAR_APP_IDX_COMP,
    .btc_ble_mesh_evt_mbt_client_set_multicast_addr_comp        = BTC_BLE_MESH_EVT_MBT_CLIENT_SET_MULTICAST_ADDR_COMP,
    .btc_ble_mesh_evt_mbt_client_clear_multicast_addr_comp      = BTC_BLE_MESH_EVT_MBT_CLIENT_CLEAR_MULTICAST_ADDR_COMP,
    .btc_ble_mesh_mbt_client_result_complete                    = BTC_BLE_MESH_MBT_CLIENT_RESULT_COMPLETE,
    .btc_ble_mesh_mbt_client_result_fail                        = BTC_BLE_MESH_MBT_CLIENT_RESULT_FAIL,
    .btc_ble_mesh_evt_odp_client_send_timeout                   = BTC_BLE_MESH_EVT_ODP_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_odp_client_recv_rsp                       = BTC_BLE_MESH_EVT_ODP_CLIENT_RECV_RSP,
    .btc_ble_mesh_evt_odp_client_recv_pub                       = BTC_BLE_MESH_EVT_ODP_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_prb_client_recv_rsp                       = BTC_BLE_MESH_EVT_PRB_CLIENT_RECV_RSP,
    .btc_ble_mesh_evt_prb_client_recv_pub                       = BTC_BLE_MESH_EVT_PRB_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_prb_client_send_timeout                   = BTC_BLE_MESH_EVT_PRB_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_rpr_client_send_timeout                   = BTC_BLE_MESH_EVT_RPR_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_rpr_client_recv_rsp                       = BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_RSP,
    .btc_ble_mesh_evt_rpr_client_recv_pub                       = BTC_BLE_MESH_EVT_RPR_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_sar_client_send_timeout                   = BTC_BLE_MESH_EVT_SAR_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_sar_client_recv_rsp                       = BTC_BLE_MESH_EVT_SAR_CLIENT_RECV_RSP,
    .btc_ble_mesh_evt_sar_client_recv_pub                       = BTC_BLE_MESH_EVT_SAR_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_srpl_client_send_timeout                  = BTC_BLE_MESH_EVT_SRPL_CLIENT_SEND_TIMEOUT,
    .btc_ble_mesh_evt_srpl_client_recv_rsp                      = BTC_BLE_MESH_EVT_SRPL_CLIENT_RECV_RSP,
    .btc_ble_mesh_evt_srpl_client_recv_pub                      = BTC_BLE_MESH_EVT_SRPL_CLIENT_RECV_PUB,
    .btc_ble_mesh_evt_rpr_server_scan_start                     = BTC_BLE_MESH_EVT_RPR_SERVER_SCAN_START,
    .btc_ble_mesh_evt_rpr_server_scan_stop                      = BTC_BLE_MESH_EVT_RPR_SERVER_SCAN_STOP,
    .btc_ble_mesh_evt_rpr_server_ext_scan_start                 = BTC_BLE_MESH_EVT_RPR_SERVER_EXT_SCAN_START,
    .btc_ble_mesh_evt_rpr_server_ext_scan_stop                  = BTC_BLE_MESH_EVT_RPR_SERVER_EXT_SCAN_STOP,
    .btc_ble_mesh_evt_rpr_server_link_open                      = BTC_BLE_MESH_EVT_RPR_SERVER_LINK_OPEN,
    .btc_ble_mesh_evt_rpr_server_link_close                     = BTC_BLE_MESH_EVT_RPR_SERVER_LINK_CLOSE,
    .btc_ble_mesh_evt_mbt_server_initialize_blob_receive_comp   = BTC_BLE_MESH_EVT_MBT_SERVER_INITIALIZE_BLOB_RECEIVE_COMP,
    .btc_ble_mesh_evt_mbt_server_cancel_blob_receive_comp       = BTC_BLE_MESH_EVT_MBT_SERVER_CANCEL_BLOB_RECEIVE_COMP,
    .btc_ble_mesh_evt_mbt_server_set_blob_capabilities_comp     = BTC_BLE_MESH_EVT_MBT_SERVER_SET_BLOB_CAPABILITIES_COMP,
    .btc_ble_mesh_evt_mbt_server_blob_transfer_get              = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_GET,
    .btc_ble_mesh_evt_mbt_server_blob_transfer_start            = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_START,
    .btc_ble_mesh_evt_mbt_server_blob_transfer_cancel           = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_TRANSFER_CANCEL,
    .btc_ble_mesh_evt_mbt_server_blob_block_get                 = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_BLOCK_GET,
    .btc_ble_mesh_evt_mbt_server_blob_block_start               = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_BLOCK_START,
    .btc_ble_mesh_evt_mbt_server_blob_chunk_transfer            = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_CHUNK_TRANSFER,
    .btc_ble_mesh_evt_mbt_server_blob_information_get           = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_INFORMATION_GET,
    .btc_ble_mesh_evt_mbt_server_block_receive_comp             = BTC_BLE_MESH_EVT_MBT_SERVER_BLOCK_RECEIVE_COMP,
    .btc_ble_mesh_evt_mbt_server_blob_receive_comp              = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_RECEIVE_COMP,
    .btc_ble_mesh_evt_mbt_server_blob_receive_timeout           = BTC_BLE_MESH_EVT_MBT_SERVER_BLOB_RECEIVE_TIMEOUT,
};

typedef struct {
/* CONFIG_BLE_MESH_NODE */
    uint8_t (*_bt_mesh_ext_prov_node_next_xact_id)(void *link);
    void *(*_bt_mesh_ext_prov_node_get_link)(void);
    void (*_bt_mesh_ext_prov_node_close_link)(uint8_t reason);
    const uint8_t *(*_bt_mesh_ext_prov_node_get_uuid)(void);
    uint16_t (*_bt_mesh_ext_prov_node_get_oob_info)(void);
    const char *(*_bt_mesh_ext_prov_node_get_uri)(void);
/* CONFIG_BLE_MESH_NODE */

/* CONFIG_BLE_MESH_PROVISIONER */
    uint8_t (*_bt_mesh_ext_prov_pvnr_next_xact_id)(void *link);
    void *(*_bt_mesh_ext_prov_pvnr_get_link)(void);
    uint8_t (*_bt_mesh_ext_prov_pvnr_get_link_count)(void);
    void (*_bt_mesh_ext_prov_pvnr_send_invite)(void *link);
    void (*_bt_mesh_ext_prov_pvnr_close_link)(void *link, uint8_t reason);
    void *(*_bt_mesh_ext_pvnr_get_node_with_addr)(uint16_t unicast_addr);
/* CONFIG_BLE_MESH_CERT_BASED_PROV */
    void (*_bt_mesh_ext_pvnr_records_list_get_cb)(uint16_t link_idx, struct net_buf_simple *data);
    void (*_bt_mesh_ext_pvnr_records_recv_comp_cb)(uint8_t status, uint16_t link_idx,
                                                   uint16_t record_id, uint16_t frag_offset,
                                                   uint16_t total_len, uint8_t *record);
    uint16_t (*_bt_mesh_ext_prov_link_get_record_id_expect)(void *link);
    void (*_bt_mesh_ext_prov_link_set_record_id_expect)(void *link, uint16_t record_id_expect);
    uint16_t (*_bt_mesh_ext_prov_link_get_offset_expect)(void *link);
    void (*_bt_mesh_ext_prov_link_set_offset_expect)(void *link, uint16_t offset_expect);
    uint16_t (*_bt_mesh_ext_prov_link_get_max_size)(void *link);
    void (*_bt_mesh_ext_prov_link_set_max_size)(void *link, uint16_t max_size);
    uint8_t *(*_bt_mesh_ext_prov_link_get_record)(void *link, uint16_t id);
    uint8_t *(*_bt_mesh_ext_prov_link_alloc_record)(void *link, uint16_t id, uint16_t len);
/* CONFIG_BLE_MESH_CERT_BASED_PROV */
    uint8_t *(*_bt_mesh_ext_pvnr_get_node_uuid)(void *node);
    uint8_t *(*_bt_mesh_ext_prov_link_get_uuid)(void *link);
    uint8_t (*_bt_mesh_ext_prov_link_get_elem_num)(void *link);
    uint16_t (*_bt_mesh_ext_prov_link_get_unicast_addr)(void *link);
/* CONFIG_BLE_MESH_PROVISIONER */

/* CONFIG_BLE_MESH_PB_GATT */
    void *(*_bt_mesh_ext_prov_link_get_conn)(void *link);
    bool (*_bt_mesh_ext_prov_link_pb_gatt_exist)(void *link);
    int (*_bt_mesh_ext_prov_link_pb_gatt_send)(void *link, struct net_buf_simple *msg);
/* CONFIG_BLE_MESH_PB_GATT */

/* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */
    const char *(*_bt_mesh_ext_get_device_name)(void);
/* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || CONFIG_BLE_MESH_GATT_PROXY_SERVER */

/* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
    uint8_t (*_bt_mesh_ext_proxy_server_get_all_client_type)(void);
    int (*_bt_mesh_ext_proxy_server_segment_send)(void *conn, uint8_t type,
                                                  struct net_buf_simple *msg);
    bool (*_bt_mesh_ext_proxy_server_find_client_by_addr)(uint16_t addr);
    void (*_bt_mesh_ext_proxy_server_update_net_id_rand_stop)(void);
    void *(*_bt_mesh_ext_proxy_server_get_client)(uint8_t index);
    uint8_t (*_bt_mesh_ext_proxy_server_get_client_count)(void);
/* CONFIG_BLE_MESH_PRB_SRV */
    void (*_bt_mesh_ext_proxy_server_update_net_id_rand)(void);
    bool (*_bt_mesh_ext_proxy_server_is_node_id_enable)(void);
    void (*_bt_mesh_ext_proxy_server_private_identity_start)(void *sub);
    void (*_bt_mesh_ext_proxy_server_private_identity_stop)(void *sub);
/* CONFIG_BLE_MESH_PRB_SRV */
    uint8_t (*_bt_mesh_ext_proxy_server_get_filter_type)(void *client);
    uint8_t (*_bt_mesh_ext_proxy_server_get_filter_count)(void *client);
    uint16_t (*_bt_mesh_ext_proxy_server_get_filter_size)(void *client);
    uint16_t (*_bt_mesh_ext_proxy_server_get_filter_addr)(void *client, uint8_t index);
    bool (*_bt_mesh_ext_proxy_server_filter_is_client)(void *client, uint8_t index);
    uint8_t (*_bt_mesh_ext_proxy_server_get_client_type)(void *client);
    bool (*_bt_mesh_ext_proxy_server_is_proxy_msg_recv)(void *client);
    void *(*_bt_mesh_ext_proxy_server_get_conn)(void *client);
    void (*_bt_mesh_ext_proxy_server_set_client_type)(void *client, uint8_t type);
    void (*_bt_mesh_ext_proxy_server_set_msg_recv)(void *client, uint8_t val);
/* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

/* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
    bool (*_bt_mesh_ext_proxy_client_beacon_send)(void *sub, bool private);
    uint8_t (*_bt_mesh_ext_proxy_client_get_opcode)(void *cfg);
    uint8_t (*_bt_mesh_ext_proxy_client_get_use_directed)(void *cfg);
    void *(*_bt_mesh_ext_proxy_client_get_client_uar)(void *cfg);
/* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

/* CONFIG_BLE_MESH_FRIEND */
    uint16_t (*_bt_mesh_ext_frnd_get_lpn)(void *frnd);
    uint8_t (*_bt_mesh_ext_frnd_get_num_elem)(void *frnd);
    bool (*_bt_mesh_ext_frnd_is_valid)(void *frnd);
    bool (*_bt_mesh_ext_frnd_is_established)(void *frnd);
    uint16_t (*_bt_mesh_ext_frnd_get_net_idx)(void *frnd);
    uint8_t (*_bt_mesh_ext_frnd_get_sub_list_count)(void *frnd);
    uint16_t (*_bt_mesh_ext_frnd_get_sub_list_size)(void *frnd);
    uint16_t (*_bt_mesh_ext_frnd_get_sub_addr)(void *frnd, uint8_t index);
    uint8_t (*_bt_mesh_ext_net_get_frnd_count)(void);
    void *(*_bt_mesh_ext_net_get_frnd)(uint8_t index);
    bool (*_bt_mesh_ext_friend_match)(uint16_t net_idx, uint16_t addr);
    bool (*_bt_mesh_ext_friend_unicast_match)(uint16_t net_idx, uint16_t addr, uint8_t *selem);
    void *(*_bt_mesh_ext_friend_find)(uint16_t net_idx, uint16_t lpn_addr, bool valid, bool established);
    void (*_bt_mesh_ext_friend_clear_net_idx)(uint16_t net_idx);
/* CONFIG_BLE_MESH_FRIEND */

/* CONFIG_BLE_MESH_LOW_POWER */
    bool (*_bt_mesh_ext_lpn_match)(uint16_t addr);
    uint16_t (*_bt_mesh_ext_lpn_frnd)(void);
/* CONFIG_BLE_MESH_LOW_POWER */

/* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
    int (*_bt_mesh_ext_update_exceptional_list)(uint8_t sub_code, uint32_t type, void *info);
/* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
/* (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) */
    int (*_bt_mesh_ext_unprov_dev_info_query)(uint8_t uuid[16], uint8_t addr[6],
                                              uint8_t *adv_type, uint8_t query_type);
/* (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) */

/* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) ||
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT ||
   (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT) */
    int (*_bt_mesh_ext_gattc_conn_create)(const void *addr, uint16_t service_uuid);
    void (*_bt_mesh_ext_gattc_disconnect)(void *conn);
/* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) ||
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT ||
   (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT) */

/* CONFIG_BLE_MESH_SETTINGS */
    void (*_bt_mesh_ext_store_dkca)(void);
    void (*_bt_mesh_ext_clear_dkca)(void);
    void (*_bt_mesh_ext_clear_rpl_single)(uint16_t src);
    void (*_bt_mesh_ext_store_net)(void);
    void (*_bt_mesh_ext_store_seq)(void);
    struct net_buf_simple *(*_bt_mesh_ext_get_core_settings_item)(const char *key);
    bool (*_bt_mesh_ext_is_settings_item_exist)(struct net_buf_simple *buf, const uint16_t val);
    int (*_bt_mesh_ext_add_core_settings_item)(const char *key, const uint16_t val);
    int (*_bt_mesh_ext_remove_core_settings_item)(const char *key, const uint16_t val);
    int (*_bt_mesh_ext_save_core_settings)(const char *key, const uint8_t *val, size_t len);
    int (*_bt_mesh_ext_erase_core_settings)(const char *key);
/* CONFIG_BLE_MESH_SETTINGS */

/* CONFIG_BLE_MESH_DF_SRV */
    uint8_t (*_bt_mesh_ext_model_get_pub_directed_pub_policy)(void *model);
    void (*_bt_mesh_ext_model_set_pub_directed_pub_policy)(void *model, uint8_t directed_pub_policy);
    uint8_t (*_bt_mesh_ext_sub_get_keys_direct_nid)(void *sub, uint8_t index);
    uint8_t *(*_bt_mesh_ext_sub_get_keys_direct_enc)(void *sub, uint8_t index);
    uint8_t *(*_bt_mesh_ext_sub_get_keys_direct_privacy)(void *sub, uint8_t index);
    uint8_t (*_bt_mesh_ext_sub_get_directed_forwarding)(void *sub);
    void (*_bt_mesh_ext_sub_set_directed_forwarding)(void *sub, uint8_t directed_forwarding);
    uint8_t (*_bt_mesh_ext_sub_get_directed_relay)(void *sub);
    void (*_bt_mesh_ext_sub_set_directed_relay)(void *sub, uint8_t directed_relay);
    uint8_t (*_bt_mesh_ext_sub_get_directed_proxy)(void *sub);
    void (*_bt_mesh_ext_sub_set_directed_proxy)(void *sub, uint8_t directed_proxy);
    uint8_t (*_bt_mesh_ext_sub_get_directed_proxy_use_default)(void *sub);
    void (*_bt_mesh_ext_sub_set_directed_proxy_use_default)(void *sub, uint8_t directed_proxy_use_default);
    uint8_t (*_bt_mesh_ext_sub_get_directed_friend)(void *sub);
    void (*_bt_mesh_ext_sub_set_directed_friend)(void *sub, uint8_t directed_friend);
    uint8_t (*_bt_mesh_ext_sub_get_use_directed)(void *sub);
    void (*_bt_mesh_ext_sub_set_use_directed)(void *sub, uint8_t use_directed);
    void *(*_bt_mesh_ext_sub_get_proxy_client_uar)(void *sub);
    bool (*_bt_mesh_ext_sub_get_proxy_client_uar_len_present)(void *sub);
    void (*_bt_mesh_ext_sub_set_proxy_client_uar_len_present)(void *sub, bool len_present);
    uint16_t (*_bt_mesh_ext_sub_get_proxy_client_uar_range_start)(void *sub);
    void (*_bt_mesh_ext_sub_set_proxy_client_uar_range_start)(void *sub, uint16_t range_start);
    uint8_t (*_bt_mesh_ext_sub_get_proxy_client_uar_range_length)(void *sub);
    void (*_bt_mesh_ext_sub_set_proxy_client_uar_range_length)(void *sub, uint8_t range_length);
    uint8_t (*_bt_mesh_ext_sub_get_path_metric_type)(void *sub);
    void (*_bt_mesh_ext_sub_set_path_metric_type)(void *sub, uint8_t path_metric_type);
    uint8_t (*_bt_mesh_ext_sub_get_path_lifetime_type)(void *sub);
    void (*_bt_mesh_ext_sub_set_path_lifetime_type)(void *sub, uint8_t path_lifetime_type);
    bool (*_bt_mesh_ext_sub_get_two_way_path)(void *sub);
    void (*_bt_mesh_ext_sub_set_two_way_path)(void *sub, bool two_way_path);
    uint8_t (*_bt_mesh_ext_sub_get_forward_number)(void *sub);
    void (*_bt_mesh_ext_sub_set_forward_number)(void *sub, uint8_t forward_number);
    uint8_t (*_bt_mesh_ext_sub_get_disc_table_max_disc_entries)(void *sub);
    void (*_bt_mesh_ext_sub_set_disc_table_max_disc_entries)(void *sub, uint8_t max_disc_entries);
    uint8_t (*_bt_mesh_ext_sub_get_disc_table_max_concurr_init)(void *sub);
    void (*_bt_mesh_ext_sub_set_disc_table_max_concurr_init)(void *sub, uint8_t max_concurr_init);
    uint8_t (*_bt_mesh_ext_sub_get_disc_table_concurr_init)(void *sub);
    void (*_bt_mesh_ext_sub_set_disc_table_concurr_init)(void *sub, uint8_t concurr_init);
    void (*_bt_mesh_ext_sub_inc_disc_table_concurr_init)(void *sub);
    void (*_bt_mesh_ext_sub_dec_disc_table_concurr_init)(void *sub);
    void *(*_bt_mesh_ext_sub_get_disc_table_mutex)(void *sub);
    void *(*_bt_mesh_ext_sub_get_disc_table_entries)(void *sub);
    uint8_t (*_bt_mesh_ext_sub_get_fwd_table_max_ford_entries)(void *sub);
    void (*_bt_mesh_ext_sub_set_fwd_table_max_ford_entries)(void *sub, uint8_t max_ford_entries);
    uint8_t (*_bt_mesh_ext_sub_get_fwd_table_max_deps_nodes)(void *sub);
    void (*_bt_mesh_ext_sub_set_fwd_table_max_deps_nodes)(void *sub, uint8_t max_deps_nodes);
    uint16_t (*_bt_mesh_ext_sub_get_fwd_table_update_id)(void *sub);
    void (*_bt_mesh_ext_sub_set_fwd_table_update_id)(void *sub, uint16_t update_id);
    void (*_bt_mesh_ext_sub_inc_fwd_table_update_id)(void *sub);
    void *(*_bt_mesh_ext_sub_get_fwd_table_mutex)(void *sub);
    void *(*_bt_mesh_ext_sub_get_fwd_table_entries)(void *sub);
    uint8_t (*_bt_mesh_ext_sub_get_wanted_lanes)(void *sub);
    void (*_bt_mesh_ext_sub_set_wanted_lanes)(void *sub, uint8_t wanted_lanes);
    uint8_t (*_bt_mesh_ext_sub_get_unicast_echo_interval)(void *sub);
    void (*_bt_mesh_ext_sub_set_unicast_echo_interval)(void *sub, uint8_t unicast_echo_interval);
    uint8_t (*_bt_mesh_ext_sub_get_multicast_echo_interval)(void *sub);
    void (*_bt_mesh_ext_sub_set_multicast_echo_interval)(void *sub, uint8_t multicast_echo_interval);
/* CONFIG_BLE_MESH_DF_SRV */

/* CONFIG_BLE_MESH_RPR_CLI */
    int (*_bt_mesh_ext_rpr_cli_pdu_send)(void *link, uint8_t type);
    int (*_bt_mesh_ext_rpr_cli_recv_pub_key_outbound_report)(void *link);
    int (*_bt_mesh_ext_rpr_cli_pdu_recv)(void *link, uint8_t type, struct net_buf_simple *buf);
    bool (*_bt_mesh_ext_bt_mesh_is_unprov_dev_being_prov)(void *uuid);
    void *(*_bt_mesh_ext_rpr_cli_get_rpr_link)(uint8_t index);
/* CONFIG_BLE_MESH_RPR_CLI */

/* CONFIG_BLE_MESH_RPR_SRV */
    void (*_bt_mesh_ext_rpr_srv_reset_prov_link)(void *link, uint8_t reason);
    int (*_bt_mesh_ext_rpr_srv_nppi_pdu_recv)(uint8_t type, const uint8_t *data);
    int (*_bt_mesh_ext_rpr_srv_set_waiting_prov_link)(void* link, bt_mesh_addr_t *addr);
/* CONFIG_BLE_MESH_RPR_SRV */

/* CONFIG_BLE_MESH_PRIVATE_BEACON */
    uint8_t *(*_bt_mesh_ext_net_get_sub_mpb_rand)(uint8_t index);
    uint32_t (*_bt_mesh_ext_sub_get_mpb_sent)(void *sub);
    void (*_bt_mesh_ext_sub_set_mpb_sent)(void *sub, uint32_t mpb_sent);
    uint8_t (*_bt_mesh_ext_sub_get_mpb_last)(void *sub);
    void (*_bt_mesh_ext_sub_set_mpb_last)(void *sub, uint8_t mpb_last);
    uint8_t (*_bt_mesh_ext_sub_get_mpb_cur)(void *sub);
    void (*_bt_mesh_ext_sub_set_mpb_cur)(void *sub, uint8_t mpb_cur);
    void (*_bt_mesh_ext_sub_inc_mpb_cur)(void *sub);
    uint8_t (*_bt_mesh_ext_sub_get_mpb_flags_last)(void *sub);
    void (*_bt_mesh_ext_sub_set_mpb_flags_last)(void *sub, uint8_t mpb_flags_last);
    uint8_t (*_bt_mesh_ext_sub_get_mpb_ivi_last)(void *sub);
    void (*_bt_mesh_ext_sub_set_mpb_ivi_last)(void *sub, uint8_t mpb_ivi_last);
    uint8_t *(*_bt_mesh_ext_sub_get_mpb_random)(void *sub);
    uint8_t (*_bt_mesh_ext_sub_get_private_node_id)(void *sub);
    uint8_t *(*_bt_mesh_ext_sub_get_keys_private_beacon)(void *sub, uint8_t index);
/* CONFIG_BLE_MESH_PRIVATE_BEACON */

/* CONFIG_BLE_MESH_BRC_SRV */
    uint16_t (*_bt_mesh_ext_sub_get_sbr_net_idx)(void *sub);
    void (*_bt_mesh_ext_sub_set_sbr_net_idx)(void *sub, uint16_t sbr_net_idx);
    void *(*_bt_mesh_ext_brc_srv_get_bridge_table_entry)(void *srv, uint8_t index);
    void *(*_bt_mesh_ext_brc_srv_get_bridge_rpl)(uint16_t index);
/* CONFIG_BLE_MESH_BRC_SRV */

/* CONFIG_BLE_MESH_AGG_CLI */
    void (*_bt_mesh_ext_agg_client_cb_evt_to_btc)(uint32_t opcode, uint8_t event,
                                                  void *model, void *ctx,
                                                  const uint8_t *val, size_t len);
/* CONFIG_BLE_MESH_AGG_CLI */

/* CONFIG_BLE_MESH_AGG_SRV */
    void (*_bt_mesh_ext_agg_server_cb_evt_to_btc)(uint8_t event, void *model, void *ctx,
                                                  const void *val, size_t len);
/* CONFIG_BLE_MESH_AGG_SRV */

/* CONFIG_BLE_MESH_BRC_CLI */
    void (*_bt_mesh_ext_brc_client_cb_evt_to_btc)(uint32_t opcode, uint8_t event,
                                                  void *model, void *ctx,
                                                  const uint8_t *val, size_t len);
/* CONFIG_BLE_MESH_BRC_CLI */

/* CONFIG_BLE_MESH_BRC_SRV */
    void (*_bt_mesh_ext_brc_server_cb_evt_to_btc)(uint8_t event, void *model, void *ctx,
                                                  const uint8_t *val, size_t len);
/* CONFIG_BLE_MESH_BRC_SRV */

/* CONFIG_BLE_MESH_DF_CLI */
    void (*_bt_mesh_ext_df_client_cb_evt_to_btc)(uint32_t opcode, uint8_t event,
                                                 void *model, void *ctx,
                                                 const uint8_t *val, size_t len);
/* CONFIG_BLE_MESH_DF_CLI */

/* CONFIG_BLE_MESH_DF_SRV */
    void (*_bt_mesh_ext_df_server_cb_evt_to_btc)(uint8_t event, void *model, void *ctx,
                                                 const uint8_t *val, size_t len);
/* CONFIG_BLE_MESH_DF_SRV */

/* CONFIG_BLE_MESH_LCD_CLI */
    void (*_bt_mesh_ext_lcd_client_cb_evt_to_btc)(uint32_t opcode, uint8_t event,
                                                  void *model, void *ctx,
                                                  const void *val, size_t len);
/* CONFIG_BLE_MESH_LCD_CLI */

/* CONFIG_BLE_MESH_ODP_CLI */
    void (*_bt_mesh_ext_odp_client_cb_evt_to_btc)(uint32_t opcode, uint8_t event,
                                                  void *model, void *ctx,
                                                  const uint8_t *val, size_t len);
/* CONFIG_BLE_MESH_ODP_CLI */

/* CONFIG_BLE_MESH_PRB_CLI */
    void (*_bt_mesh_ext_prb_client_cb_evt_to_btc)(uint32_t opcode, uint8_t evt_type,
                                                  void *model, void *ctx,
                                                  const uint8_t *val, uint16_t len);
/* CONFIG_BLE_MESH_PRB_CLI */

/* CONFIG_BLE_MESH_RPR_CLI */
    void (*_btc_ble_mesh_ext_rpr_client_link_close_cb)(void *model, uint16_t rpr_srv_addr,
                                                       uint8_t reason);

    void (*_btc_ble_mesh_ext_rpr_client_prov_comp_cb)(void *model, uint16_t rpr_srv_addr,
                                                      uint8_t nppi, uint16_t index,
                                                      uint8_t uuid[16], uint16_t unicast_addr,
                                                      uint8_t element_num, uint16_t net_idx);

    void (*_bt_mesh_ext_rpr_client_cb_evt_to_btc)(uint32_t opcode, uint8_t event,
                                                  void *model, void *ctx,
                                                  const void *val, size_t len);
/* CONFIG_BLE_MESH_RPR_CLI */

/* CONFIG_BLE_MESH_RPR_SRV */
    void (*_bt_mesh_ext_rpr_server_cb_evt_to_btc)(uint8_t event, const void *val, size_t len);
/* CONFIG_BLE_MESH_RPR_SRV */

/* CONFIG_BLE_MESH_SAR_CLI */
    void (*_bt_mesh_ext_sar_client_cb_evt_to_btc)(uint32_t opcode, uint8_t event,
                                                  void *model, void *ctx,
                                                  const uint8_t *val, size_t len);
/* CONFIG_BLE_MESH_SAR_CLI */

/* CONFIG_BLE_MESH_SRPL_CLI */
    void (*_bt_mesh_ext_srpl_client_cb_evt_to_btc)(uint32_t opcode, uint8_t event,
                                                   void *model, void *ctx,
                                                   const uint8_t *val, size_t len);
/* CONFIG_BLE_MESH_SRPL_CLI */

/* CONFIG_BLE_MESH_MBT_CLI */
    void (*_bt_mesh_ext_mbt_client_cb_evt_to_btc)(uint8_t event, uint8_t result, void *model);
/* CONFIG_BLE_MESH_MBT_CLI */

/* CONFIG_BLE_MESH_MBT_SRV */
    void (*_bt_mesh_ext_mbt_server_cb_evt_to_btc)(uint8_t event, void *model, void *ctx);
/* CONFIG_BLE_MESH_MBT_SRV */
} bt_mesh_ext_funcs_t;

static const bt_mesh_ext_funcs_t bt_mesh_ext_func = {
/* CONFIG_BLE_MESH_NODE */
    ._bt_mesh_ext_prov_node_next_xact_id                = bt_mesh_ext_prov_node_next_xact_id,
    ._bt_mesh_ext_prov_node_get_link                    = bt_mesh_ext_prov_node_get_link,
    ._bt_mesh_ext_prov_node_close_link                  = bt_mesh_ext_prov_node_close_link,
    ._bt_mesh_ext_prov_node_get_uuid                    = bt_mesh_ext_prov_node_get_uuid,
    ._bt_mesh_ext_prov_node_get_oob_info                = bt_mesh_ext_prov_node_get_oob_info,
    ._bt_mesh_ext_prov_node_get_uri                     = bt_mesh_ext_prov_node_get_uri,
/* CONFIG_BLE_MESH_NODE */

/* CONFIG_BLE_MESH_PROVISIONER */
    ._bt_mesh_ext_prov_pvnr_next_xact_id                = bt_mesh_ext_prov_pvnr_next_xact_id,
    ._bt_mesh_ext_prov_pvnr_get_link                    = bt_mesh_ext_prov_pvnr_get_link,
    ._bt_mesh_ext_prov_pvnr_get_link_count              = bt_mesh_ext_prov_pvnr_get_link_count,
    ._bt_mesh_ext_prov_pvnr_send_invite                 = bt_mesh_ext_prov_pvnr_send_invite,
    ._bt_mesh_ext_prov_pvnr_close_link                  = bt_mesh_ext_prov_pvnr_close_link,
    ._bt_mesh_ext_pvnr_get_node_with_addr               = bt_mesh_ext_pvnr_get_node_with_addr,
/* CONFIG_BLE_MESH_CERT_BASED_PROV */
    ._bt_mesh_ext_pvnr_records_list_get_cb              = bt_mesh_ext_pvnr_records_list_get_cb,
    ._bt_mesh_ext_pvnr_records_recv_comp_cb             = bt_mesh_ext_pvnr_records_recv_comp_cb,
    ._bt_mesh_ext_prov_link_get_record_id_expect        = bt_mesh_ext_prov_link_get_record_id_expect,
    ._bt_mesh_ext_prov_link_set_record_id_expect        = bt_mesh_ext_prov_link_set_record_id_expect,
    ._bt_mesh_ext_prov_link_get_offset_expect           = bt_mesh_ext_prov_link_get_offset_expect,
    ._bt_mesh_ext_prov_link_set_offset_expect           = bt_mesh_ext_prov_link_set_offset_expect,
    ._bt_mesh_ext_prov_link_get_max_size                = bt_mesh_ext_prov_link_get_max_size,
    ._bt_mesh_ext_prov_link_set_max_size                = bt_mesh_ext_prov_link_set_max_size,
    ._bt_mesh_ext_prov_link_get_record                  = bt_mesh_ext_prov_link_get_record,
    ._bt_mesh_ext_prov_link_alloc_record                = bt_mesh_ext_prov_link_alloc_record,
/* CONFIG_BLE_MESH_CERT_BASED_PROV */
    ._bt_mesh_ext_pvnr_get_node_uuid                    = bt_mesh_ext_pvnr_get_node_uuid,
    ._bt_mesh_ext_prov_link_get_uuid                    = bt_mesh_ext_prov_link_get_uuid,
    ._bt_mesh_ext_prov_link_get_elem_num                = bt_mesh_ext_prov_link_get_elem_num,
    ._bt_mesh_ext_prov_link_get_unicast_addr            = bt_mesh_ext_prov_link_get_unicast_addr,
/* CONFIG_BLE_MESH_PROVISIONER */

/* CONFIG_BLE_MESH_PB_GATT */
    ._bt_mesh_ext_prov_link_get_conn                    = bt_mesh_ext_prov_link_get_conn,
    ._bt_mesh_ext_prov_link_pb_gatt_exist               = bt_mesh_ext_prov_link_pb_gatt_exist,
    ._bt_mesh_ext_prov_link_pb_gatt_send                = bt_mesh_ext_prov_link_pb_gatt_send,
/* CONFIG_BLE_MESH_PB_GATT */

/* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_SERVER */
    ._bt_mesh_ext_get_device_name                       = bt_mesh_ext_get_device_name,
/* (CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_SERVER */

/* CONFIG_BLE_MESH_GATT_PROXY_SERVER */
    ._bt_mesh_ext_proxy_server_get_all_client_type      = bt_mesh_ext_proxy_server_get_all_client_type,
    ._bt_mesh_ext_proxy_server_segment_send             = bt_mesh_ext_proxy_server_segment_send,
    ._bt_mesh_ext_proxy_server_find_client_by_addr      = bt_mesh_ext_proxy_server_find_client_by_addr,
/* CONFIG_BLE_MESH_PRB_SRV */
    ._bt_mesh_ext_proxy_server_update_net_id_rand_stop  = bt_mesh_ext_proxy_server_update_net_id_rand_stop,
/* CONFIG_BLE_MESH_PRB_SRV */
    ._bt_mesh_ext_proxy_server_get_client               = bt_mesh_ext_proxy_server_get_client,
    ._bt_mesh_ext_proxy_server_get_client_count         = bt_mesh_ext_proxy_server_get_client_count,
/* CONFIG_BLE_MESH_PRB_SRV */
    ._bt_mesh_ext_proxy_server_update_net_id_rand       = bt_mesh_ext_proxy_server_update_net_id_rand,
    ._bt_mesh_ext_proxy_server_is_node_id_enable        = bt_mesh_ext_proxy_server_is_node_id_enable,
    ._bt_mesh_ext_proxy_server_private_identity_start   = bt_mesh_ext_proxy_server_private_identity_start,
    ._bt_mesh_ext_proxy_server_private_identity_stop    = bt_mesh_ext_proxy_server_private_identity_stop,
/* CONFIG_BLE_MESH_PRB_SRV */
    ._bt_mesh_ext_proxy_server_get_filter_type          = bt_mesh_ext_proxy_server_get_filter_type,
    ._bt_mesh_ext_proxy_server_get_filter_count         = bt_mesh_ext_proxy_server_get_filter_count,
    ._bt_mesh_ext_proxy_server_get_filter_size          = bt_mesh_ext_proxy_server_get_filter_size,
    ._bt_mesh_ext_proxy_server_get_filter_addr          = bt_mesh_ext_proxy_server_get_filter_addr,
    ._bt_mesh_ext_proxy_server_filter_is_client         = bt_mesh_ext_proxy_server_filter_is_client,
    ._bt_mesh_ext_proxy_server_get_client_type          = bt_mesh_ext_proxy_server_get_client_type,
    ._bt_mesh_ext_proxy_server_is_proxy_msg_recv        = bt_mesh_ext_proxy_server_is_proxy_msg_recv,
    ._bt_mesh_ext_proxy_server_get_conn                 = bt_mesh_ext_proxy_server_get_conn,
    ._bt_mesh_ext_proxy_server_set_client_type          = bt_mesh_ext_proxy_server_set_client_type,
    ._bt_mesh_ext_proxy_server_set_msg_recv             = bt_mesh_ext_proxy_server_set_msg_recv,
/* CONFIG_BLE_MESH_GATT_PROXY_SERVER */

/* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */
    ._bt_mesh_ext_proxy_client_beacon_send              = bt_mesh_ext_proxy_client_beacon_send,
    ._bt_mesh_ext_proxy_client_get_opcode               = bt_mesh_ext_proxy_client_get_opcode,
    ._bt_mesh_ext_proxy_client_get_use_directed         = bt_mesh_ext_proxy_client_get_use_directed,
    ._bt_mesh_ext_proxy_client_get_client_uar           = bt_mesh_ext_proxy_client_get_client_uar,
/* CONFIG_BLE_MESH_GATT_PROXY_CLIENT */

/* CONFIG_BLE_MESH_FRIEND */
    ._bt_mesh_ext_frnd_get_lpn                          = bt_mesh_ext_frnd_get_lpn,
    ._bt_mesh_ext_frnd_get_num_elem                     = bt_mesh_ext_frnd_get_num_elem,
    ._bt_mesh_ext_frnd_is_valid                         = bt_mesh_ext_frnd_is_valid,
    ._bt_mesh_ext_frnd_is_established                   = bt_mesh_ext_frnd_is_established,
    ._bt_mesh_ext_frnd_get_net_idx                      = bt_mesh_ext_frnd_get_net_idx,
    ._bt_mesh_ext_frnd_get_sub_list_count               = bt_mesh_ext_frnd_get_sub_list_count,
    ._bt_mesh_ext_frnd_get_sub_list_size                = bt_mesh_ext_frnd_get_sub_list_size,
    ._bt_mesh_ext_frnd_get_sub_addr                     = bt_mesh_ext_frnd_get_sub_addr,
    ._bt_mesh_ext_net_get_frnd_count                    = bt_mesh_ext_net_get_frnd_count,
    ._bt_mesh_ext_net_get_frnd                          = bt_mesh_ext_net_get_frnd,
    ._bt_mesh_ext_friend_match                          = bt_mesh_ext_friend_match,
    ._bt_mesh_ext_friend_unicast_match                  = bt_mesh_ext_friend_unicast_match,
    ._bt_mesh_ext_friend_find                           = bt_mesh_ext_friend_find,
    ._bt_mesh_ext_friend_clear_net_idx                  = bt_mesh_ext_friend_clear_net_idx,
/* CONFIG_BLE_MESH_FRIEND */

/* CONFIG_BLE_MESH_LOW_POWER */
    ._bt_mesh_ext_lpn_match                             = bt_mesh_ext_lpn_match,
    ._bt_mesh_ext_lpn_frnd                              = bt_mesh_ext_lpn_frnd,
/* CONFIG_BLE_MESH_LOW_POWER */

/* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
    ._bt_mesh_ext_update_exceptional_list               = bt_mesh_ext_update_exceptional_list,
/* CONFIG_BLE_MESH_USE_DUPLICATE_SCAN */
/* (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) */
    ._bt_mesh_ext_unprov_dev_info_query                 = bt_mesh_ext_unprov_dev_info_query,
/* (CONFIG_BLE_MESH_PROVISIONER || CONFIG_BLE_MESH_RPR_SRV) */

/* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
   (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT) */
    ._bt_mesh_ext_gattc_conn_create                     = bt_mesh_ext_gattc_conn_create,
    ._bt_mesh_ext_gattc_disconnect                      = bt_mesh_ext_gattc_disconnect,
/* (CONFIG_BLE_MESH_PROVISIONER && CONFIG_BLE_MESH_PB_GATT) || \
    CONFIG_BLE_MESH_GATT_PROXY_CLIENT || \
   (CONFIG_BLE_MESH_RPR_SRV && CONFIG_BLE_MESH_PB_GATT) */

/* CONFIG_BLE_MESH_SETTINGS */
    ._bt_mesh_ext_store_dkca                            = bt_mesh_ext_store_dkca,
    ._bt_mesh_ext_clear_dkca                            = bt_mesh_ext_clear_dkca,
    ._bt_mesh_ext_clear_rpl_single                      = bt_mesh_ext_clear_rpl_single,
    ._bt_mesh_ext_store_net                             = bt_mesh_ext_store_net,
    ._bt_mesh_ext_store_seq                             = bt_mesh_ext_store_seq,
    ._bt_mesh_ext_get_core_settings_item                = bt_mesh_ext_get_core_settings_item,
    ._bt_mesh_ext_is_settings_item_exist                = bt_mesh_ext_is_settings_item_exist,
    ._bt_mesh_ext_add_core_settings_item                = bt_mesh_ext_add_core_settings_item,
    ._bt_mesh_ext_remove_core_settings_item             = bt_mesh_ext_remove_core_settings_item,
    ._bt_mesh_ext_save_core_settings                    = bt_mesh_ext_save_core_settings,
    ._bt_mesh_ext_erase_core_settings                   = bt_mesh_ext_erase_core_settings,
/* CONFIG_BLE_MESH_SETTINGS */

/* CONFIG_BLE_MESH_DF_SRV */
    ._bt_mesh_ext_model_get_pub_directed_pub_policy     = bt_mesh_ext_model_get_pub_directed_pub_policy,
    ._bt_mesh_ext_model_set_pub_directed_pub_policy     = bt_mesh_ext_model_set_pub_directed_pub_policy,
    ._bt_mesh_ext_sub_get_keys_direct_nid               = bt_mesh_ext_sub_get_keys_direct_nid,
    ._bt_mesh_ext_sub_get_keys_direct_enc               = bt_mesh_ext_sub_get_keys_direct_enc,
    ._bt_mesh_ext_sub_get_keys_direct_privacy           = bt_mesh_ext_sub_get_keys_direct_privacy,
    ._bt_mesh_ext_sub_get_directed_forwarding           = bt_mesh_ext_sub_get_directed_forwarding,
    ._bt_mesh_ext_sub_set_directed_forwarding           = bt_mesh_ext_sub_set_directed_forwarding,
    ._bt_mesh_ext_sub_get_directed_relay                = bt_mesh_ext_sub_get_directed_relay,
    ._bt_mesh_ext_sub_set_directed_relay                = bt_mesh_ext_sub_set_directed_relay,
    ._bt_mesh_ext_sub_get_directed_proxy                = bt_mesh_ext_sub_get_directed_proxy,
    ._bt_mesh_ext_sub_set_directed_proxy                = bt_mesh_ext_sub_set_directed_proxy,
    ._bt_mesh_ext_sub_get_directed_proxy_use_default    = bt_mesh_ext_sub_get_directed_proxy_use_default,
    ._bt_mesh_ext_sub_set_directed_proxy_use_default    = bt_mesh_ext_sub_set_directed_proxy_use_default,
    ._bt_mesh_ext_sub_get_directed_friend               = bt_mesh_ext_sub_get_directed_friend,
    ._bt_mesh_ext_sub_set_directed_friend               = bt_mesh_ext_sub_set_directed_friend,
    ._bt_mesh_ext_sub_get_use_directed                  = bt_mesh_ext_sub_get_use_directed,
    ._bt_mesh_ext_sub_set_use_directed                  = bt_mesh_ext_sub_set_use_directed,
    ._bt_mesh_ext_sub_get_proxy_client_uar              = bt_mesh_ext_sub_get_proxy_client_uar,
    ._bt_mesh_ext_sub_get_proxy_client_uar_len_present  = bt_mesh_ext_sub_get_proxy_client_uar_len_present,
    ._bt_mesh_ext_sub_set_proxy_client_uar_len_present  = bt_mesh_ext_sub_set_proxy_client_uar_len_present,
    ._bt_mesh_ext_sub_get_proxy_client_uar_range_start  = bt_mesh_ext_sub_get_proxy_client_uar_range_start,
    ._bt_mesh_ext_sub_set_proxy_client_uar_range_start  = bt_mesh_ext_sub_set_proxy_client_uar_range_start,
    ._bt_mesh_ext_sub_get_proxy_client_uar_range_length = bt_mesh_ext_sub_get_proxy_client_uar_range_length,
    ._bt_mesh_ext_sub_set_proxy_client_uar_range_length = bt_mesh_ext_sub_set_proxy_client_uar_range_length,
    ._bt_mesh_ext_sub_get_path_metric_type              = bt_mesh_ext_sub_get_path_metric_type,
    ._bt_mesh_ext_sub_set_path_metric_type              = bt_mesh_ext_sub_set_path_metric_type,
    ._bt_mesh_ext_sub_get_path_lifetime_type            = bt_mesh_ext_sub_get_path_lifetime_type,
    ._bt_mesh_ext_sub_set_path_lifetime_type            = bt_mesh_ext_sub_set_path_lifetime_type,
    ._bt_mesh_ext_sub_get_two_way_path                  = bt_mesh_ext_sub_get_two_way_path,
    ._bt_mesh_ext_sub_set_two_way_path                  = bt_mesh_ext_sub_set_two_way_path,
    ._bt_mesh_ext_sub_get_forward_number                = bt_mesh_ext_sub_get_forward_number,
    ._bt_mesh_ext_sub_set_forward_number                = bt_mesh_ext_sub_set_forward_number,
    ._bt_mesh_ext_sub_get_disc_table_max_disc_entries   = bt_mesh_ext_sub_get_disc_table_max_disc_entries,
    ._bt_mesh_ext_sub_set_disc_table_max_disc_entries   = bt_mesh_ext_sub_set_disc_table_max_disc_entries,
    ._bt_mesh_ext_sub_get_disc_table_max_concurr_init   = bt_mesh_ext_sub_get_disc_table_max_concurr_init,
    ._bt_mesh_ext_sub_set_disc_table_max_concurr_init   = bt_mesh_ext_sub_set_disc_table_max_concurr_init,
    ._bt_mesh_ext_sub_get_disc_table_concurr_init       = bt_mesh_ext_sub_get_disc_table_concurr_init,
    ._bt_mesh_ext_sub_set_disc_table_concurr_init       = bt_mesh_ext_sub_set_disc_table_concurr_init,
    ._bt_mesh_ext_sub_inc_disc_table_concurr_init       = bt_mesh_ext_sub_inc_disc_table_concurr_init,
    ._bt_mesh_ext_sub_dec_disc_table_concurr_init       = bt_mesh_ext_sub_dec_disc_table_concurr_init,
    ._bt_mesh_ext_sub_get_disc_table_mutex              = bt_mesh_ext_sub_get_disc_table_mutex,
    ._bt_mesh_ext_sub_get_disc_table_entries            = bt_mesh_ext_sub_get_disc_table_entries,
    ._bt_mesh_ext_sub_get_fwd_table_max_ford_entries    = bt_mesh_ext_sub_get_fwd_table_max_ford_entries,
    ._bt_mesh_ext_sub_set_fwd_table_max_ford_entries    = bt_mesh_ext_sub_set_fwd_table_max_ford_entries,
    ._bt_mesh_ext_sub_get_fwd_table_max_deps_nodes      = bt_mesh_ext_sub_get_fwd_table_max_deps_nodes,
    ._bt_mesh_ext_sub_set_fwd_table_max_deps_nodes      = bt_mesh_ext_sub_set_fwd_table_max_deps_nodes,
    ._bt_mesh_ext_sub_get_fwd_table_update_id           = bt_mesh_ext_sub_get_fwd_table_update_id,
    ._bt_mesh_ext_sub_set_fwd_table_update_id           = bt_mesh_ext_sub_set_fwd_table_update_id,
    ._bt_mesh_ext_sub_inc_fwd_table_update_id           = bt_mesh_ext_sub_inc_fwd_table_update_id,
    ._bt_mesh_ext_sub_get_fwd_table_mutex               = bt_mesh_ext_sub_get_fwd_table_mutex,
    ._bt_mesh_ext_sub_get_fwd_table_entries             = bt_mesh_ext_sub_get_fwd_table_entries,
    ._bt_mesh_ext_sub_get_wanted_lanes                  = bt_mesh_ext_sub_get_wanted_lanes,
    ._bt_mesh_ext_sub_set_wanted_lanes                  = bt_mesh_ext_sub_set_wanted_lanes,
    ._bt_mesh_ext_sub_get_unicast_echo_interval         = bt_mesh_ext_sub_get_unicast_echo_interval,
    ._bt_mesh_ext_sub_set_unicast_echo_interval         = bt_mesh_ext_sub_set_unicast_echo_interval,
    ._bt_mesh_ext_sub_get_multicast_echo_interval       = bt_mesh_ext_sub_get_multicast_echo_interval,
    ._bt_mesh_ext_sub_set_multicast_echo_interval       = bt_mesh_ext_sub_set_multicast_echo_interval,
/* CONFIG_BLE_MESH_DF_SRV */

/* CONFIG_BLE_MESH_RPR_CLI */
    ._bt_mesh_ext_rpr_cli_pdu_send                      = bt_mesh_ext_rpr_cli_pdu_send,
    ._bt_mesh_ext_rpr_cli_recv_pub_key_outbound_report  = bt_mesh_ext_rpr_cli_recv_pub_key_outbound_report,
    ._bt_mesh_ext_rpr_cli_pdu_recv                      = bt_mesh_ext_rpr_cli_pdu_recv,
    ._bt_mesh_ext_bt_mesh_is_unprov_dev_being_prov      = bt_mesh_ext_bt_mesh_is_unprov_dev_being_prov,
    ._bt_mesh_ext_rpr_cli_get_rpr_link                  = bt_mesh_ext_rpr_cli_get_rpr_link,
/* CONFIG_BLE_MESH_RPR_CLI */

/* CONFIG_BLE_MESH_RPR_SRV */
    ._bt_mesh_ext_rpr_srv_reset_prov_link               = bt_mesh_ext_rpr_srv_reset_prov_link,
    ._bt_mesh_ext_rpr_srv_nppi_pdu_recv                 = bt_mesh_ext_rpr_srv_nppi_pdu_recv,
/* (CONFIG_BLE_MESH_GATT_PROXY_CLIENT && CONFIG_BLE_MESH_PB_GATT) */
    ._bt_mesh_ext_rpr_srv_set_waiting_prov_link         = bt_mesh_ext_rpr_srv_set_waiting_prov_link,
/* (CONFIG_BLE_MESH_GATT_PROXY_CLIENT && CONFIG_BLE_MESH_PB_GATT) */
/* CONFIG_BLE_MESH_RPR_SRV */

/* CONFIG_BLE_MESH_PRIVATE_BEACON */
    ._bt_mesh_ext_net_get_sub_mpb_rand                  = bt_mesh_ext_net_get_sub_mpb_rand,
    ._bt_mesh_ext_sub_get_mpb_sent                      = bt_mesh_ext_sub_get_mpb_sent,
    ._bt_mesh_ext_sub_set_mpb_sent                      = bt_mesh_ext_sub_set_mpb_sent,
    ._bt_mesh_ext_sub_get_mpb_last                      = bt_mesh_ext_sub_get_mpb_last,
    ._bt_mesh_ext_sub_set_mpb_last                      = bt_mesh_ext_sub_set_mpb_last,
    ._bt_mesh_ext_sub_get_mpb_cur                       = bt_mesh_ext_sub_get_mpb_cur,
    ._bt_mesh_ext_sub_set_mpb_cur                       = bt_mesh_ext_sub_set_mpb_cur,
    ._bt_mesh_ext_sub_inc_mpb_cur                       = bt_mesh_ext_sub_inc_mpb_cur,
    ._bt_mesh_ext_sub_get_mpb_flags_last                = bt_mesh_ext_sub_get_mpb_flags_last,
    ._bt_mesh_ext_sub_set_mpb_flags_last                = bt_mesh_ext_sub_set_mpb_flags_last,
    ._bt_mesh_ext_sub_get_mpb_ivi_last                  = bt_mesh_ext_sub_get_mpb_ivi_last,
    ._bt_mesh_ext_sub_set_mpb_ivi_last                  = bt_mesh_ext_sub_set_mpb_ivi_last,
    ._bt_mesh_ext_sub_get_mpb_random                    = bt_mesh_ext_sub_get_mpb_random,
    ._bt_mesh_ext_sub_get_private_node_id               = bt_mesh_ext_sub_get_private_node_id,
    ._bt_mesh_ext_sub_get_keys_private_beacon           = bt_mesh_ext_sub_get_keys_private_beacon,
/* CONFIG_BLE_MESH_PRIVATE_BEACON */

/* CONFIG_BLE_MESH_BRC_SRV */
    ._bt_mesh_ext_sub_get_sbr_net_idx                   = bt_mesh_ext_sub_get_sbr_net_idx,
    ._bt_mesh_ext_sub_set_sbr_net_idx                   = bt_mesh_ext_sub_set_sbr_net_idx,
    ._bt_mesh_ext_brc_srv_get_bridge_table_entry        = bt_mesh_ext_brc_srv_get_bridge_table_entry,
    ._bt_mesh_ext_brc_srv_get_bridge_rpl                = bt_mesh_ext_brc_srv_get_bridge_rpl,
/* CONFIG_BLE_MESH_BRC_SRV */

/* CONFIG_BLE_MESH_AGG_CLI */
    ._bt_mesh_ext_agg_client_cb_evt_to_btc              = bt_mesh_ext_agg_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_AGG_CLI */
/* CONFIG_BLE_MESH_AGG_SRV */
    ._bt_mesh_ext_agg_server_cb_evt_to_btc              = bt_mesh_ext_agg_server_cb_evt_to_btc,
/* CONFIG_BLE_MESH_AGG_SRV */
/* CONFIG_BLE_MESH_BRC_CLI */
    ._bt_mesh_ext_brc_client_cb_evt_to_btc              = bt_mesh_ext_brc_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_BRC_CLI */
/* CONFIG_BLE_MESH_BRC_SRV */
    ._bt_mesh_ext_brc_server_cb_evt_to_btc              = bt_mesh_ext_brc_server_cb_evt_to_btc,
/* CONFIG_BLE_MESH_BRC_SRV */
/* CONFIG_BLE_MESH_DF_CLI */
    ._bt_mesh_ext_df_client_cb_evt_to_btc               = bt_mesh_ext_df_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_DF_CLI */
/* CONFIG_BLE_MESH_DF_SRV */
    ._bt_mesh_ext_df_server_cb_evt_to_btc               = bt_mesh_ext_df_server_cb_evt_to_btc,
/* CONFIG_BLE_MESH_DF_SRV */
/* CONFIG_BLE_MESH_LCD_CLI */
    ._bt_mesh_ext_lcd_client_cb_evt_to_btc              = bt_mesh_ext_lcd_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_LCD_CLI */
/* CONFIG_BLE_MESH_ODP_CLI */
    ._bt_mesh_ext_odp_client_cb_evt_to_btc              = bt_mesh_ext_odp_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_ODP_CLI */
/* CONFIG_BLE_MESH_PRB_CLI */
    ._bt_mesh_ext_prb_client_cb_evt_to_btc              = bt_mesh_ext_prb_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_PRB_CLI */
/* CONFIG_BLE_MESH_RPR_CLI */
    ._btc_ble_mesh_ext_rpr_client_link_close_cb         = btc_ble_mesh_ext_rpr_client_link_close_cb,
    ._btc_ble_mesh_ext_rpr_client_prov_comp_cb          = btc_ble_mesh_ext_rpr_client_prov_comp_cb,
    ._bt_mesh_ext_rpr_client_cb_evt_to_btc              = bt_mesh_ext_rpr_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_RPR_CLI */
/* CONFIG_BLE_MESH_RPR_SRV */
    ._bt_mesh_ext_rpr_server_cb_evt_to_btc              = bt_mesh_ext_rpr_server_cb_evt_to_btc,
/* CONFIG_BLE_MESH_RPR_SRV */
/* CONFIG_BLE_MESH_SAR_CLI */
    ._bt_mesh_ext_sar_client_cb_evt_to_btc              = bt_mesh_ext_sar_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_SAR_CLI */
/* CONFIG_BLE_MESH_SRPL_CLI */
    ._bt_mesh_ext_srpl_client_cb_evt_to_btc             = bt_mesh_ext_srpl_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_SRPL_CLI */
/* CONFIG_BLE_MESH_MBT_CLI */
    ._bt_mesh_ext_mbt_client_cb_evt_to_btc              = bt_mesh_ext_mbt_client_cb_evt_to_btc,
/* CONFIG_BLE_MESH_MBT_CLI */
/* CONFIG_BLE_MESH_MBT_SRV */
    ._bt_mesh_ext_mbt_server_cb_evt_to_btc              = bt_mesh_ext_mbt_server_cb_evt_to_btc,
/* CONFIG_BLE_MESH_MBT_SRV */
};

#define BLE_MESH_LIB_TRACE_TAG             "BLE_MESH(lib)"
#define BLE_MESH_LOG_FORMAT_START(level)   LOG_COLOR_ ## level #level " (%" PRIu32 ") %s: "
#define BLE_MESH_LOG_FORMAT_END            LOG_RESET_COLOR "\n"

void bt_mesh_lib_log_error(const char *format, ...)
{
#if (CONFIG_BLE_MESH_NO_LOG ||\
    /* Disable log output when compressed logging
     * is enabled but ERR logs are not preserved */\
    (CONFIG_BLE_MESH_STACK_ERR_LOG_COMPRESSION &&\
    !CONFIG_BLE_MESH_STACK_ERR_LOG_PRESERVE))
    return;
#else
    if ((BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_ERROR) &&
        BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH, ERROR)) {
        va_list args = {0};
        va_start(args, format);
        esp_log_write(ESP_LOG_ERROR, BLE_MESH_LIB_TRACE_TAG, BLE_MESH_LOG_FORMAT_START(E), esp_log_timestamp(), BLE_MESH_LIB_TRACE_TAG);
        esp_log_writev(ESP_LOG_ERROR, BLE_MESH_LIB_TRACE_TAG, format, args);
        esp_log_write(ESP_LOG_ERROR, BLE_MESH_LIB_TRACE_TAG, BLE_MESH_LOG_FORMAT_END);
        va_end(args);
    }
#endif
}

void bt_mesh_lib_log_warn(const char *format, ...)
{
#if (CONFIG_BLE_MESH_NO_LOG ||\
    /* Disable log output when compressed logging
     * is enabled but WARN logs are not preserved */\
    (CONFIG_BLE_MESH_STACK_WARN_LOG_COMPRESSION &&\
    !CONFIG_BLE_MESH_STACK_WARN_LOG_PRESERVE))
    return;
#else
    if ((BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_WARN) &&
        BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH, WARN)) {
        va_list args = {0};
        va_start(args, format);
        esp_log_write(ESP_LOG_WARN, BLE_MESH_LIB_TRACE_TAG, BLE_MESH_LOG_FORMAT_START(W), esp_log_timestamp(), BLE_MESH_LIB_TRACE_TAG);
        esp_log_writev(ESP_LOG_WARN, BLE_MESH_LIB_TRACE_TAG, format, args);
        esp_log_write(ESP_LOG_WARN, BLE_MESH_LIB_TRACE_TAG, BLE_MESH_LOG_FORMAT_END);
        va_end(args);
    }
#endif
}

void bt_mesh_lib_log_info(const char *format, ...)
{
#if (CONFIG_BLE_MESH_NO_LOG ||\
    /* Disable log output when compressed logging
     * is enabled but INFO logs are not preserved */\
    (CONFIG_BLE_MESH_STACK_INFO_LOG_COMPRESSION &&\
    !CONFIG_BLE_MESH_STACK_INFO_LOG_PRESERVE))
    return;
#else
    if ((BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_INFO) &&
        BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH, INFO)) {
        va_list args = {0};
        va_start(args, format);
        esp_log_write(ESP_LOG_INFO, BLE_MESH_LIB_TRACE_TAG, BLE_MESH_LOG_FORMAT_START(I), esp_log_timestamp(), BLE_MESH_LIB_TRACE_TAG);
        esp_log_writev(ESP_LOG_INFO, BLE_MESH_LIB_TRACE_TAG, format, args);
        esp_log_write(ESP_LOG_INFO, BLE_MESH_LIB_TRACE_TAG, BLE_MESH_LOG_FORMAT_END);
        va_end(args);
    }
#endif
}

void bt_mesh_lib_log_debug(const char *format, ...)
{
#if (CONFIG_BLE_MESH_NO_LOG ||\
    /* Disable log output when compressed logging
     * is enabled but DEBUG logs are not preserved */\
    (CONFIG_BLE_MESH_STACK_DEBUG_LOG_COMPRESSION &&\
    !CONFIG_BLE_MESH_STACK_DEBUG_LOG_PRESERVE))
    return;
#else
    if ((BLE_MESH_LOG_LEVEL >= BLE_MESH_LOG_LEVEL_DEBUG) &&
        BLE_MESH_LOG_LEVEL_CHECK(BLE_MESH, DEBUG)) {
        va_list args = {0};
        va_start(args, format);
        esp_log_write(ESP_LOG_DEBUG, BLE_MESH_LIB_TRACE_TAG, BLE_MESH_LOG_FORMAT_START(D), esp_log_timestamp(), BLE_MESH_LIB_TRACE_TAG);
        esp_log_writev(ESP_LOG_DEBUG, BLE_MESH_LIB_TRACE_TAG, format, args);
        esp_log_write(ESP_LOG_DEBUG, BLE_MESH_LIB_TRACE_TAG, BLE_MESH_LOG_FORMAT_END);
        va_end(args);
    }
#endif
}

void ble_mesh_lib_compressed_out(uint8_t log_level, uint32_t log_index, size_t arg_cnt, ...)
{
#if CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE
    if (BLE_MESH_LOG_LEVEL >= log_level) {
        va_list args = {0};
        va_start(args, arg_cnt);
        extern int ble_log_compressed_hex_printv(uint8_t source, uint32_t log_index, size_t args_cnt, va_list args);
        ble_log_compressed_hex_printv(BLE_COMPRESSED_LOG_OUT_SOURCE_MESH_LIB, log_index, arg_cnt, args);
        va_end(args);
    }
#endif
    return;
}

void ble_mesh_lib_compressed_buf_out(uint8_t log_level, uint32_t log_index, uint8_t buf_idx, const uint8_t *buf, uint8_t len)
{
#if CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE
    if (BLE_MESH_LOG_LEVEL >= log_level) {
        extern int ble_log_compressed_hex_print_buf(uint8_t source, uint32_t log_index, uint8_t buf_idx, const uint8_t *buf, size_t len);
        ble_log_compressed_hex_print_buf(BLE_COMPRESSED_LOG_OUT_SOURCE_MESH_LIB, log_index, buf_idx, buf, len);
    }
#endif
    return;
}

/**
 * @brief  Keep symbols alive.
 * @note   Dummy function to stop the linker from
 *         optimizing away unused code.The dummy
 *         function is discarded after linking,
 *         so it adds zero bytes to the final binary.
 */
void bt_mesh_lib_ext_func_dummy_call(void)
{
    (void *)bt_hex(NULL, 0);
}

int bt_mesh_v11_ext_init(void)
{
    return bt_mesh_v11_init(&bt_mesh_ext_cfg, sizeof(bt_mesh_ext_cfg),
                            &bt_mesh_ext_func, sizeof(bt_mesh_ext_func));
}
