/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_DEFS_H_
#define _ESP_BLE_MESH_DEFS_H_

#include <stdint.h>

#include "mesh_config.h"
#include "mesh_common.h"
#include "proxy_server.h"
#include "provisioner_main.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!< The maximum length of a BLE Mesh message, including Opcode, Payload and TransMIC */
#define ESP_BLE_MESH_SDU_MAX_LEN            384

/*!< Length of a short Mesh MIC. */
#define ESP_BLE_MESH_MIC_SHORT              4

/*!< Length of a long Mesh MIC. */
#define ESP_BLE_MESH_MIC_LONG               8

/*!< The maximum length of a BLE Mesh provisioned node name */
#define ESP_BLE_MESH_NODE_NAME_MAX_LEN      31

/*!< The maximum length of a BLE Mesh unprovisioned device name */
#define ESP_BLE_MESH_DEVICE_NAME_MAX_LEN    DEVICE_NAME_SIZE

/*!< The maximum length of settings user id */
#define ESP_BLE_MESH_SETTINGS_UID_SIZE      20

/*!< Invalid settings index */
#define ESP_BLE_MESH_INVALID_SETTINGS_IDX   0xFF

/*!< Define the BLE Mesh octet 16 bytes size */
#define ESP_BLE_MESH_OCTET16_LEN    16
typedef uint8_t esp_ble_mesh_octet16_t[ESP_BLE_MESH_OCTET16_LEN];

/*!< Define the BLE Mesh octet 8 bytes size */
#define ESP_BLE_MESH_OCTET8_LEN     8
typedef uint8_t esp_ble_mesh_octet8_t[ESP_BLE_MESH_OCTET8_LEN];

/*!< Invalid Company ID */
#define ESP_BLE_MESH_CID_NVAL                     0xFFFF

/*!< Special TTL value to request using configured default TTL */
#define ESP_BLE_MESH_TTL_DEFAULT                  0xFF

/*!< Maximum allowed TTL value */
#define ESP_BLE_MESH_TTL_MAX                      0x7F

#define ESP_BLE_MESH_ADDR_UNASSIGNED              0x0000
#define ESP_BLE_MESH_ADDR_ALL_NODES               0xFFFF
#define ESP_BLE_MESH_ADDR_PROXIES                 0xFFFC
#define ESP_BLE_MESH_ADDR_FRIENDS                 0xFFFD
#define ESP_BLE_MESH_ADDR_RELAYS                  0xFFFE

#define ESP_BLE_MESH_KEY_UNUSED                   0xFFFF
#define ESP_BLE_MESH_KEY_DEV                      0xFFFE

#define ESP_BLE_MESH_KEY_PRIMARY                  0x0000
#define ESP_BLE_MESH_KEY_ANY                      0xFFFF

/*!< Internal macros used to initialize array members */
#define ESP_BLE_MESH_KEY_UNUSED_ELT_(IDX, _) ESP_BLE_MESH_KEY_UNUSED
#define ESP_BLE_MESH_ADDR_UNASSIGNED_ELT_(IDX, _) ESP_BLE_MESH_ADDR_UNASSIGNED
#define ESP_BLE_MESH_MODEL_KEYS_UNUSED            \
    { LISTIFY(CONFIG_BLE_MESH_MODEL_KEY_COUNT, ESP_BLE_MESH_KEY_UNUSED_ELT_, (,)) }
#define ESP_BLE_MESH_MODEL_GROUPS_UNASSIGNED      \
    { LISTIFY(CONFIG_BLE_MESH_MODEL_GROUP_COUNT, ESP_BLE_MESH_ADDR_UNASSIGNED_ELT_, (,)) }

/*!< Primary Network Key index */
#define ESP_BLE_MESH_NET_PRIMARY                  0x000

/*!< Relay state value */
#define ESP_BLE_MESH_RELAY_DISABLED               0x00
#define ESP_BLE_MESH_RELAY_ENABLED                0x01
#define ESP_BLE_MESH_RELAY_NOT_SUPPORTED          0x02

/*!< Beacon state value */
#define ESP_BLE_MESH_BEACON_DISABLED              0x00
#define ESP_BLE_MESH_BEACON_ENABLED               0x01

/*!< GATT Proxy state value */
#define ESP_BLE_MESH_GATT_PROXY_DISABLED          0x00
#define ESP_BLE_MESH_GATT_PROXY_ENABLED           0x01
#define ESP_BLE_MESH_GATT_PROXY_NOT_SUPPORTED     0x02

/*!< Friend state value */
#define ESP_BLE_MESH_FRIEND_DISABLED              0x00
#define ESP_BLE_MESH_FRIEND_ENABLED               0x01
#define ESP_BLE_MESH_FRIEND_NOT_SUPPORTED         0x02

/*!< Node identity state value */
#define ESP_BLE_MESH_NODE_IDENTITY_STOPPED        0x00
#define ESP_BLE_MESH_NODE_IDENTITY_RUNNING        0x01
#define ESP_BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED  0x02

/*!< Supported features */
#define ESP_BLE_MESH_FEATURE_RELAY                BIT(0)
#define ESP_BLE_MESH_FEATURE_PROXY                BIT(1)
#define ESP_BLE_MESH_FEATURE_FRIEND               BIT(2)
#define ESP_BLE_MESH_FEATURE_LOW_POWER            BIT(3)
#define ESP_BLE_MESH_FEATURE_ALL_SUPPORTED        (ESP_BLE_MESH_FEATURE_RELAY |     \
                                                   ESP_BLE_MESH_FEATURE_PROXY |     \
                                                   ESP_BLE_MESH_FEATURE_FRIEND |    \
                                                   ESP_BLE_MESH_FEATURE_LOW_POWER)

#define ESP_BLE_MESH_ADDR_IS_UNICAST(addr)        ((addr) && (addr) < 0x8000)
#define ESP_BLE_MESH_ADDR_IS_GROUP(addr)          ((addr) >= 0xC000 && (addr) <= 0xFF00)
#define ESP_BLE_MESH_ADDR_IS_VIRTUAL(addr)        ((addr) >= 0x8000 && (addr) < 0xC000)
#define ESP_BLE_MESH_ADDR_IS_RFU(addr)            ((addr) >= 0xFF00 && (addr) <= 0xFFFB)

#define ESP_BLE_MESH_INVALID_NODE_INDEX           0xFFFF

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
#define ESP_BLE_MESH_TRANSMIT(count, int_ms)    ((count) | (((int_ms / 10) - 1) << 3))

/** @def ESP_BLE_MESH_GET_TRANSMIT_COUNT
 *
 *  @brief Decode transmit count from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission count (actual transmissions equal to N + 1).
 */
#define ESP_BLE_MESH_GET_TRANSMIT_COUNT(transmit)   (((transmit) & (uint8_t)BIT_MASK(3)))

/** @def ESP_BLE_MESH_GET_TRANSMIT_INTERVAL
 *
 *  @brief Decode transmit interval from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define ESP_BLE_MESH_GET_TRANSMIT_INTERVAL(transmit)    ((((transmit) >> 3) + 1) * 10)

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
#define ESP_BLE_MESH_PUBLISH_TRANSMIT(count, int_ms)    ESP_BLE_MESH_TRANSMIT(count, (int_ms) / 5)

/** @def ESP_BLE_MESH_GET_PUBLISH_TRANSMIT_COUNT
 *
 *  @brief Decode Publish Retransmit count from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Retransmission count (actual transmissions equal to N + 1).
 */
#define ESP_BLE_MESH_GET_PUBLISH_TRANSMIT_COUNT(transmit)   ESP_BLE_MESH_GET_TRANSMIT_COUNT(transmit)

/** @def ESP_BLE_MESH_GET_PUBLISH_TRANSMIT_INTERVAL
 *
 *  @brief Decode Publish Retransmit interval from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define ESP_BLE_MESH_GET_PUBLISH_TRANSMIT_INTERVAL(transmit)    ((((transmit) >> 3) + 1) * 50)

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

/*!< Maximum length of value used by Static OOB authentication */
#define ESP_BLE_MESH_PROV_STATIC_OOB_MAX_LEN    16

/*!< Maximum length of string used by Output OOB authentication */
#define ESP_BLE_MESH_PROV_OUTPUT_OOB_MAX_LEN    8

/*!< Maximum length of string used by Output OOB authentication */
#define ESP_BLE_MESH_PROV_INPUT_OOB_MAX_LEN     8

