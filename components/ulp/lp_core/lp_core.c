/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_rom_caps.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_assert.h"
#include "esp_cpu.h"
#include "soc/pmu_reg.h"
#include "hal/misc.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/pmu_share_hw.h"
#include "ulp_common.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_memory_shared.h"
#include "ulp_lp_core_lp_timer_shared.h"
#include "hal/lp_core_ll.h"
#include "hal/pmu_ll.h"

#if CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/rtc.h"
#endif

#if ESP_ROM_HAS_LP_ROM
extern uint8_t _rtc_ulp_memory_start[];
#endif //ESP_ROM_HAS_LP_ROM

#if CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
#include "esp_image_format.h"
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
#include "esp_cache.h"
#endif
extern uint32_t _ulp_hp_mem_resv_start;
extern uint32_t _ulp_hp_mem_resv_end;
#endif //CONFIG_ULP_COPROC_RUN_FROM_HP_MEM

#if SOC_LP_CORE_HW_AUTO_CLRWAKEUPCAUSE
#include "hal/lp_aon_hal.h"
#include "rom/rtc.h"
#endif

const static char* TAG = "ulp-lp-core";

#define WAKEUP_SOURCE_MAX_NUMBER 6

#define RESET_HANDLER_ADDR ((intptr_t)_rtc_ulp_memory_start + 0x80) // Placed after the 0x80 byte long vector table

/* Maps the flags defined in ulp_lp_core.h e.g. ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU to their actual HW values */
static uint32_t wakeup_src_sw_to_hw_flag_lookup[WAKEUP_SOURCE_MAX_NUMBER] = {
    LP_CORE_LL_WAKEUP_SOURCE_HP_CPU,
    LP_CORE_LL_WAKEUP_SOURCE_LP_UART,
    LP_CORE_LL_WAKEUP_SOURCE_LP_IO,
    LP_CORE_LL_WAKEUP_SOURCE_ETM,
    LP_CORE_LL_WAKEUP_SOURCE_LP_TIMER,
#if SOC_LP_VAD_SUPPORTED
    LP_CORE_LL_WAKEUP_SOURCE_LP_VAD,
#endif
};

#if CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
static uintptr_t ulp_lp_core_lp_mem_start(void)
{
#if ESP_ROM_HAS_LP_ROM
    return (uintptr_t)&_rtc_ulp_memory_start;
#else
    return (uintptr_t)RTC_SLOW_MEM;
#endif
}

static uintptr_t ulp_lp_core_lp_mem_end(void)
{
    return ulp_lp_core_lp_mem_start() + CONFIG_ULP_COPROC_RESERVE_MEM;
}

static bool ulp_lp_core_addr_range_in_region(uintptr_t start, size_t len, uintptr_t region_start, uintptr_t region_end)
{
    uintptr_t end;

    if (__builtin_add_overflow(start, (uintptr_t)len, &end)) {
        return false;
    }

    return start >= region_start && end <= region_end;
}
#endif //CONFIG_ULP_COPROC_RUN_FROM_HP_MEM

/* Convert the wake-up sources defined in ulp_lp_core.h to the actual HW wake-up source values */
static uint32_t lp_core_get_wakeup_source_hw_flags(uint32_t flags)
{
    uint32_t hw_flags = 0;
    for (int i = 0; i < WAKEUP_SOURCE_MAX_NUMBER; i++) {
        if (flags & (1 << i)) {
            hw_flags |= wakeup_src_sw_to_hw_flag_lookup[i];
        }
    }
    return hw_flags;
}

