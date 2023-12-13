/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include <stdint.h>
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_cpu.h"
#include "esp_intr_alloc.h"
#include "esp_debug_helpers.h"
#include "soc/periph_defs.h"
#include "hal/crosscore_int_ll.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#if CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
#include "esp_gdbstub.h"
#endif

#define REASON_YIELD            BIT(0)
#define REASON_FREQ_SWITCH      BIT(1)
#define REASON_PRINT_BACKTRACE  BIT(2)
#define REASON_GDB_CALL         BIT(3)
#define REASON_TWDT_ABORT       BIT(4)

static portMUX_TYPE reason_spinlock = portMUX_INITIALIZER_UNLOCKED;
static volatile uint32_t reason[CONFIG_FREERTOS_NUMBER_OF_CORES];

/*
ToDo: There is a small chance the CPU already has yielded when this ISR is serviced. In that case, it's running the intended task but
the ISR will cause it to switch _away_ from it. portYIELD_FROM_ISR will probably just schedule the task again, but have to check that.
*/
static inline void IRAM_ATTR esp_crosscore_isr_handle_yield(void)
{
    portYIELD_FROM_ISR();
}

static void IRAM_ATTR esp_crosscore_isr(void *arg)
{
    uint32_t my_reason_val;
    //A pointer to the correct reason array item is passed to this ISR.
    volatile uint32_t *my_reason = arg;

    //Clear the interrupt first.
    crosscore_int_ll_clear_interrupt(esp_cpu_get_core_id());

    //Grab the reason and clear it.
    portENTER_CRITICAL_ISR(&reason_spinlock);
    my_reason_val = *my_reason;
    *my_reason = 0;
    portEXIT_CRITICAL_ISR(&reason_spinlock);

    //Check what we need to do.
    if (my_reason_val & REASON_YIELD) {
        esp_crosscore_isr_handle_yield();
    }
    if (my_reason_val & REASON_FREQ_SWITCH) {
        /* Nothing to do here; the frequency switch event was already
         * handled by a hook in xtensa_vectors.S. Could be used in the future
         * to allow DFS features without the extra latency of the ISR hook.
         */
    }
#if CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
    if (my_reason_val & REASON_GDB_CALL) {
        update_breakpoints();
    }
#endif // !CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

    if (my_reason_val & REASON_PRINT_BACKTRACE) {
        esp_backtrace_print(100);
    }

#if CONFIG_ESP_TASK_WDT_EN
    if (my_reason_val & REASON_TWDT_ABORT) {
        extern void task_wdt_timeout_abort(bool);
        /* Called from a crosscore interrupt, thus, we are not the core that received
         * the TWDT interrupt, call the function with `false` as a parameter. */
        task_wdt_timeout_abort(false);
    }
#endif // CONFIG_ESP_TASK_WDT_EN

}

//Initialize the crosscore interrupt on this core. Call this once
//on each active core.
void esp_crosscore_int_init(void)
{
    portENTER_CRITICAL(&reason_spinlock);
    reason[esp_cpu_get_core_id()] = 0;
    portEXIT_CRITICAL(&reason_spinlock);
    esp_err_t err __attribute__((unused)) = ESP_OK;
#if CONFIG_FREERTOS_NUMBER_OF_CORES > 1
    if (esp_cpu_get_core_id() == 0) {
        err = esp_intr_alloc(ETS_FROM_CPU_INTR0_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[0], NULL);
    } else {
        err = esp_intr_alloc(ETS_FROM_CPU_INTR1_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[1], NULL);
    }
#else
    err = esp_intr_alloc(ETS_FROM_CPU_INTR0_SOURCE, ESP_INTR_FLAG_IRAM, esp_crosscore_isr, (void*)&reason[0], NULL);
#endif
    ESP_ERROR_CHECK(err);
}

static void IRAM_ATTR esp_crosscore_int_send(int core_id, uint32_t reason_mask)
{
    assert(core_id < CONFIG_FREERTOS_NUMBER_OF_CORES);
    //Mark the reason we interrupt the other CPU
    portENTER_CRITICAL_ISR(&reason_spinlock);
    reason[core_id] |= reason_mask;
    portEXIT_CRITICAL_ISR(&reason_spinlock);
    //Poke the other CPU.
    crosscore_int_ll_trigger_interrupt(core_id);
}

void IRAM_ATTR esp_crosscore_int_send_yield(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_YIELD);
}

void IRAM_ATTR esp_crosscore_int_send_freq_switch(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_FREQ_SWITCH);
}

void IRAM_ATTR esp_crosscore_int_send_gdb_call(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_GDB_CALL);
}

void IRAM_ATTR esp_crosscore_int_send_print_backtrace(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_PRINT_BACKTRACE);
}

#if CONFIG_ESP_TASK_WDT_EN
void IRAM_ATTR esp_crosscore_int_send_twdt_abort(int core_id)
{
    esp_crosscore_int_send(core_id, REASON_TWDT_ABORT);
}
#endif // CONFIG_ESP_TASK_WDT_EN
