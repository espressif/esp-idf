/** @file
 *  @brief Bluetooth Mesh Access Layer APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_ACCESS_H_
#define _BLE_MESH_ACCESS_H_

#include "mesh_config.h"
#include "mesh_buf.h"
#include "mesh_timer.h"

/**
 * @brief Bluetooth Mesh Access Layer
 * @defgroup bt_mesh_access Bluetooth Mesh Access Layer
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_CID_NVAL          0xFFFF

#define BLE_MESH_ADDR_UNASSIGNED   0x0000
#define BLE_MESH_ADDR_ALL_NODES    0xffff
#define BLE_MESH_ADDR_PROXIES      0xfffc
#define BLE_MESH_ADDR_FRIENDS      0xfffd
#define BLE_MESH_ADDR_RELAYS       0xfffe

#define BLE_MESH_KEY_UNUSED        0xffff
#define BLE_MESH_KEY_DEV           0xfffe

/** Helper to define a mesh element within an array.
 *
 *  In case the element has no SIG or Vendor models the helper
 *  macro BLE_MESH_MODEL_NONE can be given instead.
 *
 *  @param _loc       Location Descriptor.
 *  @param _mods      Array of models.
 *  @param _vnd_mods  Array of vendor models.
 */
#define BLE_MESH_ELEM(_loc, _mods, _vnd_mods)   \
{                                               \
    .loc              = (_loc),                 \
    .model_count      = ARRAY_SIZE(_mods),      \
    .models           = (_mods),                \
    .vnd_model_count  = ARRAY_SIZE(_vnd_mods),  \
    .vnd_models       = (_vnd_mods),            \
}

/** Abstraction that describes a Mesh Element */
struct bt_mesh_elem {
    /* Unicast Address. Set at runtime during provisioning. */
    uint16_t addr;

    /* Location Descriptor (GATT Bluetooth Namespace Descriptors) */
    const uint16_t loc;

    const uint8_t model_count;
    const uint8_t vnd_model_count;

    struct bt_mesh_model *const models;
    struct bt_mesh_model *const vnd_models;
};

/* Foundation Models */
#define BLE_MESH_MODEL_ID_CFG_SRV                   0x0000
#define BLE_MESH_MODEL_ID_CFG_CLI                   0x0001
#define BLE_MESH_MODEL_ID_HEALTH_SRV                0x0002
#define BLE_MESH_MODEL_ID_HEALTH_CLI                0x0003

/* Models from the Mesh Model Specification */
#define BLE_MESH_MODEL_ID_GEN_ONOFF_SRV             0x1000
#define BLE_MESH_MODEL_ID_GEN_ONOFF_CLI             0x1001
#define BLE_MESH_MODEL_ID_GEN_LEVEL_SRV             0x1002
#define BLE_MESH_MODEL_ID_GEN_LEVEL_CLI             0x1003
#define BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_SRV    0x1004
#define BLE_MESH_MODEL_ID_GEN_DEF_TRANS_TIME_CLI    0x1005
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SRV       0x1006
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_SETUP_SRV 0x1007
#define BLE_MESH_MODEL_ID_GEN_POWER_ONOFF_CLI       0x1008
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV       0x1009
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_SETUP_SRV 0x100a
#define BLE_MESH_MODEL_ID_GEN_POWER_LEVEL_CLI       0x100b
#define BLE_MESH_MODEL_ID_GEN_BATTERY_SRV           0x100c
#define BLE_MESH_MODEL_ID_GEN_BATTERY_CLI           0x100d
#define BLE_MESH_MODEL_ID_GEN_LOCATION_SRV          0x100e
#define BLE_MESH_MODEL_ID_GEN_LOCATION_SETUP_SRV    0x100f
#define BLE_MESH_MODEL_ID_GEN_LOCATION_CLI          0x1010
#define BLE_MESH_MODEL_ID_GEN_ADMIN_PROP_SRV        0x1011
#define BLE_MESH_MODEL_ID_GEN_MANUFACTURER_PROP_SRV 0x1012
#define BLE_MESH_MODEL_ID_GEN_USER_PROP_SRV         0x1013
#define BLE_MESH_MODEL_ID_GEN_CLIENT_PROP_SRV       0x1014
#define BLE_MESH_MODEL_ID_GEN_PROP_CLI              0x1015
#define BLE_MESH_MODEL_ID_SENSOR_SRV                0x1100
#define BLE_MESH_MODEL_ID_SENSOR_SETUP_SRV          0x1101
#define BLE_MESH_MODEL_ID_SENSOR_CLI                0x1102
#define BLE_MESH_MODEL_ID_TIME_SRV                  0x1200
#define BLE_MESH_MODEL_ID_TIME_SETUP_SRV            0x1201
#define BLE_MESH_MODEL_ID_TIME_CLI                  0x1202
#define BLE_MESH_MODEL_ID_SCENE_SRV                 0x1203
#define BLE_MESH_MODEL_ID_SCENE_SETUP_SRV           0x1204
#define BLE_MESH_MODEL_ID_SCENE_CLI                 0x1205
#define BLE_MESH_MODEL_ID_SCHEDULER_SRV             0x1206
#define BLE_MESH_MODEL_ID_SCHEDULER_SETUP_SRV       0x1207
#define BLE_MESH_MODEL_ID_SCHEDULER_CLI             0x1208
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV       0x1300
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV 0x1301
#define BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI       0x1302
#define BLE_MESH_MODEL_ID_LIGHT_CTL_SRV             0x1303
#define BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV       0x1304
#define BLE_MESH_MODEL_ID_LIGHT_CTL_CLI             0x1305
#define BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV        0x1306
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SRV             0x1307
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV       0x1308
#define BLE_MESH_MODEL_ID_LIGHT_HSL_CLI             0x1309
#define BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV         0x130a
#define BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV         0x130b
#define BLE_MESH_MODEL_ID_LIGHT_XYL_SRV             0x130c
#define BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV       0x130d
#define BLE_MESH_MODEL_ID_LIGHT_XYL_CLI             0x130e
#define BLE_MESH_MODEL_ID_LIGHT_LC_SRV              0x130f
#define BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV        0x1310
#define BLE_MESH_MODEL_ID_LIGHT_LC_CLI              0x1311

