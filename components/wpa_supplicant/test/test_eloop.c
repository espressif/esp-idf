/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "string.h"
#include "esp_system.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "utils/common.h"
#include "utils/eloop.h"
#include "common/ieee802_11_defs.h"
#include "../esp_supplicant/src/esp_wifi_driver.h"
#include "esp_log.h"
#include "test_utils.h"
#include "memory_checks.h"
#include <time.h>

uint32_t timeouts_usec[6] = { 10000, 1000, 10000, 5000, 15000, 1000 };
uint32_t timeouts_sec[6] = { 10, 1, 10, 5, 15, 1 };
int executed_order[6];
int t;
struct os_reltime ts;


/* there is only single instance of esp_timer so no need of protection */
void callback(void *a, void *b)
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

	ESP_LOGI("Eloop Test", "timer[%d] ran after %d msec of scheduled time",
				*i, ms_diff);

}

extern const wifi_osi_funcs_t *wifi_funcs;
/* Check if eloop runs its timers correctly & in correct order */
TEST_CASE("Test eloop timers run", "[eloop]")
{
	int execution_order[6] = {1, 5, 3, 0, 2, 4};
	int index[6] = {0,1,2,3,4,5};

	wifi_funcs = WIFI_OSI_FUNCS_INITIALIZER();
	if (!wifi_funcs) {
		TEST_ASSERT(1);
	}
	eloop_init();
	os_get_reltime(&ts);
	for (int i = 0; i < 6; i++) {
		eloop_register_timeout(timeouts_sec[i], timeouts_usec[i],
			       callback, &index[i], NULL);
	}

	/* wait for all timers to run */
	os_sleep(20, 0);
	t = 0;
	/* check the execution order, this will also check whether they were fired at correct time */
	TEST_ASSERT(memcmp(execution_order, executed_order, 6*sizeof(int)) == 0);
	eloop_destroy();
}
