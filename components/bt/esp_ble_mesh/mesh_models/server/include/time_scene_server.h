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

#ifndef _TIME_SCENE_SERVER_H_
#define _TIME_SCENE_SERVER_H_

#include "mesh_slist.h"
#include "mesh_kernel.h"
#include "server_common.h"

/**
 * 1. Mesh defines times based on International Atomic Time (TAI). The base
 *    representation of times is the number of seconds after 00:00:00 TAI
 *    on 2000-01-01 (that is, 1999-12-31 T23:59:28 UTC).
 * 2. UTC: Coordinated Universal Time. For more information, please refer
 *    to https://time.is/zh/UTC
 * 3. For the algorithm used for the transfer between TAI and UTC, please
 *    refer to Mesh Model Spec Section 5.1.1
 */

#define UNKNOWN_TAI_SECONDS         0x0000000000
#define UNKNOWN_TAI_ZONE_CHANGE     0x0000000000
#define UNKNOWN_TAI_DELTA_CHANGE    0x0000000000
#define TAI_UTC_DELAT_MAX_VALUE     0x7FFF
#define TAI_SECONDS_LEN             0x05
#define TAI_OF_ZONE_CHANGE_LEN      0x05
#define TAI_OF_DELTA_CHANGE_LEN     0x05

#define INVALID_SCENE_NUMBER        0x0000
#define SCENE_NUMBER_LEN            0x02

#define SCHEDULE_YEAR_ANY_YEAR      0x64

#define SCHEDULE_DAY_ANY_DAY        0x00

#define SCHEDULE_HOUR_ANY_HOUR      0x18
#define SCHEDULE_HOUR_ONCE_A_DAY    0x19

#define SCHEDULE_SEC_ANY_OF_HOUR    0x3C
#define SCHEDULE_SEC_EVERY_15_MIN   0x3D
#define SCHEDULE_SEC_EVERY_20_MIN   0x3E
#define SCHEDULE_SEC_ONCE_AN_HOUR   0x3F

#define SCHEDULE_SEC_ANY_OF_MIN     0x3C
#define SCHEDULE_SEC_EVERY_15_SEC   0x3D
#define SCHEDULE_SEC_EVERY_20_SEC   0x3E
#define SCHEDULE_SEC_ONCE_AN_MIN    0x3F

#define SCHEDULE_ACT_TURN_OFF       0x00
#define SCHEDULE_ACT_TURN_ON        0x01
#define SCHEDULE_ACT_SCENE_RECALL   0x02
#define SCHEDULE_ACT_NO_ACTION      0x0F

#define SCHEDULE_SCENE_NO_SCENE     0x0000

#define SCHEDULE_ENTRY_MAX_INDEX    0x0F

#define TIME_NONE                   0x00
#define TIME_AUTHORITY              0x01
#define TIME_RELAY                  0x02
#define TIME_CLINET                 0x03

#define SCENE_SUCCESS               0x00
#define SCENE_REG_FULL              0x01
#define SCENE_NOT_FOUND             0x02

/**
 * The Time state represents the present TAI time, the current TAI-UTC Delta
 * and local time zone offset, and the next change to each of the latter
 * (e.g., because of a switch from winter to summer time or an announced leap
 * second). It consists of 10 fields with a total size of 183 bits.
 */
struct bt_mesh_time_state {
    struct {
        u8_t  tai_seconds[5];
        u8_t  subsecond;
        u8_t  uncertainty;
        u8_t  time_zone_offset_curr;
        u8_t  time_zone_offset_new;
        u8_t  tai_zone_change[5];
        u16_t time_authority : 1,
              tai_utc_delta_curr : 15;
        u16_t tai_utc_delta_new : 15;
        u8_t  tai_delta_change[5];
    } time;
    u8_t time_role;
};

struct bt_mesh_time_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_time_state *state;
};

struct bt_mesh_time_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_time_state *state;
};

struct scene_register {
    u16_t scene_number;
    u8_t  scene_type;   /* Indicate the type of scene value */
    /**
     * Scene value may use a union to represent later, the union contains
     * structures of all the model states which can be stored in a scene.
     */
    struct net_buf_simple *scene_value;
};

