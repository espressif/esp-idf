/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOSConfig.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"
#include "hal/memprot_ll.h"
#include "esp32s3/rom/ets_sys.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "esp_attr.h"
#include "hal/memprot_types.h"
#include "esp_private/esp_memprot_internal.h"
#include "esp_memprot.h"
#include <inttypes.h>

/*
 * LD section boundaries
 */
extern int _iram_text_start;
extern int _iram_text_end;
extern int _rtc_text_end;

/*
 * Local holder of the Memprot config required by the last esp_mprot_set_prot() call.
 * The structure is zeroed on creation => 'mem_type_mask == MEMPROT_TYPE_NONE' guarantees no interference before proper
 * update by the API function
 */
static esp_memp_config_t s_memp_cfg = ESP_MEMPROT_ZERO_CONFIG();


//////////////////////////////////////////////////////////////////////////////
// internal helpers

static esp_err_t esp_mprot_cpuid_valid(const int core)
{
    for (size_t x = 0; x < s_memp_cfg.target_cpu_count; x++) {
        if (core == s_memp_cfg.target_cpu[x]) {
            return ESP_OK;
        }
    }

    return ESP_ERR_MEMPROT_CPUID_INVALID;
}

static void *esp_memprot_iram0_get_def_split_addr(void)
{
    return (void *)(uint32_t)&_iram_text_end;
}

static void *esp_memprot_dram0_get_def_split_addr(void)
{
    return (void *)MAP_IRAM_TO_DRAM((uint32_t)&_iram_text_end);
}

static void *esp_memprot_rtcfast_get_min_split_addr(void)
{
    return (void *)(uint32_t)&_rtc_text_end;
}

//register MEMPROT-relevant interrupts in the global matrix (to deploy panic handling)
//mapped to ETS_MEMACCESS_ERR_INUM (shared with cache-access interrupts)
static esp_err_t esp_mprot_set_intr_matrix(const esp_mprot_mem_t mem_type, const int core)
{
    ESP_INTR_DISABLE(ETS_MEMACCESS_ERR_INUM);

    esp_err_t err;
    uint32_t intr_src_num = ETS_MAX_INTR_SOURCE;

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_intr_source_num(core, &intr_src_num)))
        esp_rom_route_intr_matrix(core, intr_src_num, ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_intr_source_num(core, &intr_src_num)))
        esp_rom_route_intr_matrix(core, intr_src_num, ETS_MEMACCESS_ERR_INUM);
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_intr_source_num(core, &intr_src_num)))
        esp_rom_route_intr_matrix(core, intr_src_num, ETS_MEMACCESS_ERR_INUM);
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    ESP_INTR_ENABLE(ETS_MEMACCESS_ERR_INUM);

    return ESP_OK;
}

//////////////////////////////////////////////////////////////////////////////
// PMS configuration APIs (set/get/lock)
// IRAM/DRAM settings are shared by both CPUs, RTCFAST settings are separate for each core

esp_err_t esp_mprot_set_split_addr(const esp_mprot_mem_t mem_type, const esp_mprot_split_addr_t line_type, const void *line_addr, const int core __attribute__((unused)))
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        switch (line_type) {
        case MEMPROT_SPLIT_ADDR_IRAM0_DRAM0:
            return esp_mprot_ll_err_to_esp_err(memprot_ll_set_iram0_split_line_main_I_D(line_addr));
        case MEMPROT_SPLIT_ADDR_IRAM0_LINE_0:
            return esp_mprot_ll_err_to_esp_err(memprot_ll_set_iram0_split_line_I_0(line_addr));
        case MEMPROT_SPLIT_ADDR_IRAM0_LINE_1:
            return esp_mprot_ll_err_to_esp_err(memprot_ll_set_iram0_split_line_I_1(line_addr));
        default:
            return ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID;
        } break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        switch (line_type) {
        case MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_0:
            return esp_mprot_ll_err_to_esp_err(memprot_ll_set_dram0_split_line_D_0(line_addr));
        case MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_1:
            return esp_mprot_ll_err_to_esp_err(memprot_ll_set_dram0_split_line_D_1(line_addr));
        default:
            return ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID;
        } break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        if (line_type == MEMPROT_SPLIT_ADDR_MAIN) { /* so far only WORLD_0 is supported */
            esp_err_t err;
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_set_rtcfast_split_line(core, line_addr, MEMP_HAL_WORLD_0)))
            return ESP_OK;
        } else {
            return ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID;
        }
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }
}

