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

#ifndef _ESP_BLE_MESH_DEFS_H_
#define _ESP_BLE_MESH_DEFS_H_

#include <stdint.h>

#include "mesh_proxy.h"
#include "mesh_access.h"
#include "mesh_main.h"

#include "mesh.h"
#include "proxy_server.h"
#include "foundation.h"
#include "provisioner_main.h"

#include "model_opcode.h"
#include "mesh_common.h"

#ifdef CONFIG_BT_BLUEDROID_ENABLED
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#define ESP_BLE_HOST_STATUS_ENABLED ESP_BLUEDROID_STATUS_ENABLED
#define ESP_BLE_HOST_STATUS_CHECK(status) ESP_BLUEDROID_STATUS_CHECK(status)
#else
#define ESP_BLE_HOST_STATUS_ENABLED 0
#define ESP_BLE_HOST_STATUS_CHECK(status)  do {} while (0)
#endif

/*!< The maximum length of a BLE Mesh message, including Opcode, Payload and TransMIC */
#define ESP_BLE_MESH_SDU_MAX_LEN            384

/*!< The maximum length of a BLE Mesh provisioned node name */
#define ESP_BLE_MESH_NODE_NAME_MAX_LEN      31

/*!< The maximum length of a BLE Mesh unprovisioned device name */
#define ESP_BLE_MESH_DEVICE_NAME_MAX_LEN    DEVICE_NAME_SIZE

/*!< Define the BLE Mesh octet 16 bytes size */
#define ESP_BLE_MESH_OCTET16_LEN    16
typedef uint8_t esp_ble_mesh_octet16_t[ESP_BLE_MESH_OCTET16_LEN];

/*!< Define the BLE Mesh octet 8 bytes size */
#define ESP_BLE_MESH_OCTET8_LEN     8
typedef uint8_t esp_ble_mesh_octet8_t[ESP_BLE_MESH_OCTET8_LEN];

#define ESP_BLE_MESH_ADDR_UNASSIGNED              BLE_MESH_ADDR_UNASSIGNED
#define ESP_BLE_MESH_ADDR_ALL_NODES               BLE_MESH_ADDR_ALL_NODES
#define ESP_BLE_MESH_ADDR_PROXIES                 BLE_MESH_ADDR_PROXIES
#define ESP_BLE_MESH_ADDR_FRIENDS                 BLE_MESH_ADDR_FRIENDS
#define ESP_BLE_MESH_ADDR_RELAYS                  BLE_MESH_ADDR_RELAYS

#define ESP_BLE_MESH_KEY_UNUSED                   BLE_MESH_KEY_UNUSED
#define ESP_BLE_MESH_KEY_DEV                      BLE_MESH_KEY_DEV

#define ESP_BLE_MESH_KEY_PRIMARY                  BLE_MESH_KEY_PRIMARY
#define ESP_BLE_MESH_KEY_ANY                      BLE_MESH_KEY_ANY

/*!< Primary Network Key index */
#define ESP_BLE_MESH_NET_PRIMARY                  BLE_MESH_NET_PRIMARY

/*!< Relay state value */
#define ESP_BLE_MESH_RELAY_DISABLED               BLE_MESH_RELAY_DISABLED
#define ESP_BLE_MESH_RELAY_ENABLED                BLE_MESH_RELAY_ENABLED
#define ESP_BLE_MESH_RELAY_NOT_SUPPORTED          BLE_MESH_RELAY_NOT_SUPPORTED

/*!< Beacon state value */
#define ESP_BLE_MESH_BEACON_DISABLED              BLE_MESH_BEACON_DISABLED
#define ESP_BLE_MESH_BEACON_ENABLED               BLE_MESH_BEACON_ENABLED

/*!< GATT Proxy state value */
#define ESP_BLE_MESH_GATT_PROXY_DISABLED          BLE_MESH_GATT_PROXY_DISABLED
#define ESP_BLE_MESH_GATT_PROXY_ENABLED           BLE_MESH_GATT_PROXY_ENABLED
#define ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED     BLE_MESH_GATT_PROXY_NOT_SUPPORTED

/*!< Friend state value */
#define ESP_BLE_MESH_FRIEND_DISABLED              BLE_MESH_FRIEND_DISABLED
#define ESP_BLE_MESH_FRIEND_ENABLED               BLE_MESH_FRIEND_ENABLED
#define ESP_BLE_MESH_FRIEND_NOT_SUPPORTED         BLE_MESH_FRIEND_NOT_SUPPORTED

/*!< Node identity state value */
#define ESP_BLE_MESH_NODE_IDENTITY_STOPPED        BLE_MESH_NODE_IDENTITY_STOPPED
#define ESP_BLE_MESH_NODE_IDENTITY_RUNNING        BLE_MESH_NODE_IDENTITY_RUNNING
#define ESP_BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED  BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED

/*!< Supported features */
#define ESP_BLE_MESH_FEATURE_RELAY                BLE_MESH_FEAT_RELAY
#define ESP_BLE_MESH_FEATURE_PROXY                BLE_MESH_FEAT_PROXY
#define ESP_BLE_MESH_FEATURE_FRIEND               BLE_MESH_FEAT_FRIEND
#define ESP_BLE_MESH_FEATURE_LOW_POWER            BLE_MESH_FEAT_LOW_POWER
#define ESP_BLE_MESH_FEATURE_ALL_SUPPORTED        BLE_MESH_FEAT_SUPPORTED

#define ESP_BLE_MESH_ADDR_IS_UNICAST(addr)        BLE_MESH_ADDR_IS_UNICAST(addr)
#define ESP_BLE_MESH_ADDR_IS_GROUP(addr)          BLE_MESH_ADDR_IS_GROUP(addr)
#define ESP_BLE_MESH_ADDR_IS_VIRTUAL(addr)        BLE_MESH_ADDR_IS_VIRTUAL(addr)
#define ESP_BLE_MESH_ADDR_IS_RFU(addr)            BLE_MESH_ADDR_IS_RFU(addr)

#define ESP_BLE_MESH_INVALID_NODE_INDEX          (-1)

/** @def    ESP_BLE_MESH_TRANSMIT
 *
 *  @brief  Encode transmission count & interval steps.
 *
 *  @note   For example, ESP_BLE_MESH_TRANSMIT(2, 20) means that the message
 *          will be sent about 90ms(count is 3, step is 1, interval is 30 ms
 *          which includes 10ms of advertising interval random delay).
 *
 *  @param  count   Number of retransmissions (first transmission is excluded).
 *  @param  int_ms  Interval steps in milliseconds. Must be greater than 0
 *                  and a multiple of 10.
 *
 *  @return BLE Mesh transmit value that can be used e.g. for the default
 *          values of the Configuration Model data.
 */
#define ESP_BLE_MESH_TRANSMIT(count, int_ms) BLE_MESH_TRANSMIT(count, int_ms)

/** @def ESP_BLE_MESH_GET_TRANSMIT_COUNT
 *
 *  @brief Decode transmit count from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission count (actual transmissions equal to N + 1).
 */
#define ESP_BLE_MESH_GET_TRANSMIT_COUNT(transmit) BLE_MESH_TRANSMIT_COUNT(transmit)

/** @def ESP_BLE_MESH_GET_TRANSMIT_INTERVAL
 *
 *  @brief Decode transmit interval from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define ESP_BLE_MESH_GET_TRANSMIT_INTERVAL(transmit) BLE_MESH_TRANSMIT_INT(transmit)

/** @def ESP_BLE_MESH_PUBLISH_TRANSMIT
 *
 *  @brief Encode Publish Retransmit count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0
 *                 and a multiple of 50.
 *
 *  @return BLE Mesh transmit value that can be used e.g. for the default
 *          values of the Configuration Model data.
 */
#define ESP_BLE_MESH_PUBLISH_TRANSMIT(count, int_ms) BLE_MESH_PUB_TRANSMIT(count, int_ms)

/** @def ESP_BLE_MESH_GET_PUBLISH_TRANSMIT_COUNT
 *
 *  @brief Decode Publish Retransmit count from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Retransmission count (actual transmissions equal to N + 1).
 */
#define ESP_BLE_MESH_GET_PUBLISH_TRANSMIT_COUNT(transmit) BLE_MESH_PUB_TRANSMIT_COUNT(transmit)

/** @def ESP_BLE_MESH_GET_PUBLISH_TRANSMIT_INTERVAL
 *
 *  @brief Decode Publish Retransmit interval from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define ESP_BLE_MESH_GET_PUBLISH_TRANSMIT_INTERVAL(transmit) BLE_MESH_PUB_TRANSMIT_INT(transmit)

/*!< Callbacks which are not needed to be initialized by users (set with 0 and will be initialized internally) */
typedef uint32_t esp_ble_mesh_cb_t;

typedef enum {
    ESP_BLE_MESH_TYPE_PROV_CB,
    ESP_BLE_MESH_TYPE_OUTPUT_NUM_CB,
    ESP_BLE_MESH_TYPE_OUTPUT_STR_CB,
    ESP_BLE_MESH_TYPE_INTPUT_CB,
    ESP_BLE_MESH_TYPE_LINK_OPEN_CB,
    ESP_BLE_MESH_TYPE_LINK_CLOSE_CB,
    ESP_BLE_MESH_TYPE_COMPLETE_CB,
    ESP_BLE_MESH_TYPE_RESET_CB,
} esp_ble_mesh_cb_type_t;

/*!< This enum value is provisioning authentication oob method */
typedef enum {
    ESP_BLE_MESH_NO_OOB,
    ESP_BLE_MESH_STATIC_OOB,
    ESP_BLE_MESH_OUTPUT_OOB,
    ESP_BLE_MESH_INPUT_OOB,
} esp_ble_mesh_oob_method_t;

/*!< This enum value is associated with bt_mesh_output_action_t in mesh_main.h */
typedef enum {
    ESP_BLE_MESH_NO_OUTPUT       = 0,
    ESP_BLE_MESH_BLINK           = BIT(0),
    ESP_BLE_MESH_BEEP            = BIT(1),
    ESP_BLE_MESH_VIBRATE         = BIT(2),
    ESP_BLE_MESH_DISPLAY_NUMBER  = BIT(3),
    ESP_BLE_MESH_DISPLAY_STRING  = BIT(4),
} esp_ble_mesh_output_action_t;

/*!< This enum value is associated with bt_mesh_input_action_t in mesh_main.h */
typedef enum {
    ESP_BLE_MESH_NO_INPUT      = 0,
    ESP_BLE_MESH_PUSH          = BIT(0),
    ESP_BLE_MESH_TWIST         = BIT(1),
    ESP_BLE_MESH_ENTER_NUMBER  = BIT(2),
    ESP_BLE_MESH_ENTER_STRING  = BIT(3),
} esp_ble_mesh_input_action_t;

/*!< This enum value is associated with bt_mesh_prov_bearer_t in mesh_main.h */
typedef enum {
    ESP_BLE_MESH_PROV_ADV   = BIT(0),
    ESP_BLE_MESH_PROV_GATT  = BIT(1),
} esp_ble_mesh_prov_bearer_t;

/*!< This enum value is associated with bt_mesh_prov_oob_info_t in mesh_main.h */
typedef enum {
    ESP_BLE_MESH_PROV_OOB_OTHER     = BIT(0),
    ESP_BLE_MESH_PROV_OOB_URI       = BIT(1),
    ESP_BLE_MESH_PROV_OOB_2D_CODE   = BIT(2),
    ESP_BLE_MESH_PROV_OOB_BAR_CODE  = BIT(3),
    ESP_BLE_MESH_PROV_OOB_NFC       = BIT(4),
    ESP_BLE_MESH_PROV_OOB_NUMBER    = BIT(5),
    ESP_BLE_MESH_PROV_OOB_STRING    = BIT(6),
    /* 7 - 10 are reserved */
    ESP_BLE_MESH_PROV_OOB_ON_BOX    = BIT(11),
    ESP_BLE_MESH_PROV_OOB_IN_BOX    = BIT(12),
    ESP_BLE_MESH_PROV_OOB_ON_PAPER  = BIT(13),
    ESP_BLE_MESH_PROV_OOB_IN_MANUAL = BIT(14),
    ESP_BLE_MESH_PROV_OOB_ON_DEV    = BIT(15),
} esp_ble_mesh_prov_oob_info_t;

