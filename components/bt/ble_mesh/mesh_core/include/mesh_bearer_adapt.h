/*
 * Copyright (c) 2017 Nordic Semiconductor ASA
 * Copyright (c) 2015-2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MESH_BEARER_ADRPT_H_
#define _MESH_BEARER_ADRPT_H_

#include <string.h>
#include "mesh_types.h"
#include "mesh_buf.h"
#include "mesh_bt_uuid.h"

/* Advertising types */
#define BT_LE_ADV_IND                           0x00
#define BT_LE_ADV_DIRECT_IND                    0x01
#define BT_LE_ADV_SCAN_IND                      0x02
#define BT_LE_ADV_NONCONN_IND                   0x03
#define BT_LE_ADV_DIRECT_IND_LOW_DUTY           0x04
/* Needed in advertising reports when getting info about */
#define BT_LE_ADV_SCAN_RSP                      0x04

#define BT_HCI_LE_SCAN_PASSIVE                  0x00
#define BT_HCI_LE_SCAN_ACTIVE                   0x01

#define BT_HCI_LE_SCAN_DISABLE                  0x00
#define BT_HCI_LE_SCAN_ENABLE                   0x01

#define BT_HCI_LE_SCAN_FILTER_DUP_DISABLE       0x00
#define BT_HCI_LE_SCAN_FILTER_DUP_ENABLE        0x01

/* Error codes for Error response PDU */
#define BT_ATT_ERR_INVALID_HANDLE       0x01
#define BT_ATT_ERR_READ_NOT_PERMITTED       0x02
#define BT_ATT_ERR_WRITE_NOT_PERMITTED      0x03
#define BT_ATT_ERR_INVALID_PDU          0x04
#define BT_ATT_ERR_AUTHENTICATION       0x05
#define BT_ATT_ERR_NOT_SUPPORTED        0x06
#define BT_ATT_ERR_INVALID_OFFSET       0x07
#define BT_ATT_ERR_AUTHORIZATION        0x08
#define BT_ATT_ERR_PREPARE_QUEUE_FULL       0x09
#define BT_ATT_ERR_ATTRIBUTE_NOT_FOUND      0x0a
#define BT_ATT_ERR_ATTRIBUTE_NOT_LONG       0x0b
#define BT_ATT_ERR_ENCRYPTION_KEY_SIZE      0x0c
#define BT_ATT_ERR_INVALID_ATTRIBUTE_LEN    0x0d
#define BT_ATT_ERR_UNLIKELY         0x0e
#define BT_ATT_ERR_INSUFFICIENT_ENCRYPTION  0x0f
#define BT_ATT_ERR_UNSUPPORTED_GROUP_TYPE   0x10
#define BT_ATT_ERR_INSUFFICIENT_RESOURCES   0x11

/* Common Profile Error Codes (from CSS) */
#define BT_ATT_ERR_WRITE_REQ_REJECTED       0xfc
#define BT_ATT_ERR_CCC_IMPROPER_CONF        0xfd
#define BT_ATT_ERR_PROCEDURE_IN_PROGRESS    0xfe
#define BT_ATT_ERR_OUT_OF_RANGE         0xff

/* EIR/AD data type definitions */
#define BT_DATA_FLAGS                   0x01 /* AD flags */
#define BT_DATA_UUID16_SOME             0x02 /* 16-bit UUID, more available */
#define BT_DATA_UUID16_ALL              0x03 /* 16-bit UUID, all listed */
#define BT_DATA_UUID32_SOME             0x04 /* 32-bit UUID, more available */
#define BT_DATA_UUID32_ALL              0x05 /* 32-bit UUID, all listed */
#define BT_DATA_UUID128_SOME            0x06 /* 128-bit UUID, more available */
#define BT_DATA_UUID128_ALL             0x07 /* 128-bit UUID, all listed */
#define BT_DATA_NAME_SHORTENED          0x08 /* Shortened name */
#define BT_DATA_NAME_COMPLETE           0x09 /* Complete name */
#define BT_DATA_TX_POWER                0x0a /* Tx Power */
#define BT_DATA_SOLICIT16               0x14 /* Solicit UUIDs, 16-bit */
#define BT_DATA_SOLICIT128              0x15 /* Solicit UUIDs, 128-bit */
#define BT_DATA_SVC_DATA16              0x16 /* Service data, 16-bit UUID */
#define BT_DATA_GAP_APPEARANCE          0x19 /* GAP appearance */
#define BT_DATA_SOLICIT32               0x1f /* Solicit UUIDs, 32-bit */
#define BT_DATA_SVC_DATA32              0x20 /* Service data, 32-bit UUID */
#define BT_DATA_SVC_DATA128             0x21 /* Service data, 128-bit UUID */
#define BT_DATA_MESH_PROV               0x29 /* Mesh Provisioning PDU */
#define BT_DATA_MESH_MESSAGE            0x2a /* Mesh Networking PDU */
#define BT_DATA_MESH_BEACON             0x2b /* Mesh Beacon */