esp_err_t esp_mprot_get_split_addr(const esp_mprot_mem_t mem_type, const esp_mprot_split_addr_t line_type, void **line_addr, const int core __attribute__((unused)))
{
    if (line_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        switch (line_type) {
        case MEMPROT_SPLIT_ADDR_IRAM0_DRAM0:
            *line_addr = memprot_ll_get_iram0_split_line_main_I_D();
            break;
        case MEMPROT_SPLIT_ADDR_IRAM0_LINE_0:
            *line_addr = memprot_ll_get_iram0_split_line_I_0();
            break;
        case MEMPROT_SPLIT_ADDR_IRAM0_LINE_1:
            *line_addr = memprot_ll_get_iram0_split_line_I_1();
            break;
        default:
            return ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID;
        } break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        switch (line_type) {
        case MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_0:
            *line_addr = memprot_ll_get_dram0_split_line_D_0();
            break;
        case MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_1:
            *line_addr = memprot_ll_get_dram0_split_line_D_1();
            break;
        default:
            return ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID;
        } break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        if (line_type == MEMPROT_SPLIT_ADDR_MAIN) { /* so far only WORLD_0 is supported */
            esp_err_t err;
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_get_rtcfast_split_line(core, MEMP_HAL_WORLD_0, *line_addr)))
        } else {
            return ESP_ERR_MEMPROT_SPLIT_ADDR_INVALID;
        }
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_default_main_split_addr(const esp_mprot_mem_t mem_type, void **def_split_addr)
{
    if (def_split_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        *def_split_addr = esp_memprot_iram0_get_def_split_addr();
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        *def_split_addr = esp_memprot_dram0_get_def_split_addr();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        *def_split_addr = esp_memprot_rtcfast_get_min_split_addr();
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_set_split_addr_lock(const esp_mprot_mem_t mem_type, const int core __attribute__((unused)))
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
    case MEMPROT_TYPE_DRAM0_SRAM:
        memprot_ll_set_iram0_dram0_split_line_lock();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST: { //caution: RTCFAST shares the lock with other PIF PMS constraints!
        esp_err_t err;
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_set_pif_constraint_lock(core)))
    } break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_split_addr_lock(const esp_mprot_mem_t mem_type, bool *locked, const int core __attribute__((unused)))
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
    case MEMPROT_TYPE_DRAM0_SRAM:
        *locked = memprot_ll_get_iram0_dram0_split_line_lock();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST: {
        esp_err_t err;
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_get_pif_constraint_lock(core, locked)))
    } break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_set_pms_lock(const esp_mprot_mem_t mem_type, const int core __attribute__((unused)))
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        memprot_ll_iram0_set_pms_lock();
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        memprot_ll_dram0_set_pms_lock();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST: { //caution: RTCFAST shares the lock with other PIF PMS constraints!
        esp_err_t err;
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_set_pif_constraint_lock(core)))
    } break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_pms_lock(const esp_mprot_mem_t mem_type, bool *locked, const int core __attribute__((unused)))
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        *locked = memprot_ll_iram0_get_pms_lock();
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        *locked = memprot_ll_dram0_get_pms_lock();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST: {
        esp_err_t err;
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_get_pif_constraint_lock(core, locked)))
    } break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_set_pms_area(const esp_mprot_pms_area_t area_type, const uint32_t flags, const int core __attribute__((unused)))
{
    esp_err_t err;
    bool r = flags & MEMPROT_OP_READ;
    bool w = flags & MEMPROT_OP_WRITE;
    bool x = flags & MEMPROT_OP_EXEC;

    switch (area_type) {
    case MEMPROT_PMS_AREA_IRAM0_0:
        memprot_ll_iram0_set_pms_area_0(r, w, x);
        break;
    case MEMPROT_PMS_AREA_IRAM0_1:
        memprot_ll_iram0_set_pms_area_1(r, w, x);
        break;
    case MEMPROT_PMS_AREA_IRAM0_2:
        memprot_ll_iram0_set_pms_area_2(r, w, x);
        break;
    case MEMPROT_PMS_AREA_IRAM0_3:
        memprot_ll_iram0_set_pms_area_3(r, w, x);
        break;
    case MEMPROT_PMS_AREA_DRAM0_0:
        memprot_ll_dram0_set_pms_area_0(r, w);
        break;
    case MEMPROT_PMS_AREA_DRAM0_1:
        memprot_ll_dram0_set_pms_area_1(r, w);
        break;
    case MEMPROT_PMS_AREA_DRAM0_2:
        memprot_ll_dram0_set_pms_area_2(r, w);
        break;
    case MEMPROT_PMS_AREA_DRAM0_3:
        memprot_ll_dram0_set_pms_area_3(r, w);
        break;
    case MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_set_pms_area(core, r, w, x, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_LOW)))
        break;
    case MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_set_pms_area(core, r, w, x, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_HIGH)))
        break;
    case MEMPROT_PMS_AREA_ICACHE_0:
        memprot_ll_icache_set_pms_area_0(r, w, x);
        break;
    case MEMPROT_PMS_AREA_ICACHE_1:
        memprot_ll_icache_set_pms_area_1(r, w, x);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_pms_area(const esp_mprot_pms_area_t area_type, uint32_t *flags, const int core __attribute__((unused)))
{
    if (flags == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    bool r = false;
    bool w = false;
    bool x = false;

    switch (area_type) {
    case MEMPROT_PMS_AREA_IRAM0_0:
        memprot_ll_iram0_get_pms_area_0(&r, &w, &x);
        break;
    case MEMPROT_PMS_AREA_IRAM0_1:
        memprot_ll_iram0_get_pms_area_1(&r, &w, &x);
        break;
    case MEMPROT_PMS_AREA_IRAM0_2:
        memprot_ll_iram0_get_pms_area_2(&r, &w, &x);
        break;
    case MEMPROT_PMS_AREA_IRAM0_3:
        memprot_ll_iram0_get_pms_area_3(&r, &w, &x);
        break;
    case MEMPROT_PMS_AREA_DRAM0_0:
        memprot_ll_dram0_get_pms_area_0(&r, &w);
        break;
    case MEMPROT_PMS_AREA_DRAM0_1:
        memprot_ll_dram0_get_pms_area_1(&r, &w);
        break;
    case MEMPROT_PMS_AREA_DRAM0_2:
        memprot_ll_dram0_get_pms_area_2(&r, &w);
        break;
    case MEMPROT_PMS_AREA_DRAM0_3:
        memprot_ll_dram0_get_pms_area_3(&r, &w);
        break;
    case MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_pms_area(core, &r, &w, &x, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_LOW)))
        break;
    case MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_pms_area(core, &r, &w, &x, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_HIGH)))
        break;
    case MEMPROT_PMS_AREA_ICACHE_0:
        memprot_ll_icache_get_pms_area_0(&r, &w, &x);
        break;
    case MEMPROT_PMS_AREA_ICACHE_1:
        memprot_ll_icache_get_pms_area_1(&r, &w, &x);
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    *flags = 0;
    if (r) {
        *flags |= MEMPROT_OP_READ;
    }
    if (w) {
        *flags |= MEMPROT_OP_WRITE;
    }
    if (x) {
        *flags |= MEMPROT_OP_EXEC;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_set_monitor_lock(const esp_mprot_mem_t mem_type, const int core)
{
    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_set_monitor_lock(core)))
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_set_monitor_lock(core)))
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_set_monitor_lock(core)))
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_monitor_lock(const esp_mprot_mem_t mem_type, bool *locked, const int core)
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_monitor_lock(core, locked)))
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_monitor_lock(core, locked)))
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_monitor_lock(core, locked)))
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_set_monitor_en(const esp_mprot_mem_t mem_type, const bool enable, const int core)
{
    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_set_monitor_en(core, enable)))
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_set_monitor_en(core, enable)))
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_set_monitor_en(core, enable)))
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_monitor_en(esp_mprot_mem_t mem_type, bool *enabled, const int core)
{
    if (enabled == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_monitor_en(core, enabled)))
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_monitor_en(core, enabled)))
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_monitor_en(core, enabled)))
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

