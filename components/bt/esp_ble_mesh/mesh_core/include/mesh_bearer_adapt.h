/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_BEARER_ADRPT_H_
#define _BLE_MESH_BEARER_ADRPT_H_

#include <string.h>
#include "mesh_types.h"
#include "mesh_util.h"
#include "mesh_buf.h"
#include "mesh_uuid.h"

/* BLE Mesh Max Connection Count */
#ifdef CONFIG_BT_BLUEDROID_ENABLED
#define BLE_MESH_MAX_CONN   \
    MIN(CONFIG_BT_ACL_CONNECTIONS, CONFIG_BTDM_CONTROLLER_BLE_MAX_CONN)

#define ADV_TASK_CORE TASK_PINNED_TO_CORE
#endif

#ifdef CONFIG_BT_NIMBLE_ENABLED
#define BLE_MESH_MAX_CONN   CONFIG_BT_NIMBLE_MAX_CONNECTIONS

#ifdef CONFIG_BT_NIMBLE_PINNED_TO_CORE
#define ADV_TASK_CORE              (CONFIG_BT_NIMBLE_PINNED_TO_CORE < portNUM_PROCESSORS ? CONFIG_BT_NIMBLE_PINNED_TO_CORE : tskNO_AFFINITY)
#else
#define ADV_TASK_CORE              (0)
#endif

#endif

/* BD ADDR types */
#define BLE_MESH_ADDR_PUBLIC         0x00
#define BLE_MESH_ADDR_RANDOM         0x01
#define BLE_MESH_ADDR_PUBLIC_ID      0x02
#define BLE_MESH_ADDR_RANDOM_ID      0x03

/* BD ADDR length */
#define BLE_MESH_ADDR_LEN                   0x06

/* Advertising types */
#define BLE_MESH_ADV_IND                    0x00
#define BLE_MESH_ADV_DIRECT_IND             0x01
#define BLE_MESH_ADV_SCAN_IND               0x02
#define BLE_MESH_ADV_NONCONN_IND            0x03
#define BLE_MESH_ADV_DIRECT_IND_LOW_DUTY    0x04

/* advertising channel map */
#define BLE_MESH_ADV_CHNL_37                BIT(0)
#define BLE_MESH_ADV_CHNL_38                BIT(1)
#define BLE_MESH_ADV_CHNL_39                BIT(2)

/* Advertising filter policy */
#define BLE_MESH_AP_SCAN_CONN_ALL           0x00
#define BLE_MESH_AP_SCAN_WL_CONN_ALL        0x01
#define BLE_MESH_AP_SCAN_ALL_CONN_WL        0x02
#define BLE_MESH_AP_SCAN_CONN_WL            0x03

/* Scan types */
#define BLE_MESH_SCAN_PASSIVE               0x00
#define BLE_MESH_SCAN_ACTIVE                0x01

/* Scan operation */
#define BLE_MESH_SCAN_DISABLE               0x00
#define BLE_MESH_SCAN_ENABLE                0x01

/* Scan duplicate operation */
#define BLE_MESH_SCAN_FILTER_DUP_DISABLE    0x00
#define BLE_MESH_SCAN_FILTER_DUP_ENABLE     0x01

/* Scan filter policy */
#define BLE_MESH_SP_ADV_ALL                 0x00
#define BLE_MESH_SP_ADV_WL                  0x01
#define BLE_MESH_SP_ADV_ALL_RPA_DIR_ADV     0x02
#define BLE_MESH_SP_ADV_WL_RPA_DIR_ADV      0x03

