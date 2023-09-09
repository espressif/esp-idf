/** @file
 *  @brief Bluetooth Mesh Profile APIs.
 */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_MAIN_H_
#define _BLE_MESH_MAIN_H_

#include "mesh/access.h"

/**
 * @brief Bluetooth Mesh Provisioning
 * @defgroup bt_mesh_prov Bluetooth Mesh Provisioning
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BLE_MESH_NO_OUTPUT       = 0,
    BLE_MESH_BLINK           = BIT(0),
    BLE_MESH_BEEP            = BIT(1),
    BLE_MESH_VIBRATE         = BIT(2),
    BLE_MESH_DISPLAY_NUMBER  = BIT(3),
    BLE_MESH_DISPLAY_STRING  = BIT(4),
} bt_mesh_output_action_t;

typedef enum {
    BLE_MESH_NO_INPUT      = 0,
    BLE_MESH_PUSH          = BIT(0),
    BLE_MESH_TWIST         = BIT(1),
    BLE_MESH_ENTER_NUMBER  = BIT(2),
    BLE_MESH_ENTER_STRING  = BIT(3),
} bt_mesh_input_action_t;

typedef enum {
    BLE_MESH_PROV_ADV   = BIT(0),
    BLE_MESH_PROV_GATT  = BIT(1),
} bt_mesh_prov_bearer_t;

typedef enum {
    BLE_MESH_PROV_OOB_OTHER     = BIT(0),
    BLE_MESH_PROV_OOB_URI       = BIT(1),
    BLE_MESH_PROV_OOB_2D_CODE   = BIT(2),
    BLE_MESH_PROV_OOB_BAR_CODE  = BIT(3),
    BLE_MESH_PROV_OOB_NFC       = BIT(4),
    BLE_MESH_PROV_OOB_NUMBER    = BIT(5),
    BLE_MESH_PROV_OOB_STRING    = BIT(6),
    BLE_MESH_PROV_CERT_BASED    = BIT(7),
    BLE_MESH_PROV_RECORDS       = BIT(8),
    /* 9 - 10 are reserved */
    BLE_MESH_PROV_OOB_ON_BOX    = BIT(11),
    BLE_MESH_PROV_OOB_IN_BOX    = BIT(12),
    BLE_MESH_PROV_OOB_ON_PAPER  = BIT(13),
    BLE_MESH_PROV_OOB_IN_MANUAL = BIT(14),
    BLE_MESH_PROV_OOB_ON_DEV    = BIT(15),
} bt_mesh_prov_oob_info_t;

#if CONFIG_BLE_MESH_PROV_EPA
#define BLE_MESH_PROV_STATIC_OOB_MAX_LEN    32
#else /* CONFIG_BLE_MESH_PROV_EPA */
#define BLE_MESH_PROV_STATIC_OOB_MAX_LEN    16
#endif /* CONFIG_BLE_MESH_PROV_EPA */

#define BLE_MESH_PROV_OUTPUT_OOB_MAX_LEN    8
#define BLE_MESH_PROV_INPUT_OOB_MAX_LEN     8

/** Provisioning properties & capabilities. */
struct bt_mesh_prov {
#if CONFIG_BLE_MESH_NODE
    /** The UUID that's used when advertising as unprovisioned */
    const uint8_t *uuid;

    /** Optional URI. This will be advertised separately from the
     *  unprovisioned beacon, however the unprovisioned beacon will
     *  contain a hash of it so the two can be associated by the
     *  provisioner.
     */
    const char *uri;

    /** Out of Band information field. */
    bt_mesh_prov_oob_info_t oob_info;

    /** Flag indicates whether unprovisioned devices support OOB public key */
    bool oob_pub_key;

    /** @brief Set device OOB public key.
     *
     *  This callback notifies the application to
     *  set OOB public key & private key pair.
     */
    void (*oob_pub_key_cb)(void);