#define BT_DATA_MANUFACTURER_DATA       0xff /* Manufacturer Specific Data */

#define BT_LE_AD_LIMITED                0x01 /* Limited Discoverable */
#define BT_LE_AD_GENERAL                0x02 /* General Discoverable */
#define BT_LE_AD_NO_BREDR               0x04 /* BR/EDR not supported */

/* Client Characteristic Configuration Values */

/** @def BT_GATT_CCC_NOTIFY
 *  @brief Client Characteristic Configuration Notification.
 *
 *  If set, changes to Characteristic Value shall be notified.
 */
#define BT_GATT_CCC_NOTIFY          0x0001
/** @def BT_GATT_CCC_INDICATE
 *  @brief Client Characteristic Configuration Indication.
 *
 *  If set, changes to Characteristic Value shall be indicated.
 */
#define BT_GATT_CCC_INDICATE            0x0002


/**  @def BT_GATT_ERR
  *  @brief Construct error return value for attribute read and write callbacks.
  *
  *  @param _att_err ATT error code
  *
  *  @return Appropriate error code for the attribute callbacks.
  *
  */
#define BT_GATT_ERR(_att_err)                  (-(_att_err))

enum {
    BT_GATT_ITER_STOP = 0,
    BT_GATT_ITER_CONTINUE,
};

/* GATT attribute permission bit field values */
enum {
    /** No operations supported, e.g. for notify-only */
    BT_GATT_PERM_NONE = 0,

    /** Attribute read permission. */
    BT_GATT_PERM_READ = BIT(0),

    /** Attribute write permission. */
    BT_GATT_PERM_WRITE = BIT(1),

    /** Attribute read permission with encryption.
     *
     *  If set, requires encryption for read access.
     */
    BT_GATT_PERM_READ_ENCRYPT = BIT(2),

    /** Attribute write permission with encryption.
     *
     *  If set, requires encryption for write access.
     */
    BT_GATT_PERM_WRITE_ENCRYPT = BIT(3),

    /** Attribute read permission with authentication.
     *
     *  If set, requires encryption using authenticated link-key for read
     *  access.
     */
    BT_GATT_PERM_READ_AUTHEN = BIT(4),

    /** Attribute write permission with authentication.
     *
     *  If set, requires encryption using authenticated link-key for write
     *  access.
     */
    BT_GATT_PERM_WRITE_AUTHEN = BIT(5),

    /** Attribute prepare write permission.
     *
     *  If set, allows prepare writes with use of BT_GATT_WRITE_FLAG_PREPARE
     *  passed to write callback.
     */
    BT_GATT_PERM_PREPARE_WRITE = BIT(6),
};

/** Advertising options */
enum {
    /** Convenience value when no options are specified. */
    BT_LE_ADV_OPT_NONE = 0,

    /** Advertise as connectable. Type of advertising is determined by
     * providing SCAN_RSP data and/or enabling local privacy support.
     */
    BT_LE_ADV_OPT_CONNECTABLE = BIT(0),

    /** Don't try to resume connectable advertising after a connection.
     *  This option is only meaningful when used together with
     *  BT_LE_ADV_OPT_CONNECTABLE. If set the advertising will be stopped
     *  when bt_le_adv_stop() is called or when an incoming (slave)
     *  connection happens. If this option is not set the stack will
     *  take care of keeping advertising enabled even as connections
     *  occur.
     */
    BT_LE_ADV_OPT_ONE_TIME = BIT(1),
};

