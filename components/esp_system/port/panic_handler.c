/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>

#include "spi_flash_mmap.h"
#include "esp_ipc_isr.h"
#include "esp_private/system_internal.h"
#include "esp_private/cache_utils.h"

#include "soc/soc_memory_layout.h"
#include "esp_cpu.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"

#include "hal/soc_hal.h"

#include "esp_private/cache_err_int.h"

#include "sdkconfig.h"
#include "esp_rom_sys.h"

#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#ifdef CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/memprot.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/memprot.h"
#else
#include "esp_memprot.h"
#endif
#endif

#include "esp_private/panic_internal.h"
#include "esp_private/panic_reason.h"

#include "hal/wdt_types.h"
#include "hal/wdt_hal.h"

extern int _invalid_pc_placeholder;

extern void esp_panic_handler_reconfigure_wdts(void);

extern void esp_panic_handler(panic_info_t *);

static wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};

void *g_exc_frames[SOC_CPU_CORES_NUM] = {NULL};

/*
  Panic handlers; these get called when an unhandled exception occurs or the assembly-level
  task switching / interrupt code runs into an unrecoverable error. The default task stack
  overflow handler and abort handler are also in here.
*/

/*
  Note: The linker script will put everything in this file in IRAM/DRAM, so it also works with flash cache disabled.
*/
static void print_state_for_core(const void *f, int core)
{
    /* On Xtensa (with Window ABI), register dump is not required for backtracing.
     * Don't print it on abort to reduce clutter.
     * On other architectures, register values need to be known for backtracing.
     */
#if (CONFIG_IDF_TARGET_ARCH_XTENSA && defined(XCHAL_HAVE_WINDOWED)) || \
    (CONFIG_IDF_TARGET_ARCH_RISCV && CONFIG_ESP_SYSTEM_USE_EH_FRAME)
    if (!g_panic_abort) {
#else
    if (true) {
#endif
        panic_print_registers(f, core);
        panic_print_str("\r\n");
    }
    panic_print_backtrace(f, core);
}

static void print_state(const void *f)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    int err_core = f == g_exc_frames[0] ? 0 : 1;
#else
    int err_core = 0;
#endif

    print_state_for_core(f, err_core);

    panic_print_str("\r\n");

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // If there are other frame info, print them as well
    for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
        // `f` is the frame for the offending core, see note above.
        if (err_core != i && g_exc_frames[i] != NULL) {
            print_state_for_core(g_exc_frames[i], i);
            panic_print_str("\r\n");
        }
    }
#endif
}

static void frame_to_panic_info(void *frame, panic_info_t *info, bool pseudo_excause)
{
    info->core = esp_cpu_get_core_id();
    info->exception = PANIC_EXCEPTION_FAULT;
    info->details = NULL;
    info->reason = "Unknown";
    info->pseudo_excause = pseudo_excause;

    if (pseudo_excause) {
        panic_soc_fill_info(frame, info);
    } else {
        panic_arch_fill_info(frame, info);
    }

    info->state = print_state;
    info->frame = frame;
}