    /** OOB type */
    uint8_t oob_type;

    /** Static OOB value */
    const uint8_t *static_val;
    /** Static OOB value length */
    uint8_t        static_val_len;

    /** Maximum size of Output OOB supported */
    uint8_t        output_size;
    /** Supported Output OOB Actions */
    uint16_t       output_actions;

    /* Maximum size of Input OOB supported */
    uint8_t        input_size;
    /** Supported Input OOB Actions */
    uint16_t       input_actions;

    /** @brief Output of a number is requested.
     *
     *  This callback notifies the application to
     *  output the given number using the given action.
     *
     *  @param act Action for outputting the number.
     *  @param num Number to be out-put.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_number)(bt_mesh_output_action_t act, uint32_t num);

    /** @brief Output of a string is requested.
     *
     *  This callback notifies the application to
     *  display the given string to the user.
     *
     *  @param str String to be displayed.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*output_string)(const char *str);

    /** @brief Input is requested.
     *
     *  This callback notifies the application to request
     *  input from the user using the given action. The
     *  requested input will either be a string or a number, and
     *  the application needs to consequently call the
     *  bt_mesh_input_string() or bt_mesh_input_number() functions
     *  once the data has been acquired from the user.
     *
     *  @param act Action for inputting data.
     *  @param num Maximum size of the in-put data.
     *
     *  @return Zero on success or negative error code otherwise
     */
    int         (*input)(bt_mesh_input_action_t act, uint8_t size);

    /** @brief Provisioning link has been opened.
     *
     *  This callback notifies the application that a provisioning
     *  link has been opened on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void        (*link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioning link has been closed.
     *
     *  This callback notifies the application that a provisioning
     *  link has been closed on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     *  @param reason Provisioning link close reason(disconnect reason)
     */
    void        (*link_close)(bt_mesh_prov_bearer_t bearer, uint8_t reason);

    /** @brief Provisioning is complete.
     *
     *  This callback notifies the application that provisioning has
     *  been successfully completed, and that the local node has been
     *  assigned the specified NetKeyIndex and primary element address.
     *
     *  @param net_idx  NetKeyIndex given during provisioning.
     *  @param net_key  NetKey given during provisioning.
     *  @param addr     Primary element address.
     *  @param flags    Key Refresh & IV Update flags
     *  @param iv_index IV Index.
     */
    void        (*complete)(uint16_t net_idx, const uint8_t net_key[16], uint16_t addr, uint8_t flags, uint32_t iv_index);

    /** @brief Node has been reset.
     *
     *  This callback notifies the application that the local node
     *  has been reset and needs to be re-provisioned. The node will
     *  not automatically advertise as unprovisioned, rather the
     *  bt_mesh_prov_enable() API needs to be called to enable
     *  unprovisioned advertising on one or more provisioning bearers.
     */
    void        (*reset)(void);
#endif /* CONFIG_BLE_MESH_NODE */

#if CONFIG_BLE_MESH_PROVISIONER
    /* Provisioner device uuid */
    const uint8_t *prov_uuid;

    /*
     * Primary element address of the provisioner.
     * No need to initialize it for fast provisioning.
     */
    const uint16_t prov_unicast_addr;

    /*
     * Starting unicast address going to assigned.
     * No need to initialize it for fast provisioning.
     */
    uint16_t prov_start_address;

    /* Attention timer contained in Provisioning Invite */
    uint8_t  prov_attention;

    /* Provisioner provisioning Algorithm */
    uint8_t  prov_algorithm;

    /* Provisioner public key oob */
    uint8_t  prov_pub_key_oob;

    /** @brief Input is requested.
     *
     *  This callback notifies the application that it should
     *  read device's public key with OOB
     *
     *  @param link_idx: The provisioning link index
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_pub_key_oob_cb)(uint8_t link_idx);

    /* Provisioner static oob value */
    uint8_t *prov_static_oob_val;

