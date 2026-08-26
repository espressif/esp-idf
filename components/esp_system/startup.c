/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_private/esp_system_attr.h"
#include "esp_err.h"
#include "esp_compiler.h"
#include "esp_macros.h"

#include "esp_system.h"
#include "esp_log.h"

#include "sdkconfig.h"

#include "soc/soc_caps.h"
#include "esp_cpu.h"

#include "esp_private/startup_internal.h"
#include "esp_private/esp_sys_event_internal.h"

// Ensure that system configuration matches the underlying number of cores.
// This should enable us to avoid checking for both every time.
#if !(SOC_CPU_CORES_NUM > 1) && !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#error "System has been configured to run on multiple cores, but target SoC only has a single core."
#endif

uint64_t g_startup_time = 0;

// App entry point for core 0
extern void esp_startup_start_app(void);

// Entry point for core 0 from hardware init (port layer)
#if defined(__APPLE__) && defined(__MACH__)
static void start_cpu0_default(void);
__attribute__((weak, noreturn)) void start_cpu0(void)
{
    start_cpu0_default();
    __builtin_unreachable();
}
#else
void start_cpu0(void) __attribute__((weak, alias("start_cpu0_default"))) __attribute__((noreturn));
#endif

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
// Entry point for core [1..X] from hardware init (port layer)
#if defined(__APPLE__) && defined(__MACH__)
static void start_cpu_other_cores_default(void);
__attribute__((weak, noreturn)) void start_cpu_other_cores(void)
{
    start_cpu_other_cores_default();
    __builtin_unreachable();
}
#else
void start_cpu_other_cores(void) __attribute__((weak, alias("start_cpu_other_cores_default"))) __attribute__((noreturn));
#endif

// App entry point for core [1..X]
#if defined(__APPLE__) && defined(__MACH__)
static void esp_startup_start_app_other_cores_default(void);
__attribute__((weak, noreturn)) void esp_startup_start_app_other_cores(void)
{
    esp_startup_start_app_other_cores_default();
    __builtin_unreachable();
}
#else
void esp_startup_start_app_other_cores(void) __attribute__((weak, alias("esp_startup_start_app_other_cores_default"))) __attribute__((noreturn));
#endif

static volatile bool s_system_inited[SOC_CPU_CORES_NUM] = { false };

const sys_startup_fn_t g_startup_fn[SOC_CPU_CORES_NUM] = { [0] = start_cpu0,
#if SOC_CPU_CORES_NUM > 1
                                                           [1 ... SOC_CPU_CORES_NUM - 1] = start_cpu_other_cores
#endif
                                                         };

static volatile bool s_system_full_inited = false;
#else
const sys_startup_fn_t g_startup_fn[1] = { start_cpu0 };
#endif

ESP_LOG_ATTR_TAG(TAG, "cpu_start");

#if CONFIG_COMPILER_CXX_EXCEPTIONS && !CONFIG_IDF_TARGET_LINUX
/**
 * @brief A helper function for __do_global_ctors, which is in crtend.o.
 * It has been adapted from GCC source code. In GCC, it resides under
 * the USE_EH_FRAME_REGISTRY macro, which is not enabled in Espressif
 * toolchains to save small memory amount. Nevertheless, when C++ exceptions
 * are enabled this initialization becomes necessary.
 */
static void __do_global_ctors_1(void)
{
    struct object {
        long placeholder[10];
    };
    void __register_frame_info(const void *begin, struct object * ob);
    extern char __eh_frame[];

    static struct object ob;
    __register_frame_info(__eh_frame, &ob);
}
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS && !CONFIG_IDF_TARGET_LINUX

static void do_system_init_event(esp_sys_event_id_t id)
{
    int core_id = esp_cpu_get_core_id();
    ESP_SYS_EVENT_FOREACH(h, id) {
        // During core init, stdout is not initialized yet, so use early logging.
        ESP_EARLY_LOGD(TAG, "calling init function: %p on core: %d", (void *)h->handler, core_id);
        esp_err_t err = h->handler(NULL, NULL);
        if (err != ESP_OK) {
            ESP_EARLY_LOGE(TAG, "init function %p has failed (0x%x), aborting", (void *)h->handler, err);
            abort();
        }
    }

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_system_inited[core_id] = true;
#endif
}

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
static void  esp_startup_start_app_other_cores_default(void)
{
    while (1) {
        esp_rom_delay_us(UINT32_MAX);
    }
}

/* This function has to be in IRAM, as while it is running on CPU1, CPU0 may do some flash operations
 * (e.g. initialize the core dump), which means that cache will be disabled.
 */
static void ESP_SYSTEM_IRAM_ATTR start_cpu_other_cores_default(void)
{
    // Run secondary init handlers for this core. Core-scoped handlers skip
    // themselves inside their registration wrappers.
    do_system_init_event(ESP_SYS_EVENT_SYSTEM_INIT_SECONDARY);

    while (!s_system_full_inited) {
        esp_rom_delay_us(100);
    }

    esp_startup_start_app_other_cores();
}
#endif

static void do_core_init(void)
{
    // Run core init handlers (core 0 only, before cache/MMU is configured).
    // Secondary cores are not started yet at this point, so there is no
    // per-CPU CORE-stage event.
    do_system_init_event(ESP_SYS_EVENT_SYSTEM_INIT_CORE);
}

static void do_secondary_init(void)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // The port layer transferred control to this function with other cores 'paused',
    // resume execution so that cores might execute component initialization functions.
    startup_resume_other_cores();
#endif

    // Run secondary init handlers for core 0. While this is happening, all
    // other cores are executing the same priority-ordered secondary list and
    // skipping handlers whose core mask does not include them.
    do_system_init_event(ESP_SYS_EVENT_SYSTEM_INIT_SECONDARY);

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // Wait for all cores to finish secondary init.
    volatile bool system_inited = false;

    while (!system_inited) {
        system_inited = true;
        for (int i = 0; i < SOC_CPU_CORES_NUM; i++) {
            system_inited &= s_system_inited[i];
        }
        esp_rom_delay_us(100);
    }
#endif
}

static void start_cpu0_default(void)
{
    // Initialize core components and services.
    // Operations that needs the cache to be disabled have to be done here.
    do_core_init();

#if !CONFIG_IDF_TARGET_LINUX
    // Execute constructors.
#if CONFIG_COMPILER_CXX_EXCEPTIONS
    __do_global_ctors_1();
#endif
    extern void __libc_init_array(void);
    __libc_init_array();
#endif // !CONFIG_IDF_TARGET_LINUX

    /* ----------------------------------Separator-----------------------------
     * After this stage, other CPU start running with the cache, however the scheduler (and ipc service) is not available.
     * Don't touch the cache/MMU until the OS is up.
     */

    // Execute init functions of other components; blocks
    // until all cores finish (when !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE).
    do_secondary_init();

#if SOC_CPU_CORES_NUM > 1 && !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_system_full_inited = true;
#endif

    esp_startup_start_app();

    ESP_INFINITE_LOOP();
}

#if CONFIG_IDF_TARGET_LINUX && !defined(ESP_SYSTEM_LINUX_NO_MAIN)
__attribute__((weak)) int main(int argc, char **argv)
{
    start_cpu0();
    return 0;
}
#endif // CONFIG_IDF_TARGET_LINUX