/*!< Macros used to define message opcode */
#define ESP_BLE_MESH_MODEL_OP_1(b0)         (b0)
#define ESP_BLE_MESH_MODEL_OP_2(b0, b1)     (((b0) << 8) | (b1))
#define ESP_BLE_MESH_MODEL_OP_3(b0, cid)    ((((b0) << 16) | 0xC00000) | (cid))

/*!< This macro is associated with BLE_MESH_MODEL_CB in mesh_access.h */
#define ESP_BLE_MESH_SIG_MODEL(_id, _op, _pub, _user_data)          \
{                                                                   \
    .model_id = (_id),                                              \
    .pub = _pub,                                                    \
    .keys = ESP_BLE_MESH_MODEL_KEYS_UNUSED,                         \
    .groups = ESP_BLE_MESH_MODEL_GROUPS_UNASSIGNED,                 \
    .op = _op,                                                      \
    .user_data = _user_data,                                        \
}

/*!< This macro is associated with BLE_MESH_MODEL_VND_CB in mesh_access.h */
#define ESP_BLE_MESH_VENDOR_MODEL(_company, _id, _op, _pub, _user_data) \
{                                                                       \
    .vnd = {                                                            \
        .company_id = (_company),                                       \
        .model_id = (_id),                                              \
    },                                                                  \
    .pub = _pub,                                                        \
    .keys = ESP_BLE_MESH_MODEL_KEYS_UNUSED,                             \
    .groups = ESP_BLE_MESH_MODEL_GROUPS_UNASSIGNED,                     \
    .op = _op,                                                          \
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
    .vnd_model_count  = ARRAY_SIZE(_vnd_mods),          \
    .sig_models       = (_mods),                        \
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

#define ESP_BLE_MESH_ADDR_TYPE_PUBLIC       0x00
#define ESP_BLE_MESH_ADDR_TYPE_RANDOM       0x01
#define ESP_BLE_MESH_ADDR_TYPE_RPA_PUBLIC   0x02
#define ESP_BLE_MESH_ADDR_TYPE_RPA_RANDOM   0x03
/// BLE device address type
typedef uint8_t esp_ble_mesh_addr_type_t;

/** BLE Mesh deinit parameters */
typedef struct {
    bool erase_flash;   /*!< Indicate if erasing flash when deinit mesh stack */
} esp_ble_mesh_deinit_param_t;

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

    uint16_t publish_addr;  /*!< Publish Address. */
    uint16_t app_idx:12,    /*!< Publish AppKey Index. */
             cred:1,        /*!< Friendship Credentials Flag. */
             send_rel:1;    /*!< Force reliable sending (segment acks) */

    uint8_t  ttl;           /*!< Publish Time to Live. */
    uint8_t  retransmit;    /*!< Retransmit Count & Interval Steps. */

    uint8_t  period;        /*!< Publish Period. */
    uint8_t  period_div:4,  /*!< Divisor for the Period. */
             fast_period:1, /*!< Use FastPeriodDivisor */
             count:3;       /*!< Retransmissions left. */

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

    /** Publish Period Timer. Initialized by the stack. */
    struct k_delayed_work timer;

    /** Role of the device that is going to publish messages */
    uint8_t dev_role;
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
        .msg = &bt_mesh_pub_msg_##_name, \
        .update = (uint32_t)NULL, \
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

/** Abstraction that describes a model callback structure.
 *  This structure is associated with struct bt_mesh_model_cb in mesh_access.h.
 */
typedef struct {
    /** Callback used during model initialization. Initialized by the stack. */
    esp_ble_mesh_cb_t init_cb;

#if CONFIG_BLE_MESH_DEINIT
    /** Callback used during model deinitialization. Initialized by the stack. */
    esp_ble_mesh_cb_t deinit_cb;
#endif /* CONFIG_BLE_MESH_DEINIT */
} esp_ble_mesh_model_cbs_t;

/** Abstraction that describes a Mesh Model instance.
 *  This structure is associated with struct bt_mesh_model in mesh_access.h
 */
struct esp_ble_mesh_model {
    /** Model ID */
    union {
        const uint16_t model_id; /*!< 16-bit model identifier */
        struct {
            uint16_t company_id; /*!< 16-bit company identifier */
            uint16_t model_id; /*!< 16-bit model identifier */
        } vnd; /*!< Structure encapsulating a model ID with a company ID */
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

    /** Model callback structure */
    esp_ble_mesh_model_cbs_t *cb;

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

    /** RSSI of received packet. Not used for sending. */
    int8_t   recv_rssi;

    /** Received TTL value. Not used for sending. */
    uint8_t  recv_ttl: 7;

    /** Force sending reliably by using segment acknowledgement */
    uint8_t  send_rel: 1;

    /** TTL, or ESP_BLE_MESH_TTL_DEFAULT for default TTL. */
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

    /* NOTE: In order to avoid suffering brute-forcing attack (CVE-2020-26559).
     * The Bluetooth SIG recommends that potentially vulnerable mesh provisioners
     * support an out-of-band mechanism to exchange the public keys.
     * So as an unprovisioned device, it should enable this flag to support
     * using an out-of-band mechanism to exchange Public Key.
     */
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

    /* NOTE: In order to avoid suffering brute-forcing attack (CVE-2020-26559).
     * The Bluetooth SIG recommends that potentially vulnerable mesh provisioners
     * use an out-of-band mechanism to exchange the public keys.
     */
    /** Provisioner public key oob */
    uint8_t        prov_pub_key_oob;

    /** Callback used to notify to set device OOB Public Key. Initialized by the stack. */
    esp_ble_mesh_cb_t provisioner_prov_read_oob_pub_key;

    /* NOTE: The Bluetooth SIG recommends that mesh implementations enforce a randomly
     * selected AuthValue using all of the available bits, where permitted by the
     * implementation. A large entropy helps ensure that a brute-force of the AuthValue,
     * even a static AuthValue, cannot normally be completed in a reasonable time (CVE-2020-26557).
     *
     * AuthValues selected using a cryptographically secure random or pseudorandom number
     * generator and having the maximum permitted entropy (128-bits) will be most difficult
     * to brute-force. AuthValues with reduced entropy or generated in a predictable manner
     * will not grant the same level of protection against this vulnerability. Selecting a
     * new AuthValue with each provisioning attempt can also make it more difficult to launch
     * a brute-force attack by requiring the attacker to restart the search with each
     * provisioning attempt (CVE-2020-26556).
     */
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
            esp_ble_mesh_bd_addr_t addr;         /*!< Device address */
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

/** Information of the provisioned node */
typedef struct {
    /* Device information */
    esp_ble_mesh_bd_addr_t   addr;      /*!< Node device address */
    esp_ble_mesh_addr_type_t addr_type; /*!< Node device address type */
    uint8_t  dev_uuid[16];  /*!< Device UUID */
    uint16_t oob_info;      /*!< Node OOB information */

    /* Provisioning information */
    uint16_t unicast_addr;  /*!< Node unicast address */
    uint8_t  element_num;   /*!< Node element number */
    uint16_t net_idx;       /*!< Node NetKey Index */
    uint8_t  flags;         /*!< Node key refresh flag and iv update flag */
    uint32_t iv_index;      /*!< Node IV Index */
    uint8_t  dev_key[16];   /*!< Node device key */

    /* Additional information */
    char name[ESP_BLE_MESH_NODE_NAME_MAX_LEN + 1]; /*!< Node name */
    uint16_t comp_length;  /*!< Length of Composition Data */
    uint8_t *comp_data;    /*!< Value of Composition Data */
} __attribute__((packed)) esp_ble_mesh_node_t;

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

/*!< Provisioner heartbeat filter type */
#define ESP_BLE_MESH_HEARTBEAT_FILTER_ACCEPTLIST    0x00
#define ESP_BLE_MESH_HEARTBEAT_FILTER_REJECTLIST    0x01

/*!< Provisioner heartbeat filter operation */
#define ESP_BLE_MESH_HEARTBEAT_FILTER_ADD           0x00
#define ESP_BLE_MESH_HEARTBEAT_FILTER_REMOVE        0x01

/** Context of Provisioner heartbeat filter information to be set */
typedef struct {
    uint16_t hb_src;    /*!< Heartbeat source address (unicast address) */
    uint16_t hb_dst;    /*!< Heartbeat destination address (unicast address or group address) */
} esp_ble_mesh_heartbeat_filter_info_t;

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
    ESP_BLE_MESH_NODE_ADD_LOCAL_NET_KEY_COMP_EVT,               /*!< Node add NetKey locally completion event */
    ESP_BLE_MESH_NODE_ADD_LOCAL_APP_KEY_COMP_EVT,               /*!< Node add AppKey locally completion event */
    ESP_BLE_MESH_NODE_BIND_APP_KEY_TO_MODEL_COMP_EVT,           /*!< Node bind AppKey to model locally completion event */
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
    ESP_BLE_MESH_PROVISIONER_PROV_DEV_WITH_ADDR_COMP_EVT,       /*!< Provisioner start to provision an unprovisioned device completion event */
    ESP_BLE_MESH_PROVISIONER_DELETE_DEV_COMP_EVT,               /*!< Provisioner delete a device from the list, close provisioning link with the device completion event */
    ESP_BLE_MESH_PROVISIONER_SET_DEV_UUID_MATCH_COMP_EVT,       /*!< Provisioner set the value to be compared with part of the unprovisioned device UUID completion event */
    ESP_BLE_MESH_PROVISIONER_SET_PROV_DATA_INFO_COMP_EVT,       /*!< Provisioner set net_idx/flags/iv_index used for provisioning completion event */
    ESP_BLE_MESH_PROVISIONER_SET_STATIC_OOB_VALUE_COMP_EVT,     /*!< Provisioner set static oob value used for provisioning completion event */
    ESP_BLE_MESH_PROVISIONER_SET_PRIMARY_ELEM_ADDR_COMP_EVT,    /*!< Provisioner set unicast address of primary element completion event */
    ESP_BLE_MESH_PROVISIONER_PROV_READ_OOB_PUB_KEY_COMP_EVT,    /*!< Provisioner read unprovisioned device OOB public key completion event */
    ESP_BLE_MESH_PROVISIONER_PROV_INPUT_NUMBER_COMP_EVT,        /*!< Provisioner input number completion event */
    ESP_BLE_MESH_PROVISIONER_PROV_INPUT_STRING_COMP_EVT,        /*!< Provisioner input string completion event */
    ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT,            /*!< Provisioner set node name completion event */
    ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT,        /*!< Provisioner add local app key completion event */
    ESP_BLE_MESH_PROVISIONER_UPDATE_LOCAL_APP_KEY_COMP_EVT,     /*!< Provisioner update local app key completion event */
    ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT,    /*!< Provisioner bind local model with local app key completion event */
    ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT,        /*!< Provisioner add local network key completion event */
    ESP_BLE_MESH_PROVISIONER_UPDATE_LOCAL_NET_KEY_COMP_EVT,     /*!< Provisioner update local network key completion event */
    ESP_BLE_MESH_PROVISIONER_STORE_NODE_COMP_DATA_COMP_EVT,     /*!< Provisioner store node composition data completion event */
    ESP_BLE_MESH_PROVISIONER_DELETE_NODE_WITH_UUID_COMP_EVT,    /*!< Provisioner delete node with uuid completion event */
    ESP_BLE_MESH_PROVISIONER_DELETE_NODE_WITH_ADDR_COMP_EVT,    /*!< Provisioner delete node with unicast address completion event */
    ESP_BLE_MESH_PROVISIONER_ENABLE_HEARTBEAT_RECV_COMP_EVT,     /*!< Provisioner start to receive heartbeat message completion event */
    ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_TYPE_COMP_EVT, /*!< Provisioner set the heartbeat filter type completion event */
    ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_INFO_COMP_EVT, /*!< Provisioner set the heartbeat filter information completion event */
    ESP_BLE_MESH_PROVISIONER_RECV_HEARTBEAT_MESSAGE_EVT,         /*!< Provisioner receive heartbeat message event */
    ESP_BLE_MESH_PROVISIONER_DIRECT_ERASE_SETTINGS_COMP_EVT,        /*!< Provisioner directly erase settings completion event */
    ESP_BLE_MESH_PROVISIONER_OPEN_SETTINGS_WITH_INDEX_COMP_EVT,     /*!< Provisioner open settings with index completion event */
    ESP_BLE_MESH_PROVISIONER_OPEN_SETTINGS_WITH_UID_COMP_EVT,       /*!< Provisioner open settings with user id completion event */
    ESP_BLE_MESH_PROVISIONER_CLOSE_SETTINGS_WITH_INDEX_COMP_EVT,    /*!< Provisioner close settings with index completion event */
    ESP_BLE_MESH_PROVISIONER_CLOSE_SETTINGS_WITH_UID_COMP_EVT,      /*!< Provisioner close settings with user id completion event */
    ESP_BLE_MESH_PROVISIONER_DELETE_SETTINGS_WITH_INDEX_COMP_EVT,   /*!< Provisioner delete settings with index completion event */
    ESP_BLE_MESH_PROVISIONER_DELETE_SETTINGS_WITH_UID_COMP_EVT,     /*!< Provisioner delete settings with user id completion event */
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
    ESP_BLE_MESH_PROXY_SERVER_CONNECTED_EVT,                    /*!< Proxy Server establishes connection successfully event */
    ESP_BLE_MESH_PROXY_SERVER_DISCONNECTED_EVT,                 /*!< Proxy Server terminates connection successfully event */
    ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT,           /*!< Local model subscribes group address completion event */
    ESP_BLE_MESH_MODEL_UNSUBSCRIBE_GROUP_ADDR_COMP_EVT,         /*!< Local model unsubscribes group address completion event */
    ESP_BLE_MESH_DEINIT_MESH_COMP_EVT,                          /*!< De-initialize BLE Mesh stack completion event */
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
     * @brief ESP_BLE_MESH_NODE_ADD_LOCAL_NET_KEY_COMP_EVT
     */
    struct ble_mesh_node_add_local_net_key_comp_param {
        int err_code;                           /*!< Indicate the result of adding local NetKey by the node */
        uint16_t net_idx;                       /*!< NetKey Index */
    } node_add_net_key_comp;                    /*!< Event parameter of ESP_BLE_MESH_NODE_ADD_LOCAL_NET_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_ADD_LOCAL_APP_KEY_COMP_EVT
     */
    struct ble_mesh_node_add_local_app_key_comp_param {
        int err_code;                           /*!< Indicate the result of adding local AppKey by the node */
        uint16_t net_idx;                       /*!< NetKey Index */
        uint16_t app_idx;                       /*!< AppKey Index */
    } node_add_app_key_comp;                    /*!< Event parameter of ESP_BLE_MESH_NODE_ADD_LOCAL_APP_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_NODE_BIND_APP_KEY_TO_MODEL_COMP_EVT
     */
    struct ble_mesh_node_bind_local_mod_app_comp_param {
        int err_code;                           /*!< Indicate the result of binding AppKey with model by the node */
        uint16_t element_addr;                  /*!< Element address */
        uint16_t app_idx;                       /*!< AppKey Index */
        uint16_t company_id;                    /*!< Company ID */
        uint16_t model_id;                      /*!< Model ID */
    } node_bind_app_key_to_model_comp;          /*!< Event parameter of ESP_BLE_MESH_NODE_BIND_APP_KEY_TO_MODEL_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT
     */
    struct ble_mesh_provisioner_recv_unprov_adv_pkt_param {
        uint8_t  dev_uuid[16];                  /*!< Device UUID of the unprovisioned device */
        esp_ble_mesh_bd_addr_t addr;            /*!< Device address of the unprovisioned device */
        esp_ble_mesh_addr_type_t addr_type;     /*!< Device address type */
        uint16_t oob_info;                      /*!< OOB Info of the unprovisioned device */
        uint8_t  adv_type;                      /*!< Avertising type of the unprovisioned device */
        esp_ble_mesh_prov_bearer_t bearer;      /*!< Bearer of the unprovisioned device */
        int8_t   rssi;                          /*!< RSSI of the received advertising packet */
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
        uint16_t node_idx;                      /*!< Index of the provisioned device */
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
     * @brief ESP_BLE_MESH_PROVISIONER_PROV_DEV_WITH_ADDR_COMP_EVT
     */
    struct ble_mesh_provisioner_prov_dev_with_addr_comp_param {
        int err_code;                           /*!< Indicate the result of Provisioner starting to provision a device */
    } provisioner_prov_dev_with_addr_comp;      /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_PROV_DEV_WITH_ADDR_COMP_EVT */
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
     * @brief ESP_BLE_MESH_PROVISIONER_SET_STATIC_OOB_VALUE_COMP_EVT
     */
    struct ble_mesh_provisioner_set_static_oob_val_comp_param {
        int err_code;                           /*!< Indicate the result of setting static oob value by the Provisioner */
    } provisioner_set_static_oob_val_comp;      /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_SET_STATIC_OOB_VALUE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_SET_PRIMARY_ELEM_ADDR_COMP_EVT
     */
    struct ble_mesh_provisioner_set_primary_elem_addr_comp_param {
        int err_code;                           /*!< Indicate the result of setting unicast address of primary element by the Provisioner */
    } provisioner_set_primary_elem_addr_comp;   /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_SET_PRIMARY_ELEM_ADDR_COMP_EVT */
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
        uint16_t node_index;                    /*!< Index of the provisioned device */
    } provisioner_set_node_name_comp;           /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_add_local_app_key_comp_param {
        int err_code;                           /*!< Indicate the result of adding local AppKey by the Provisioner */
        uint16_t net_idx;                       /*!< NetKey Index */
        uint16_t app_idx;                       /*!< AppKey Index */
    } provisioner_add_app_key_comp;             /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_UPDATE_LOCAL_APP_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_update_local_app_key_comp_param {
        int err_code;                           /*!< Indicate the result of updating local AppKey by the Provisioner */
        uint16_t net_idx;                       /*!< NetKey Index */
        uint16_t app_idx;                       /*!< AppKey Index */
    } provisioner_update_app_key_comp;          /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_UPDATE_LOCAL_APP_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT
     */
    struct ble_mesh_provisioner_bind_local_mod_app_comp_param {
        int err_code;                           /*!< Indicate the result of binding AppKey with model by the Provisioner */
        uint16_t element_addr;                  /*!< Element address */
        uint16_t app_idx;                       /*!< AppKey Index */
        uint16_t company_id;                    /*!< Company ID */
        uint16_t model_id;                      /*!< Model ID */
    } provisioner_bind_app_key_to_model_comp;   /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_BIND_APP_KEY_TO_MODEL_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_add_local_net_key_comp_param {
        int err_code;                           /*!< Indicate the result of adding local NetKey by the Provisioner */
        uint16_t net_idx;                       /*!< NetKey Index */
    } provisioner_add_net_key_comp;             /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_NET_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_UPDATE_LOCAL_NET_KEY_COMP_EVT
     */
    struct ble_mesh_provisioner_update_local_net_key_comp_param {
        int err_code;                           /*!< Indicate the result of updating local NetKey by the Provisioner */
        uint16_t net_idx;                       /*!< NetKey Index */
    } provisioner_update_net_key_comp;          /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_UPDATE_LOCAL_NET_KEY_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_STORE_NODE_COMP_DATA_COMP_EVT
     */
    struct ble_mesh_provisioner_store_node_comp_data_comp_param {
        int err_code;                           /*!< Indicate the result of storing node composition data by the Provisioner */
        uint16_t addr;                          /*!< Node element address */
    } provisioner_store_node_comp_data_comp;    /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_STORE_NODE_COMP_DATA_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_DELETE_NODE_WITH_UUID_COMP_EVT
     */
    struct ble_mesh_provisioner_delete_node_with_uuid_comp_param {
        int err_code;                           /*!< Indicate the result of deleting node with uuid by the Provisioner */
        uint8_t uuid[16];                       /*!< Node device uuid */
    } provisioner_delete_node_with_uuid_comp;   /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_DELETE_NODE_WITH_UUID_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_DELETE_NODE_WITH_ADDR_COMP_EVT
     */
    struct ble_mesh_provisioner_delete_node_with_addr_comp_param {
        int err_code;                           /*!< Indicate the result of deleting node with unicast address by the Provisioner */
        uint16_t unicast_addr;                  /*!< Node unicast address */
    } provisioner_delete_node_with_addr_comp;   /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_DELETE_NODE_WITH_ADDR_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_ENABLE_HEARTBEAT_RECV_COMP_EVT
     */
    struct {
        int err_code;                           /*!< Indicate the result of enabling/disabling to receive heartbeat messages by the Provisioner */
        bool enable;                            /*!< Indicate enabling or disabling receiving heartbeat messages */
    } provisioner_enable_heartbeat_recv_comp;   /*!< Event parameters of ESP_BLE_MESH_PROVISIONER_ENABLE_HEARTBEAT_RECV_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_TYPE_COMP_EVT
     */
    struct {
        int err_code;                               /*!< Indicate the result of setting the heartbeat filter type by the Provisioner */
        uint8_t type;                               /*!< Type of the filter used for receiving heartbeat messages */
    } provisioner_set_heartbeat_filter_type_comp;   /*!< Event parameters of ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_TYPE_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_INFO_COMP_EVT
     */
    struct {
        int err_code;                               /*!< Indicate the result of setting the heartbeat filter address by the Provisioner */
        uint8_t  op;                                /*!< Operation (add, remove, clean) */
        uint16_t hb_src;                            /*!< Heartbeat source address */
        uint16_t hb_dst;                            /*!< Heartbeat destination address */
    } provisioner_set_heartbeat_filter_info_comp;   /*!< Event parameters of ESP_BLE_MESH_PROVISIONER_SET_HEARTBEAT_FILTER_INFO_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_RECV_HEARTBEAT_MESSAGE_EVT
     */
    struct {
        uint16_t hb_src;            /*!< Heartbeat source address */
        uint16_t hb_dst;            /*!< Heartbeat destination address */
        uint8_t  init_ttl;          /*!< Heartbeat InitTTL */
        uint8_t  rx_ttl;            /*!< Heartbeat RxTTL */
        uint8_t  hops;              /*!< Heartbeat hops (InitTTL - RxTTL + 1) */
        uint16_t feature;           /*!< Bit field of currently active features of the node */
        int8_t   rssi;              /*!< RSSI of the heartbeat message */
    } provisioner_recv_heartbeat;   /*!< Event parameters of ESP_BLE_MESH_PROVISIONER_RECV_HEARTBEAT_MESSAGE_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_DIRECT_ERASE_SETTINGS_COMP_EVT
     */
    struct {
        int err_code;                           /*!< Indicate the result of directly erasing settings by the Provisioner */
    } provisioner_direct_erase_settings_comp;   /*!< Event parameters of ESP_BLE_MESH_PROVISIONER_DIRECT_ERASE_SETTINGS_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_OPEN_SETTINGS_WITH_INDEX_COMP_EVT
     */
    struct {
        int err_code;                               /*!< Indicate the result of opening settings with index by the Provisioner */
        uint8_t index;                              /*!< Index of Provisioner settings */
    } provisioner_open_settings_with_index_comp;    /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_OPEN_SETTINGS_WITH_INDEX_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_OPEN_SETTINGS_WITH_UID_COMP_EVT
     */
    struct {
        int err_code;                                   /*!< Indicate the result of opening settings with user id by the Provisioner */
        uint8_t index;                                  /*!< Index of Provisioner settings */
        char uid[ESP_BLE_MESH_SETTINGS_UID_SIZE + 1];   /*!< Provisioner settings user id */
    } provisioner_open_settings_with_uid_comp;          /*!< Event parameters of ESP_BLE_MESH_PROVISIONER_OPEN_SETTINGS_WITH_UID_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_CLOSE_SETTINGS_WITH_INDEX_COMP_EVT
     */
    struct {
        int err_code;                               /*!< Indicate the result of closing settings with index by the Provisioner */
        uint8_t index;                              /*!< Index of Provisioner settings */
    } provisioner_close_settings_with_index_comp;   /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_CLOSE_SETTINGS_WITH_INDEX_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_CLOSE_SETTINGS_WITH_UID_COMP_EVT
     */
    struct {
        int err_code;                                   /*!< Indicate the result of closing settings with user id by the Provisioner */
        uint8_t index;                                  /*!< Index of Provisioner settings */
        char uid[ESP_BLE_MESH_SETTINGS_UID_SIZE + 1];   /*!< Provisioner settings user id */
    } provisioner_close_settings_with_uid_comp;         /*!< Event parameters of ESP_BLE_MESH_PROVISIONER_CLOSE_SETTINGS_WITH_UID_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_DELETE_SETTINGS_WITH_INDEX_COMP_EVT
     */
    struct {
        int err_code;                               /*!< Indicate the result of deleting settings with index by the Provisioner */
        uint8_t index;                              /*!< Index of Provisioner settings */
    } provisioner_delete_settings_with_index_comp;  /*!< Event parameter of ESP_BLE_MESH_PROVISIONER_DELETE_SETTINGS_WITH_INDEX_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_PROVISIONER_DELETE_SETTINGS_WITH_UID_COMP_EVT
     */
    struct {
        int err_code;                                   /*!< Indicate the result of deleting settings with user id by the Provisioner */
        uint8_t index;                                  /*!< Index of Provisioner settings */
        char uid[ESP_BLE_MESH_SETTINGS_UID_SIZE + 1];   /*!< Provisioner settings user id */
    } provisioner_delete_settings_with_uid_comp;        /*!< Event parameters of ESP_BLE_MESH_PROVISIONER_DELETE_SETTINGS_WITH_UID_COMP_EVT */
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
        uint16_t lpn_addr;                      /*!< Low Power Node unicast address */
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
        int8_t   rssi;                          /*!< RSSI of the received advertising packet */
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
    /**
     * @brief ESP_BLE_MESH_PROXY_SERVER_CONNECTED_EVT
     */
    struct ble_mesh_proxy_server_connected_param {
        uint8_t conn_handle;                    /*!< Proxy connection handle */
    } proxy_server_connected;                   /*!< Event parameter of ESP_BLE_MESH_PROXY_SERVER_CONNECTED_EVT */
    /**
     * @brief ESP_BLE_MESH_PROXY_SERVER_DISCONNECTED_EVT
     */
    struct ble_mesh_proxy_server_disconnected_param {
        uint8_t conn_handle;                    /*!< Proxy connection handle */
        uint8_t reason;                         /*!< Proxy disconnect reason */
    } proxy_server_disconnected;                /*!< Event parameter of ESP_BLE_MESH_PROXY_SERVER_DISCONNECTED_EVT */
    /**
     * @brief ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT
     */
    struct ble_mesh_model_sub_group_addr_comp_param {
        int err_code;                           /*!< Indicate the result of local model subscribing group address */
        uint16_t element_addr;                  /*!< Element address */
        uint16_t company_id;                    /*!< Company ID */
        uint16_t model_id;                      /*!< Model ID */
        uint16_t group_addr;                    /*!< Group Address */
    } model_sub_group_addr_comp;                /*!< Event parameters of ESP_BLE_MESH_MODEL_SUBSCRIBE_GROUP_ADDR_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_MODEL_UNSUBSCRIBE_GROUP_ADDR_COMP_EVT
     */
    struct ble_mesh_model_unsub_group_addr_comp_param {
        int err_code;                           /*!< Indicate the result of local model unsubscribing group address */
        uint16_t element_addr;                  /*!< Element address */
        uint16_t company_id;                    /*!< Company ID */
        uint16_t model_id;                      /*!< Model ID */
        uint16_t group_addr;                    /*!< Group Address */
    } model_unsub_group_addr_comp;              /*!< Event parameters of ESP_BLE_MESH_MODEL_UNSUBSCRIBE_GROUP_ADDR_COMP_EVT */
    /**
     * @brief ESP_BLE_MESH_DEINIT_MESH_COMP_EVT
     */
    struct ble_mesh_deinit_mesh_comp_param {
        int err_code;                           /*!< Indicate the result of BLE Mesh deinitialization */
    } deinit_mesh_comp;                         /*!< Event parameter of ESP_BLE_MESH_DEINIT_MESH_COMP_EVT */
} esp_ble_mesh_prov_cb_param_t;

/**
 * @brief BLE Mesh models related Model ID and Opcode definitions
 */

/*!< Foundation Models */
#define ESP_BLE_MESH_MODEL_ID_CONFIG_SRV                            0x0000
#define ESP_BLE_MESH_MODEL_ID_CONFIG_CLI                            0x0001
#define ESP_BLE_MESH_MODEL_ID_HEALTH_SRV                            0x0002
#define ESP_BLE_MESH_MODEL_ID_HEALTH_CLI                            0x0003

/*!< Models from the Mesh Model Specification */
#define ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_SRV                         0x1000
#define ESP_BLE_MESH_MODEL_ID_GEN_ONOFF_CLI                         0x1001
#define ESP_BLE_MESH_MODEL_ID_GEN_LEVEL_SRV                         0x1002
#define ESP_BLE_MESH_MODEL_ID_GEN_LEVEL_CLI                         0x1003
#define ESP_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV                0x1004
#define ESP_BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI                0x1005
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV                   0x1006
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV             0x1007
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI                   0x1008
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV                   0x1009
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV             0x100a
#define ESP_BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI                   0x100b
#define ESP_BLE_MESH_MODEL_ID_GEN_BATTERY_SRV                       0x100c
#define ESP_BLE_MESH_MODEL_ID_GEN_BATTERY_CLI                       0x100d
#define ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_SRV                      0x100e
#define ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV                0x100f
#define ESP_BLE_MESH_MODEL_ID_GEN_LOCATION_CLI                      0x1010
#define ESP_BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV                    0x1011
#define ESP_BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV             0x1012
#define ESP_BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV                     0x1013
#define ESP_BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV                   0x1014
#define ESP_BLE_MESH_MODEL_ID_GEN_PROP_CLI                          0x1015
#define ESP_BLE_MESH_MODEL_ID_SENSOR_SRV                            0x1100
#define ESP_BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV                      0x1101
#define ESP_BLE_MESH_MODEL_ID_SENSOR_CLI                            0x1102
#define ESP_BLE_MESH_MODEL_ID_TIME_SRV                              0x1200
#define ESP_BLE_MESH_MODEL_ID_TIME_SETUP_SRV                        0x1201
#define ESP_BLE_MESH_MODEL_ID_TIME_CLI                              0x1202
#define ESP_BLE_MESH_MODEL_ID_SCENE_SRV                             0x1203
#define ESP_BLE_MESH_MODEL_ID_SCENE_SETUP_SRV                       0x1204
#define ESP_BLE_MESH_MODEL_ID_SCENE_CLI                             0x1205
#define ESP_BLE_MESH_MODEL_ID_SCHEDULER_SRV                         0x1206
#define ESP_BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV                   0x1207
#define ESP_BLE_MESH_MODEL_ID_SCHEDULER_CLI                         0x1208
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV                   0x1300
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV             0x1301
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI                   0x1302
#define ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV                         0x1303
#define ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV                   0x1304
#define ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI                         0x1305
#define ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV                    0x1306
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV                         0x1307
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV                   0x1308
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI                         0x1309
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV                     0x130a
#define ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV                     0x130b
#define ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_SRV                         0x130c
#define ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV                   0x130d
#define ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI                         0x130e
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LC_SRV                          0x130f
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV                    0x1310
#define ESP_BLE_MESH_MODEL_ID_LIGHT_LC_CLI                          0x1311

/**
 * esp_ble_mesh_opcode_config_client_get_t belongs to esp_ble_mesh_opcode_t, this typedef is only
 * used to locate the opcodes used by esp_ble_mesh_config_client_get_state.
 * The following opcodes will only be used in the esp_ble_mesh_config_client_get_state function.
 */
typedef uint32_t esp_ble_mesh_opcode_config_client_get_t;

#define ESP_BLE_MESH_MODEL_OP_BEACON_GET                            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x09) /*!< Config Beacon Get */
#define ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x08) /*!< Config Composition Data Get */
#define ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_GET                       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x0C) /*!< Config Default TTL Get */
#define ESP_BLE_MESH_MODEL_OP_GATT_PROXY_GET                        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x12) /*!< Config GATT Proxy Get */
#define ESP_BLE_MESH_MODEL_OP_RELAY_GET                             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x26) /*!< Config Relay Get */
#define ESP_BLE_MESH_MODEL_OP_MODEL_PUB_GET                         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x18) /*!< Config Model Publication Get */
#define ESP_BLE_MESH_MODEL_OP_FRIEND_GET                            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x0F) /*!< Config Friend Get */
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_GET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x38) /*!< Config Heartbeat Publication Get */
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_GET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x3a) /*!< Config Heartbeat Subscription Get */
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_GET                           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x42) /*!< Config NetKey Get */
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_GET                           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x01) /*!< Config AppKey Get */
#define ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_GET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x46) /*!< Config Node Identity Get */
#define ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_GET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x29) /*!< Config SIG Model Subscription Get */
#define ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_GET                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x2B) /*!< Config Vendor Model Subscription Get */
#define ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_GET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x4B) /*!< Config SIG Model App Get */
#define ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_GET                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x4D) /*!< Config Vendor Model App Get */
#define ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_GET                 ESP_BLE_MESH_MODEL_OP_2(0x80, 0x15) /*!< Config Key Refresh Phase Get */
#define ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_GET                   ESP_BLE_MESH_MODEL_OP_2(0x80, 0x2D) /*!< Config Low Power Node PollTimeout Get */
#define ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_GET                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x23) /*!< Config Network Transmit Get */