    /* Provisioner static oob value length */
    uint8_t  prov_static_oob_len;

    /** @brief Provisioner input a number read from device output
     *
     *  This callback notifies the application that it should
     *  input the number given by the device.
     *
     *  @param method:   The OOB authentication method
     *  @param act:      The output action of the device
     *  @param size:     The output size of the device
     *  @param link_idx: The provisioning link index
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_input_num)(uint8_t method, bt_mesh_output_action_t act, uint8_t size, uint8_t link_idx);

    /** @brief Provisioner output a number to the device
     *
     *  This callback notifies the application that it should
     *  output the number to the device.
     *
     *  @param method:   The OOB authentication method
     *  @param act:      The input action of the device
     *  @param data:     The input number/string of the device
     *  @param size:     The input size of the device
     *  @param link_idx: The provisioning link index
     *
     *  @return Zero on success or negative error code otherwise
     */
    int  (*prov_output_num)(uint8_t method, bt_mesh_input_action_t act, void *data, uint8_t size, uint8_t link_idx);

    /*
     * Key refresh and IV update flag.
     * No need to initialize it for fast provisioning.
     */
    uint8_t  flags;

    /*
     * IV index. No need to initialize it for fast provisioning.
     */
    uint32_t iv_index;

    /** @brief Provisioner has opened a provisioning link.
     *
     *  This callback notifies the application that a provisioning
     *  link has been opened on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     */
    void (*prov_link_open)(bt_mesh_prov_bearer_t bearer);

    /** @brief Provisioner has closed a provisioning link.
     *
     *  This callback notifies the application that a provisioning
     *  link has been closed on the given provisioning bearer.
     *
     *  @param bearer Provisioning bearer.
     *  @param reason Provisioning link close reason(disconnect reason)
     */
    void (*prov_link_close)(bt_mesh_prov_bearer_t bearer, uint8_t reason);

    /** @brief Provision one device is complete.
     *
     *  This callback notifies the application that provisioner has
     *  successfully provisioned a device, and the node has been assigned
     *  the specified NetKeyIndex and primary element address.
     *
     *  @param node_idx     Node index within the node(provisioned device) queue.
     *  @param device_uuid  Provisioned device uuid pointer.
     *  @param unicast_addr Provisioned device assigned unicast address.
     *  @param element_num  Provisioned device element number.
     *  @param netkey_idx   Provisioned device assigned netkey index.
     */
    void (*prov_complete)(uint16_t node_idx, const uint8_t device_uuid[16],
                          uint16_t unicast_addr, uint8_t element_num,
                          uint16_t netkey_idx);

    /** @brief Provisioner start certificate based provisioning.
     *
     *  This callback notifies the application that certificate based provisioning
     *  has started. Provisioner need to send prov_records_get, prov_record_request
     *  or prov_invite message.
     *
     * @param link_idx: The provisioning link index
     */
    void (*cert_based_prov_start)(uint16_t link_idx);

    /** @brief Provisioner receive provisioning records list from device.
     *
     *  This callback notifies the application that provisioner has received
     *  provisioning records list, and will send the list to application.
     *
     * @param link_idx: The provisioning link index
     * @param data:     The provisioning records list.
     */
    void (*records_list_get)(uint16_t link_idx, struct net_buf_simple *data);

    /** @brief Provisioner receive complete provisioning record from device.
     *
     *  This callback notifies the application that provisioner has received
     *  complete provisioning record, and will send the record to application.
     *
     * @param status:      Status
     * @param link_idx:    The provisioning link index.
     * @param record_id:   The provisioning record index.
     * @param frag_offset: The starting of the fragment.
     * @param total_len:   The length of the record.
     * @param record:      The data of the record.
     */
    void (*prov_record_recv_comp)(uint8_t status, uint16_t link_idx, uint16_t record_id,
                                  uint16_t frag_offset, uint16_t total_len, uint8_t *record);
#endif /* CONFIG_BLE_MESH_PROVISIONER */
};