/*!< Macros used to define message opcode */
#define ESP_BLE_MESH_MODEL_OP_1(b0)            BLE_MESH_MODEL_OP_1(b0)
#define ESP_BLE_MESH_MODEL_OP_2(b0, b1)        BLE_MESH_MODEL_OP_2(b0, b1)
#define ESP_BLE_MESH_MODEL_OP_3(b0, cid)       BLE_MESH_MODEL_OP_3(b0, cid)

/*!< This macro is associated with BLE_MESH_MODEL in mesh_access.h */
#define ESP_BLE_MESH_SIG_MODEL(_id, _op, _pub, _user_data)          \
{                                                                   \
    .model_id = (_id),                                              \
    .op = _op,                                                      \
    .keys = { [0 ... (CONFIG_BLE_MESH_MODEL_KEY_COUNT - 1)] =       \
            ESP_BLE_MESH_KEY_UNUSED },                              \
    .pub = _pub,                                                    \
    .groups = { [0 ... (CONFIG_BLE_MESH_MODEL_GROUP_COUNT - 1)] =   \
            ESP_BLE_MESH_ADDR_UNASSIGNED },                         \
    .user_data = _user_data,                                        \
}

/*!< This macro is associated with BLE_MESH_MODEL_VND in mesh_access.h */
#define ESP_BLE_MESH_VENDOR_MODEL(_company, _id, _op, _pub, _user_data) \
{                                                                       \
    .vnd.company_id = (_company),                                       \
    .vnd.model_id = (_id),                                              \
    .op = _op,                                                          \
    .pub = _pub,                                                        \
    .keys = { [0 ... (CONFIG_BLE_MESH_MODEL_KEY_COUNT - 1)] =           \
            ESP_BLE_MESH_KEY_UNUSED },                                  \
    .groups = { [0 ... (CONFIG_BLE_MESH_MODEL_GROUP_COUNT - 1)] =       \
            ESP_BLE_MESH_ADDR_UNASSIGNED },                             \
    .user_data = _user_data,                                            \
}

/** @brief Helper to define a BLE Mesh element within an array.
 *
 *  In case the element has no SIG or Vendor models, the helper
 *  macro ESP_BLE_MESH_MODEL_NONE can be given instead.
 *
 *  @note This macro is associated with BLE_MESH_ELEM in mesh_access.h
 *
 *  @param _loc       Location Descriptor.
 *  @param _mods      Array of SIG models.
 *  @param _vnd_mods  Array of vendor models.
 */
#define ESP_BLE_MESH_ELEMENT(_loc, _mods, _vnd_mods)    \
{                                                       \
    .location         = (_loc),                         \
    .sig_model_count  = ARRAY_SIZE(_mods),              \
    .sig_models       = (_mods),                        \
    .vnd_model_count  = ARRAY_SIZE(_vnd_mods),          \
    .vnd_models       = (_vnd_mods),                    \
}

#define ESP_BLE_MESH_PROV(uuid, sta_val, sta_val_len, out_size, out_act, in_size, in_act) { \
    .uuid           = uuid,         \
    .static_val     = sta_val,      \
    .static_val_len = sta_val_len,  \
    .output_size    = out_size,     \
    .output_action  = out_act,      \
    .input_size     = in_size,      \
    .input_action   = in_act,       \
}

typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;

#define BT_OCTET32_LEN    32
typedef UINT8 BT_OCTET32[BT_OCTET32_LEN];   /* octet array: size 32 */


#ifndef BD_ADDR_LEN
#define BD_ADDR_LEN     6
typedef uint8_t BD_ADDR[BD_ADDR_LEN];
#endif

typedef uint8_t esp_ble_mesh_bd_addr_t[BD_ADDR_LEN];

/// BLE device address type
typedef enum {
    ESP_BLE_MESH_ADDR_TYPE_PUBLIC        = 0x00,
    ESP_BLE_MESH_ADDR_TYPE_RANDOM        = 0x01,
    ESP_BLE_MESH_ADDR_TYPE_RPA_PUBLIC    = 0x02,
    ESP_BLE_MESH_ADDR_TYPE_RPA_RANDOM    = 0x03,
} esp_ble_mesh_addr_type_t;

typedef struct esp_ble_mesh_model esp_ble_mesh_model_t;

/** Abstraction that describes a BLE Mesh Element.
 *  This structure is associated with struct bt_mesh_elem in mesh_access.h
 */
typedef struct {
    /** Element Address, assigned during provisioning. */
    uint16_t element_addr;

    /** Location Descriptor (GATT Bluetooth Namespace Descriptors) */
    const uint16_t location;

    const uint8_t sig_model_count;      /*!< SIG Model count */
    const uint8_t vnd_model_count;      /*!< Vendor Model count */

    esp_ble_mesh_model_t *sig_models;   /*!< SIG Models */
    esp_ble_mesh_model_t *vnd_models;   /*!< Vendor Models */
} esp_ble_mesh_elem_t;

/** Abstraction that describes a model publication context.
 *  This structure is associated with struct bt_mesh_model_pub in mesh_access.h
 */
typedef struct {
    /** Pointer to the model to which the context belongs. Initialized by the stack. */
    esp_ble_mesh_model_t *model;

    uint16_t publish_addr; /*!< Publish Address. */
    uint16_t app_idx;      /*!< Publish AppKey Index. */

    uint8_t  ttl;          /*!< Publish Time to Live. */
    uint8_t  retransmit;   /*!< Retransmit Count & Interval Steps. */

    uint8_t  period;        /*!< Publish Period. */
    uint16_t period_div: 4, /*!< Divisor for the Period. */
             cred: 1,       /*!< Friendship Credentials Flag. */
             fast_period: 1, /*!< Use FastPeriodDivisor */
             count: 3;      /*!< Retransmissions left. */

    uint32_t period_start; /*!< Start of the current period. */

    /** @brief Publication buffer, containing the publication message.
     *
     *  This will get correctly created when the publication context
     *  has been defined using the ESP_BLE_MESH_MODEL_PUB_DEFINE macro.
     *
     *  ESP_BLE_MESH_MODEL_PUB_DEFINE(name, size);
     */
    struct net_buf_simple *msg;

    /** Callback used to update publish message. Initialized by the stack. */
    esp_ble_mesh_cb_t update;

    /** Role of the device that is going to publish messages */
    uint8_t dev_role;

    /** Publish Period Timer. Initialized by the stack. */
    struct k_delayed_work timer;
} esp_ble_mesh_model_pub_t;

/** @def ESP_BLE_MESH_MODEL_PUB_DEFINE
 *
 *  Define a model publication context.
 *
 *  @param _name    Variable name given to the context.
 *  @param _msg_len Length of the publication message.
 *  @param _role    Role of the device which contains the model.
 */
