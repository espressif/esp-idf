/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _FOUNDATION_H_
#define _FOUNDATION_H_

#include "mesh/byteorder.h"
#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OP_APP_KEY_ADD                              BLE_MESH_MODEL_OP_1(0x00)
#define OP_APP_KEY_UPDATE                           BLE_MESH_MODEL_OP_1(0x01)
#define OP_COMP_DATA_STATUS                         BLE_MESH_MODEL_OP_1(0x02)
#define OP_MOD_PUB_SET                              BLE_MESH_MODEL_OP_1(0x03)
#define OP_HEALTH_CURRENT_STATUS                    BLE_MESH_MODEL_OP_1(0x04)
#define OP_HEALTH_FAULT_STATUS                      BLE_MESH_MODEL_OP_1(0x05)
#define OP_HEARTBEAT_PUB_STATUS                     BLE_MESH_MODEL_OP_1(0x06)
#define OP_APP_KEY_DEL                              BLE_MESH_MODEL_OP_2(0x80, 0x00)
#define OP_APP_KEY_GET                              BLE_MESH_MODEL_OP_2(0x80, 0x01)
#define OP_APP_KEY_LIST                             BLE_MESH_MODEL_OP_2(0x80, 0x02)
#define OP_APP_KEY_STATUS                           BLE_MESH_MODEL_OP_2(0x80, 0x03)
#define OP_ATTENTION_GET                            BLE_MESH_MODEL_OP_2(0x80, 0x04)
#define OP_ATTENTION_SET                            BLE_MESH_MODEL_OP_2(0x80, 0x05)
#define OP_ATTENTION_SET_UNREL                      BLE_MESH_MODEL_OP_2(0x80, 0x06)
#define OP_ATTENTION_STATUS                         BLE_MESH_MODEL_OP_2(0x80, 0x07)
#define OP_COMP_DATA_GET                            BLE_MESH_MODEL_OP_2(0x80, 0x08)
#define OP_BEACON_GET                               BLE_MESH_MODEL_OP_2(0x80, 0x09)
#define OP_BEACON_SET                               BLE_MESH_MODEL_OP_2(0x80, 0x0A)
#define OP_BEACON_STATUS                            BLE_MESH_MODEL_OP_2(0x80, 0x0B)
#define OP_DEFAULT_TTL_GET                          BLE_MESH_MODEL_OP_2(0x80, 0x0C)
#define OP_DEFAULT_TTL_SET                          BLE_MESH_MODEL_OP_2(0x80, 0x0D)
#define OP_DEFAULT_TTL_STATUS                       BLE_MESH_MODEL_OP_2(0x80, 0x0E)
#define OP_FRIEND_GET                               BLE_MESH_MODEL_OP_2(0x80, 0x0F)
#define OP_FRIEND_SET                               BLE_MESH_MODEL_OP_2(0x80, 0x10)
#define OP_FRIEND_STATUS                            BLE_MESH_MODEL_OP_2(0x80, 0x11)
#define OP_GATT_PROXY_GET                           BLE_MESH_MODEL_OP_2(0x80, 0x12)
#define OP_GATT_PROXY_SET                           BLE_MESH_MODEL_OP_2(0x80, 0x13)
#define OP_GATT_PROXY_STATUS                        BLE_MESH_MODEL_OP_2(0x80, 0x14)
#define OP_KRP_GET                                  BLE_MESH_MODEL_OP_2(0x80, 0x15)
#define OP_KRP_SET                                  BLE_MESH_MODEL_OP_2(0x80, 0x16)
#define OP_KRP_STATUS                               BLE_MESH_MODEL_OP_2(0x80, 0x17)
#define OP_MOD_PUB_GET                              BLE_MESH_MODEL_OP_2(0x80, 0x18)
#define OP_MOD_PUB_STATUS                           BLE_MESH_MODEL_OP_2(0x80, 0x19)
#define OP_MOD_PUB_VA_SET                           BLE_MESH_MODEL_OP_2(0x80, 0x1A)
#define OP_MOD_SUB_ADD                              BLE_MESH_MODEL_OP_2(0x80, 0x1B)
#define OP_MOD_SUB_DEL                              BLE_MESH_MODEL_OP_2(0x80, 0x1C)
#define OP_MOD_SUB_DEL_ALL                          BLE_MESH_MODEL_OP_2(0x80, 0x1D)
#define OP_MOD_SUB_OVERWRITE                        BLE_MESH_MODEL_OP_2(0x80, 0x1E)
#define OP_MOD_SUB_STATUS                           BLE_MESH_MODEL_OP_2(0x80, 0x1F)
#define OP_MOD_SUB_VA_ADD                           BLE_MESH_MODEL_OP_2(0x80, 0x20)
#define OP_MOD_SUB_VA_DEL                           BLE_MESH_MODEL_OP_2(0x80, 0x21)
#define OP_MOD_SUB_VA_OVERWRITE                     BLE_MESH_MODEL_OP_2(0x80, 0x22)
#define OP_NET_TRANSMIT_GET                         BLE_MESH_MODEL_OP_2(0x80, 0x23)
#define OP_NET_TRANSMIT_SET                         BLE_MESH_MODEL_OP_2(0x80, 0x24)
#define OP_NET_TRANSMIT_STATUS                      BLE_MESH_MODEL_OP_2(0x80, 0x25)
#define OP_RELAY_GET                                BLE_MESH_MODEL_OP_2(0x80, 0x26)
#define OP_RELAY_SET                                BLE_MESH_MODEL_OP_2(0x80, 0x27)
#define OP_RELAY_STATUS                             BLE_MESH_MODEL_OP_2(0x80, 0x28)
#define OP_MOD_SUB_GET                              BLE_MESH_MODEL_OP_2(0x80, 0x29)
#define OP_MOD_SUB_LIST                             BLE_MESH_MODEL_OP_2(0x80, 0x2A)
#define OP_MOD_SUB_GET_VND                          BLE_MESH_MODEL_OP_2(0x80, 0x2B)
#define OP_MOD_SUB_LIST_VND                         BLE_MESH_MODEL_OP_2(0x80, 0x2C)
#define OP_LPN_TIMEOUT_GET                          BLE_MESH_MODEL_OP_2(0x80, 0x2D)
#define OP_LPN_TIMEOUT_STATUS                       BLE_MESH_MODEL_OP_2(0x80, 0x2E)
#define OP_HEALTH_FAULT_CLEAR                       BLE_MESH_MODEL_OP_2(0x80, 0x2F)
#define OP_HEALTH_FAULT_CLEAR_UNREL                 BLE_MESH_MODEL_OP_2(0x80, 0x30)
#define OP_HEALTH_FAULT_GET                         BLE_MESH_MODEL_OP_2(0x80, 0x31)
#define OP_HEALTH_FAULT_TEST                        BLE_MESH_MODEL_OP_2(0x80, 0x32)
#define OP_HEALTH_FAULT_TEST_UNREL                  BLE_MESH_MODEL_OP_2(0x80, 0x33)
#define OP_HEALTH_PERIOD_GET                        BLE_MESH_MODEL_OP_2(0x80, 0x34)
#define OP_HEALTH_PERIOD_SET                        BLE_MESH_MODEL_OP_2(0x80, 0x35)
#define OP_HEALTH_PERIOD_SET_UNREL                  BLE_MESH_MODEL_OP_2(0x80, 0x36)
#define OP_HEALTH_PERIOD_STATUS                     BLE_MESH_MODEL_OP_2(0x80, 0x37)
#define OP_HEARTBEAT_PUB_GET                        BLE_MESH_MODEL_OP_2(0x80, 0x38)
#define OP_HEARTBEAT_PUB_SET                        BLE_MESH_MODEL_OP_2(0x80, 0x39)
#define OP_HEARTBEAT_SUB_GET                        BLE_MESH_MODEL_OP_2(0x80, 0x3A)
#define OP_HEARTBEAT_SUB_SET                        BLE_MESH_MODEL_OP_2(0x80, 0x3B)
#define OP_HEARTBEAT_SUB_STATUS                     BLE_MESH_MODEL_OP_2(0x80, 0x3C)
#define OP_MOD_APP_BIND                             BLE_MESH_MODEL_OP_2(0x80, 0x3D)
#define OP_MOD_APP_STATUS                           BLE_MESH_MODEL_OP_2(0x80, 0x3E)
#define OP_MOD_APP_UNBIND                           BLE_MESH_MODEL_OP_2(0x80, 0x3F)
#define OP_NET_KEY_ADD                              BLE_MESH_MODEL_OP_2(0x80, 0x40)
#define OP_NET_KEY_DEL                              BLE_MESH_MODEL_OP_2(0x80, 0x41)
#define OP_NET_KEY_GET                              BLE_MESH_MODEL_OP_2(0x80, 0x42)
#define OP_NET_KEY_LIST                             BLE_MESH_MODEL_OP_2(0x80, 0x43)
#define OP_NET_KEY_STATUS                           BLE_MESH_MODEL_OP_2(0x80, 0x44)
#define OP_NET_KEY_UPDATE                           BLE_MESH_MODEL_OP_2(0x80, 0x45)
#define OP_NODE_IDENTITY_GET                        BLE_MESH_MODEL_OP_2(0x80, 0x46)
#define OP_NODE_IDENTITY_SET                        BLE_MESH_MODEL_OP_2(0x80, 0x47)
#define OP_NODE_IDENTITY_STATUS                     BLE_MESH_MODEL_OP_2(0x80, 0x48)
#define OP_NODE_RESET                               BLE_MESH_MODEL_OP_2(0x80, 0x49)
#define OP_NODE_RESET_STATUS                        BLE_MESH_MODEL_OP_2(0x80, 0x4A)
#define OP_SIG_MOD_APP_GET                          BLE_MESH_MODEL_OP_2(0x80, 0x4B)
#define OP_SIG_MOD_APP_LIST                         BLE_MESH_MODEL_OP_2(0x80, 0x4C)
#define OP_VND_MOD_APP_GET                          BLE_MESH_MODEL_OP_2(0x80, 0x4D)
#define OP_VND_MOD_APP_LIST                         BLE_MESH_MODEL_OP_2(0x80, 0x4E)