/**
 * esp_ble_mesh_opcode_config_client_set_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by esp_ble_mesh_config_client_set_state.
 * The following opcodes will only be used in the esp_ble_mesh_config_client_set_state function.
 */
typedef uint32_t esp_ble_mesh_opcode_config_client_set_t;

#define ESP_BLE_MESH_MODEL_OP_BEACON_SET                            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x0A) /*!< Config Beacon Set */
#define ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_SET                       ESP_BLE_MESH_MODEL_OP_2(0x80, 0x0D) /*!< Config Default TTL Set */
#define ESP_BLE_MESH_MODEL_OP_GATT_PROXY_SET                        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x13) /*!< Config GATT Proxy Set */
#define ESP_BLE_MESH_MODEL_OP_RELAY_SET                             ESP_BLE_MESH_MODEL_OP_2(0x80, 0x27) /*!< Config Relay Set */
#define ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET                         ESP_BLE_MESH_MODEL_OP_1(0x03)       /*!< Config Model Publication Set */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD                         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x1B) /*!< Config Model Subscription Add */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_ADD            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x20) /*!< Config Model Subscription Virtual Address Add */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x1C) /*!< Config Model Subscription Delete */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_DELETE         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x21) /*!< Config Model Subscription Virtual Address Delete */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_OVERWRITE                   ESP_BLE_MESH_MODEL_OP_2(0x80, 0x1E) /*!< Config Model Subscription Overwrite */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_VIRTUAL_ADDR_OVERWRITE      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x22) /*!< Config Model Subscription Virtual Address Overwrite */
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_ADD                           ESP_BLE_MESH_MODEL_OP_2(0x80, 0x40) /*!< Config NetKey Add */
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD                           ESP_BLE_MESH_MODEL_OP_1(0x00)       /*!< Config AppKey Add */
#define ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND                        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x3D) /*!< Config Model App Bind */
#define ESP_BLE_MESH_MODEL_OP_NODE_RESET                            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x49) /*!< Config Node Reset */
#define ESP_BLE_MESH_MODEL_OP_FRIEND_SET                            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x10) /*!< Config Friend Set */
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_SET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x39) /*!< Config Heartbeat Publication Set */
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_SET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x3B) /*!< Config Heartbeat Subscription Set */
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_UPDATE                        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x45) /*!< Config NetKey Update */
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_DELETE                        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x41) /*!< Config NetKey Delete */
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_UPDATE                        ESP_BLE_MESH_MODEL_OP_1(0x01)       /*!< Config AppKey Update */
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_DELETE                        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x00) /*!< Config AppKey Delete */
#define ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_SET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x47) /*!< Config Node Identity Set */
#define ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_SET                 ESP_BLE_MESH_MODEL_OP_2(0x80, 0x16) /*!< Config Key Refresh Phase Set */
#define ESP_BLE_MESH_MODEL_OP_MODEL_PUB_VIRTUAL_ADDR_SET            ESP_BLE_MESH_MODEL_OP_2(0x80, 0x1A) /*!< Config Model Publication Virtual Address Set */
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x1D) /*!< Config Model Subscription Delete All */
#define ESP_BLE_MESH_MODEL_OP_MODEL_APP_UNBIND                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x3F) /*!< Config Model App Unbind */
#define ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_SET                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x24) /*!< Config Network Transmit Set */