//////////////////////////////////////////////////////////////////////////////
// PMS-violation interrupt handling APIs (IRAM section - called from panic-handler)

esp_err_t esp_mprot_get_active_intr(esp_memp_intr_source_t *active_memp_intr)
{
    if (active_memp_intr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_mprot_mem_t mt;
    int c;

    do {
        uint32_t intr_on = 0;
        esp_err_t err;

        //IRAM0
        if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_IRAM0_SRAM) {

            mt = MEMPROT_TYPE_IRAM0_SRAM;

            c = PRO_CPU_NUM;
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_monitor_status_intr(c, &intr_on)))
            if (intr_on) {
                break;
            }

            //2-core
            if (s_memp_cfg.target_cpu_count > 1) {
                c = APP_CPU_NUM;
                ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_monitor_status_intr(c, &intr_on)))
                if (intr_on) {
                    break;
                }
            }
        }

        //DRAM0
        if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_DRAM0_SRAM) {

            mt = MEMPROT_TYPE_DRAM0_SRAM;

            c = PRO_CPU_NUM;
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_monitor_status_intr(c, &intr_on)))
            if (intr_on) {
                break;
            }

            //2-core
            if (s_memp_cfg.target_cpu_count > 1) {
                c = APP_CPU_NUM;
                ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_monitor_status_intr(c, &intr_on)))
                if (intr_on) {
                    break;
                }
            }
        }

        //RTCFAST
        if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_IRAM0_RTCFAST) {

            mt = MEMPROT_TYPE_IRAM0_RTCFAST;

            c = PRO_CPU_NUM;
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_monitor_status_intr(c, &intr_on)))
            if (intr_on) {
                break;
            }

            //2-core
            if (s_memp_cfg.target_cpu_count > 1) {
                c = APP_CPU_NUM;
                ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_monitor_status_intr(c, &intr_on)))
                if (intr_on) {
                    break;
                }
            }
        }

        mt = MEMPROT_TYPE_NONE;
        c = -1;

    } while (0);

    active_memp_intr->mem_type = mt;
    active_memp_intr->core = c;

    return ESP_OK;
}

esp_err_t esp_mprot_monitor_clear_intr(esp_mprot_mem_t mem_type, const int core)
{
    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_set_monitor_intrclr(core)))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_reset_monitor_intrclr(core)))
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_set_monitor_intrclr(core)))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_reset_monitor_intrclr(core)))
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_set_monitor_intrclr(core)))
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_reset_monitor_intrclr(core)))
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_is_conf_locked_any(bool *locked)
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bool lock_on = false;
    esp_err_t err;

    //IRAM0
    if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_IRAM0_SRAM) {

        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_split_addr_lock(MEMPROT_TYPE_IRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
        *locked |= lock_on;

        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_pms_lock(MEMPROT_TYPE_IRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
        *locked |= lock_on;

        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_lock(MEMPROT_TYPE_IRAM0_SRAM, &lock_on, PRO_CPU_NUM))
        *locked |= lock_on;

        //2-core
        if (s_memp_cfg.target_cpu_count > 1) {
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_lock(MEMPROT_TYPE_IRAM0_SRAM, &lock_on, APP_CPU_NUM))
            *locked |= lock_on;
        }
    }

    //DRAM0
    if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_DRAM0_SRAM) {

        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_split_addr_lock(MEMPROT_TYPE_DRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
        *locked |= lock_on;

        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_pms_lock(MEMPROT_TYPE_DRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
        *locked |= lock_on;

        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_lock(MEMPROT_TYPE_DRAM0_SRAM, &lock_on, PRO_CPU_NUM));
        *locked |= lock_on;

        //2-core
        if (s_memp_cfg.target_cpu_count > 1) {
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_lock(MEMPROT_TYPE_DRAM0_SRAM, &lock_on, APP_CPU_NUM));
            *locked |= lock_on;
        }
    }

    //RTCFAST
    if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_IRAM0_RTCFAST) {

        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_pms_lock(MEMPROT_TYPE_IRAM0_RTCFAST, &lock_on, PRO_CPU_NUM))
        *locked |= lock_on;

        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_lock(MEMPROT_TYPE_IRAM0_RTCFAST, &lock_on, PRO_CPU_NUM));
        *locked |= lock_on;

        //2-core
        if (s_memp_cfg.target_cpu_count > 1) {
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_pms_lock(MEMPROT_TYPE_IRAM0_RTCFAST, &lock_on, APP_CPU_NUM))
            *locked |= lock_on;

            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_lock(MEMPROT_TYPE_IRAM0_RTCFAST, &lock_on, APP_CPU_NUM));
            *locked |= lock_on;
        }
    }

    return ESP_OK;
}

