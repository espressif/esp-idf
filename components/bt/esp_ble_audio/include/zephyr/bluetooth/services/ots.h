/*
 * SPDX-FileCopyrightText: 2020-2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_OTS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_OTS_H_

/**
 * @brief Object Transfer Service (OTS)
 * @defgroup bt_ots Object Transfer Service (OTS)
 * @ingroup bluetooth
 * @{
 *
 * [Experimental] Users should note that the APIs can change
 * as a part of ongoing development.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/types.h>

#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/crc.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Size of OTS object ID (in bytes). */
#define BT_OTS_OBJ_ID_SIZE 6

/** @brief Minimum allowed value for object ID (except ID for directory listing) */
#define BT_OTS_OBJ_ID_MIN 0x000000000100ULL

/** @brief Maximum allowed value for object ID (except ID for directory listing) */
#define BT_OTS_OBJ_ID_MAX 0xFFFFFFFFFFFFULL

/** @brief ID of the Directory Listing Object */
#define OTS_OBJ_ID_DIR_LIST 0x000000000000ULL

/** @brief Mask for OTS object IDs, preserving the 48 bits */
#define BT_OTS_OBJ_ID_MASK BIT64_MASK(48)

/** @brief Length of OTS object ID string (in bytes). */
#define BT_OTS_OBJ_ID_STR_LEN 15

/** @brief Type of an OTS object. */
struct bt_ots_obj_type {
    union {
        /* Used to indicate UUID type */
        struct bt_uuid uuid;

        /* 16-bit UUID value */
        struct bt_uuid_16 uuid_16;

        /* 128-bit UUID value */
        struct bt_uuid_128 uuid_128;
    };
};

/** @brief Properties of an OTS object. */
enum {
    /** Bit 0 Deletion of this object is permitted */
    BT_OTS_OBJ_PROP_DELETE    = 0,

    /** Bit 1 Execution of this object is permitted */
    BT_OTS_OBJ_PROP_EXECUTE   = 1,

    /** Bit 2 Reading this object is permitted */
    BT_OTS_OBJ_PROP_READ      = 2,

    /** Bit 3 Writing data to this object is permitted */
    BT_OTS_OBJ_PROP_WRITE     = 3,

    /** @brief Bit 4 Appending data to this object is permitted.
     *
     * Appending data increases its Allocated Size.
     */
    BT_OTS_OBJ_PROP_APPEND    = 4,

    /** Bit 5 Truncation of this object is permitted */
    BT_OTS_OBJ_PROP_TRUNCATE  = 5,

    /** @brief Bit 6 Patching this object is permitted
     *
     *  Patching this object overwrites some of
     *  the object's existing contents.
     */
    BT_OTS_OBJ_PROP_PATCH     = 6,

    /** Bit 7 This object is a marked object */
    BT_OTS_OBJ_PROP_MARKED    = 7,
};