/**
 * esp_ble_mesh_opcode_config_status_t belongs to esp_ble_mesh_opcode_t, this typedef is only
 * used to locate the opcodes used by the Config Model messages
 * The following opcodes are used by the BLE Mesh Config Server Model internally to respond
 * to the Config Client Model's request messages.
 */
typedef uint32_t esp_ble_mesh_opcode_config_status_t;

#define ESP_BLE_MESH_MODEL_OP_BEACON_STATUS                         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x0B)
#define ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_STATUS               ESP_BLE_MESH_MODEL_OP_1(0x02)
#define ESP_BLE_MESH_MODEL_OP_DEFAULT_TTL_STATUS                    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x0E)
#define ESP_BLE_MESH_MODEL_OP_GATT_PROXY_STATUS                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x14)
#define ESP_BLE_MESH_MODEL_OP_RELAY_STATUS                          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x28)
#define ESP_BLE_MESH_MODEL_OP_MODEL_PUB_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x19)
#define ESP_BLE_MESH_MODEL_OP_MODEL_SUB_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x1F)
#define ESP_BLE_MESH_MODEL_OP_SIG_MODEL_SUB_LIST                    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x2A)
#define ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_SUB_LIST                 ESP_BLE_MESH_MODEL_OP_2(0x80, 0x2C)
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_STATUS                        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x44)
#define ESP_BLE_MESH_MODEL_OP_NET_KEY_LIST                          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x43)
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_STATUS                        ESP_BLE_MESH_MODEL_OP_2(0x80, 0x03)
#define ESP_BLE_MESH_MODEL_OP_APP_KEY_LIST                          ESP_BLE_MESH_MODEL_OP_2(0x80, 0x02)
#define ESP_BLE_MESH_MODEL_OP_NODE_IDENTITY_STATUS                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x48)
#define ESP_BLE_MESH_MODEL_OP_MODEL_APP_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x3E)
#define ESP_BLE_MESH_MODEL_OP_SIG_MODEL_APP_LIST                    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x4C)
#define ESP_BLE_MESH_MODEL_OP_VENDOR_MODEL_APP_LIST                 ESP_BLE_MESH_MODEL_OP_2(0x80, 0x4E)
#define ESP_BLE_MESH_MODEL_OP_NODE_RESET_STATUS                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x4A)
#define ESP_BLE_MESH_MODEL_OP_FRIEND_STATUS                         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x11)
#define ESP_BLE_MESH_MODEL_OP_KEY_REFRESH_PHASE_STATUS              ESP_BLE_MESH_MODEL_OP_2(0x80, 0x17)
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_PUB_STATUS                  ESP_BLE_MESH_MODEL_OP_1(0x06)
#define ESP_BLE_MESH_MODEL_OP_HEARTBEAT_SUB_STATUS                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x3C)
#define ESP_BLE_MESH_MODEL_OP_LPN_POLLTIMEOUT_STATUS                ESP_BLE_MESH_MODEL_OP_2(0x80, 0x2E)
#define ESP_BLE_MESH_MODEL_OP_NETWORK_TRANSMIT_STATUS               ESP_BLE_MESH_MODEL_OP_2(0x80, 0x25)

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

