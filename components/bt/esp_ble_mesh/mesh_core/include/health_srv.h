/** @file
 *  @brief Bluetooth Mesh Health Server Model APIs.
 */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_HEALTH_SRV_H_
#define _BLE_MESH_HEALTH_SRV_H_

#include "mesh_access.h"

/**
 * @brief Bluetooth Mesh Health Server Model
 * @defgroup bt_mesh_health_srv Bluetooth Mesh Health Server Model
 * @ingroup bt_mesh
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_health_srv_cb {
    /* Clear registered faults */
    void (*fault_clear)(struct bt_mesh_model *model, uint16_t company_id);

    /* Run a specific test */
    void (*fault_test)(struct bt_mesh_model *model, uint8_t test_id,
                       uint16_t company_id);

    /* Attention on */
    void (*attn_on)(struct bt_mesh_model *model, uint8_t time);

    /* Attention off */
    void (*attn_off)(struct bt_mesh_model *model);
};

/** @def BLE_MESH_HEALTH_PUB_DEFINE
 *
 *  A helper to define a health publication context
 *
 *  @param _name Name given to the publication context variable.
 *  @param _max_faults Maximum number of faults the element can have.
 */
#define BLE_MESH_HEALTH_PUB_DEFINE(_name, _max_faults) \
    BLE_MESH_MODEL_PUB_DEFINE(_name, NULL, (1 + 3 + (_max_faults)))

struct bt_mesh_health_test {
    uint8_t  id_count;         /* Number of Health self-test ID */
    const uint8_t *test_ids;   /* Array of Health self-test IDs */
    uint16_t company_id;       /* Company ID used to identify the Health Fault state */
    uint8_t  prev_test_id;     /* Most currently performed test id */
    uint8_t  curr_faults[32];  /* Array of current faults */
    uint8_t  reg_faults[32];   /* Array of registered faults */
} __attribute__((packed));

/** Mesh Health Server Model Context */
struct bt_mesh_health_srv {
    struct bt_mesh_model *model;

    /* Optional callback struct */
    struct bt_mesh_health_srv_cb cb;

    /* Attention Timer state */
    struct k_delayed_work attn_timer;

    /* Attention Timer start flag */
    bool attn_timer_start;

    /* Health Server fault test */
    struct bt_mesh_health_test test;
};

extern const struct bt_mesh_model_op bt_mesh_health_srv_op[];
extern const struct bt_mesh_model_cb bt_mesh_health_srv_cb;

/** @def BLE_MESH_MODEL_HEALTH_SRV
 *
 *  Define a new health server model. Note that this API needs to be
 *  repeated for each element which the application wants to have a
 *  health server model on. Each instance also needs a unique
 *  bt_mesh_health_srv and bt_mesh_model_pub context.
 *
 *  @param srv Pointer to a unique struct bt_mesh_health_srv.
 *  @param pub Pointer to a unique struct bt_mesh_model_pub.
 *
 *  @return New mesh model instance.
 */
#define BLE_MESH_MODEL_HEALTH_SRV(srv, pub)             \
        BLE_MESH_MODEL_CB(BLE_MESH_MODEL_ID_HEALTH_SRV,    \
            bt_mesh_health_srv_op, pub, srv, &bt_mesh_health_srv_cb)

int bt_mesh_fault_update(struct bt_mesh_elem *elem);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* __BLE_MESH_HEALTH_SRV_H */