/** @brief Set @ref BT_OTS_OBJ_PROP_DELETE property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_SET_PROP_DELETE(prop) \
    WRITE_BIT(prop, BT_OTS_OBJ_PROP_DELETE, 1)

/** @brief Set @ref BT_OTS_OBJ_PROP_EXECUTE property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_SET_PROP_EXECUTE(prop) \
    WRITE_BIT(prop, BT_OTS_OBJ_PROP_EXECUTE, 1)

/** @brief Set @ref BT_OTS_OBJ_PROP_READ property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_SET_PROP_READ(prop) \
    WRITE_BIT(prop, BT_OTS_OBJ_PROP_READ, 1)

/** @brief Set @ref BT_OTS_OBJ_PROP_WRITE property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_SET_PROP_WRITE(prop) \
    WRITE_BIT(prop, BT_OTS_OBJ_PROP_WRITE, 1)

/** @brief Set @ref BT_OTS_OBJ_PROP_APPEND property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_SET_PROP_APPEND(prop) \
    WRITE_BIT(prop, BT_OTS_OBJ_PROP_APPEND, 1)

/** @brief Set @ref BT_OTS_OBJ_PROP_TRUNCATE property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_SET_PROP_TRUNCATE(prop) \
    WRITE_BIT(prop, BT_OTS_OBJ_PROP_TRUNCATE, 1)

/** @brief Set @ref BT_OTS_OBJ_PROP_PATCH property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_SET_PROP_PATCH(prop) \
    WRITE_BIT(prop, BT_OTS_OBJ_PROP_PATCH, 1)

/** @brief Set @ref BT_OTS_OBJ_SET_PROP_MARKED property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_SET_PROP_MARKED(prop) \
    WRITE_BIT(prop, BT_OTS_OBJ_PROP_MARKED, 1)

/** @brief Get @ref BT_OTS_OBJ_PROP_DELETE property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_GET_PROP_DELETE(prop) \
    ((prop) & BIT(BT_OTS_OBJ_PROP_DELETE))

/** @brief Get @ref BT_OTS_OBJ_PROP_EXECUTE property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_GET_PROP_EXECUTE(prop) \
    ((prop) & BIT(BT_OTS_OBJ_PROP_EXECUTE))

/** @brief Get @ref BT_OTS_OBJ_PROP_READ property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_GET_PROP_READ(prop) \
    ((prop) & BIT(BT_OTS_OBJ_PROP_READ))

/** @brief Get @ref BT_OTS_OBJ_PROP_WRITE property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_GET_PROP_WRITE(prop) \
    ((prop) & BIT(BT_OTS_OBJ_PROP_WRITE))

/** @brief Get @ref BT_OTS_OBJ_PROP_APPEND property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_GET_PROP_APPEND(prop) \
    ((prop) & BIT(BT_OTS_OBJ_PROP_APPEND))

/** @brief Get @ref BT_OTS_OBJ_PROP_TRUNCATE property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_GET_PROP_TRUNCATE(prop) \
    ((prop) & BIT(BT_OTS_OBJ_PROP_TRUNCATE))

/** @brief Get @ref BT_OTS_OBJ_PROP_PATCH property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_GET_PROP_PATCH(prop) \
    ((prop) & BIT(BT_OTS_OBJ_PROP_PATCH))

/** @brief Get @ref BT_OTS_OBJ_PROP_MARKED property.
 *
 *  @param prop Object properties.
 */
#define BT_OTS_OBJ_GET_PROP_MARKED(prop) \
    ((prop) & BIT(BT_OTS_OBJ_PROP_MARKED))

/** @brief Descriptor for OTS Object Size parameter. */
struct bt_ots_obj_size {
    /** @brief Current Size */
    uint32_t cur;

    /** @brief Allocated Size */
    uint32_t alloc;
} __packed;

/** @brief Object Action Control Point Feature bits. */
enum {
    /** Bit 0 OACP Create Op Code Supported */
    BT_OTS_OACP_FEAT_CREATE     = 0,

    /** Bit 1 OACP Delete Op Code Supported  */
    BT_OTS_OACP_FEAT_DELETE     = 1,

    /** Bit 2 OACP Calculate Checksum Op Code Supported */
    BT_OTS_OACP_FEAT_CHECKSUM   = 2,

    /** Bit 3 OACP Execute Op Code Supported */
    BT_OTS_OACP_FEAT_EXECUTE    = 3,

    /** Bit 4 OACP Read Op Code Supported */
    BT_OTS_OACP_FEAT_READ       = 4,

    /** Bit 5 OACP Write Op Code Supported */
    BT_OTS_OACP_FEAT_WRITE      = 5,

    /** Bit 6 Appending Additional Data to Objects Supported  */
    BT_OTS_OACP_FEAT_APPEND     = 6,

    /** Bit 7 Truncation of Objects Supported */
    BT_OTS_OACP_FEAT_TRUNCATE   = 7,

    /** Bit 8 Patching of Objects Supported  */
    BT_OTS_OACP_FEAT_PATCH      = 8,

    /** Bit 9 OACP Abort Op Code Supported */
    BT_OTS_OACP_FEAT_ABORT      = 9,
};

/*
 * @enum bt_ots_oacp_write_op_mode
 * @brief Mode Parameter for OACP Write Op Code.
 */
enum bt_ots_oacp_write_op_mode {
    BT_OTS_OACP_WRITE_OP_MODE_NONE = 0,
    BT_OTS_OACP_WRITE_OP_MODE_TRUNCATE = BIT(1),
};

