/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @file
 *  @brief Bluetooth Mesh Light Client Model APIs.
 */

#ifndef _ESP_BLE_MESH_LIGHTING_MODEL_API_H_
#define _ESP_BLE_MESH_LIGHTING_MODEL_API_H_

#include "esp_ble_mesh_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @def    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI
 *
 *  @brief  Define a new Light Lightness Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Light Lightness Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Light Lightness Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_CLI(cli_pub, cli_data)           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_CLI,   \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_CTL_CLI
 *
 *  @brief  Define a new Light CTL Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Light CTL Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Light CTL Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_CTL_CLI(cli_pub, cli_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_CLI,     \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_HSL_CLI
 *
 *  @brief  Define a new Light HSL Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Light HSL Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Light HSL Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_HSL_CLI(cli_pub, cli_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_CLI,     \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_XYL_CLI
 *
 *  @brief  Define a new Light xyL Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Light xyL Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Light xyL Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_XYL_CLI(cli_pub, cli_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_CLI,     \
                    NULL, cli_pub, cli_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_LC_CLI
 *
 *  @brief  Define a new Light LC Client Model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a Light LC Client Model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New Light LC Client Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_LC_CLI(cli_pub, cli_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_LC_CLI,     \
                    NULL, cli_pub, cli_data)

/**
 *  @brief Bluetooth Mesh Light Lightness Client Model Get and Set parameters structure.
 */

/** Parameters of Light Lightness Set */
typedef struct {
    bool     op_en;        /*!< Indicate if optional parameters are included */
    uint16_t lightness;    /*!< Target value of light lightness actual state */
    uint8_t  tid;          /*!< Transaction ID */
    uint8_t  trans_time;   /*!< Time to complete state transition (optional) */
    uint8_t  delay;        /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_lightness_set_t;

/** Parameters of Light Lightness Linear Set */
typedef struct {
    bool     op_en;        /*!< Indicate if optional parameters are included */
    uint16_t lightness;    /*!< Target value of light lightness linear state */
    uint8_t  tid;          /*!< Transaction ID */
    uint8_t  trans_time;   /*!< Time to complete state transition (optional) */
    uint8_t  delay;        /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_lightness_linear_set_t;

/** Parameter of Light Lightness Default Set */
typedef struct {
    uint16_t lightness;    /*!< The value of the Light Lightness Default state */
} esp_ble_mesh_light_lightness_default_set_t;

/** Parameters of Light Lightness Range Set */
typedef struct {
    uint16_t range_min;    /*!< Value of range min field of light lightness range state */
    uint16_t range_max;    /*!< Value of range max field of light lightness range state */
} esp_ble_mesh_light_lightness_range_set_t;

/** Parameters of Light CTL Set */
typedef struct {
    bool     op_en;            /*!< Indicate if optional parameters are included */
    uint16_t ctl_lightness;    /*!< Target value of light ctl lightness state */
    uint16_t ctl_temperatrue;  /*!< Target value of light ctl temperature state */
    int16_t  ctl_delta_uv;     /*!< Target value of light ctl delta UV state */
    uint8_t  tid;              /*!< Transaction ID */
    uint8_t  trans_time;       /*!< Time to complete state transition (optional) */
    uint8_t  delay;            /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_ctl_set_t;

/** Parameters of Light CTL Temperature Set */
typedef struct {
    bool     op_en;            /*!< Indicate if optional parameters are included */
    uint16_t ctl_temperatrue;  /*!< Target value of light ctl temperature state */
    int16_t  ctl_delta_uv;     /*!< Target value of light ctl delta UV state */
    uint8_t  tid;              /*!< Transaction ID */
    uint8_t  trans_time;       /*!< Time to complete state transition (optional) */
    uint8_t  delay;            /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_ctl_temperature_set_t;

/** Parameters of Light CTL Temperature Range Set */
typedef struct {
    uint16_t range_min;        /*!< Value of temperature range min field of light ctl temperature range state */
    uint16_t range_max;        /*!< Value of temperature range max field of light ctl temperature range state */
} esp_ble_mesh_light_ctl_temperature_range_set_t;

/** Parameters of Light CTL Default Set */
typedef struct {
    uint16_t lightness;        /*!< Value of light lightness default state */
    uint16_t temperature;      /*!< Value of light temperature default state */
    int16_t  delta_uv;         /*!< Value of light delta UV default state */
} esp_ble_mesh_light_ctl_default_set_t;

/** Parameters of Light HSL Set */
typedef struct {
    bool     op_en;            /*!< Indicate if optional parameters are included */
    uint16_t hsl_lightness;    /*!< Target value of light hsl lightness state */
    uint16_t hsl_hue;          /*!< Target value of light hsl hue state */
    uint16_t hsl_saturation;   /*!< Target value of light hsl saturation state */
    uint8_t  tid;              /*!< Transaction ID */
    uint8_t  trans_time;       /*!< Time to complete state transition (optional) */
    uint8_t  delay;            /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_hsl_set_t;

/** Parameters of Light HSL Hue Set */
typedef struct {
    bool     op_en;            /*!< Indicate if optional parameters are included */
    uint16_t hue;              /*!< Target value of light hsl hue state */
    uint8_t  tid;              /*!< Transaction ID */
    uint8_t  trans_time;       /*!< Time to complete state transition (optional) */
    uint8_t  delay;            /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_hsl_hue_set_t;

/** Parameters of Light HSL Saturation Set */
typedef struct {
    bool     op_en;            /*!< Indicate if optional parameters are included */
    uint16_t saturation;       /*!< Target value of light hsl hue state */
    uint8_t  tid;              /*!< Transaction ID */
    uint8_t  trans_time;       /*!< Time to complete state transition (optional) */
    uint8_t  delay;            /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_hsl_saturation_set_t;

/** Parameters of Light HSL Default Set */
typedef struct {
    uint16_t lightness;        /*!< Value of light lightness default state */
    uint16_t hue;              /*!< Value of light hue default state */
    uint16_t saturation;       /*!< Value of light saturation default state */
} esp_ble_mesh_light_hsl_default_set_t;

/** Parameters of Light HSL Range Set */
typedef struct {
    uint16_t hue_range_min;        /*!< Value of hue range min field of light hsl hue range state */
    uint16_t hue_range_max;        /*!< Value of hue range max field of light hsl hue range state */
    uint16_t saturation_range_min; /*!< Value of saturation range min field of light hsl saturation range state */
    uint16_t saturation_range_max; /*!< Value of saturation range max field of light hsl saturation range state */
} esp_ble_mesh_light_hsl_range_set_t;

/** Parameters of Light xyL Set */
typedef struct {
    bool     op_en;            /*!< Indicate whether optional parameters included */
    uint16_t xyl_lightness;    /*!< The target value of the Light xyL Lightness state */
    uint16_t xyl_x;            /*!< The target value of the Light xyL x state */
    uint16_t xyl_y;            /*!< The target value of the Light xyL y state */
    uint8_t  tid;              /*!< Transaction Identifier */
    uint8_t  trans_time;       /*!< Time to complete state transition (optional) */
    uint8_t  delay;            /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_xyl_set_t;

/** Parameters of Light xyL Default Set */
typedef struct {
    uint16_t lightness;        /*!< The value of the Light Lightness Default state */
    uint16_t xyl_x;            /*!< The value of the Light xyL x Default state */
    uint16_t xyl_y;            /*!< The value of the Light xyL y Default state */
} esp_ble_mesh_light_xyl_default_set_t;

/** Parameters of Light xyL Range Set */
typedef struct {
    uint16_t xyl_x_range_min;  /*!< The value of the xyL x Range Min field of the Light xyL x Range state */
    uint16_t xyl_x_range_max;  /*!< The value of the xyL x Range Max field of the Light xyL x Range state */
    uint16_t xyl_y_range_min;  /*!< The value of the xyL y Range Min field of the Light xyL y Range state */
    uint16_t xyl_y_range_max;  /*!< The value of the xyL y Range Max field of the Light xyL y Range state */
} esp_ble_mesh_light_xyl_range_set_t;

/** Parameter of Light LC Mode Set */
typedef struct {
    uint8_t mode;              /*!< The target value of the Light LC Mode state */
} esp_ble_mesh_light_lc_mode_set_t;

/** Parameter of Light LC OM Set */
typedef struct {
    uint8_t mode;              /*!< The target value of the Light LC Occupancy Mode state */
} esp_ble_mesh_light_lc_om_set_t;

/** Parameters of Light LC Light OnOff Set */
typedef struct {
    bool    op_en;             /*!< Indicate whether optional parameters included */
    uint8_t light_onoff;       /*!< The target value of the Light LC Light OnOff state */
    uint8_t tid;               /*!< Transaction Identifier */
    uint8_t trans_time;        /*!< Time to complete state transition (optional) */
    uint8_t delay;             /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_light_lc_light_onoff_set_t;

/** Parameter of Light LC Property Get */
typedef struct {
    uint16_t property_id;      /*!< Property ID identifying a Light LC Property */
} esp_ble_mesh_light_lc_property_get_t;

/** Parameters of Light LC Property Set */
typedef struct {
    uint16_t property_id;      /*!< Property ID identifying a Light LC Property */
    struct net_buf_simple *property_value;  /*!< Raw value for the Light LC Property */
} esp_ble_mesh_light_lc_property_set_t;

/**
 * @brief Lighting Client Model get message union
 */
typedef union {
    esp_ble_mesh_light_lc_property_get_t           lc_property_get;             /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_GET */
} esp_ble_mesh_light_client_get_state_t;

/**
 * @brief Lighting Client Model set message union
 */
typedef union {
    esp_ble_mesh_light_lightness_set_t             lightness_set;               /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_SET_UNACK */
    esp_ble_mesh_light_lightness_linear_set_t      lightness_linear_set;        /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_SET_UNACK */
    esp_ble_mesh_light_lightness_default_set_t     lightness_default_set;       /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_SET_UNACK */
    esp_ble_mesh_light_lightness_range_set_t       lightness_range_set;         /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_SET_UNACK */
    esp_ble_mesh_light_ctl_set_t                   ctl_set;                     /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_SET_UNACK */
    esp_ble_mesh_light_ctl_temperature_set_t       ctl_temperature_set;         /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_SET_UNACK */
    esp_ble_mesh_light_ctl_temperature_range_set_t ctl_temperature_range_set;   /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACK */
    esp_ble_mesh_light_ctl_default_set_t           ctl_default_set;             /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_SET_UNACK */
    esp_ble_mesh_light_hsl_set_t                   hsl_set;                     /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SET_UNACK */
    esp_ble_mesh_light_hsl_hue_set_t               hsl_hue_set;                 /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_SET_UNACK */
    esp_ble_mesh_light_hsl_saturation_set_t        hsl_saturation_set;          /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_SET_UNACK */
    esp_ble_mesh_light_hsl_default_set_t           hsl_default_set;             /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_SET_UNACK */
    esp_ble_mesh_light_hsl_range_set_t             hsl_range_set;               /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_SET_UNACK */
    esp_ble_mesh_light_xyl_set_t                   xyl_set;                     /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_SET_UNACK */
    esp_ble_mesh_light_xyl_default_set_t           xyl_default_set;             /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_SET_UNACK */
    esp_ble_mesh_light_xyl_range_set_t             xyl_range_set;               /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_SET_UNACK */
    esp_ble_mesh_light_lc_mode_set_t               lc_mode_set;                 /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_SET_UNACK */
    esp_ble_mesh_light_lc_om_set_t                 lc_om_set;                   /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_SET_UNACK */
    esp_ble_mesh_light_lc_light_onoff_set_t        lc_light_onoff_set;          /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_SET_UNACK */
    esp_ble_mesh_light_lc_property_set_t           lc_property_set;             /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET & ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_SET_UNACK */
} esp_ble_mesh_light_client_set_state_t;

/**
 *  @brief Bluetooth Mesh Light Lightness Client Model Get and Set callback parameters structure.
 */

/** Parameters of Light Lightness Status */
typedef struct {
    bool     op_en;                /*!< Indicate if optional parameters are included */
    uint16_t present_lightness;    /*!< Current value of light lightness actual state */
    uint16_t target_lightness;     /*!< Target value of light lightness actual state (optional) */
    uint8_t  remain_time;          /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_light_lightness_status_cb_t;

/** Parameters of Light Lightness Linear Status */
typedef struct {
    bool     op_en;                /*!< Indicate if optional parameters are included */
    uint16_t present_lightness;    /*!< Current value of light lightness linear state */
    uint16_t target_lightness;     /*!< Target value of light lightness linear state (optional) */
    uint8_t  remain_time;          /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_light_lightness_linear_status_cb_t;

/** Parameter of Light Lightness Last Status */
typedef struct {
    uint16_t lightness;            /*!< The value of the Light Lightness Last state */
} esp_ble_mesh_light_lightness_last_status_cb_t;

/** Parameter of Light Lightness Default Status */
typedef struct {
    uint16_t lightness;            /*!< The value of the Light Lightness default State */
} esp_ble_mesh_light_lightness_default_status_cb_t;

/** Parameters of Light Lightness Range Status */
typedef struct {
    uint8_t  status_code;          /*!< Status Code for the request message */
    uint16_t range_min;            /*!< Value of range min field of light lightness range state */
    uint16_t range_max;            /*!< Value of range max field of light lightness range state */
} esp_ble_mesh_light_lightness_range_status_cb_t;

/** Parameters of Light CTL Status */
typedef struct {
    bool     op_en;                    /*!< Indicate if optional parameters are included */
    uint16_t present_ctl_lightness;    /*!< Current value of light ctl lightness state */
    uint16_t present_ctl_temperature;  /*!< Current value of light ctl temperature state */
    uint16_t target_ctl_lightness;     /*!< Target value of light ctl lightness state (optional) */
    uint16_t target_ctl_temperature;   /*!< Target value of light ctl temperature state (C.1) */
    uint8_t  remain_time;              /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_light_ctl_status_cb_t;

/** Parameters of Light CTL Temperature Status */
typedef struct {
    bool     op_en;                    /*!< Indicate if optional parameters are included */
    uint16_t present_ctl_temperature;  /*!< Current value of light ctl temperature state */
    uint16_t present_ctl_delta_uv;     /*!< Current value of light ctl delta UV state */
    uint16_t target_ctl_temperature;   /*!< Target value of light ctl temperature state (optional) */
    uint16_t target_ctl_delta_uv;      /*!< Target value of light ctl delta UV state (C.1) */
    uint8_t  remain_time;              /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_light_ctl_temperature_status_cb_t;

/** Parameters of Light CTL Temperature Range Status */
typedef struct {
    uint8_t  status_code;      /*!< Status code for the request message */
    uint16_t range_min;        /*!< Value of temperature range min field of light ctl temperature range state */
    uint16_t range_max;        /*!< Value of temperature range max field of light ctl temperature range state */
} esp_ble_mesh_light_ctl_temperature_range_status_cb_t;

/** Parameters of Light CTL Default Status */
typedef struct {
    uint16_t lightness;        /*!< Value of light lightness default state */
    uint16_t temperature;      /*!< Value of light temperature default state */
    int16_t  delta_uv;         /*!< Value of light delta UV default state */
} esp_ble_mesh_light_ctl_default_status_cb_t;

/** Parameters of Light HSL Status */
typedef struct {
    bool     op_en;            /*!< Indicate if optional parameters are included */
    uint16_t hsl_lightness;    /*!< Current value of light hsl lightness state */
    uint16_t hsl_hue;          /*!< Current value of light hsl hue state */
    uint16_t hsl_saturation;   /*!< Current value of light hsl saturation state */
    uint8_t  remain_time;      /*!< Time to complete state transition (optional) */
} esp_ble_mesh_light_hsl_status_cb_t;

/** Parameters of Light HSL Target Status */
typedef struct {
    bool  op_en;                    /*!< Indicate if optional parameters are included */
    uint16_t hsl_lightness_target;     /*!< Target value of light hsl lightness state */
    uint16_t hsl_hue_target;           /*!< Target value of light hsl hue state */
    uint16_t hsl_saturation_target;    /*!< Target value of light hsl saturation state */
    uint8_t  remain_time;              /*!< Time to complete state transition (optional) */
} esp_ble_mesh_light_hsl_target_status_cb_t;

/** Parameters of Light HSL Hue Status */
typedef struct {
    bool     op_en;        /*!< Indicate if optional parameters are included */
    uint16_t present_hue;  /*!< Current value of light hsl hue state */
    uint16_t target_hue;   /*!< Target value of light hsl hue state (optional) */
    uint8_t  remain_time;  /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_light_hsl_hue_status_cb_t;

/** Parameters of Light HSL Saturation Status */
typedef struct {
    bool     op_en;                /*!< Indicate if optional parameters are included */
    uint16_t present_saturation;   /*!< Current value of light hsl saturation state */
    uint16_t target_saturation;    /*!< Target value of light hsl saturation state (optional) */
    uint8_t  remain_time;          /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_light_hsl_saturation_status_cb_t;

/** Parameters of Light HSL Default Status */
typedef struct {
    uint16_t lightness;    /*!< Value of light lightness default state */
    uint16_t hue;          /*!< Value of light hue default state */
    uint16_t saturation;   /*!< Value of light saturation default state */
} esp_ble_mesh_light_hsl_default_status_cb_t;

/** Parameters of Light HSL Range Status */
typedef struct {
    uint8_t  status_code;          /*!< Status code for the request message */
    uint16_t hue_range_min;        /*!< Value of hue range min field of light hsl hue range state */
    uint16_t hue_range_max;        /*!< Value of hue range max field of light hsl hue range state */
    uint16_t saturation_range_min; /*!< Value of saturation range min field of light hsl saturation range state */
    uint16_t saturation_range_max; /*!< Value of saturation range max field of light hsl saturation range state */
} esp_ble_mesh_light_hsl_range_status_cb_t;

/** Parameters of Light xyL Status */
typedef struct {
    bool     op_en;                /*!< Indicate whether optional parameters included */
    uint16_t xyl_lightness;        /*!< The present value of the Light xyL Lightness state */
    uint16_t xyl_x;                /*!< The present value of the Light xyL x state */
    uint16_t xyl_y;                /*!< The present value of the Light xyL y state */
    uint8_t  remain_time;          /*!< Time to complete state transition (optional) */
} esp_ble_mesh_light_xyl_status_cb_t;

/** Parameters of Light xyL Target Status */
typedef struct {
    bool     op_en;                /*!< Indicate whether optional parameters included */
    uint16_t target_xyl_lightness; /*!< The target value of the Light xyL Lightness state */
    uint16_t target_xyl_x;         /*!< The target value of the Light xyL x state */
    uint16_t target_xyl_y;         /*!< The target value of the Light xyL y state */
    uint8_t  remain_time;          /*!< Time to complete state transition (optional) */
} esp_ble_mesh_light_xyl_target_status_cb_t;

/** Parameters of Light xyL Default Status */
typedef struct {
    uint16_t lightness;        /*!< The value of the Light Lightness Default state */
    uint16_t xyl_x;            /*!< The value of the Light xyL x Default state */
    uint16_t xyl_y;            /*!< The value of the Light xyL y Default state */
} esp_ble_mesh_light_xyl_default_status_cb_t;

/** Parameters of Light xyL Range Status */
typedef struct {
    uint8_t  status_code;      /*!< Status Code for the requesting message */
    uint16_t xyl_x_range_min;  /*!< The value of the xyL x Range Min field of the Light xyL x Range state */
    uint16_t xyl_x_range_max;  /*!< The value of the xyL x Range Max field of the Light xyL x Range state */
    uint16_t xyl_y_range_min;  /*!< The value of the xyL y Range Min field of the Light xyL y Range state */
    uint16_t xyl_y_range_max;  /*!< The value of the xyL y Range Max field of the Light xyL y Range state */
} esp_ble_mesh_light_xyl_range_status_cb_t;

/** Parameter of Light LC Mode Status */
typedef struct {
    uint8_t mode;              /*!< The present value of the Light LC Mode state */
} esp_ble_mesh_light_lc_mode_status_cb_t;

/** Parameter of Light LC OM Status */
typedef struct {
    uint8_t mode;              /*!< The present value of the Light LC Occupancy Mode state */
} esp_ble_mesh_light_lc_om_status_cb_t;

/** Parameters of Light LC Light OnOff Status */
typedef struct {
    bool    op_en;                 /*!< Indicate whether optional parameters included */
    uint8_t present_light_onoff;   /*!< The present value of the Light LC Light OnOff state */
    uint8_t target_light_onoff;    /*!< The target value of the Light LC Light OnOff state (Optional) */
    uint8_t remain_time;           /*!< Time to complete state transition (C.1) */
} esp_ble_mesh_light_lc_light_onoff_status_cb_t;

/** Parameters of Light LC Property Status */
typedef struct {
    uint16_t property_id;      /*!< Property ID identifying a Light LC Property */
    struct net_buf_simple *property_value;  /*!< Raw value for the Light LC Property */
} esp_ble_mesh_light_lc_property_status_cb_t;

/**
 * @brief Lighting Client Model received message union
 */
typedef union {
    esp_ble_mesh_light_lightness_status_cb_t             lightness_status;             /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_STATUS */
    esp_ble_mesh_light_lightness_linear_status_cb_t      lightness_linear_status;      /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LINEAR_STATUS */
    esp_ble_mesh_light_lightness_last_status_cb_t        lightness_last_status;        /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_LAST_STATUS */
    esp_ble_mesh_light_lightness_default_status_cb_t     lightness_default_status;     /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_DEFAULT_STATUS */
    esp_ble_mesh_light_lightness_range_status_cb_t       lightness_range_status;       /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LIGHTNESS_RANGE_STATUS */
    esp_ble_mesh_light_ctl_status_cb_t                   ctl_status;                   /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_STATUS */
    esp_ble_mesh_light_ctl_temperature_status_cb_t       ctl_temperature_status;       /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_STATUS */
    esp_ble_mesh_light_ctl_temperature_range_status_cb_t ctl_temperature_range_status; /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_TEMPERATURE_RANGE_STATUS */
    esp_ble_mesh_light_ctl_default_status_cb_t           ctl_default_status;           /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_CTL_DEFAULT_STATUS */
    esp_ble_mesh_light_hsl_status_cb_t                   hsl_status;                   /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_STATUS */
    esp_ble_mesh_light_hsl_target_status_cb_t            hsl_target_status;            /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_TARGET_STATUS */
    esp_ble_mesh_light_hsl_hue_status_cb_t               hsl_hue_status;               /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_HUE_STATUS */
    esp_ble_mesh_light_hsl_saturation_status_cb_t        hsl_saturation_status;        /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_SATURATION_STATUS */
    esp_ble_mesh_light_hsl_default_status_cb_t           hsl_default_status;           /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_DEFAULT_STATUS */
    esp_ble_mesh_light_hsl_range_status_cb_t             hsl_range_status;             /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_HSL_RANGE_STATUS */
    esp_ble_mesh_light_xyl_status_cb_t                   xyl_status;                   /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_STATUS */
    esp_ble_mesh_light_xyl_target_status_cb_t            xyl_target_status;            /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_TARGET_STATUS */
    esp_ble_mesh_light_xyl_default_status_cb_t           xyl_default_status;           /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_DEFAULT_STATUS */
    esp_ble_mesh_light_xyl_range_status_cb_t             xyl_range_status;             /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_XYL_RANGE_STATUS */
    esp_ble_mesh_light_lc_mode_status_cb_t               lc_mode_status;               /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_MODE_STATUS */
    esp_ble_mesh_light_lc_om_status_cb_t                 lc_om_status;                 /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_OM_STATUS */
    esp_ble_mesh_light_lc_light_onoff_status_cb_t        lc_light_onoff_status;        /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_LIGHT_ONOFF_STATUS */
    esp_ble_mesh_light_lc_property_status_cb_t           lc_property_status;           /*!< For ESP_BLE_MESH_MODEL_OP_LIGHT_LC_PROPERTY_STATUS */
} esp_ble_mesh_light_client_status_cb_t;

/** Lighting Client Model callback parameters */
typedef struct {
    int error_code;                                     /*!< Appropriate error code */
    esp_ble_mesh_client_common_param_t   *params;       /*!< The client common parameters. */
    esp_ble_mesh_light_client_status_cb_t status_cb;    /*!< The light status message callback values */
} esp_ble_mesh_light_client_cb_param_t;

/** This enum value is the event of Lighting Client Model */
typedef enum {
    ESP_BLE_MESH_LIGHT_CLIENT_GET_STATE_EVT,
    ESP_BLE_MESH_LIGHT_CLIENT_SET_STATE_EVT,
    ESP_BLE_MESH_LIGHT_CLIENT_PUBLISH_EVT,
    ESP_BLE_MESH_LIGHT_CLIENT_TIMEOUT_EVT,
    ESP_BLE_MESH_LIGHT_CLIENT_EVT_MAX,
} esp_ble_mesh_light_client_cb_event_t;

/**
 *  @brief Bluetooth Mesh Light Client Model function.
 */

/**
 * @brief   Lighting Client Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_light_client_cb_t)(esp_ble_mesh_light_client_cb_event_t event,
                                                esp_ble_mesh_light_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Light Client Model callback.
 *
 * @param[in]   callback: pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_light_client_callback(esp_ble_mesh_light_client_cb_t callback);

/**
 * @brief       Get the value of Light Server Model states using the Light Client Model get messages.
 *
 * @note        If you want to know the opcodes and corresponding meanings accepted by this API,
 *              please refer to esp_ble_mesh_light_message_opcode_t in esp_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   get_state: Pointer of light get message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_light_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                              esp_ble_mesh_light_client_get_state_t *get_state);

/**
 * @brief       Set the value of Light Server Model states using the Light Client Model set messages.
 *
 * @note        If you want to know the opcodes and corresponding meanings accepted by this API,
 *              please refer to esp_ble_mesh_light_message_opcode_t in esp_ble_mesh_defs.h
 *
 * @param[in]   params:    Pointer to BLE Mesh common client parameters.
 * @param[in]   set_state: Pointer of light set message value.
 *                         Shall not be set to NULL.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_light_client_set_state(esp_ble_mesh_client_common_param_t *params,
                                              esp_ble_mesh_light_client_set_state_t *set_state);

/**
 * @brief Lighting Server Models related context.
 */

/** @def    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV
 *
 *  @brief  Define a new Light Lightness Server Model.
 *
 *  @note   1. The Light Lightness Server model extends the Generic Power OnOff
 *             Server model and the Generic Level Server model. When this model
 *             is present on an Element, the corresponding Light Lightness Setup
 *             Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_lightness_srv_t.
 *
 *  @return New Light Lightness Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV
 *
 *  @brief  Define a new Light Lightness Setup Server Model.
 *
 *  @note   1. The Light Lightness Setup Server model extends the Light Lightness
 *             Server model and the Generic Power OnOff Setup Server model.
 *          2. This model shall support model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_lightness_setup_srv_t.
 *
 *  @return New Light Lightness Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_LIGHTNESS_SETUP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_LIGHTNESS_SETUP_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_CTL_SRV
 *
 *  @brief  Define a new Light CTL Server Model.
 *
 *  @note   1. The Light CTL Server model extends the Light Lightness Server model.
 *             When this model is present on an Element, the corresponding Light
 *             CTL Temperature Server model and the corresponding Light CTL Setup
 *             Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *          3. The model requires two elements: the main element and the Temperature
 *             element. The Temperature element contains the corresponding Light CTL
 *             Temperature Server model and an instance of a Generic Level state
 *             bound to the Light CTL Temperature state on the Temperature element.
 *             The Light CTL Temperature state on the Temperature element is bound to
 *             the Light CTL state on the main element.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_ctl_srv_t.
 *
 *  @return New Light CTL Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_CTL_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_CTL_SETUP_SRV
 *
 *  @brief  Define a new Light CTL Setup Server Model.
 *
 *  @note   1. The Light CTL Setup Server model extends the Light CTL Server and
 *             the Light Lightness Setup Server.
 *          2. This model shall support model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_ctl_setup_srv_t.
 *
 *  @return New Light CTL Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_CTL_SETUP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_SETUP_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_CTL_TEMP_SRV
 *
 *  @brief  Define a new Light CTL Temperature Server Model.
 *
 *  @note   1. The Light CTL Temperature Server model extends the Generic Level
 *             Server model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_ctl_temp_srv_t.
 *
 *  @return New Light CTL Temperature Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_CTL_TEMP_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_HSL_SRV
 *
 *  @brief  Define a new Light HSL Server Model.
 *
 *  @note   1. The Light HSL Server model extends the Light Lightness Server model. When
 *             this model is present on an Element, the corresponding Light HSL Hue
 *             Server model and the corresponding Light HSL Saturation Server model and
 *             the corresponding Light HSL Setup Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *          3. The model requires three elements: the main element and the Hue element
 *             and the Saturation element. The Hue element contains the corresponding
 *             Light HSL Hue Server model and an instance of a Generic Level state bound
 *             to the Light HSL Hue state on the Hue element. The Saturation element
 *             contains the corresponding Light HSL Saturation Server model and an
 *             instance of a Generic Level state bound to the Light HSL Saturation state
 *             on the Saturation element. The Light HSL Hue state on the Hue element is
 *             bound to the Light HSL state on the main element and the Light HSL
 *             Saturation state on the Saturation element is bound to the Light HSL state
 *             on the main element.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_hsl_srv_t.
 *
 *  @return New Light HSL Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_HSL_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_HSL_SETUP_SRV
 *
 *  @brief  Define a new Light HSL Setup Server Model.
 *
 *  @note   1. The Light HSL Setup Server model extends the Light HSL Server and
 *             the Light Lightness Setup Server.
 *          2. This model shall support model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_hsl_setup_srv_t.
 *
 *  @return New Light HSL Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_HSL_SETUP_SRV(srv_pub, srv_data)           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SETUP_SRV,   \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_HSL_HUE_SRV
 *
 *  @brief  Define a new Light HSL Hue Server Model.
 *
 *  @note   1. The Light HSL Hue Server model extends the Generic Level Server model.
 *             This model is associated with the Light HSL Server model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_hsl_hue_srv_t.
 *
 *  @return New Light HSL Hue Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_HSL_HUE_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_HUE_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_HSL_SAT_SRV
 *
 *  @brief  Define a new Light HSL Saturation Server Model.
 *
 *  @note   1. The Light HSL Saturation Server model extends the Generic Level Server
 *             model. This model is associated with the Light HSL Server model.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_hsl_sat_srv_t.
 *
 *  @return New Light HSL Saturation Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_HSL_SAT_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_HSL_SAT_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_XYL_SRV
 *
 *  @brief  Define a new Light xyL Server Model.
 *
 *  @note   1. The Light xyL Server model extends the Light Lightness Server model.
 *             When this model is present on an Element, the corresponding Light xyL
 *             Setup Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_xyl_srv_t.
 *
 *  @return New Light xyL Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_XYL_SRV(srv_pub, srv_data)             \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_SRV,     \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_XYL_SETUP_SRV
 *
 *  @brief  Define a new Light xyL Setup Server Model.
 *
 *  @note   1. The Light xyL Setup Server model extends the Light xyL Server and
 *             the Light Lightness Setup Server.
 *          2. This model shall support model subscription.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_xyl_setup_srv_t.
 *
 *  @return New Light xyL Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_XYL_SETUP_SRV(srv_pub, srv_data)           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_XYL_SETUP_SRV,   \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_LC_SRV
 *
 *  @brief  Define a new Light LC Server Model.
 *
 *  @note   1. The Light LC (Lightness Control) Server model extends the Light
 *             Lightness Server model and the Generic OnOff Server model. When
 *             this model is present on an Element, the corresponding Light LC
 *             Setup Server model shall also be present.
 *          2. This model shall support model publication and model subscription.
 *          3. This model may be used to represent an element that is a client to
 *             a Sensor Server model and controls the Light Lightness Actual state
 *             via defined state bindings.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_lc_srv_t.
 *
 *  @return New Light LC Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_LC_SRV(srv_pub, srv_data)           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_LC_SRV,   \
                    NULL, srv_pub, srv_data)

/** @def    ESP_BLE_MESH_MODEL_LIGHT_LC_SETUP_SRV
 *
 *  @brief  Define a new Light LC Setup Server Model.
 *
 *  @note   1. The Light LC (Lightness Control) Setup model extends the Light LC
 *             Server model.
 *          2. This model shall support model publication and model subscription.
 *          3. This model may be used to configure setup parameters for the Light
 *             LC Server model.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_light_lc_setup_srv_t.
 *
 *  @return New Light LC Setup Server Model instance.
 */
#define ESP_BLE_MESH_MODEL_LIGHT_LC_SETUP_SRV(srv_pub, srv_data)           \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_LIGHT_LC_SETUP_SRV,   \
                    NULL, srv_pub, srv_data)

/** Parameters of Light Lightness state */
typedef struct {
    uint16_t lightness_linear;          /*!< The present value of Light Lightness Linear state */
    uint16_t target_lightness_linear;   /*!< The target value of Light Lightness Linear state */

    uint16_t lightness_actual;          /*!< The present value of Light Lightness Actual state */
    uint16_t target_lightness_actual;   /*!< The target value of Light Lightness Actual state */

    uint16_t lightness_last;            /*!< The value of Light Lightness Last state */
    uint16_t lightness_default;         /*!< The value of Light Lightness Default state */

    uint8_t  status_code;               /*!< The status code of setting Light Lightness Range state */
    uint16_t lightness_range_min;       /*!< The minimum value of Light Lightness Range state */
    uint16_t lightness_range_max;       /*!< The maximum value of Light Lightness Range state */
} esp_ble_mesh_light_lightness_state_t;

/** User data of Light Lightness Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the Lighting Lightness Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;        /*!< Response control of the server model received messages */
    esp_ble_mesh_light_lightness_state_t *state;    /*!< Parameters of the Light Lightness state */
    esp_ble_mesh_last_msg_info_t last;              /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t actual_transition;  /*!< Parameters of state transition */
    esp_ble_mesh_state_transition_t linear_transition;  /*!< Parameters of state transition */
    int32_t tt_delta_lightness_actual;      /*!< Delta change value of lightness actual state transition */
    int32_t tt_delta_lightness_linear;      /*!< Delta change value of lightness linear state transition */
} esp_ble_mesh_light_lightness_srv_t;

/** User data of Light Lightness Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                    /*!< Pointer to the Lighting Lightness Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;        /*!< Response control of the server model received messages */
    esp_ble_mesh_light_lightness_state_t *state;    /*!< Parameters of the Light Lightness state */
} esp_ble_mesh_light_lightness_setup_srv_t;

/** Parameters of Light CTL state */
typedef struct {
    uint16_t lightness;             /*!< The present value of Light CTL Lightness state */
    uint16_t target_lightness;      /*!< The target value of Light CTL Lightness state */

    uint16_t temperature;           /*!< The present value of Light CTL Temperature state */
    uint16_t target_temperature;    /*!< The target value of Light CTL Temperature state */

    int16_t  delta_uv;              /*!< The present value of Light CTL Delta UV state */
    int16_t  target_delta_uv;       /*!< The target value of Light CTL Delta UV state */

    uint8_t  status_code;           /*!< The statue code of setting Light CTL Temperature Range state */
    uint16_t temperature_range_min; /*!< The minimum value of Light CTL Temperature Range state */
    uint16_t temperature_range_max; /*!< The maximum value of Light CTL Temperature Range state */

    uint16_t lightness_default;     /*!< The value of Light Lightness Default state */
    uint16_t temperature_default;   /*!< The value of Light CTL Temperature Default state */
    int16_t  delta_uv_default;      /*!< The value of Light CTL Delta UV Default state */
} esp_ble_mesh_light_ctl_state_t;

/** User data of Light CTL Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting CTL Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_ctl_state_t *state;      /*!< Parameters of the Light CTL state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
    int32_t tt_delta_lightness;     /*!< Delta change value of lightness state transition */
    int32_t tt_delta_temperature;   /*!< Delta change value of temperature state transition */
    int32_t tt_delta_delta_uv;      /*!< Delta change value of delta uv state transition */
} esp_ble_mesh_light_ctl_srv_t;

/** User data of Light CTL Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting CTL Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_ctl_state_t *state;      /*!< Parameters of the Light CTL state */
} esp_ble_mesh_light_ctl_setup_srv_t;

/** User data of Light CTL Temperature Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting CTL Temperature Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_ctl_state_t *state;      /*!< Parameters of the Light CTL state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
    int32_t tt_delta_temperature;   /*!< Delta change value of temperature state transition */
    int32_t tt_delta_delta_uv;      /*!< Delta change value of delta uv state transition */
} esp_ble_mesh_light_ctl_temp_srv_t;

/** Parameters of Light HSL state */
typedef struct {
    uint16_t lightness;             /*!< The present value of Light HSL Lightness state */
    uint16_t target_lightness;      /*!< The target value of Light HSL Lightness state */

    uint16_t hue;                   /*!< The present value of Light HSL Hue state */
    uint16_t target_hue;            /*!< The target value of Light HSL Hue state */

    uint16_t saturation;            /*!< The present value of Light HSL Saturation state */
    uint16_t target_saturation;     /*!< The target value of Light HSL Saturation state */

    uint16_t lightness_default;     /*!< The value of Light Lightness Default state */
    uint16_t hue_default;           /*!< The value of Light HSL Hue Default state */
    uint16_t saturation_default;    /*!< The value of Light HSL Saturation Default state */

    uint8_t  status_code;           /*!< The status code of setting Light HSL Hue & Saturation Range state */
    uint16_t hue_range_min;         /*!< The minimum value of Light HSL Hue Range state */
    uint16_t hue_range_max;         /*!< The maximum value of Light HSL Hue Range state */
    uint16_t saturation_range_min;  /*!< The minimum value of Light HSL Saturation state */
    uint16_t saturation_range_max;  /*!< The maximum value of Light HSL Saturation state */
} esp_ble_mesh_light_hsl_state_t;

/** User data of Light HSL Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting HSL Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_hsl_state_t *state;      /*!< Parameters of the Light HSL state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
    int32_t tt_delta_lightness;     /*!< Delta change value of lightness state transition */
    int32_t tt_delta_hue;           /*!< Delta change value of hue state transition */
    int32_t tt_delta_saturation;    /*!< Delta change value of saturation state transition */
} esp_ble_mesh_light_hsl_srv_t;

/** User data of Light HSL Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting HSL Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_hsl_state_t *state;      /*!< Parameters of the Light HSL state */
} esp_ble_mesh_light_hsl_setup_srv_t;

/** User data of Light HSL Hue Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting HSL Hue Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_hsl_state_t *state;      /*!< Parameters of the Light HSL state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
    int32_t tt_delta_hue;   /*!< Delta change value of hue state transition */
} esp_ble_mesh_light_hsl_hue_srv_t;

/** User data of Light HSL Saturation Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;    /*!< Pointer to the Lighting HSL Saturation Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_hsl_state_t *state;      /*!< Parameters of the Light HSL state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
    int32_t tt_delta_saturation;    /*!< Delta change value of saturation state transition */
} esp_ble_mesh_light_hsl_sat_srv_t;

/** Parameters of Light xyL state */
typedef struct {
    uint16_t lightness;         /*!< The present value of Light xyL Lightness state */
    uint16_t target_lightness;  /*!< The target value of Light xyL Lightness state */

    uint16_t x;                 /*!< The present value of Light xyL x state */
    uint16_t target_x;          /*!< The target value of Light xyL x state */

    uint16_t y;                 /*!< The present value of Light xyL y state */
    uint16_t target_y;          /*!< The target value of Light xyL y state */

    uint16_t lightness_default; /*!< The value of Light Lightness Default state */
    uint16_t x_default;         /*!< The value of Light xyL x Default state */
    uint16_t y_default;         /*!< The value of Light xyL y Default state */

    uint8_t  status_code;       /*!< The status code of setting Light xyL x & y Range state */
    uint16_t x_range_min;       /*!< The minimum value of Light xyL x Range state */
    uint16_t x_range_max;       /*!< The maximum value of Light xyL x Range state */
    uint16_t y_range_min;       /*!< The minimum value of Light xyL y Range state */
    uint16_t y_range_max;       /*!< The maximum value of Light xyL y Range state */
} esp_ble_mesh_light_xyl_state_t;

/** User data of Light xyL Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting xyL Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_xyl_state_t *state;      /*!< Parameters of the Light xyL state */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
    int32_t tt_delta_lightness; /*!< Delta change value of lightness state transition */
    int32_t tt_delta_x;         /*!< Delta change value of x state transition */
    int32_t tt_delta_y;         /*!< Delta change value of y state transition */
} esp_ble_mesh_light_xyl_srv_t;

/** User data of Light xyL Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting xyL Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_xyl_state_t *state;      /*!< Parameters of the Light xyL state */
} esp_ble_mesh_light_xyl_setup_srv_t;

/** Parameters of Light LC states */
typedef struct {
    /**
     * 0b0 The controller is turned off.
     * - The binding with the Light Lightness state is disabled.
     * 0b1 The controller is turned on.
     * - The binding with the Light Lightness state is enabled.
     */
    uint32_t mode : 1,                  /*!< The value of Light LC Mode state */
             occupancy_mode : 1,        /*!< The value of Light LC Occupancy Mode state */
             light_onoff : 1,           /*!< The present value of Light LC Light OnOff state */
             target_light_onoff : 1,    /*!< The target value of Light LC Light OnOff state */
             occupancy : 1,             /*!< The value of Light LC Occupancy state */
             ambient_luxlevel : 24;     /*!< The value of Light LC Ambient LuxLevel state */

    /**
     * 1. Light LC Linear Output = max((Lightness Out)^2/65535, Regulator Output)
     * 2. If the Light LC Mode state is set to 0b1, the binding is enabled and upon
     *    a change of the Light LC Linear Output state, the following operation
     *    shall be performed:
     *    Light Lightness Linear = Light LC Linear Output
     * 3. If the Light LC Mode state is set to 0b0, the binding is disabled (i.e.,
     *    upon a change of the Light LC Linear Output state, no operation on the
     *    Light Lightness Linear state is performed).
     */
    uint16_t linear_output;     /*!< The value of Light LC Linear Output state */
} esp_ble_mesh_light_lc_state_t;

/**
 * Parameters of Light Property states.
 * The Light LC Property states are read / write states that determine the
 * configuration of a Light Lightness Controller. Each state is represented
 * by a device property and is controlled by Light LC Property messages.
 */
typedef struct {
    /**
     * A timing state that determines the delay for changing the Light LC
     * Occupancy state upon receiving a Sensor Status message from an
     * occupancy sensor.
     */
    uint32_t time_occupancy_delay;  /*!< The value of Light LC Time Occupancy Delay state */
    /**
     * A timing state that determines the time the controlled lights fade
     * to the level determined by the Light LC Lightness On state.
     */
    uint32_t time_fade_on;      /*!< The value of Light LC Time Fade On state */
    /**
     * A timing state that determines the time the controlled lights stay
     * at the level determined by the Light LC Lightness On state.
     */
    uint32_t time_run_on;       /*!< The value of Light LC Time Run On state */
    /**
     * A timing state that determines the time the controlled lights fade
     * from the level determined by the Light LC Lightness On state to the
     * level determined by the Light Lightness Prolong state.
     */
    uint32_t time_fade;         /*!< The value of Light LC Time Fade state */
    /**
     * A timing state that determines the time the controlled lights stay at
     * the level determined by the Light LC Lightness Prolong state.
     */
    uint32_t time_prolong;      /*!< The value of Light LC Time Prolong state */
    /**
     * A timing state that determines the time the controlled lights fade from
     * the level determined by the Light LC Lightness Prolong state to the level
     * determined by the Light LC Lightness Standby state when the transition is
     * automatic.
     */
    uint32_t time_fade_standby_auto;    /*!< The value of Light LC Time Fade Standby Auto state */
    /**
     * A timing state that determines the time the controlled lights fade from
     * the level determined by the Light LC Lightness Prolong state to the level
     * determined by the Light LC Lightness Standby state when the transition is
     * triggered by a change in the Light LC Light OnOff state.
     */
    uint32_t time_fade_standby_manual;  /*!< The value of Light LC Time Fade Standby Manual state */

    /**
     * A lightness state that determines the perceptive light lightness at the
     * Occupancy and Run internal controller states.
     */
    uint16_t lightness_on;          /*!< The value of Light LC Lightness On state */
    /**
     * A lightness state that determines the light lightness at the Prolong
     * internal controller state.
     */
    uint16_t lightness_prolong;     /*!< The value of Light LC Lightness Prolong state */
    /**
     * A lightness state that determines the light lightness at the Standby
     * internal controller state.
     */
    uint16_t lightness_standby;     /*!< The value of Light LC Lightness Standby state */

    /**
     * A uint16 state representing the Ambient LuxLevel level that determines
     * if the controller transitions from the Light Control Standby state.
     */
    uint16_t ambient_luxlevel_on;       /*!< The value of Light LC Ambient LuxLevel On state */
    /**
     * A uint16 state representing the required Ambient LuxLevel level in the
     * Prolong state.
     */
    uint16_t ambient_luxlevel_prolong;  /*!< The value of Light LC Ambient LuxLevel Prolong state */
    /**
     * A uint16 state representing the required Ambient LuxLevel level in the
     * Standby state.
     */
    uint16_t ambient_luxlevel_standby;  /*!< The value of Light LC Ambient LuxLevel Standby state */

    /**
     * A float32 state representing the integral coefficient that determines the
     * integral part of the equation defining the output of the Light LC PI
     * Feedback Regulator, when Light LC Ambient LuxLevel is less than LuxLevel
     * Out. Valid range: 0.0 ~ 1000.0. The default value is 250.0.
     */
    float regulator_kiu;    /*!< The value of Light LC Regulator Kiu state */
    /**
     * A float32 state representing the integral coefficient that determines the
     * integral part of the equation defining the output of the Light LC PI
     * Feedback Regulator, when Light LC Ambient LuxLevel is greater than or equal
     * to the value of the LuxLevel Out state. Valid range: 0.0 ~ 1000.0. The
     * default value is 25.0.
     */
    float regulator_kid;    /*!< The value of Light LC Regulator Kid state */
    /**
     * A float32 state representing the proportional coefficient that determines
     * the proportional part of the equation defining the output of the Light LC
     * PI Feedback Regulator, when Light LC Ambient LuxLevel is less than the value
     * of the LuxLevel Out state. Valid range: 0.0 ~ 1000.0. The default value is 80.0.
     */
    float regulator_kpu;    /*!< The value of Light LC Regulator Kpu state */
    /**
     * A float32 state representing the proportional coefficient that determines
     * the proportional part of the equation defining the output of the Light LC PI
     * Feedback Regulator, when Light LC Ambient LuxLevel is greater than or equal
     * to the value of the LuxLevel Out state. Valid range: 0.0 ~ 1000.0. The default
     * value is 80.0.
     */
    float regulator_kpd;    /*!< The value of Light LC Regulator Kpd state */
    /**
     * A int8 state representing the percentage accuracy of the Light LC PI Feedback
     * Regulator. Valid range: 0.0 ~ 100.0. The default value is 2.0.
     */
    int8_t regulator_accuracy;  /*!< The value of Light LC Regulator Accuracy state */

    /**
     * If the message Raw field contains a Raw Value for the Time Since Motion
     * Sensed device property, which represents a value less than or equal to
     * the value of the Light LC Occupancy Delay state, it shall delay setting
     * the Light LC Occupancy state to 0b1 by the difference between the value
     * of the Light LC Occupancy Delay state and the received Time Since Motion
     * value.
     */
    uint32_t set_occupancy_to_1_delay;  /*!< The value of the difference between value of the
                                            Light LC Occupancy Delay state and the received
                                            Time Since Motion value */
} esp_ble_mesh_light_lc_property_state_t;

/** This enum value is the Light LC State Machine states */
typedef enum {
    ESP_BLE_MESH_LC_OFF,
    ESP_BLE_MESH_LC_STANDBY,
    ESP_BLE_MESH_LC_FADE_ON,
    ESP_BLE_MESH_LC_RUN,
    ESP_BLE_MESH_LC_FADE,
    ESP_BLE_MESH_LC_PROLONG,
    ESP_BLE_MESH_LC_FADE_STANDBY_AUTO,
    ESP_BLE_MESH_LC_FADE_STANDBY_MANUAL,
} esp_ble_mesh_lc_state_t;

/** Parameters of Light LC state machine */
typedef struct {
    /**
     * The Fade On, Fade, Fade Standby Auto, and Fade Standby Manual states are
     * transition states that define the transition of the Lightness Out and
     * LuxLevel Out states. This transition can be started as a result of the
     * Light LC State Machine change or as a result of receiving the Light LC
     * Light OnOff Set or Light LC Light Set Unacknowledged message.
     */
    struct {
        uint8_t fade_on;                /*!< The value of transition time of Light LC Time Fade On */
        uint8_t fade;                   /*!< The value of transition time of Light LC Time Fade */
        uint8_t fade_standby_auto;      /*!< The value of transition time of Light LC Time Fade Standby Auto */
        uint8_t fade_standby_manual;    /*!< The value of transition time of Light LC Time Fade Standby Manual */
    } trans_time;                       /*!< The value of transition time */
    esp_ble_mesh_lc_state_t state;      /*!< The value of Light LC state machine state */
    struct k_delayed_work timer;        /*!< Timer of Light LC state machine */
} esp_ble_mesh_light_lc_state_machine_t;

/** Parameters of Light Lightness controller */
typedef struct {
    esp_ble_mesh_light_lc_state_t          state;           /*!< Parameters of Light LC state */
    esp_ble_mesh_light_lc_property_state_t prop_state;      /*!< Parameters of Light LC Property state */
    esp_ble_mesh_light_lc_state_machine_t  state_machine;   /*!< Parameters of Light LC state machine */
} esp_ble_mesh_light_control_t;

/** User data of Light LC Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting LC Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_control_t *lc;           /*!< Parameters of the Light controller */
    esp_ble_mesh_last_msg_info_t last;          /*!< Parameters of the last received set message */
    esp_ble_mesh_state_transition_t transition; /*!< Parameters of state transition */
} esp_ble_mesh_light_lc_srv_t;

/** User data of Light LC Setup Server Model */
typedef struct {
    esp_ble_mesh_model_t *model;                /*!< Pointer to the Lighting LC Setup Server Model. Initialized internally. */
    esp_ble_mesh_server_rsp_ctrl_t rsp_ctrl;    /*!< Response control of the server model received messages */
    esp_ble_mesh_light_control_t *lc;           /*!< Parameters of the Light controller */
} esp_ble_mesh_light_lc_setup_srv_t;

/** Parameter of Light Lightness Actual state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light Lightness Actual state */
} esp_ble_mesh_state_change_light_lightness_set_t;

/** Parameter of Light Lightness Linear state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light Lightness Linear state */
} esp_ble_mesh_state_change_light_lightness_linear_set_t;

/** Parameter of Light Lightness Default state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light Lightness Default state */
} esp_ble_mesh_state_change_light_lightness_default_set_t;

/** Parameters of Light Lightness Range state change event */
typedef struct {
    uint16_t range_min;     /*!< The minimum value of Light Lightness Range state */
    uint16_t range_max;     /*!< The maximum value of Light Lightness Range state */
} esp_ble_mesh_state_change_light_lightness_range_set_t;

/** Parameters of Light CTL state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light CTL Lightness state */
    uint16_t temperature;   /*!< The value of Light CTL Temperature state */
    int16_t  delta_uv;      /*!< The value of Light CTL Delta UV state */
} esp_ble_mesh_state_change_light_ctl_set_t;

/** Parameters of Light CTL Temperature state change event */
typedef struct {
    uint16_t temperature;   /*!< The value of Light CTL Temperature state */
    int16_t  delta_uv;      /*!< The value of Light CTL Delta UV state */
} esp_ble_mesh_state_change_light_ctl_temperature_set_t;

/** Parameters of Light CTL Temperature Range state change event */
typedef struct {
    uint16_t range_min;     /*!< The minimum value of Light CTL Temperature Range state */
    uint16_t range_max;     /*!< The maximum value of Light CTL Temperature Range state */
} esp_ble_mesh_state_change_light_ctl_temperature_range_set_t;

/** Parameters of Light CTL Default state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light Lightness Default state */
    uint16_t temperature;   /*!< The value of Light CTL Temperature Default state */
    int16_t  delta_uv;      /*!< The value of Light CTL Delta UV Default state */
} esp_ble_mesh_state_change_light_ctl_default_set_t;