#define OP_REMOTE_PROV_SCAN_CAPS_GET                BLE_MESH_MODEL_OP_2(0x80, 0x4F)
#define OP_REMOTE_PROV_SCAN_CAPS_STATUS             BLE_MESH_MODEL_OP_2(0x80, 0x50)
#define OP_REMOTE_PROV_SCAN_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x51)
#define OP_REMOTE_PROV_SCAN_START                   BLE_MESH_MODEL_OP_2(0x80, 0x52)
#define OP_REMOTE_PROV_SCAN_STOP                    BLE_MESH_MODEL_OP_2(0x80, 0x53)
#define OP_REMOTE_PROV_SCAN_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x54)
#define OP_REMOTE_PROV_SCAN_REPORT                  BLE_MESH_MODEL_OP_2(0x80, 0x55)
#define OP_REMOTE_PROV_EXTENDED_SCAN_START          BLE_MESH_MODEL_OP_2(0x80, 0x56)
#define OP_REMOTE_PROV_EXTENDED_SCAN_REPORT         BLE_MESH_MODEL_OP_2(0x80, 0x57)
#define OP_REMOTE_PROV_LINK_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x58)
#define OP_REMOTE_PROV_LINK_OPEN                    BLE_MESH_MODEL_OP_2(0x80, 0x59)
#define OP_REMOTE_PROV_LINK_CLOSE                   BLE_MESH_MODEL_OP_2(0x80, 0x5A)
#define OP_REMOTE_PROV_LINK_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x5B)
#define OP_REMOTE_PROV_LINK_REPORT                  BLE_MESH_MODEL_OP_2(0x80, 0x5C)
#define OP_REMOTE_PROV_PDU_SEND                     BLE_MESH_MODEL_OP_2(0x80, 0x5D)
#define OP_REMOTE_PROV_PDU_OUTBOUND_REPORT          BLE_MESH_MODEL_OP_2(0x80, 0x5E)
#define OP_REMOTE_PROV_PDU_REPORT                   BLE_MESH_MODEL_OP_2(0x80, 0x5F)