/* Defined GAP timers */
#define BT_GAP_SCAN_FAST_INTERVAL               0x0060  /* 60 ms    */
#define BT_GAP_SCAN_FAST_WINDOW                 0x0030  /* 30 ms    */
#define BT_GAP_SCAN_SLOW_INTERVAL_1             0x0800  /* 1.28 s   */
#define BT_GAP_SCAN_SLOW_WINDOW_1               0x0012  /* 11.25 ms */
#define BT_GAP_SCAN_SLOW_INTERVAL_2             0x1000  /* 2.56 s   */
#define BT_GAP_SCAN_SLOW_WINDOW_2               0x0012  /* 11.25 ms */
#define BT_GAP_ADV_FAST_INT_MIN_0               0x0020  /* 20 ms    */
#define BT_GAP_ADV_FAST_INT_MAX_0               0x0020  /* 20 ms    */
#define BT_GAP_ADV_FAST_INT_MIN_1               0x0030  /* 30 ms    */
#define BT_GAP_ADV_FAST_INT_MAX_1               0x0060  /* 60 ms    */
#define BT_GAP_ADV_FAST_INT_MIN_2               0x00a0  /* 100 ms   */
#define BT_GAP_ADV_FAST_INT_MAX_2               0x00f0  /* 150 ms   */
#define BT_GAP_ADV_SLOW_INT_MIN                 0x0320  //0x0640  /* 1 s      */
#define BT_GAP_ADV_SLOW_INT_MAX                 0x0320  //0x0780  /* 1.2 s    */
#define BT_GAP_INIT_CONN_INT_MIN                0x0018  /* 30 ms    */
#define BT_GAP_INIT_CONN_INT_MAX                0x0028  /* 50 ms    */



/* Characteristic Properties Bit field values */

/** @def BT_GATT_CHRC_BROADCAST
 *  @brief Characteristic broadcast property.
 *
 *  If set, permits broadcasts of the Characteristic Value using Server
 *  Characteristic Configuration Descriptor.
 */
#define BT_GATT_CHRC_BROADCAST          0x01
/** @def BT_GATT_CHRC_READ
 *  @brief Characteristic read property.
 *
 *  If set, permits reads of the Characteristic Value.
 */
#define BT_GATT_CHRC_READ           0x02
/** @def BT_GATT_CHRC_WRITE_WITHOUT_RESP
 *  @brief Characteristic write without response property.
 *
 *  If set, permits write of the Characteristic Value without response.
 */
#define BT_GATT_CHRC_WRITE_WITHOUT_RESP     0x04
/** @def BT_GATT_CHRC_WRITE
 *  @brief Characteristic write with response property.
 *
 *  If set, permits write of the Characteristic Value with response.
 */
#define BT_GATT_CHRC_WRITE          0x08
/** @def BT_GATT_CHRC_NOTIFY
 *  @brief Characteristic notify property.
 *
 *  If set, permits notifications of a Characteristic Value without
 *  acknowledgment.
 */
#define BT_GATT_CHRC_NOTIFY         0x10
/** @def BT_GATT_CHRC_INDICATE
 *  @brief Characteristic indicate property.
 *
 * If set, permits indications of a Characteristic Value with acknowledgment.
 */
#define BT_GATT_CHRC_INDICATE           0x20
/** @def BT_GATT_CHRC_AUTH
 *  @brief Characteristic Authenticated Signed Writes property.
 *
 *  If set, permits signed writes to the Characteristic Value.
 */
#define BT_GATT_CHRC_AUTH           0x40
/** @def BT_GATT_CHRC_EXT_PROP
 *  @brief Characteristic Extended Properties property.
 *
 * If set, additional characteristic properties are defined in the
 * Characteristic Extended Properties Descriptor.
 */
#define BT_GATT_CHRC_EXT_PROP           0x80


/** @brief Characteristic Attribute Value. */
struct bt_gatt_chrc {
    /** Characteristic UUID. */
    const struct bt_uuid    *uuid;
    /** Characteristic properties. */
    u8_t            properties;
};

/** @brief GATT Service structure */
struct bt_gatt_service {
    /** Service Attributes */
    struct bt_gatt_attr *attrs;
    /** Service Attribute count */
    u16_t           attr_count;
    sys_snode_t     node;
};