/** Parameters of Light HSL state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light HSL Lightness state */
    uint16_t hue;           /*!< The value of Light HSL Hue state */
    uint16_t saturation;    /*!< The value of Light HSL Saturation state */
} esp_ble_mesh_state_change_light_hsl_set_t;

/** Parameter of Light HSL Hue state change event */
typedef struct {
    uint16_t hue;           /*!< The value of Light HSL Hue state */
} esp_ble_mesh_state_change_light_hsl_hue_set_t;

/** Parameter of Light HSL Saturation state change event */
typedef struct {
    uint16_t saturation;    /*!< The value of Light HSL Saturation state */
} esp_ble_mesh_state_change_light_hsl_saturation_set_t;

/** Parameters of Light HSL Default state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light HSL Lightness Default state */
    uint16_t hue;           /*!< The value of Light HSL Hue Default state */
    uint16_t saturation;    /*!< The value of Light HSL Saturation Default state */
} esp_ble_mesh_state_change_light_hsl_default_set_t;

/** Parameters of Light HSL Range state change event */
typedef struct {
    uint16_t hue_range_min;         /*!< The minimum hue value of Light HSL Range state */
    uint16_t hue_range_max;         /*!< The maximum hue value of Light HSL Range state */
    uint16_t saturation_range_min;  /*!< The minimum saturation value of Light HSL Range state */
    uint16_t saturation_range_max;  /*!< The maximum saturation value of Light HSL Range state */
} esp_ble_mesh_state_change_light_hsl_range_set_t;

