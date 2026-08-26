/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef H_BLECSC_SENSOR_
#define H_BLECSC_SENSOR_

#include "modlog/modlog.h"
#include "nimble/ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Cycling Speed and Cadence configuration */
#define GATT_CSC_UUID                           0x1816
#define GATT_CSC_MEASUREMENT_UUID               0x2A5B
#define GATT_CSC_FEATURE_UUID                   0x2A5C
#define GATT_SENSOR_LOCATION_UUID               0x2A5D
#define GATT_SC_CONTROL_POINT_UUID              0x2A55
/* Device Information configuration */
#define GATT_DEVICE_INFO_UUID                   0x180A
#define GATT_MANUFACTURER_NAME_UUID             0x2A29
#define GATT_MODEL_NUMBER_UUID                  0x2A24

/*CSC Measurement flags*/
#define CSC_MEASUREMENT_WHEEL_REV_PRESENT       0x01
#define CSC_MEASUREMENT_CRANK_REV_PRESENT       0x02

/* CSC feature flags */
#define CSC_FEATURE_WHEEL_REV_DATA              0x01
#define CSC_FEATURE_CRANK_REV_DATA              0x02
#define CSC_FEATURE_MULTIPLE_SENSOR_LOC         0x04

/* Sensor location enum */
#define SENSOR_LOCATION_OTHER                   0
#define SENSOR_LOCATION_TOP_OF_SHOE             1
#define SENSOR_LOCATION_IN_SHOE                 2
#define SENSOR_LOCATION_HIP                     3
#define SENSOR_LOCATION_FRONT_WHEEL             4
#define SENSOR_LOCATION_LEFT_CRANK              5
#define SENSOR_LOCATION_RIGHT_CRANK             6
#define SENSOR_LOCATION_LEFT_PEDAL              7
#define SENSOR_LOCATION_RIGHT_PEDAL             8
#define SENSOR_LOCATION_FROT_HUB                9
#define SENSOR_LOCATION_REAR_DROPOUT            10
#define SENSOR_LOCATION_CHAINSTAY               11
#define SENSOR_LOCATION_REAR_WHEEL              12
#define SENSOR_LOCATION_REAR_HUB                13
#define SENSOR_LOCATION_CHEST                   14
#define SENSOR_LOCATION_SPIDER                  15
#define SENSOR_LOCATION_CHAIN_RING              16

/* SC Control Point op codes */
#define SC_CP_OP_SET_CUMULATIVE_VALUE           1
#define SC_CP_OP_START_SENSOR_CALIBRATION       2
#define SC_CP_OP_UPDATE_SENSOR_LOCATION         3
#define SC_CP_OP_REQ_SUPPORTED_SENSOR_LOCATIONS 4
#define SC_CP_OP_RESPONSE                       16

/*SC Control Point response values */
#define SC_CP_RESPONSE_SUCCESS                  1
#define SC_CP_RESPONSE_OP_NOT_SUPPORTED         2
#define SC_CP_RESPONSE_INVALID_PARAM            3
#define SC_CP_RESPONSE_OP_FAILED                4

/* CSC simulation configuration */
#define CSC_FEATURES                         (CSC_FEATURE_WHEEL_REV_DATA | \
                                              CSC_FEATURE_CRANK_REV_DATA |\
                                              CSC_FEATURE_MULTIPLE_SENSOR_LOC)

struct ble_csc_measurement_state {
    uint32_t cumulative_wheel_rev;
    uint16_t last_wheel_evt_time;
    uint16_t cumulative_crank_rev;
    uint16_t last_crank_evt_time;
};

extern uint16_t csc_measurement_handle;
extern uint16_t csc_control_point_handle;

int gatt_svr_init(struct ble_csc_measurement_state * csc_measurement_state);
int gatt_svr_chr_notify_csc_measurement(uint16_t conn_handle);
void gatt_svr_set_cp_indicate(uint8_t indication_status);

#ifdef __cplusplus
}
#endif

#endif