#define ESP_BLE_MESH_MODEL_PUB_DEFINE(_name, _msg_len, _role) \
    NET_BUF_SIMPLE_DEFINE_STATIC(bt_mesh_pub_msg_##_name, _msg_len); \
    static esp_ble_mesh_model_pub_t _name = { \
        .update = (uint32_t)NULL, \
        .msg = &bt_mesh_pub_msg_##_name, \
        .dev_role = _role, \
    }

/** @def ESP_BLE_MESH_MODEL_OP
 *
 *  Define a model operation context.
 *
 *  @param _opcode  Message opcode.
 *  @param _min_len Message minimum length.
 */
#define ESP_BLE_MESH_MODEL_OP(_opcode, _min_len) \
{ \
    .opcode = _opcode, \
    .min_len = _min_len, \
    .param_cb = (uint32_t)NULL, \
}

/** Abstraction that describes a model operation context.
 *  This structure is associated with struct bt_mesh_model_op in mesh_access.h
 */
typedef struct {
    const uint32_t    opcode;   /*!< Message opcode */
    const size_t      min_len;  /*!< Message minimum length */
    esp_ble_mesh_cb_t param_cb; /*!< Callback used to handle message. Initialized by the stack. */
} esp_ble_mesh_model_op_t;

/** Define the terminator for the model operation table.
 *  Each model operation struct array must use this terminator as
 *  the end tag of the operation unit.
 */
#define ESP_BLE_MESH_MODEL_OP_END {0, 0, 0}

/** Abstraction that describes a Mesh Model instance.
 *  This structure is associated with struct bt_mesh_model in mesh_access.h
 */
struct esp_ble_mesh_model {
    /** Model ID */
    union {
        const uint16_t model_id;
        struct {
            uint16_t company_id;
            uint16_t model_id;
        } vnd;
    };

    /** Internal information, mainly for persistent storage */
    uint8_t  element_idx;   /*!< Belongs to Nth element */
    uint8_t  model_idx;     /*!< Is the Nth model in the element */
    uint16_t flags;         /*!< Information about what has changed */

    /** The Element to which this Model belongs */
    esp_ble_mesh_elem_t *element;

    /** Model Publication */
    esp_ble_mesh_model_pub_t *const pub;

    /** AppKey List */
    uint16_t keys[CONFIG_BLE_MESH_MODEL_KEY_COUNT];

    /** Subscription List (group or virtual addresses) */
    uint16_t groups[CONFIG_BLE_MESH_MODEL_GROUP_COUNT];

    /** Model operation context */
    esp_ble_mesh_model_op_t *op;

    /** Model-specific user data */
    void *user_data;
};

/** Helper to define an empty model array.
 *  This structure is associated with BLE_MESH_MODEL_NONE in mesh_access.h
 */
#define ESP_BLE_MESH_MODEL_NONE ((esp_ble_mesh_model_t []){})

/** Message sending context.
 *  This structure is associated with struct bt_mesh_msg_ctx in mesh_access.h
 */
typedef struct {
    /** NetKey Index of the subnet through which to send the message. */
    uint16_t net_idx;

    /** AppKey Index for message encryption. */
    uint16_t app_idx;

    /** Remote address. */
    uint16_t addr;

    /** Destination address of a received message. Not used for sending. */
    uint16_t recv_dst;

    /** Received TTL value. Not used for sending. */
    uint8_t  recv_ttl: 7;

    /** Force sending reliably by using segment acknowledgement */
    uint8_t  send_rel: 1;

    /** TTL, or BLE_MESH_TTL_DEFAULT for default TTL. */
    uint8_t  send_ttl;

    /** Opcode of a received message. Not used for sending message. */
    uint32_t recv_op;

    /** Model corresponding to the message, no need to be initialized before sending message */
    esp_ble_mesh_model_t *model;

    /** Indicate if the message is sent by a node server model, no need to be initialized before sending message */
    bool srv_send;
} esp_ble_mesh_msg_ctx_t;

/** Provisioning properties & capabilities.
 *  This structure is associated with struct bt_mesh_prov in mesh_access.h
 */
typedef struct {
#if CONFIG_BLE_MESH_NODE
    /** The UUID that is used when advertising as an unprovisioned device */
    const uint8_t *uuid;

    /** Optional URI. This will be advertised separately from the
     *  unprovisioned beacon, however the unprovisioned beacon will
     *  contain a hash of it so the two can be associated by the
     *  provisioner.
     */
    const char *uri;

    /** Out of Band information field. */
    esp_ble_mesh_prov_oob_info_t oob_info;

    /** Flag indicates whether unprovisioned devices support OOB public key */
    bool oob_pub_key;

    /** Callback used to notify to set OOB Public Key. Initialized by the stack. */
    esp_ble_mesh_cb_t oob_pub_key_cb;

    /** Static OOB value */
    const uint8_t *static_val;
    /** Static OOB value length */
    uint8_t        static_val_len;

    /** Maximum size of Output OOB supported */
    uint8_t        output_size;
    /** Supported Output OOB Actions */
    uint16_t       output_actions;

    /** Maximum size of Input OOB supported */
    uint8_t        input_size;
    /** Supported Input OOB Actions */
    uint16_t       input_actions;

    /** Callback used to output the number. Initialized by the stack. */
    esp_ble_mesh_cb_t  output_num_cb;
    /** Callback used to output the string. Initialized by the stack. */
    esp_ble_mesh_cb_t  output_str_cb;
    /** Callback used to notify to input number/string. Initialized by the stack. */
    esp_ble_mesh_cb_t  input_cb;
    /** Callback used to indicate that link is opened. Initialized by the stack. */
    esp_ble_mesh_cb_t  link_open_cb;
    /** Callback used to indicate that link is closed. Initialized by the stack. */
    esp_ble_mesh_cb_t  link_close_cb;
    /** Callback used to indicate that provisioning is completed. Initialized by the stack. */
    esp_ble_mesh_cb_t  complete_cb;
    /** Callback used to indicate that node has been reset. Initialized by the stack. */
    esp_ble_mesh_cb_t  reset_cb;
#endif /* CONFIG_BLE_MESH_NODE */

#ifdef CONFIG_BLE_MESH_PROVISIONER
    /** Provisioner device UUID */
    const uint8_t *prov_uuid;

    /** Primary element address of the provisioner */
    const uint16_t prov_unicast_addr;

    /** Pre-incremental unicast address value to be assigned to the first device */
    uint16_t       prov_start_address;

    /** Attention timer contained in Provisioning Invite PDU */
    uint8_t        prov_attention;

    /** Provisioning Algorithm for the Provisioner */
    uint8_t        prov_algorithm;

    /** Provisioner public key oob */
    uint8_t        prov_pub_key_oob;

    /** Callback used to notify to set device OOB Public Key. Initialized by the stack. */
    esp_ble_mesh_cb_t provisioner_prov_read_oob_pub_key;

    /** Provisioner static oob value */
    uint8_t        *prov_static_oob_val;
    /** Provisioner static oob value length */
    uint8_t         prov_static_oob_len;

    /** Callback used to notify to input number/string. Initialized by the stack. */
    esp_ble_mesh_cb_t provisioner_prov_input;
    /** Callback used to output number/string. Initialized by the stack. */
    esp_ble_mesh_cb_t provisioner_prov_output;

    /** Key refresh and IV update flag */
    uint8_t        flags;

    /** IV index */
    uint32_t       iv_index;

    /** Callback used to indicate that link is opened. Initialized by the stack. */
    esp_ble_mesh_cb_t  provisioner_link_open;
    /** Callback used to indicate that link is closed. Initialized by the stack. */
    esp_ble_mesh_cb_t  provisioner_link_close;
    /** Callback used to indicate that a device is provisioned. Initialized by the stack. */
    esp_ble_mesh_cb_t  provisioner_prov_comp;
#endif /* CONFIG_BLE_MESH_PROVISIONER */
} esp_ble_mesh_prov_t;

/** Node Composition data context.
 *  This structure is associated with struct bt_mesh_comp in mesh_access.h
 */
typedef struct {
    uint16_t cid;   /*!< 16-bit SIG-assigned company identifier */
    uint16_t pid;   /*!< 16-bit vendor-assigned product identifier */
    uint16_t vid;   /*!< 16-bit vendor-assigned product version identifier */

    size_t element_count;           /*!< Element count */
    esp_ble_mesh_elem_t *elements;  /*!< A sequence of elements */
} esp_ble_mesh_comp_t;

/*!< This enum value is the role of the device */
typedef enum {
    ROLE_NODE = 0,
    ROLE_PROVISIONER,
    ROLE_FAST_PROV,
} esp_ble_mesh_dev_role_t;

/*!< Flag which will be set when device is going to be added. */
typedef uint8_t esp_ble_mesh_dev_add_flag_t;
#define ADD_DEV_RM_AFTER_PROV_FLAG  BIT(0)  /*!< Device will be removed from queue after provisioned successfully */
#define ADD_DEV_START_PROV_NOW_FLAG BIT(1)  /*!< Start provisioning device immediately */
#define ADD_DEV_FLUSHABLE_DEV_FLAG  BIT(2)  /*!< Device can be remove when queue is full and new device is going to added */

/** Information of the device which is going to be added for provisioning. */
typedef struct {
    esp_ble_mesh_bd_addr_t addr;                 /*!< Device address */
    esp_ble_mesh_addr_type_t addr_type;      /*!< Device address type */
    uint8_t  uuid[16];                  /*!< Device UUID */
    uint16_t oob_info;                  /*!< Device OOB Info */
    /*!< ADD_DEV_START_PROV_NOW_FLAG shall not be set if the bearer has both PB-ADV and PB-GATT enabled */
    esp_ble_mesh_prov_bearer_t bearer;  /*!< Provisioning Bearer */
} esp_ble_mesh_unprov_dev_add_t;

#define DEL_DEV_ADDR_FLAG BIT(0)
#define DEL_DEV_UUID_FLAG BIT(1)
/** Information of the device which is going to be deleted. */
typedef struct {
    union {
        struct {
            esp_ble_mesh_bd_addr_t addr;             /*!< Device address */
            esp_ble_mesh_addr_type_t addr_type;  /*!< Device address type */
        };
        uint8_t uuid[16];                   /*!< Device UUID */
    };
    uint8_t flag;                           /*!< BIT0: device address; BIT1: device UUID */
} esp_ble_mesh_device_delete_t;

#define PROV_DATA_NET_IDX_FLAG  BIT(0)
#define PROV_DATA_FLAGS_FLAG    BIT(1)
#define PROV_DATA_IV_INDEX_FLAG BIT(2)
/** Information of the provisioner which is going to be updated. */
typedef struct {
    union {
        uint16_t net_idx;   /*!< NetKey Index */
        uint8_t  flags;     /*!< Flags */
        uint32_t iv_index;  /*!< IV Index */
    };
    uint8_t flag;           /*!< BIT0: net_idx; BIT1: flags; BIT2: iv_index */
} esp_ble_mesh_prov_data_info_t;

/** Context of fast provisioning which need to be set. */
typedef struct {
    uint16_t unicast_min;   /*!< Minimum unicast address used for fast provisioning */
    uint16_t unicast_max;   /*!< Maximum unicast address used for fast provisioning */
    uint16_t net_idx;       /*!< Netkey index used for fast provisioning */
    uint8_t  flags;         /*!< Flags used for fast provisioning */
    uint32_t iv_index;      /*!< IV Index used for fast provisioning */
    uint8_t  offset;        /*!< Offset of the UUID to be compared */
    uint8_t  match_len;     /*!< Length of the UUID to be compared */
    uint8_t  match_val[16]; /*!< Value of UUID to be compared */
} esp_ble_mesh_fast_prov_info_t;

/*!< This enum value is the action of fast provisioning */
typedef enum {
    FAST_PROV_ACT_NONE,
    FAST_PROV_ACT_ENTER,
    FAST_PROV_ACT_SUSPEND,
    FAST_PROV_ACT_EXIT,
    FAST_PROV_ACT_MAX,
} esp_ble_mesh_fast_prov_action_t;

/*!< This enum value is the type of proxy filter */
typedef enum {
    PROXY_FILTER_WHITELIST,
    PROXY_FILTER_BLACKLIST,
} esp_ble_mesh_proxy_filter_type_t;

/*!< This enum value is the event of node/provisioner/fast provisioning */
typedef enum {
    ESP_BLE_MESH_PROV_REGISTER_COMP_EVT,                        /*!< Initialize BLE Mesh provisioning capabilities and internal data information completion event */
    ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT,             /*!< Set the unprovisioned device name completion event */
    ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT,                     /*!< Enable node provisioning functionality completion event */
    ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT,                    /*!< Disable node provisioning functionality completion event */
    ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT,                       /*!< Establish a BLE Mesh link event */
    ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT,                      /*!< Close a BLE Mesh link event */
    ESP_BLE_MESH_NODE_PROV_OOB_PUB_KEY_EVT,                     /*!< Generate Node input OOB public key event */
    ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT,                   /*!< Generate Node Output Number event */
    ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT,                   /*!< Generate Node Output String event */
    ESP_BLE_MESH_NODE_PROV_INPUT_EVT,                           /*!< Event requiring the user to input a number or string */
    ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT,                        /*!< Provisioning done event */
    ESP_BLE_MESH_NODE_PROV_RESET_EVT,                           /*!< Provisioning reset event */
    ESP_BLE_MESH_NODE_PROV_SET_OOB_PUB_KEY_COMP_EVT,            /*!< Node set oob public key completion event */
    ESP_BLE_MESH_NODE_PROV_INPUT_NUMBER_COMP_EVT,               /*!< Node input number completion event */
    ESP_BLE_MESH_NODE_PROV_INPUT_STRING_COMP_EVT,               /*!< Node input string completion event */
    ESP_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT,           /*!< Enable BLE Mesh Proxy Identity advertising completion event */
    ESP_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT,               /*!< Enable BLE Mesh GATT Proxy Service completion event */
    ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT,              /*!< Disable BLE Mesh GATT Proxy Service completion event */
    ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT,              /*!< Provisioner enable provisioning functionality completion event */
    ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT,             /*!< Provisioner disable provisioning functionality completion event */
    ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT,           /*!< Provisioner receives unprovisioned device beacon event */
    ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_EVT,         /*!< Provisioner read unprovisioned device OOB public key event */
    ESP_BLE_MESH_PROVISIONER_PROV_INPUT_EVT,                    /*!< Provisioner input value for provisioning procedure event */
    ESP_BLE_MESH_PROVISIONER_PROV_OUTPUT_EVT,                   /*!< Provisioner output value for provisioning procedure event */
    ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT,                /*!< Provisioner establish a BLE Mesh link event */
    ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT,               /*!< Provisioner close a BLE Mesh link event */
    ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT,                 /*!< Provisioner provisioning done event */
    ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT,           /*!< Provisioner add a device to the list which contains devices that are waiting/going to be provisioned completion event */
    ESP_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT,               /*!< Provisioner delete a device from the list, close provisioning link with the device if it exists and remove the device from network completion event */
    ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT,       /*!< Provisioner set the value to be compared with part of the unprovisioned device UUID completion event */
    ESP_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT,       /*!< Provisioner set net_idx/flags/iv_index used for provisioning completion event */
    ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT,    /*!< Provisioner read unprovisioned device OOB public key completion event */
    ESP_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT,        /*!< Provisioner input number completion event */
    ESP_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT,        /*!< Provisioner input string completion event */
    ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT,            /*!< Provisioner set node name completion event */
    ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT,        /*!< Provisioner add local app key completion event */
    ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT,    /*!< Provisioner bind local model with local app key completion event */
    ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT,        /*!< Provisioner add local network key completion event */
    ESP_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT,                   /*!< Set fast provisioning information (e.g. unicast address range, net_idx, etc.) completion event */
    ESP_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT,                 /*!< Set fast provisioning action completion event */
    ESP_BLE_MESH_HEARTBEAT_MESSAGE_RECV_EVT,                    /*!< Receive Heartbeat message event */
    ESP_BLE_MESH_LPN_ENABLE_COMP_EVT,                           /*!< Enable Low Power Node completion event */
    ESP_BLE_MESH_LPN_DISABLE_COMP_EVT,                          /*!< Disable Low Power Node completion event */
    ESP_BLE_MESH_LPN_POLL_COMP_EVT,                             /*!< Low Power Node send Friend Poll completion event */
    ESP_BLE_MESH_LPN_FRIENDSHIP_ESTABLISH_EVT,                  /*!< Low Power Node establishes friendship event */
    ESP_BLE_MESH_LPN_FRIENDSHIP_TERMINATE_EVT,                  /*!< Low Power Node terminates friendship event */
    ESP_BLE_MESH_FRIEND_FRIENDSHIP_ESTABLISH_EVT,               /*!< Friend Node establishes friendship event */
    ESP_BLE_MESH_FRIEND_FRIENDSHIP_TERMINATE_EVT,               /*!< Friend Node terminates friendship event */
    ESP_BLE_MESH_PROXY_CLIENT_RECV_ADV_PKT_EVT,                 /*!< Proxy Client receives Network ID advertising packet event */
    ESP_BLE_MESH_PROXY_CLIENT_CONNECTED_EVT,                    /*!< Proxy Client establishes connection successfully event */
    ESP_BLE_MESH_PROXY_CLIENT_DISCONNECTED_EVT,                 /*!< Proxy Client terminates connection successfully event */
    ESP_BLE_MESH_PROXY_CLIENT_RECV_FILTER_STATUS_EVT,           /*!< Proxy Client receives Proxy Filter Status event */
    ESP_BLE_MESH_PROXY_CLIENT_CONNECT_COMP_EVT,                 /*!< Proxy Client connect completion event */
    ESP_BLE_MESH_PROXY_CLIENT_DISCONNECT_COMP_EVT,              /*!< Proxy Client disconnect completion event */
    ESP_BLE_MESH_PROXY_CLIENT_SET_FILTER_TYPE_COMP_EVT,         /*!< Proxy Client set filter type completion event */
    ESP_BLE_MESH_PROXY_CLIENT_ADD_FILTER_ADDR_COMP_EVT,         /*!< Proxy Client add filter address completion event */
    ESP_BLE_MESH_PROXY_CLIENT_REMOVE_FILTER_ADDR_COMP_EVT,      /*!< Proxy Client remove filter address completion event */
    ESP_BLE_MESH_PROV_EVT_MAX,
} esp_ble_mesh_prov_cb_event_t;

/**
 * @brief BLE Mesh Node/Provisioner callback parameters union
 */
typedef union {
    /**
     * @brief ESP_BLE_MESH_PROV_REGISTER_COMP_EVT
     */
    struct ble_mesh_prov_register_comp_param {
        int err_code;                           /*!< Indicate the result of BLE Mesh initialization */
    } prov_register_comp;                       /*!< Event parameter of ESP_BLE_MESH_PROV_REGISTER_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT
     */
    struct ble_mesh_set_unprov_dev_name_comp_param {
        int err_code;                           /*!< Indicate the result of setting BLE Mesh device name */
    } node_set_unprov_dev_name_comp;            /*!< Event parameter of ESP_BLE_MESH_NODE_SET_UNPROV_DEV_NAME_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT
     */
    struct ble_mesh_prov_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling BLE Mesh device */
    } node_prov_enable_comp;                    /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_ENABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT
     */
    struct ble_mesh_prov_disable_comp_param {
        int err_code;                           /*!< Indicate the result of disabling BLE Mesh device */
    } node_prov_disable_comp;                   /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_DISABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT
     */
    struct ble_mesh_link_open_evt_param {
        esp_ble_mesh_prov_bearer_t bearer;      /*!< Type of the bearer used when device link is open */
    } node_prov_link_open;                      /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_LINK_OPEN_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT
     */
    struct ble_mesh_link_close_evt_param {
        esp_ble_mesh_prov_bearer_t bearer;      /*!< Type of the bearer used when device link is closed */
    } node_prov_link_close;                     /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_LINK_CLOSE_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT
     */
    struct ble_mesh_output_num_evt_param {
        esp_ble_mesh_output_action_t action;    /*!< Action of Output OOB Authentication */
        uint32_t number;                        /*!< Number of Output OOB Authentication  */
    } node_prov_output_num;                     /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_OUTPUT_NUMBER_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT
     */
    struct ble_mesh_output_str_evt_param {
        char string[8];                         /*!< String of Output OOB Authentication */
    } node_prov_output_str;                     /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_OUTPUT_STRING_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_INPUT_EVT
     */
    struct ble_mesh_input_evt_param {
        esp_ble_mesh_input_action_t action;     /*!< Action of Input OOB Authentication */
        uint8_t size;                           /*!< Size of Input OOB Authentication */
    } node_prov_input;                          /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_INPUT_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT
     */
    struct ble_mesh_provision_complete_evt_param {
        uint16_t net_idx;                       /*!< NetKey Index */
        uint8_t  net_key[16];                   /*!< NetKey */
        uint16_t addr;                          /*!< Primary address */
        uint8_t  flags;                         /*!< Flags */
        uint32_t iv_index;                      /*!< IV Index */
    } node_prov_complete;                       /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_COMPLETE_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_RESET_EVT
     */
    struct ble_mesh_provision_reset_param {

    } node_prov_reset;                          /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_RESET_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_SET_OOB_PUB_KEY_COMP_EVT
     */
    struct ble_mesh_set_oob_pub_key_comp_param {
        int err_code;                           /*!< Indicate the result of setting OOB Public Key */
    } node_prov_set_oob_pub_key_comp;           /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_SET_OOB_PUB_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_INPUT_NUM_COMP_EVT
     */
    struct ble_mesh_input_number_comp_param {
        int err_code;                           /*!< Indicate the result of inputting number */
    } node_prov_input_num_comp;                 /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_INPUT_NUM_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROV_INPUT_STR_COMP_EVT
     */
    struct ble_mesh_input_string_comp_param {
        int err_code;                           /*!< Indicate the result of inputting string */
    } node_prov_input_str_comp;                 /*!< Event parameter of ESP_BLE_MESH_NODE_PROV_INPUT_STR_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT
     */
    struct ble_mesh_proxy_identity_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling Mesh Proxy advertising */
    } node_proxy_identity_enable_comp;          /*!< Event parameter of ESP_BLE_MESH_NODE_PROXY_IDENTITY_ENABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT
     */
    struct ble_mesh_proxy_gatt_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling Mesh Proxy Service */
    } node_proxy_gatt_enable_comp;              /*!< Event parameter of ESP_BLE_MESH_NODE_PROXY_GATT_ENABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT
     */
    struct ble_mesh_proxy_gatt_disable_comp_param {
        int err_code;                           /*!< Indicate the result of disabling Mesh Proxy Service */
    } node_proxy_gatt_disable_comp;             /*!< Event parameter of ESP_BLE_MESH_NODE_PROXY_GATT_DISABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT
     */
    struct ble_mesh_provisioner_recv_unprov_adv_pkt_param {
        uint8_t  dev_uuid[16];                  /*!< Device UUID of the unprovisoned device */
        esp_ble_mesh_bd_addr_t addr;            /*!< Device address of the unprovisoned device */
        esp_ble_mesh_addr_type_t addr_type;     /*!< Device address type */
        uint16_t oob_info;                      /*!< OOB Info of the unprovisoned device */
        uint8_t  adv_type;                      /*!< Avertising type of the unprovisoned device */
        esp_ble_mesh_prov_bearer_t bearer;      /*!< Bearer of the unprovisoned device */
    } provisioner_recv_unprov_adv_pkt;          /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling BLE Mesh Provisioner */
    } provisioner_prov_enable_comp;             /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_ENABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_disable_comp_param {
        int err_code;                           /*!< Indicate the result of disabling BLE Mesh Provisioner */
    } provisioner_prov_disable_comp;            /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_DISABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT
     */
    struct ble_mesh_provisioner_link_open_evt_param {
        esp_ble_mesh_prov_bearer_t bearer;      /*!< Type of the bearer used when Provisioner link is opened */
    } provisioner_prov_link_open;               /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_EVT
     */
    struct ble_mesh_provisioner_prov_read_oob_pub_key_evt_param {
        uint8_t link_idx;                       /*!< Index of the provisioning link */
    } provisioner_prov_read_oob_pub_key;        /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_INPUT_EVT
     */
    struct ble_mesh_provisioner_prov_input_evt_param {
        esp_ble_mesh_oob_method_t method;       /*!< Method of device Output OOB Authentication */
        esp_ble_mesh_output_action_t action;    /*!< Action of device Output OOB Authentication */
        uint8_t size;                           /*!< Size of device Output OOB Authentication */
        uint8_t link_idx;                       /*!< Index of the provisioning link */
    } provisioner_prov_input;                   /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_INPUT_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_OUTPUT_EVT
     */
    struct ble_mesh_provisioner_prov_output_evt_param {
        esp_ble_mesh_oob_method_t method;       /*!< Method of device Input OOB Authentication */
        esp_ble_mesh_input_action_t action;     /*!< Action of device Input OOB Authentication */
        uint8_t size;                           /*!< Size of device Input OOB Authentication */
        uint8_t link_idx;                       /*!< Index of the provisioning link */
        union {
            char string[8];                     /*!< String output by the Provisioner */
            uint32_t number;                    /*!< Number output by the Provisioner */
        };
    } provisioner_prov_output;                  /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_OUTPUT_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT
     */
    struct ble_mesh_provisioner_link_close_evt_param {
        esp_ble_mesh_prov_bearer_t bearer;      /*!< Type of the bearer used when Provisioner link is closed */
        uint8_t reason;                         /*!< Reason of the closed provisioning link */
    } provisioner_prov_link_close;              /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT
     */
    struct ble_mesh_provisioner_prov_comp_param {
        int node_idx;                           /*!< Index of the provisioned device */
        esp_ble_mesh_octet16_t device_uuid;     /*!< Device UUID of the provisioned device */
        uint16_t unicast_addr;                  /*!< Primary address of the provisioned device */
        uint8_t element_num;                    /*!< Element count of the provisioned device */
        uint16_t netkey_idx;                    /*!< NetKey Index of the provisioned device */
    } provisioner_prov_complete;                /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT
     */
    struct ble_mesh_provisioner_add_unprov_dev_comp_param {
        int err_code;                           /*!< Indicate the result of adding device into queue by the Provisioner */
    } provisioner_add_unprov_dev_comp;          /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_ADD_UNPROV_DEV_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT
     */
    struct ble_mesh_provisioner_delete_dev_comp_param {
        int err_code;                           /*!< Indicate the result of deleting device by the Provisioner */
    } provisioner_delete_dev_comp;              /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT
     */
    struct ble_mesh_provisioner_set_dev_uuid_match_comp_param {
        int err_code;                           /*!< Indicate the result of setting Device UUID match value by the Provisioner */
    } provisioner_set_dev_uuid_match_comp;      /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT
     */
    struct ble_mesh_provisioner_set_prov_data_info_comp_param {
        int err_code;                           /*!< Indicate the result of setting provisioning info by the Provisioner */
    } provisioner_set_prov_data_info_comp;      /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_read_oob_pub_key_comp_param {
        int err_code;                           /*!< Indicate the result of setting OOB Public Key by the Provisioner */
    } provisioner_prov_read_oob_pub_key_comp;   /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_input_num_comp_param {
        int err_code;                           /*!< Indicate the result of inputting number by the Provisioner */
    } provisioner_prov_input_num_comp;          /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_input_str_comp_param {
        int err_code;                           /*!< Indicate the result of inputting string by the Provisioner */
    } provisioner_prov_input_str_comp;          /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT
     */
    struct ble_mesh_provisioner_set_node_name_comp_param {
        int err_code;                           /*!< Indicate the result of setting provisioned device name by the Provisioner */
        int node_index;                         /*!< Index of the provisioned device */
    } provisioner_set_node_name_comp;           /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_add_local_app_key_comp_param {
        int err_code;                           /*!< Indicate the result of adding local AppKey by the Provisioner */
        uint16_t app_idx;                       /*!< AppKey Index */
    } provisioner_add_app_key_comp;             /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT
     */
    struct ble_mesh_provisioner_bind_local_mod_app_comp_param {
        int err_code;                           /*!< Indicate the result of binding AppKey with model by the Provisioner */
    } provisioner_bind_app_key_to_model_comp;   /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_add_local_net_key_comp_param {
        int err_code;                           /*!< Indicate the result of adding local NetKey by the Provisioner */
        uint16_t net_idx;                       /*!< NetKey Index */
    } provisioner_add_net_key_comp;             /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT
     */
    struct ble_mesh_set_fast_prov_info_comp_param {
        uint8_t status_unicast;                 /*!< Indicate the result of setting unicast address range of fast provisioning */
        uint8_t status_net_idx;                 /*!< Indicate the result of setting NetKey Index of fast provisioning */
        uint8_t status_match;                   /*!< Indicate the result of setting matching Device UUID of fast provisioning */
    } set_fast_prov_info_comp;                  /*!< Event parameter of ESP_BLE_MESH_SET_FAST_PROV_INFO_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT
     */
    struct ble_mesh_set_fast_prov_action_comp_param {
        uint8_t status_action;                  /*!< Indicate the result of setting action of fast provisioning */
    } set_fast_prov_action_comp;                /*!< Event parameter of ESP_BLE_MESH_SET_FAST_PROV_ACTION_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_HEARTBEAT_MESSAGE_RECV_EVT
     */
    struct ble_mesh_heartbeat_msg_recv_param {
        uint8_t  hops;                          /*!< Heartbeat hops (InitTTL - RxTTL + 1) */
        uint16_t feature;                       /*!< Bit field of currently active features of the node */
    } heartbeat_msg_recv;                       /*!< Event parameter of ESP_BLE_MESH_HEARTBEAT_MESSAGE_RECV_EVT */
    /**
     * @brief ESP_BLE_MESH_LPN_ENABLE_COMP_EVT
     */
    struct ble_mesh_lpn_enable_comp_param {
        int err_code;                           /*!< Indicate the result of enabling LPN functionality */
    } lpn_enable_comp;                          /*!< Event parameter of ESP_BLE_MESH_LPN_ENABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_LPN_DISABLE_COMP_EVT
     */
    struct ble_mesh_lpn_disable_comp_param {
        int err_code;                           /*!< Indicate the result of disabling LPN functionality */
    } lpn_disable_comp;                         /*!< Event parameter of ESP_BLE_MESH_LPN_DISABLE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_LPN_POLL_COMP_EVT
     */
    struct ble_mesh_lpn_poll_comp_param {
        int err_code;                           /*!< Indicate the result of sending Friend Poll */
    } lpn_poll_comp;                            /*!< Event parameter of ESP_BLE_MESH_LPN_POLL_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_LPN_FRIENDSHIP_ESTABLISH_EVT
     */
    struct ble_mesh_lpn_friendship_establish_param {
        uint16_t friend_addr;                   /*!< Friend Node unicast address */
    } lpn_friendship_establish;                 /*!< Event parameter of ESP_BLE_MESH_LPN_FRIENDSHIP_ESTABLISH_EVT */
    /**
     * @brief ESP_BLE_MESH_LPN_FRIENDSHIP_TERMINATE_EVT
     */
    struct ble_mesh_lpn_friendship_terminate_param {
        uint16_t friend_addr;                   /*!< Friend Node unicast address */
    } lpn_friendship_terminate;                 /*!< Event parameter of ESP_BLE_MESH_LPN_FRIENDSHIP_TERMINATE_EVT */
    /**
     * @brief ESP_BLE_MESH_FRIEND_FRIENDSHIP_ESTABLISH_EVT
     */
    struct ble_mesh_friend_friendship_establish_param {
        uint16_t lpn_addr;                      /*!< Low Power Node unciast address */
    } frnd_friendship_establish;                /*!< Event parameter of ESP_BLE_MESH_FRIEND_FRIENDSHIP_ESTABLISH_EVT */
    /**
     * @brief ESP_BLE_MESH_FRIEND_FRIENDSHIP_TERMINATE_EVT
     */
    struct ble_mesh_friend_friendship_terminate_param {
        uint16_t lpn_addr;                      /*!< Low Power Node unicast address */
        /** This enum value is the reason of friendship termination on the friend node side */
        enum {
            ESP_BLE_MESH_FRND_FRIENDSHIP_TERMINATE_ESTABLISH_FAIL,  /*!< Friend Offer has been sent, but Friend Offer is not received within 1 second, friendship fails to be established */
            ESP_BLE_MESH_FRND_FRIENDSHIP_TERMINATE_POLL_TIMEOUT,    /*!< Friendship is established, PollTimeout timer expires and no Friend Poll/Sub Add/Sub Remove is received */
            ESP_BLE_MESH_FRND_FRIENDSHIP_TERMINATE_RECV_FRND_REQ,   /*!< Receive Friend Request from existing Low Power Node */
            ESP_BLE_MESH_FRND_FRIENDSHIP_TERMINATE_RECV_FRND_CLEAR, /*!< Receive Friend Clear from other friend node */
            ESP_BLE_MESH_FRND_FRIENDSHIP_TERMINATE_DISABLE,         /*!< Friend feature disabled or corresponding NetKey is deleted */
        } reason;                               /*!< Friendship terminated reason */
    } frnd_friendship_terminate;                /*!< Event parameter of ESP_BLE_MESH_FRIEND_FRIENDSHIP_TERMINATE_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_RECV_ADV_PKT_EVT
     */
    struct ble_mesh_proxy_client_recv_adv_pkt_param {
        esp_ble_mesh_bd_addr_t addr;            /*!< Device address */
        esp_ble_mesh_addr_type_t addr_type;     /*!< Device address type */
        uint16_t net_idx;                       /*!< Network ID related NetKey Index */
        uint8_t  net_id[8];                     /*!< Network ID contained in the advertising packet */
    } proxy_client_recv_adv_pkt;                /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_RECV_ADV_PKT_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_CONNECTED_EVT
     */
    struct ble_mesh_proxy_client_connected_param {
        esp_ble_mesh_bd_addr_t addr;            /*!< Device address of the Proxy Server */
        esp_ble_mesh_addr_type_t addr_type;     /*!< Device address type */
        uint8_t conn_handle;                    /*!< Proxy connection handle */
        uint16_t net_idx;                       /*!< Corresponding NetKey Index */
    } proxy_client_connected;                   /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_CONNECTED_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_DISCONNECTED_EVT
     */
    struct ble_mesh_proxy_client_disconnected_param {
        esp_ble_mesh_bd_addr_t addr;            /*!< Device address of the Proxy Server */
        esp_ble_mesh_addr_type_t addr_type;     /*!< Device address type */
        uint8_t conn_handle;                    /*!< Proxy connection handle */
        uint16_t net_idx;                       /*!< Corresponding NetKey Index */
        uint8_t reason;                         /*!< Proxy disconnect reason */
    } proxy_client_disconnected;                /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_DISCONNECTED_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_RECV_FILTER_STATUS_EVT
     */
    struct ble_mesh_proxy_client_recv_filter_status_param {
        uint8_t  conn_handle;                   /*!< Proxy connection handle */
        uint16_t server_addr;                   /*!< Proxy Server primary element address */
        uint16_t net_idx;                       /*!< Corresponding NetKey Index */
        uint8_t  filter_type;                   /*!< Proxy Server filter type(whitelist or blacklist) */
        uint16_t list_size;                     /*!< Number of addresses in the Proxy Server filter list */
    } proxy_client_recv_filter_status;          /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_RECV_FILTER_STATUS_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_CONNECT_COMP_EVT
     */
    struct ble_mesh_proxy_client_connect_comp_param {
        int err_code;                           /*!< Indicate the result of Proxy Client connect */
        esp_ble_mesh_bd_addr_t addr;            /*!< Device address of the Proxy Server */
        esp_ble_mesh_addr_type_t addr_type;     /*!< Device address type */
        uint16_t net_idx;                       /*!< Corresponding NetKey Index */
    } proxy_client_connect_comp;                /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_CONNECT_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_DISCONNECT_COMP_EVT
     */
    struct ble_mesh_proxy_client_disconnect_comp_param {
        int err_code;                           /*!< Indicate the result of Proxy Client disconnect */
        uint8_t conn_handle;                    /*!< Proxy connection handle */
    } proxy_client_disconnect_comp;             /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_DISCONNECT_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_SET_FILTER_TYPE_COMP_EVT
     */
    struct ble_mesh_proxy_client_set_filter_type_comp_param {
        int err_code;                           /*!< Indicate the result of Proxy Client set filter type */
        uint8_t conn_handle;                    /*!< Proxy connection handle */
        uint16_t net_idx;                       /*!< Corresponding NetKey Index */
    } proxy_client_set_filter_type_comp;        /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_SET_FILTER_TYPE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_ADD_FILTER_ADDR_COMP_EVT
     */
    struct ble_mesh_proxy_client_add_filter_addr_comp_param {
        int err_code;                           /*!< Indicate the result of Proxy Client add filter address */
        uint8_t conn_handle;                    /*!< Proxy connection handle */
        uint16_t net_idx;                       /*!< Corresponding NetKey Index */
    } proxy_client_add_filter_addr_comp;        /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_ADD_FILTER_ADDR_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_CLIENT_REMOVE_FILTER_ADDR_COMP_EVT
     */
    struct ble_mesh_proxy_client_remove_filter_addr_comp_param {
        int err_code;                           /*!< Indicate the result of Proxy Client remove filter address */
        uint8_t conn_handle;                    /*!< Proxy connection handle */
        uint16_t net_idx;                       /*!< Corresponding NetKey Index */
    } proxy_client_remove_filter_addr_comp;     /*!< Event parameter of ESP_BLE_MESH_PROXY_CLIENT_REMOVE_FILTER_ADDR_COMP_EVT */
} esp_ble_mesh_prov_cb_param_t;

