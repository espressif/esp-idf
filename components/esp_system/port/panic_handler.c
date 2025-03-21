/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
#else
#include "esp_memprot.h"
#endif
#endif

#include "esp_private/panic_internal.h"
#include "esp_private/panic_reason.h"

#include "hal/wdt_types.h"
#include "hal/wdt_hal.h"

extern int _invalid_pc_placeholder;

extern void esp_panic_handler(panic_info_t *);

extern void esp_panic_handler_increment_entry_count(void);

extern void esp_panic_handler_feed_wdts(void);

extern void esp_panic_handler_enable_rtc_wdt(uint32_t timeout_ms);

extern void esp_panic_handler_disable_timg_wdts(void);

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
    /* If watchdogs are enabled, the panic handler runs the risk of getting aborted pre-emptively because
     * an overzealous watchdog decides to reset it. Hence, we feed the WDTs here.
     *
     * However, we do not feed the WDTs in multi-core mode because we do not have a reliable way to handle
     * concurrency issues when both cores enter the panic handler at the same time. Hence, we avoid performing
     * any WDT configurations until one of the cores is put in to a busy_wait() state below. As a side note,
     * it may so happen that neither of the cores end up in a busy_wait() state and still try to work with the
     * WDTs simultaneously but chances of that happening are low. (TODO: IDF-12900)
     *
     * We do this before we increment the panic handler entry count to ensure that the WDTs are fed.
     */
#if CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    esp_panic_handler_feed_wdts();
#endif // CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE

    /* Increment the panic handler entry count */
    esp_panic_handler_increment_entry_count();

    panic_info_t info = { 0 };

    /*
     * Setup environment and perform necessary architecture/chip specific
     * steps here prior to the system panic handler.
     */
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
#endif // !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE

    /* Configuring the RTC WDT is critical for system safety.
     *
     * The RTC WDT is relied upon for a complete system reset, as it is the only
     * watchdog timer capable of resetting both the main system and the RTC subsystem.
     * In contrast, the Timer Group Watchdog Timers can only reset the main system
     * but not the RTC module.
     *
     * We have to do this before we do anything that might cause issues in the WDT interrupt handlers,
     * for example stalling the other core on ESP32 may cause the ESP32_ECO3_CACHE_LOCK_FIX
     * handler to get stuck.
     *
     * The timeout value for the RTC WDT is set to 10 seconds. The primary reason for
     * choosing a 10 second timeout is to allow the panic handler to run to completion
     * which may include core dump collection and apptrace flushing.
     *
     * Explanation for why the core dump takes time:
     * 64KB of core dump data (stacks of about 30 tasks) will produce ~85KB base64 data.
     * @ 115200 UART speed it will take more than 6 sec to print them out.
     *
     * TODO: Make the timeout configurable or more intelligent based on the panic reason and the
     * config options.
     */
#if CONFIG_ESP_SYSTEM_PANIC_REBOOT_DELAY_SECONDS
    esp_panic_handler_enable_rtc_wdt((CONFIG_ESP_SYSTEM_PANIC_REBOOT_DELAY_SECONDS + 10) * 1000);
#else
    esp_panic_handler_enable_rtc_wdt(10000);
#endif /* CONFIG_ESP_SYSTEM_PANIC_REBOOT_DELAY_SECONDS */

    /* Before we stall the other CPU, we need to disable all WDTs except the RTC WDT.
     * This is because the TIMG WDTs cannot reset the RTC subsystem, which stores the CPU stalling
     * configuration. If the other CPU is stalled and the TIMG WDTs trigger before we can unstall the
     * CPU then we have a chance of locking up the system without rebooting it.
     */
    esp_panic_handler_disable_timg_wdts();

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
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
#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
static void IRAM_ATTR panic_enable_cache(void)
{
    int core_id = esp_cpu_get_core_id();
    if (!spi_flash_cache_enabled()) {
        esp_ipc_isr_stall_abort();
        spi_flash_enable_cache(core_id);
    }
}
#endif

void IRAM_ATTR panicHandler(void *frame)
{
#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    panic_enable_cache();
#endif
    // This panic handler gets called for when the double exception vector,
    // kernel exception vector gets used; as well as handling interrupt-based
    // faults cache error, wdt expiry. EXCAUSE register gets written with
    // one of PANIC_RSN_* values.
    panic_handler(frame, true);
}

void IRAM_ATTR xt_unhandled_exception(void *frame)
{
#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    panic_enable_cache();
#endif
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