/** @brief Set @ref BT_OTS_OACP_SET_FEAT_CREATE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_CREATE(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_CREATE, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_DELETE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_DELETE(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_DELETE, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_CHECKSUM feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_CHECKSUM(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_CHECKSUM, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_EXECUTE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_EXECUTE(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_EXECUTE, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_READ feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_READ(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_READ, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_WRITE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_WRITE(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_WRITE, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_APPEND feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_APPEND(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_APPEND, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_TRUNCATE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_TRUNCATE(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_TRUNCATE, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_PATCH feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_PATCH(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_PATCH, 1)

/** @brief Set @ref BT_OTS_OACP_FEAT_ABORT feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_SET_FEAT_ABORT(feat) \
    WRITE_BIT(feat, BT_OTS_OACP_FEAT_ABORT, 1)

/** @brief Get @ref BT_OTS_OACP_FEAT_CREATE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_CREATE(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_CREATE))

/** @brief Get @ref BT_OTS_OACP_FEAT_DELETE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_DELETE(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_DELETE))

/** @brief Get @ref BT_OTS_OACP_FEAT_CHECKSUM feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_CHECKSUM(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_CHECKSUM))

/** @brief Get @ref BT_OTS_OACP_FEAT_EXECUTE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_EXECUTE(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_EXECUTE))

/** @brief Get @ref BT_OTS_OACP_FEAT_READ feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_READ(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_READ))

/** @brief Get @ref BT_OTS_OACP_FEAT_WRITE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_WRITE(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_WRITE))

/** @brief Get @ref BT_OTS_OACP_FEAT_APPEND feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_APPEND(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_APPEND))

/** @brief Get @ref BT_OTS_OACP_FEAT_TRUNCATE feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_TRUNCATE(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_TRUNCATE))

/** @brief Get @ref BT_OTS_OACP_FEAT_PATCH feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_PATCH(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_PATCH))

/** @brief Get @ref BT_OTS_OACP_FEAT_ABORT feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OACP_GET_FEAT_ABORT(feat) \
    ((feat) & BIT(BT_OTS_OACP_FEAT_ABORT))

/** @brief Object List Control Point Feature bits. */
enum {
    /** Bit 0 OLCP Go To Op Code Supported */
    BT_OTS_OLCP_FEAT_GO_TO      = 0,

    /** Bit 1 OLCP Order Op Code Supported */
    BT_OTS_OLCP_FEAT_ORDER      = 1,

    /** Bit 2 OLCP Request Number of Objects Op Code Supported */
    BT_OTS_OLCP_FEAT_NUM_REQ    = 2,

    /** Bit 3 OLCP Clear Marking Op Code Supported*/
    BT_OTS_OLCP_FEAT_CLEAR      = 3,
};

/** @brief Set @ref BT_OTS_OLCP_FEAT_GO_TO feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OLCP_SET_FEAT_GO_TO(feat) \
    WRITE_BIT(feat, BT_OTS_OLCP_FEAT_GO_TO, 1)

/** @brief Set @ref BT_OTS_OLCP_FEAT_ORDER feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OLCP_SET_FEAT_ORDER(feat) \
    WRITE_BIT(feat, BT_OTS_OLCP_FEAT_ORDER, 1)

/** @brief Set @ref BT_OTS_OLCP_FEAT_NUM_REQ feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OLCP_SET_FEAT_NUM_REQ(feat) \
    WRITE_BIT(feat, BT_OTS_OLCP_FEAT_NUM_REQ, 1)

/** @brief Set @ref BT_OTS_OLCP_FEAT_CLEAR feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OLCP_SET_FEAT_CLEAR(feat) \
    WRITE_BIT(feat, BT_OTS_OLCP_FEAT_CLEAR, 1)

/** @brief Get @ref BT_OTS_OLCP_GET_FEAT_GO_TO feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OLCP_GET_FEAT_GO_TO(feat) \
    ((feat) & BIT(BT_OTS_OLCP_FEAT_GO_TO))

/** @brief Get @ref BT_OTS_OLCP_GET_FEAT_ORDER feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OLCP_GET_FEAT_ORDER(feat) \
    ((feat) & BIT(BT_OTS_OLCP_FEAT_ORDER))

/** @brief Get @ref BT_OTS_OLCP_GET_FEAT_NUM_REQ feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OLCP_GET_FEAT_NUM_REQ(feat) \
    ((feat) & BIT(BT_OTS_OLCP_FEAT_NUM_REQ))

/** @brief Get @ref BT_OTS_OLCP_GET_FEAT_CLEAR feature.
 *
 *  @param feat OTS features.
 */
#define BT_OTS_OLCP_GET_FEAT_CLEAR(feat) \
    ((feat) & BIT(BT_OTS_OLCP_FEAT_CLEAR))

/**@brief Features of the OTS. */
struct bt_ots_feat {
    /* OACP Features */
    uint32_t oacp;

    /* OLCP Features */
    uint32_t olcp;
} __packed;