/**
 * @brief BLE Mesh models related Model ID and Opcode definitions
 */

/*!< Foundation Models */
#define ESP_BLE_MESH_MODEL_ID_CONFIG_SRV                            BLE_MESH_MODEL_ID_CFG_SRV
#define ESP_BLE_MESH_MODEL_ID_CONFIG_CLI                            BLE_MESH_MODEL_ID_CFG_CLI
#define ESP_BLE_MESH_MODEL_ID_HEALTH_SRV                            BLE_MESH_MODEL_ID_HEALTH_SRV
#define ESP_BLE_MESH_MODEL_ID_HEALTH_CLI                            BLE_MESH_MODEL_ID_HEALTH_CLI

/*!< Models from the Mesh Model Specification */
#define ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV                         BLE_MESH_MODEL_ID_GEN_ONOFF_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI                         BLE_MESH_MODEL_ID_GEN_ONOFF_CLI
#define ESP_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV                         BLE_MESH_MODEL_ID_GEN_LEVEL_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_LEVEL_CLI                         BLE_MESH_MODEL_ID_GEN_LEVEL_CLI
#define ESP_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV                BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI                BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV                   BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV             BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI                   BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV                   BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV             BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI                   BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI
#define ESP_BLE_MESH_MODEL_ID_GEN_BATTERY_SRV                       BLE_MESH_MODEL_ID_GEN_BATTERY_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_BATTERY_CLI                       BLE_MESH_MODEL_ID_GEN_BATTERY_CLI
#define ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_SRV                      BLE_MESH_MODEL_ID_GEN_LOCATION_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV                BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_CLI                      BLE_MESH_MODEL_ID_GEN_LOCATION_CLI
#define ESP_BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV                    BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV             BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV                     BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV                   BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV
#define ESP_BLE_MESH_MODEL_ID_GEN_PROP_CLI                          BLE_MESH_MODEL_ID_GEN_PROP_CLI
#define ESP_BLE_MESH_MODEL_ID_SENSOR_SRV                            BLE_MESH_MODEL_ID_SENSOR_SRV
#define ESP_BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV                      BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_SENSOR_CLI                            BLE_MESH_MODEL_ID_SENSOR_CLI
#define ESP_BLE_MESH_MODEL_ID_TIME_SRV                              BLE_MESH_MODEL_ID_TIME_SRV
#define ESP_BLE_MESH_MODEL_ID_TIME_SETUP_SRV                        BLE_MESH_MODEL_ID_TIME_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_TIME_CLI                              BLE_MESH_MODEL_ID_TIME_CLI
#define ESP_BLE_MESH_MODEL_ID_SCENE_SRV                             BLE_MESH_MODEL_ID_SCENE_SRV
#define ESP_BLE_MESH_MODEL_ID_SCENE_SETUP_SRV                       BLE_MESH_MODEL_ID_SCENE_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_SCENE_CLI                             BLE_MESH_MODEL_ID_SCENE_CLI
#define ESP_BLE_MESH_MODEL_ID_SCHEDULER_SRV                         BLE_MESH_MODEL_ID_SCHEDULER_SRV
#define ESP_BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV                   BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_SCHEDULER_CLI                         BLE_MESH_MODEL_ID_SCHEDULER_CLI
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV                   BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV             BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI                   BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI
#define ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV                         BLE_MESH_MODEL_ID_LIGHT_CTL_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV                   BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI                         BLE_MESH_MODEL_ID_LIGHT_CTL_CLI
#define ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV                    BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV                         BLE_MESH_MODEL_ID_LIGHT_HSL_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV                   BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI                         BLE_MESH_MODEL_ID_LIGHT_HSL_CLI
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV                     BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV                     BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_SRV                         BLE_MESH_MODEL_ID_LIGHT_XYL_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV                   BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI                         BLE_MESH_MODEL_ID_LIGHT_XYL_CLI
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LC_SRV                          BLE_MESH_MODEL_ID_LIGHT_LC_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV                    BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LC_CLI                          BLE_MESH_MODEL_ID_LIGHT_LC_CLI

