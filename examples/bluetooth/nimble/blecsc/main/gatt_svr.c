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

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "blecsc_sens.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "services/ans/ble_svc_ans.h"

#define CSC_ERR_CCC_DESC_IMPROPERLY_CONFIGURED  0x81

static const char *manuf_name = "Apache Mynewt";
static const char *model_num = "Mynewt CSC Sensor";

static const uint8_t csc_supported_sensor_locations[] = {
    SENSOR_LOCATION_FRONT_WHEEL,
    SENSOR_LOCATION_REAR_DROPOUT,
    SENSOR_LOCATION_CHAINSTAY,
    SENSOR_LOCATION_REAR_WHEEL
};

static uint8_t sensor_location = SENSOR_LOCATION_REAR_DROPOUT;
static struct ble_csc_measurement_state * measurement_state;
uint16_t csc_measurement_handle;
uint16_t csc_control_point_handle;
uint8_t csc_cp_indication_status;

static int
gatt_svr_chr_access_csc_measurement(uint16_t conn_handle,
                                    uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt,
                                    void *arg);

static int
gatt_svr_chr_access_csc_feature(uint16_t conn_handle,
                                uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt,
                                void *arg);

static int
gatt_svr_chr_access_sensor_location(uint16_t conn_handle,
                                    uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt,
                                    void *arg);

static int
gatt_svr_chr_access_sc_control_point(uint16_t conn_handle,
                                     uint16_t attr_handle,
                                     struct ble_gatt_access_ctxt *ctxt,
                                     void *arg);

static int
gatt_svr_chr_access_device_info(uint16_t conn_handle,
                                uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt,
                                void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: Cycling Speed and Cadence */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_CSC_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: Cycling Speed and Cadence Measurement */
            .uuid = BLE_UUID16_DECLARE(GATT_CSC_MEASUREMENT_UUID),
            .access_cb = gatt_svr_chr_access_csc_measurement,
            .val_handle = &csc_measurement_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY,
        }, {
            /* Characteristic: Cycling Speed and Cadence features */
            .uuid = BLE_UUID16_DECLARE(GATT_CSC_FEATURE_UUID),
            .access_cb = gatt_svr_chr_access_csc_feature,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            /* Characteristic: Sensor Location */
            .uuid = BLE_UUID16_DECLARE(GATT_SENSOR_LOCATION_UUID),
            .access_cb = gatt_svr_chr_access_sensor_location,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            /* Characteristic: SC Control Point*/
            .uuid = BLE_UUID16_DECLARE(GATT_SC_CONTROL_POINT_UUID),
            .access_cb = gatt_svr_chr_access_sc_control_point,
            .val_handle = &csc_control_point_handle,
            .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_INDICATE,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },

    {
        /* Service: Device Information */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            /* Characteristic: * Manufacturer name */
            .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
            .access_cb = gatt_svr_chr_access_device_info,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            /* Characteristic: Model number string */
            .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
            .access_cb = gatt_svr_chr_access_device_info,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* No more characteristics in this service */
        }, }
    },

    {
        0, /* No more services */
    },
};

static int
gatt_svr_chr_access_csc_measurement(uint16_t conn_handle, uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    return BLE_ATT_ERR_READ_NOT_PERMITTED;
}

static int
gatt_svr_chr_access_csc_feature(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    static const uint16_t csc_feature = CSC_FEATURES;
    int rc;

    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);
    rc = os_mbuf_append(ctxt->om, &csc_feature, sizeof(csc_feature));

    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int