/** Message sending context. */
struct bt_mesh_msg_ctx {
    /** NetKey Index of the subnet to send the message on. */
    uint16_t net_idx;

    /** AppKey Index to encrypt the message with. */
    uint16_t app_idx;

    /** Remote address. */
    uint16_t addr;

    /** Destination address of a received message. Not used for sending. */
    uint16_t recv_dst;

    /** RSSI of received packet. Not used for sending. */
    int8_t  recv_rssi;

    /** Received TTL value. Not used for sending. */
    uint8_t  recv_ttl: 7;

    /** Force sending reliably by using segment acknowledgement */
    uint8_t  send_rel: 1;

    /** TTL, or BLE_MESH_TTL_DEFAULT for default TTL. */
    uint8_t  send_ttl;

    /** Change by Espressif, opcode of a received message.
     *  Not used for sending message. */
    uint32_t recv_op;

    /** Change by Espressif, model corresponds to the message */
    struct bt_mesh_model *model;

    /** Change by Espressif, if the message is sent by a server
     *  model. Not used for receiving message. */
    bool srv_send;
};

struct bt_mesh_model_op {
    /* OpCode encoded using the BLE_MESH_MODEL_OP_* macros */
    const uint32_t  opcode;

    /* Minimum required message length */
    const size_t min_len;

    /* Message handler for the opcode */
    void (*const func)(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *buf);
};

#define BLE_MESH_MODEL_OP_1(b0)         (b0)
#define BLE_MESH_MODEL_OP_2(b0, b1)     (((b0) << 8) | (b1))
#define BLE_MESH_MODEL_OP_3(b0, cid)    ((((b0) << 16) | 0xc00000) | (cid))

#define BLE_MESH_MODEL_OP_END           { 0, 0, NULL }
#define BLE_MESH_MODEL_NO_OPS           ((struct bt_mesh_model_op []) \
                                        { BLE_MESH_MODEL_OP_END })

/** Helper to define an empty model array */
#define BLE_MESH_MODEL_NONE             ((struct bt_mesh_model []){})

/** Length of a short Mesh MIC. */
#define BLE_MESH_MIC_SHORT              4
/** Length of a long Mesh MIC. */
#define BLE_MESH_MIC_LONG               8

/** @def BLE_MESH_MODEL_OP_LEN
 *
 * @brief Helper to determine the length of an opcode.
 *
 * @param _op Opcode.
 */
#define BLE_MESH_MODEL_OP_LEN(_op) ((_op) <= 0xff ? 1 : (_op) <= 0xffff ? 2 : 3)

/** @def BLE_MESH_MODEL_BUF_LEN
 *
 * @brief Helper for model message buffer length.
 *
 * Returns the length of a Mesh model message buffer, including the opcode
 * length and a short MIC.
 *
 * @param _op Opcode of the message.
 * @param _payload_len Length of the model payload.
 */
