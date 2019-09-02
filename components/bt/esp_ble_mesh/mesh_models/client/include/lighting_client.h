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
 *  @brief Bluetooth Mesh Lighting Client Model APIs.
 */

#ifndef _LIGHTING_CLIENT_H_
#define _LIGHTING_CLIENT_H_

#include "mesh_access.h"
#include "mesh_kernel.h"

#include "client_common.h"

/* Light client model common structure */
typedef bt_mesh_client_user_data_t      bt_mesh_light_client_t;
typedef bt_mesh_client_internal_data_t  light_internal_data_t;

/* Light Lightness Client Model Context */
extern const struct bt_mesh_model_op light_lightness_cli_op[];

/** @def BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI
 *
 *  Define a new light lightness client model. Note that this API
 *  needs to be repeated for each element which the application
 *  wants to have a light lightness client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_light_lightness_cli.
 *
 *  @return New light lightness client model instance.
 */
#define BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI(cli_pub, cli_data)   \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,   \
                    light_lightness_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t  bt_mesh_light_lightness_client_t;

struct bt_mesh_light_lightness_status {
    bool  op_en;             /* Indicate whether optional parameters included           */
    u16_t present_lightness; /* Present value of light lightness actual state           */
    u16_t target_lightness;  /* Target value of light lightness actual state (optional) */
    u8_t  remain_time;       /* Time to complete state transition (C.1)                 */
};

struct bt_mesh_light_lightness_linear_status {
    bool  op_en;             /* Indicate whether optional parameters included           */
    u16_t present_lightness; /* Present value of light lightness linear state           */
    u16_t target_lightness;  /* Target value of light lightness linear state (optional) */
    u8_t  remain_time;       /* Time to complete state transition (C.1)                 */
};

struct bt_mesh_light_lightness_last_status {
    u16_t lightness; /* The value of the Light Lightness Last state */
};

struct bt_mesh_light_lightness_default_status {
    u16_t lightness; /* The value of the Light Lightness default state */
};

struct bt_mesh_light_lightness_range_status {
    u8_t  status_code; /* Status Code for the requesting message                  */
    u16_t range_min;   /* Value of range min field of light lightness range state */
    u16_t range_max;   /* Value of range max field of light lightness range state */
};

