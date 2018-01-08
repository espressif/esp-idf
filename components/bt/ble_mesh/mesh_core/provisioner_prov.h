// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _PROVISIONER_PROV_H_
#define _PROVISIONER_PROV_H_

#include "mesh_bearer_adapt.h"
#include "mesh_main.h"

#if !CONFIG_BT_MESH_PROVISIONER

#define CONFIG_BT_MESH_PBA_SAME_TIME 0
#define CONFIG_BT_MESH_PBG_SAME_TIME 0

#else

#if !defined(CONFIG_BT_MESH_PB_ADV)
#define CONFIG_BT_MESH_PBA_SAME_TIME 0
#endif /* !CONFIG_BT_MESH_PB_ADV */

#if !defined(CONFIG_BT_MESH_PB_GATT)
#define CONFIG_BT_MESH_PBG_SAME_TIME 0
#endif /* !CONFIG_BT_MESH_PB_GATT */

#define BT_DATA_FLAGS         0x01
#define BT_DATA_SERVICE_UUID  0x03
#define BT_DATA_SERVICE_DATA  0X16

#define RM_AFTER_PROV  BIT(0)
#define START_PROV_NOW BIT(1)
#define FLUSHABLE_DEV  BIT(2)

struct bt_mesh_unprov_dev_add {
    u8_t  addr[6];
    u8_t  addr_type;
    u8_t  uuid[16];
    u16_t oob_info;
    u8_t  bearer;
};

struct bt_mesh_device_delete {
    u8_t addr[6];
    u8_t addr_type;
    u8_t uuid[16];
};

#define NET_IDX_FLAG  BIT(0)
#define FLAGS_FLAG    BIT(1)
#define IV_INDEX_FLAG BIT(2)

struct bt_mesh_prov_data_info {
    union {
        u16_t net_idx;
        u8_t  flags;
        u32_t iv_index;
    };
    u8_t flag;
};

/* The following APIs are for primary provisioner internal use */

/**
 * @brief This function is called to decrement the current PB-GATT count.
 *
 * @return None
 */
void provisioner_pbg_count_dec(void);

/**
 * @brief This function is called to increment the current PB-GATT count.
 *
 * @return None
 */
void provisioner_pbg_count_inc(void);

/**
 * @brief This function is called to clear part of the link
 *        info of the proper device.
 *
 * @param[in] index: Index of link within the array.
 *
 * @return None
 */
void provisioner_clear_connecting(int index);

/**
 * @brief This function is called to deal with the received PB-ADV pdus.
 *
 * @param[in] buf: Pointer to the buffer containing generic provisioning pdus
 *
 * @return Zero-success, other-fail
 */
void provisioner_pb_adv_recv(struct net_buf_simple *buf);

/**
 * @brief This function is called to send provisioning invite to start
 *        provisioning this unprovisioned device.
 *
 * @param[in] conn:  Pointer to the bt_conn structure
 * @param[in] index: The 'index' link whose conn will be set
 *
 * @return Zero-success, other-fail
 */
int provisioner_set_prov_conn(struct bt_conn *conn, int index);

/**
 * @brief This function is called to send provisioning invite to start
 *        provisioning this unprovisioned device.
 *
 * @param[in] conn: Pointer to the bt_conn structure
 * @param[in] addr: Address of the connected device
 *
 * @return Zero-success, other-fail
 */
int provisioner_pb_gatt_open(struct bt_conn *conn, u8_t *addr);

/**
 * @brief This function is called to reset the used information when
 *        related connection is terminated.
 *
 * @param[in] conn:   Pointer to the bt_conn structure
 * @param[in] reason: Connection terminated reason
 *
 * @return Zero-success, other-fail
 */
int provisioner_pb_gatt_close(struct bt_conn *conn, u8_t reason);

/**
 * @brief This function is called to deal with the received PB-GATT provision
 *        pdus.
 *
 * @param[in] conn: Pointer to the bt_conn structure
 * @param[in] buf:  Pointer to the buffer containing provision pdus
 *
 * @return Zero-success, other-fail
 */
int provisioner_pb_gatt_recv(struct bt_conn *conn, struct net_buf_simple *buf);

/**
 * @brief This function is called to initialize provisioner's PB-GATT and PB-ADV
 *        related informations.
 *
 * @param[in] prov_info: Pointer to the application-initialized provisioner info.
 *
 * @return Zero-success, other-fail
 */
int provisioner_prov_init(const struct bt_mesh_prov *prov_info);

/**
 * @brief This function is called to parse the received unprovisioned device
 *        beacon adv pkts, and if checked, start to provision this device
 *        using PB-ADV bearer.
 *
 * @param[in] buf: Pointer to the buffer containing unprovisioned device beacon
 *
 * @return None
 */
void provisioner_unprov_beacon_recv(struct net_buf_simple *buf);

/**
 * @brief This function is called to parse the flags part of the
 *        received connectable mesh provisioning adv pkts.
 *
 * @param[in] buf: Pointer to the buffer containing adv flags part
 *
 * @return True-success, False-fail
 */
bool provisioner_flags_match(struct net_buf_simple *buf);

/**
 * @brief This function is called to parse the service uuid part of the
 *        received connectable mesh provisioning adv pkts.
 *
 * @param[in] buf: Pointer to the buffer containing service uuid part
 *
 * @return Zero-fail, other-Service UUID(0x1827 or 0x1828)
 */
u16_t provisioner_srv_uuid_recv(struct net_buf_simple *buf);