/* Error codes for Error response PDU */
#define BLE_MESH_ATT_ERR_INVALID_HANDLE             0x01
#define BLE_MESH_ATT_ERR_READ_NOT_PERMITTED         0x02
#define BLE_MESH_ATT_ERR_WRITE_NOT_PERMITTED        0x03
#define BLE_MESH_ATT_ERR_INVALID_PDU                0x04
#define BLE_MESH_ATT_ERR_AUTHENTICATION             0x05
#define BLE_MESH_ATT_ERR_NOT_SUPPORTED              0x06
#define BLE_MESH_ATT_ERR_INVALID_OFFSET             0x07
#define BLE_MESH_ATT_ERR_AUTHORIZATION              0x08
#define BLE_MESH_ATT_ERR_PREPARE_QUEUE_FULL         0x09
#define BLE_MESH_ATT_ERR_ATTRIBUTE_NOT_FOUND        0x0a
#define BLE_MESH_ATT_ERR_ATTRIBUTE_NOT_LONG         0x0b
#define BLE_MESH_ATT_ERR_ENCRYPTION_KEY_SIZE        0x0c
#define BLE_MESH_ATT_ERR_INVALID_ATTRIBUTE_LEN      0x0d
#define BLE_MESH_ATT_ERR_UNLIKELY                   0x0e
#define BLE_MESH_ATT_ERR_INSUFFICIENT_ENCRYPTION    0x0f
#define BLE_MESH_ATT_ERR_UNSUPPORTED_GROUP_TYPE     0x10
#define BLE_MESH_ATT_ERR_INSUFFICIENT_RESOURCES     0x11

/* Common Profile Error Codes (from CSS) */
#define BLE_MESH_ATT_ERR_WRITE_REQ_REJECTED         0xfc
#define BLE_MESH_ATT_ERR_CCC_IMPROPER_CONF          0xfd
#define BLE_MESH_ATT_ERR_PROCEDURE_IN_PROGRESS      0xfe
#define BLE_MESH_ATT_ERR_OUT_OF_RANGE               0xff

/* EIR/AD data type definitions */
#define BLE_MESH_DATA_FLAGS                 0x01 /* AD flags */
#define BLE_MESH_DATA_UUID16_SOME           0x02 /* 16-bit UUID, more available */
#define BLE_MESH_DATA_UUID16_ALL            0x03 /* 16-bit UUID, all listed */
#define BLE_MESH_DATA_UUID32_SOME           0x04 /* 32-bit UUID, more available */
#define BLE_MESH_DATA_UUID32_ALL            0x05 /* 32-bit UUID, all listed */
#define BLE_MESH_DATA_UUID128_SOME          0x06 /* 128-bit UUID, more available */
#define BLE_MESH_DATA_UUID128_ALL           0x07 /* 128-bit UUID, all listed */
#define BLE_MESH_DATA_NAME_SHORTENED        0x08 /* Shortened name */
#define BLE_MESH_DATA_NAME_COMPLETE         0x09 /* Complete name */
#define BLE_MESH_DATA_TX_POWER              0x0a /* Tx Power */
#define BLE_MESH_DATA_SOLICIT16             0x14 /* Solicit UUIDs, 16-bit */
#define BLE_MESH_DATA_SOLICIT128            0x15 /* Solicit UUIDs, 128-bit */
#define BLE_MESH_DATA_SVC_DATA16            0x16 /* Service data, 16-bit UUID */
#define BLE_MESH_DATA_GAP_APPEARANCE        0x19 /* GAP appearance */
#define BLE_MESH_DATA_SOLICIT32             0x1f /* Solicit UUIDs, 32-bit */
#define BLE_MESH_DATA_SVC_DATA32            0x20 /* Service data, 32-bit UUID */
#define BLE_MESH_DATA_SVC_DATA128           0x21 /* Service data, 128-bit UUID */
#define BLE_MESH_DATA_URI                   0x24 /* URI */
#define BLE_MESH_DATA_MESH_PROV             0x29 /* Mesh Provisioning PDU */
#define BLE_MESH_DATA_MESH_MESSAGE          0x2a /* Mesh Networking PDU */
#define BLE_MESH_DATA_MESH_BEACON           0x2b /* Mesh Beacon */

#define BLE_MESH_DATA_MANUFACTURER_DATA     0xff /* Manufacturer Specific Data */

#define BLE_MESH_AD_LIMITED     0x01 /* Limited Discoverable */
#define BLE_MESH_AD_GENERAL     0x02 /* General Discoverable */
#define BLE_MESH_AD_NO_BREDR    0x04 /* BR/EDR not supported */

/* Client Characteristic Configuration Values */