/**
 * esp_ble_mesh_opcode_config_client_get_t belongs to esp_ble_mesh_opcode_t, this typedef is only
 * used to locate the opcodes used by esp_ble_mesh_config_client_get_state.
 * The following opcodes will only be used in the esp_ble_mesh_config_client_get_state function.
 */
typedef uint32_t esp_ble_mesh_opcode_config_client_get_t;

#define ESP_BLE_MESH_MODEL_OP_BEACON_GET                            OP_BEACON_GET           /*!< Config Beacon Get */
#define ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET                  OP_DEV_COMP_DATA_GET    /*!< Config Composition Data Get */
#define ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_GET                       OP_DEFAULT_TTL_GET      /*!< Config Default TTL Get */
#define ESP_BLE_MESH_MODEL_OP_GATT_PROXY_GET                        OP_GATT_PROXY_GET       /*!< Config GATT Proxy Get */
#define ESP_BLE_MESH_MODEL_OP_RELAY_GET                             OP_RELAY_GET            /*!< Config Relay Get */
#define ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET                         OP_MOD_PUB_GET          /*!< Config Model Publication Get */
#define ESP_BLE_MESH_MODEL_OP_FRIEND_GET                            OP_FRIEND_GET           /*!< Config Friend Get */
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_GET                     OP_HEARTBEAT_PUB_GET    /*!< Config Heartbeat Publication Get */
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_GET                     OP_HEARTBEAT_SUB_GET    /*!< Config Heartbeat Subscription Get */
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_GET                           OP_NET_KEY_GET          /*!< Config NetKey Get */
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_GET                           OP_APP_KEY_GET          /*!< Config AppKey Get */
#define ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET                     OP_NODE_IDENTITY_GET    /*!< Config Node Identity Get */
#define ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET                     OP_MOD_SUB_GET          /*!< Config SIG Model Subscription Get */
#define ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET                  OP_MOD_SUB_GET_VND      /*!< Config Vendor Model Subscription Get */
#define ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET                     OP_SIG_MOD_APP_GET      /*!< Config SIG Model App Get */
#define ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET                  OP_VND_MOD_APP_GET      /*!< Config Vendor Model App Get */
#define ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET                 OP_KRP_GET              /*!< Config Key Refresh Phase Get */
#define ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET                   OP_LPN_TIMEOUT_GET      /*!< Config Low Power Node PollTimeout Get */
#define ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_GET                  OP_NET_TRANSMIT_GET     /*!< Config Network Transmit Get */

