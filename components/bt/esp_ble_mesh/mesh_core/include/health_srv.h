/** @file
 *  @brief Bluetooth Mesh Health Server Model APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_HEALTH_SRV_H_
#define _BLE_MESH_HEALTH_SRV_H_

#include "mesh_access.h"
#include "mesh_kernel.h"

/**
 * @brief Bluetooth Mesh Health Server Model
 * @defgroup bt_mesh_health_srv Bluetooth Mesh Health Server Model
 * @ingroup bt_mesh
 * @{
 */

struct bt_mesh_health_srv_cb {
    /* Fetch current faults */
    int (*fault_get_cur)(struct bt_mesh_model *model, u8_t *test_id,
                         u16_t *company_id, u8_t *faults,
                         u8_t *fault_count);

    /* Fetch registered faults */
    int (*fault_get_reg)(struct bt_mesh_model *model, u16_t company_id,
                         u8_t *test_id, u8_t *faults,
                         u8_t *fault_count);

    /* Clear registered faults */
    int (*fault_clear)(struct bt_mesh_model *model, u16_t company_id);

    /* Run a specific test */
    int (*fault_test)(struct bt_mesh_model *model, u8_t test_id,
                      u16_t company_id);

    /* Attention on */
    void (*attn_on)(struct bt_mesh_model *model);

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

/** Mesh Health Server Model Context */
struct bt_mesh_health_srv {
    struct bt_mesh_model *model;

    /* Optional callback struct */
    const struct bt_mesh_health_srv_cb *cb;

    /* Attention Timer state */
    struct k_delayed_work attn_timer;
};

extern const struct bt_mesh_model_op bt_mesh_health_srv_op[];

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
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_HEALTH_SRV,    \
            bt_mesh_health_srv_op, pub, srv)

int bt_mesh_fault_update(struct bt_mesh_elem *elem);

/**
 * @}
 */

#endif /* __BLE_MESH_HEALTH_SRV_H */