/** @def BLE_MESH_GATT_CCC_NOTIFY
 *  @brief Client Characteristic Configuration Notification.
 *
 *  If set, changes to Characteristic Value shall be notified.
 */
#define BLE_MESH_GATT_CCC_NOTIFY        0x0001

/** @def BLE_MESH_GATT_CCC_INDICATE
 *  @brief Client Characteristic Configuration Indication.
 *
 *  If set, changes to Characteristic Value shall be indicated.
 */
#define BLE_MESH_GATT_CCC_INDICATE      0x0002

/**  @def BLE_MESH_GATT_ERR
  *  @brief Construct error return value for attribute read and write callbacks.
  *
  *  @param _att_err ATT error code
  *
  *  @return Appropriate error code for the attribute callbacks.
  *
  */
#define BLE_MESH_GATT_ERR(_att_err)     (-(_att_err))

enum {
    BLE_MESH_GATT_ITER_STOP = 0,
    BLE_MESH_GATT_ITER_CONTINUE,
};

/* GATT attribute permission bit field values */
enum {
    /** No operations supported, e.g. for notify-only */
    BLE_MESH_GATT_PERM_NONE = 0,

    /** Attribute read permission. */
    BLE_MESH_GATT_PERM_READ = BIT(0),

    /** Attribute write permission. */
    BLE_MESH_GATT_PERM_WRITE = BIT(1),

    /** Attribute read permission with encryption.
     *
     *  If set, requires encryption for read access.
     */
    BLE_MESH_GATT_PERM_READ_ENCRYPT = BIT(2),

    /** Attribute write permission with encryption.
     *
     *  If set, requires encryption for write access.
     */
    BLE_MESH_GATT_PERM_WRITE_ENCRYPT = BIT(3),

    /** Attribute read permission with authentication.
     *
     *  If set, requires encryption using authenticated link-key for read
     *  access.
     */
    BLE_MESH_GATT_PERM_READ_AUTHEN = BIT(4),

    /** Attribute write permission with authentication.
     *
     *  If set, requires encryption using authenticated link-key for write
     *  access.
     */
    BLE_MESH_GATT_PERM_WRITE_AUTHEN = BIT(5),

    /** Attribute prepare write permission.
     *
     *  If set, allows prepare writes with use of BT_GATT_WRITE_FLAG_PREPARE
     *  passed to write callback.
     */
    BLE_MESH_GATT_PERM_PREPARE_WRITE = BIT(6),
};

/** Advertising options */
enum {
    /** Convenience value when no options are specified. */
    BLE_MESH_ADV_OPT_NONE = 0,

    /** Advertise as connectable. Type of advertising is determined by
     * providing SCAN_RSP data and/or enabling local privacy support.
     */
    BLE_MESH_ADV_OPT_CONNECTABLE = BIT(0),

    /** Don't try to resume connectable advertising after a connection.
     *  This option is only meaningful when used together with
     *  BLE_MESH_ADV_OPT_CONNECTABLE. If set the advertising will be stopped
     *  when bt_le_adv_stop() is called or when an incoming (slave)
     *  connection happens. If this option is not set the stack will
     *  take care of keeping advertising enabled even as connections
     *  occur.
     */
    BLE_MESH_ADV_OPT_ONE_TIME = BIT(1),
};