/**
 * esp_ble_mesh_opcode_config_client_set_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by esp_ble_mesh_config_client_set_state.
 * The following opcodes will only be used in the esp_ble_mesh_config_client_set_state function.
 */
typedef uint32_t esp_ble_mesh_opcode_config_client_set_t;

#define ESP_BLE_MESH_MODEL_OP_BEACON_SET                            OP_BEACON_SET           /*!< Config Beacon Set */
#define ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET                       OP_DEFAULT_TTL_SET      /*!< Config Default TTL Set */
#define ESP_BLE_MESH_MODEL_OP_GATT_PROXY_SET                        OP_GATT_PROXY_SET       /*!< Config GATT Proxy Set */
#define ESP_BLE_MESH_MODEL_OP_RELAY_SET                             OP_RELAY_SET            /*!< Config Relay Set */
#define ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET                         OP_MOD_PUB_SET          /*!< Config Model Publication Set */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD                         OP_MOD_SUB_ADD          /*!< Config Model Subscription Add */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD            OP_MOD_SUB_VA_ADD       /*!< Config Model Subscription Vritual Address Add */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE                      OP_MOD_SUB_DEL          /*!< Config Model Subscription Delete */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE         OP_MOD_SUB_VA_DEL       /*!< Config Model Subscription Virtual Address Delete */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE                   OP_MOD_SUB_OVERWRITE    /*!< Config Model Subscription Overwrite */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE      OP_MOD_SUB_VA_OVERWRITE /*!< Config Model Subscription Virtual Address Overwrite */
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD                           OP_NET_KEY_ADD          /*!< Config NetKey Add */
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD                           OP_APP_KEY_ADD          /*!< Config AppKey Add */
#define ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND                        OP_MOD_APP_BIND         /*!< Config Model App Bind */
#define ESP_BLE_MESH_MODEL_OP_NODE_RESET                            OP_NODE_RESET           /*!< Config Node Reset */
#define ESP_BLE_MESH_MODEL_OP_FRIEND_SET                            OP_FRIEND_SET           /*!< Config Friend Set */
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET                     OP_HEARTBEAT_PUB_SET    /*!< Config Heartbeat Publication Set */
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET                     OP_HEARTBEAT_SUB_SET    /*!< Config Heartbeat Subscription Set */
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_UPDATE                        OP_NET_KEY_UPDATE       /*!< Config NetKey Update */
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_DELETE                        OP_NET_KEY_DEL          /*!< Config NetKey Delete */
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_UPDATE                        OP_APP_KEY_UPDATE       /*!< Config AppKey Update */
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_DELETE                        OP_APP_KEY_DEL          /*!< Config AppKey Delete */
#define ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_SET                     OP_NODE_IDENTITY_SET    /*!< Config Node Identity Set */
#define ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_SET                 OP_KRP_SET              /*!< Config Key Refresh Phase Set */
#define ESP_BLE_MESH_MODEL_OP_MODEL_PUB_VIRTUAL_ADDR_SET            OP_MOD_PUB_VA_SET       /*!< Config Model Publication Virtual Address Set */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL                  OP_MOD_SUB_DEL_ALL      /*!< Config Model Subscription Delete All */
#define ESP_BLE_MESH_MODEL_OP_MODEL_APP_UNBIND                      OP_MOD_APP_UNBIND       /*!< Config Model App Unbind */
#define ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_SET                  OP_NET_TRANSMIT_SET     /*!< Config Network Transmit Set */

/**
 * esp_ble_mesh_opcode_config_status_t belongs to esp_ble_mesh_opcode_t, this typedef is only
 * used to locate the opcodes used by the Config Model messages
 * The following opcodes are used by the BLE Mesh Config Server Model internally to respond
 * to the Config Client Model's request messages.
 */
typedef uint32_t esp_ble_mesh_opcode_config_status_t;

#define ESP_BLE_MESH_MODEL_OP_BEACON_STATUS                         OP_BEACON_STATUS
#define ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_STATUS               OP_DEV_COMP_DATA_STATUS
#define ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_STATUS                    OP_DEFAULT_TTL_STATUS
#define ESP_BLE_MESH_MODEL_OP_GATT_PROXY_STATUS                     OP_GATT_PROXY_STATUS
#define ESP_BLE_MESH_MODEL_OP_RELAY_STATUS                          OP_RELAY_STATUS
#define ESP_BLE_MESH_MODEL_OP_MODEL_PUB_STATUS                      OP_MOD_PUB_STATUS
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_STATUS                      OP_MOD_SUB_STATUS
#define ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_LIST                    OP_MOD_SUB_LIST
#define ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_LIST                 OP_MOD_SUB_LIST_VND
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_STATUS                        OP_NET_KEY_STATUS
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_LIST                          OP_NET_KEY_LIST
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_STATUS                        OP_APP_KEY_STATUS
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_LIST                          OP_APP_KEY_LIST
#define ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_STATUS                  OP_NODE_IDENTITY_STATUS
#define ESP_BLE_MESH_MODEL_OP_MODEL_APP_STATUS                      OP_MOD_APP_STATUS
#define ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_LIST                    OP_SIG_MOD_APP_LIST
#define ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_LIST                 OP_VND_MOD_APP_LIST
#define ESP_BLE_MESH_MODEL_OP_NODE_RESET_STATUS                     OP_NODE_RESET_STATUS
#define ESP_BLE_MESH_MODEL_OP_FRIEND_STATUS                         OP_FRIEND_STATUS
#define ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_STATUS              OP_KRP_STATUS
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_STATUS                  OP_HEARTBEAT_PUB_STATUS
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_STATUS                  OP_HEARTBEAT_SUB_STATUS
#define ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_STATUS                OP_LPN_TIMEOUT_STATUS
#define ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_STATUS               OP_NET_TRANSMIT_STATUS

/**
 * This typedef is only used to indicate the status code contained in some of
 * the Configuration Server Model status message.
 */
typedef uint8_t esp_ble_mesh_cfg_status_t;

#define ESP_BLE_MESH_CFG_STATUS_SUCCESS                             0x00
#define ESP_BLE_MESH_CFG_STATUS_INVALID_ADDRESS                     0x01
#define ESP_BLE_MESH_CFG_STATUS_INVALID_MODEL                       0x02
#define ESP_BLE_MESH_CFG_STATUS_INVALID_APPKEY                      0x03
#define ESP_BLE_MESH_CFG_STATUS_INVALID_NETKEY                      0x04
#define ESP_BLE_MESH_CFG_STATUS_INSUFFICIENT_RESOURCES              0x05
#define ESP_BLE_MESH_CFG_STATUS_KEY_INDEX_ALREADY_STORED            0x06
#define ESP_BLE_MESH_CFG_STATUS_INVALID_PUBLISH_PARAMETERS          0x07
#define ESP_BLE_MESH_CFG_STATUS_NOT_A_SUBSCRIBE_MODEL               0x08
#define ESP_BLE_MESH_CFG_STATUS_STORAGE_FAILURE                     0x09
#define ESP_BLE_MESH_CFG_STATUS_FEATURE_NOT_SUPPORTED               0x0A
#define ESP_BLE_MESH_CFG_STATUS_CANNOT_UPDATE                       0x0B
#define ESP_BLE_MESH_CFG_STATUS_CANNOT_REMOVE                       0x0C
#define ESP_BLE_MESH_CFG_STATUS_CANNOT_BIND                         0x0D
#define ESP_BLE_MESH_CFG_STATUS_TEMP_UNABLE_TO_CHANGE_STATE         0x0E
#define ESP_BLE_MESH_CFG_STATUS_CANNOT_SET                          0x0F
#define ESP_BLE_MESH_CFG_STATUS_UNSPECIFIED_ERROR                   0x10
#define ESP_BLE_MESH_CFG_STATUS_INVALID_BINDING                     0x11