#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_GET                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x31) /*!< Health Fault Get */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_GET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x34) /*!< Health Period Get */
#define ESP_BLE_MESH_MODEL_OP_ATTENTION_GET                         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x04) /*!< Health Attention Get */

/**
 * esp_ble_mesh_opcode_health_client_set_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by esp_ble_mesh_health_client_set_state.
 * The following opcodes will only be used in the esp_ble_mesh_health_client_set_state function.
 */
typedef uint32_t esp_ble_mesh_opcode_health_client_set_t;

#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR                    ESP_BLE_MESH_MODEL_OP_2(0x80, 0x2F) /*!< Health Fault Clear */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_CLEAR_UNACK              ESP_BLE_MESH_MODEL_OP_2(0x80, 0x30) /*!< Health Fault Clear Unacknowledged */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x32) /*!< Health Fault Test */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_TEST_UNACK               ESP_BLE_MESH_MODEL_OP_2(0x80, 0x33) /*!< Health Fault Test Unacknowledged */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET                     ESP_BLE_MESH_MODEL_OP_2(0x80, 0x35) /*!< Health Period Set */
#define ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_SET_UNACK               ESP_BLE_MESH_MODEL_OP_2(0x80, 0x36) /*!< Health Period Set Unacknowledged */
#define ESP_BLE_MESH_MODEL_OP_ATTENTION_SET                         ESP_BLE_MESH_MODEL_OP_2(0x80, 0x05) /*!< Health Attention Set */
#define ESP_BLE_MESH_MODEL_OP_ATTENTION_SET_UNACK                   ESP_BLE_MESH_MODEL_OP_2(0x80, 0x06) /*!< Health Attention Set Unacknowledged */