enum ble_mesh_role {
    NODE = 0,
    PROVISIONER,
    FAST_PROV,
    ROLE_NVAL,
};

/* The following APIs are for BLE Mesh Node */

/** @brief Provide provisioning input OOB string.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BLE_MESH_ENTER_STRING as the action.
 *
 *  @param str String.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_input_string(const char *str);

/** @brief Provide provisioning input OOB number.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BLE_MESH_ENTER_NUMBER as the action.
 *
 *  @param num Number.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_input_number(uint32_t num);

/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param Bit-wise OR of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_enable(bt_mesh_prov_bearer_t bearers);

/** @brief Disable specific provisioning bearers
 *
 *  Disable one or more provisioning bearers.
 *
 *  @param Bit-wise OR of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_disable(bt_mesh_prov_bearer_t bearers);

/* The following APIs are for BLE Mesh Provisioner */

/** @brief Provide provisioning input OOB string.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BLE_MESH_ENTER_STRING as the action.
 *
 *  @param str String.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_input_string(const char *str);

/** @brief Provide provisioning input OOB number.
 *
 *  This is intended to be called after the bt_mesh_prov input callback
 *  has been called with BLE_MESH_ENTER_NUMBER as the action.
 *
 *  @param num Number.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_prov_input_number(uint32_t num);

/** @brief Enable specific provisioning bearers
 *
 *  Enable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise OR of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provisioner_enable(bt_mesh_prov_bearer_t bearers);

/** @brief Disable specific provisioning bearers
 *
 *  Disable one or more provisioning bearers.
 *
 *  @param bearers Bit-wise OR of provisioning bearers.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provisioner_disable(bt_mesh_prov_bearer_t bearers);

/**
 * @}
 */

/**
 * @brief Bluetooth Mesh
 * @defgroup bt_mesh Bluetooth Mesh
 * @ingroup bluetooth
 * @{
 */

/* Primary Network Key index */
#define BLE_MESH_NET_PRIMARY                            0x000

#define BLE_MESH_RELAY_DISABLED                         0x00
#define BLE_MESH_RELAY_ENABLED                          0x01
#define BLE_MESH_RELAY_NOT_SUPPORTED                    0x02

#define BLE_MESH_SECURE_BEACON_DISABLED                 0x00
#define BLE_MESH_SECURE_BEACON_ENABLED                  0x01

#define BLE_MESH_GATT_PROXY_DISABLED                    0x00
#define BLE_MESH_GATT_PROXY_ENABLED                     0x01
#define BLE_MESH_GATT_PROXY_NOT_SUPPORTED               0x02

#define BLE_MESH_FRIEND_DISABLED                        0x00
#define BLE_MESH_FRIEND_ENABLED                         0x01
#define BLE_MESH_FRIEND_NOT_SUPPORTED                   0x02

#define BLE_MESH_NODE_IDENTITY_STOPPED                  0x00
#define BLE_MESH_NODE_IDENTITY_RUNNING                  0x01
#define BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED            0x02

#define BLE_MESH_PRIVATE_BEACON_DISABLED                0x00
#define BLE_MESH_PRIVATE_BEACON_ENABLED                 0x01

#define BLE_MESH_RANDOM_UPDATE_EVERY_PRIVATE_BEACON     0x00

#define BLE_MESH_PRIVATE_GATT_PROXY_DISABLED            0x00
#define BLE_MESH_PRIVATE_GATT_PROXY_ENABLED             0x01
#define BLE_MESH_PRIVATE_GATT_PROXY_NOT_SUPPORTED       0x02

#define BLE_MESH_PRIVATE_NODE_IDENTITY_STOPPED          0x00
#define BLE_MESH_PRIVATE_NODE_IDENTITY_RUNNING          0x01
#define BLE_MESH_PRIVATE_NODE_IDENTITY_NOT_SUPPORTED    0x02