/**
 * Scenes serve as memory banks for storage of states (e.g., a power level
 * or a light level/color). Values of states of an element can be stored
 * as a scene and can be recalled later from the scene memory.
 *
 * A scene is represented by a Scene Number, which is a 16-bit non-zero,
 * mesh-wide value. (There can be a maximum of 65535 scenes in a mesh
 * network.) The meaning of a scene, as well as the state storage container
 * associated with it, are determined by a model.
 *
 * The Scenes state change may start numerous parallel model transitions.
 * In that case, each individual model handles the transition internally.
 *
 * The scene transition is defined as a group of individual model transitions
 * started by a Scene Recall operation. The scene transition is in progress
 * when at least one transition from the group of individual model transitions
 * is in progress.
 */
struct bt_mesh_scenes_state {
    const u16_t scene_count;
    struct scene_register *scenes;

    /**
     * The Current Scene state is a 16-bit value that contains either the Scene
     * Number of the currently active scene or a value of 0x0000 when no scene
     * is active.
     *
     * When a Scene Store operation or a Scene Recall operation completes with
     * success, the Current Scene state value shall be to the Scene Number used
     * during that operation.
     *
     * When the Current Scene Number is deleted from a Scene Register state as a
     * result of Scene Delete operation, the Current Scene state shall be set to
     * 0x0000.
     *
     * When any of the element's state that is marked as “Stored with Scene” has
     * changed not as a result of a Scene Recall operation, the value of the
     * Current Scene state shall be set to 0x0000.
     *
     * When a scene transition is in progress, the value of the Current Scene
     * state shall be set to 0x0000.
     */
    u16_t current_scene;

    /**
     * The Target Scene state is a 16-bit value that contains the target Scene
     * Number when a scene transition is in progress.
     *
     * When the scene transition is in progress and the target Scene Number is
     * deleted from a Scene Register state as a result of Scene Delete operation,
     * the Target Scene state shall be set to 0x0000.
     *
     * When the scene transition is in progress and a new Scene Number is stored
     * in the Scene Register as a result of Scene Store operation, the Target
     * Scene state shall be set to the new Scene Number.
     *
     * When the scene transition is not in progress, the value of the Target Scene
     * state shall be set to 0x0000.
     */
    u16_t target_scene;

    /* Indicate the status code for the last operation */
    u8_t status_code;

    /* Indicate if scene transition is in progress */
    bool in_progress;
};

struct bt_mesh_scene_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_scenes_state *state;
    struct bt_mesh_last_msg_info last;
    struct bt_mesh_state_transition transition;
};

struct bt_mesh_scene_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_scenes_state *state;
};

struct schedule_register {
    bool in_use;
    u64_t year : 7,
          month : 12,
          day : 5,
          hour : 5,
          minute : 6,
          second : 6,
          day_of_week : 7,
          action : 4,
          trans_time : 8;
    u16_t scene_number;
};

struct bt_mesh_scheduler_state {
    const u8_t schedule_count;
    struct schedule_register *schedules; /* Up to 16 scheduled entries */

    /**
     * A recommended implementation of the Scheduler should calculate the
     * value of the TAI Seconds of the next scheduled event and put it in
     * a queue of scheduled events sorted by time.
     *
     * Every second, the first event in the queue is compared with the value
     * of the Time state. The first event is executed if it is less than or
     * equal to the Time state and then removed from the queue. After
     * execution, the Repeat Flag shall be checked, and the next occurrence
     * of the scheduled event is calculated and put in the queue.
     *
     * One second timeout value, and compare the first event in queue with the
     * Time state. If it is satisfied, then execute the first event. Also the
     * Repeat Flag need to be checked, if it is set then the event needs to
     * be put into the end of queue.
     *
     * sys_slist_t event_queue;
     *
     * For each event_queue item, it can use the following struct:
     * struct schedule_event {
     *     sys_snode_t node;
     *     u8_t event_index;
     * };
     *
     * Also we need a "struct k_delayed_work track_timer" which can be used to
     * track the schedule timer and handle proper scheduled events.
     */
};

