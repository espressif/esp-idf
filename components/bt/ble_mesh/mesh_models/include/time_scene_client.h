// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** @file
 *  @brief Bluetooth Mesh Time and Scene Client Model APIs.
 */

#ifndef __BT_MESH_TIME_SCENE_CLI_H
#define __BT_MESH_TIME_SCENE_CLI_H

#include "mesh_access.h"
#include "mesh_kernel.h"
#include "common.h"
#include "bt_mesh_client_common.h"

/* Time scene client model common structure */
typedef bt_mesh_client_common_t bt_mesh_time_scene_client_t;
typedef bt_mesh_internal_data_t time_scene_internal_data_t;

/* Time Client Model Context */

/* Scene Client Model Context */
extern const struct bt_mesh_model_op scene_cli_op[];

/** @def BT_MESH_MODEL_SCENE_CLI
 *
 *  Define a new scene client model. Note that this API needs to
 *  be repeated for each element which the application wants to
 *  have a sensor scene model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_scene_cli.
 *
 *  @return New scene client model instance.
 */
#define BT_MESH_MODEL_SCENE_CLI(cli_pub, cli_data) \
        BT_MESH_MODEL(BT_MESH_MODEL_ID_SCENE_CLI,  \
                    scene_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_common_t bt_mesh_scene_cli_t;

struct bt_mesh_scene_status {
    bool  op_en;         /* Indicate whether optional parameters included */
    u8_t  status_code;   /* Status code for the last operation            */
    u16_t current_scene; /* Scene Number of a current scene               */
    u16_t target_scene;  /* Scene Number of a target scene (optional)     */
    u8_t  remain_time;   /* Time to complete state transition (C.1)       */
};

struct bt_mesh_scene_register_status {
    u8_t  status_code;   /* Status code for the previous operation              */
    u16_t current_scene; /* Scene Number of a current scene                     */
    struct net_buf_simple *scenes; /* A list of scenes stored within an element */
};

struct bt_mesh_scene_store {
    u16_t scene_number; /* The number of the scene to be stored */
};

struct bt_mesh_scene_recall {
    bool  op_en;        /* Indicate whether optional parameters included */
    u16_t scene_number; /* The number of the scene to be recalled        */
    u8_t  tid;          /* Transaction Identifier                        */
    u8_t  trans_time;   /* Time to complete state transition (optional)  */
    u8_t  delay;        /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_scene_delete {
    u16_t scene_number; /* The number of the scene to be deleted */
};

/**
 * @brief This function is called to initialize scene client model user_data.
 *
 * @param[in] model:   Pointer to scene client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_scene_cli_init(struct bt_mesh_model *model, bool primary);

/**
 * @brief This function is called to get scene states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  get:    Pointer of time scene get message value
 * @param[out] status: Pointer of time scene status message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_time_scene_client_get_state(struct bt_mesh_common_param *common, void *get, void *status);

/**
 * @brief This function is called to set scene states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  set:    Pointer of time scene set message value
 * @param[out] status: Pointer of time scene status message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_time_scene_client_set_state(struct bt_mesh_common_param *common, void *set, void *status);

#endif /* __BT_MESH_TIME_SCENE_CLI_H */