/** @brief Object metadata request bit field values */
enum {
    /** @brief Request object name */
    BT_OTS_METADATA_REQ_NAME                        = BIT(0),
    /** @brief Request object type */
    BT_OTS_METADATA_REQ_TYPE                        = BIT(1),
    /** @brief Request object size */
    BT_OTS_METADATA_REQ_SIZE                        = BIT(2),
    /** @brief Request object first created time */
    BT_OTS_METADATA_REQ_CREATED                     = BIT(3),
    /** @brief Request object last modified time */
    BT_OTS_METADATA_REQ_MODIFIED                    = BIT(4),
    /** @brief Request object ID */
    BT_OTS_METADATA_REQ_ID                          = BIT(5),
    /** @brief Request object properties */
    BT_OTS_METADATA_REQ_PROPS                       = BIT(6),
    /** @brief Request all object metadata */
    BT_OTS_METADATA_REQ_ALL                         = 0x7F,
};

/** @brief Date and Time structure */
struct bt_ots_date_time {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};
#define BT_OTS_DATE_TIME_FIELD_SIZE 7

#ifndef CONFIG_BT_OTS_OBJ_MAX_NAME_LEN
#define CONFIG_BT_OTS_OBJ_MAX_NAME_LEN 120
#endif /* CONFIG_BT_OTS_OBJ_MAX_NAME_LEN */

/** @brief Metadata of an OTS object
 *
 * Used by the server as a descriptor for OTS object initialization.
 * Used by the client to present object metadata to the application.
 */
struct bt_ots_obj_metadata {

    /** @brief Object Name */
    char                           *name;

    /* TODO: Unify client/server name */
    /** @brief Object name (client) */
    char                           name_c[CONFIG_BT_OTS_OBJ_MAX_NAME_LEN + 1];

    /** @brief Object Type */
    struct bt_ots_obj_type         type;

    /** @brief Object Size */
    struct bt_ots_obj_size         size;

    /** @brief Object first created time */
    struct bt_ots_date_time        first_created;

    /** @brief Object last modified time */
    struct bt_ots_date_time        modified;

    /** @brief Object ID */
    uint64_t                       id;

    /** @brief Object Properties */
    uint32_t                       props;
};

/** @brief Opaque OTS instance. */
struct bt_ots;

/** @brief Descriptor for OTS object addition */
struct bt_ots_obj_add_param {
    /** @brief Object size to allocate */
    uint32_t size;

    /** @brief Object type */
    struct bt_ots_obj_type type;
};

/** @brief Descriptor for OTS created object.
 *
 *  Descriptor for OTS object created by the application. This descriptor is
 *  returned by @ref bt_ots_cb.obj_created callback which contains further
 *  documentation on distinguishing between server and client object creation.
 */
struct bt_ots_obj_created_desc {
    /** @brief Object name
     *
     *  The object name as a NULL terminated string.
     *
     *  When the server creates a new object the name
     *  shall be > 0 and <= BT_OTS_OBJ_MAX_NAME_LEN
     *  When the client creates a new object the name
     *  shall be an empty string
     */
    char *name;

    /** @brief Object size
     *
     *  @ref bt_ots_obj_size.alloc shall be >= @ref bt_ots_obj_add_param.size
     *
     *  When the server creates a new object @ref bt_ots_obj_size.cur
     *  shall be <= @ref bt_ots_obj_add_param.size
     *  When the client creates a new object @ref bt_ots_obj_size.cur
     *  shall be 0
     */
    struct bt_ots_obj_size size;

    /** @brief Object properties */
    uint32_t props;
};

/** @brief OTS callback structure. */
struct bt_ots_cb {
    /** @brief Object created callback
     *
     *  This callback is called whenever a new object is created.
     *  Application can reject this request by returning an error
     *  when it does not have necessary resources to hold this new
     *  object. This callback is also triggered when the server
     *  creates a new object with bt_ots_obj_add() API.
     *
     *  @param ots           OTS instance.
     *  @param conn          The connection that is requesting object creation or
     *                       NULL if object is created by bt_ots_obj_add().
     *  @param id            Object ID.
     *  @param add_param     Object creation requested parameters.
     *  @param created_desc  Created object descriptor that shall be filled by the
     *                       receiver of this callback.
     *
     *  @return 0 in case of success or negative value in case of error.
     *  @return -ENOTSUP if object type is not supported
     *  @return -ENOMEM if no available space for new object.
     *  @return -EINVAL if an invalid parameter is provided
     *  @return other negative values are treated as a generic operation failure
     */
    int (*obj_created)(struct bt_ots *ots, struct bt_conn *conn, uint64_t id,
                       const struct bt_ots_obj_add_param *add_param,
                       struct bt_ots_obj_created_desc *created_desc);