/**
 * esp_ble_mesh_health_model_status_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by the Health Model messages.
 * The following opcodes are used by the BLE Mesh Health Server Model internally to
 * respond to the Health Client Model's request messages.
 */
typedef uint32_t esp_ble_mesh_health_model_status_t;

#define ESP_BLE_MESH_MODEL_OP_HEALTH_CURRENT_STATUS                 ESP_BLE_MESH_MODEL_OP_1(0x04)
#define ESP_BLE_MESH_MODEL_OP_HEALTH_FAULT_STATUS                   ESP_BLE_MESH_MODEL_OP_1(0x05)
#define ESP_BLE_MESH_MODEL_OP_HEALTH_PERIOD_STATUS                  ESP_BLE_MESH_MODEL_OP_2(0x80, 0x37)
#define ESP_BLE_MESH_MODEL_OP_ATTENTION_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x80, 0x07)

/**
 * esp_ble_mesh_generic_message_opcode_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by functions esp_ble_mesh_generic_client_get_state
 * & esp_ble_mesh_generic_client_set_state.
 */
typedef uint32_t esp_ble_mesh_generic_message_opcode_t;

/*!< Generic OnOff Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_GET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x01)
#define ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x02)
#define ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x03)
#define ESP_BLE_MESH_MODEL_OP_GEN_ONOFF_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x04)

/*!< Generic Level Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_GET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x05)
#define ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x06)
#define ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_SET_UNACK                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x07)
#define ESP_BLE_MESH_MODEL_OP_GEN_LEVEL_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x08)
#define ESP_BLE_MESH_MODEL_OP_GEN_DELTA_SET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x09)
#define ESP_BLE_MESH_MODEL_OP_GEN_DELTA_SET_UNACK                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x0A)
#define ESP_BLE_MESH_MODEL_OP_GEN_MOVE_SET                          ESP_BLE_MESH_MODEL_OP_2(0x82, 0x0B)
#define ESP_BLE_MESH_MODEL_OP_GEN_MOVE_SET_UNACK                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x0C)

/*!< Generic Default Transition Time Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_GET                ESP_BLE_MESH_MODEL_OP_2(0x82, 0x0D)
#define ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET                ESP_BLE_MESH_MODEL_OP_2(0x82, 0x0E)
#define ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_SET_UNACK          ESP_BLE_MESH_MODEL_OP_2(0x82, 0x0F)
#define ESP_BLE_MESH_MODEL_OP_GEN_DEF_TRANS_TIME_STATUS             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x10)

/*!< Generic Power OnOff Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_GET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x11)
#define ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_STATUS                  ESP_BLE_MESH_MODEL_OP_2(0x82, 0x12)

/*!< Generic Power OnOff Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x13)
#define ESP_BLE_MESH_MODEL_OP_GEN_ONPOWERUP_SET_UNACK               ESP_BLE_MESH_MODEL_OP_2(0x82, 0x14)

/*!< Generic Power Level Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_GET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x15)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x16)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_SET_UNACK             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x17)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LEVEL_STATUS                ESP_BLE_MESH_MODEL_OP_2(0x82, 0x18)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LAST_GET                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x19)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_LAST_STATUS                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x1A)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_GET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x1B)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_STATUS              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x1C)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_GET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x1D)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_STATUS                ESP_BLE_MESH_MODEL_OP_2(0x82, 0x1E)

/*!< Generic Power Level Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x1F)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_DEFAULT_SET_UNACK           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x20)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x21)
#define ESP_BLE_MESH_MODEL_OP_GEN_POWER_RANGE_SET_UNACK             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x22)

/*!< Generic Battery Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_BATTERY_GET                       ESP_BLE_MESH_MODEL_OP_2(0x82, 0x23)
#define ESP_BLE_MESH_MODEL_OP_GEN_BATTERY_STATUS                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x24)

/*!< Generic Location Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_GET                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x25)
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_STATUS                 ESP_BLE_MESH_MODEL_OP_1(0x40)
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_GET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x26)
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_STATUS                  ESP_BLE_MESH_MODEL_OP_2(0x82, 0x27)

/*!< Generic Location Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET                    ESP_BLE_MESH_MODEL_OP_1(0x41)
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_GLOBAL_SET_UNACK              ESP_BLE_MESH_MODEL_OP_1(0x42)
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x28)
#define ESP_BLE_MESH_MODEL_OP_GEN_LOC_LOCAL_SET_UNACK               ESP_BLE_MESH_MODEL_OP_2(0x82, 0x29)

/*!< Generic Manufacturer Property Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_GET       ESP_BLE_MESH_MODEL_OP_2(0x82, 0x2A)
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTIES_STATUS    ESP_BLE_MESH_MODEL_OP_1(0x43)
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_GET         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x2B)
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET         ESP_BLE_MESH_MODEL_OP_1(0x44)
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_SET_UNACK   ESP_BLE_MESH_MODEL_OP_1(0x45)
#define ESP_BLE_MESH_MODEL_OP_GEN_MANUFACTURER_PROPERTY_STATUS      ESP_BLE_MESH_MODEL_OP_1(0x46)

/*!< Generic Admin Property Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_GET              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x2C)
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTIES_STATUS           ESP_BLE_MESH_MODEL_OP_1(0x47)
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_GET                ESP_BLE_MESH_MODEL_OP_2(0x82, 0x2D)
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET                ESP_BLE_MESH_MODEL_OP_1(0x48)
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_SET_UNACK          ESP_BLE_MESH_MODEL_OP_1(0x49)
#define ESP_BLE_MESH_MODEL_OP_GEN_ADMIN_PROPERTY_STATUS             ESP_BLE_MESH_MODEL_OP_1(0x4A)

/*!< Generic User Property Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_GET               ESP_BLE_MESH_MODEL_OP_2(0x82, 0x2E)
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTIES_STATUS            ESP_BLE_MESH_MODEL_OP_1(0x4B)
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_GET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x2F)
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET                 ESP_BLE_MESH_MODEL_OP_1(0x4C)
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_SET_UNACK           ESP_BLE_MESH_MODEL_OP_1(0x4D)
#define ESP_BLE_MESH_MODEL_OP_GEN_USER_PROPERTY_STATUS              ESP_BLE_MESH_MODEL_OP_1(0x4E)

/*!< Generic Client Property Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_GET             ESP_BLE_MESH_MODEL_OP_1(0x4F)
#define ESP_BLE_MESH_MODEL_OP_GEN_CLIENT_PROPERTIES_STATUS          ESP_BLE_MESH_MODEL_OP_1(0x50)

/**
 * esp_ble_mesh_sensor_message_opcode_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by functions esp_ble_mesh_sensor_client_get_state
 * & esp_ble_mesh_sensor_client_set_state.
 */