struct bt_mesh_light_lightness_set {
    bool  op_en;      /* Indicate whether optional parameters included */
    u16_t lightness;  /* Target value of light lightness actual state  */
    u8_t  tid;        /* Transaction Identifier                        */
    u8_t  trans_time; /* Time to complete state transition (optional)  */
    u8_t  delay;      /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_light_lightness_linear_set {
    bool  op_en;      /* Indicate whether optional parameters included */
    u16_t lightness;  /* Target value of light lightness linear state  */
    u8_t  tid;        /* Transaction Identifier                        */
    u8_t  trans_time; /* Time to complete state transition (optional)  */
    u8_t  delay;      /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_light_lightness_default_set {
    u16_t lightness;  /* The value of the Light Lightness Default state */
};

struct bt_mesh_light_lightness_range_set {
    u16_t range_min; /* Value of range min field of light lightness range state */
    u16_t range_max; /* Value of range max field of light lightness range state */
};

/* Light CTL Client Model Context */
extern const struct bt_mesh_model_op light_ctl_cli_op[];

/** @def BLE_MESH_MODEL_LIGHT_CTL_CLI
 *
 *  Define a new light CTL client model. Note that this API needs
 *  to be repeated for each element which the application wants to
 *  have a light CTL client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_light_ctl_cli.
 *
 *  @return New light CTL client model instance.
 */
#define BLE_MESH_MODEL_LIGHT_CTL_CLI(cli_pub, cli_data)     \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,     \
                    light_ctl_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t  bt_mesh_light_ctl_client_t;

struct bt_mesh_light_ctl_status {
    bool  op_en;                   /* Indicate whether optional parameters included        */
    u16_t present_ctl_lightness;   /* Present value of light ctl lightness state           */
    u16_t present_ctl_temperature; /* Present value of light ctl temperature state         */
    u16_t target_ctl_lightness;    /* Target value of light ctl lightness state (optional) */
    u16_t target_ctl_temperature;  /* Target value of light ctl temperature state (C.1)    */
    u8_t  remain_time;             /* Time to complete state transition (C.1)              */
};

struct bt_mesh_light_ctl_temperature_status {
    bool  op_en;                   /* Indicate whether optional parameters included          */
    u16_t present_ctl_temperature; /* Present value of light ctl temperature state           */
    u16_t present_ctl_delta_uv;    /* Present value of light ctl delta UV state              */
    u16_t target_ctl_temperature;  /* Target value of light ctl temperature state (optional) */
    u16_t target_ctl_delta_uv;     /* Target value of light ctl delta UV state (C.1)         */
    u8_t  remain_time;             /* Time to complete state transition (C.1)                */
};

struct bt_mesh_light_ctl_temperature_range_status {
    u8_t  status_code; /* Status code for the requesting message                                    */
    u16_t range_min;   /* Value of temperature range min field of light ctl temperature range state */
    u16_t range_max;   /* Value of temperature range max field of light ctl temperature range state */
};

struct bt_mesh_light_ctl_default_status {
    u16_t lightness;   /* Value of light lightness default state   */
    u16_t temperature; /* Value of light temperature default state */
    s16_t delta_uv;    /* Value of light delta UV default state    */
};

struct bt_mesh_light_ctl_set {
    bool  op_en;           /* Indicate whether optional parameters included */
    u16_t ctl_lightness;   /* Target value of light ctl lightness state     */
    u16_t ctl_temperature; /* Target value of light ctl temperature state   */
    s16_t ctl_delta_uv;    /* Target value of light ctl delta UV state      */
    u8_t  tid;             /* Transaction Identifier                        */
    u8_t  trans_time;      /* Time to complete state transition (optional)  */
    u8_t  delay;           /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_light_ctl_temperature_set {
    bool  op_en;           /* Indicate whether optional parameters included */
    u16_t ctl_temperature; /* Target value of light ctl temperature state   */
    s16_t ctl_delta_uv;    /* Target value of light ctl delta UV state      */
    u8_t  tid;             /* Transaction Identifier                        */
    u8_t  trans_time;      /* Time to complete state transition (optional)  */
    u8_t  delay;           /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_light_ctl_temperature_range_set {
    u16_t range_min; /* Value of temperature range min field of light ctl temperature range state */
    u16_t range_max; /* Value of temperature range max field of light ctl temperature range state */
};

struct bt_mesh_light_ctl_default_set {
    u16_t lightness;   /* Value of light lightness default state   */
    u16_t temperature; /* Value of light temperature default state */
    s16_t delta_uv;    /* Value of light delta UV default state    */
};

/* Light HSL Client Model Context */
extern const struct bt_mesh_model_op light_hsl_cli_op[];

/** @def BLE_MESH_MODEL_LIGHT_HSL_CLI
 *
 *  Define a new light HSL client model. Note that this API needs
 *  to be repeated for each element which the application wants to
 *  have a light HSL client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_light_hsl_cli.
 *
 *  @return New light HSL client model instance.
 */
#define BLE_MESH_MODEL_LIGHT_HSL_CLI(cli_pub, cli_data)     \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,     \
                    light_hsl_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t  bt_mesh_light_hsl_client_t;

struct bt_mesh_light_hsl_status {
    bool  op_en;          /* Indicate whether optional parameters included */
    u16_t hsl_lightness;  /* Present value of light hsl lightness state    */
    u16_t hsl_hue;        /* Present value of light hsl hue state          */
    u16_t hsl_saturation; /* Present value of light hsl saturation state   */
    u8_t  remain_time;    /* Time to complete state transition (optional)  */
};

struct bt_mesh_light_hsl_target_status {
    bool  op_en;                 /* Indicate whether optional parameters included */
    u16_t hsl_lightness_target;  /* Target value of light hsl lightness state     */
    u16_t hsl_hue_target;        /* Target value of light hsl hue state           */
    u16_t hsl_saturation_target; /* Target value of light hsl saturation state    */
    u8_t  remain_time;           /* Time to complete state transition (optional)  */
};

struct bt_mesh_light_hsl_hue_status {
    bool  op_en;       /* Indicate whether optional parameters included  */
    u16_t present_hue; /* Present value of light hsl hue state           */
    u16_t target_hue;  /* Target value of light hsl hue state (optional) */
    u8_t  remain_time; /* Time to complete state transition (C.1)   */
};

struct bt_mesh_light_hsl_saturation_status {
    bool  op_en;              /* Indicate whether optional parameters included         */
    u16_t present_saturation; /* Present value of light hsl saturation state           */
    u16_t target_saturation;  /* Target value of light hsl saturation state (optional) */
    u8_t  remain_time;        /* Time to complete state transition (C.1)               */
};

struct bt_mesh_light_hsl_default_status {
    u16_t lightness;  /* Value of light lightness default state  */
    u16_t hue;        /* Value of light hue default state        */
    u16_t saturation; /* Value of light saturation default state */
};

struct bt_mesh_light_hsl_range_status {
    u8_t  status_code;          /* Status code for the requesting message                                  */
    u16_t hue_range_min;        /* Value of hue range min field of light hsl hue range state               */
    u16_t hue_range_max;        /* Value of hue range max field of light hsl hue range state               */
    u16_t saturation_range_min; /* Value of saturation range min field of light hsl saturation range state */
    u16_t saturation_range_max; /* Value of saturation range max field of light hsl saturation range state */
};

struct bt_mesh_light_hsl_set {
    bool  op_en;          /* Indicate whether optional parameters included */
    u16_t hsl_lightness;  /* Target value of light hsl lightness state     */
    u16_t hsl_hue;        /* Target value of light hsl hue state           */
    u16_t hsl_saturation; /* Target value of light hsl saturation state    */
    u8_t  tid;            /* Transaction Identifier                        */
    u8_t  trans_time;     /* Time to complete state transition (optional)  */
    u8_t  delay;          /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_light_hsl_hue_set {
    bool  op_en;      /* Indicate whether optional parameters included */
    u16_t hue;        /* Target value of light hsl hue state           */
    u8_t  tid;        /* Transaction Identifier                        */
    u8_t  trans_time; /* Time to complete state transition (optional)  */
    u8_t  delay;      /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_light_hsl_saturation_set {
    bool  op_en;      /* Indicate whether optional parameters included */
    u16_t saturation; /* Target value of light hsl hue state           */
    u8_t  tid;        /* Transaction Identifier                        */
    u8_t  trans_time; /* Time to complete state transition (optional)  */
    u8_t  delay;      /* Indicate message execution delay (C.1)        */
};

struct bt_mesh_light_hsl_default_set {
    u16_t lightness;  /* Value of light lightness default state  */
    u16_t hue;        /* Value of light hue default state        */
    u16_t saturation; /* Value of light saturation default state */
};

struct bt_mesh_light_hsl_range_set {
    u16_t hue_range_min;        /* Value of hue range min field of light hsl hue range state               */
    u16_t hue_range_max;        /* Value of hue range max field of light hsl hue range state               */
    u16_t saturation_range_min; /* Value of saturation range min field of light hsl saturation range state */
    u16_t saturation_range_max; /* Value of saturation range max field of light hsl saturation range state */
};

/* Light xyL Client Model Context */
extern const struct bt_mesh_model_op light_xyl_cli_op[];

/** @def BLE_MESH_MODEL_LIGHT_XYL_CLI
 *
 *  Define a new light xyL client model. Note that this API needs
 *  to be repeated for each element which the application wants
 *  to have a light xyL client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_light_xyl_cli.
 *
 *  @return New light xyL client model instance.
 */
#define BLE_MESH_MODEL_LIGHT_XYL_CLI(cli_pub, cli_data)     \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,     \
                    light_xyl_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t  bt_mesh_light_xyl_client_t;

struct bt_mesh_light_xyl_status {
    bool  op_en;         /* Indicate whether optional parameters included      */
    u16_t xyl_lightness; /* The present value of the Light xyL Lightness state */
    u16_t xyl_x;         /* The present value of the Light xyL x state         */
    u16_t xyl_y;         /* The present value of the Light xyL y state         */
    u8_t  remain_time;   /* Time to complete state transition (optional)       */
};

struct bt_mesh_light_xyl_target_status {
    bool  op_en;                /* Indicate whether optional parameters included     */
    u16_t target_xyl_lightness; /* The target value of the Light xyL Lightness state */
    u16_t target_xyl_x;         /* The target value of the Light xyL x state         */
    u16_t target_xyl_y;         /* The target value of the Light xyL y state         */
    u8_t  remain_time;          /* Time to complete state transition (optional)      */
};

struct bt_mesh_light_xyl_default_status {
    u16_t lightness; /* The value of the Light Lightness Default state */
    u16_t xyl_x;     /* The value of the Light xyL x Default state     */
    u16_t xyl_y;     /* The value of the Light xyL y Default state     */
};

struct bt_mesh_light_xyl_range_status {
    u8_t  status_code;     /* Status Code for the requesting message */
    u16_t xyl_x_range_min; /* The value of the xyL x Range Min field of the Light xyL x Range state */
    u16_t xyl_x_range_max; /* The value of the xyL x Range Max field of the Light xyL x Range state */
    u16_t xyl_y_range_min; /* The value of the xyL y Range Min field of the Light xyL y Range state */
    u16_t xyl_y_range_max; /* The value of the xyL y Range Max field of the Light xyL y Range state */
};

struct bt_mesh_light_xyl_set {
    bool  op_en;         /* Indicate whether optional parameters included     */
    u16_t xyl_lightness; /* The target value of the Light xyL Lightness state */
    u16_t xyl_x;         /* The target value of the Light xyL x state         */
    u16_t xyl_y;         /* The target value of the Light xyL y state         */
    u8_t  tid;           /* Transaction Identifier                            */
    u8_t  trans_time;    /* Time to complete state transition (optional)      */
    u8_t  delay;         /* Indicate message execution delay (C.1)            */
};

struct bt_mesh_light_xyl_default_set {
    u16_t lightness; /* The value of the Light Lightness Default state */
    u16_t xyl_x;     /* The value of the Light xyL x Default state     */
    u16_t xyl_y;     /* The value of the Light xyL y Default state     */
};

struct bt_mesh_light_xyl_range_set {
    u16_t xyl_x_range_min; /* The value of the xyL x Range Min field of the Light xyL x Range state */
    u16_t xyl_x_range_max; /* The value of the xyL x Range Max field of the Light xyL x Range state */
    u16_t xyl_y_range_min; /* The value of the xyL y Range Min field of the Light xyL y Range state */
    u16_t xyl_y_range_max; /* The value of the xyL y Range Max field of the Light xyL y Range state */
};

/* Light LC Client Model Context */
extern const struct bt_mesh_model_op light_lc_cli_op[];

/** @def BLE_MESH_MODEL_LIGHT_LC_CLI
 *
 *  Define a new light lc client model. Note that this API needs
 *  to be repeated for each element which the application wants
 *  to have a light lc client model on.
 *  @param cli_pub  Pointer to a unique struct bt_mesh_model_pub.
 *  @param cli_data Pointer to a unique struct bt_mesh_light_lc_cli.
 *
 *  @return New light lc client model instance.
 */
#define BLE_MESH_MODEL_LIGHT_LC_CLI(cli_pub, cli_data)      \
        BLE_MESH_MODEL(BLE_MESH_MODEL_ID_LIGHT_LC_CLI,      \
                    light_lc_cli_op, cli_pub, cli_data)

typedef bt_mesh_client_user_data_t  bt_mesh_light_lc_client_t;

struct bt_mesh_light_lc_mode_status {
    u8_t mode; /* The present value of the Light LC Mode state */
};

struct bt_mesh_light_lc_om_status {
    u8_t mode; /* The present value of the Light LC Occupancy Mode state */
};

struct bt_mesh_light_lc_light_onoff_status {
    bool op_en;               /* Indicate whether optional parameters included                 */
    u8_t present_light_onoff; /* The present value of the Light LC Light OnOff state           */
    u8_t target_light_onoff;  /* The target value of the Light LC Light OnOff state (Optional) */
    u8_t remain_time;         /* Time to complete state transition (C.1)                       */
};

struct bt_mesh_light_lc_property_status {
    u16_t light_lc_property_id;  /* Property ID identifying a Light LC Property */
    struct net_buf_simple *light_lc_property_value; /* Raw value for the Light LC Property */
};

struct bt_mesh_light_lc_mode_set {
    u8_t mode; /* The target value of the Light LC Mode state */
};

struct bt_mesh_light_lc_om_set {
    u8_t mode; /* The target value of the Light LC Occupancy Mode state */
};

struct bt_mesh_light_lc_light_onoff_set {
    bool op_en;       /* Indicate whether optional parameters included      */
    u8_t light_onoff; /* The target value of the Light LC Light OnOff state */
    u8_t tid;         /* Transaction Identifier                             */
    u8_t trans_time;  /* Time to complete state transition (optional)       */
    u8_t delay;       /* Indicate message execution delay (C.1)             */
};

struct bt_mesh_light_lc_property_get {
    u16_t light_lc_property_id;  /* Property ID identifying a Light LC Property */
};

struct bt_mesh_light_lc_property_set {
    u16_t light_lc_property_id;  /* Property ID identifying a Light LC Property */
    struct net_buf_simple *light_lc_property_value; /* Raw value for the Light LC Property */
};

/**
 * @brief This function is called to initialize light lightness client model user_data.
 *
 * @param[in] model:   Pointer to light lightness client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_light_lightness_cli_init(struct bt_mesh_model *model, bool primary);

/**
 * @brief This function is called to initialize light ctl client model user_data.
 *
 * @param[in] model:   Pointer to light ctl client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_light_ctl_cli_init(struct bt_mesh_model *model, bool primary);

/**
 * @brief This function is called to initialize light hsl client model user_data.
 *
 * @param[in] model:   Pointer to light hsl client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_light_hsl_cli_init(struct bt_mesh_model *model, bool primary);

/**
 * @brief This function is called to initialize light xyl client model user_data.
 *
 * @param[in] model:   Pointer to light xyl client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_light_xyl_cli_init(struct bt_mesh_model *model, bool primary);

/**
 * @brief This function is called to initialize light lc client model user_data.
 *
 * @param[in] model:   Pointer to light lc client model
 * @param[in] primary: Whether belongs to primary element
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_light_lc_cli_init(struct bt_mesh_model *model, bool primary);

/**
 * @brief This function is called to get light states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  get:    Pointer of light get message value
 * @param[out] status: Pointer of light status message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_light_client_get_state(bt_mesh_client_common_param_t *common, void *get, void *status);

/**
 * @brief This function is called to set light states.
 *
 * @param[in]  common: Message common information structure
 * @param[in]  set:    Pointer of light set message value
 * @param[out] status: Pointer of light status message value
 *
 * @return Zero-success, other-fail
 */
int bt_mesh_light_client_set_state(bt_mesh_client_common_param_t *common, void *set, void *status);

#endif /* _LIGHTING_CLIENT_H_ */