esp_err_t esp_mprot_is_intr_ena_any(bool *enabled)
{
    if (enabled == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bool ena_on = false;
    esp_err_t err;

    //IRAM0
    if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_IRAM0_SRAM) {
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_en(MEMPROT_TYPE_IRAM0_SRAM, &ena_on, PRO_CPU_NUM))
        *enabled |= ena_on;
        //2-core
        if (s_memp_cfg.target_cpu_count > 1) {
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_en(MEMPROT_TYPE_IRAM0_SRAM, &ena_on, APP_CPU_NUM))
            *enabled |= ena_on;
        }
    }

    //DRAM0
    if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_DRAM0_SRAM) {
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_en(MEMPROT_TYPE_DRAM0_SRAM, &ena_on, PRO_CPU_NUM))
        *enabled |= ena_on;
        //2-core
        if (s_memp_cfg.target_cpu_count > 1) {
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_en(MEMPROT_TYPE_DRAM0_SRAM, &ena_on, APP_CPU_NUM))
            *enabled |= ena_on;
        }
    }

    //RTCFAST
    if (s_memp_cfg.mem_type_mask & MEMPROT_TYPE_IRAM0_RTCFAST) {
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_en(MEMPROT_TYPE_IRAM0_RTCFAST, &ena_on, PRO_CPU_NUM))
        *enabled |= ena_on;
        //2-core
        if (s_memp_cfg.target_cpu_count > 1) {
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_get_monitor_en(MEMPROT_TYPE_IRAM0_RTCFAST, &ena_on, APP_CPU_NUM))
            *enabled |= ena_on;
        }
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_violate_addr(const esp_mprot_mem_t mem_type, void **fault_addr, const int core)
{
    if (fault_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    void *fault_addr_temp = NULL;

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_monitor_status_fault_addr(core, &fault_addr_temp)))
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_monitor_status_fault_addr(core, &fault_addr_temp)))
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_monitor_status_fault_addr(core, &fault_addr_temp)))
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    *fault_addr = fault_addr_temp;

    return ESP_OK;
}

esp_err_t esp_mprot_get_violate_world(const esp_mprot_mem_t mem_type, esp_mprot_pms_world_t *world, const int core)
{
    if (world == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    uint32_t regval = MEMPROT_PMS_WORLD_INVALID;

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_monitor_status_fault_world(core, &regval)))
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_monitor_status_fault_world(core, &regval)))
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_monitor_status_fault_world(core, &regval)))
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    if (regval == MEMPROT_PMS_WORLD_INVALID) {
        return ESP_ERR_MEMPROT_WORLD_INVALID;
    }

    *world = esp_mprot_ll_world_to_hl_world(regval);

    return ESP_OK;
}

esp_err_t esp_mprot_get_violate_operation(const esp_mprot_mem_t mem_type, uint32_t *oper, const int core)
{
    if (oper == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    uint32_t operation = MEMPROT_OP_NONE;
    uint32_t regval = 0xFFFFFFFF;

    //check 1: LoadStore, 2: R/W
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM: {
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_monitor_status_fault_loadstore(core, &regval)))
        if (regval == 0) {
            operation = MEMPROT_OP_EXEC;
        } else {
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_iram0_get_monitor_status_fault_wr(core, &regval)))
            operation = regval == 0 ? MEMPROT_OP_READ : MEMPROT_OP_WRITE;
        }
    } break;
    case MEMPROT_TYPE_IRAM0_RTCFAST: {
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_monitor_status_fault_loadstore(core, &regval)))
        if (regval == 0) {
            operation = MEMPROT_OP_EXEC;
        } else {
            ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_monitor_status_fault_wr(core, &regval)))
            operation = regval == 0 ? MEMPROT_OP_READ : MEMPROT_OP_WRITE;
        }
    } break;
    case MEMPROT_TYPE_DRAM0_SRAM: {
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_monitor_status_fault_wr(core, &regval)))
        operation = regval == 0 ? MEMPROT_OP_WRITE : MEMPROT_OP_READ;
    } break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    *oper = operation;

    return ESP_OK;
}

bool esp_mprot_has_byte_enables(const esp_mprot_mem_t mem_type)
{
    return mem_type == MEMPROT_TYPE_DRAM0_SRAM;
}