/* Defined GAP timers */
#define BLE_MESH_GAP_SCAN_FAST_INTERVAL     0x0060  /* 60 ms    */
#define BLE_MESH_GAP_SCAN_FAST_WINDOW       0x0030  /* 30 ms    */
#define BLE_MESH_GAP_SCAN_SLOW_INTERVAL_1   0x0800  /* 1.28 s   */
#define BLE_MESH_GAP_SCAN_SLOW_WINDOW_1     0x0012  /* 11.25 ms */
#define BLE_MESH_GAP_SCAN_SLOW_INTERVAL_2   0x1000  /* 2.56 s   */
#define BLE_MESH_GAP_SCAN_SLOW_WINDOW_2     0x0012  /* 11.25 ms */
#define BLE_MESH_GAP_ADV_FAST_INT_MIN_0     0x0020  /* 20 ms    */
#define BLE_MESH_GAP_ADV_FAST_INT_MAX_0     0x0020  /* 20 ms    */
#define BLE_MESH_GAP_ADV_FAST_INT_MIN_1     0x0030  /* 30 ms    */
#define BLE_MESH_GAP_ADV_FAST_INT_MAX_1     0x0060  /* 60 ms    */
#define BLE_MESH_GAP_ADV_FAST_INT_MIN_2     0x00a0  /* 100 ms   */
#define BLE_MESH_GAP_ADV_FAST_INT_MAX_2     0x00f0  /* 150 ms   */
#define BLE_MESH_GAP_ADV_SLOW_INT_MIN       0x0320  /* 500 ms   */
#define BLE_MESH_GAP_ADV_SLOW_INT_MAX       0x0320  /* 500 ms   */
#define BLE_MESH_GAP_INIT_CONN_INT_MIN      0x0018  /* 30 ms    */
#define BLE_MESH_GAP_INIT_CONN_INT_MAX      0x0028  /* 50 ms    */

/* Characteristic Properties Bit field values */

/** @def BLE_MESH_GATT_CHRC_BROADCAST
 *  @brief Characteristic broadcast property.
 *
 *  If set, permits broadcasts of the Characteristic Value using Server
 *  Characteristic Configuration Descriptor.
 */
#define BLE_MESH_GATT_CHRC_BROADCAST              0x01

/** @def BLE_MESH_GATT_CHRC_READ
 *  @brief Characteristic read property.
 *
 *  If set, permits reads of the Characteristic Value.
 */
#define BLE_MESH_GATT_CHRC_READ                   0x02

/** @def BLE_MESH_GATT_CHRC_WRITE_WITHOUT_RESP
 *  @brief Characteristic write without response property.
 *
 *  If set, permits write of the Characteristic Value without response.
 */
#define BLE_MESH_GATT_CHRC_WRITE_WITHOUT_RESP     0x04

/** @def BLE_MESH_GATT_CHRC_WRITE
 *  @brief Characteristic write with response property.
 *
 *  If set, permits write of the Characteristic Value with response.
 */
#define BLE_MESH_GATT_CHRC_WRITE                  0x08

/** @def BLE_MESH_GATT_CHRC_NOTIFY
 *  @brief Characteristic notify property.
 *
 *  If set, permits notifications of a Characteristic Value without
 *  acknowledgment.
 */
#define BLE_MESH_GATT_CHRC_NOTIFY                 0x10

/** @def BLE_MESH_GATT_CHRC_INDICATE
 *  @brief Characteristic indicate property.
 *
 * If set, permits indications of a Characteristic Value with acknowledgment.
 */
#define BLE_MESH_GATT_CHRC_INDICATE               0x20

/** @def BLE_MESH_GATT_CHRC_AUTH
 *  @brief Characteristic Authenticated Signed Writes property.
 *
 *  If set, permits signed writes to the Characteristic Value.
 */
#define BLE_MESH_GATT_CHRC_AUTH                   0x40

/** @def BLE_MESH_GATT_CHRC_EXT_PROP
 *  @brief Characteristic Extended Properties property.
 *
 * If set, additional characteristic properties are defined in the
 * Characteristic Extended Properties Descriptor.
 */
#define BLE_MESH_GATT_CHRC_EXT_PROP               0x80

/** @brief Characteristic Attribute Value. */
struct bt_mesh_gatt_char {
    /** Characteristic UUID. */
    const struct bt_mesh_uuid *uuid;
    /** Characteristic properties. */
    u8_t                        properties;
};

/** @brief GATT Service structure */
struct bt_mesh_gatt_service {
    /** Service Attributes */
    struct bt_mesh_gatt_attr *attrs;
    /** Service Attribute count */
    u16_t                      attr_count;
    sys_snode_t                node;
};

struct bt_mesh_ecb_param {
    u8_t key[16];
    u8_t clear_text[16];
    u8_t cipher_text[16];
} __packed;

typedef struct {
    u8_t type;
    u8_t val[6];
} bt_mesh_addr_t;