/**
 * @brief This function is called to parse the service data part of the
 *        received connectable mesh provisioning adv pkts.
 *
 * @param[in] buf:  Pointer to the buffer containing remianing service data part
 * @param[in] addr: Pointer to the received device address
 * @param[in] uuid: Service UUID contained in the service uuid part
 *
 * @return None
 */
void provisioner_srv_data_recv(struct net_buf_simple *buf, const bt_addr_le_t *addr, u16_t uuid);

/**
 * @brief This function is called to get bt_mesh_prov pointer.
 *
 * @return bt_mesh_prov pointer(prov)
 */
const struct bt_mesh_prov *provisioner_get_prov_info(void);

/**
 * @brief This function is called to reset all nodes information in provisioner_prov.c.
 *
 * @return Zero
 */
int provisioner_prov_reset_all_nodes(void);

/* The following APIs are for primary provisioner application use */

/** @brief Add unprovisioned device info to unprov_dev queue
 *
 *  @param[in] add_dev: Pointer to the structure containing the device information
 *  @param[in] flags:   Flags indicate several oprations of the device information
 *                       - Remove device information from queue after been provisioned (BIT0)
 *                       - Start provisioning at once when device is added to queue (BIT1)
 *                       - Device can be flushed when device queue is full (BIT2)
 *
 *  @return Zero on success or (negative) error code otherwise.
 *
 *  @Note: 1. Currently address type only supports public address and static random address.
 *         2. If device UUID and/or device address and address type already exist in the
 *            device queue, but the bearer is different with the existing one, add operation
 *            will also be successful and it will update the provision bearer supported by
 *            the device.
 */
int bt_mesh_provisioner_add_unprov_dev(struct bt_mesh_unprov_dev_add *add_dev, u8_t flags);

/** @brief Delete device from queue, reset current provisioning link and reset the node
 *
 *  @param[in] del_dev: Pointer to the structure containing the device information
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provisioner_delete_device(struct bt_mesh_device_delete *del_dev);

/**
 * @brief This function is called to set part of the device uuid to be compared before
 *        starting to provision the device.
 *
 * @param[in] offset: offset of the device uuid to be compared
 * @param[in] length: length of the device uuid to be compared
 * @param[in] match:  value to be compared
 * @param[in] prov_flag: flags indicate when received uuid_match adv pkts, the device
 *                       will be provisioned at once or report to application layer
 *
 * @return 0 - success, others - fail
 */
int bt_mesh_provisioner_set_dev_uuid_match(u8_t offset, u8_t length,
        const u8_t *match, bool prov_flag);

/** @brief Callback for provisioner received adv pkt from unprovisioned devices which are
 *  not in the unprovisioned device queue.
 *
 *  Notify the unprovisioned device beacon and mesh provisioning service adv data to application
 *
 *  @param addr      Unprovisioned device address pointer
 *  @param addr_type Unprovisioned device address type
 *  @param dev_uuid  Unprovisioned device device UUID pointer
 *  @param bearer    Adv packet received from PB-GATT or PB-ADV bearer
 *  @param adv_type  Adv packet type, currently this is not used and we can use bearer to device
 *                   the adv_type(ADV_IND or ADV_NONCONN_IND). Later when we support scan response
 *                   data, this parameter will be used.
 *
 */
typedef void (*prov_adv_pkt_cb)(const u8_t addr[6], const u8_t addr_type,
                                const u8_t adv_type, const u8_t dev_uuid[16],
                                u16_t oob_info, bt_mesh_prov_bearer_t bearer);

/**
 * @brief This function is called to register the callback which notifies the
 *        application layer of the received mesh provisioning or unprovisioned
 *        device beacon adv pkts(from devices not in the unprov device queue).
 *
 * @param[in] cb: Callback of the notifying adv pkts function
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_prov_adv_pkt_cb_register(prov_adv_pkt_cb cb);

/**
 * @brief This function is called to change net_idx or flags or iv_index used in provisioning data.
 *
 * @param[in] info: Pointer of structure containing net_idx or flags or iv_index
 *
 * @return 0 - success, others - fail
 */
int bt_mesh_provisioner_set_prov_data_info(struct bt_mesh_prov_data_info *info);

/* The following APIs are for temporary provisioner internal/application(bt_mesh_xxx) use */

/**
 * @brief This function is called to set temp_prov_flag.
 *
 * @param[in] flag: Flag set to temp_prov_flag
 *
 * @return None
 */
void provisioner_temp_prov_flag_set(bool flag);

/**
 * @brief This function is called to set unicast address range used for temp prov.
 *
 * @param[in] min: Minimum unicast address
 * @param[in] max: Maximum unicast address
 *
 * @return status for set unicast address range msg
 */
u8_t bt_mesh_temp_prov_set_unicast_addr(u16_t min, u16_t max);

/**
 * @brief This function is called to set flags & iv_index used for temp prov.
 *
 * @param[in] flags:    Key refresh flag and iv update flag
 * @param[in] iv_index: IV index
 *
 * @return 0 - success
 */
int bt_mesh_temp_prov_set_flags_iv_index(u8_t flags, u32_t iv_index);

/**
 * @brief This function is called to set netkey index used for temp prov.
 *
 * @param[in] net_key: Netkey pointer
 * @param[in] net_idx: Netkey index
 *
 * @return status for set netkey index msg
 */
u8_t provisioner_temp_prov_set_net_idx(const u8_t *net_key, u16_t net_idx);

#endif /* CONFIG_BT_MESH_PROVISIONER */

#endif /* #ifndef _PROVISIONER_PROV_H_ */
