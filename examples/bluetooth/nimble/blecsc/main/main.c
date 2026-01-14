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


#include "esp_log.h"
#include "nvs_flash.h"
/* BLE */
#include "console/console.h"
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "blecsc_sens.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#if CONFIG_EXAMPLE_EXTENDED_ADV
static uint8_t ext_adv_pattern_1[] = {
    0x02, BLE_HS_ADV_TYPE_FLAGS, 0x06,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0xab, 0xcd,
    0x03, BLE_HS_ADV_TYPE_COMP_UUIDS16, 0x18, 0x11,
    0x10, BLE_HS_ADV_TYPE_COMP_NAME, 'n', 'i', 'm', 'b', 'l', 'e', '-', 'b', 'l', 'e', 'c', 's', 'c','-', 'e',
};
#endif

/* Wheel size for simulation calculations */
#define CSC_SIM_WHEEL_CIRCUMFERENCE_MM            2000
/* Simulated cadence lower limit */
#define CSC_SIM_CRANK_RPM_MIN                     20
/* Simulated cadence upper limit */
#define CSC_SIM_CRANK_RPM_MAX                     100
/* Simulated speed lower limit */
#define CSC_SIM_SPEED_KPH_MIN                     0
/* Simulated speed upper limit */
#define CSC_SIM_SPEED_KPH_MAX                     35

/* Notification status */
static bool notify_state = false;

/* Connection handle */
static uint16_t conn_handle;

static uint8_t blecsc_addr_type;

/* Advertised device name  */
static const char *device_name = "blecsc_sensor";

/* Measurement and notification timer */
static struct ble_npl_callout blecsc_measure_timer;

/* Variable holds current CSC measurement state */
static struct ble_csc_measurement_state csc_measurement_state;

/* Variable holds simulated speed (kilometers per hour) */
static uint16_t csc_sim_speed_kph = CSC_SIM_SPEED_KPH_MIN;

/* Variable holds simulated cadence (RPM) */
static uint8_t csc_sim_crank_rpm = CSC_SIM_CRANK_RPM_MIN;

static int blecsc_gap_event(struct ble_gap_event *event, void *arg);

static const char *tag = "NimBLE_BLE_CSC";

/*
 * Enables advertising with parameters:
 *     o General discoverable mode
 *     o Undirected connectable mode
 */
static void
blecsc_advertise(void)
{

    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info)
     *     o Advertising tx power
     *     o Device name
     */
    memset(&fields, 0, sizeof(fields));

    /*
     * Advertise two flags:
     *      o Discoverability in forthcoming advertisement (general)
     *      o BLE-only (BR/EDR unsupported)
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    /*
     * Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;

    /*
     * Set appearance.
     */
    fields.appearance = ble_svc_gap_device_appearance();
    fields.appearance_is_present = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    /* Begin advertising */
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(blecsc_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, blecsc_gap_event, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}


/* Update simulated CSC measurements.
 * Each call increments wheel and crank revolution counters by one and
 * computes last event time in order to match simulated candence and speed.
 * Last event time is expressedd in 1/1024th of second units.
 *
 *                 60 * 1024
 * crank_dt =    --------------
 *                cadence[RPM]
 *
 *
 *                circumference[mm] * 1024 * 60 * 60
 * wheel_dt =    -------------------------------------
 *                         10^6 * speed [kph]
 */
static void
blecsc_simulate_speed_and_cadence(void)
{

    uint16_t wheel_rev_period;
    uint16_t crank_rev_period;

    /* Update simulated crank and wheel rotation speed */
    csc_sim_speed_kph++;
    if (csc_sim_speed_kph >= CSC_SIM_SPEED_KPH_MAX) {
         csc_sim_speed_kph = CSC_SIM_SPEED_KPH_MIN;
    }

    csc_sim_crank_rpm++;
    if (csc_sim_crank_rpm >= CSC_SIM_CRANK_RPM_MAX) {
         csc_sim_crank_rpm = CSC_SIM_CRANK_RPM_MIN;
    }

    /* Calculate simulated measurement values */
    if (csc_sim_speed_kph > 0){
        wheel_rev_period = (36*64*CSC_SIM_WHEEL_CIRCUMFERENCE_MM) /
                           (625*csc_sim_speed_kph);
        csc_measurement_state.cumulative_wheel_rev++;
        csc_measurement_state.last_wheel_evt_time += wheel_rev_period;
    }

    if (csc_sim_crank_rpm > 0){
        crank_rev_period = (60*1024) / csc_sim_crank_rpm;
        csc_measurement_state.cumulative_crank_rev++;
        csc_measurement_state.last_crank_evt_time += crank_rev_period;
    }

    MODLOG_DFLT(INFO, "CSC simulated values: speed = %d kph, cadence = %d \n",
                csc_sim_speed_kph, csc_sim_crank_rpm);
}

