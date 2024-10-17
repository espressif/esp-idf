/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_debug_helpers.h"
#include "esp_private/panic_reason.h"
#include "esp_private/panic_internal.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_private/freertos_debug.h"
#include "esp_err.h"
#include "esp_attr.h"
#include "esp_private/esp_cpu_internal.h"
#include <string.h>

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
#include "esp_private/eh_frame_parser.h"
#endif // CONFIG_ESP_SYSTEM_USE_EH_FRAME

#if !CONFIG_ESP_SYSTEM_USE_EH_FRAME
/* Function used to print all the registers pointed by the given frame .*/
extern void panic_print_registers(const void *frame, int core);
#endif // !CONFIG_ESP_SYSTEM_USE_EH_FRAME

/* Targets based on a RISC-V CPU cannot perform backtracing that easily.
    * We have two options here:
    *     - Perform backtracing at runtime.
    *     - Let IDF monitor do the backtracing for us. Used during panic already.
    * This could be configurable, choosing one or the other depending on
    * CONFIG_ESP_SYSTEM_USE_EH_FRAME configuration option.
    *
    * In both cases, this takes time, and we might be in an ISR, we must
    * exit this handler as fast as possible, then we will simply print
    * the interruptee's registers.
    */
esp_err_t IRAM_ATTR esp_backtrace_print(int depth)
{
    (void)depth;

    const int current_core = xPortGetCoreID();

    TaskSnapshot_t snapshot = { 0 };
    BaseType_t ret = vTaskGetSnapshot(xTaskGetCurrentTaskHandleForCore(current_core), &snapshot);

    if (ret != pdTRUE) {
        return ESP_ERR_NOT_FOUND;
    }

    void *frame = snapshot.pxTopOfStack;

    esp_cpu_frame_t backtrace_frame = {};
    memcpy(&backtrace_frame, frame, sizeof(esp_cpu_frame_t));

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
    esp_rom_printf("esp_backtrace_print: Print CPU %d (current core) backtrace\n", current_core);
    esp_eh_frame_print_backtrace(frame);
#else // CONFIG_ESP_SYSTEM_USE_EH_FRAME
    esp_rom_printf("esp_backtrace_print: Print CPU %d (current core) registers\n", current_core);
    panic_prepare_frame_from_ctx(&backtrace_frame);

    panic_print_registers(&backtrace_frame, current_core);
    esp_rom_printf("\r\n");
#endif // CONFIG_ESP_SYSTEM_USE_EH_FRAME

    return ESP_OK;
}