/** Parameters of Light xyL state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light xyL Lightness state */
    uint16_t x;             /*!< The value of Light xyL x state */
    uint16_t y;             /*!< The value of Light xyL y state */
} esp_ble_mesh_state_change_light_xyl_set_t;

/** Parameters of Light xyL Default state change event */
typedef struct {
    uint16_t lightness;     /*!< The value of Light Lightness Default state */
    uint16_t x;             /*!< The value of Light xyL x Default state */
    uint16_t y;             /*!< The value of Light xyL y Default state */
} esp_ble_mesh_state_change_light_xyl_default_set_t;

/** Parameters of Light xyL Range state change event */
typedef struct {
    uint16_t x_range_min;   /*!< The minimum value of Light xyL x Range state */
    uint16_t x_range_max;   /*!< The maximum value of Light xyL x Range state */
    uint16_t y_range_min;   /*!< The minimum value of Light xyL y Range state */
    uint16_t y_range_max;   /*!< The maximum value of Light xyL y Range state */
} esp_ble_mesh_state_change_light_xyl_range_set_t;

/** Parameter of Light LC Mode state change event */
typedef struct {
    uint8_t mode;       /*!< The value of Light LC Mode state */
} esp_ble_mesh_state_change_light_lc_mode_set_t;

/** Parameter of Light LC Occupancy Mode state change event */
typedef struct {
    uint8_t mode;       /*!< The value of Light LC Occupancy Mode state */
} esp_ble_mesh_state_change_light_lc_om_set_t;