/*
 * Copyright (c) 2015 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
typedef enum __packed {
    BT_CONN_DISCONNECTED,
    BT_CONN_CONNECT_SCAN,
    BT_CONN_CONNECT,
    BT_CONN_CONNECTED,
    BT_CONN_DISCONNECT,
} bt_conn_state_t;

/** Security level. */
typedef enum __packed {
    /** Only for BR/EDR special cases, like SDP */
    BT_SECURITY_NONE,
    /** No encryption and no authentication. */
    BT_SECURITY_LOW,
    /** Encryption and no authentication (no MITM). */
    BT_SECURITY_MEDIUM,
    /** Encryption and authentication (MITM). */
    BT_SECURITY_HIGH,
    /** Authenticated Secure Connections */
    BT_SECURITY_FIPS,
} bt_security_t;

struct ecb_param {
    u8_t key[16];
    u8_t clear_text[16];
    u8_t cipher_text[16];
} __packed;

typedef struct {
    u8_t  val[6];
} bt_addr_t;

typedef struct {
    u8_t      type;
    bt_addr_t a;
} bt_addr_le_t;

/** Description of different data types that can be encoded into
  * advertising data. Used to form arrays that are passed to the
  * bt_le_adv_start() function.
  */
struct bt_data {
    u8_t type;
    u8_t data_len;
    const u8_t *data;
};

/** @brief Helper to declare elements of bt_data arrays
 *
 *  This macro is mainly for creating an array of struct bt_data
 *  elements which is then passed to bt_le_adv_start().
 *
 *  @param _type Type of advertising data field
 *  @param _data Pointer to the data field payload
 *  @param _data_len Number of bytes behind the _data pointer
 */
#define BT_DATA(_type, _data, _data_len) \
    { \
        .type = (_type), \
        .data_len = (_data_len), \
        .data = (const u8_t *)(_data), \
    }

/** @brief Helper to declare elements of bt_data arrays
 *
 *  This macro is mainly for creating an array of struct bt_data
 *  elements which is then passed to bt_le_adv_start().
 *
 *  @param _type Type of advertising data field
 *  @param _bytes Variable number of single-byte parameters
 */
#define BT_DATA_BYTES(_type, _bytes...) \
    BT_DATA(_type, ((u8_t []) { _bytes }), \
        sizeof((u8_t []) { _bytes }))

/** LE Advertising Parameters. */
struct bt_le_adv_param {
    /** Bit-field of advertising options */
    u8_t  options;

    /** Minimum Advertising Interval (N * 0.625) */
    u16_t interval_min;

    /** Maximum Advertising Interval (N * 0.625) */
    u16_t interval_max;

    /** Optional predefined (random) own address. Currently
     *  the only permitted use of this is for NRPA with
     *  non-connectable advertising.
     */
    const bt_addr_t *own_addr;
};

/** LE scan parameters */
struct bt_le_scan_param {
    /** Scan type (BT_HCI_LE_SCAN_ACTIVE or BT_HCI_LE_SCAN_PASSIVE) */
    u8_t  type;

    /** Duplicate filtering (BT_HCI_LE_SCAN_FILTER_DUP_ENABLE or
     *  BT_HCI_LE_SCAN_FILTER_DUP_DISABLE)
     */
    u8_t  filter_dup;

    /** Scan interval (N * 0.625 ms) */
    u16_t interval;

    /** Scan window (N * 0.625 ms) */
    u16_t window;
};

/** Connection parameters for LE connections */
struct bt_le_conn_param {
    u16_t interval_min;
    u16_t interval_max;
    u16_t latency;
    u16_t timeout;
};

struct bt_conn {
    u16_t           handle;
    u8_t            type;
    u8_t            role;

    /* Connection error or reason for disconnect */
    u8_t            err;

    bt_conn_state_t     state;

    u16_t               rx_len;
    struct net_buf      *rx;

    /* Sent but not acknowledged TX packets */
    sys_slist_t     tx_pending;
    /* Acknowledged but not yet notified TX packets */
    struct k_fifo       tx_notify;

    /* Queue for outgoing ACL data */
    struct k_fifo       tx_queue;

    /* Active L2CAP channels */
    sys_slist_t     channels;

    atomic_t        ref;
};


