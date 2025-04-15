
/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_cpu.h"
#include "esp_private/panic_internal.h"
#include "esp_rom_sys.h"

extern volatile uint32_t g_panic_handler_stuck;
extern volatile uint32_t g_panic_handler_crash;
extern void __real_esp_panic_handler(panic_info_t *info);

void __wrap_esp_panic_handler(panic_info_t *info)
{
    if (g_panic_handler_stuck) {
        /* Override the panic handler for the panic handler stuck test */

        /* Stall the other core */
        int core_id = esp_cpu_get_core_id();
        for (uint32_t i = 0; i < SOC_CPU_CORES_NUM; i++) {
            if (i != core_id) {
                esp_cpu_stall(i);
            }
        }

        /* Get the panic handler to loop */
        esp_rom_printf("Panic handler stuck\n");
        while (1) {
            ;
        }
    } else if (g_panic_handler_crash) {
        /* Override the panic handler for the panic handler crash test */

        static int crash_count = 0;
        esp_rom_printf("Panic handler crashed %d times\n", ++crash_count);

        /* We just return from the panic handler to cause a crash again
         * since the offending program counter is not reset. Forcing
         * a crash here via another illegal instruction can cause some
         * RISC-V cores (such as esp32c5, esp32c61) to take evasive action
         * due to a CPU LOCKUP type exception and reboot without the panic
         * handler being called again.
         */
        return;
    } else {
        /* Fall back to the default panic handler for all other tests */
        __real_esp_panic_handler(info);
    }
}