/** Description of different data types that can be encoded into
  * advertising data. Used to form arrays that are passed to the
  * bt_le_adv_start() function.
  */
struct bt_mesh_adv_data {
    u8_t type;
    u8_t data_len;
    const u8_t *data;
};

/** @brief Helper to declare elements of bt_data arrays
 *
 *  This macro is mainly for creating an array of struct
 *  bt_mesh_adv_data elements which is then passed to
 *  bt_le_adv_start().
 *
 *  @param _type Type of advertising data field
 *  @param _data Pointer to the data field payload
 *  @param _data_len Number of bytes behind the _data pointer
 */
#define BLE_MESH_ADV_DATA(_type, _data, _data_len)  \
    {                                               \
        .type = (_type),                            \
        .data_len = (_data_len),                    \
        .data = (const u8_t *)(_data),              \
    }

/** @brief Helper to declare elements of bt_data arrays
 *
 *  This macro is mainly for creating an array of struct bt_mesh_adv_data
 *  elements which is then passed to bt_le_adv_start().
 *
 *  @param _type Type of advertising data field
 *  @param _bytes Variable number of single-byte parameters
 */
#define BLE_MESH_ADV_DATA_BYTES(_type, _bytes...)       \
    BLE_MESH_ADV_DATA(_type, ((u8_t []) { _bytes }),    \
        sizeof((u8_t []) { _bytes }))

/* BLE Mesh Advertising Parameters */
struct bt_mesh_adv_param {
    /** Bit-field of advertising options */
    u8_t  options;

    /** Minimum Advertising Interval (N * 0.625) */
    u16_t interval_min;

    /** Maximum Advertising Interval (N * 0.625) */
    u16_t interval_max;
};

/* BLE Mesh scan parameters */
struct bt_mesh_scan_param {
    /** Scan type (BLE_MESH_SCAN_ACTIVE or BLE_MESH_SCAN_PASSIVE) */
    u8_t  type;

    /** Duplicate filtering (BLE_MESH_SCAN_FILTER_DUP_ENABLE or
     *  BLE_MESH_SCAN_FILTER_DUP_DISABLE)
     */
    u8_t  filter_dup;

    /** Scan interval (N * 0.625 ms) */
    u16_t interval;

    /** Scan window (N * 0.625 ms) */
    u16_t window;
};

struct bt_mesh_conn {
    u16_t    handle;
    bt_mesh_atomic_t ref;
};

/** @typedef bt_mesh_scan_cb_t
 *  @brief Callback type for reporting LE scan results.
 *
 *  A function of this type is given to the bt_le_scan_start() function
 *  and will be called for any discovered LE device.
 *
 *  @param addr Advertiser LE address and type.
 *  @param rssi Strength of advertiser signal.
 *  @param adv_type Type of advertising response from advertiser.
 *  @param data Buffer containing advertiser data.
 */
typedef void bt_mesh_scan_cb_t(const bt_mesh_addr_t *addr, s8_t rssi,
                               u8_t adv_type, struct net_buf_simple *buf);

/*  @typedef bt_mesh_dh_key_cb_t
 *  @brief Callback type for DH Key calculation.
 *
 *  Used to notify of the calculated DH Key.
 *
 *  @param key Public key.
 *  @param idx Provisioning link index, only used by Provisioner.
 *
 *  @return The DH Key, or NULL in case of failure.
 */
typedef void (*bt_mesh_dh_key_cb_t)(const u8_t key[32], const u8_t idx);

/** @typedef bt_mesh_gatt_attr_func_t
 *  @brief Attribute iterator callback.
 *
 *  @param attr Attribute found.
 *  @param user_data Data given.
 *
 *  @return BLE_MESH_GATT_ITER_CONTINUE if should continue to the next attribute
 *  or BLE_MESH_GATT_ITER_STOP to stop.
 */
typedef u8_t (*bt_mesh_gatt_attr_func_t)(const struct bt_mesh_gatt_attr *attr,
        void *user_data);