/** @typedef bt_le_scan_cb_t
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
typedef void bt_le_scan_cb_t(const bt_addr_le_t *addr, s8_t rssi,
                             u8_t adv_type, struct net_buf_simple *buf);

/*  @typedef bt_dh_key_cb_t
 *  @brief Callback type for DH Key calculation.
 *
 *  Used to notify of the calculated DH Key.
 *
 *  @param key The DH Key, or NULL in case of failure.
 */
typedef void (*bt_dh_key_cb_t)(const u8_t key[32]);

/** @typedef bt_gatt_attr_func_t
 *  @brief Attribute iterator callback.
 *
 *  @param attr Attribute found.
 *  @param user_data Data given.
 *
 *  @return BT_GATT_ITER_CONTINUE if should continue to the next attribute
 *  or BT_GATT_ITER_STOP to stop.
 */
typedef u8_t (*bt_gatt_attr_func_t)(const struct bt_gatt_attr *attr,
                                    void *user_data);

/** @brief Connection callback structure.
 *
 *  This structure is used for tracking the state of a connection.
 *  It is registered with the help of the bt_conn_cb_register() API.
 *  It's permissible to register multiple instances of this @ref bt_conn_cb
 *  type, in case different modules of an application are interested in
 *  tracking the connection state. If a callback is not of interest for
 *  an instance, it may be set to NULL and will as a consequence not be
 *  used for that instance.
 */
struct bt_conn_cb {
    /** @brief A new connection has been established.
     *
     *  This callback notifies the application of a new connection.
     *  In case the err parameter is non-zero it means that the
     *  connection establishment failed.
     *
     *  @param conn New connection object.
     *  @param err HCI error. Zero for success, non-zero otherwise.
     */
    void (*connected)(struct bt_conn *conn, u8_t err);

    /** @brief A connection has been disconnected.
     *
     *  This callback notifies the application that a connection
     *  has been disconnected.
     *
     *  @param conn Connection object.
     *  @param reason HCI reason for the disconnection.
     */
    void (*disconnected)(struct bt_conn *conn, u8_t reason);

    /** @brief LE connection parameter update request.
     *
     *  This callback notifies the application that a remote device
     *  is requesting to update the connection parameters. The
     *  application accepts the parameters by returning true, or
     *  rejects them by returning false. Before accepting, the
     *  application may also adjust the parameters to better suit
     *  its needs.
     *
     *  It is recommended for an application to have just one of these
     *  callbacks for simplicity. However, if an application registers
     *  multiple it needs to manage the potentially different
     *  requirements for each callback. Each callback gets the
     *  parameters as returned by previous callbacks, i.e. they are not
     *  necessarily the same ones as the remote originally sent.
     *
     *  @param conn Connection object.
     *  @param param Proposed connection parameters.
     *
     *  @return true to accept the parameters, or false to reject them.
     */
    bool (*le_param_req)(struct bt_conn *conn,
                         struct bt_le_conn_param *param);

    /** @brief The parameters for an LE connection have been updated.
     *
     *  This callback notifies the application that the connection
     *  parameters for an LE connection have been updated.
     *
     *  @param conn Connection object.
     *  @param interval Connection interval.
     *  @param latency Connection latency.
     *  @param timeout Connection supervision timeout.
     */
    void (*le_param_updated)(struct bt_conn *conn, u16_t interval,
                             u16_t latency, u16_t timeout);
#if defined(CONFIG_BT_SMP)
    /** @brief Remote Identity Address has been resolved.
     *
     *  This callback notifies the application that a remote
     *  Identity Address has been resolved
     *
     *  @param conn Connection object.
     *  @param rpa Resolvable Private Address.
     *  @param identity Identity Address.
     */
    void (*identity_resolved)(struct bt_conn *conn,
                              const bt_addr_le_t *rpa,
                              const bt_addr_le_t *identity);
#endif /* CONFIG_BT_SMP */
#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
    /** @brief The security level of a connection has changed.
     *
     *  This callback notifies the application that the security level
     *  of a connection has changed.
     *
     *  @param conn Connection object.
     *  @param level New security level of the connection.
     */
    void (*security_changed)(struct bt_conn *conn, bt_security_t level);
#endif /* defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR) */
    struct bt_conn_cb *_next;
};

#if CONFIG_BT_MESH_PROVISIONER
struct bt_prov_conn_cb {
    void (*connected)(struct bt_conn *conn, int id);