#define OP_DIRECTED_CONTROL_GET                     BLE_MESH_MODEL_OP_2(0x80, 0x7B)
#define OP_DIRECTED_CONTROL_SET                     BLE_MESH_MODEL_OP_2(0x80, 0x7C)
#define OP_DIRECTED_CONTROL_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x7D)
#define OP_PATH_METRIC_GET                          BLE_MESH_MODEL_OP_2(0x80, 0x7E)
#define OP_PATH_METRIC_SET                          BLE_MESH_MODEL_OP_2(0x80, 0x7F)
#define OP_PATH_METRIC_STATUS                       BLE_MESH_MODEL_OP_2(0x80, 0x80)
#define OP_DISCOVERY_TABLE_CAPS_GET                 BLE_MESH_MODEL_OP_2(0x80, 0x81)
#define OP_DISCOVERY_TABLE_CAPS_SET                 BLE_MESH_MODEL_OP_2(0x80, 0x82)
#define OP_DISCOVERY_TABLE_CAPS_STATUS              BLE_MESH_MODEL_OP_2(0x80, 0x83)
#define OP_FORWARDING_TABLE_ADD                     BLE_MESH_MODEL_OP_2(0x80, 0x84)
#define OP_FORWARDING_TABLE_DEL                     BLE_MESH_MODEL_OP_2(0x80, 0x85)
#define OP_FORWARDING_TABLE_STATUS                  BLE_MESH_MODEL_OP_2(0x80, 0x86)
#define OP_FORWARDING_TABLE_DEPS_ADD                BLE_MESH_MODEL_OP_2(0x80, 0x87)
#define OP_FORWARDING_TABLE_DEPS_DEL                BLE_MESH_MODEL_OP_2(0x80, 0x88)
#define OP_FORWARDING_TABLE_DEPS_STATUS             BLE_MESH_MODEL_OP_2(0x80, 0x89)
#define OP_FORWARDING_TABLE_DEPS_GET                BLE_MESH_MODEL_OP_2(0x80, 0x8A)
#define OP_FORWARDING_TABLE_DEPS_GET_STATUS         BLE_MESH_MODEL_OP_2(0x80, 0x8B)
#define OP_FORWARDING_TABLE_ENTRIES_CNT_GET         BLE_MESH_MODEL_OP_2(0x80, 0x8C)
#define OP_FORWARDING_TABLE_ENTRIES_CNT_STATUS      BLE_MESH_MODEL_OP_2(0x80, 0x8D)
#define OP_FORWARDING_TABLE_ENTRIES_GET             BLE_MESH_MODEL_OP_2(0x80, 0x8E)
#define OP_FORWARDING_TABLE_ENTRIES_STATUS          BLE_MESH_MODEL_OP_2(0x80, 0x8F)
#define OP_WANTED_LANES_GET                         BLE_MESH_MODEL_OP_2(0x80, 0x90)
#define OP_WANTED_LANES_SET                         BLE_MESH_MODEL_OP_2(0x80, 0x91)
#define OP_WANTED_LANES_STATUS                      BLE_MESH_MODEL_OP_2(0x80, 0x92)
#define OP_TWO_WAY_PATH_GET                         BLE_MESH_MODEL_OP_2(0x80, 0x93)
#define OP_TWO_WAY_PATH_SET                         BLE_MESH_MODEL_OP_2(0x80, 0x94)
#define OP_TWO_WAY_PATH_STATUS                      BLE_MESH_MODEL_OP_2(0x80, 0x95)
#define OP_PATH_ECHO_INTERVAL_GET                   BLE_MESH_MODEL_OP_2(0x80, 0x96)
#define OP_PATH_ECHO_INTERVAL_SET                   BLE_MESH_MODEL_OP_2(0x80, 0x97)
#define OP_PATH_ECHO_INTERVAL_STATUS                BLE_MESH_MODEL_OP_2(0x80, 0x98)
#define OP_DIRECTED_NET_TRANSMIT_GET                BLE_MESH_MODEL_OP_2(0x80, 0x99)
#define OP_DIRECTED_NET_TRANSMIT_SET                BLE_MESH_MODEL_OP_2(0x80, 0x9A)
#define OP_DIRECTED_NET_TRANSMIT_STATUS             BLE_MESH_MODEL_OP_2(0x80, 0x9B)
#define OP_DIRECTED_RELAY_RETRANSMIT_GET            BLE_MESH_MODEL_OP_2(0x80, 0x9C)
#define OP_DIRECTED_RELAY_RETRANSMIT_SET            BLE_MESH_MODEL_OP_2(0x80, 0x9D)
#define OP_DIRECTED_RELAY_RETRANSMIT_STATUS         BLE_MESH_MODEL_OP_2(0x80, 0x9E)
#define OP_RSSI_THRESHOLD_GET                       BLE_MESH_MODEL_OP_2(0x80, 0x9F)
#define OP_RSSI_THRESHOLD_SET                       BLE_MESH_MODEL_OP_2(0x80, 0xA0)
#define OP_RSSI_THRESHOLD_STATUS                    BLE_MESH_MODEL_OP_2(0x80, 0xA1)
#define OP_DIRECTED_PATHS_GET                       BLE_MESH_MODEL_OP_2(0x80, 0xA2)
#define OP_DIRECTED_PATHS_STATUS                    BLE_MESH_MODEL_OP_2(0x80, 0xA3)
#define OP_DIRECTED_PUB_POLICY_GET                  BLE_MESH_MODEL_OP_2(0x80, 0xA4)
#define OP_DIRECTED_PUB_POLICY_SET                  BLE_MESH_MODEL_OP_2(0x80, 0xA5)
#define OP_DIRECTED_PUB_POLICY_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0xA6)
#define OP_PATH_DISCOVERY_TIMING_CTL_GET            BLE_MESH_MODEL_OP_2(0x80, 0xA7)
#define OP_PATH_DISCOVERY_TIMING_CTL_SET            BLE_MESH_MODEL_OP_2(0x80, 0xA8)
#define OP_PATH_DISCOVERY_TIMING_CTL_STATUS         BLE_MESH_MODEL_OP_2(0x80, 0xA9)
#define OP_DIRECTED_CTL_NET_TRANSMIT_GET            BLE_MESH_MODEL_OP_2(0x80, 0xAB)
#define OP_DIRECTED_CTL_NET_TRANSMIT_SET            BLE_MESH_MODEL_OP_2(0x80, 0xAC)
#define OP_DIRECTED_CTL_NET_TRANSMIT_STATUS         BLE_MESH_MODEL_OP_2(0x80, 0xAD)
#define OP_DIRECTED_CTL_RELAY_RETRANSMIT_GET        BLE_MESH_MODEL_OP_2(0x80, 0xAE)
#define OP_DIRECTED_CTL_RELAY_RETRANSMIT_SET        BLE_MESH_MODEL_OP_2(0x80, 0xAF)
#define OP_DIRECTED_CTL_RELAY_RETRANSMIT_STATUS     BLE_MESH_MODEL_OP_2(0x80, 0xB0)