/** @brief Connection callback structure.
 *
 *  This structure is used for tracking the state of a connection.
 *  It is registered with the help of the bt_mesh_gatts_conn_cb_register() API.
 *  It's permissible to register multiple instances of this @ref bt_conn_cb
 *  type, in case different modules of an application are interested in
 *  tracking the connection state. If a callback is not of interest for
 *  an instance, it may be set to NULL and will as a consequence not be
 *  used for that instance.
 */
struct bt_mesh_conn_cb {
    /** @brief A new connection has been established.
     *
     *  This callback notifies the application of a new connection.
     *  In case the err parameter is non-zero it means that the
     *  connection establishment failed.
     *
     *  @param conn New connection object.
     *  @param err HCI error. Zero for success, non-zero otherwise.
     */
    void (*connected)(struct bt_mesh_conn *conn, u8_t err);

    /** @brief A connection has been disconnected.
     *
     *  This callback notifies the application that a connection
     *  has been disconnected.
     *
     *  @param conn Connection object.
     *  @param reason HCI reason for the disconnection.
     */
    void (*disconnected)(struct bt_mesh_conn *conn, u8_t reason);
};

struct bt_mesh_prov_conn_cb {
    void (*connected)(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn, int id);

    void (*disconnected)(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn, u8_t reason);

    ssize_t (*prov_write_descr)(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn);

    ssize_t (*prov_notify)(struct bt_mesh_conn *conn, u8_t *data, u16_t len);

    ssize_t (*proxy_write_descr)(bt_mesh_addr_t *addr, struct bt_mesh_conn *conn);

    ssize_t (*proxy_notify)(struct bt_mesh_conn *conn, u8_t *data, u16_t len);
};

/** @brief GATT Attribute structure. */
struct bt_mesh_gatt_attr {
    /** Attribute UUID */
    const struct bt_mesh_uuid *uuid;

    /** Attribute read callback
     *
     *  @param conn   The connection that is requesting to read
     *  @param attr   The attribute that's being read
     *  @param buf    Buffer to place the read result in
     *  @param len    Length of data to read
     *  @param offset Offset to start reading from
     *
     *  @return Number fo bytes read, or in case of an error
     *          BLE_MESH_GATT_ERR() with a specific ATT error code.
     */
    ssize_t (*read)(struct bt_mesh_conn *conn,
                    const struct bt_mesh_gatt_attr *attr,
                    void *buf, u16_t len,
                    u16_t offset);

    /** Attribute write callback
     *
     *  @param conn   The connection that is requesting to write
     *  @param attr   The attribute that's being written
     *  @param buf    Buffer with the data to write
     *  @param len    Number of bytes in the buffer
     *  @param offset Offset to start writing from
     *  @param flags  Flags (BT_GATT_WRITE_*)
     *
     *  @return Number of bytes written, or in case of an error
     *          BLE_MESH_GATT_ERR() with a specific ATT error code.
     */
    ssize_t (*write)(struct bt_mesh_conn *conn,
                     const struct bt_mesh_gatt_attr *attr,
                     const void *buf, u16_t len,
                     u16_t offset, u8_t flags);

    /** Attribute user data */
    void *user_data;
    /** Attribute handle */
    u16_t handle;
    /** Attribute permissions */
    u8_t  perm;
};

/** @def BLE_MESH_GATT_PRIMARY_SERVICE
 *  @brief Primary Service Declaration Macro.
 *
 *  Helper macro to declare a primary service attribute.
 *
 *  @param _service Service attribute value.
 */
#define BLE_MESH_GATT_PRIMARY_SERVICE(_service)     \
{                                                   \
    .uuid = BLE_MESH_UUID_GATT_PRIMARY,             \
    .perm = BLE_MESH_GATT_PERM_READ,                \
    .read = bt_mesh_gatts_attr_read_service,        \
    .user_data = _service,                          \
}

/** @def BLE_MESH_GATT_SECONDARY_SERVICE
 *  @brief Secondary Service Declaration Macro.
 *
 *  Helper macro to declare a secondary service attribute.
 *
 *  @param _service Service attribute value.
 */