typedef uint32_t esp_ble_mesh_sensor_message_opcode_t;

/*!< Sensor Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_GET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x30)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_DESCRIPTOR_STATUS              ESP_BLE_MESH_MODEL_OP_1(0x51)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_GET                            ESP_BLE_MESH_MODEL_OP_2(0x82, 0x31)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_STATUS                         ESP_BLE_MESH_MODEL_OP_1(0x52)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_GET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x32)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_COLUMN_STATUS                  ESP_BLE_MESH_MODEL_OP_1(0x53)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_GET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x33)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SERIES_STATUS                  ESP_BLE_MESH_MODEL_OP_1(0x54)

/*!< Sensor Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_GET                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x34)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET                    ESP_BLE_MESH_MODEL_OP_1(0x55)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_SET_UNACK              ESP_BLE_MESH_MODEL_OP_1(0x56)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_CADENCE_STATUS                 ESP_BLE_MESH_MODEL_OP_1(0x57)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_GET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x35)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTINGS_STATUS                ESP_BLE_MESH_MODEL_OP_1(0x58)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_GET                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x36)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET                    ESP_BLE_MESH_MODEL_OP_1(0x59)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_SET_UNACK              ESP_BLE_MESH_MODEL_OP_1(0x5A)
#define ESP_BLE_MESH_MODEL_OP_SENSOR_SETTING_STATUS                 ESP_BLE_MESH_MODEL_OP_1(0x5B)

/**
 * esp_ble_mesh_time_scene_message_opcode_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by functions esp_ble_mesh_time_scene_client_get_state
 * & esp_ble_mesh_time_scene_client_set_state.
 */
