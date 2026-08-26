/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#define CYBERGEAR_CMD_BROADCAST                 0x00
#define CYBERGEAR_CMD_MOTION_CONTROL            0x01
#define CYBERGEAR_CMD_FEEDBACK_STATUS           0x02
#define CYBERGEAR_CMD_ENABLE                    0x03
#define CYBERGEAR_CMD_DISABLE                   0x04
#define CYBERGEAR_CMD_SET_LOCATION_TO_ZERO      0x06
#define CYBERGEAR_CMD_SET_MOTOR_ID              0x07
#define CYBERGEAR_CMD_READ_PARA                 0x11
#define CYBERGEAR_CMD_SET_PARA                  0x12
#define CYBERGEAR_CMD_FEEDBACK_FAULT            0x15

#define CYBERGEAR_ADDR_RUN_MODE                 0x7005
#define CYBERGEAR_ADDR_CURRENT_REF              0x7006
#define CYBERGEAR_ADDR_SPEED_REF                0x700A
#define CYBERGEAR_ADDR_LIMIT_TORQUE             0x700B
#define CYBERGEAR_ADDR_CURRENT_KP               0x7010
#define CYBERGEAR_ADDR_CURRENT_KI               0x7011
#define CYBERGEAR_ADDR_CURRENT_FILTER_GAIN      0x7014
#define CYBERGEAR_ADDR_LOCATION_REF             0x7016
#define CYBERGEAR_ADDR_LIMIT_SPEED              0x7017
#define CYBERGEAR_ADDR_LIMIT_CURRENT            0x7018

#define CYBERGEAR_LOCATION_MIN                 -12.5f
#define CYBERGEAR_LOCATION_MAX                  12.5f
#define CYBERGEAR_SPEED_MIN                    -30.0f
#define CYBERGEAR_SPEED_MAX                     30.0f
#define CYBERGEAR_KP_MIN                        0.0f
#define CYBERGEAR_KP_MAX                        500.0f
#define CYBERGEAR_KD_MIN                        0.0f
#define CYBERGEAR_KD_MAX                        5.0f
#define CYBERGEAR_TORQUE_MIN                   -12.0f
#define CYBERGEAR_TORQUE_MAX                    12.0f
#define CYBERGEAR_CURRENT_MIN                  -23.0f
#define CYBERGEAR_CURRENT_MAX                   23.0f
#define CYBERGEAR_CURRENT_FILTER_GAIN_MIN       0.0f
#define CYBERGEAR_CURRENT_FILTER_GAIN_MAX       1.0f

#define CYBERGEAR_MOTOR_TIMEOUT                 100

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CyberGear running mode
 */
typedef enum {
    MODE_MOTION,         /*!< Motion mode */
    MODE_LOCATION,       /*!< Location mode */
    MODE_SPEED,          /*!< Speed mode */
    MODE_CURRENT,        /*!< Current mode */
    MODE_MAX,            /*!< Mode count maximum */
} cybergear_mode_t;

/**
 * @brief CyberGear motion control parameters
 */
typedef struct {
    float torque;           /*!< Motion torque */
    float location;         /*!< Motion location */
    float speed;            /*!< Motion speed */
    float kp;               /*!< Motion proportional gain */
    float kd;               /*!< Motion derivative gain */
} cybergear_motion_control_t;

/**
 * @brief CyberGear status
 */
typedef struct {
    uint8_t motor_id;          /*!< Motor ID */
    uint8_t fault;             /*!< Fault status */
    uint8_t state;             /*!< Motor state */
    float location;            /*!< Location */
    float speed;               /*!< Feedback speed */
    float torque;              /*!< Feedback torque */
    float temperature;         /*!< Feedback temperature */
} cybergear_status_t;

/**
 * @brief CyberGear handle
 */
typedef struct cybergear_t *cybergear_handle_t;  /*!< CyberGear handle */

#ifdef __cplusplus
}
#endif