#define BLE_MESH_GATT_SECONDARY_SERVICE(_service)   \
{                                                   \
    .uuid = BLE_MESH_UUID_GATT_SECONDARY,           \
    .perm = BLE_MESH_GATT_PERM_READ,                \
    .read = bt_mesh_gatts_attr_read_service,        \
    .user_data = _service,                          \
}

/** @def BLE_MESH_GATT_INCLUDE_SERVICE
 *  @brief Include Service Declaration Macro.
 *
 *  Helper macro to declare database internal include service attribute.
 *
 *  @param _service_incl the first service attribute of service to include
 */
#define BLE_MESH_GATT_INCLUDE_SERVICE(_service_incl)    \
{                                                       \
    .uuid = BLE_MESH_UUID_GATT_INCLUDE,                 \
    .perm = BLE_MESH_GATT_PERM_READ,                    \
    .read = bt_mesh_gatts_attr_read_included,           \
    .user_data = _service_incl,                         \
}

/** @def BLE_MESH_GATT_CHARACTERISTIC
 *  @brief Characteristic Declaration Macro.
 *
 *  Helper macro to declare a characteristic attribute.
 *
 *  @param _uuid Characteristic attribute uuid.
 *  @param _props Characteristic attribute properties.
 */
#define BLE_MESH_GATT_CHARACTERISTIC(_uuid, _props)             \
{                                                               \
    .uuid = BLE_MESH_UUID_GATT_CHRC,                            \
    .perm = BLE_MESH_GATT_PERM_READ,                            \
    .read = bt_mesh_gatts_attr_read_chrc,                       \
    .user_data = (&(struct bt_mesh_gatt_char) { .uuid = _uuid, \
                           .properties = _props, }),            \
}

/** @def BLE_MESH_GATT_DESCRIPTOR
 *  @brief Descriptor Declaration Macro.
 *
 *  Helper macro to declare a descriptor attribute.
 *
 *  @param _uuid Descriptor attribute uuid.
 *  @param _perm Descriptor attribute access permissions.
 *  @param _read Descriptor attribute read callback.
 *  @param _write Descriptor attribute write callback.
 *  @param _value Descriptor attribute value.
 */
#define BLE_MESH_GATT_DESCRIPTOR(_uuid, _perm, _read, _write, _value)   \
{                           \
    .uuid = _uuid,          \
    .perm = _perm,          \
    .read = _read,          \
    .write = _write,        \
    .user_data = _value,    \
}

/** @def BLE_MESH_GATT_SERVICE
 *  @brief Service Structure Declaration Macro.
 *
 *  Helper macro to declare a service structure.
 *
 *  @param _attrs Service attributes.
 */
#define BLE_MESH_GATT_SERVICE(_attrs)   \
{                                       \
    .attrs = _attrs,                    \
    .attr_count = ARRAY_SIZE(_attrs),   \
}

esp_err_t bt_mesh_host_init(void);

int bt_le_adv_start(const struct bt_mesh_adv_param *param,
                    const struct bt_mesh_adv_data *ad, size_t ad_len,
                    const struct bt_mesh_adv_data *sd, size_t sd_len);

int bt_le_adv_stop(void);

int bt_le_scan_start(const struct bt_mesh_scan_param *param, bt_mesh_scan_cb_t cb);

int bt_le_scan_stop(void);

void bt_mesh_gatts_conn_cb_register(struct bt_mesh_conn_cb *cb);

int bt_mesh_gatts_disconnect(struct bt_mesh_conn *conn, u8_t reason);

int bt_mesh_gatts_service_register(struct bt_mesh_gatt_service *svc);

int bt_mesh_gatts_service_unregister(struct bt_mesh_gatt_service *svc);

ssize_t bt_mesh_gatts_attr_read_included(struct bt_mesh_conn *conn,
        const struct bt_mesh_gatt_attr *attr,
        void *buf, u16_t len, u16_t offset);

ssize_t bt_mesh_gatts_attr_read(struct bt_mesh_conn *conn, const struct bt_mesh_gatt_attr *attr,
                                void *buf, u16_t buf_len, u16_t offset,
                                const void *value, u16_t value_len);

