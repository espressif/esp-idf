/*
 * SPDX-FileCopyrightText: 2020-2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BT_GATT_OTS_INTERNAL_H_
#define BT_GATT_OTS_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/types.h>
#include "ots_l2cap_internal.h"
#include "ots_oacp_internal.h"
#include "ots_olcp_internal.h"

/**
 * Both OACP and OLCP have same max size of 7 bytes
 *
 * Table 3.10: Format of OACP Response Value
 * OACP Response Value contains
 * 1 octet Procedure code
 * 1 octet Request op code
 * 1 octet Result Code
 * 4 octet CRC checksum (if present)
 *
 * Table 3.24: Format of the OLCP Response Value
 * 1 octet Procedure code
 * 1 octet Request op code
 * 1 octet Result Code
 * 0 or 4 octets Response Parameter
 *
 **/
#define OACP_OLCP_RES_MAX_SIZE  7

#define BT_OTS_VALID_OBJ_ID(id) (IN_RANGE((id), BT_OTS_OBJ_ID_MIN, BT_OTS_OBJ_ID_MAX) || \
                 (id) == OTS_OBJ_ID_DIR_LIST)

#define BT_OTS_SET_METADATA_REQ_NAME(metadata) \
    ((metadata) = (metadata) | BT_OTS_METADATA_REQ_NAME)
#define BT_OTS_SET_METADATA_REQ_TYPE(metadata) \
    ((metadata) = (metadata) | BT_OTS_METADATA_REQ_TYPE)
#define BT_OTS_SET_METADATA_REQ_SIZE(metadata) \
    ((metadata) = (metadata) | BT_OTS_METADATA_REQ_SIZE)
#define BT_OTS_SET_METADATA_REQ_CREATED(metadata) \
    ((metadata) = (metadata) | BT_OTS_METADATA_REQ_CREATED)
#define BT_OTS_SET_METADATA_REQ_MODIFIED(metadata) \
    ((metadata) = (metadata) | BT_OTS_METADATA_REQ_MODIFIED)
#define BT_OTS_SET_METADATA_REQ_ID(metadata) \
    ((metadata) = (metadata) | BT_OTS_METADATA_REQ_ID)
#define BT_OTS_SET_METADATA_REQ_PROPS(metadata) \
    ((metadata) = (metadata) | BT_OTS_METADATA_REQ_PROPS)

#define BT_OTS_GET_METADATA_REQ_NAME(metadata) \
    ((metadata) & BT_OTS_METADATA_REQ_NAME)
#define BT_OTS_GET_METADATA_REQ_TYPE(metadata) \
    ((metadata) & BT_OTS_METADATA_REQ_TYPE)
#define BT_OTS_GET_METADATA_REQ_SIZE(metadata) \
    ((metadata) & BT_OTS_METADATA_REQ_SIZE)
#define BT_OTS_GET_METADATA_REQ_CREATED(metadata) \
    ((metadata) & BT_OTS_METADATA_REQ_CREATED)
#define BT_OTS_GET_METADATA_REQ_MODIFIED(metadata) \
    ((metadata) & BT_OTS_METADATA_REQ_MODIFIED)
#define BT_OTS_GET_METADATA_REQ_ID(metadata) \
    ((metadata) & BT_OTS_METADATA_REQ_ID)
#define BT_OTS_GET_METADATA_REQ_PROPS(metadata) \
    ((metadata) & BT_OTS_METADATA_REQ_PROPS)

/**@brief OTS Attribute Protocol Application Error codes. */
enum bt_gatt_ots_att_err_codes {
    /** An attempt was made to write a value that is invalid or
     *  not supported by this Server for a reason other than
     *  the attribute permissions.
     */
    BT_GATT_OTS_WRITE_REQUEST_REJECTED     = 0x80,
    /** An attempt was made to read or write to an Object Metadata
     *  characteristic while the Current Object was an Invalid Object.
     */
    BT_GATT_OTS_OBJECT_NOT_SELECTED        = 0x81,
    /** The Server is unable to service the Read Request or Write Request
     *  because it exceeds the concurrency limit of the service.
     */
    BT_GATT_OTS_CONCURRENCY_LIMIT_EXCEEDED = 0x82,
    /** The requested object name was rejected because
     *  the name was already in use by an existing object on the Server.
     */
    BT_GATT_OTS_OBJECT_NAME_ALREADY_EXISTS = 0x83,
};

enum bt_gatt_ots_object_state_type {
    BT_GATT_OTS_OBJECT_IDLE_STATE,

    BT_GATT_OTS_OBJECT_READ_OP_STATE,

    BT_GATT_OTS_OBJECT_WRITE_OP_STATE,
};

struct bt_gatt_ots_object_state {
    enum bt_gatt_ots_object_state_type type;
    union {
        struct bt_gatt_ots_object_read_op {
            struct bt_gatt_ots_oacp_read_params oacp_params;
            uint32_t sent_len;
        } read_op;
        struct bt_gatt_ots_object_write_op {
            struct bt_gatt_ots_oacp_write_params oacp_params;
            uint32_t recv_len;
        } write_op;
    };
};

struct bt_gatt_ots_object {
    uint64_t id;
    struct bt_ots_obj_metadata metadata;
    struct bt_gatt_ots_object_state state;
};

struct bt_gatt_ots_indicate {
    struct bt_gatt_indicate_params params;
    struct bt_gatt_attr attr;
    struct bt_gatt_ccc_managed_user_data ccc;
    bool is_enabled;
    struct k_work work;
    uint8_t res[OACP_OLCP_RES_MAX_SIZE];
};

struct bt_ots {
    struct bt_ots_feat features;
    struct bt_gatt_ots_object *cur_obj;
    struct bt_gatt_service *service;
    struct bt_gatt_ots_indicate oacp_ind;
    struct bt_gatt_ots_indicate olcp_ind;
    struct bt_gatt_ots_l2cap l2cap;
    struct bt_ots_cb *cb;
    struct bt_ots_dir_list *dir_list;
    void *obj_manager;
};

int bt_ots_obj_add_internal(struct bt_ots *ots, struct bt_conn *conn,
                            const struct bt_ots_obj_add_param *param,
                            struct bt_gatt_ots_object **obj);
#ifdef __cplusplus
}
#endif

#endif /* BT_GATT_OTS_INTERNAL_H_ */