typedef uint32_t esp_ble_mesh_time_scene_message_opcode_t;

/*!< Time Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_TIME_GET                              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x37)
#define ESP_BLE_MESH_MODEL_OP_TIME_SET                              ESP_BLE_MESH_MODEL_OP_1(0x5C)
#define ESP_BLE_MESH_MODEL_OP_TIME_STATUS                           ESP_BLE_MESH_MODEL_OP_1(0x5D)
#define ESP_BLE_MESH_MODEL_OP_TIME_ROLE_GET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x38)
#define ESP_BLE_MESH_MODEL_OP_TIME_ROLE_SET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x39)
#define ESP_BLE_MESH_MODEL_OP_TIME_ROLE_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x3A)
#define ESP_BLE_MESH_MODEL_OP_TIME_ZONE_GET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x3B)
#define ESP_BLE_MESH_MODEL_OP_TIME_ZONE_SET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x3C)
#define ESP_BLE_MESH_MODEL_OP_TIME_ZONE_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x3D)
#define ESP_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_GET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x3E)
#define ESP_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_SET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x3F)
#define ESP_BLE_MESH_MODEL_OP_TAI_UTC_DELTA_STATUS                  ESP_BLE_MESH_MODEL_OP_2(0x82, 0x40)

/*!< Scene Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SCENE_GET                             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x41)
#define ESP_BLE_MESH_MODEL_OP_SCENE_RECALL                          ESP_BLE_MESH_MODEL_OP_2(0x82, 0x42)
#define ESP_BLE_MESH_MODEL_OP_SCENE_RECALL_UNACK                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x43)
#define ESP_BLE_MESH_MODEL_OP_SCENE_STATUS                          ESP_BLE_MESH_MODEL_OP_1(0x5E)
#define ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_GET                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x44)
#define ESP_BLE_MESH_MODEL_OP_SCENE_REGISTER_STATUS                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x45)

/*!< Scene Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SCENE_STORE                           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x46)
#define ESP_BLE_MESH_MODEL_OP_SCENE_STORE_UNACK                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x47)
#define ESP_BLE_MESH_MODEL_OP_SCENE_DELETE                          ESP_BLE_MESH_MODEL_OP_2(0x82, 0x9E)
#define ESP_BLE_MESH_MODEL_OP_SCENE_DELETE_UNACK                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x9F)

/*!< Scheduler Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_GET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x48)
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_STATUS                  ESP_BLE_MESH_MODEL_OP_1(0x5F)
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_GET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x49)
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x4A)

/*!< Scheduler Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET                     ESP_BLE_MESH_MODEL_OP_1(0x60)
#define ESP_BLE_MESH_MODEL_OP_SCHEDULER_ACT_SET_UNACK               ESP_BLE_MESH_MODEL_OP_1(0x61)

/**
 * esp_ble_mesh_light_message_opcode_t belongs to esp_ble_mesh_opcode_t, this typedef is
 * only used to locate the opcodes used by functions esp_ble_mesh_light_client_get_state
 * & esp_ble_mesh_light_client_set_state.
 */
typedef uint32_t esp_ble_mesh_light_message_opcode_t;

/*!< Light Lightness Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_GET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x4B)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x4C)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x4D)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS                ESP_BLE_MESH_MODEL_OP_2(0x82, 0x4E)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_GET            ESP_BLE_MESH_MODEL_OP_2(0x82, 0x4F)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET            ESP_BLE_MESH_MODEL_OP_2(0x82, 0x50)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x51)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x52)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_GET              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x53)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x54)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_GET           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x55)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS        ESP_BLE_MESH_MODEL_OP_2(0x82, 0x56)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_GET             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x57)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS          ESP_BLE_MESH_MODEL_OP_2(0x82, 0x58)

/*!< Light Lightness Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x59)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x5A)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x5B)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK       ESP_BLE_MESH_MODEL_OP_2(0x82, 0x5C)

/*!< Light CTL Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_GET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x5D)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x5E)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x5F)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x60)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_GET             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x61)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_GET       ESP_BLE_MESH_MODEL_OP_2(0x82, 0x62)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x63)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x64)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK       ESP_BLE_MESH_MODEL_OP_2(0x82, 0x65)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS          ESP_BLE_MESH_MODEL_OP_2(0x82, 0x66)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_GET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x67)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x68)

/*!< Light CTL Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x69)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x6A)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET       ESP_BLE_MESH_MODEL_OP_2(0x82, 0x6B)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK ESP_BLE_MESH_MODEL_OP_2(0x82, 0x6C)

/*!< Light HSL Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_GET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x6D)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_GET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x6E)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x6F)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK               ESP_BLE_MESH_MODEL_OP_2(0x82, 0x70)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS                  ESP_BLE_MESH_MODEL_OP_2(0x82, 0x71)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_GET              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x72)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x73)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK        ESP_BLE_MESH_MODEL_OP_2(0x82, 0x74)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x75)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x76)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x77)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x78)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_GET                  ESP_BLE_MESH_MODEL_OP_2(0x82, 0x79)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS               ESP_BLE_MESH_MODEL_OP_2(0x82, 0x7A)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_GET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x7B)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x7C)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_GET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x7D)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS                ESP_BLE_MESH_MODEL_OP_2(0x82, 0x7E)

/*!< Light HSL Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x7F)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x80)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x81)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x82)

/*!< Light xyL Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_GET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x83)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_SET                         ESP_BLE_MESH_MODEL_OP_2(0x82, 0x84)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x85)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x82, 0x86)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_GET                  ESP_BLE_MESH_MODEL_OP_2(0x82, 0x87)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS               ESP_BLE_MESH_MODEL_OP_2(0x82, 0x88)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_GET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x89)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x8A)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_GET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x8B)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS                ESP_BLE_MESH_MODEL_OP_2(0x82, 0x8C)

/*!< Light xyL Setup Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x8D)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x8E)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET                   ESP_BLE_MESH_MODEL_OP_2(0x82, 0x8F)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK             ESP_BLE_MESH_MODEL_OP_2(0x82, 0x90)

/*!< Light Control Message Opcode */
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_GET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x91)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET                     ESP_BLE_MESH_MODEL_OP_2(0x82, 0x92)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK               ESP_BLE_MESH_MODEL_OP_2(0x82, 0x93)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS                  ESP_BLE_MESH_MODEL_OP_2(0x82, 0x94)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_GET                       ESP_BLE_MESH_MODEL_OP_2(0x82, 0x95)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET                       ESP_BLE_MESH_MODEL_OP_2(0x82, 0x96)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x97)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS                    ESP_BLE_MESH_MODEL_OP_2(0x82, 0x98)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_GET              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x99)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET              ESP_BLE_MESH_MODEL_OP_2(0x82, 0x9A)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK        ESP_BLE_MESH_MODEL_OP_2(0x82, 0x9B)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS           ESP_BLE_MESH_MODEL_OP_2(0x82, 0x9C)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET                 ESP_BLE_MESH_MODEL_OP_2(0x82, 0x9D)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET                 ESP_BLE_MESH_MODEL_OP_1(0x62)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK           ESP_BLE_MESH_MODEL_OP_1(0x63)
#define ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS              ESP_BLE_MESH_MODEL_OP_1(0x64)

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
    uint32_t publish_status;                        /*!< Callback used to handle the received unsolicited message. Initialized by the stack. */
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
    uint16_t dst;       /*!< Destination address of the last message */
    int64_t  timestamp; /*!< Time when the last message is received */
} esp_ble_mesh_last_msg_info_t;

#define ESP_BLE_MESH_SERVER_RSP_BY_APP  0   /*!< Response need to be sent in the application */
#define ESP_BLE_MESH_SERVER_AUTO_RSP    1   /*!< Response will be sent internally */

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
        uint16_t hue;           /*!< The value of the Light HSL Hue state */
        uint16_t saturation;    /*!< The value of the Light HSL Saturation state */
    } light_hsl;                /*!< The Light HSL composite state */
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
    ESP_BLE_MESH_LIGHT_HSL_STATE,
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
        uint32_t opcode;                /*!< Opcode of the received message */
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
        uint32_t opcode;                /*!< Opcode of the unsolicited received message */
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

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_DEFS_H_ */