/**
 * esp_ble_mesh_opcode_health_client_get_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by esp_ble_mesh_health_client_get_state.
 * The following opcodes will only be used in the esp_ble_mesh_health_client_get_state function.
 */
typedef uint32_t esp_ble_mesh_opcode_health_client_get_t;

#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET                      OP_HEALTH_FAULT_GET         /*!< Health Fault Get */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_GET                     OP_HEALTH_PERIOD_GET        /*!< Health Period Get */
#define ESP_BLE_MESH_MODEL_OP_ATTENTION_GET                         OP_ATTENTION_GET            /*!< Health Attention Get */

/**
 * esp_ble_mesh_opcode_health_client_set_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by esp_ble_mesh_health_client_set_state.
 * The following opcodes will only be used in the esp_ble_mesh_health_client_set_state function.
 */
typedef uint32_t esp_ble_mesh_opcode_health_client_set_t;

#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR                    OP_HEALTH_FAULT_CLEAR       /*!< Health Fault Clear */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK              OP_HEALTH_FAULT_CLEAR_UNREL /*!< Health Fault Clear Unacknowledged */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST                     OP_HEALTH_FAULT_TEST        /*!< Health Fault Test */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK               OP_HEALTH_FAULT_TEST_UNREL  /*!< Health Fault Test Unacknowledged */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET                     OP_HEALTH_PERIOD_SET        /*!< Health Period Set */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK               OP_HEALTH_PERIOD_SET_UNREL  /*!< Health Period Set Unacknowledged */
#define ESP_BLE_MESH_MODEL_OP_ATTENTION_SET                         OP_ATTENTION_SET            /*!< Health Attention Set */
#define ESP_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK                   OP_ATTENTION_SET_UNREL      /*!< Health Attention Set Unacknowledged */

/**
 * esp_ble_mesh_health_model_status_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by the Health Model messages.
 * The following opcodes are used by the BLE Mesh Health Server Model internally to
 * respond to the Health Client Model's request messages.
 */
typedef uint32_t esp_ble_mesh_health_model_status_t;

#define ESP_BLE_MESH_MODEL_OP_HEALTH_CURRENT_STATUS                 OP_HEALTH_CURRENT_STATUS
#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_STATUS                   OP_HEALTH_FAULT_STATUS
#define ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_STATUS                  OP_HEALTH_PERIOD_STATUS
#define ESP_BLE_MESH_MODEL_OP_ATTENTION_STATUS                      OP_ATTENTION_STATUS

/**
 * esp_ble_mesh_generic_message_opcode_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by functions esp_ble_mesh_generic_client_get_state
 * & esp_ble_mesh_generic_client_set_state.
 */
typedef uint32_t esp_ble_mesh_generic_message_opcode_t;

/*!< Generic OnOff Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET                         BLE_MESH_MODEL_OP_GEN_ONOFF_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET                         BLE_MESH_MODEL_OP_GEN_ONOFF_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK                   BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS                      BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS

/*!< Generic Level Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_GET                         BLE_MESH_MODEL_OP_GEN_LEVEL_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET                         BLE_MESH_MODEL_OP_GEN_LEVEL_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK                   BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS                      BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS
#define ESP_BLE_MESH_MODEL_OP_GEN_DELTA_SET                         BLE_MESH_MODEL_OP_GEN_DELTA_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK                   BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_MOVE_SET                          BLE_MESH_MODEL_OP_GEN_MOVE_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK                    BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK

/*!< Generic Default Transition Time Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET                BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET                BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK          BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS             BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS

/*!< Generic Power OnOff Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET                     BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS                  BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS

/*!< Generic Power OnOff Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET                     BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK               BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK

/*!< Generic Power Level Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET                   BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET                   BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK             BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS                BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET                    BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS                 BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET                 BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS              BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET                   BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS                BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS

/*!< Generic Power Level Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET                 BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK           BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET                   BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK             BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK

/*!< Generic Battery Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_BATTERY_GET                       BLE_MESH_MODEL_OP_GEN_BATTERY_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS                    BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS

/*!< Generic Location Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET                    BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS                 BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET                     BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS                  BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS

/*!< Generic Location Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET                    BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK              BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET                     BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK               BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK

/*!< Generic Manufacturer Property Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_GET       BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_STATUS    BLE_MESH_MODEL_OP_GEN_MANU_PROPERTIES_STATUS
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_GET         BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET         BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET_UNACK   BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_STATUS      BLE_MESH_MODEL_OP_GEN_MANU_PROPERTY_STATUS

/*!< Generic Admin Property Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET              BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS           BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET                BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET                BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK          BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS             BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS

/*!< Generic User Property Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET               BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS            BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET                 BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET                 BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK           BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS              BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS

/*!< Generic Client Property Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET             BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET
#define ESP_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS          BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS

/**
 * esp_ble_mesh_sensor_message_opcode_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by functions esp_ble_mesh_sensor_client_get_state
 * & esp_ble_mesh_sensor_client_set_state.
 */
typedef uint32_t esp_ble_mesh_sensor_message_opcode_t;

/*!< Sensor Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET                 BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS              BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS
#define ESP_BLE_MESH_MODEL_OP_SENSOR_GET                            BLE_MESH_MODEL_OP_SENSOR_GET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_STATUS                         BLE_MESH_MODEL_OP_SENSOR_STATUS
#define ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET                     BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS                  BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET                     BLE_MESH_MODEL_OP_SENSOR_SERIES_GET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS                  BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS

/*!< Sensor Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET                    BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET                    BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK              BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS                 BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET                   BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS                BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_GET                    BLE_MESH_MODEL_OP_SENSOR_SETTING_GET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET                    BLE_MESH_MODEL_OP_SENSOR_SETTING_SET
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK              BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS                 BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS

/**
 * esp_ble_mesh_time_scene_message_opcode_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by functions esp_ble_mesh_time_scene_client_get_state
 * & esp_ble_mesh_time_scene_client_set_state.
 */
typedef uint32_t esp_ble_mesh_time_scene_message_opcode_t;

/*!< Time Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_TIME_GET                              BLE_MESH_MODEL_OP_TIME_GET
#define ESP_BLE_MESH_MODEL_OP_TIME_SET                              BLE_MESH_MODEL_OP_TIME_SET
#define ESP_BLE_MESH_MODEL_OP_TIME_STATUS                           BLE_MESH_MODEL_OP_TIME_STATUS
#define ESP_BLE_MESH_MODEL_OP_TIME_ROLE_GET                         BLE_MESH_MODEL_OP_TIME_ROLE_GET
#define ESP_BLE_MESH_MODEL_OP_TIME_ROLE_SET                         BLE_MESH_MODEL_OP_TIME_ROLE_SET
#define ESP_BLE_MESH_MODEL_OP_TIME_ROLE_STATUS                      BLE_MESH_MODEL_OP_TIME_ROLE_STATUS
#define ESP_BLE_MESH_MODEL_OP_TIME_ZONE_GET                         BLE_MESH_MODEL_OP_TIME_ZONE_GET
#define ESP_BLE_MESH_MODEL_OP_TIME_ZONE_SET                         BLE_MESH_MODEL_OP_TIME_ZONE_SET
#define ESP_BLE_MESH_MODEL_OP_TIME_ZONE_STATUS                      BLE_MESH_MODEL_OP_TIME_ZONE_STATUS
#define ESP_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET                     BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET
#define ESP_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET                     BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET
#define ESP_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS                  BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS

/*!< Scene Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SCENE_GET                             BLE_MESH_MODEL_OP_SCENE_GET
#define ESP_BLE_MESH_MODEL_OP_SCENE_RECALL                          BLE_MESH_MODEL_OP_SCENE_RECALL
#define ESP_BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK                    BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK
#define ESP_BLE_MESH_MODEL_OP_SCENE_STATUS                          BLE_MESH_MODEL_OP_SCENE_STATUS
#define ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_GET                    BLE_MESH_MODEL_OP_SCENE_REGISTER_GET
#define ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS                 BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS

/*!< Scene Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SCENE_STORE                           BLE_MESH_MODEL_OP_SCENE_STORE
#define ESP_BLE_MESH_MODEL_OP_SCENE_STORE_UNACK                     BLE_MESH_MODEL_OP_SCENE_STORE_UNACK
#define ESP_BLE_MESH_MODEL_OP_SCENE_DELETE                          BLE_MESH_MODEL_OP_SCENE_DELETE
#define ESP_BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK                    BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK

/*!< Scheduler Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET                     BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS                  BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_GET                         BLE_MESH_MODEL_OP_SCHEDULER_GET
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_STATUS                      BLE_MESH_MODEL_OP_SCHEDULER_STATUS

/*!< Scheduler Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET                     BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK               BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK

/**
 * esp_ble_mesh_light_message_opcode_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by functions esp_ble_mesh_light_client_get_state
 * & esp_ble_mesh_light_client_set_state.
 */
typedef uint32_t esp_ble_mesh_light_message_opcode_t;

/*!< Light Lightness Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET                   BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET                   BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK             BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS                BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET            BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET            BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK      BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS         BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET              BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS           BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET           BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS        BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET             BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS          BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS

/*!< Light Lightness Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET           BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK     BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET             BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK       BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK

/*!< Light CTL Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_GET                         BLE_MESH_MODEL_OP_LIGHT_CTL_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET                         BLE_MESH_MODEL_OP_LIGHT_CTL_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK                   BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS                      BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET             BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET       BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS    BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET             BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK       BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS          BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET                 BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS              BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS

/*!< Light CTL Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET                 BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK           BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET       BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK

/*!< Light HSL Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_GET                         BLE_MESH_MODEL_OP_LIGHT_HSL_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET                     BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET                     BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK               BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS                  BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET              BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET              BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK        BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS           BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET                         BLE_MESH_MODEL_OP_LIGHT_HSL_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK                   BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS                      BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET                  BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS               BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET                 BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS              BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET                   BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS                BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS

/*!< Light HSL Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET                 BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK           BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET                   BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK             BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK               /* Model spec is wrong */

/*!< Light xyL Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_GET                         BLE_MESH_MODEL_OP_LIGHT_XYL_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_SET                         BLE_MESH_MODEL_OP_LIGHT_XYL_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK                   BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS                      BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET                  BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS               BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET                 BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS              BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET                   BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS                BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS

/*!< Light xyL Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET                 BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK           BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET                   BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK             BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK

/*!< Light Control Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET                     BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET                     BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK               BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS                  BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET                       BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET                       BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK                 BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS                    BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET              BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET              BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK        BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS           BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET                 BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET                 BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK           BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS              BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS

typedef uint32_t esp_ble_mesh_opcode_t;
/*!< End of defines of esp_ble_mesh_opcode_t */

/**
 * This typedef is only used to indicate the status code contained in some of the
 * server models (e.g. Generic Server Model) status message.
 */
typedef uint8_t esp_ble_mesh_model_status_t;

#define ESP_BLE_MESH_MODEL_STATUS_SUCCESS                           0x00
#define ESP_BLE_MESH_MODEL_STATUS_CANNOT_SET_RANGE_MIN              0x01
#define ESP_BLE_MESH_MODEL_STATUS_CANNOT_SET_RANGE_MAX              0x02

/**
 * @brief BLE Mesh client models related definitions
 */

/** Client model Get/Set message opcode and corresponding Status message opcode */
typedef struct {
    uint32_t cli_op;        /*!< The client message opcode */
    uint32_t status_op;     /*!< The server status opcode corresponding to the client message opcode */
} esp_ble_mesh_client_op_pair_t;

/** Client Model user data context. */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the client model. Initialized by the stack. */
    int op_pair_size;                               /*!< Size of the op_pair */
    const esp_ble_mesh_client_op_pair_t *op_pair;   /*!< Table containing get/set message opcode and corresponding status message opcode */
    uint32_t publish_status;                        /*!< Callback used to handle the received unsoliciated message. Initialized by the stack. */
    void *internal_data;                            /*!< Pointer to the internal data of client model */
    uint8_t msg_role;                               /*!< Role of the device (Node/Provisioner) that is going to send messages */
} esp_ble_mesh_client_t;

