// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "esp_system.h"
#include "esp_heap_caps_init.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "esp_task.h"
#include "esp_private/crosscore_int.h"
#include "esp_private/startup_internal.h"
#include "esp_log.h"
#include "soc/dport_access.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/spiram.h"
#elif CONFIG_IDF_TARGET_ESP32C3
// SPIRAM is not supported on ESP32-C3
#endif

#if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
static const char* TAG = "cpu_start";
#endif

/* Architecture-agnostic parts of the FreeRTOS ESP-IDF port layer can go here.
 *
 * The actual call flow will be to call esp_startup_start_app() in <ARCH>/port.c,
 * which will then call esp_startup_start_app_common()
 */

// Duplicate of inaccessible xSchedulerRunning; needed at startup to avoid counting nesting
volatile unsigned port_xSchedulerRunning[portNUM_PROCESSORS] = {0};

// For now, running FreeRTOS on one core and a bare metal on the other (or other OSes)
// is not supported. For now CONFIG_FREERTOS_UNICORE and CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
// should mirror each other's values.
//
// And since this should be true, we can just check for CONFIG_FREERTOS_UNICORE.
#if CONFIG_FREERTOS_UNICORE != CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
	#error "FreeRTOS and system configuration mismatch regarding the use of multiple cores."
#endif

static void main_task(void* args);

extern void app_main(void);

void esp_startup_start_app_common(void)
{
#if CONFIG_ESP_INT_WDT
	esp_int_wdt_init();
	//Initialize the interrupt watch dog for CPU0.
	esp_int_wdt_cpu_init();
#endif

	esp_crosscore_int_init();

#ifndef CONFIG_FREERTOS_UNICORE
#if CONFIG_IDF_TARGET_ESP32
	esp_dport_access_int_init();
#endif
#endif

	portBASE_TYPE res = xTaskCreatePinnedToCore(&main_task, "main",
												ESP_TASK_MAIN_STACK, NULL,
												ESP_TASK_MAIN_PRIO, NULL, 0);
	assert(res == pdTRUE);
}

static void main_task(void* args)
{
#if !CONFIG_FREERTOS_UNICORE
	// Wait for FreeRTOS initialization to finish on APP CPU, before replacing its startup stack
	while (port_xSchedulerRunning[1] == 0) {
		;
	}
#endif

	// [refactor-todo] check if there is a way to move the following block to esp_system startup
	heap_caps_enable_nonos_stack_heaps();

	// Now we have startup stack RAM available for heap, enable any DMA pool memory
#if CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL
	if (g_spiram_ok) {
		esp_err_t r = esp_spiram_reserve_dma_pool(CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL);
		if (r != ESP_OK) {
			ESP_EARLY_LOGE(TAG, "Could not reserve internal/DMA pool (error 0x%x)", r);
			abort();
		}
	}
#endif

	//Initialize task wdt if configured to do so
#ifdef CONFIG_ESP_TASK_WDT_PANIC
	ESP_ERROR_CHECK(esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S, true));
#elif CONFIG_ESP_TASK_WDT
	ESP_ERROR_CHECK(esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S, false));
#endif

	//Add IDLE 0 to task wdt
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
	TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
	if(idle_0 != NULL){
		ESP_ERROR_CHECK(esp_task_wdt_add(idle_0));
	}
#endif
	//Add IDLE 1 to task wdt
#ifdef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
	TaskHandle_t idle_1 = xTaskGetIdleTaskHandleForCPU(1);
	if(idle_1 != NULL){
		ESP_ERROR_CHECK(esp_task_wdt_add(idle_1));
	}
#endif

	app_main();
	vTaskDelete(NULL);
}
