/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_gpio.h"
#include "ulp_lp_core_interrupts.h"
#include "ulp_lp_core_print.h"
#include "riscv/csr.h"

#define DEBOUNCE_INTERVAL_CYCLES 10 // 10 cycles is about 0.625 us at 16 MHz

#define SIMULATED_PULSE_FREQUENCY_HZ 2
#define SIMULATED_PULSE_DELAY_US (1000000 / SIMULATED_PULSE_FREQUENCY_HZ) / 2

uint32_t pulse_count;
static uint32_t last_trigger_time_cycles;

void LP_CORE_ISR_ATTR ulp_lp_core_lp_io_intr_handler(void)
{
    ulp_lp_core_gpio_clear_intr_status();
    uint32_t trigger_time_cycles = RV_READ_CSR(mcycle);
    /* Do some simple debouncing, do not count spurious pulses */
    if (trigger_time_cycles - last_trigger_time_cycles > DEBOUNCE_INTERVAL_CYCLES) {
        pulse_count++;
        last_trigger_time_cycles = trigger_time_cycles;
    }

    if (pulse_count % CONFIG_EXAMPLE_PULSE_COUNT_WAKEUP_LIMIT == 0) {
        lp_core_printf("Pulse count: %d, wake-up main CPU\n", pulse_count);
        ulp_lp_core_wakeup_main_processor();
    }

}



int main (void)
{
    lp_core_printf("LP Core pulse counter started\n");
    ulp_lp_core_intr_enable();
    ulp_lp_core_gpio_intr_enable(CONFIG_EXAMPLE_PULSE_COUNT_PIN, GPIO_INTR_POSEDGE);

    while(1) {

#if CONFIG_EXAMPLE_PULSE_COUNT_SIMULATE
        /* No external device connected to generate pulses, we simulate them ourselves instead */
        ulp_lp_core_delay_us(SIMULATED_PULSE_DELAY_US);
        ulp_lp_core_gpio_set_level(CONFIG_EXAMPLE_PULSE_COUNT_PIN, 1);
        ulp_lp_core_delay_us(SIMULATED_PULSE_DELAY_US);
        ulp_lp_core_gpio_set_level(CONFIG_EXAMPLE_PULSE_COUNT_PIN, 0);
#else
        /* Put CPU into a wait state to reduce power consumption while waiting for pulses */
        ulp_lp_core_wait_for_intr();
#endif //CONFIG_EXAMPLE_PULSE_COUNT_SIMULATE
    }

    return 0;
}