#define BLE_MESH_SUBNET_BRIDGE_DISABLED                 0x00
#define BLE_MESH_SUBNET_BRIDGE_ENABLED                  0x01

#define BLE_MESH_PRIVATE_NET_ID_CANNOT_EN_ON_DEMAND     0x00

#define BLE_MESH_DIRECTED_FORWARDING_DISABLED           0x00
#define BLE_MESH_DIRECTED_FORWARDING_ENABLED            0x01

#define BLE_MESH_DIRECTED_RELAY_DISABLED                0x00
#define BLE_MESH_DIRECTED_RELAY_ENABLED                 0x01

#define BLE_MESH_DIRECTED_PROXY_DISABLED                0x00
#define BLE_MESH_DIRECTED_PROXY_ENABLED                 0x01
#define BLE_MESH_DIRECTED_PROXY_NOT_SUPPORTED           0x02

#define BLE_MESH_DIRECTED_PROXY_USE_DEF_DISABLED        0x00
#define BLE_MESH_DIRECTED_PROXY_USE_DEF_ENABLED         0x01
#define BLE_MESH_DIRECTED_PROXY_USE_DEF_NOT_SUPPORTED   0x02

#define BLE_MESH_DIRECTED_FRIEND_DISABLED               0x00
#define BLE_MESH_DIRECTED_FRIEND_ENABLED                0x01
#define BLE_MESH_DIRECTED_FRIEND_NOT_SUPPORTED          0x02

#define BLE_MESH_DIRECTED_PUB_POLICY_FLOODING           0x00
#define BLE_MESH_DIRECTED_PUB_POLICY_FORWARD            0x01

#define BLE_MESH_PROXY_USE_DIRECTED_DISABLED            0x00
#define BLE_MESH_PROXY_USE_DIRECTED_ENABLED             0x01

#define BLE_MESH_FLOODING_CRED                          0x00
#define BLE_MESH_FRIENDSHIP_CRED                        0x01
#define BLE_MESH_DIRECTED_CRED                          0x02

/* Features */
#define BLE_MESH_FEAT_RELAY                             BIT(0)
#define BLE_MESH_FEAT_PROXY                             BIT(1)
#define BLE_MESH_FEAT_FRIEND                            BIT(2)
#define BLE_MESH_FEAT_LOW_POWER                         BIT(3)
#define BLE_MESH_FEAT_SUPPORTED                         (BLE_MESH_FEAT_RELAY | \
                                                         BLE_MESH_FEAT_PROXY | \
                                                         BLE_MESH_FEAT_FRIEND | \
                                                         BLE_MESH_FEAT_LOW_POWER)

/** @brief Check if the mesh stack is initialized.
 *
 *  @return true - yes, false - no.
 */
bool bt_mesh_is_initialized(void);

/** @brief Initialize Mesh support
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 *  @param prov Node provisioning information.
 *  @param comp Node Composition.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_init(const struct bt_mesh_prov *prov,
                 const struct bt_mesh_comp *comp);

/* BLE Mesh deinit parameters */
struct bt_mesh_deinit_param {
    bool erase; /* Indicate if erasing flash when deinit mesh stack */
};

/** @brief De-initialize Mesh support
 *
 *  @param param BLE Mesh deinit parameters.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_deinit(struct bt_mesh_deinit_param *param);

/** @brief Reset the state of the local Mesh node.
 *
 *  Resets the state of the node, which means that it needs to be
 *  re-provisioned to become an active node in a Mesh network again.
 *
 *  After calling this API, the node will not automatically advertise as
 *  unprovisioned, rather the bt_mesh_prov_enable() API needs to be called
 *  to enable unprovisioned advertising on one or more provisioning bearers.
 *
 */
void bt_mesh_node_reset(void);

