/*
 * SPDX-FileCopyrightText: 2020-2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BT_GATT_OTS_OLCP_H_
#define BT_GATT_OTS_OLCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <zephyr/types.h>
#include <zephyr/bluetooth/gatt.h>

/* The types of OLCP procedures. */
enum bt_gatt_ots_olcp_proc_type {
    /* Select the first object.*/
    BT_GATT_OTS_OLCP_PROC_FIRST         = 0x01,
    /* Select the last object.*/
    BT_GATT_OTS_OLCP_PROC_LAST          = 0x02,
    /* Select the previous object.*/
    BT_GATT_OTS_OLCP_PROC_PREV          = 0x03,
    /* Select the next object.*/
    BT_GATT_OTS_OLCP_PROC_NEXT          = 0x04,
    /* Select the object with the given object ID.*/
    BT_GATT_OTS_OLCP_PROC_GOTO          = 0x05,
    /* Order the objects.*/
    BT_GATT_OTS_OLCP_PROC_ORDER         = 0x06,
    /* Request the number of objects.*/
    BT_GATT_OTS_OLCP_PROC_REQ_NUM_OBJS  = 0x07,
    /* Clear Marking.*/
    BT_GATT_OTS_OLCP_PROC_CLEAR_MARKING = 0x08,
    /* Response.*/
    BT_GATT_OTS_OLCP_PROC_RESP          = 0x70,
};

/** @brief The types of OLCP sort orders. */
enum bt_ots_olcp_sort_order {
    /** Order the list by object name, ascending */
    BT_OTS_SORT_BY_NAME_ASCEND     = 0x01,
    /** Order the list by object type, ascending*/
    BT_OTS_SORT_BY_TYPE_ASCEND     = 0x02,
    /** Order the list by object current size, ascending*/
    BT_OTS_SORT_BY_SIZE_ASCEND     = 0x03,
    /** Order the list by object first-created timestamp, ascending*/
    BT_OTS_SORT_BY_FC_ASCEND       = 0x04,
    /** Order the list by object last-modified timestamp, ascending */
    BT_OTS_SORT_BY_LM_ASCEND       = 0x05,
    /** Order the list by object name, descending */
    BT_OTS_SORT_BY_NAME_DESCEND    = 0x11,
    /** Order the list by object type, descending*/
    BT_OTS_SORT_BY_TYPE_DESCEND    = 0x12,
    /** Order the list by object current size, descending*/
    BT_OTS_SORT_BY_SIZE_DESCEND    = 0x13,
    /** Order the list by object first-created timestamp, descending*/
    BT_OTS_SORT_BY_FC_DESCEND      = 0x14,
    /** Order the list by object last-modified timestamp, descending */
    BT_OTS_SORT_BY_LM_DESCEND      = 0x15,
};

/* Definition of a OLCP procedure. */
struct bt_gatt_ots_olcp_proc {
    enum bt_gatt_ots_olcp_proc_type type;
    union {
        struct {
            uint64_t id;
        } goto_params;
    };
};

/* Size of Object List Control Point goto procedure */
#define BT_GATT_OTS_OLCP_GOTO_PARAMS_SIZE 6

/* The return codes obtained from doing OLCP procedures. */
enum bt_gatt_ots_olcp_res_code {
    /* Response for successful operation. */
    BT_GATT_OTS_OLCP_RES_SUCCESS = 0x01,
    /* Response if unsupported Op Code is received.*/
    BT_GATT_OTS_OLCP_RES_PROC_NOT_SUP = 0x02,
    /* Response if Parameter received does not meet
     * the requirements of the service.
     */
    BT_GATT_OTS_OLCP_RES_INVALID_PARAMETER = 0x03,
    /* Response if the requested procedure failed for a reason
     * other than those enumerated below.
     */
    BT_GATT_OTS_OLCP_RES_OPERATION_FAILED = 0x04,
    /* Response if the requested procedure attempted to select an object
     * beyond the first object or
     * beyond the last object in the current list.
     */
    BT_GATT_OTS_OLCP_RES_OUT_OF_BONDS = 0x05,
    /* Response if the requested procedure failed due
     * to too many objects in the current list.
     */
    BT_GATT_OTS_OLCP_RES_TOO_MANY_OBJECTS = 0x06,
    /* Response if the requested procedure failed due
     * to there being zero objects in the current list.
     */
    BT_GATT_OTS_OLCP_RES_NO_OBJECT = 0x07,
    /* Response if the requested procedure failed due
     * to there being no object with the requested Object ID.
     */
    BT_GATT_OTS_OLCP_RES_OBJECT_ID_NOT_FOUND = 0x08,
};

ssize_t bt_gatt_ots_olcp_write(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len,
                               uint16_t offset, uint8_t flags);

void bt_gatt_ots_olcp_cfg_changed(const struct bt_gatt_attr *attr,
                                  uint16_t value);

#ifdef __cplusplus
}
#endif

#endif /* BT_GATT_OTS_OLCP_H_ */