/** Common parameters of the messages sent by Client Model. */
typedef struct {
    esp_ble_mesh_opcode_t opcode;   /*!< Message opcode */
    esp_ble_mesh_model_t *model;    /*!< Pointer to the client model structure */
    esp_ble_mesh_msg_ctx_t ctx;     /*!< The context used to send message */
    int32_t msg_timeout;            /*!< Timeout value (ms) to get response to the sent message */
    /*!< Note: if using default timeout value in menuconfig, make sure to set this value to 0 */
    uint8_t msg_role;               /*!< Role of the device - Node/Provisioner */
} esp_ble_mesh_client_common_param_t;

/**
 * @brief BLE Mesh server models related definitions
 */

/** This enum value is the flag of transition timer operation */
enum {
    ESP_BLE_MESH_SERVER_TRANS_TIMER_START,  /* Proper transition timer has been started */
    ESP_BLE_MESH_SERVER_FLAG_MAX,
};

/** Parameters of the server model state transition */
typedef struct {
    bool just_started;          /*!< Indicate if the state transition has just started */

    uint8_t  trans_time;        /*!< State transition time */
    uint8_t  remain_time;       /*!< Remaining time of state transition */
    uint8_t  delay;             /*!< Delay before starting state transition */
    uint32_t quo_tt;            /*!< Duration of each divided transition step */
    uint32_t counter;           /*!< Number of steps which the transition duration is divided */
    uint32_t total_duration;    /*!< State transition total duration */
    int64_t  start_timestamp;   /*!< Time when the state transition is started */

    /**
     * Flag used to indicate if the transition timer has been started internally.
     *
     * If the model which contains esp_ble_mesh_state_transition_t sets "set_auto_rsp"
     * to ESP_BLE_MESH_SERVER_RSP_BY_APP, the handler of the timer shall be initialized
     * by the users.
     *
     * And users can use this flag to indicate whether the timer is started or not.
     */
    BLE_MESH_ATOMIC_DEFINE(flag, ESP_BLE_MESH_SERVER_FLAG_MAX);
    struct k_delayed_work timer;    /*!< Timer used for state transition */
} esp_ble_mesh_state_transition_t;

/** Parameters of the server model received last same set message. */
typedef struct {
    uint8_t  tid;       /*!< Transaction number of the last message */
    uint16_t src;       /*!< Source address of the last message */
    uint16_t dst;       /*!< Destination address of the last messgae */
    int64_t  timestamp; /*!< Time when the last message is received */
} esp_ble_mesh_last_msg_info_t;

#define ESP_BLE_MESH_SERVER_RSP_BY_APP  0   /*!< Response will be sent internally */
#define ESP_BLE_MESH_SERVER_AUTO_RSP    1   /*!< Response need to be sent in the application */

/** Parameters of the Server Model response control */
typedef struct {
    /**
     * @brief BLE Mesh Server Response Option
     *        1. If get_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, then the
     *           response of Client Get messages need to be replied by the application;
     *        2. If get_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, then the
     *           response of Client Get messages will be replied by the server models;
     *        3. If set_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, then the
     *           response of Client Set messages need to be replied by the application;
     *        4. If set_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, then the
     *           response of Client Set messages will be replied by the server models;
     *        5. If status_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, then the
     *           response of Server Status messages need to be replied by the application;
     *        6. If status_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, then the
     *           response of Server Status messages will be replied by the server models;
     */
    uint8_t get_auto_rsp : 1,       /*!< Response control for Client Get messages */
            set_auto_rsp : 1,       /*!< Response control for Client Set messages */
            status_auto_rsp : 1;    /*!< Response control for Server Status messages */
} esp_ble_mesh_server_rsp_ctrl_t;

/**
 * @brief Server model state value union
 */
typedef union {
    struct {
        uint8_t onoff;          /*!< The value of the Generic OnOff state */
    } gen_onoff;                /*!< The Generic OnOff state */
    struct {
        int16_t level;          /*!< The value of the Generic Level state */
    } gen_level;                /*!< The Generic Level state */
    struct {
        uint8_t onpowerup;      /*!< The value of the Generic OnPowerUp state */
    } gen_onpowerup;            /*!< The Generic OnPowerUp state */
    struct {
        uint16_t power;         /*!< The value of the Generic Power Actual state */
    } gen_power_actual;         /*!< The Generic Power Actual state */
    struct {
        uint16_t lightness;     /*!< The value of the Light Lightness Actual state */
    } light_lightness_actual;   /*!< The Light Lightness Actual state */
    struct {
        uint16_t lightness;     /*!< The value of the Light Lightness Linear state */
    } light_lightness_linear;   /*!< The Light Lightness Linear state */
    struct {
        uint16_t lightness;     /*!< The value of the Light CTL Lightness state */
    } light_ctl_lightness;      /*!< The Light CTL Lightness state */
    struct {
        uint16_t temperature;   /*!< The value of the Light CTL Temperature state */
        int16_t  delta_uv;      /*!< The value of the Light CTL Delta UV state */
    } light_ctl_temp_delta_uv;  /*!< The Light CTL Temperature & Delta UV states */
    struct {
        uint16_t lightness;     /*!< The value of the Light HSL Lightness state */
    } light_hsl_lightness;      /*!< The Light HSL Lightness state */
    struct {
        uint16_t hue;           /*!< The value of the Light HSL Hue state */
    } light_hsl_hue;            /*!< The Light HSL Hue state */
    struct {
        uint16_t saturation;    /*!< The value of the Light HSL Saturation state */
    } light_hsl_saturation;     /*!< The Light HSL Saturation state */
    struct {
        uint16_t lightness;     /*!< The value of the Light xyL Lightness state */
    } light_xyl_lightness;      /*!< The Light xyL Lightness state */
    struct {
        uint8_t onoff;          /*!< The value of the Light LC Light OnOff state */
    } light_lc_light_onoff;     /*!< The Light LC Light OnOff state */
} esp_ble_mesh_server_state_value_t;

/** This enum value is the type of server model states */
typedef enum {
    ESP_BLE_MESH_GENERIC_ONOFF_STATE,
    ESP_BLE_MESH_GENERIC_LEVEL_STATE,
    ESP_BLE_MESH_GENERIC_ONPOWERUP_STATE,
    ESP_BLE_MESH_GENERIC_POWER_ACTUAL_STATE,
    ESP_BLE_MESH_LIGHT_LIGHTNESS_ACTUAL_STATE,
    ESP_BLE_MESH_LIGHT_LIGHTNESS_LINEAR_STATE,
    ESP_BLE_MESH_LIGHT_CTL_LIGHTNESS_STATE,
    ESP_BLE_MESH_LIGHT_CTL_TEMP_DELTA_UV_STATE,
    ESP_BLE_MESH_LIGHT_HSL_LIGHTNESS_STATE,
    ESP_BLE_MESH_LIGHT_HSL_HUE_STATE,
    ESP_BLE_MESH_LIGHT_HSL_SATURATION_STATE,
    ESP_BLE_MESH_LIGHT_XYL_LIGHTNESS_STATE,
    ESP_BLE_MESH_LIGHT_LC_LIGHT_ONOFF_STATE,
    ESP_BLE_MESH_SERVER_MODEL_STATE_MAX,
} esp_ble_mesh_server_state_type_t;

/*!< This enum value is the event of undefined SIG models and vendor models */
typedef enum {
    ESP_BLE_MESH_MODEL_OPERATION_EVT,                   /*!< User-defined models receive messages from peer devices (e.g. get, set, status, etc) event */
    ESP_BLE_MESH_MODEL_SEND_COMP_EVT,                   /*!< User-defined models send messages completion event */
    ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT,                /*!< User-defined models publish messages completion event */
    ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT,     /*!< User-defined client models receive publish messages event */
    ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT,         /*!< Timeout event for the user-defined client models that failed to receive response from peer server models */
    ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT,              /*!< When a model is configured to publish messages periodically, this event will occur during every publish period */
    ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT,    /*!< Server models update state value completion event */
    ESP_BLE_MESH_MODEL_EVT_MAX,
} esp_ble_mesh_model_cb_event_t;

/**
 * @brief BLE Mesh model callback parameters union
 */
typedef union {
    /**
     * @brief ESP_BLE_MESH_MODEL_OPERATION_EVT
     */
    struct ble_mesh_model_operation_evt_param {
        uint32_t opcode;                /*!< Opcode of the recieved message */
        esp_ble_mesh_model_t *model;    /*!< Pointer to the model which receives the message */
        esp_ble_mesh_msg_ctx_t *ctx;    /*!< Pointer to the context of the received message */
        uint16_t length;                /*!< Length of the received message */
        uint8_t *msg;                   /*!< Value of the received message */
    } model_operation;                  /*!< Event parameter of ESP_BLE_MESH_MODEL_OPERATION_EVT */
    /**
     * @brief ESP_BLE_MESH_MODEL_SEND_COMP_EVT
     */
    struct ble_mesh_model_send_comp_param {
        int err_code;                   /*!< Indicate the result of sending a message */
        uint32_t opcode;                /*!< Opcode of the message */
        esp_ble_mesh_model_t *model;    /*!< Pointer to the model which sends the message */
        esp_ble_mesh_msg_ctx_t *ctx;    /*!< Context of the message */
    } model_send_comp;                  /*!< Event parameter of ESP_BLE_MESH_MODEL_SEND_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT
     */
    struct ble_mesh_model_publish_comp_param {
        int err_code;                   /*!< Indicate the result of publishing a message */
        esp_ble_mesh_model_t *model;    /*!< Pointer to the model which publishes the message */
    } model_publish_comp;               /*!< Event parameter of ESP_BLE_MESH_MODEL_PUBLISH_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT
     */
    struct ble_mesh_mod_recv_publish_msg_param {
        uint32_t opcode;                /*!< Opcode of the unsoliciated received message */
        esp_ble_mesh_model_t *model;    /*!< Pointer to the model which receives the message */
        esp_ble_mesh_msg_ctx_t *ctx;    /*!< Pointer to the context of the message */
        uint16_t length;                /*!< Length of the received message */
        uint8_t *msg;                   /*!< Value of the received message */
    } client_recv_publish_msg;          /*!< Event parameter of ESP_BLE_MESH_CLIENT_MODEL_RECV_PUBLISH_MSG_EVT */
    /**
     * @brief ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT
     */
    struct ble_mesh_client_model_send_timeout_param {
        uint32_t opcode;                /*!< Opcode of the previously sent message */
        esp_ble_mesh_model_t *model;    /*!< Pointer to the model which sends the previous message */
        esp_ble_mesh_msg_ctx_t *ctx;    /*!< Pointer to the context of the previous message */
    } client_send_timeout;              /*!< Event parameter of ESP_BLE_MESH_CLIENT_MODEL_SEND_TIMEOUT_EVT */
    /**
     * @brief ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT
     */
    struct ble_mesh_model_publish_update_evt_param {
        esp_ble_mesh_model_t *model;    /*!< Pointer to the model which is going to update its publish message */
    } model_publish_update;             /*!< Event parameter of ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT */
    /**
     * @brief ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT
     */
    struct ble_mesh_server_model_update_state_comp_param {
        int err_code;                           /*!< Indicate the result of updating server model state */
        esp_ble_mesh_model_t *model;            /*!< Pointer to the server model which state value is updated */
        esp_ble_mesh_server_state_type_t type;  /*!< Type of the updated server state */
    } server_model_update_state;                /*!< Event parameter of ESP_BLE_MESH_SERVER_MODEL_UPDATE_STATE_COMP_EVT */
} esp_ble_mesh_model_cb_param_t;

#endif /* _ESP_BLE_MESH_DEFS_H_ */
