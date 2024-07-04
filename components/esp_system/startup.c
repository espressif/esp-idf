/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "esp_attr.h"
#include "esp_err.h"
#include "esp_compiler.h"
#include "esp_macros.h"

#include "esp_system.h"
#include "esp_log.h"

#include "sdkconfig.h"

#include "soc/soc_caps.h"
#include "esp_cpu.h"

#include "esp_private/startup_internal.h"

// Ensure that system configuration matches the underlying number of cores.
// This should enable us to avoid checking for both every time.
#if !(SOC_CPU_CORES_NUM > 1) && !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
#error "System has been configured to run on multiple cores, but target SoC only has a single core."
#endif

uint64_t g_startup_time = 0;

// App entry point for core 0
extern void esp_startup_start_app(void);

// Entry point for core 0 from hardware init (port layer)
void start_cpu0(void) __attribute__((weak, alias("start_cpu0_default"))) __attribute__((noreturn));

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
// Entry point for core [1..X] from hardware init (port layer)
void start_cpu_other_cores(void) __attribute__((weak, alias("start_cpu_other_cores_default"))) __attribute__((noreturn));

// App entry point for core [1..X]
void esp_startup_start_app_other_cores(void) __attribute__((weak, alias("esp_startup_start_app_other_cores_default"))) __attribute__((noreturn));

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

static const char* TAG = "cpu_start";

/**
 * Xtensa gcc is configured to emit a .ctors section, RISC-V gcc is configured with --enable-initfini-array
 * so it emits an .init_array section instead.
 * But the init_priority sections will be sorted for iteration in ascending order during startup.
 * The rest of the init_array sections is sorted for iteration in descending order during startup, however.
 * Hence a different section is generated for the init_priority functions which is looped
 * over in ascending direction instead of descending direction.
 * The RISC-V-specific behavior is dependent on the linker script ld/esp32c3/sections.ld.in.
 */
__attribute__((no_sanitize_undefined)) /* TODO: IDF-8133 */
static void do_global_ctors(void)
{
#if __riscv
    extern void (*__init_priority_array_start)(void);
    extern void (*__init_priority_array_end)(void);
#endif

    extern void (*__init_array_start)(void);
    extern void (*__init_array_end)(void);

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS
    struct object {
        long placeholder[ 10 ];
    };
    void __register_frame_info(const void *begin, struct object * ob);
    extern char __eh_frame[];

    static struct object ob;
    __register_frame_info(__eh_frame, &ob);
#endif // CONFIG_COMPILER_CXX_EXCEPTIONS

    void (**p)(void);

#if __riscv
    for (p = &__init_priority_array_start; p < &__init_priority_array_end; ++p) {
        ESP_LOGD(TAG, "calling init function: %p", *p);
        (*p)();
    }
#endif

    ESP_COMPILER_DIAGNOSTIC_PUSH_IGNORE("-Wanalyzer-out-of-bounds")
    for (p = &__init_array_end - 1; p >= &__init_array_start; --p) {
        ESP_LOGD(TAG, "calling init function: %p", *p);
        (*p)();
    }
    ESP_COMPILER_DIAGNOSTIC_POP("-Wanalyzer-out-of-bounds")

}

/**
 * @brief Call component init functions defined using ESP_SYSTEM_INIT_Fn macros.
 * The esp_system_init_fn_t structures describing these functions are collected into
 * an array [_esp_system_init_fn_array_start, _esp_system_init_fn_array_end) by the
 * linker. The functions are sorted by their priority value.
 * The sequence of the init function calls (sorted by priority) is documented in
 * system_init_fn.txt file.
 * @param stage_num Stage number of the init function call (0, 1).
 */
__attribute__((no_sanitize_undefined)) /* TODO: IDF-8133 */
static void do_system_init_fn(uint32_t stage_num)
{
    extern esp_system_init_fn_t _esp_system_init_fn_array_start;
    extern esp_system_init_fn_t _esp_system_init_fn_array_end;

    esp_system_init_fn_t *p;

    int core_id = esp_cpu_get_core_id();
    for (p = &_esp_system_init_fn_array_start; p < &_esp_system_init_fn_array_end; ++p) {
        if (p->stage == stage_num && (p->cores & BIT(core_id)) != 0) {
            // During core init, stdout is not initialized yet, so use early logging.
            ESP_EARLY_LOGD(TAG, "calling init function: %p on core: %d", p->fn, core_id);
            esp_err_t err = (*(p->fn))();
            if (err != ESP_OK) {
                ESP_EARLY_LOGE(TAG, "init function %p has failed (0x%x), aborting", p->fn, err);
                abort();
            }
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
static void IRAM_ATTR start_cpu_other_cores_default(void)
{
    do_system_init_fn(ESP_SYSTEM_INIT_STAGE_SECONDARY);

    while (!s_system_full_inited) {
        esp_rom_delay_us(100);
    }

    esp_startup_start_app_other_cores();
}
#endif

static void do_core_init(void)
{
    do_system_init_fn(ESP_SYSTEM_INIT_STAGE_CORE);
}

static void do_secondary_init(void)
{
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // The port layer transferred control to this function with other cores 'paused',
    // resume execution so that cores might execute component initialization functions.
    startup_resume_other_cores();
#endif

    // Execute initialization functions esp_system_init_fn_t assigned to the main core. While
    // this is happening, all other cores are executing the initialization functions
    // assigned to them since they have been resumed already.
    do_system_init_fn(ESP_SYSTEM_INIT_STAGE_SECONDARY);

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
    do_core_init();

    // Execute constructors.
    do_global_ctors();

    // Execute init functions of other components; blocks
    // until all cores finish (when !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE).
    do_secondary_init();

#if SOC_CPU_CORES_NUM > 1 && !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    s_system_full_inited = true;
#endif

    esp_startup_start_app();

    ESP_INFINITE_LOOP();
}