    /** @brief Object deleted callback
     *
     *  This callback is called whenever an object is deleted. It is
     *  also triggered when the server deletes an object with
     *  bt_ots_obj_delete() API.
     *
     *  @param ots  OTS instance.
     *  @param conn The connection that deleted the object or NULL if
     *              this request came from the server.
     *  @param id   Object ID.
     *
     *  @retval When an error is indicated by using a negative value, the
     *          object delete procedure is aborted and a corresponding failed
     *          status is returned to the client.
     *  @return 0 in case of success.
     *  @return -EBUSY if the object is locked. This is generally not expected
     *          to be returned by the application as the OTS layer tracks object
     *          accesses. An object locked status is returned to the client.
     *  @return Other negative values in case of error. A generic operation
     *          failed status is returned to the client.
     */
    int (*obj_deleted)(struct bt_ots *ots, struct bt_conn *conn,
                       uint64_t id);

    /** @brief Object selected callback
     *
     *  This callback is called on successful object selection.
     *
     *  @param ots  OTS instance.
     *  @param conn The connection that selected new object.
     *  @param id   Object ID.
     */
    void (*obj_selected)(struct bt_ots *ots, struct bt_conn *conn,
                         uint64_t id);

    /** @brief Object read callback
     *
     *  This callback is called multiple times during the Object read
     *  operation. OTS module will keep requesting successive Object
     *  fragments from the application until the read operation is
     *  completed. The end of read operation is indicated by NULL data
     *  parameter.
     *
     *  @param ots    OTS instance.
     *  @param conn   The connection that read object.
     *  @param id     Object ID.
     *  @param data   In:  NULL once the read operations is completed.
     *                Out: Next chunk of data to be sent.
     *  @param len    Remaining length requested by the client.
     *  @param offset Object data offset.
     *
     *  @return Data length to be sent via data parameter. This value
     *          shall be smaller or equal to the len parameter.
     *  @return Negative value in case of an error.
     */
    ssize_t (*obj_read)(struct bt_ots *ots, struct bt_conn *conn,
                        uint64_t id, void **data, size_t len,
                        off_t offset);

    /** @brief Object write callback
     *
     *  This callback is called multiple times during the Object write
     *  operation. OTS module will keep providing successive Object
     *  fragments to the application until the write operation is
     *  completed. The offset and length of each write fragment is
     *  validated by the OTS module to be within the allocated size
     *  of the object. The remaining length indicates data length
     *  remaining to be written and will decrease each write iteration
     *  until it reaches 0 in the last write fragment.
     *
     *  @param ots    OTS instance.
     *  @param conn   The connection that wrote object.
     *  @param id     Object ID.
     *  @param data   Next chunk of data to be written.
     *  @param len    Length of the current chunk of data in the buffer.
     *  @param offset Object data offset.
     *  @param rem    Remaining length in the write operation.
     *
     *  @return Number of bytes written in case of success, if the number
     *          of bytes written does not match len, -EIO is returned to
     *          the L2CAP layer.
     *  @return A negative value in case of an error.
     *  @return -EINPROGRESS has a special meaning and is unsupported at
     *          the moment. It should not be returned.
     */
    ssize_t (*obj_write)(struct bt_ots *ots, struct bt_conn *conn, uint64_t id,
                         const void *data, size_t len, off_t offset,
                         size_t rem);

    /** @brief Object name written callback
     *
     *  This callback is called when the object name is written.
     *  This is a notification to the application that the object name
     *  will be updated by the OTS service implementation.
     *
     *  @param ots        OTS instance.
     *  @param conn       The connection that wrote object name.
     *  @param id         Object ID.
     *  @param cur_name   Current object name.
     *  @param new_name   New object name.
     */
    void (*obj_name_written)(struct bt_ots *ots, struct bt_conn *conn,
                             uint64_t id, const char *cur_name, const char *new_name);

    /** @brief Object Calculate checksum callback
     *
     *  This callback is called when the OACP Calculate Checksum procedure is performed.
     *  Because object data is opaque to OTS, the application is the only one who
     *  knows where data is and should return pointer of actual object data.
     *
     *  @param[in]  ots      OTS instance.
     *  @param[in]  conn     The connection that wrote object.
     *  @param[in]  id       Object ID.
     *  @param[in]  offset   The first octet of the object contents need to be calculated.
     *  @param[in]  len      The length number of octets object name.
     *  @param[out] data     Pointer of actual object data.
     *
     *  @return 0 to accept, or any negative value to reject.
     */
    int (*obj_cal_checksum)(struct bt_ots *ots, struct bt_conn *conn, uint64_t id,
                            off_t offset, size_t len, void **data);
};

