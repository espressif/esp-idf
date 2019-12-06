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

#ifndef _PROVISIONER_PROV_H_
#define _PROVISIONER_PROV_H_

#include "mesh_bearer_adapt.h"
#include "mesh_main.h"

#if !CONFIG_BLE_MESH_PROVISIONER

#define CONFIG_BLE_MESH_PBA_SAME_TIME   0
#define CONFIG_BLE_MESH_PBG_SAME_TIME   0

#else

#if !defined(CONFIG_BLE_MESH_PB_ADV)
#define CONFIG_BLE_MESH_PBA_SAME_TIME   0
#endif /* !CONFIG_BLE_MESH_PB_ADV */

#if !defined(CONFIG_BLE_MESH_PB_GATT)
#define CONFIG_BLE_MESH_PBG_SAME_TIME   0
#endif /* !CONFIG_BLE_MESH_PB_GATT */

#endif /* !CONFIG_BLE_MESH_PROVISIONER */

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
 * @brief This function decrements the current PB-GATT count.
 *
 * @return None
 */
void provisioner_pbg_count_dec(void);

/**
 * @brief This function clears the part of the link info of the proper device.
 *
 * @param[in] addr: Remote device address
 *
 * @return None
 */
void provisioner_clear_link_conn_info(const u8_t addr[6]);

/**
 * @brief This function handles the received PB-ADV PDUs.
 *
 * @param[in] buf: Pointer to the buffer containing generic provisioning PDUs
 *
 * @return Zero - success, otherwise - fail
 */
void provisioner_pb_adv_recv(struct net_buf_simple *buf);

/**
 * @brief This function sends provisioning invite to start
 *        provisioning this unprovisioned device.
 *
 * @param[in] addr: Remote device address
 * @param[in] conn: Pointer to the bt_conn structure
 *
 * @return Zero - success, otherwise - fail
 */
int provisioner_set_prov_conn(const u8_t addr[6], struct bt_mesh_conn *conn);

/**
 * @brief This function sends provisioning invite to start
 *        provisioning this unprovisioned device.
 *
 * @param[in] conn: Pointer to the bt_conn structure
 * @param[in] addr: Address of the connected device
 *
 * @return Zero - success, otherwise - fail
 */
int provisioner_pb_gatt_open(struct bt_mesh_conn *conn, u8_t *addr);

/**
 * @brief This function resets the used information when
 *        related connection is terminated.
 *
 * @param[in] conn:   Pointer to the bt_conn structure
 * @param[in] reason: Connection terminated reason
 *
 * @return Zero - success, otherwise - fail
 */
int provisioner_pb_gatt_close(struct bt_mesh_conn *conn, u8_t reason);

/**
 * @brief This function handles the received PB-GATT provision
 *        PDUs.
 *
 * @param[in] conn: Pointer to the bt_conn structure
 * @param[in] buf:  Pointer to the buffer containing provision PDUs
 *
 * @return Zero - success, otherwise - fail
 */
int provisioner_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf);

/**
 * @brief This function initializes provisioner's PB-GATT and PB-ADV
 *        related information.
 *
 * @param[in] prov_info: Pointer to the application-initialized provisioner info.
 *
 * @return Zero - success, otherwise - fail
 */
int provisioner_prov_init(const struct bt_mesh_prov *prov_info);

/**
 * @brief This function parses the received unprovisioned device
 *        beacon advertising packets, and if checked, starts to provision this device
 *        using PB-ADV bearer.
 *
 * @param[in] buf: Pointer to the buffer containing unprovisioned device beacon
 *
 * @return None
 */
void provisioner_unprov_beacon_recv(struct net_buf_simple *buf);

void provisioner_prov_adv_ind_recv(struct net_buf_simple *buf, const bt_mesh_addr_t *addr);

/**
 * @brief This function gets the bt_mesh_prov pointer.
 *
 * @return bt_mesh_prov pointer(prov)
 */
const struct bt_mesh_prov *provisioner_get_prov_info(void);

/**
 * @brief This function resets all nodes information in provisioner_prov.c.
 *
 * @return Zero
 */
int provisioner_prov_reset_all_nodes(void);

/* The following APIs are for primary provisioner application use */