static void panic_handler(void *frame, bool pseudo_excause)
{
    panic_info_t info = { 0 };

    /*
     * Setup environment and perform necessary architecture/chip specific
     * steps here prior to the system panic handler.
     * */
    int core_id = esp_cpu_get_core_id();

    // If multiple cores arrive at panic handler, save frames for all of them
    g_exc_frames[core_id] = frame;

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // These are cases where both CPUs both go into panic handler. The following code ensures
    // only one core proceeds to the system panic handler.
    if (pseudo_excause) {
#define BUSY_WAIT_IF_TRUE(b)                { if (b) while(1); }
        // For WDT expiry, pause the non-offending core - offending core handles panic
        BUSY_WAIT_IF_TRUE(panic_get_cause(frame) == PANIC_RSN_INTWDT_CPU0 && core_id == 1);
        BUSY_WAIT_IF_TRUE(panic_get_cause(frame) == PANIC_RSN_INTWDT_CPU1 && core_id == 0);

        // For cache error, pause the non-offending core - offending core handles panic
        if (panic_get_cause(frame) == PANIC_RSN_CACHEERR && core_id != esp_cache_err_get_cpuid()) {
            // Only print the backtrace for the offending core in case of the cache error
            g_exc_frames[core_id] = NULL;
            while (1) {
                ;
            }
        }
    }

    // Need to reconfigure WDTs before we stall any other CPU
    esp_panic_handler_reconfigure_wdts();

    esp_rom_delay_us(1);
    // Stall all other cores
    for (uint32_t i = 0; i < SOC_CPU_CORES_NUM; i++) {
        if (i != core_id) {
            esp_cpu_stall(i);
        }
    }
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE

    esp_ipc_isr_stall_abort();

    if (esp_cpu_dbgr_is_attached()) {
#if __XTENSA__
        if (!(esp_ptr_executable(esp_cpu_pc_to_addr(panic_get_address(frame))) && (panic_get_address(frame) & 0xC0000000U))) {
            /* Xtensa ABI sets the 2 MSBs of the PC according to the windowed call size
             * Incase the PC is invalid, GDB will fail to translate addresses to function names
             * Hence replacing the PC to a placeholder address in case of invalid PC
             */
            panic_set_address(frame, (uint32_t)&_invalid_pc_placeholder);
        }
#endif
        if (panic_get_cause(frame) == PANIC_RSN_INTWDT_CPU0
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
                || panic_get_cause(frame) == PANIC_RSN_INTWDT_CPU1
#endif
           ) {
            wdt_hal_write_protect_disable(&wdt0_context);
            wdt_hal_handle_intr(&wdt0_context);
            wdt_hal_write_protect_enable(&wdt0_context);
        }
    }

    // Convert architecture exception frame into abstracted panic info
    frame_to_panic_info(frame, &info, pseudo_excause);

    // Call the system panic handler
    esp_panic_handler(&info);
}

/**
 * This function must always be in IRAM as it is required to
 * re-enable the flash cache.
 */
static void IRAM_ATTR panic_enable_cache(void)
{
    int core_id = esp_cpu_get_core_id();

    if (!spi_flash_cache_enabled()) {
        esp_ipc_isr_stall_abort();
        spi_flash_enable_cache(core_id);
    }
}

void IRAM_ATTR panicHandler(void *frame)
{

    panic_enable_cache();
    // This panic handler gets called for when the double exception vector,
    // kernel exception vector gets used; as well as handling interrupt-based
    // faults cache error, wdt expiry. EXCAUSE register gets written with
    // one of PANIC_RSN_* values.
    panic_handler(frame, true);
}

void IRAM_ATTR xt_unhandled_exception(void *frame)
{
    panic_enable_cache();
    panic_handler(frame, false);
}

void __attribute__((noreturn)) panic_restart(void)
{
    bool digital_reset_needed = false;
#ifdef CONFIG_IDF_TARGET_ESP32
    // On the ESP32, cache error status can only be cleared by system reset
    if (esp_cache_err_get_cpuid() != -1) {
        digital_reset_needed = true;
    }
#endif
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#if CONFIG_IDF_TARGET_ESP32S2
    if (esp_memprot_is_intr_ena_any() || esp_memprot_is_locked_any()) {
        digital_reset_needed = true;
    }
#else
    bool is_on = false;
    if (esp_mprot_is_intr_ena_any(&is_on) != ESP_OK || is_on) {
        digital_reset_needed = true;
    } else if (esp_mprot_is_conf_locked_any(&is_on) != ESP_OK || is_on) {
        digital_reset_needed = true;
    }
#endif
#endif
    if (digital_reset_needed) {
        esp_restart_noos_dig();
    }
    esp_restart_noos();
}