#define BLE_MESH_MODEL_BUF_LEN(_op, _payload_len)                \
        (BLE_MESH_MODEL_OP_LEN(_op) + (_payload_len) + BLE_MESH_MIC_SHORT)

/** @def BLE_MESH_MODEL_BUF_LEN_LONG_MIC
 *
 * @brief Helper for model message buffer length.
 *
 * Returns the length of a Mesh model message buffer, including the opcode
 * length and a long MIC.
 *
 * @param _op Opcode of the message.
 * @param _payload_len Length of the model payload.
 */
#define BLE_MESH_MODEL_BUF_LEN_LONG_MIC(_op, _payload_len)       \
        (BLE_MESH_MODEL_OP_LEN(_op) + (_payload_len) + BLE_MESH_MIC_LONG)

/** @def BLE_MESH_MODEL_BUF_DEFINE
 *
 * @brief Define a Mesh model message buffer using @ref NET_BUF_SIMPLE_DEFINE.
 *
 * @param _buf Buffer name.
 * @param _op Opcode of the message.
 * @param _payload_len Length of the model message payload.
 */
#define BLE_MESH_MODEL_BUF_DEFINE(_buf, _op, _payload_len)       \
        NET_BUF_SIMPLE_DEFINE(_buf, BLE_MESH_MODEL_BUF_LEN((_op), (_payload_len)))

/** @def BLE_MESH_MODEL_CB
 *
 *  @brief Composition data SIG model entry with callback functions.
 *
 *  @param _id        Model ID.
 *  @param _op        Array of model opcode handlers.
 *  @param _pub       Model publish parameters.
 *  @param _user_data User data for the model.
 *  @param _cb        Callback structure, or NULL to keep no callbacks.
 */
#define BLE_MESH_MODEL_CB(_id, _op, _pub, _user_data, _cb)          \
{                                                                   \
    .id = (_id),                                                    \
    .op = (_op),                                                    \
    .keys = { [0 ... (CONFIG_BLE_MESH_MODEL_KEY_COUNT - 1)] =       \
            BLE_MESH_KEY_UNUSED },                                  \
    .pub = (_pub),                                                  \
    .groups = { [0 ... (CONFIG_BLE_MESH_MODEL_GROUP_COUNT - 1)] =   \
            BLE_MESH_ADDR_UNASSIGNED },                             \
    .user_data = (_user_data),                                      \
    .cb = (_cb),                                                    \
}

/** @def BLE_MESH_MODEL_VND_CB
 *
 *  @brief Composition data vendor model entry with callback functions.
 *
 *  @param _company   Company ID.
 *  @param _id        Model ID.
 *  @param _op        Array of model opcode handlers.
 *  @param _pub       Model publish parameters.
 *  @param _user_data User data for the model.
 *  @param _cb        Callback structure, or NULL to keep no callbacks.
 */
#define BLE_MESH_MODEL_VND_CB(_company, _id, _op, _pub, _user_data, _cb) \
{                                                                   \
    .vnd.company = (_company),                                      \
    .vnd.id = (_id),                                                \
    .op = (_op),                                                    \
    .pub = (_pub),                                                  \
    .keys = { [0 ... (CONFIG_BLE_MESH_MODEL_KEY_COUNT - 1)] =       \
            BLE_MESH_KEY_UNUSED },                                  \
    .groups = { [0 ... (CONFIG_BLE_MESH_MODEL_GROUP_COUNT - 1)] =   \
            BLE_MESH_ADDR_UNASSIGNED },                             \
    .user_data = (_user_data),                                      \
    .cb = (_cb),                                                    \
}

/** @def BLE_MESH_TRANSMIT
 *
 *  @brief Encode transmission count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0
 *                 and a multiple of 10.
 *
 *  @return Mesh transmit value that can be used e.g. for the default
 *          values of the configuration model data.
 */
#define BLE_MESH_TRANSMIT(count, int_ms) ((count) | ((((int_ms) / 10) - 1) << 3))

/** @def BLE_MESH_TRANSMIT_COUNT
 *
 *  @brief Decode transmit count from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission count (actual transmissions is N + 1).
 */
#define BLE_MESH_TRANSMIT_COUNT(transmit) (((transmit) & (uint8_t)BIT_MASK(3)))