/** @brief Descriptor for OTS initialization. */
struct bt_ots_init_param {
    /* OTS features */
    struct bt_ots_feat features;

    /* Callbacks */
    struct bt_ots_cb *cb;
};

/** @brief Add an object to the OTS instance.
 *
 *  This function adds an object to the OTS database. When the
 *  object is being added, a callback obj_created() is called
 *  to notify the user about a new object ID.
 *
 *  @param ots      OTS instance.
 *  @param param    Object addition parameters.
 *
 *  @return ID of created object in case of success.
 *  @return negative value in case of error.
 */
int bt_ots_obj_add(struct bt_ots *ots, const struct bt_ots_obj_add_param *param);

/** @brief Delete an object from the OTS instance.
 *
 *  This function deletes an object from the OTS database. When the
 *  object is deleted a callback obj_deleted() is called
 *  to notify the user about this event. At this point, it is possible
 *  to free allocated buffer for object data.
 *
 *  @param ots OTS instance.
 *  @param id  ID of the object to be deleted (uint48).
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_ots_obj_delete(struct bt_ots *ots, uint64_t id);

/** @brief Get the service declaration attribute.
 *
 *  This function is enabled for CONFIG_BT_OTS_SECONDARY_SVC configuration.
 *  The first service attribute can be included in any other GATT service.
 *
 *  @param ots OTS instance.
 *
 *  @return The first OTS attribute instance.
 */
void *bt_ots_svc_decl_get(struct bt_ots *ots);

/** @brief Initialize the OTS instance.
 *
 *  @param ots      OTS instance.
 *  @param ots_init OTS initialization descriptor.
 *
 *  @return 0 in case of success or negative value in case of error.
 */
int bt_ots_init(struct bt_ots *ots, struct bt_ots_init_param *ots_init);

/** @brief Get a free instance of OTS from the pool.
 *
 *  @return OTS instance in case of success or NULL in case of error.
 */
struct bt_ots *bt_ots_free_instance_get(void);

#define BT_OTS_STOP                       0
#define BT_OTS_CONTINUE                   1

/* TODO: Merge server and client instance as opaque type */
/** @brief OTS client instance */
struct bt_ots_client {
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t feature_handle;
    uint16_t obj_name_handle;
    uint16_t obj_type_handle;
    uint16_t obj_size_handle;
    uint16_t obj_properties_handle;
    uint16_t obj_created_handle;
    uint16_t obj_modified_handle;
    uint16_t obj_id_handle;
    uint16_t oacp_handle;
    uint16_t olcp_handle;

    struct bt_gatt_subscribe_params oacp_sub_params;
    struct bt_gatt_discover_params oacp_sub_disc_params;
    struct bt_gatt_subscribe_params olcp_sub_params;
    struct bt_gatt_discover_params olcp_sub_disc_params;

    struct bt_gatt_write_params write_params;
    struct bt_gatt_read_params read_proc;
    struct bt_ots_client_cb *cb;

    struct bt_ots_feat features;

    struct bt_ots_obj_metadata cur_object;
};

/** OTS client callback structure */
struct bt_ots_client_cb {
    /** @brief Callback function when a new object is selected.
     *
     *  Called when the a new object is selected and the current
     *  object has changed. The `cur_object` in `ots_inst` will
     *  have been reset, and metadata should be read again with
     *  bt_ots_client_read_object_metadata().
     *
     *  @param ots_inst          Pointer to the OTC instance.
     *  @param conn              The connection to the peer device.
     *  @param err               Error code (bt_ots_olcp_res_code).
     */
    void (*obj_selected)(struct bt_ots_client *ots_inst,
                         struct bt_conn *conn, int err);

    /** @brief Callback function for the data of the selected
     * object.
     *
     *  Called when the data of the selected object are read using
     *  bt_ots_client_read_object_data().
     *
     *  @param ots_inst      Pointer to the OTC instance.
     *  @param conn          The connection to the peer device.
     *  @param offset        Offset of the received data.
     *  @param len           Length of the received data.
     *  @param data_p        Pointer to the received data.
     *  @param is_complete   Indicate if the whole object has been received.
     *
     *  @return int          BT_OTS_STOP or BT_OTS_CONTINUE. BT_OTS_STOP can
     *                       be used to stop reading.
     */
    int (*obj_data_read)(struct bt_ots_client *ots_inst,
                         struct bt_conn *conn, uint32_t offset,
                         uint32_t len, uint8_t *data_p, bool is_complete);