    void (*disconnected)(struct bt_conn *conn, u8_t reason);

    ssize_t (*prov_write_descr)(struct bt_conn *conn, u8_t *addr);

    ssize_t (*prov_notify)(struct bt_conn *conn, u8_t *data, u16_t len);

    ssize_t (*proxy_write_descr)(struct bt_conn *conn);

    ssize_t (*proxy_notify)(struct bt_conn *conn, u8_t *data, u16_t len);

    bool (*le_param_req)(struct bt_conn *conn, struct bt_le_conn_param *param);

    void (*le_param_updated)(struct bt_conn *conn, u16_t interval,
                             u16_t latency, u16_t timeout);

#if defined(CONFIG_BT_SMP)
    void (*identity_resolved)(struct bt_conn *conn,
                              const bt_addr_le_t *rpa,
                              const bt_addr_le_t *identity);
#endif /* CONFIG_BT_SMP */

#if defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR)
    void (*security_changed)(struct bt_conn *conn, bt_security_t level);
#endif /* defined(CONFIG_BT_SMP) || defined(CONFIG_BT_BREDR) */

    struct bt_conn_cb *_next;
};
#endif /* CONFIG_BT_MESH_PROVISIONER */

/** @brief GATT Attribute structure. */
struct bt_gatt_attr {
    /** Attribute UUID */
    const struct bt_uuid    *uuid;

    /** Attribute read callback
     *
     *  @param conn   The connection that is requesting to read
     *  @param attr   The attribute that's being read
     *  @param buf    Buffer to place the read result in
     *  @param len    Length of data to read
     *  @param offset Offset to start reading from
     *
     *  @return Number fo bytes read, or in case of an error
     *          BT_GATT_ERR() with a specific ATT error code.
     */
    ssize_t         (*read)(struct bt_conn *conn,
                            const struct bt_gatt_attr *attr,
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
     *          BT_GATT_ERR() with a specific ATT error code.
     */
    ssize_t         (*write)(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, u16_t len,
                             u16_t offset, u8_t flags);

    /** Attribute user data */
    void            *user_data;
    /** Attribute handle */
    u16_t           handle;
    /** Attribute permissions */
    u8_t            perm;
};

/** @def BT_GATT_PRIMARY_SERVICE
 *  @brief Primary Service Declaration Macro.
 *
 *  Helper macro to declare a primary service attribute.
 *
 *  @param _service Service attribute value.
 */
#define BT_GATT_PRIMARY_SERVICE(_service)               \
{                                   \
    .uuid = BT_UUID_GATT_PRIMARY,                   \
    .perm = BT_GATT_PERM_READ,                  \
    .read = bt_gatt_attr_read_service,              \
    .user_data = _service,                      \
}

/** @def BT_GATT_SECONDARY_SERVICE
 *  @brief Secondary Service Declaration Macro.
 *
 *  Helper macro to declare a secondary service attribute.
 *
 *  @param _service Service attribute value.
 */
#define BT_GATT_SECONDARY_SERVICE(_service)             \
{                                   \
    .uuid = BT_UUID_GATT_SECONDARY,                 \
    .perm = BT_GATT_PERM_READ,                  \
    .read = bt_gatt_attr_read_service,              \
    .user_data = _service,                      \
}

/** @def BT_GATT_INCLUDE_SERVICE
 *  @brief Include Service Declaration Macro.
 *
 *  Helper macro to declare database internal include service attribute.
 *
 *  @param _service_incl the first service attribute of service to include
 */
#define BT_GATT_INCLUDE_SERVICE(_service_incl)              \
{                                   \
    .uuid = BT_UUID_GATT_INCLUDE,                   \
    .perm = BT_GATT_PERM_READ,                  \
    .read = bt_gatt_attr_read_included,             \
    .user_data = _service_incl,                 \
}


/** @def BT_GATT_CHARACTERISTIC
 *  @brief Characteristic Declaration Macro.
 *
 *  Helper macro to declare a characteristic attribute.
 *
 *  @param _uuid Characteristic attribute uuid.
 *  @param _props Characteristic attribute properties.
 */
#define BT_GATT_CHARACTERISTIC(_uuid, _props)               \
{                                   \
    .uuid = BT_UUID_GATT_CHRC,                  \
    .perm = BT_GATT_PERM_READ,                  \
    .read = bt_gatt_attr_read_chrc,                 \
    .user_data = (&(struct bt_gatt_chrc) { .uuid = _uuid,       \
                           .properties = _props, }),\
}

/** @def BT_GATT_DESCRIPTOR
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
#define BT_GATT_DESCRIPTOR(_uuid, _perm, _read, _write, _value)     \
{                                   \
    .uuid = _uuid,                          \
    .perm = _perm,                          \
    .read = _read,                          \
    .write = _write,                        \
    .user_data = _value,                        \
}

/** @def BT_GATT_SERVICE
 *  @brief Service Structure Declaration Macro.
 *
 *  Helper macro to declare a service structure.
 *
 *  @param _attrs Service attributes.
 */
#define BT_GATT_SERVICE(_attrs)                     \
{                                   \
    .attrs = _attrs,                        \
    .attr_count = ARRAY_SIZE(_attrs),               \
}

/*  @brief Container for public key callback */
struct bt_pub_key_cb {
    /** @brief Callback type for Public Key generation.
     *
     *  Used to notify of the local public key or that the local key is not
     *  available (either because of a failure to read it or because it is
     *  being regenerated).
     *
     *  @param key The local public key, or NULL in case of no key.
     */
    void (*func)(const u8_t key[64]);