/** @def BLE_MESH_TRANSMIT_INT
 *
 *  @brief Decode transmit interval from a transmit value.
 *
 *  @param transmit Encoded transmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define BLE_MESH_TRANSMIT_INT(transmit) ((((transmit) >> 3) + 1) * 10)

/** @def BLE_MESH_PUB_TRANSMIT
 *
 *  @brief Encode Publish Retransmit count & interval steps.
 *
 *  @param count   Number of retransmissions (first transmission is excluded).
 *  @param int_ms  Interval steps in milliseconds. Must be greater than 0
 *                 and a multiple of 50.
 *
 *  @return Mesh transmit value that can be used e.g. for the default
 *          values of the configuration model data.
 */
#define BLE_MESH_PUB_TRANSMIT(count, int_ms) BLE_MESH_TRANSMIT((count), (int_ms) / 5)

/** @def BLE_MESH_PUB_TRANSMIT_COUNT
 *
 *  @brief Decode Publish Retransmit count from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Retransmission count (actual transmissions is N + 1).
 */
#define BLE_MESH_PUB_TRANSMIT_COUNT(transmit) BLE_MESH_TRANSMIT_COUNT(transmit)

/** @def BLE_MESH_PUB_TRANSMIT_INT
 *
 *  @brief Decode Publish Retransmit interval from a given value.
 *
 *  @param transmit Encoded Publish Retransmit count & interval value.
 *
 *  @return Transmission interval in milliseconds.
 */
#define BLE_MESH_PUB_TRANSMIT_INT(transmit) ((((transmit) >> 3) + 1) * 50)

/** Model publication context. */
struct bt_mesh_model_pub {
    /** The model the context belongs to. Initialized by the stack. */
    struct bt_mesh_model *mod;

    uint16_t addr;          /**< Publish Address. */
    uint16_t key:12,        /**< Publish AppKey Index. */
             cred:1,        /**< Friendship Credentials Flag. */
             send_rel:1;    /**< Force reliable sending (segment acks) */

    uint8_t  ttl;           /**< Publish Time to Live. */
    uint8_t  retransmit;    /**< Retransmit Count & Interval Steps. */
    uint8_t  period;        /**< Publish Period. */
    uint8_t  period_div:4,  /**< Divisor for the Period. */
             fast_period:1, /**< Use FastPeriodDivisor */
             count:3;       /**< Retransmissions left. */

    uint32_t period_start;  /**< Start of the current period. */

    /** @brief Publication buffer, containing the publication message.
     *
     *  This will get correctly created when the publication context
     *  has been defined using the BLE_MESH_MODEL_PUB_DEFINE macro.
     *
     *  BLE_MESH_MODEL_PUB_DEFINE(name, update, size);
     */
    struct net_buf_simple *msg;

    /** @brief Callback for updating the publication buffer.
     *
     *  When set to NULL, the model is assumed not to support
     *  periodic publishing. When set to non-NULL the callback
     *  will be called periodically and is expected to update
     *  @ref bt_mesh_model_pub.msg with a valid publication
     *  message.
     *
     *  If the callback returns non-zero, the publication is skipped
     *  and will resume on the next periodic publishing interval.
     *
     *  @param mod The Model the Publication Context belongs to.
     *
     *  @return Zero on success or (negative) error code otherwise.
     */
    int (*update)(struct bt_mesh_model *mod);

    /** Publish Period Timer. Only for stack-internal use. */
    struct k_delayed_work timer;

    /* Change by Espressif, role of the device going to publish messages */
    uint8_t dev_role;
};

/** @def BLE_MESH_MODEL_PUB_DEFINE
 *
 *  Define a model publication context.
 *
 *  @param _name Variable name given to the context.
 *  @param _update Optional message update callback (may be NULL).
 *  @param _msg_len Length of the publication message.
 */
