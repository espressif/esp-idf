/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <time.h>
#include <string.h>
#include <inttypes.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "unity.h"
#include "utils/common.h"
#include "utils/eloop.h"
#include "common/ieee802_11_defs.h"
#include "esp_wifi_driver.h"
#include "memory_checks.h"
#include "test_wpa_supplicant_common.h"

static uint32_t timeouts_usec[6] = { 10000, 1000, 10000, 5000, 15000, 1000 };
static uint32_t timeouts_sec[6] = { 10, 1, 10, 5, 15, 1 };
static int executed_order[6];
static int t;
static struct os_reltime ts;


/* there is only single instance of esp_timer so no need of protection */
static void callback(void *a, void *b)
{
    int *i = a;
    struct os_time age, now;

    os_get_reltime(&now);
    os_time_sub(&now, &ts, &age);

    int32_t ms_diff = (age.sec - timeouts_sec[*i]) * 1000 +
                      (age.usec - timeouts_usec[*i]) / 1000;

    /* let's give 50 ms offset for this small block */
    if (ms_diff > 50) {
        executed_order[t] = -1;
    } else {
        executed_order[t] = *i;
    }
    t++;

    ESP_LOGI("Eloop Test", "timer[%d] ran after %" PRId32 " msec of scheduled time",
             *i, ms_diff);

}

extern const wifi_osi_funcs_t *wifi_funcs;
/* Check if eloop runs its timers correctly & in correct order */
TEST_CASE("Test eloop timers run", "[eloop]")
{
    set_leak_threshold(1000);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    cfg.nvs_enable = false;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    TEST_ESP_OK(esp_wifi_stop());
    TEST_ESP_OK(esp_wifi_deinit());
    /* Reset memory stats since some is leaked during the first initialization */
    test_utils_record_free_mem();

    int execution_order[6] = {1, 5, 3, 0, 2, 4};
    int index[6] = {0, 1, 2, 3, 4, 5};
    t = 0;

    /* We need pptask to run eloop, wifi init will do that */
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    os_get_reltime(&ts);
    for (int i = 0; i < 6; i++) {
        eloop_register_timeout(timeouts_sec[i], timeouts_usec[i],
                               callback, &index[i], NULL);
    }

    /* wait for all timers to run */
    os_sleep(20, 0);
    /* check the execution order, this will also check whether they were fired at correct time */
    TEST_ASSERT(memcmp(execution_order, executed_order, 6 * sizeof(int)) == 0);
    TEST_ESP_OK(esp_wifi_stop());
    TEST_ESP_OK(esp_wifi_deinit());
    os_sleep(3, 0);
}
