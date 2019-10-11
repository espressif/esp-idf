/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _FOUNDATION_H_
#define _FOUNDATION_H_

#include "mesh_access.h"
#include "net.h"

#define OP_APP_KEY_ADD                     BLE_MESH_MODEL_OP_1(0x00)
#define OP_APP_KEY_UPDATE                  BLE_MESH_MODEL_OP_1(0x01)
#define OP_DEV_COMP_DATA_STATUS            BLE_MESH_MODEL_OP_1(0x02)
#define OP_MOD_PUB_SET                     BLE_MESH_MODEL_OP_1(0x03)
#define OP_HEALTH_CURRENT_STATUS           BLE_MESH_MODEL_OP_1(0x04)
#define OP_HEALTH_FAULT_STATUS             BLE_MESH_MODEL_OP_1(0x05)
#define OP_HEARTBEAT_PUB_STATUS            BLE_MESH_MODEL_OP_1(0x06)
#define OP_APP_KEY_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x00)
#define OP_APP_KEY_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x01)
#define OP_APP_KEY_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x02)
#define OP_APP_KEY_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x03)
#define OP_ATTENTION_GET                   BLE_MESH_MODEL_OP_2(0x80, 0x04)
#define OP_ATTENTION_SET                   BLE_MESH_MODEL_OP_2(0x80, 0x05)
#define OP_ATTENTION_SET_UNREL             BLE_MESH_MODEL_OP_2(0x80, 0x06)
#define OP_ATTENTION_STATUS                BLE_MESH_MODEL_OP_2(0x80, 0x07)
#define OP_DEV_COMP_DATA_GET               BLE_MESH_MODEL_OP_2(0x80, 0x08)
#define OP_BEACON_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x09)
#define OP_BEACON_SET                      BLE_MESH_MODEL_OP_2(0x80, 0x0a)
#define OP_BEACON_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x0b)
#define OP_DEFAULT_TTL_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x0c)
#define OP_DEFAULT_TTL_SET                 BLE_MESH_MODEL_OP_2(0x80, 0x0d)
#define OP_DEFAULT_TTL_STATUS              BLE_MESH_MODEL_OP_2(0x80, 0x0e)
#define OP_FRIEND_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x0f)
#define OP_FRIEND_SET                      BLE_MESH_MODEL_OP_2(0x80, 0x10)
#define OP_FRIEND_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x11)
#define OP_GATT_PROXY_GET                  BLE_MESH_MODEL_OP_2(0x80, 0x12)
#define OP_GATT_PROXY_SET                  BLE_MESH_MODEL_OP_2(0x80, 0x13)
#define OP_GATT_PROXY_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0x14)
#define OP_KRP_GET                         BLE_MESH_MODEL_OP_2(0x80, 0x15)
#define OP_KRP_SET                         BLE_MESH_MODEL_OP_2(0x80, 0x16)
#define OP_KRP_STATUS                      BLE_MESH_MODEL_OP_2(0x80, 0x17)
#define OP_MOD_PUB_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x18)
#define OP_MOD_PUB_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x19)
#define OP_MOD_PUB_VA_SET                  BLE_MESH_MODEL_OP_2(0x80, 0x1a)
#define OP_MOD_SUB_ADD                     BLE_MESH_MODEL_OP_2(0x80, 0x1b)
#define OP_MOD_SUB_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x1c)
#define OP_MOD_SUB_DEL_ALL                 BLE_MESH_MODEL_OP_2(0x80, 0x1d)
#define OP_MOD_SUB_OVERWRITE               BLE_MESH_MODEL_OP_2(0x80, 0x1e)
#define OP_MOD_SUB_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x1f)
#define OP_MOD_SUB_VA_ADD                  BLE_MESH_MODEL_OP_2(0x80, 0x20)
#define OP_MOD_SUB_VA_DEL                  BLE_MESH_MODEL_OP_2(0x80, 0x21)
#define OP_MOD_SUB_VA_OVERWRITE            BLE_MESH_MODEL_OP_2(0x80, 0x22)
#define OP_NET_TRANSMIT_GET                BLE_MESH_MODEL_OP_2(0x80, 0x23)
#define OP_NET_TRANSMIT_SET                BLE_MESH_MODEL_OP_2(0x80, 0x24)
#define OP_NET_TRANSMIT_STATUS             BLE_MESH_MODEL_OP_2(0x80, 0x25)
#define OP_RELAY_GET                       BLE_MESH_MODEL_OP_2(0x80, 0x26)
#define OP_RELAY_SET                       BLE_MESH_MODEL_OP_2(0x80, 0x27)
#define OP_RELAY_STATUS                    BLE_MESH_MODEL_OP_2(0x80, 0x28)
#define OP_MOD_SUB_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x29)
#define OP_MOD_SUB_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x2a)
#define OP_MOD_SUB_GET_VND                 BLE_MESH_MODEL_OP_2(0x80, 0x2b)
#define OP_MOD_SUB_LIST_VND                BLE_MESH_MODEL_OP_2(0x80, 0x2c)
#define OP_LPN_TIMEOUT_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x2d)
#define OP_LPN_TIMEOUT_STATUS              BLE_MESH_MODEL_OP_2(0x80, 0x2e)
#define OP_HEALTH_FAULT_CLEAR              BLE_MESH_MODEL_OP_2(0x80, 0x2f)
#define OP_HEALTH_FAULT_CLEAR_UNREL        BLE_MESH_MODEL_OP_2(0x80, 0x30)
#define OP_HEALTH_FAULT_GET                BLE_MESH_MODEL_OP_2(0x80, 0x31)
#define OP_HEALTH_FAULT_TEST               BLE_MESH_MODEL_OP_2(0x80, 0x32)
#define OP_HEALTH_FAULT_TEST_UNREL         BLE_MESH_MODEL_OP_2(0x80, 0x33)
#define OP_HEALTH_PERIOD_GET               BLE_MESH_MODEL_OP_2(0x80, 0x34)
#define OP_HEALTH_PERIOD_SET               BLE_MESH_MODEL_OP_2(0x80, 0x35)
#define OP_HEALTH_PERIOD_SET_UNREL         BLE_MESH_MODEL_OP_2(0x80, 0x36)
#define OP_HEALTH_PERIOD_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x37)
#define OP_HEARTBEAT_PUB_GET               BLE_MESH_MODEL_OP_2(0x80, 0x38)
#define OP_HEARTBEAT_PUB_SET               BLE_MESH_MODEL_OP_2(0x80, 0x39)
#define OP_HEARTBEAT_SUB_GET               BLE_MESH_MODEL_OP_2(0x80, 0x3a)
#define OP_HEARTBEAT_SUB_SET               BLE_MESH_MODEL_OP_2(0x80, 0x3b)
#define OP_HEARTBEAT_SUB_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x3c)
#define OP_MOD_APP_BIND                    BLE_MESH_MODEL_OP_2(0x80, 0x3d)
#define OP_MOD_APP_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x3e)
#define OP_MOD_APP_UNBIND                  BLE_MESH_MODEL_OP_2(0x80, 0x3f)
#define OP_NET_KEY_ADD                     BLE_MESH_MODEL_OP_2(0x80, 0x40)
#define OP_NET_KEY_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x41)
#define OP_NET_KEY_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x42)
#define OP_NET_KEY_LIST                    BLE_MESH_MODEL_OP_2(0x80, 0x43)
#define OP_NET_KEY_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x44)
#define OP_NET_KEY_UPDATE                  BLE_MESH_MODEL_OP_2(0x80, 0x45)
#define OP_NODE_IDENTITY_GET               BLE_MESH_MODEL_OP_2(0x80, 0x46)
#define OP_NODE_IDENTITY_SET               BLE_MESH_MODEL_OP_2(0x80, 0x47)
#define OP_NODE_IDENTITY_STATUS            BLE_MESH_MODEL_OP_2(0x80, 0x48)
#define OP_NODE_RESET                      BLE_MESH_MODEL_OP_2(0x80, 0x49)
#define OP_NODE_RESET_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0x4a)
#define OP_SIG_MOD_APP_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x4b)
#define OP_SIG_MOD_APP_LIST                BLE_MESH_MODEL_OP_2(0x80, 0x4c)
#define OP_VND_MOD_APP_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x4d)
#define OP_VND_MOD_APP_LIST                BLE_MESH_MODEL_OP_2(0x80, 0x4e)

