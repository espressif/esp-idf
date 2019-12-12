// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _TIME_SCENE_CLIENT_H_
#define _TIME_SCENE_CLIENT_H_

#include "mesh_access.h"
#include "mesh_kernel.h"

#include "client_common.h"

/* Time scene client model common structure */
typedef bt_mesh_client_user_data_t      bt_mesh_time_scene_client_t;
typedef bt_mesh_client_internal_data_t  time_scene_internal_data_t;

/* Time Client Model Context */
extern const struct bt_mesh_model_op time_cli_op[];

/** @def BLE_MESH_MODEL_TIME_CLI
 *
 *  Define a new time client model. Note that this API needs to
 *  be repeated for each element which the application wants to
 *  have a time model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_time_cli.
 *
 *  @return New time client model instance.
 */
#define BLE_MESH_MODEL_TIME_CLI(cli_pub, cli_data)      \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_TIME_CLI,      \
                    time_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t  bt_mesh_time_client_t;

struct bt_mesh_time_status {
    u8_t  tai_seconds[5];     /* The current TAI time in seconds */
    u8_t  sub_second;         /* The sub-second time in units of 1/256 second */
    u8_t  uncertainty;        /* The estimated uncertainty in 10-millisecond steps */
    u16_t time_authority : 1; /* 0 = No Time Authority, 1 = Time Authority */
    u16_t tai_utc_delta : 15; /* Current difference between TAI and UTC in seconds */
    u8_t  time_zone_offset;   /* The local time zone offset in 15-minute increments */
};

struct bt_mesh_time_zone_status {
    u8_t time_zone_offset_curr; /* Current local time zone offset */
    u8_t time_zone_offset_new;  /* Upcoming local time zone offset */
    u8_t tai_zone_change[5];    /* TAI Seconds time of the upcoming Time Zone Offset change */
};

struct bt_mesh_tai_utc_delta_status {
    u16_t tai_utc_delta_curr : 15; /* Current difference between TAI and UTC in seconds */
    u16_t padding_1 : 1;           /* Always 0b0. Other values are Prohibited. */
    u16_t tai_utc_delta_new : 15;  /* Upcoming difference between TAI and UTC in seconds */
    u16_t padding_2 : 1;           /* Always 0b0. Other values are Prohibited. */
    u8_t tai_delta_change[5];      /* TAI Seconds time of the upcoming TAI-UTC Delta change */
};

struct bt_mesh_time_role_status {
    u8_t time_role; /* The Time Role for the element */
};

struct bt_mesh_time_set {
    u8_t  tai_seconds[5];     /* The current TAI time in seconds */
    u8_t  sub_second;         /* The sub-second time in units of 1/256 second */
    u8_t  uncertainty;        /* The estimated uncertainty in 10-millisecond steps */
    u16_t time_authority : 1; /* 0 = No Time Authority, 1 = Time Authority */
    u16_t tai_utc_delta : 15; /* Current difference between TAI and UTC in seconds */
    u8_t  time_zone_offset;   /* The local time zone offset in 15-minute increments */
};

struct bt_mesh_time_zone_set {
    u8_t time_zone_offset_new; /* Upcoming local time zone offset */
    u8_t tai_zone_change[5];   /* TAI Seconds time of the upcoming Time Zone Offset change */
};

struct bt_mesh_tai_utc_delta_set {
    u16_t tai_utc_delta_new : 15; /* Upcoming difference between TAI and UTC in seconds */
    u16_t padding : 1;            /* Always 0b0. Other values are Prohibited. */
    u8_t tai_delta_change[5];     /* TAI Seconds time of the upcoming TAI-UTC Delta change */
};

struct bt_mesh_time_role_set {
    u8_t time_role; /* The Time Role for the element */
};

/* Scene Client Model Context */
extern const struct bt_mesh_model_op scene_cli_op[];

/** @def BLE_MESH_MODEL_SCENE_CLI
 *
 *  Define a new scene client model. Note that this API needs to
 *  be repeated for each element which the application wants to
 *  have a scene model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_scene_cli.
 *
 *  @return New scene client model instance.
 */
#define BLE_MESH_MODEL_SCENE_CLI(cli_pub, cli_data)     \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_SCENE_CLI,     \
                    scene_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t  bt_mesh_scene_client_t;

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

/* Scheduler Client Model Context */
extern const struct bt_mesh_model_op scheduler_cli_op[];

/** @def BLE_MESH_MODEL_SCHEDULER_CLI
 *
 *  Define a new scheduler client model. Note that this API needs to
 *  be repeated for each element which the application wants to
 *  have a scheduler model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_scheduler_cli.
 *
 *  @return New scheduler client model instance.
 */
#define BLE_MESH_MODEL_SCHEDULER_CLI(cli_pub, cli_data)     \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_SCHEDULER_CLI,     \
                    scheduler_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t  bt_mesh_scheduler_client_t;

struct bt_mesh_scheduler_status {
    u16_t schedules; /* Bit field indicating defined Actions in the Schedule Register */
};

struct bt_mesh_scheduler_act_status {
    u64_t index : 4;        /* Enumerates (selects) a Schedule Register entry */
    u64_t year : 7;         /* Scheduled year for the action */
    u64_t month : 12;       /* Scheduled month for the action */
    u64_t day : 5;          /* Scheduled day of the month for the action */
    u64_t hour : 5;         /* Scheduled hour for the action */
    u64_t minute : 6;       /* Scheduled minute for the action */
    u64_t second : 6;       /* Scheduled second for the action */
    u64_t day_of_week : 7;  /* Schedule days of the week for the action */
    u64_t action : 4;       /* Action to be performed at the scheduled time */
    u64_t trans_time : 8;   /* Transition time for this action */
    u16_t scene_number;     /* Transition time for this action */
};

struct bt_mesh_scheduler_act_get {
    u8_t index; /* Index of the Schedule Register entry to get */
};

struct bt_mesh_scheduler_act_set {
    u64_t index : 4;        /* Index of the Schedule Register entry to set */
    u64_t year : 7;         /* Scheduled year for the action */
    u64_t month : 12;       /* Scheduled month for the action */
    u64_t day : 5;          /* Scheduled day of the month for the action */
    u64_t hour : 5;         /* Scheduled hour for the action */
    u64_t minute : 6;       /* Scheduled minute for the action */
    u64_t second : 6;       /* Scheduled second for the action */
    u64_t day_of_week : 7;  /* Schedule days of the week for the action */
    u64_t action : 4;       /* Action to be performed at the scheduled time */
    u64_t trans_time : 8;   /* Transition time for this action */
    u16_t scene_number;     /* Transition time for this action */
};

/**
 * @brief This function is called to initialize time client model user_data.
 *
 * @param[in] model:   Pointer to time client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_time_cli_init(struct bt_mesh_model *model, bool primary);

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
 * @brief This function is called to initialize scheduler client model user_data.
 *
 * @param[in] model:   Pointer to scheduler client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_scheduler_cli_init(struct bt_mesh_model *model, bool primary);

/**
 * @brief This function is called to get scene states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  get:    Pointer of time scene get message value
 * @param[out] status: Pointer of time scene status message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_time_scene_client_get_state(bt_mesh_client_common_param_t *common, void *get, void *status);

/**
 * @brief This function is called to set scene states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  set:    Pointer of time scene set message value
 * @param[out] status: Pointer of time scene status message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_time_scene_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *status);

#endif /* _TIME_SCENE_CLIENT_H_ */