/* Run CSC measurement simulation and notify it to the client */
static void
blecsc_measurement(struct ble_npl_event *ev)
{
    int rc;

    rc = ble_npl_callout_reset(&blecsc_measure_timer, portTICK_PERIOD_MS * 10);
    assert(rc == 0);

    blecsc_simulate_speed_and_cadence();

    if (notify_state) {
        rc = gatt_svr_chr_notify_csc_measurement(conn_handle);
        assert(rc == 0);
    }
}

static int
blecsc_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed */
        MODLOG_DFLT(INFO, "connection %s; status=%d\n",
                    event->connect.status == 0 ? "established" : "failed",
                    event->connect.status);

        if (event->connect.status != 0) {
            /* Connection failed; resume advertising */
            blecsc_advertise();
            conn_handle = 0;
        }
        else {
          conn_handle = event->connect.conn_handle;
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        MODLOG_DFLT(INFO, "disconnect; reason=%d\n", event->disconnect.reason);
        conn_handle = 0;
        /* Connection terminated; resume advertising */
        blecsc_advertise();
        break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        MODLOG_DFLT(INFO, "adv complete\n");
        break;

    case BLE_GAP_EVENT_SUBSCRIBE:
        MODLOG_DFLT(INFO, "subscribe event attr_handle=%d\n",
                    event->subscribe.attr_handle);

        if (event->subscribe.attr_handle == csc_measurement_handle) {
            notify_state = event->subscribe.cur_notify;
            MODLOG_DFLT(INFO, "csc measurement notify state = %d\n",
                        notify_state);
        }
        else if (event->subscribe.attr_handle == csc_control_point_handle) {
            gatt_svr_set_cp_indicate(event->subscribe.cur_indicate);
            MODLOG_DFLT(INFO, "csc control point indicate state = %d\n",
                        event->subscribe.cur_indicate);
        }
        break;

    case BLE_GAP_EVENT_MTU:
        MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d mtu=%d\n",
                    event->mtu.conn_handle,
                    event->mtu.value);
        break;

    }

    return 0;
}

static void
blecsc_on_sync(void)
{
    int rc;

    /* Figure out address to use while advertising (no privacy) */
    rc = ble_hs_id_infer_auto(0, &blecsc_addr_type);
    assert(rc == 0);

    /* Begin advertising */
    blecsc_advertise();
}

void blecsc_host_task(void *param)
{
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    nimble_port_freertos_deinit();
}
/*
 * main
 *
 * The main task for the project. This function initializes the packages,
 * then starts serving events from default event queue.
 *
 * @return int NOTE: this function should never return!
 */
int
app_main(void)
{
    int rc;

	 /* Initialize NVS — it is used to store PHY calibration data */
	 esp_err_t ret = nvs_flash_init();
	 if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	     ESP_ERROR_CHECK(nvs_flash_erase());
	     ret = nvs_flash_init();
	 }
	 ESP_ERROR_CHECK(ret);

	 ret = nimble_port_init();
	 if (ret != ESP_OK) {
	     ESP_LOGE(tag, "Failed to init nimble %d ", ret);
	     return -1;
	 }

    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.sync_cb = blecsc_on_sync;

    /* Initialize measurement and notification timer */
    ble_npl_callout_init(&blecsc_measure_timer, nimble_port_get_dflt_eventq(),
                    blecsc_measurement, NULL);
    rc = ble_npl_callout_reset(&blecsc_measure_timer, portTICK_PERIOD_MS * 100);
    assert(rc == 0);

#if MYNEWT_VAL(BLE_GATTS)
    rc = gatt_svr_init(&csc_measurement_state);
    assert(rc == 0);

    /* Set the default device name */
    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);
#endif

    nimble_port_freertos_init(blecsc_host_task);

    return 0;
}