/** @brief Suspend the Mesh network temporarily.
 *
 *  This API can be used for power saving purposes, but the user should be
 *  aware that leaving the local node suspended for a long period of time
 *  may cause it to become permanently disconnected from the Mesh network.
 *  If at all possible, the Friendship feature should be used instead, to
 *  make the node into a Low Power Node.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_suspend(void);

/** @brief Resume a suspended Mesh network.
 *
 *  This API resumes the local node, after it has been suspended using the
 *  bt_mesh_suspend() API.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_resume(void);

/** @brief Provision the local Mesh Node.
 *
 *  This API should normally not be used directly by the application. The
 *  only exception is for testing purposes where manual provisioning is
 *  desired without an actual external provisioner.
 *
 *  @param net_key  Network Key
 *  @param net_idx  Network Key Index
 *  @param flags    Provisioning Flags
 *  @param iv_index IV Index
 *  @param addr     Primary element address
 *  @param dev_key  Device Key
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_provision(const uint8_t net_key[16], uint16_t net_idx,
                      uint8_t flags, uint32_t iv_index, uint16_t addr,
                      const uint8_t dev_key[16]);

/** @brief Check if the device is an unprovisioned device
 *         and will act as a node once provisioned.
 *
 *  @return true - yes, false - no.
 */
bool bt_mesh_is_node(void);

/** @brief Check if the local node has been provisioned.
 *
 *  This API can be used to check if the local node has been provisioned
 *  or not. It can e.g. be helpful to determine if there was a stored
 *  network in flash, i.e. if the network was restored after calling
 *  settings_load().
 *
 *  @return True if the node is provisioned. False otherwise.
 */
bool bt_mesh_is_provisioned(void);

/** @brief Check if the device is a Provisioner.
 *
 *  @return true - yes, false - no.
 */
bool bt_mesh_is_provisioner(void);

/** @brief Check if the Provisioner is enabled
 *
 *  @return true - enabled, false - disabled.
 */
bool bt_mesh_is_provisioner_en(void);

/** @brief Toggle the IV Update test mode
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @param enable true to enable IV Update test mode, false to disable it.
 */
void bt_mesh_iv_update_test(bool enable);

/** @brief Toggle the IV Update state
 *
 *  This API is only available if the IV Update test mode has been enabled
 *  in Kconfig. It is needed for passing most of the IV Update qualification
 *  test cases.
 *
 *  @return true if IV Update In Progress state was entered, false otherwise.
 */
bool bt_mesh_iv_update(void);

/** @brief Toggle the Low Power feature of the local device
 *
 *  Enables or disables the Low Power feature of the local device. This is
 *  exposed as a run-time feature, since the device might want to change
 *  this e.g. based on being plugged into a stable power source or running
 *  from a battery power source.
 *
 *  @param enable  true to enable LPN functionality, false to disable it.
 *  @param force   when disable LPN functionality, use this flag to indicate
 *                 whether directly clear corresponding information or sending
 *                 friend clear to disable it.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_set(bool enable, bool force);

/** @brief Send out a Friend Poll message.
 *
 *  Send a Friend Poll message to the Friend of this node. If there is no
 *  established Friendship the function will return an error.
 *
 *  @return Zero on success or (negative) error code otherwise.
 */
int bt_mesh_lpn_poll(void);

/** @brief Register a callback for Friendship changes.
 *
 *  Registers a callback that will be called whenever Friendship gets
 *  established or is lost.
 *
 *  @param cb Function to call when the Friendship status changes.
 */
void bt_mesh_lpn_set_cb(void (*cb)(uint16_t friend_addr, bool established));

/** @brief Register a callback for Friendship changes of friend node.
 *
 *  Registers a callback that will be called whenever Friendship gets
 *  established or is terminated.
 *
 *  @param cb Function to call when the Friendship status of friend node changes.
 */
void bt_mesh_friend_set_cb(void (*cb)(bool establish, uint16_t lpn_addr, uint8_t reason));

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* _BLE_MESH_MAIN_H_ */