#define BLE_MESH_MODEL_PUB_DEFINE(_name, _update, _msg_len) \
    NET_BUF_SIMPLE_DEFINE_STATIC(bt_mesh_pub_msg_##_name, _msg_len); \
    static struct bt_mesh_model_pub _name = { \
        .update = _update, \
        .msg = &bt_mesh_pub_msg_##_name, \
    }

/** Model callback functions. */
struct bt_mesh_model_cb {
    /** @brief Model init callback.
     *
     *  Called on every model instance during mesh initialization.
     *
     *  If any of the model init callbacks return an error, the mesh
     *  subsystem initialization will be aborted, and the error will
     *  be returned to the caller of @ref bt_mesh_init.
     *
     *  @param model Model to be initialized.
     *
     *  @return 0 on success, error otherwise.
     */
    int (*const init)(struct bt_mesh_model *model);

#if CONFIG_BLE_MESH_DEINIT
    /** @brief Model deinit callback.
     *
     *  Called on every model instance during mesh deinitialization.
     *  All model data is deleted, and the model should clear its state.
     *
     *  If any of the model deinit callbacks return an error, the mesh
     *  subsystem deinitialization will be aborted, and the error will
     *  be returned to the caller of @ref bt_mesh_deinit.
     *
     *  @param model Model to be de-initialized.
     */
    int (*const deinit)(struct bt_mesh_model *model);
#endif /* CONFIG_BLE_MESH_DEINIT */
};

/** Abstraction that describes a Mesh Model instance */
struct bt_mesh_model {
    union {
        const uint16_t id;
        struct {
            uint16_t company;
            uint16_t id;
        } vnd;
    };

    /* Internal information, mainly for persistent storage */
    uint8_t  elem_idx;  /* Belongs to Nth element */
    uint8_t  model_idx; /* Is the Nth model in the element */
    uint16_t flags;     /* Information about what has changed */

    /* The Element this Model belongs to */
    struct bt_mesh_elem *elem;

    /* Model Publication */
    struct bt_mesh_model_pub *const pub;

    /* AppKey List */
    uint16_t keys[CONFIG_BLE_MESH_MODEL_KEY_COUNT];

    /* Subscription List (group or virtual addresses) */
    uint16_t groups[CONFIG_BLE_MESH_MODEL_GROUP_COUNT];

    /** Opcode handler list */
    const struct bt_mesh_model_op *const op;

    /** Model callback structure. */
    const struct bt_mesh_model_cb *const cb;

    /* Model-specific user data */
    void *user_data;
};

struct bt_mesh_send_cb {
    void (*start)(uint16_t duration, int err, void *cb_data);
    void (*end)(int err, void *cb_data);
};

void bt_mesh_model_msg_init(struct net_buf_simple *msg, uint32_t opcode);

/** Special TTL value to request using configured default TTL */
#define BLE_MESH_TTL_DEFAULT    0xff

/** Maximum allowed TTL value */
#define BLE_MESH_TTL_MAX        0x7f

/**
 * @brief Send an Access Layer message.
 *
 * @param model     Mesh (client) Model that the message belongs to.
 * @param ctx       Message context, includes keys, TTL, etc.
 * @param msg       Access Layer payload (the actual message to be sent).
 * @param cb        Optional "message sent" callback.
 * @param cb_data   User data to be passed to the callback.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_model_send(struct bt_mesh_model *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple *msg,
                       const struct bt_mesh_send_cb *cb,
                       void *cb_data);

/**
 * @brief Send a model publication message.
 *
 * Before calling this function, the user needs to ensure that the model
 * publication message (@ref bt_mesh_model_pub.msg) contains a valid
 * message to be sent. Note that this API is only to be used for
 * non-period publishing. For periodic publishing the app only needs
 * to make sure that @ref bt_mesh_model_pub.msg contains a valid message
 * whenever the @ref bt_mesh_model_pub.update callback is called.
 *
 * @param model  Mesh (client) Model that's publishing the message.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_model_publish(struct bt_mesh_model *model);

/**
 * @brief Get the element that a model belongs to.
 *
 * @param mod  Mesh model.
 *
 * @return Pointer to the element that the given model belongs to.
 */
struct bt_mesh_elem *bt_mesh_model_elem(struct bt_mesh_model *mod);

/** @brief Find a SIG model.
 *
 *  @param elem Element to search for the model in.
 *  @param id   Model ID of the model.
 *
 *  @return A pointer to the Mesh model matching the given parameters, or NULL
 *          if no SIG model with the given ID exists in the given element.
 */
struct bt_mesh_model *bt_mesh_model_find(struct bt_mesh_elem *elem, uint16_t id);

/** @brief Find a vendor model.
 *
 *  @param elem    Element to search for the model in.
 *  @param company Company ID of the model.
 *  @param id      Model ID of the model.
 *
 *  @return A pointer to the Mesh model matching the given parameters, or NULL
 *          if no vendor model with the given ID exists in the given element.
 */
struct bt_mesh_model *bt_mesh_model_find_vnd(struct bt_mesh_elem *elem,
                                             uint16_t company, uint16_t id);

/** @brief Get whether the model is in the primary element of the device.
 *
 *  @param mod Mesh model.
 *
 *  @return true if the model is on the primary element, false otherwise.
 */
static inline bool bt_mesh_model_in_primary(const struct bt_mesh_model *mod)
{
    return (mod->elem_idx == 0);
}

/** Node Composition */
struct bt_mesh_comp {
    uint16_t cid;
    uint16_t pid;
    uint16_t vid;

    size_t elem_count;
    struct bt_mesh_elem *elem;
};

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __BLE_MESH_ACCESS_H */