/** Parameter of Light LC Light OnOff state change event */
typedef struct {
    uint8_t onoff;      /*!< The value of Light LC Light OnOff state */
} esp_ble_mesh_state_change_light_lc_light_onoff_set_t;

/** Parameters of Light LC Property state change event */
typedef struct {
    uint16_t property_id;   /*!< The property id of Light LC Property state */
    struct net_buf_simple *property_value;  /*!< The property value of Light LC Property state */
} esp_ble_mesh_state_change_light_lc_property_set_t;

/** Parameters of Sensor Status state change event */
typedef struct {
    uint16_t property_id;       /*!< The value of Sensor Property ID */
    /** Parameters of Sensor Status related state */
    union {
        uint8_t  occupancy;                 /*!< The value of Light LC Occupancy state */
        uint32_t set_occupancy_to_1_delay;  /*!< The value of Light LC Set Occupancy to 1 Delay state */
        uint32_t ambient_luxlevel;          /*!< The value of Light LC Ambient Luxlevel state */
    } state;
} esp_ble_mesh_state_change_sensor_status_t;

/**
 * @brief Lighting Server Model state change value union
 */
typedef union {
    /**
     * The recv_op in ctx can be used to decide which state is changed.
     */
    esp_ble_mesh_state_change_light_lightness_set_t             lightness_set;          /*!< Light Lightness Set */
    esp_ble_mesh_state_change_light_lightness_linear_set_t      lightness_linear_set;   /*!< Light Lightness Linear Set */
    esp_ble_mesh_state_change_light_lightness_default_set_t     lightness_default_set;  /*!< Light Lightness Default Set */
    esp_ble_mesh_state_change_light_lightness_range_set_t       lightness_range_set;    /*!< Light Lightness Range Set */
    esp_ble_mesh_state_change_light_ctl_set_t                   ctl_set;                /*!< Light CTL Set */
    esp_ble_mesh_state_change_light_ctl_temperature_set_t       ctl_temp_set;           /*!< Light CTL Temperature Set */
    esp_ble_mesh_state_change_light_ctl_temperature_range_set_t ctl_temp_range_set;     /*!< Light CTL Temperature Range Set */
    esp_ble_mesh_state_change_light_ctl_default_set_t           ctl_default_set;        /*!< Light CTL Default Set */
    esp_ble_mesh_state_change_light_hsl_set_t                   hsl_set;                /*!< Light HSL Set */
    esp_ble_mesh_state_change_light_hsl_hue_set_t               hsl_hue_set;            /*!< Light HSL Hue Set */
    esp_ble_mesh_state_change_light_hsl_saturation_set_t        hsl_saturation_set;     /*!< Light HSL Saturation Set */
    esp_ble_mesh_state_change_light_hsl_default_set_t           hsl_default_set;        /*!< Light HSL Default Set */
    esp_ble_mesh_state_change_light_hsl_range_set_t             hsl_range_set;          /*!< Light HSL Range Set */
    esp_ble_mesh_state_change_light_xyl_set_t                   xyl_set;                /*!< Light xyL Set */
    esp_ble_mesh_state_change_light_xyl_default_set_t           xyl_default_set;        /*!< Light xyL Default Set */
    esp_ble_mesh_state_change_light_xyl_range_set_t             xyl_range_set;          /*!< Light xyL Range Set */
    esp_ble_mesh_state_change_light_lc_mode_set_t               lc_mode_set;            /*!< Light LC Mode Set */
    esp_ble_mesh_state_change_light_lc_om_set_t                 lc_om_set;              /*!< Light LC Occupancy Mode Set */
    esp_ble_mesh_state_change_light_lc_light_onoff_set_t        lc_light_onoff_set;     /*!< Light LC Light OnOff Set */
    esp_ble_mesh_state_change_light_lc_property_set_t           lc_property_set;        /*!< Light LC Property Set */
    esp_ble_mesh_state_change_sensor_status_t                   sensor_status;          /*!< Sensor Status */
} esp_ble_mesh_lighting_server_state_change_t;