gatt_svr_chr_access_sensor_location(uint16_t conn_handle, uint16_t attr_handle,
                                  struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    int rc;

    assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);
    rc = os_mbuf_append(ctxt->om, &sensor_location, sizeof(sensor_location));

    return (rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int
gatt_svr_chr_access_sc_control_point(uint16_t conn_handle,
                                     uint16_t attr_handle,
                                     struct ble_gatt_access_ctxt *ctxt,
                                     void *arg)
{
    uint8_t op_code;
    uint8_t new_sensor_location;
    uint8_t new_cumulative_wheel_rev_arr[4];
    struct os_mbuf *om_indication;
    uint8_t response = SC_CP_RESPONSE_OP_NOT_SUPPORTED;
    int ii;
    int rc;

    assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);

    if (!csc_cp_indication_status) {
        return CSC_ERR_CCC_DESC_IMPROPERLY_CONFIGURED;
    }

    /* Read control point op code*/
    rc = os_mbuf_copydata(ctxt->om, 0, sizeof(op_code), &op_code);
    if (rc != 0){
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    /* Allocate response buffer */
    om_indication = ble_hs_mbuf_att_pkt();
    if (om_indication == NULL) {
        return BLE_HS_ENOMEM;
    }

    switch(op_code){
#if (CSC_FEATURES & CSC_FEATURE_WHEEL_REV_DATA)
    case SC_CP_OP_SET_CUMULATIVE_VALUE:
        /* Read new cumulative wheel revolutions value*/
        rc = os_mbuf_copydata(ctxt->om, 1,
                              sizeof(new_cumulative_wheel_rev_arr),
                              new_cumulative_wheel_rev_arr);
        if (rc != 0){
            os_mbuf_free_chain(om_indication);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }

        measurement_state->cumulative_wheel_rev =
                           get_le32(new_cumulative_wheel_rev_arr);


        response = SC_CP_RESPONSE_SUCCESS;
        break;
#endif

#if (CSC_FEATURES & CSC_FEATURE_MULTIPLE_SENSOR_LOC)
    case SC_CP_OP_UPDATE_SENSOR_LOCATION:
        /* Read new sensor location value*/
        rc = os_mbuf_copydata(ctxt->om, 1, 1, &new_sensor_location);
        if (rc != 0){
            os_mbuf_free_chain(om_indication);
            return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
        }


        /* Verify if requested new location is on supported locations list */
        response = SC_CP_RESPONSE_INVALID_PARAM;
        for (ii = 0; ii < sizeof(csc_supported_sensor_locations); ii++){
            if (new_sensor_location == csc_supported_sensor_locations[ii]){
                sensor_location = new_sensor_location;
                response = SC_CP_RESPONSE_SUCCESS;
                break;
            }
        }
        break;

    case SC_CP_OP_REQ_SUPPORTED_SENSOR_LOCATIONS:
        response = SC_CP_RESPONSE_SUCCESS;
        break;
#endif

    default:
        break;
    }

    /* Append response value */
    rc = os_mbuf_append(om_indication, &response, sizeof(response));

    if (rc != 0){
        os_mbuf_free_chain(om_indication);
        return BLE_ATT_ERR_INSUFFICIENT_RES;
    }

#if (CSC_FEATURES & CSC_FEATURE_MULTIPLE_SENSOR_LOC)
    /* In case of supported locations request append locations list */
    if (op_code == SC_CP_OP_REQ_SUPPORTED_SENSOR_LOCATIONS){
      rc = os_mbuf_append(om_indication, &csc_supported_sensor_locations,
                          sizeof(csc_supported_sensor_locations));
      if (rc != 0){
          os_mbuf_free_chain(om_indication);
          return BLE_ATT_ERR_INSUFFICIENT_RES;
      }
    }
#endif

    rc = ble_gatts_indicate_custom(conn_handle, csc_control_point_handle,
                                   om_indication);
    if (rc != 0) {
        goto done;
    }

    return rc;

done:
    os_mbuf_free_chain(om_indication);
    return rc;
}

static int
gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == GATT_MODEL_NUMBER_UUID) {
        rc = os_mbuf_append(ctxt->om, model_num, strlen(model_num));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (uuid == GATT_MANUFACTURER_NAME_UUID) {
        rc = os_mbuf_append(ctxt->om, manuf_name, strlen(manuf_name));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

int
gatt_svr_chr_notify_csc_measurement(uint16_t conn_handle)
{
    int rc;
    struct os_mbuf *om;
    uint8_t data_buf[11];
    uint8_t data_offset = 1;

    memset(data_buf, 0, sizeof(data_buf));

#if (CSC_FEATURES & CSC_FEATURE_WHEEL_REV_DATA)
    data_buf[0] |= CSC_MEASUREMENT_WHEEL_REV_PRESENT;
    put_le16(&(data_buf[5]), measurement_state->last_wheel_evt_time);
    put_le32(&(data_buf[1]), measurement_state->cumulative_wheel_rev);
    data_offset += 6;
#endif

#if (CSC_FEATURES & CSC_FEATURE_CRANK_REV_DATA)
    data_buf[0] |= CSC_MEASUREMENT_CRANK_REV_PRESENT;
    put_le16(&(data_buf[data_offset]),
             measurement_state->cumulative_crank_rev);
    put_le16(&(data_buf[data_offset + 2]),
             measurement_state->last_crank_evt_time);
    data_offset += 4;
#endif

    om = ble_hs_mbuf_from_flat(data_buf, data_offset);

    rc = ble_gatts_notify_custom(conn_handle, csc_measurement_handle, om);
    return rc;
}

void
gatt_svr_set_cp_indicate(uint8_t indication_status)
{
  csc_cp_indication_status = indication_status;
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        break;

    case BLE_GATT_REGISTER_OP_DSC:

        break;

    default:
        assert(0);
        break;
    }
}

int
gatt_svr_init(struct ble_csc_measurement_state * csc_measurement_state)
{
    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    measurement_state = csc_measurement_state;

    return 0;
}
