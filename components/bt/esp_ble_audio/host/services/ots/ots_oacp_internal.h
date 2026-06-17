/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BT_GATT_OTS_OACP_H_
#define BT_GATT_OTS_OACP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <zephyr/types.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/services/ots.h>

/* Types of Object Action Control Point Procedures. */
enum bt_gatt_ots_oacp_proc_type {
    /** Create object.*/
    BT_GATT_OTS_OACP_PROC_CREATE        = 0x01,
    /** Delete object.*/
    BT_GATT_OTS_OACP_PROC_DELETE        = 0x02,
    /** Calculate Checksum.*/
    BT_GATT_OTS_OACP_PROC_CHECKSUM_CALC = 0x03,
    /** Execute Object.*/
    BT_GATT_OTS_OACP_PROC_EXECUTE       = 0x04,
    /** Read object.*/
    BT_GATT_OTS_OACP_PROC_READ          = 0x05,
    /** Write object.*/
    BT_GATT_OTS_OACP_PROC_WRITE         = 0x06,
    /** Abort object.*/
    BT_GATT_OTS_OACP_PROC_ABORT         = 0x07,
    /** Procedure response.*/
    BT_GATT_OTS_OACP_PROC_RESP          = 0x60
};

/* Object Action Control Point return codes. */
enum bt_gatt_ots_oacp_res_code {
    /** Success.*/
    BT_GATT_OTS_OACP_RES_SUCCESS        = 0x01,
    /** Not supported*/
    BT_GATT_OTS_OACP_RES_OPCODE_NOT_SUP = 0x02,
    /** Invalid parameter*/
    BT_GATT_OTS_OACP_RES_INV_PARAM      = 0x03,
    /** Insufficient resources.*/
    BT_GATT_OTS_OACP_RES_INSUFF_RES     = 0x04,
    /** Invalid object.*/
    BT_GATT_OTS_OACP_RES_INV_OBJ        = 0x05,
    /** Channel unavailable.*/
    BT_GATT_OTS_OACP_RES_CHAN_UNAVAIL   = 0x06,
    /** Unsupported procedure.*/
    BT_GATT_OTS_OACP_RES_UNSUP_TYPE     = 0x07,
    /** Procedure not permitted.*/
    BT_GATT_OTS_OACP_RES_NOT_PERMITTED  = 0x08,
    /** Object locked.*/
    BT_GATT_OTS_OACP_RES_OBJ_LOCKED     = 0x09,
    /** Operation Failed.*/
    BT_GATT_OTS_OACP_RES_OPER_FAILED    = 0x0A
};

#define BT_GATT_OTS_OACP_PROC_WRITE_MODE_TRUNC 1

#define BT_GATT_OTS_OACP_PROC_WRITE_MODE_GET_TRUNC(mode) \
    ((mode) & BIT(BT_GATT_OTS_OACP_PROC_WRITE_MODE_TRUNC))

#define BT_GATT_OTS_OACP_PROC_WRITE_MODE_GET_RFU(mode) \
    ((mode) & ~BIT(BT_GATT_OTS_OACP_PROC_WRITE_MODE_TRUNC))

/* Object Action Control Point procedure definition. */
struct bt_gatt_ots_oacp_proc {
    enum bt_gatt_ots_oacp_proc_type type;
    union {
        struct bt_gatt_ots_oacp_create_params {
            uint32_t size;
            struct bt_ots_obj_type type;
        } create_params;
        struct bt_gatt_ots_oacp_cs_calc_params {
            uint32_t offset;
            uint32_t len;
        } cs_calc_params;
        struct bt_gatt_ots_oacp_read_params {
            uint32_t offset;
            uint32_t len;
        } read_params;
        struct bt_gatt_ots_oacp_write_params {
            uint32_t offset;
            uint32_t len;
            uint8_t mode;
        } write_params;
    };
};

/* Size of the generic part of the Object Action Control Point create procedure */
#define BT_GATT_OTS_OACP_CREATE_GENERIC_PARAMS_SIZE 4

/* Size of Object Action Control Point checksum calculation procedure */
#define BT_GATT_OTS_OACP_CS_CALC_PARAMS_SIZE 8

/* Size of Object Action Control Point read procedure */
#define BT_GATT_OTS_OACP_READ_PARAMS_SIZE 8

/* Size of Object Action Control Point write procedure */
#define BT_GATT_OTS_OACP_WRITE_PARAMS_SIZE 9

ssize_t bt_gatt_ots_oacp_write(struct bt_conn *conn,
                               const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len,
                               uint16_t offset, uint8_t flags);

void bt_gatt_ots_oacp_cfg_changed(const struct bt_gatt_attr *attr,
                                  uint16_t value);

#ifdef __cplusplus
}
#endif

#endif /* BT_GATT_OTS_OACP_H_ */