esp_err_t ulp_lp_core_run(ulp_lp_core_cfg_t* cfg)
{
    if (!cfg->wakeup_source) {
        ESP_LOGE(TAG, "No valid wakeup source specified");
        return ESP_ERR_INVALID_ARG;
    }

#if ESP_ROM_HAS_LP_ROM
    /* If we have a LP ROM we boot from it, before jumping to the app code */
    intptr_t boot_addr;
    if (cfg->skip_lp_rom_boot) {
        boot_addr = RESET_HANDLER_ADDR;
    } else {
        boot_addr = SOC_LP_ROM_LOW;
        /* Disable UART init in ROM, it defaults to XTAL clk src
         * which is not powered on during deep sleep
         * This will cause the ROM code to get stuck during UART output
         * if used
         */
        REG_SET_BIT(LP_UART_INIT_CTRL_REG, 1 << 0);
    }

    lp_core_ll_set_boot_address(boot_addr);
    lp_core_ll_set_app_boot_address(RESET_HANDLER_ADDR);

#endif //ESP_ROM_HAS_LP_ROM

#if SOC_LP_CORE_CONFIGURABLE_BOOT_ADDR
    /* Chips without LP ROM but with a SW-configurable boot address register
     * (e.g. ESP32-S31) must set the boot address explicitly.  The hardware
     * default points to SOC_RTC_DRAM_LOW (the interrupt vector table base),
     * not to reset_vector which is placed 0x80 bytes into LP RAM. */
    lp_core_ll_set_boot_address((intptr_t)SOC_RTC_DRAM_LOW + 0x80);
#endif // SOC_LP_CORE_CONFIGURABLE_BOOT_ADDR

    PERIPH_RCC_ATOMIC() {
#if CONFIG_ULP_NORESET_UNDER_DEBUG
        /* lp_core module reset causes loss of configured HW breakpoints and dcsr.ebreak* */
        if (!esp_cpu_dbgr_is_attached()) {
            lp_core_ll_reset_register();
        }
#else
        lp_core_ll_reset_register();
#endif
        lp_core_ll_enable_bus_clock(true);
    }

#if SOC_RTC_MEM_SUPPORT_SPEED_MODE_SWITCH
    /* Disable fast LP mem access to allow LP core to access LP memory during sleep */
    lp_core_ll_fast_lp_mem_enable(false);
#endif

    /* Enable stall at sleep request*/
    lp_core_ll_stall_at_sleep_request(true);

    /* Enable reset CPU when going to sleep */
    /* Avoid resetting chip in sleep mode when debugger is attached,
       otherwise configured HW breakpoints and dcsr.ebreak* bits will be missed */
    lp_core_ll_rst_at_sleep_enable(!(CONFIG_ULP_NORESET_UNDER_DEBUG && esp_cpu_dbgr_is_attached()));

    /* Enable JTAG debugging */
    lp_core_ll_debug_module_enable(true);

#if SOC_ULP_LP_UART_SUPPORTED
    if (cfg->wakeup_source & ULP_LP_CORE_WAKEUP_SOURCE_LP_UART) {
        lp_core_ll_enable_lp_uart_wakeup(true);
    }
#endif

#if SOC_RTC_TIMER_SUPPORTED
    ulp_lp_core_memory_shared_cfg_t* shared_mem = ulp_lp_core_memory_shared_cfg_get();

    if (cfg->wakeup_source & ULP_LP_CORE_WAKEUP_SOURCE_LP_TIMER) {
        if (!cfg->lp_timer_sleep_duration_us) {
            ESP_LOGI(TAG, "LP timer specified as wakeup source, but no sleep duration set. ULP will only wake-up once unless it calls ulp_lp_core_lp_timer_set_wakeup_time()");
        }
        shared_mem->sleep_duration_us = cfg->lp_timer_sleep_duration_us;
        shared_mem->sleep_duration_ticks = ulp_lp_core_lp_timer_calculate_sleep_ticks(cfg->lp_timer_sleep_duration_us);

        /* Set first wakeup alarm */
        ulp_lp_core_lp_timer_set_wakeup_time(cfg->lp_timer_sleep_duration_us);
    }
#endif

    /* Set wake-up sources */
    lp_core_ll_set_wakeup_source(lp_core_get_wakeup_source_hw_flags(cfg->wakeup_source));

    if (cfg->wakeup_source & ULP_LP_CORE_WAKEUP_SOURCE_HP_CPU) {
        lp_core_ll_hp_wake_lp();
    }

    return ESP_OK;
}

