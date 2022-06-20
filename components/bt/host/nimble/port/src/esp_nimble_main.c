/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "sdkconfig.h"
#include "esp_nimble_mem.h"
#include "host/ble_hs.h"

static TaskHandle_t host_task_h;

extern void ble_hs_deinit(void);
static struct ble_hs_stop_listener stop_listener;

static struct ble_npl_eventq g_eventq_dflt;
static struct ble_npl_sem ble_hs_stop_sem;
static struct ble_npl_event ble_hs_ev_stop;

esp_err_t esp_nimble_init(void)
{
#if !SOC_ESP_NIMBLE_CONTROLLER
    /* Initialize the function pointers for OS porting */
    npl_freertos_funcs_init();

    npl_freertos_mempool_init();

    /* Initialize default event queue */

    ble_npl_eventq_init(&g_eventq_dflt);


    os_msys_init();

    void ble_store_ram_init(void); // Do we need this?
    /* XXX Need to have template for store */
    ble_store_ram_init();
#endif

    /* Initialize the host */
    ble_hs_init();
    return ESP_OK;
}

esp_err_t esp_nimble_enable(void *host_task)
{
    /*
     * Create task where NimBLE host will run. It is not strictly necessary to
     * have separate task for NimBLE host, but since something needs to handle
     * default queue it is just easier to make separate task which does this.
     */
    xTaskCreatePinnedToCore(host_task, "nimble_host", NIMBLE_HS_STACK_SIZE,
                            NULL, (configMAX_PRIORITIES - 4), &host_task_h, NIMBLE_CORE);
    return ESP_OK;

}

esp_err_t esp_nimble_disable(void)
{
    esp_err_t err = ESP_OK;
    ble_npl_sem_init(&ble_hs_stop_sem, 0);

    /* Initiate a host stop procedure. */
    rc = ble_hs_stop(&stop_listener, ble_hs_stop_cb,
                     NULL);
    if (rc != 0) {
        ble_npl_sem_deinit(&ble_hs_stop_sem);
        return rc;
    }

    /* Wait till the host stop procedure is complete */
    ble_npl_sem_pend(&ble_hs_stop_sem, BLE_NPL_TIME_FOREVER);

    ble_npl_event_init(&ble_hs_ev_stop, nimble_port_stop_cb,
                       NULL);
    ble_npl_eventq_put(&g_eventq_dflt, &ble_hs_ev_stop);

    /* Wait till the event is serviced */
    ble_npl_sem_pend(&ble_hs_stop_sem, BLE_NPL_TIME_FOREVER);

    ble_npl_sem_deinit(&ble_hs_stop_sem);

    return ESP_OK;

}

esp_err_t esp_nimble_deinit(void)
{
#if !(SOC_ESP_NIMBLE_CONTROLLER && CONFIG_BT_CONTROLLER_ENABLED)
    ble_npl_eventq_deinit(&g_eventq_dflt);
#endif

    if (host_task_h) {
        vTaskDelete(host_task_h);
    }
    ble_hs_deinit();
    return ESP_OK;
}