ssize_t bt_mesh_gatts_attr_read_service(struct bt_mesh_conn *conn,
                                        const struct bt_mesh_gatt_attr *attr,
                                        void *buf, u16_t len, u16_t offset);

ssize_t bt_mesh_gatts_attr_read_chrc(struct bt_mesh_conn *conn,
                                     const struct bt_mesh_gatt_attr *attr, void *buf,
                                     u16_t len, u16_t offset);

int bt_mesh_gatts_notify(struct bt_mesh_conn *conn, const struct bt_mesh_gatt_attr *attr,
                         const void *data, u16_t len);

u16_t bt_mesh_gatt_get_mtu(struct bt_mesh_conn *conn);

/** APIs added by Espressif */
int bt_mesh_gatts_service_stop(struct bt_mesh_gatt_service *svc);
int bt_mesh_gatts_service_start(struct bt_mesh_gatt_service *svc);

void bt_mesh_gattc_conn_cb_register(struct bt_mesh_prov_conn_cb *cb);

u8_t bt_mesh_gattc_get_free_conn_count(void);

u16_t bt_mesh_gattc_get_service_uuid(struct bt_mesh_conn *conn);

int bt_mesh_gattc_conn_create(const bt_mesh_addr_t *addr, u16_t service_uuid);

void bt_gattc_conn_close(struct bt_mesh_conn *conn);

void bt_mesh_gattc_exchange_mtu(u8_t index);

u16_t bt_mesh_gattc_get_mtu_info(struct bt_mesh_conn *conn);

int bt_mesh_gattc_write_no_rsp(struct bt_mesh_conn *conn, const struct bt_mesh_gatt_attr *attr,
                               const void *data, u16_t len);

void bt_mesh_gattc_disconnect(struct bt_mesh_conn *conn);

struct bt_mesh_conn *bt_mesh_conn_ref(struct bt_mesh_conn *conn);

void bt_mesh_conn_unref(struct bt_mesh_conn *conn);

void bt_mesh_gatt_init(void);

void bt_mesh_adapt_init(void);

int bt_mesh_rand(void *buf, size_t len);

void bt_mesh_set_private_key(const u8_t pri_key[32]);

const u8_t *bt_mesh_pub_key_get(void);

bool bt_mesh_check_public_key(const uint8_t key[64]);

int bt_mesh_dh_key_gen(const u8_t remote_pk[64], bt_mesh_dh_key_cb_t cb, const u8_t idx);

int bt_mesh_encrypt_le(const u8_t key[16], const u8_t plaintext[16],
                       u8_t enc_data[16]);

int bt_mesh_encrypt_be(const u8_t key[16], const u8_t plaintext[16],
                       u8_t enc_data[16]);

enum {
    BLE_MESH_EXCEP_LIST_ADD = 0,
    BLE_MESH_EXCEP_LIST_REMOVE,
    BLE_MESH_EXCEP_LIST_CLEAN,
};

enum {
    BLE_MESH_EXCEP_INFO_ADV_ADDR = 0,
    BLE_MESH_EXCEP_INFO_MESH_LINK_ID,
    BLE_MESH_EXCEP_INFO_MESH_BEACON,
    BLE_MESH_EXCEP_INFO_MESH_PROV_ADV,
    BLE_MESH_EXCEP_INFO_MESH_PROXY_ADV,
};

enum {
    BLE_MESH_EXCEP_CLEAN_ADDR_LIST           = BIT(0),
    BLE_MESH_EXCEP_CLEAN_MESH_LINK_ID_LIST   = BIT(1),
    BLE_MESH_EXCEP_CLEAN_MESH_BEACON_LIST    = BIT(2),
    BLE_MESH_EXCEP_CLEAN_MESH_PROV_ADV_LIST  = BIT(3),
    BLE_MESH_EXCEP_CLEAN_MESH_PROXY_ADV_LIST = BIT(4),
    BLE_MESH_EXCEP_CLEAN_ALL_LIST            = 0xFFFF,
};

int bt_mesh_update_exceptional_list(u8_t sub_code, u8_t type, void *info);

#endif /* _BLE_MESH_BEARER_ADRPT_H_ */

