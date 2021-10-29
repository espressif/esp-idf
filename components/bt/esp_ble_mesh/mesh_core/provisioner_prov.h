/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROVISIONER_PROV_H_
#define _PROVISIONER_PROV_H_

#include "mesh_main.h"
#include "mesh_bearer_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BLE_MESH_PBA_SAME_TIME
#define CONFIG_BLE_MESH_PBA_SAME_TIME   0
#endif

#ifndef CONFIG_BLE_MESH_PBG_SAME_TIME
#define CONFIG_BLE_MESH_PBG_SAME_TIME   0
#endif

#define RM_AFTER_PROV  BIT(0)
#define START_PROV_NOW BIT(1)
#define FLUSHABLE_DEV  BIT(2)

struct bt_mesh_unprov_dev_add {
    uint8_t  addr[6];
    uint8_t  addr_type;
    uint8_t  uuid[16];
    uint16_t oob_info;
    uint8_t  bearer;
};

struct bt_mesh_device_delete {
    uint8_t addr[6];
    uint8_t addr_type;
    uint8_t uuid[16];
};

#define NET_IDX_FLAG  BIT(0)
#define FLAGS_FLAG    BIT(1)
#define IV_INDEX_FLAG BIT(2)

struct bt_mesh_prov_data_info {
    union {
        uint16_t net_idx;
        uint8_t  flags;
        uint32_t iv_index;
    };
    uint8_t flag;
};

/* The following APIs are for primary provisioner internal use */

/**
 * @brief This function decrements the current PB-GATT count.
 *
 * @return None
 */
void bt_mesh_provisioner_pbg_count_dec(void);

/**
 * @brief This function clears the part of the link info of the proper device.
 *
 * @param[in] addr: Remote device address
 *
 * @return None
 */
void bt_mesh_provisioner_clear_link_info(const uint8_t addr[6]);

/**
 * @brief This function handles the received PB-ADV PDUs.
 *
 * @param[in] buf: Pointer to the buffer containing generic provisioning PDUs
 *
 * @return Zero - success, otherwise - fail
 */
void bt_mesh_provisioner_pb_adv_recv(struct net_buf_simple *buf);

/**
 * @brief This function sends provisioning invite to start
 *        provisioning this unprovisioned device.
 *
 * @param[in] addr: Remote device address
 * @param[in] conn: Pointer to the bt_conn structure
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_set_prov_conn(const uint8_t addr[6], struct bt_mesh_conn *conn);

/**
 * @brief This function sends provisioning invite to start
 *        provisioning this unprovisioned device.
 *
 * @param[in] conn: Pointer to the bt_conn structure
 * @param[in] addr: Address of the connected device
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_pb_gatt_open(struct bt_mesh_conn *conn, uint8_t *addr);

/**
 * @brief This function resets the used information when
 *        related connection is terminated.
 *
 * @param[in] conn:   Pointer to the bt_conn structure
 * @param[in] reason: Connection terminated reason
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_pb_gatt_close(struct bt_mesh_conn *conn, uint8_t reason);

/**
 * @brief This function handles the received PB-GATT provision
 *        PDUs.
 *
 * @param[in] conn: Pointer to the bt_conn structure
 * @param[in] buf:  Pointer to the buffer containing provision PDUs
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_pb_gatt_recv(struct bt_mesh_conn *conn, struct net_buf_simple *buf);

/**
 * @brief This function initializes provisioner's PB-GATT and PB-ADV
 *        related information.
 *
 * @param[in] prov_info: Pointer to the application-initialized provisioner info.
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_prov_init(const struct bt_mesh_prov *prov_info);

int bt_mesh_provisioner_prov_reset(bool erase);

/**
 * @brief This function de-initializes provisioner's PB-GATT and PB-ADV
 *        related information.
 *
 * @param[in] erase: Indicate if erasing provisioning information from flash.
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_prov_deinit(bool erase);

/**
 * @brief This function parses the received unprovisioned device
 *        beacon advertising packets, and if checked, starts to provision this device
 *        using PB-ADV bearer.
 *
 * @param[in] buf:  Pointer to the buffer containing unprovisioned device beacon
 * @param[in] rssi: RSSI of the received unprovisioned device beacon
 *
 * @return None
 */
void bt_mesh_provisioner_unprov_beacon_recv(struct net_buf_simple *buf, int8_t rssi);

void bt_mesh_provisioner_prov_adv_recv(struct net_buf_simple *buf,
                                       const bt_mesh_addr_t *addr, int8_t rssi);

/**
 * @brief This function gets the bt_mesh_prov pointer.
 *
 * @return bt_mesh_prov pointer(prov)
 */
const struct bt_mesh_prov *bt_mesh_provisioner_get_prov_info(void);

void bt_mesh_provisioner_restore_prov_info(uint16_t primary_addr, uint16_t alloc_addr);

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
 *  @note  1. Currently address type only supports public address and static random address.
 *         2. If device UUID and/or device address and address type already exist in the
 *            device queue, but the bearer differs from the existing one, add operation
 *            will also be successful and it will update the provision bearer supported by
 *            the device.
 */
int bt_mesh_provisioner_add_unprov_dev(struct bt_mesh_unprov_dev_add *add_dev, uint8_t flags);

/** @brief Provision an unprovisioned device with fixed unicast address.
 *
 *  @param[in] uuid:         Device UUID of the unprovisioned device
 *  @param[in] addr:         Device address of the unprovisioned device
 *  @param[in] addr_type:    Device address type of the unprovisioned device
 *  @param[in] bearer:       Provisioning bearer going to be used
 *  @param[in] oob_info:     OOB info of the unprovisioned device
 *  @param[in] unicast_addr: Unicast address going to be allocated for the unprovisioned device
 *
 *  @return Zero on success or (negative) error code otherwise.
 *
 *  @note  1. Currently address type only supports public address and static random address.
 *         2. Bearer must be equal to BLE_MESH_PROV_ADV or BLE_MESH_PROV_GATT
 */