#define OP_SUBNET_BRIDGE_GET                        BLE_MESH_MODEL_OP_2(0x80, 0xB1)
#define OP_SUBNET_BRIDGE_SET                        BLE_MESH_MODEL_OP_2(0x80, 0xB2)
#define OP_SUBNET_BRIDGE_STATUS                     BLE_MESH_MODEL_OP_2(0x80, 0xB3)
#define OP_BRIDGING_TABLE_ADD                       BLE_MESH_MODEL_OP_2(0x80, 0xB4)
#define OP_BRIDGING_TABLE_REMOVE                    BLE_MESH_MODEL_OP_2(0x80, 0xB5)
#define OP_BRIDGING_TABLE_STATUS                    BLE_MESH_MODEL_OP_2(0x80, 0xB6)
#define OP_BRIDGED_SUBNETS_GET                      BLE_MESH_MODEL_OP_2(0x80, 0xB7)
#define OP_BRIDGED_SUBNETS_LIST                     BLE_MESH_MODEL_OP_2(0x80, 0xB8)
#define OP_BRIDGING_TABLE_GET                       BLE_MESH_MODEL_OP_2(0x80, 0xB9)
#define OP_BRIDGING_TABLE_LIST                      BLE_MESH_MODEL_OP_2(0x80, 0xBA)
#define OP_BRIDGING_TABLE_SIZE_GET                  BLE_MESH_MODEL_OP_2(0x80, 0xBB)
#define OP_BRIDGING_TABLE_SIZE_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0xBC)