/** Context of the received Light LC Property Get message */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Light LC Property */
} esp_ble_mesh_server_recv_light_lc_property_get_t;

/**
 * @brief Lighting Server Model received get message union
 */
typedef union {
    esp_ble_mesh_server_recv_light_lc_property_get_t lc_property;   /*!< Light LC Property Get */
} esp_ble_mesh_lighting_server_recv_get_msg_t;

/** Context of the received Light Lightness Set message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t lightness;     /*!< Target value of light lightness actual state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_lightness_set_t;

/** Context of the received Light Lightness Linear Set message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t lightness;     /*!< Target value of light lightness linear state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_lightness_linear_set_t;

/** Context of the received Light Lightness Default Set message */
typedef struct {
    uint16_t lightness;     /*!< The value of the Light Lightness Default state */
} esp_ble_mesh_server_recv_light_lightness_default_set_t;

/** Context of the received Light Lightness Range Set message */
typedef struct {
    uint16_t range_min;     /*!< Value of range min field of light lightness range state */
    uint16_t range_max;     /*!< Value of range max field of light lightness range state */
} esp_ble_mesh_server_recv_light_lightness_range_set_t;

/** Context of the received Light CTL Set message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t lightness;     /*!< Target value of light ctl lightness state */
    uint16_t temperature;   /*!< Target value of light ctl temperature state */
    int16_t  delta_uv;      /*!< Target value of light ctl delta UV state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_ctl_set_t;

/** Context of the received Light CTL Temperature Set message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t temperature;   /*!< Target value of light ctl temperature state */
    int16_t  delta_uv;      /*!< Target value of light ctl delta UV state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_ctl_temperature_set_t;

/** Context of the received Light CTL Temperature Range Set message */
typedef struct {
    uint16_t range_min;     /*!< Value of temperature range min field of light ctl temperature range state */
    uint16_t range_max;     /*!< Value of temperature range max field of light ctl temperature range state */
} esp_ble_mesh_server_recv_light_ctl_temperature_range_set_t;