#if CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
static esp_err_t ulp_lp_core_load_segments(const uint8_t *program_binary, size_t program_size_bytes)
{
    if (program_size_bytes < sizeof(esp_image_header_t)) {
        ESP_LOGE(TAG, "Binary too small for image header");
        return ESP_ERR_INVALID_SIZE;
    }

    const esp_image_header_t *image_header = (const esp_image_header_t *)program_binary;
    const uintptr_t lp_mem_start = ulp_lp_core_lp_mem_start();
    const uintptr_t lp_mem_end = ulp_lp_core_lp_mem_end();
    const uintptr_t hp_mem_start = (uintptr_t)&_ulp_hp_mem_resv_start;
    const uintptr_t hp_mem_end = (uintptr_t)&_ulp_hp_mem_resv_end;

    if (image_header->magic != ESP_IMAGE_HEADER_MAGIC) {
        ESP_LOGE(TAG, "Invalid image magic 0x%02x, expected 0x%02x", image_header->magic, ESP_IMAGE_HEADER_MAGIC);
        return ESP_ERR_IMAGE_INVALID;
    }

    if (image_header->segment_count > ESP_IMAGE_MAX_SEGMENTS) {
        ESP_LOGE(TAG, "Segment count %d exceeds max %d", image_header->segment_count, ESP_IMAGE_MAX_SEGMENTS);
        return ESP_ERR_IMAGE_INVALID;
    }

    const uint8_t *seg_ptr = program_binary + sizeof(esp_image_header_t);
    for (int i = 0; i < image_header->segment_count; i++) {
        size_t seg_offset = (size_t)(seg_ptr - program_binary);
        if (__builtin_add_overflow(seg_offset, sizeof(esp_image_segment_header_t), &seg_offset)
                || seg_offset > program_size_bytes) {
            ESP_LOGE(TAG, "Segment %d header extends beyond binary size", i);
            return ESP_ERR_INVALID_SIZE;
        }

        const esp_image_segment_header_t *seg_hdr = (const esp_image_segment_header_t *)seg_ptr;
        seg_ptr += sizeof(esp_image_segment_header_t);

        size_t data_offset = (size_t)(seg_ptr - program_binary);
        size_t data_end_offset;
        if (__builtin_add_overflow(data_offset, (size_t)seg_hdr->data_len, &data_end_offset)
                || data_end_offset > program_size_bytes) {
            ESP_LOGE(TAG, "Segment %d extends beyond binary size", i);
            return ESP_ERR_INVALID_SIZE;
        }

        uintptr_t load_addr = (uintptr_t)seg_hdr->load_addr;
        uintptr_t load_end;
        if (__builtin_add_overflow(load_addr, (uintptr_t)seg_hdr->data_len, &load_end)) {
            ESP_LOGE(TAG, "Segment %d load range overflows", i);
            return ESP_ERR_INVALID_SIZE;
        }

        bool load_in_lp_mem = ulp_lp_core_addr_range_in_region(load_addr, seg_hdr->data_len, lp_mem_start, lp_mem_end);
        bool load_in_hp_mem = ulp_lp_core_addr_range_in_region(load_addr, seg_hdr->data_len, hp_mem_start, hp_mem_end);

        if (!load_in_lp_mem && !load_in_hp_mem) {
            ESP_LOGE(TAG, "Segment %d load range [0x%08x, 0x%08x) is outside LP/HP ULP memory", i,
                     (unsigned)load_addr, (unsigned)load_end);
            return ESP_ERR_INVALID_ARG;
        }

        ESP_LOGD(TAG, "Loading segment %d: load_addr=0x%08x size=0x%x",
                 i, (unsigned)seg_hdr->load_addr, (unsigned)seg_hdr->data_len);
        hal_memcpy((void *)(uintptr_t)seg_hdr->load_addr, seg_ptr, seg_hdr->data_len);
        seg_ptr += seg_hdr->data_len;
    }

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    /* Flush HP CPU write-back cache so the LP CPU sees the data written above.
     * Both _ulp_hp_mem_resv_start and _ulp_hp_mem_resv_end are aligned to the
     * L1 cache line size in the linker script, so the size is always a multiple
     * of the cache line size as required by esp_cache_msync. */
    size_t hp_mem_size = (uintptr_t)&_ulp_hp_mem_resv_end - (uintptr_t)&_ulp_hp_mem_resv_start;
    esp_cache_msync(&_ulp_hp_mem_resv_start, hp_mem_size,
                    ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);
#endif

    return ESP_OK;
}
#endif /* CONFIG_ULP_COPROC_RUN_FROM_HP_MEM */