#define OP_PRIVATE_BEACON_GET                       BLE_MESH_MODEL_OP_2(0x80, 0x60)
#define OP_PRIVATE_BEACON_SET                       BLE_MESH_MODEL_OP_2(0x80, 0x61)
#define OP_PRIVATE_BEACON_STATUS                    BLE_MESH_MODEL_OP_2(0x80, 0x62)
#define OP_PRIVATE_GATT_PROXY_GET                   BLE_MESH_MODEL_OP_2(0x80, 0x63)
#define OP_PRIVATE_GATT_PROXY_SET                   BLE_MESH_MODEL_OP_2(0x80, 0x64)
#define OP_PRIVATE_GATT_PROXY_STATUS                BLE_MESH_MODEL_OP_2(0x80, 0x65)
#define OP_PRIVATE_NODE_IDENTITY_GET                BLE_MESH_MODEL_OP_2(0x80, 0x66)
#define OP_PRIVATE_NODE_IDENTITY_SET                BLE_MESH_MODEL_OP_2(0x80, 0x67)
#define OP_PRIVATE_NODE_IDENTITY_STATUS             BLE_MESH_MODEL_OP_2(0x80, 0x68)

#define OP_ON_DEMAND_PRIVATE_PROXY_GET              BLE_MESH_MODEL_OP_2(0x80, 0x69)
#define OP_ON_DEMAND_PRIVATE_PROXY_SET              BLE_MESH_MODEL_OP_2(0x80, 0x6A)
#define OP_ON_DEMAND_PRIVATE_PROXY_STATUS           BLE_MESH_MODEL_OP_2(0x80, 0x6B)