    /** @brief Callback function for metadata of the selected object.
     *
     *  Called when metadata of the selected object are read using
     *  bt_ots_client_read_object_metadata().
     *  Not all of the metadata may have been initialized.
     *
     *  @param ots_inst          Pointer to the OTC instance.
     *  @param conn              The connection to the peer device.
     *  @param err               Error value. 0 on success,
     *                           GATT error or ERRNO on fail.
     *  @param metadata_read     Bitfield of the metadata that was
     *                           successfully read.
     */
    void (*obj_metadata_read)(struct bt_ots_client *ots_inst,
                              struct bt_conn *conn, int err,
                              uint8_t metadata_read);

    /** @brief Callback function for the data of the write object.
     *
     *  Called when the data of the selected object is written using
     *  bt_ots_client_write_object_data().
     *
     *  @param ots_inst      Pointer to the OTC instance.
     *  @param conn          The connection to the peer device.
     *  @param len           Length of the written data.
     */
    void (*obj_data_written)(struct bt_ots_client *ots_inst,
                             struct bt_conn *conn, size_t len);

    /** @brief Callback function when checksum indication is received.
     *
     *  Called when the oacp_ind_handler received response of
     *  OP BT_GATT_OTS_OACP_PROC_CHECKSUM_CALC.
     *
     *  @param ots_inst          Pointer to the OTC instance.
     *  @param conn              The connection to the peer device.
     *  @param err               Error code (bt_gatt_ots_oacp_res_code).
     *  @param checksum          Checksum if error code is BT_GATT_OTS_OACP_RES_SUCCESS,
     *                           otherwise 0.
     */
    void (*obj_checksum_calculated)(struct bt_ots_client *ots_inst, struct bt_conn *conn,
                                    int err, uint32_t checksum);
};

/** @brief Register an Object Transfer Service Instance.
 *
 *  Register an Object Transfer Service instance discovered on the peer.
 *  Call this function when an OTS instance is discovered
 *  (discovery is to be handled by the higher layer).
 *
 *  @param[in]  ots_inst      Discovered OTS instance.
 *
 *  @return int               0 if success, ERRNO on failure.
 */
int bt_ots_client_register(struct bt_ots_client *ots_inst);

/** @brief Unregister an Object Transfer Service Instance.
 *
 *  Unregister an Object Transfer Service instance when disconnect from the peer.
 *  Call this function when an ACL using OTS instance is disconnected.
 *
 *  @param[in]  index         Index of OTS instance.
 *
 *  @return int               0 if success, ERRNO on failure.
 */
int bt_ots_client_unregister(uint8_t index);

/** @brief OTS Indicate Handler function.
 *
 *  Set this function as callback for indicate handler when discovering OTS.
 *
 *   @param conn        Connection object. May be NULL, indicating that the
 *                      peer is being unpaired.
 *   @param params      Subscription parameters.
 *   @param data        Attribute value data. If NULL then subscription was
 *                      removed.
 *   @param length      Attribute value length.
 */
uint8_t bt_ots_client_indicate_handler(struct bt_conn *conn,
                                       struct bt_gatt_subscribe_params *params,
                                       const void *data, uint16_t length);