    struct bt_pub_key_cb *_next;
};

int bt_le_adv_start(const struct bt_le_adv_param *param,
                    const struct bt_data *ad, size_t ad_len,
                    const struct bt_data *sd, size_t sd_len);

int bt_le_adv_stop(void);

int bt_le_scan_start(const struct bt_le_scan_param *param, bt_le_scan_cb_t cb);

int bt_le_scan_stop(void);

void bt_conn_cb_register(struct bt_conn_cb *cb);

#if CONFIG_BT_MESH_PROVISIONER
void bt_prov_conn_cb_register(struct bt_prov_conn_cb *cb);

bool bt_prov_check_gattc_id(int id, const bt_addr_le_t *addr);

u16_t bt_mesh_get_srvc_uuid(struct bt_conn *conn);

void bt_gattc_conn_create(int id, u16_t srvc_uuid);

void bt_gattc_conn_close(struct bt_conn *conn);

void bt_gattc_exchange_mtu(int id);

u16_t bt_gatt_prov_get_mtu(struct bt_conn *conn);

int bt_gattc_write_no_rsp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          const void *data, u16_t len);

void bt_gattc_disconnect(struct bt_conn *conn);
#endif /* CONFIG_BT_MESH_PROVISIONER */

int bt_conn_disconnect(struct bt_conn *conn, u8_t reason);

struct bt_conn *bt_conn_ref(struct bt_conn *conn);

void bt_conn_unref(struct bt_conn *conn);

int bt_gatt_service_register(struct bt_gatt_service *svc);

int bt_gatt_service_unregister(struct bt_gatt_service *svc);

ssize_t bt_gatt_attr_read_included(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, u16_t len, u16_t offset);

ssize_t bt_gatt_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, u16_t buf_len, u16_t offset,
                          const void *value, u16_t value_len);


ssize_t bt_gatt_attr_read_service(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr,
                                  void *buf, u16_t len, u16_t offset);

ssize_t bt_gatt_attr_read_chrc(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr, void *buf,
                               u16_t len, u16_t offset);

int bt_gatt_notify(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                   const void *data, u16_t len);

u16_t bt_gatt_get_mtu(struct bt_conn *conn);

int bt_rand(void *buf, size_t len);

int bt_encrypt_le(const u8_t key[16], const u8_t plaintext[16],
                  u8_t enc_data[16]);

int bt_encrypt_be(const u8_t key[16], const u8_t plaintext[16],
                  u8_t enc_data[16]);

void bt_mesh_gatt_init(void);

const u8_t *bt_pub_key_get(void);

void bt_mesh_adapt_init(void);

int bt_dh_key_gen(const u8_t remote_pk[64], bt_dh_key_cb_t cb);

/** APIs added by Espressif */
int bt_gatt_service_stop(struct bt_gatt_service *svc);
int bt_gatt_service_start(struct bt_gatt_service *svc);

#endif /* #ifndef __MESH_BEARER_ADRPT_H__ */