#define OP_SAR_TRANSMITTER_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x6C)
#define OP_SAR_TRANSMITTER_SET                      BLE_MESH_MODEL_OP_2(0x80, 0x6D)
#define OP_SAR_TRANSMITTER_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x6E)
#define OP_SAR_RECEIVER_GET                         BLE_MESH_MODEL_OP_2(0x80, 0x6F)
#define OP_SAR_RECEIVER_SET                         BLE_MESH_MODEL_OP_2(0x80, 0x70)
#define OP_SAR_RECEIVER_STATUS                      BLE_MESH_MODEL_OP_2(0x80, 0x71)

#define OP_OPCODES_AGG_SEQUENCE                     BLE_MESH_MODEL_OP_2(0x80, 0x72)
#define OP_OPCODES_AGG_STATUS                       BLE_MESH_MODEL_OP_2(0x80, 0x73)

#define OP_LARGE_COMP_DATA_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x74)
#define OP_LARGE_COMP_DATA_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x75)
#define OP_MODELS_METADATA_GET                      BLE_MESH_MODEL_OP_2(0x80, 0x76)
#define OP_MODELS_METADATA_STATUS                   BLE_MESH_MODEL_OP_2(0x80, 0x77)

#define OP_SOLIC_PDU_RPL_ITEMS_CLEAR                BLE_MESH_MODEL_OP_2(0x80, 0x78)
#define OP_SOLIC_PDU_RPL_ITEMS_CLEAR_UNACK          BLE_MESH_MODEL_OP_2(0x80, 0x79)
#define OP_SOLIC_PDU_RPL_ITEMS_STATUS               BLE_MESH_MODEL_OP_2(0x80, 0x7A)