/** @brief Read the OTS feature characteristic.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_read_feature(struct bt_ots_client *otc_inst,
                               struct bt_conn *conn);

/** @brief Select an object by its Object ID.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *  @param obj_id       Object's ID.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_select_id(struct bt_ots_client *otc_inst,
                            struct bt_conn *conn,
                            uint64_t obj_id);

/** @brief Select the first object.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_select_first(struct bt_ots_client *otc_inst,
                               struct bt_conn *conn);

/** @brief Select the last object.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_select_last(struct bt_ots_client *otc_inst,
                              struct bt_conn *conn);

/** @brief Select the next object.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_select_next(struct bt_ots_client *otc_inst,
                              struct bt_conn *conn);

/** @brief Select the previous object.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_select_prev(struct bt_ots_client *otc_inst,
                              struct bt_conn *conn);

/** @brief Read the metadata of the current object.
 *
 *  The metadata are returned in the obj_metadata_read() callback.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *  @param metadata     Bitfield (`BT_OTS_METADATA_REQ_*`) of the metadata
 *                      to read.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_read_object_metadata(struct bt_ots_client *otc_inst,
                                       struct bt_conn *conn,
                                       uint8_t metadata);

/** @brief Read the data of the current selected object.
 *
 *  This will trigger an OACP read operation for the current size of the object
 *  with a 0 offset and then expect receiving the content via the L2CAP CoC.
 *
 *  The data of the object are returned in the obj_data_read() callback.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_read_object_data(struct bt_ots_client *otc_inst,
                                   struct bt_conn *conn);

/** @brief Write the data of the current selected object.
 *
 *  This will trigger an OACP write operation for the current object
 *  with a specified offset and then expect transferring the content via the L2CAP CoC.
 *
 *  The length of the data written to object is returned in the obj_data_written() callback.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *  @param buf          Pointer to the data buffer to be written.
 *  @param len          Size of data.
 *  @param offset       Offset to write, usually 0.
 *  @param mode         Mode Parameter for OACP Write Op Code. See @ref bt_ots_oacp_write_op_mode.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_write_object_data(struct bt_ots_client *otc_inst, struct bt_conn *conn,
                                    const void *buf, size_t len, off_t offset,
                                    enum bt_ots_oacp_write_op_mode mode);

/** @brief Get the checksum of the current selected object.
 *
 *  This will trigger an OACP calculate checksum operation for the current object
 *  with a specified offset and length.
 *
 *  The checksum goes to OACP IND and obj_checksum_calculated() callback.
 *
 *  @param otc_inst     Pointer to the OTC instance.
 *  @param conn         Pointer to the connection object.
 *  @param offset       Offset to calculate, usually 0.
 *  @param len          Len of data to calculate checksum for. May be less than the current object's
 *                      size, but shall not be larger.
 *
 *  @return int         0 if success, ERRNO on failure.
 */
int bt_ots_client_get_object_checksum(struct bt_ots_client *otc_inst, struct bt_conn *conn,
                                      off_t offset, size_t len);

/** @brief Directory listing object metadata callback
 *
 * If a directory listing is decoded using bt_ots_client_decode_dirlisting(),
 * this callback will be called for each object in the directory listing.
 *
 *  @param meta The metadata of the decoded object
 *
 *  @return int   BT_OTS_STOP or BT_OTS_CONTINUE. BT_OTS_STOP can be used to
 *                stop the decoding.
 */
typedef int (*bt_ots_client_dirlisting_cb)(struct bt_ots_obj_metadata *meta);

/** @brief Decode Directory Listing object into object metadata.
 *
 *  If the Directory Listing object contains multiple objects, then the
 *  callback will be called for each of them.
 *
 *  @param data        The data received for the directory listing object.
 *  @param length      Length of the data.
 *  @param cb          The callback that will be called for each object.
 */
int bt_ots_client_decode_dirlisting(uint8_t *data, uint16_t length,
                                    bt_ots_client_dirlisting_cb cb);

/** @brief Converts binary OTS Object ID to string.
 *
 *  @param obj_id Object ID.
 *  @param str    Address of user buffer with enough room to store
 *                formatted string containing binary Object ID.
 *  @param len    Length of data to be copied to user string buffer.
 *                Refer to BT_OTS_OBJ_ID_STR_LEN about
 *                recommended value.
 *
 *  @return Number of successfully formatted bytes from binary ID.
 */
static inline int bt_ots_obj_id_to_str(uint64_t obj_id, char *str, size_t len)
{
    uint8_t id[6];

    sys_put_le48(obj_id, id);

    return snprintf(str, len, "%02X%02X%02X%02X%02X%02X",
                    id[5], id[4], id[3], id[2], id[1], id[0]);
}

/** @brief Displays one or more object metadata as text with LOG_INF.
 *
 * @param metadata Pointer to the first (or only) metadata in an array.
 * @param count    Number of metadata objects to display information of.
 */
void bt_ots_metadata_display(struct bt_ots_obj_metadata *metadata,
                             uint16_t count);

/**
 * @brief  Generate IEEE conform CRC32 checksum.
 *
 * To abstract IEEE implementation to service layer.
 *
 * @param  data        Pointer to data on which the CRC should be calculated.
 * @param  len          Data length.
 *
 * @return CRC32 value.
 *
 */
static inline uint32_t bt_ots_client_calc_checksum(const uint8_t *data, size_t len)
{
    return crc32_ieee(data, len);
}

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_OTS_H_ */