esp_err_t ulp_lp_core_load_binary(const uint8_t* program_binary, size_t program_size_bytes)
{
    if (program_binary == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Turn off LP CPU before loading binary */
    ulp_lp_core_stop();

#if ESP_ROM_HAS_LP_ROM
    uint32_t* base = (uint32_t*)_rtc_ulp_memory_start;
#else
    uint32_t* base = RTC_SLOW_MEM;
#endif

    /* Clear LP RAM — also initializes LP-side bss */
    hal_memset(base, 0, CONFIG_ULP_COPROC_RESERVE_MEM);

#if CONFIG_ULP_COPROC_RUN_FROM_HP_MEM
    ESP_LOGW(TAG, "Running ULP app from HP memory. ULP will not be able to run if system enters deep sleep and power down HP memory.");
    /* Clear HP RAM — initializes HP-side bss before the LP core runs */
    hal_memset(&_ulp_hp_mem_resv_start, 0, CONFIG_ULP_COPROC_RESERVE_HP_MEM_BYTES);
    return ulp_lp_core_load_segments(program_binary, program_size_bytes);
#else
    if (program_size_bytes > CONFIG_ULP_COPROC_RESERVE_MEM) {
        return ESP_ERR_INVALID_SIZE;
    }
    hal_memcpy(base, program_binary, program_size_bytes);

    return ESP_OK;
#endif
}

void ulp_lp_core_sleep_start(void)
{
#if SOC_LP_CORE_HW_AUTO_CLRWAKEUPCAUSE
    /* LP store register to save wakeup cause for HP core to query.
     * Using a hardware register avoids symbol linking issues between
     * the independently compiled HP and LP core binaries.
     * Save PMU wakeup cause to LP store register for HP core to query */
    lp_aon_hal_store_wakeup_cause(pmu_ll_hp_get_wakeup_cause(&PMU));
#endif

    lp_core_ll_request_sleep();
}

void ulp_lp_core_stop(void)
{
    if (esp_cpu_dbgr_is_attached()) {
        /* upon SW reset debugger puts LP core into the infinite loop at reset vector,
           so configure it to stall when going to sleep */
        lp_core_ll_stall_at_sleep_request(true);
        /* Avoid resetting chip in sleep mode when debugger is attached,
        otherwise configured HW breakpoints and dcsr.ebreak* bits will be missed */
        lp_core_ll_rst_at_sleep_enable(!CONFIG_ULP_NORESET_UNDER_DEBUG);
        lp_core_ll_debug_module_enable(true);
    }
    /* Disable wake-up source and put lp core to sleep */
    lp_core_ll_set_wakeup_source(0);
    ulp_lp_core_sleep_start();
}

void ulp_lp_core_sw_intr_to_lp_trigger(void)
{
    /* Write-only register, no need to protect it */
    lp_core_ll_hp_wake_lp();
}

void ulp_lp_core_sw_intr_trigger_self(void)
{
    /* Write-only register, no need to protect it */
    pmu_ll_lp_trigger_sw_intr(&PMU);
}

void ulp_lp_core_sw_intr_from_lp_enable(bool enable)
{
    /* Interrupt enable register must be protected since it is a R/W one */
    pmu_lock_acquire();
    pmu_ll_hp_enable_sw_intr(&PMU, enable);
    pmu_lock_release();
}

void ulp_lp_core_sw_intr_from_lp_clear(void)
{
    /* Write-only register, no need to protect it */
    pmu_ll_hp_clear_sw_intr_status(&PMU);
}