int bt_mesh_provisioner_prov_device_with_addr(const uint8_t uuid[16], const uint8_t addr[6],
                                              uint8_t addr_type, bt_mesh_prov_bearer_t bearer,
                                              uint16_t oob_info, uint16_t unicast_addr);

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
int bt_mesh_provisioner_set_dev_uuid_match(uint8_t offset, uint8_t length,
                                           const uint8_t *match, bool prov_flag);

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
 *  @param rssi      RSSI of the received advertising packet
 *
 */
typedef void (*unprov_adv_pkt_cb_t)(const uint8_t addr[6], const uint8_t addr_type,
                                    const uint8_t adv_type, const uint8_t dev_uuid[16],
                                    uint16_t oob_info, bt_mesh_prov_bearer_t bearer, int8_t rssi);

/**
 * @brief This function registers the callback which notifies the application
 *        layer of the received mesh provisioning or unprovisioned device
 *        beacon advertizing packets (from devices not in the unprov device queue).
 *
 * @param[in] cb: Callback of the notifying adv pkts function
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_adv_pkt_cb_register(unprov_adv_pkt_cb_t cb);

/**
 * @brief This function changes net_idx or flags or iv_index used in provisioning data.
 *
 * @param[in] info: Pointer of structure containing net_idx or flags or iv_index
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_set_prov_data_info(struct bt_mesh_prov_data_info *info);

/**
 * @brief This function initializes the provisioning information needed by Provisioner,
 *        including NetKey Index, flags, IV Index, etc.
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_init_prov_info(void);

/**
 * @brief This function sets the provisioning bearer type used by Provisioner.
 *
 * @param[in] bearers: Provisioning bearer type
 * @param[in] clear:   Indicate if the corresponding bearer type will be cleared
 *
 * @return None
 */
void bt_mesh_provisioner_set_prov_bearer(bt_mesh_prov_bearer_t bearers, bool clear);

/**
 * @brief This function gets the provisioning bearer type used by Provisioner.
 *
 * @return Currently supported provisioning bearer type
 */
bt_mesh_prov_bearer_t bt_mesh_provisioner_get_prov_bearer(void);

/**
 * @brief This function sets the Static OOB value used by Provisioner.
 *
 * @param[in] value:  Static OOB value
 * @param[in] length: Static OOB value length
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_set_static_oob_value(const uint8_t *value, uint8_t length);

/**
 * @brief This function gets the unicast address of primary element of Provisioner.
 *
 * @return Unicast address of primary element of Provisioner.
 */
uint16_t bt_mesh_provisioner_get_primary_elem_addr(void);

/**
 * @brief This function sets the unicast address of primary element of Provisioner.
 *
 * @param[in] addr: unicast address of primary element
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_set_primary_elem_addr(uint16_t addr);

/**
 * @brief This function is used to update next allocated address by Provisioner.
 *
 * @note  This function is used for mesh internal test.
 *
 * @param[in] unicast_addr: unicast address of the node
 * @param[in] element_num:  element count of the node
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_test_provisioner_update_alloc_addr(uint16_t unicast_addr, uint16_t element_num);

/**
 * @brief This function is called to input number/string out-put by unprovisioned device.
 *
 * @param[in] idx       The provisioning link index
 * @param[in] val       Pointer of the input number/string
 * @param[in] num_flag  Flag indicates if it is a number or string
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_set_oob_input_data(const uint8_t idx, const uint8_t *val, bool num_flag);

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
int bt_mesh_provisioner_set_oob_output_data(const uint8_t idx, const uint8_t *num,
                                            uint8_t size, bool num_flag);

/**
 * @brief This function is called to read unprovisioned device's oob public key.
 *
 * @param[in] idx        The provisioning link index
 * @param[in] pub_key_x  Unprovisioned device's Public Key X
 * @param[in] pub_key_y  Unprovisioned device's Public Key Y
 *
 * @return Zero - success, otherwise - fail
 */
int bt_mesh_provisioner_read_oob_pub_key(const uint8_t idx, const uint8_t pub_key_x[32],
                                         const uint8_t pub_key_y[32]);

/* The following APIs are for fast provisioning */

/**
 * @brief This function is called to set fast_prov_flag.
 *
 * @param[in] enable: Enable or disable fast provisioning
 *
 * @return None
 */
void bt_mesh_provisioner_fast_prov_enable(bool enable);

/**
 * @brief This function is called to set netkey index used for fast provisioning.
 *
 * @param[in] net_idx: Netkey index
 *
 * @return None
 */
void bt_mesh_provisioner_set_fast_prov_net_idx(uint16_t net_idx);

/**
 * @brief This function is called to get netkey index used for fast provisioning.
 *
 * @return net_idx of fast provisioning
 */
uint16_t bt_mesh_provisioner_get_fast_prov_net_idx(void);

/**
 * @brief This function is called to set unicast address range used for fast provisioning.
 *
 * @param[in] min: Minimum unicast address
 * @param[in] max: Maximum unicast address
 *
 * @return status for set unicast address range message
 */
uint8_t bt_mesh_set_fast_prov_unicast_addr_range(uint16_t min, uint16_t max);

/**
 * @brief This function is called to set flags & iv_index used for fast provisioning.
 *
 * @param[in] flags:    Key refresh flag and iv update flag
 * @param[in] iv_index: IV index
 *
 * @return None
 */
void bt_mesh_set_fast_prov_flags_iv_index(uint8_t flags, uint32_t iv_index);

#ifdef __cplusplus
}
#endif

#endif /* _PROVISIONER_PROV_H_ */