/** @brief Add unprovisioned device info to unprov_dev queue
 *
 *  @param[in] add_dev: Pointer to the structure containing the device information
 *  @param[in] flags:   Flags indicate several operations of the device information
 *                       - Remove device information from queue after it is provisioned (BIT0)
 *                       - Start provisioning as soon as device is added to queue (BIT1)
 *                       - Device can be flushed when device queue is full (BIT2)
 *
 *  @return Zero on success or (negative) error code otherwise.
 *
 *  @Note: 1. Currently address type only supports public address and static random address.
 *         2. If device UUID and/or device address and address type already exist in the
 *            device queue, but the bearer differs from the existing one, add operation
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
 * @brief This function sets a part of the device UUID for comparison before
 *        starting to provision the device.
 *
 * @param[in] offset: offset of the device UUID to be compared
 * @param[in] length: length of the device UUID to be compared
 * @param[in] match:  value to be compared
 * @param[in] prov_flag: flags indicate if uuid_match advertising packets are received, after that
 *                       the device will be provisioned at once or reported to the application layer
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_set_dev_uuid_match(u8_t offset, u8_t length,
        const u8_t *match, bool prov_flag);

/** @brief Callback for provisioner receiving advertising packet from unprovisioned devices which are
 *  not in the unprovisioned device queue.
 *
 *  Report on the unprovisioned device beacon and mesh provisioning service advertising data to application layer
 *
 *  @param addr      Unprovisioned device address pointer
 *  @param addr_type Unprovisioned device address type
 *  @param dev_uuid  Unprovisioned device device UUID pointer
 *  @param bearer    Advertising packet received from PB-GATT or PB-ADV bearer
 *  @param adv_type  Adv packet type, currently this is not used and we can use bearer to device
 *                   the adv_type(ADV_IND or ADV_NONCONN_IND). This parameter will be used, when
 *                   scan response data will be supported.
 *
 */
typedef void (*unprov_adv_pkt_cb_t)(const u8_t addr[6], const u8_t addr_type,
                                    const u8_t adv_type, const u8_t dev_uuid[16],
                                    u16_t oob_info, bt_mesh_prov_bearer_t bearer);

/**
 * @brief This function registers the callback which notifies the application
 *        layer of the received mesh provisioning or unprovisioned device
 *        beacon advertizing packets (from devices not in the unprov device queue).
 *
 * @param[in] cb: Callback of the notifying adv pkts function
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_prov_adv_pkt_cb_register(unprov_adv_pkt_cb_t cb);

/**
 * @brief This function changes net_idx or flags or iv_index used in provisioning data.
 *
 * @param[in] info: Pointer of structure containing net_idx or flags or iv_index
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_set_prov_data_info(struct bt_mesh_prov_data_info *info);

/**
 * @brief This function is called to input number/string out-put by unprovisioned device.
 *
 * @param[in] idx       The provisioning link index
 * @param[in] val       Pointer of the input number/string
 * @param[in] num_flag  Flag indicates if it is a number or string
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_prov_set_oob_input_data(const u8_t idx, const u8_t *val, bool num_flag);

/**
 * @brief This function is called to output number/string which will be input by unprovisioned device.
 *
 * @param[in] idx       The provisioning link index
 * @param[in] num       Pointer of the output number/string
 * @param[in] size      Size of the output number/string
 * @param[in] num_flag  Flag indicates if it is a number or string
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_prov_set_oob_output_data(const u8_t idx, const u8_t *num, u8_t size, bool num_flag);

/**
 * @brief This function is called to read unprovisioned device's oob public key.
 *
 * @param[in] idx        The provisioning link index
 * @param[in] pub_key_x  Unprovisioned device's Public Key X
 * @param[in] pub_key_y  Unprovisioned device's Public Key Y
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_prov_read_oob_pub_key(const u8_t idx, const u8_t pub_key_x[32], const u8_t pub_key_y[32]);

/* The following APIs are for fast provisioning */

/**
 * @brief This function is called to set fast_prov_flag.
 *
 * @param[in] flag: Flag set to fast_prov_flag
 *
 * @return None
 */
void provisioner_set_fast_prov_flag(bool flag);

/**
 * @brief This function is called to set netkey index used for fast provisioning.
 *
 * @param[in] net_key: Netkey value
 * @param[in] net_idx: Netkey index
 *
 * @return status for set netkey index msg
 */
u8_t provisioner_set_fast_prov_net_idx(const u8_t *net_key, u16_t net_idx);

/**
 * @brief This function is called to get netkey index used for fast provisioning.
 *
 * @return net_idx of fast provisioning
 */
u16_t provisioner_get_fast_prov_net_idx(void);

/**
 * @brief This function is called to set unicast address range used for fast provisioning.
 *
 * @param[in] min: Minimum unicast address
 * @param[in] max: Maximum unicast address
 *
 * @return status for set unicast address range message
 */
u8_t bt_mesh_set_fast_prov_unicast_addr_range(u16_t min, u16_t max);

/**
 * @brief This function is called to set flags & iv_index used for fast provisioning.
 *
 * @param[in] flags:    Key refresh flag and iv update flag
 * @param[in] iv_index: IV index
 *
 * @return None
 */
void bt_mesh_set_fast_prov_flags_iv_index(u8_t flags, u32_t iv_index);

#endif /* _PROVISIONER_PROV_H_ */