/** Context of the received Light CTL Default Set message */
typedef struct {
    uint16_t lightness;     /*!< Value of light lightness default state */
    uint16_t temperature;   /*!< Value of light temperature default state */
    int16_t  delta_uv;      /*!< Value of light delta UV default state */
} esp_ble_mesh_server_recv_light_ctl_default_set_t;

/** Context of the received Light HSL Set message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t lightness;     /*!< Target value of light hsl lightness state */
    uint16_t hue;           /*!< Target value of light hsl hue state */
    uint16_t saturation;    /*!< Target value of light hsl saturation state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_hsl_set_t;

/** Context of the received Light HSL Hue Set message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t hue;           /*!< Target value of light hsl hue state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_hsl_hue_set_t;

/** Context of the received Light HSL Saturation Set message */
typedef struct {
    bool     op_en;         /*!< Indicate if optional parameters are included */
    uint16_t saturation;    /*!< Target value of light hsl hue state */
    uint8_t  tid;           /*!< Transaction ID */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_hsl_saturation_set_t;

/** Context of the received Light HSL Default Set message */
typedef struct {
    uint16_t lightness;     /*!< Value of light lightness default state */
    uint16_t hue;           /*!< Value of light hue default state */
    uint16_t saturation;    /*!< Value of light saturation default state */
} esp_ble_mesh_server_recv_light_hsl_default_set_t;

/** Context of the received Light HSL Range Set message */
typedef struct {
    uint16_t hue_range_min;         /*!< Value of hue range min field of light hsl hue range state */
    uint16_t hue_range_max;         /*!< Value of hue range max field of light hsl hue range state */
    uint16_t saturation_range_min;  /*!< Value of saturation range min field of light hsl saturation range state */
    uint16_t saturation_range_max;  /*!< Value of saturation range max field of light hsl saturation range state */
} esp_ble_mesh_server_recv_light_hsl_range_set_t;

/** Context of the received Light xyL Set message */
typedef struct {
    bool     op_en;         /*!< Indicate whether optional parameters included */
    uint16_t lightness;     /*!< The target value of the Light xyL Lightness state */
    uint16_t x;             /*!< The target value of the Light xyL x state */
    uint16_t y;             /*!< The target value of the Light xyL y state */
    uint8_t  tid;           /*!< Transaction Identifier */
    uint8_t  trans_time;    /*!< Time to complete state transition (optional) */
    uint8_t  delay;         /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_xyl_set_t;

/** Context of the received Light xyL Default Set message */
typedef struct {
    uint16_t lightness;     /*!< The value of the Light Lightness Default state */
    uint16_t x;             /*!< The value of the Light xyL x Default state */
    uint16_t y;             /*!< The value of the Light xyL y Default state */
} esp_ble_mesh_server_recv_light_xyl_default_set_t;

/** Context of the received Light xyl Range Set message */
typedef struct {
    uint16_t x_range_min;   /*!< The value of the xyL x Range Min field of the Light xyL x Range state */
    uint16_t x_range_max;   /*!< The value of the xyL x Range Max field of the Light xyL x Range state */
    uint16_t y_range_min;   /*!< The value of the xyL y Range Min field of the Light xyL y Range state */
    uint16_t y_range_max;   /*!< The value of the xyL y Range Max field of the Light xyL y Range state */
} esp_ble_mesh_server_recv_light_xyl_range_set_t;

/** Context of the received Light LC Mode Set message */
typedef struct {
    uint8_t mode;   /*!< The target value of the Light LC Mode state */
} esp_ble_mesh_server_recv_light_lc_mode_set_t;

/** Context of the received Light OM Set message */
typedef struct {
    uint8_t mode;   /*!< The target value of the Light LC Occupancy Mode state */
} esp_ble_mesh_server_recv_light_lc_om_set_t;

/** Context of the received Light LC Light OnOff Set message */
typedef struct {
    bool    op_en;          /*!< Indicate whether optional parameters included */
    uint8_t light_onoff;    /*!< The target value of the Light LC Light OnOff state */
    uint8_t tid;            /*!< Transaction Identifier */
    uint8_t trans_time;     /*!< Time to complete state transition (optional) */
    uint8_t delay;          /*!< Indicate message execution delay (C.1) */
} esp_ble_mesh_server_recv_light_lc_light_onoff_set_t;

/** Context of the received Light LC Property Set message */
typedef struct {
    uint16_t property_id;   /*!< Property ID identifying a Light LC Property */
    struct net_buf_simple *property_value;  /*!< Raw value for the Light LC Property */
} esp_ble_mesh_server_recv_light_lc_property_set_t;

/**
 * @brief Lighting Server Model received set message union
 */
typedef union {
    esp_ble_mesh_server_recv_light_lightness_set_t             lightness;           /*!< Light Lightness Set/Light Lightness Set Unack */
    esp_ble_mesh_server_recv_light_lightness_linear_set_t      lightness_linear;    /*!< Light Lightness Linear Set/Light Lightness Linear Set Unack */
    esp_ble_mesh_server_recv_light_lightness_default_set_t     lightness_default;   /*!< Light Lightness Default Set/Light Lightness Default Set Unack */
    esp_ble_mesh_server_recv_light_lightness_range_set_t       lightness_range;     /*!< Light Lightness Range Set/Light Lightness Range Set Unack */
    esp_ble_mesh_server_recv_light_ctl_set_t                   ctl;                 /*!< Light CTL Set/Light CTL Set Unack */
    esp_ble_mesh_server_recv_light_ctl_temperature_set_t       ctl_temp;            /*!< Light CTL Temperature Set/Light CTL Temperature Set Unack */
    esp_ble_mesh_server_recv_light_ctl_temperature_range_set_t ctl_temp_range;      /*!< Light CTL Temperature Range Set/Light CTL Temperature Range Set Unack */
    esp_ble_mesh_server_recv_light_ctl_default_set_t           ctl_default;         /*!< Light CTL Default Set/Light CTL Default Set Unack */
    esp_ble_mesh_server_recv_light_hsl_set_t                   hsl;                 /*!< Light HSL Set/Light HSL Set Unack */
    esp_ble_mesh_server_recv_light_hsl_hue_set_t               hsl_hue;             /*!< Light HSL Hue Set/Light HSL Hue Set Unack */
    esp_ble_mesh_server_recv_light_hsl_saturation_set_t        hsl_saturation;      /*!< Light HSL Saturation Set/Light HSL Saturation Set Unack */
    esp_ble_mesh_server_recv_light_hsl_default_set_t           hsl_default;         /*!< Light HSL Default Set/Light HSL Default Set Unack */
    esp_ble_mesh_server_recv_light_hsl_range_set_t             hsl_range;           /*!< Light HSL Range Set/Light HSL Range Set Unack */
    esp_ble_mesh_server_recv_light_xyl_set_t                   xyl;                 /*!< Light xyL Set/Light xyL Set Unack */
    esp_ble_mesh_server_recv_light_xyl_default_set_t           xyl_default;         /*!< Light xyL Default Set/Light xyL Default Set Unack */
    esp_ble_mesh_server_recv_light_xyl_range_set_t             xyl_range;           /*!< Light xyL Range Set/Light xyL Range Set Unack */
    esp_ble_mesh_server_recv_light_lc_mode_set_t               lc_mode;             /*!< Light LC Mode Set/Light LC Mode Set Unack */
    esp_ble_mesh_server_recv_light_lc_om_set_t                 lc_om;               /*!< Light LC OM Set/Light LC OM Set Unack */
    esp_ble_mesh_server_recv_light_lc_light_onoff_set_t        lc_light_onoff;      /*!< Light LC Light OnOff Set/Light LC Light OnOff Set Unack */
    esp_ble_mesh_server_recv_light_lc_property_set_t           lc_property;         /*!< Light LC Property Set/Light LC Property Set Unack */
} esp_ble_mesh_lighting_server_recv_set_msg_t;

/** Context of the received Sensor Status message */
typedef struct {
    struct net_buf_simple *data;    /*!< Value of sensor data state (optional) */
} esp_ble_mesh_server_recv_sensor_status_t;

/**
 * @brief Lighting Server Model received status message union
 */
typedef union {
    esp_ble_mesh_server_recv_sensor_status_t sensor_status;     /*!< Sensor Status */
} esp_ble_mesh_lighting_server_recv_status_msg_t;

/**
 * @brief Lighting Server Model callback value union
 */
typedef union {
    esp_ble_mesh_lighting_server_state_change_t state_change;   /*!< ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT */
    esp_ble_mesh_lighting_server_recv_get_msg_t get;            /*!< ESP_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT */
    esp_ble_mesh_lighting_server_recv_set_msg_t set;            /*!< ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT */
    esp_ble_mesh_lighting_server_recv_status_msg_t status;      /*!< ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT */
} esp_ble_mesh_lighting_server_cb_value_t;

/** Lighting Server Model callback parameters */
typedef struct {
    esp_ble_mesh_model_t  *model;   /*!< Pointer to Lighting Server Models */
    esp_ble_mesh_msg_ctx_t ctx;     /*!< Context of the received messages */
    esp_ble_mesh_lighting_server_cb_value_t value;  /*!< Value of the received Lighting Messages */
} esp_ble_mesh_lighting_server_cb_param_t;

/** This enum value is the event of Lighting Server Model */
typedef enum {
    /**
     * 1. When get_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, no event will be
     *    callback to the application layer when Lighting Get messages are received.
     * 2. When set_auto_rsp is set to ESP_BLE_MESH_SERVER_AUTO_RSP, this event will
     *    be callback to the application layer when Lighting Set/Set Unack messages
     *    are received.
     */
    ESP_BLE_MESH_LIGHTING_SERVER_STATE_CHANGE_EVT,
    /**
     * When get_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will be
     * callback to the application layer when Lighting Get messages are received.
     */
    ESP_BLE_MESH_LIGHTING_SERVER_RECV_GET_MSG_EVT,
    /**
     * When set_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will be
     * callback to the application layer when Lighting Set/Set Unack messages are received.
     */
    ESP_BLE_MESH_LIGHTING_SERVER_RECV_SET_MSG_EVT,
    /**
     * When status_auto_rsp is set to ESP_BLE_MESH_SERVER_RSP_BY_APP, this event will
     * be callback to the application layer when Sensor Status message is received.
     */
    ESP_BLE_MESH_LIGHTING_SERVER_RECV_STATUS_MSG_EVT,
    ESP_BLE_MESH_LIGHTING_SERVER_EVT_MAX,
} esp_ble_mesh_lighting_server_cb_event_t;

/**
 *  @brief Bluetooth Mesh Lighting Server Model function.
 */

/**
 * @brief   Lighting Server Model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_lighting_server_cb_t)(esp_ble_mesh_lighting_server_cb_event_t event,
                                                   esp_ble_mesh_lighting_server_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Lighting Server Model callback.
 *
 * @param[in]   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_lighting_server_callback(esp_ble_mesh_lighting_server_cb_t callback);

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_LIGHTING_MODEL_API_H_ */