#define STATUS_SUCCESS                              0x00
#define STATUS_INVALID_ADDRESS                      0x01
#define STATUS_INVALID_MODEL                        0x02
#define STATUS_INVALID_APPKEY                       0x03
#define STATUS_INVALID_NETKEY                       0x04
#define STATUS_INSUFF_RESOURCES                     0x05
#define STATUS_IDX_ALREADY_STORED                   0x06
#define STATUS_NVAL_PUB_PARAM                       0x07
#define STATUS_NOT_SUB_MOD                          0x08
#define STATUS_STORAGE_FAIL                         0x09
#define STATUS_FEAT_NOT_SUPP                        0x0A
#define STATUS_CANNOT_UPDATE                        0x0B
#define STATUS_CANNOT_REMOVE                        0x0C
#define STATUS_CANNOT_BIND                          0x0D
#define STATUS_TEMP_STATE_CHG_FAIL                  0x0E
#define STATUS_CANNOT_SET                           0x0F
#define STATUS_UNSPECIFIED                          0x10
#define STATUS_INVALID_BINDING                      0x11
#define STATUS_INVALID_PATH_ENTRY                   0x12
#define STATUS_CANNOT_GET                           0x13
#define STATUS_OBSOLETE_INFO                        0x14
#define STATUS_INVALID_BEARER                       0x15

/* Defines the status codes for Opcodes Aggregator messages. */
#define AGG_STATUS_SUCCESS                          0x00
#define AGG_STATUS_INVALID_ADDRESS                  0x01
#define AGG_STATUS_WRONG_ACCESS_KEY                 0x02
#define AGG_STATUS_WRONG_OPCODE                     0x03
#define AGG_STATUS_MSG_NOT_UNDERSTOOD               0x04
#define AGG_STATUS_RESPONSE_OVERFLOW                0x05

enum {
    BLE_MESH_VA_CHANGED,    /* Label information changed */
};

struct label {
    uint16_t ref;
    uint16_t addr;
    uint8_t  uuid[16];
    bt_mesh_atomic_t flags[1];
};

void bt_mesh_mod_sub_reset(bool store);

void bt_mesh_cfg_reset(bool store);

void bt_mesh_attention(struct bt_mesh_model *model, uint8_t time);

struct label *get_label(uint16_t index);

uint8_t *bt_mesh_label_uuid_get(uint16_t addr);

struct bt_mesh_hb_pub *bt_mesh_hb_pub_get(void);
void bt_mesh_hb_pub_disable(void);
struct bt_mesh_cfg_srv *bt_mesh_cfg_get(void);

uint8_t bt_mesh_net_transmit_get(void);
uint8_t bt_mesh_relay_get(void);
uint8_t bt_mesh_friend_get(void);
uint8_t bt_mesh_relay_retransmit_get(void);
uint8_t bt_mesh_secure_beacon_get(void);
uint8_t bt_mesh_gatt_proxy_get(void);
uint8_t bt_mesh_default_ttl_get(void);

void bt_mesh_subnet_del(struct bt_mesh_subnet *sub, bool store);

struct bt_mesh_app_key *bt_mesh_app_key_alloc(uint16_t app_idx);
void bt_mesh_app_key_del(struct bt_mesh_app_key *key, bool store);

static inline void key_idx_pack(struct net_buf_simple *buf,
                                uint16_t idx1, uint16_t idx2)
{
    net_buf_simple_add_le16(buf, idx1 | ((idx2 & 0x00f) << 12));
    net_buf_simple_add_u8(buf, idx2 >> 4);
}

static inline void key_idx_unpack(struct net_buf_simple *buf,
                                  uint16_t *idx1, uint16_t *idx2)
{
    *idx1 = sys_get_le16(&buf->data[0]) & 0xfff;
    *idx2 = sys_get_le16(&buf->data[1]) >> 4;
    net_buf_simple_pull(buf, 3);
}

#ifdef __cplusplus
}
#endif

#endif /* _FOUNDATION_H_ */