esp_err_t esp_mprot_get_violate_byte_enables(const esp_mprot_mem_t mem_type, uint32_t *byte_en, const int core)
{
    if (byte_en == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err;
    ESP_MEMPROT_ERR_CHECK(err, esp_mprot_cpuid_valid(core))

    uint32_t byteen = 0xFFFFFFFF;

    //NOTE: more memory types coming in future updates, hence using switch
    switch (mem_type) {
    case MEMPROT_TYPE_DRAM0_SRAM:
        ESP_MEMPROT_ERR_CHECK(err, esp_mprot_ll_err_to_esp_err(memprot_ll_dram0_get_monitor_status_fault_byte_en(core, &byteen)))
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    *byte_en = byteen;

    return ESP_OK;
}

//////////////////////////////////////////////////////////////////////////////
// convenient "public" APIs
esp_err_t esp_mprot_set_prot(const esp_memp_config_t *memp_config)
{
    //debugger connected:
    // 1.check the signal repeatedly to avoid possible glitching attempt
    // 2.leave the Memprot unset to allow debug operations

    if (esp_cpu_dbgr_is_attached()) {
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());
        return ESP_OK;
    }

    //sanity checks
    if (memp_config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (memp_config->mem_type_mask == MEMPROT_TYPE_NONE) {
        return ESP_ERR_NO_MEM;
    }
    if (memp_config->target_cpu_count < 1 || memp_config->target_cpu_count > CONFIG_FREERTOS_NUMBER_OF_CORES) {
        return ESP_ERR_MEMPROT_CPUID_INVALID;
    }

    esp_err_t ret = ESP_OK;
    size_t core_count = memp_config->target_cpu_count;
    for (size_t x = 0; x < core_count; x++) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_cpuid_valid(memp_config->target_cpu[x]))
    }

    bool use_iram0 = memp_config->mem_type_mask & MEMPROT_TYPE_IRAM0_SRAM;
    bool use_dram0 = memp_config->mem_type_mask & MEMPROT_TYPE_DRAM0_SRAM;
    bool use_rtcfast = memp_config->mem_type_mask & MEMPROT_TYPE_IRAM0_RTCFAST;

    //disable protection (must be unlocked)
    if (use_iram0) {
        for (size_t x = 0; x < core_count; x++) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_IRAM0_SRAM, false, memp_config->target_cpu[x]))
        }
    }
    if (use_dram0) {
        for (size_t x = 0; x < core_count; x++) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_DRAM0_SRAM, false, memp_config->target_cpu[x]))
        }
    }
    if (use_rtcfast) {
        for (size_t x = 0; x < core_count; x++) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_IRAM0_RTCFAST, false, memp_config->target_cpu[x]))
        }
    }

    //panic handling
    if (memp_config->invoke_panic_handler) {
        if (use_iram0) {
            for (size_t x = 0; x < core_count; x++) {
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_intr_matrix(MEMPROT_TYPE_IRAM0_SRAM, memp_config->target_cpu[x]))
            }
        }
        if (use_dram0) {
            for (size_t x = 0; x < core_count; x++) {
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_intr_matrix(MEMPROT_TYPE_DRAM0_SRAM, memp_config->target_cpu[x]))
            }
        }
        if (use_rtcfast) {
            for (size_t x = 0; x < core_count; x++) {
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_intr_matrix(MEMPROT_TYPE_IRAM0_RTCFAST, memp_config->target_cpu[x]))
            }
        }
    }

    //set split lines (must-have for all mem_types). This version sets only the main I/D which is then shared for all PMS regions
    void *line_addr __attribute__((unused)) = NULL;
    if (use_iram0 || use_dram0) {
        line_addr = memp_config->split_addr;
        if (line_addr == NULL) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_default_main_split_addr(MEMPROT_TYPE_IRAM0_SRAM, &line_addr))
        }

        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_IRAM0_SRAM, MEMPROT_SPLIT_ADDR_IRAM0_LINE_1, line_addr, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_IRAM0_SRAM, MEMPROT_SPLIT_ADDR_IRAM0_LINE_0, line_addr, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_IRAM0_SRAM, MEMPROT_SPLIT_ADDR_IRAM0_DRAM0, line_addr, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_DRAM0_SRAM, MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_0, (void *)(MAP_IRAM_TO_DRAM((uint32_t)line_addr)), DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_DRAM0_SRAM, MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_1, (void *)(MAP_IRAM_TO_DRAM((uint32_t)line_addr)), DEFAULT_CPU_NUM))
    }

    //set permissions
    if (use_iram0) {
        ret = ESP_OK;
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_ICACHE_0, MEMPROT_OP_NONE, DEFAULT_CPU_NUM));
#if CONFIG_ESP32S3_INSTRUCTION_CACHE_16KB
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_ICACHE_1, MEMPROT_OP_READ | MEMPROT_OP_EXEC, DEFAULT_CPU_NUM));
#else
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_ICACHE_1, MEMPROT_OP_NONE, DEFAULT_CPU_NUM));
#endif
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_0, MEMPROT_OP_READ | MEMPROT_OP_EXEC, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_1, MEMPROT_OP_READ | MEMPROT_OP_EXEC, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_2, MEMPROT_OP_READ | MEMPROT_OP_EXEC, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_3, MEMPROT_OP_NONE, DEFAULT_CPU_NUM))
    }
    if (use_dram0) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_0, MEMPROT_OP_READ, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_1, MEMPROT_OP_READ | MEMPROT_OP_WRITE, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_2, MEMPROT_OP_READ | MEMPROT_OP_WRITE, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_3, MEMPROT_OP_READ | MEMPROT_OP_WRITE, DEFAULT_CPU_NUM))
    }

    void *rtc_fast_line __attribute__((unused)) = NULL;
    if (use_rtcfast) {
        //RTCFAST split-line cannot be set manually - always use default
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_default_main_split_addr(MEMPROT_TYPE_IRAM0_RTCFAST, &rtc_fast_line));
        for (size_t x = 0; x < core_count; x++) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_IRAM0_RTCFAST, MEMPROT_SPLIT_ADDR_MAIN, rtc_fast_line, memp_config->target_cpu[x]))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO, MEMPROT_OP_READ | MEMPROT_OP_EXEC, memp_config->target_cpu[x]))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI, MEMPROT_OP_READ | MEMPROT_OP_WRITE, memp_config->target_cpu[x]))
        }
    }

    //reenable the protection
    if (use_iram0) {
        for (size_t x = 0; x < core_count; x++) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_monitor_clear_intr(MEMPROT_TYPE_IRAM0_SRAM, memp_config->target_cpu[x]))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_IRAM0_SRAM, true, memp_config->target_cpu[x]))
        }
    }

    if (use_dram0) {
        for (size_t x = 0; x < core_count; x++) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_monitor_clear_intr(MEMPROT_TYPE_DRAM0_SRAM, memp_config->target_cpu[x]))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_DRAM0_SRAM, true, memp_config->target_cpu[x]))
        }
    }
    if (use_rtcfast) {
        for (size_t x = 0; x < core_count; x++) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_monitor_clear_intr(MEMPROT_TYPE_IRAM0_RTCFAST, memp_config->target_cpu[x]))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_IRAM0_RTCFAST, true, memp_config->target_cpu[x]))
        }
    }

    //lock if required
    if (memp_config->lock_feature) {
        if (use_iram0) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr_lock(MEMPROT_TYPE_IRAM0_SRAM, DEFAULT_CPU_NUM))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_lock(MEMPROT_TYPE_IRAM0_SRAM, DEFAULT_CPU_NUM))
            for (size_t x = 0; x < core_count; x++) {
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_lock(MEMPROT_TYPE_IRAM0_SRAM, memp_config->target_cpu[x]))
            }
        }
        if (use_dram0) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr_lock(MEMPROT_TYPE_DRAM0_SRAM, DEFAULT_CPU_NUM))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_lock(MEMPROT_TYPE_DRAM0_SRAM, DEFAULT_CPU_NUM))
            for (size_t x = 0; x < core_count; x++) {
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_lock(MEMPROT_TYPE_DRAM0_SRAM, memp_config->target_cpu[x]))
            }
        }
        if (use_rtcfast) {
            //split address and area permissions are locked by the same PMS register
            for (size_t x = 0; x < core_count; x++) {
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_lock(MEMPROT_TYPE_IRAM0_RTCFAST, memp_config->target_cpu[x]))
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_lock(MEMPROT_TYPE_IRAM0_RTCFAST, memp_config->target_cpu[x]))
            }
        }
    }

    //sanity check (RTC FAST tbd within IDF-5208)
    if (use_iram0 || use_dram0) {

        uint32_t check_val;

        //IRAM0 split lines
        memprot_ll_prepare_iram0_split_line_regval((const uint32_t) line_addr, &check_val);

        if (memprot_ll_get_iram0_split_line_main_I_D_regval() != check_val) {
            esp_rom_printf(
                "Fatal error: Main I/D split line configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                check_val, memprot_ll_get_iram0_split_line_main_I_D_regval());
            abort();
        }
        if (memprot_ll_get_iram0_split_line_main_I_0_regval() != check_val) {
            esp_rom_printf(
                "Fatal error: IRAM0 I_0 split line configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                check_val, memprot_ll_get_iram0_split_line_main_I_0_regval());
            abort();
        }
        if (memprot_ll_get_iram0_split_line_main_I_1_regval() != check_val) {
            esp_rom_printf(
                "Fatal error: IRAM0 I_1 split line configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                check_val, memprot_ll_get_iram0_split_line_main_I_1_regval());
            abort();
        }

        //DRAM0 split lines
        memprot_ll_prepare_dram0_split_line_regval(MAP_IRAM_TO_DRAM((const uint32_t) line_addr), &check_val);

        if (memprot_ll_get_dram0_split_line_main_D_0_regval() != check_val) {
            esp_rom_printf(
                "Fatal error: DRAM0 D_0 split line configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                check_val, memprot_ll_get_dram0_split_line_main_D_0_regval());
            abort();
        }
        if (memprot_ll_get_dram0_split_line_main_D_1_regval() != check_val) {
            esp_rom_printf(
                "Fatal error: DRAM0 D_1 split line configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                check_val, memprot_ll_get_dram0_split_line_main_D_1_regval());
            abort();
        }

        //IRAM0 perms
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_area(MEMPROT_PMS_AREA_IRAM0_0, &check_val, DEFAULT_CPU_NUM))
        if (check_val != (MEMPROT_OP_READ | MEMPROT_OP_EXEC)) {
            esp_rom_printf("Fatal error: IRAM0 PMS AREA_0 configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                           (uint32_t)MEMPROT_OP_READ | MEMPROT_OP_EXEC, check_val);
            abort();
        }

        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_area(MEMPROT_PMS_AREA_IRAM0_1, &check_val, DEFAULT_CPU_NUM))
        if (check_val != (MEMPROT_OP_READ | MEMPROT_OP_EXEC)) {
            esp_rom_printf("Fatal error: IRAM0 PMS AREA_1 configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                           (uint32_t)MEMPROT_OP_READ | MEMPROT_OP_EXEC, check_val);
            abort();
        }

        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_area(MEMPROT_PMS_AREA_IRAM0_2, &check_val, DEFAULT_CPU_NUM))
        if (check_val != (MEMPROT_OP_READ | MEMPROT_OP_EXEC)) {
            esp_rom_printf("Fatal error: IRAM0 PMS AREA_2 configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                           (uint32_t)MEMPROT_OP_READ | MEMPROT_OP_EXEC, check_val);
            abort();
        }

        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_area(MEMPROT_PMS_AREA_IRAM0_3, &check_val, DEFAULT_CPU_NUM))
        if (check_val != MEMPROT_OP_NONE) {
            esp_rom_printf("Fatal error: IRAM0 PMS AREA_3 configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                          (uint32_t) MEMPROT_OP_NONE, check_val);
            abort();
        }

        //DRAM0 perms
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_area(MEMPROT_PMS_AREA_DRAM0_0, &check_val, DEFAULT_CPU_NUM))
        if (check_val != MEMPROT_OP_READ) {
            esp_rom_printf("Fatal error: DRAM0 PMS AREA_0 configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                           (uint32_t)MEMPROT_OP_READ, check_val);
            abort();
        }

        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_area(MEMPROT_PMS_AREA_DRAM0_1, &check_val, DEFAULT_CPU_NUM))
        if (check_val != (MEMPROT_OP_READ | MEMPROT_OP_WRITE)) {
            esp_rom_printf("Fatal error: DRAM0 PMS AREA_1 configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                           (uint32_t)MEMPROT_OP_READ | MEMPROT_OP_WRITE, check_val);
            abort();
        }

        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_area(MEMPROT_PMS_AREA_DRAM0_2, &check_val, DEFAULT_CPU_NUM))
        if (check_val != (MEMPROT_OP_READ | MEMPROT_OP_WRITE)) {
            esp_rom_printf("Fatal error: DRAM0 PMS AREA_2 configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                           (uint32_t)MEMPROT_OP_READ | MEMPROT_OP_WRITE, check_val);
            abort();
        }

        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_area(MEMPROT_PMS_AREA_DRAM0_3, &check_val, DEFAULT_CPU_NUM))
        if (check_val != (MEMPROT_OP_READ | MEMPROT_OP_WRITE)) {
            esp_rom_printf("Fatal error: DRAM0 PMS AREA_3 configuration corrupted (expected 0x%08" PRIX32 ", stored 0x%08" PRIX32 ")\n",
                           (uint32_t)MEMPROT_OP_READ | MEMPROT_OP_WRITE, check_val);
            abort();
        }

        //memory protection enabled
        bool enabled;
        if (use_iram0) {
            for (size_t x = 0; x < core_count; x++) {
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_monitor_en(MEMPROT_TYPE_IRAM0_SRAM, &enabled,
                                      memp_config->target_cpu[x]))
                if (!enabled) {
                    esp_rom_printf(
                        "Fatal error: IRAM0 PMS configuration corrupted (memory protection not enabled on core %d)\n",
                        memp_config->target_cpu[x]);
                    abort();
                }
            }
        }
        if (use_dram0) {
            for (size_t x = 0; x < core_count; x++) {
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_monitor_en(MEMPROT_TYPE_DRAM0_SRAM, &enabled,
                                      memp_config->target_cpu[x]))
                if (!enabled) {
                    esp_rom_printf(
                        "Fatal error: DRAM0 PMS configuration corrupted (memory protection not enabled on core %d)\n",
                        memp_config->target_cpu[x]);
                    abort();
                }
            }
        }

        //locks
        if (memp_config->lock_feature) {

            bool locked;

            if (use_iram0) {
                ESP_MEMPROT_ERR_CHECK(ret,
                                      esp_mprot_get_split_addr_lock(MEMPROT_TYPE_IRAM0_SRAM, &locked, DEFAULT_CPU_NUM))
                if (!locked) {
                    esp_rom_printf(
                        "Fatal error: IRAM0 PMS configuration corrupted (memory protection not locked - split address lock)\n");
                    abort();
                }
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_lock(MEMPROT_TYPE_IRAM0_SRAM, &locked, DEFAULT_CPU_NUM))
                if (!locked) {
                    esp_rom_printf(
                        "Fatal error: IRAM0 PMS configuration corrupted (memory protection not locked - global PMS lock)\n");
                    abort();
                }
                for (size_t x = 0; x < core_count; x++) {
                    ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_monitor_lock(MEMPROT_TYPE_IRAM0_SRAM, &locked,
                                          memp_config->target_cpu[x]))
                    if (!locked) {
                        esp_rom_printf(
                            "Fatal error: IRAM0 PMS configuration corrupted (memory protection not locked - monitor lock on core %d)\n",
                            memp_config->target_cpu[x]);
                        abort();
                    }
                }
            }

            if (use_dram0) {
                ESP_MEMPROT_ERR_CHECK(ret,
                                      esp_mprot_get_split_addr_lock(MEMPROT_TYPE_DRAM0_SRAM, &locked, DEFAULT_CPU_NUM))
                if (!locked) {
                    esp_rom_printf(
                        "Fatal error: DRAM0 PMS configuration corrupted (memory protection not locked - split address lock)\n");
                    abort();
                }
                ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_pms_lock(MEMPROT_TYPE_DRAM0_SRAM, &locked, DEFAULT_CPU_NUM))
                if (!locked) {
                    esp_rom_printf(
                        "Fatal error: DRAM0 PMS configuration corrupted (memory protection not locked - global PMS lock)\n");
                    abort();
                }
                for (size_t x = 0; x < core_count; x++) {
                    ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_monitor_lock(MEMPROT_TYPE_DRAM0_SRAM, &locked,
                                          memp_config->target_cpu[x]))
                    if (!locked) {
                        esp_rom_printf(
                            "Fatal error: DRAM0 PMS configuration corrupted (memory protection not locked - monitor lock on core %d)\n",
                            memp_config->target_cpu[x]);
                        abort();
                    }
                }
            }
        }
    }

    //keep current configuration copy if all went well
    if (ret == ESP_OK) {
        s_memp_cfg = *memp_config;
    }

    return ret;
}