#define STATUS_SUCCESS                     0x00
#define STATUS_INVALID_ADDRESS             0x01
#define STATUS_INVALID_MODEL               0x02
#define STATUS_INVALID_APPKEY              0x03
#define STATUS_INVALID_NETKEY              0x04
#define STATUS_INSUFF_RESOURCES            0x05
#define STATUS_IDX_ALREADY_STORED          0x06
#define STATUS_NVAL_PUB_PARAM              0x07
#define STATUS_NOT_SUB_MOD                 0x08
#define STATUS_STORAGE_FAIL                0x09
#define STATUS_FEAT_NOT_SUPP               0x0a
#define STATUS_CANNOT_UPDATE               0x0b
#define STATUS_CANNOT_REMOVE               0x0c
#define STATUS_CANNOT_BIND                 0x0d
#define STATUS_TEMP_STATE_CHG_FAIL         0x0e
#define STATUS_CANNOT_SET                  0x0f
#define STATUS_UNSPECIFIED                 0x10
#define STATUS_INVALID_BINDING             0x11

int bt_mesh_cfg_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_health_srv_init(struct bt_mesh_model *model, bool primary);

int bt_mesh_cfg_cli_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_health_cli_init(struct bt_mesh_model *model, bool primary);

void bt_mesh_cfg_reset(void);

void bt_mesh_heartbeat(u16_t src, u16_t dst, u8_t hops, u16_t feat);

void bt_mesh_attention(struct bt_mesh_model *model, u8_t time);

u8_t *bt_mesh_label_uuid_get(u16_t addr);

struct bt_mesh_hb_pub *bt_mesh_hb_pub_get(void);
void bt_mesh_hb_pub_disable(void);
struct bt_mesh_cfg_srv *bt_mesh_cfg_get(void);

u8_t bt_mesh_net_transmit_get(void);
u8_t bt_mesh_relay_get(void);
u8_t bt_mesh_friend_get(void);
u8_t bt_mesh_relay_retransmit_get(void);
u8_t bt_mesh_beacon_get(void);
u8_t bt_mesh_gatt_proxy_get(void);
u8_t bt_mesh_default_ttl_get(void);

void bt_mesh_subnet_del(struct bt_mesh_subnet *sub, bool store);

struct bt_mesh_app_key *bt_mesh_app_key_alloc(u16_t app_idx);
void bt_mesh_app_key_del(struct bt_mesh_app_key *key, bool store);

static inline void key_idx_pack(struct net_buf_simple *buf,
                                u16_t idx1, u16_t idx2)
{
    net_buf_simple_add_le16(buf, idx1 | ((idx2 & 0x00f) << 12));
    net_buf_simple_add_u8(buf, idx2 >> 4);
}

static inline void key_idx_unpack(struct net_buf_simple *buf,
                                  u16_t *idx1, u16_t *idx2)
{
    *idx1 = sys_get_le16(&buf->data[0]) & 0xfff;
    *idx2 = sys_get_le16(&buf->data[1]) >> 4;
    net_buf_simple_pull(buf, 3);
}

#endif /* _FOUNDATION_H_ */