struct bt_mesh_scheduler_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_scheduler_state *state;
};

struct bt_mesh_scheduler_setup_srv {
    struct bt_mesh_model *model;
    struct bt_mesh_server_rsp_ctrl rsp_ctrl;
    struct bt_mesh_scheduler_state *state;
};

typedef union {
    struct {
        u8_t  tai_seconds[5];
        u8_t  subsecond;
        u8_t  uncertainty;
        u16_t time_authority : 1;
        u16_t tai_utc_delta_curr : 15;
        u8_t  time_zone_offset_curr;
    } time_set;
    struct {
        u8_t  tai_seconds[5];
        u8_t  subsecond;
        u8_t  uncertainty;
        u16_t time_authority : 1;
        u16_t tai_utc_delta_curr : 15;
        u8_t  time_zone_offset_curr;
    } time_status;
    struct {
        u8_t time_zone_offset_new;
        u8_t tai_zone_change[5];
    } time_zone_set;
    struct {
        u16_t tai_utc_delta_new : 15;
        u8_t  tai_delta_change[5];
    } tai_utc_delta_set;
    struct {
        u8_t role;
    } time_role_set;
    struct {
        u16_t scene_number;
    } scene_store;
    struct {
        u16_t scene_number;
    } scene_recall;
    struct {
        u16_t scene_number;
    } scene_delete;
    struct {
        u64_t index : 4,
              year : 7,
              month : 12,
              day : 5,
              hour : 5,
              minute : 6,
              second : 6,
              day_of_week : 7,
              action : 4,
              trans_time : 8;
        u16_t scene_number;
    } scheduler_act_set;
} bt_mesh_time_scene_server_state_change_t;

typedef union {
    struct {
        u8_t index;
    } scheduler_act_get;
} bt_mesh_time_scene_server_recv_get_msg_t;

typedef union {
    struct {
        u8_t  tai_seconds[5];
        u8_t  subsecond;
        u8_t  uncertainty;
        u16_t time_authority : 1;
        u16_t tai_utc_delta : 15;
        u8_t  time_zone_offset;
    } time_set;
    struct {
        u8_t time_zone_offset_new;
        u8_t tai_zone_change[5];
    } time_zone_set;
    struct {
        u16_t tai_utc_delta_new : 15;
        u16_t padding : 1;
        u8_t tai_delta_change[5];
    } tai_utc_delta_set;
    struct {
        u8_t time_role;
    } time_role_set;
    struct {
        u16_t scene_number;
    } scene_store;
    struct {
        bool  op_en;
        u16_t scene_number;
        u8_t  tid;
        u8_t  trans_time;
        u8_t  delay;
    } scene_recall;
    struct {
        u16_t scene_number;
    } scene_delete;
    struct {
        u64_t index : 4,
              year : 7,
              month : 12,
              day : 5,
              hour : 5,
              minute : 6,
              second : 6,
              day_of_week : 7,
              action : 4,
              trans_time : 8;
        u16_t scene_number;
    } scheduler_act_set;
} bt_mesh_time_scene_server_recv_set_msg_t;

typedef union {
    struct {
        u8_t  tai_seconds[5];
        u8_t  subsecond;
        u8_t  uncertainty;
        u16_t time_authority : 1;
        u16_t tai_utc_delta : 15;
        u8_t  time_zone_offset;
    } time_status;
} bt_mesh_time_scene_server_recv_status_msg_t;

void bt_mesh_time_scene_server_lock(void);
void bt_mesh_time_scene_server_unlock(void);

void scene_publish(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, u16_t opcode);

int bt_mesh_time_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_time_setup_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_scene_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_scene_setup_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_scheduler_srv_init(struct bt_mesh_model *model, bool primary);
int bt_mesh_scheduler_setup_srv_init(struct bt_mesh_model *model, bool primary);

#endif /* _TIME_SCENE_SERVER_H_ */