esp_err_t esp_mprot_dump_configuration(char **dump_info_string)
{
    if (dump_info_string == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *dump_info_string = (char *)calloc(1, 2048);

    if (*dump_info_string == NULL) {
        return ESP_ERR_NO_MEM;
    }

    sprintf(*dump_info_string,
            "Memory sections:\n"
            " _iram_text_start: 0x%08"PRIX32"\n _iram_text_end: 0x%08"PRIX32"\n",
            (uint32_t)&_iram_text_start, (uint32_t)&_iram_text_end);

    uint32_t offset = strlen(*dump_info_string);

    bool line_lock = memprot_ll_get_iram0_dram0_split_line_lock();
    uint32_t line_ID = (uint32_t)memprot_ll_get_iram0_split_line_main_I_D();
    uint32_t line_I0 = (uint32_t)memprot_ll_get_iram0_split_line_I_0();
    uint32_t line_I1 = (uint32_t)memprot_ll_get_iram0_split_line_I_1();
    uint32_t line_D0 = (uint32_t)memprot_ll_get_dram0_split_line_D_0();
    uint32_t line_D1 = (uint32_t)memprot_ll_get_dram0_split_line_D_1();
    uint32_t line_ID_cat = (uint32_t)memprot_ll_get_iram0_split_line_main_I_D_cat();
    uint32_t line_I0_cat = (uint32_t)memprot_ll_get_iram0_split_line_I_0_cat();
    uint32_t line_I1_cat = (uint32_t)memprot_ll_get_iram0_split_line_I_1_cat();
    uint32_t line_D0_cat = (uint32_t)memprot_ll_get_dram0_split_line_D_0_cat();
    uint32_t line_D1_cat = (uint32_t)memprot_ll_get_dram0_split_line_D_1_cat();

    sprintf((*dump_info_string + offset),
            "Split line settings (lock=%u):\n"
            " IRAM0:\n   line ID (main): 0x%08"PRIX32" (cat=0x%08"PRIX32")\n   line I0: 0x%08"PRIX32" (cat=0x%08"PRIX32")\n   line I1: 0x%08"PRIX32" (cat=0x%08"PRIX32")\n"
            " DRAM0:\n   line D0: 0x%08"PRIX32" (cat=0x%08"PRIX32")\n   line D1: 0x%08"PRIX32" (cat=0x%08"PRIX32")\n",
            line_lock, line_ID, line_ID_cat, line_I0, line_I0_cat, line_I1, line_I1_cat, line_D0, line_D0_cat, line_D1, line_D1_cat);

    offset = strlen(*dump_info_string);

    void *line_RTC = NULL;
    esp_err_t err = esp_mprot_ll_err_to_esp_err(memprot_ll_get_rtcfast_split_line(PRO_CPU_NUM, MEMP_HAL_WORLD_0, &line_RTC));
    if (err != ESP_OK) {
        sprintf((*dump_info_string + offset), " RTCFAST:\n   line main: N/A (world=0) - %s\n", esp_err_to_name(err));
    } else {
        sprintf((*dump_info_string + offset), " RTCFAST:\n   line main: 0x%08"PRIX32" (world=0)\n", (uint32_t)line_RTC);
    }
    offset = strlen(*dump_info_string);

    bool ar0i, ar1i, ar2i, ar3i;
    bool aw0i, aw1i, aw2i, aw3i;
    bool ax0i, ax1i, ax2i, ax3i;
    bool ar0d, ar1d, ar2d, ar3d;
    bool aw0d, aw1d, aw2d, aw3d;

    bool pms_lock_i = memprot_ll_iram0_get_pms_lock();
    memprot_ll_iram0_get_pms_area_0(&ar0i, &aw0i, &ax0i);
    memprot_ll_iram0_get_pms_area_1(&ar1i, &aw1i, &ax1i);
    memprot_ll_iram0_get_pms_area_2(&ar2i, &aw2i, &ax2i);
    memprot_ll_iram0_get_pms_area_3(&ar3i, &aw3i, &ax3i);

    bool pms_lock_d = memprot_ll_dram0_get_pms_lock();
    memprot_ll_dram0_get_pms_area_0(&ar0d, &aw0d);
    memprot_ll_dram0_get_pms_area_1(&ar1d, &aw1d);
    memprot_ll_dram0_get_pms_area_2(&ar2d, &aw2d);
    memprot_ll_dram0_get_pms_area_3(&ar3d, &aw3d);

    bool rtc_line_lock_0;
    memprot_ll_get_pif_constraint_lock(PRO_CPU_NUM, &rtc_line_lock_0);

    sprintf((*dump_info_string + offset),
            "PMS area settings:\n"
            " IRAM0 (lock=%u):\n   area 0: r=%u,w=%u,x=%u\n   area 1: r=%u,w=%u,x=%u\n   area 2: r=%u,w=%u,x=%u\n   area 3: r=%u,w=%u,x=%u\n"
            " DRAM0 (lock=%u):\n   area 0: r=%u,w=%u\n   area 1: r=%u,w=%u\n   area 2: r=%u,w=%u\n   area 3: r=%u,w=%u\n"
            " RTCFAST (PRO_CPU, lock=%u):\n",
            pms_lock_i, ar0i, aw0i, ax0i, ar1i, aw1i, ax1i, ar2i, aw2i, ax2i, ar3i, aw3i, ax3i,
            pms_lock_d, ar0d, aw0d, ar1d, aw1d, ar2d, aw2d, ar3d, aw3d, rtc_line_lock_0);

    offset = strlen(*dump_info_string);

    bool arl0rtc, awl0rtc, axl0rtc;
    bool arh0rtc, awh0rtc, axh0rtc;

    err = esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_pms_area(PRO_CPU_NUM, &arl0rtc, &awl0rtc, &axl0rtc, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_LOW));
    if (err != ESP_OK) {
        sprintf((*dump_info_string + offset), "   area low: N/A - %s\n", esp_err_to_name(err));
    } else {
        sprintf((*dump_info_string + offset), "   area low: r=%u,w=%u,x=%u\n", arl0rtc, awl0rtc, axl0rtc);
    }

    offset = strlen(*dump_info_string);

    err = esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_pms_area(PRO_CPU_NUM, &arh0rtc, &awh0rtc, &axh0rtc, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_HIGH));
    if (err != ESP_OK) {
        sprintf((*dump_info_string + offset), "   area high: N/A - %s\n", esp_err_to_name(err));
    } else {
        sprintf((*dump_info_string + offset), "   area high: r=%u,w=%u,x=%u\n", arh0rtc, awh0rtc, axh0rtc);
    }

    offset = strlen(*dump_info_string);

    //2-CPU setup
    if (s_memp_cfg.target_cpu_count > 1) {

        bool rtc_line_lock_1;
        memprot_ll_get_pif_constraint_lock(APP_CPU_NUM, &rtc_line_lock_1);
        sprintf((*dump_info_string + offset), " RTCFAST (APP_CPU, lock=%u):\n", rtc_line_lock_1);

        offset = strlen(*dump_info_string);

        err = esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_pms_area(APP_CPU_NUM, &arl0rtc, &awl0rtc, &axl0rtc, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_LOW));
        if (err != ESP_OK) {
            sprintf((*dump_info_string + offset), "   area low: N/A - %s\n", esp_err_to_name(err));
        } else {
            sprintf((*dump_info_string + offset), "   area low: r=%u,w=%u,x=%u\n", arl0rtc, awl0rtc, axl0rtc);
        }

        offset = strlen(*dump_info_string);

        err = esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_pms_area(APP_CPU_NUM, &arh0rtc, &awh0rtc, &axh0rtc, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_HIGH));
        if (err != ESP_OK) {
            sprintf((*dump_info_string + offset), "   area high: N/A - %s\n", esp_err_to_name(err));
        } else {
            sprintf((*dump_info_string + offset), "   area high: r=%u,w=%u,x=%u\n", arh0rtc, awh0rtc, axh0rtc);
        }

        offset = strlen(*dump_info_string);
    }

    return ESP_OK;
}
