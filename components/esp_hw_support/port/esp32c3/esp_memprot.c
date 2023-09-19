/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "soc/periph_defs.h"
#include "esp_intr_alloc.h"
#include "hal/memprot_ll.h"
#include "riscv/interrupt.h"
#include "esp_rom_sys.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "esp_attr.h"
#include "hal/memprot_types.h"
#include "esp_private/esp_memprot_internal.h"
#include "esp_memprot.h"
#include <inttypes.h>

extern int _iram_text_end;
extern int _rtc_text_end;

static void *esp_memprot_iram0_get_def_split_addr(void)
{
    return &_iram_text_end;
}

static void *esp_memprot_dram0_get_def_split_addr(void)
{
    return (void *)MAP_IRAM_TO_DRAM((uint32_t)&_iram_text_end);
}

static void *esp_memprot_rtcfast_get_min_split_addr(void)
{
    return &_rtc_text_end;
}

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
        if (line_type == MEMPROT_SPLIT_ADDR_MAIN) {
            /* so far only WORLD_0 is supported */
            return esp_mprot_ll_err_to_esp_err(memprot_ll_set_rtcfast_split_line(line_addr, MEMP_HAL_WORLD_0));
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
        if (line_type == MEMPROT_SPLIT_ADDR_MAIN) {
            /* so far only WORLD_0 is supported */
            return esp_mprot_ll_err_to_esp_err(memprot_ll_get_rtcfast_split_line(MEMP_HAL_WORLD_0, line_addr));
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
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        //caution: RTCFAST shares the lock with other PIF PMS constraints!
        memprot_ll_set_pif_constraint_lock();
        break;
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
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        *locked = memprot_ll_get_pif_constraint_lock();
        break;
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
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        //caution: RTCFAST shares the lock with other PIF PMS constraints!
        memprot_ll_set_pif_constraint_lock();
        break;
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
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        *locked = memprot_ll_get_pif_constraint_lock();
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_set_pms_area(const esp_mprot_pms_area_t area_type, const uint32_t flags, const int core __attribute__((unused)))
{
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
        memprot_ll_rtcfast_set_pms_area(r, w, x, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_LOW);
        break;
    case MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI:
        memprot_ll_rtcfast_set_pms_area(r, w, x, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_HIGH);
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
        memprot_ll_rtcfast_get_pms_area(&r, &w, &x, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_LOW);
        break;
    case MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI:
        memprot_ll_rtcfast_get_pms_area(&r, &w, &x, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_HIGH);
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

esp_err_t esp_mprot_set_monitor_lock(const esp_mprot_mem_t mem_type, const int core __attribute__((unused)))
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        memprot_ll_iram0_set_monitor_lock();
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        memprot_ll_dram0_set_monitor_lock();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        memprot_ll_rtcfast_set_monitor_lock();
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_monitor_lock(const esp_mprot_mem_t mem_type, bool *locked, const int core __attribute__((unused)))
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        *locked = memprot_ll_iram0_get_monitor_lock();
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        *locked = memprot_ll_dram0_get_monitor_lock();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        *locked = memprot_ll_rtcfast_get_monitor_lock();
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_set_monitor_en(const esp_mprot_mem_t mem_type, const bool enable, const int core __attribute__((unused)))
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        memprot_ll_iram0_set_monitor_en(enable);
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        memprot_ll_dram0_set_monitor_en(enable);
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        memprot_ll_rtcfast_set_monitor_en(enable);
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_monitor_en(const esp_mprot_mem_t mem_type, bool *enabled, const int core __attribute__((unused)))
{
    if (enabled == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        *enabled = memprot_ll_iram0_get_monitor_en();
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        *enabled = memprot_ll_dram0_get_monitor_en();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        *enabled = memprot_ll_rtcfast_get_monitor_en();
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_monitor_clear_intr(const esp_mprot_mem_t mem_type, const int core __attribute__((unused)))
{
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        memprot_ll_iram0_set_monitor_intrclr();
        memprot_ll_iram0_reset_monitor_intrclr();
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        memprot_ll_dram0_set_monitor_intrclr();
        memprot_ll_dram0_reset_monitor_intrclr();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        memprot_ll_rtcfast_set_monitor_intrclr();
        memprot_ll_rtcfast_reset_monitor_intrclr();
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_active_intr(esp_memp_intr_source_t *active_memp_intr)
{
    if (active_memp_intr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    active_memp_intr->core = PRO_CPU_NUM;

    if (memprot_ll_iram0_get_monitor_status_intr() > 0) {
        active_memp_intr->mem_type = MEMPROT_TYPE_IRAM0_SRAM;
    } else if (memprot_ll_dram0_get_monitor_status_intr() > 0) {
        active_memp_intr->mem_type = MEMPROT_TYPE_DRAM0_SRAM;
    } else if (memprot_ll_rtcfast_get_monitor_status_intr() > 0) {
        active_memp_intr->mem_type = MEMPROT_TYPE_IRAM0_RTCFAST;
    } else {
        active_memp_intr->mem_type = MEMPROT_TYPE_NONE;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_is_conf_locked_any(bool *locked)
{
    if (locked == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bool lock_on = false;
    esp_err_t res;

    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_split_addr_lock(MEMPROT_TYPE_IRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
    *locked |= lock_on;
    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_split_addr_lock(MEMPROT_TYPE_DRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
    *locked |= lock_on;
    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_pms_lock(MEMPROT_TYPE_IRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
    *locked |= lock_on;
    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_pms_lock(MEMPROT_TYPE_DRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
    *locked |= lock_on;
    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_monitor_lock(MEMPROT_TYPE_IRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM))
    *locked |= lock_on;
    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_monitor_lock(MEMPROT_TYPE_DRAM0_SRAM, &lock_on, DEFAULT_CPU_NUM));
    *locked |= lock_on;
    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_monitor_lock(MEMPROT_TYPE_IRAM0_RTCFAST, &lock_on, DEFAULT_CPU_NUM));
    *locked |= lock_on;

    return ESP_OK;
}

esp_err_t esp_mprot_is_intr_ena_any(bool *enabled)
{
    if (enabled == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bool ena_on = false;
    esp_err_t res;

    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_monitor_en(MEMPROT_TYPE_IRAM0_SRAM, &ena_on, DEFAULT_CPU_NUM))
    *enabled |= ena_on;
    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_monitor_en(MEMPROT_TYPE_DRAM0_SRAM, &ena_on, DEFAULT_CPU_NUM))
    *enabled |= ena_on;
    ESP_MEMPROT_ERR_CHECK(res, esp_mprot_get_monitor_en(MEMPROT_TYPE_IRAM0_RTCFAST, &ena_on, DEFAULT_CPU_NUM))
    *enabled |= ena_on;

    return ESP_OK;
}

esp_err_t esp_mprot_get_violate_addr(const esp_mprot_mem_t mem_type, void **fault_addr, const int core __attribute__((unused)))
{
    if (fault_addr == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        *fault_addr = (void *)memprot_ll_iram0_get_monitor_status_fault_addr();
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        *fault_addr = (void *)memprot_ll_dram0_get_monitor_status_fault_addr();
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        *fault_addr = (void *)memprot_ll_rtcfast_get_monitor_status_fault_addr();
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

esp_err_t esp_mprot_get_violate_world(const esp_mprot_mem_t mem_type, esp_mprot_pms_world_t *world, const int core __attribute__((unused)))
{
    if (world == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_mprot_pms_world_t res;
    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        res = esp_mprot_ll_world_to_hl_world(memprot_ll_iram0_get_monitor_status_fault_world());
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        res = esp_mprot_ll_world_to_hl_world(memprot_ll_dram0_get_monitor_status_fault_world());
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        res = esp_mprot_ll_world_to_hl_world(memprot_ll_rtcfast_get_monitor_status_fault_world());
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    if (res == MEMPROT_PMS_WORLD_INVALID) {
        return ESP_ERR_MEMPROT_WORLD_INVALID;
    }

    *world = res;

    return ESP_OK;
}

esp_err_t esp_mprot_get_violate_operation(const esp_mprot_mem_t mem_type, uint32_t *oper, const int core __attribute__((unused)))
{
    if (oper == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t operation = MEMPROT_OP_NONE;

    if ((mem_type == MEMPROT_TYPE_IRAM0_SRAM && memprot_ll_iram0_get_monitor_status_fault_loadstore() == 0) ||
            (mem_type == MEMPROT_TYPE_IRAM0_RTCFAST && memprot_ll_rtcfast_get_monitor_status_fault_loadstore() == 0)) {
        operation = MEMPROT_OP_EXEC;
    }

    if (operation == MEMPROT_OP_NONE) {
        switch (mem_type) {
        case MEMPROT_TYPE_IRAM0_SRAM:
            operation = memprot_ll_iram0_get_monitor_status_fault_wr() == 1 ? MEMPROT_OP_WRITE : MEMPROT_OP_READ;
            break;
        case MEMPROT_TYPE_DRAM0_SRAM:
            operation = memprot_ll_dram0_get_monitor_status_fault_wr() == 1 ? MEMPROT_OP_WRITE : MEMPROT_OP_READ;
            break;
        case MEMPROT_TYPE_IRAM0_RTCFAST:
            operation = memprot_ll_rtcfast_get_monitor_status_fault_wr() == 1 ? MEMPROT_OP_WRITE : MEMPROT_OP_READ;
            break;
        default:
            return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
        }
    }

    *oper = operation;

    return ESP_OK;
}

bool esp_mprot_has_byte_enables(const esp_mprot_mem_t mem_type)
{
    return mem_type == MEMPROT_TYPE_DRAM0_SRAM;
}

esp_err_t esp_mprot_get_violate_byte_enables(const esp_mprot_mem_t mem_type, uint32_t *byte_en, const int core __attribute__((unused)))
{
    if (byte_en == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    switch (mem_type) {
    case MEMPROT_TYPE_DRAM0_SRAM:
        *byte_en = memprot_ll_dram0_get_monitor_status_fault_byte_en();
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    return ESP_OK;
}

/* internal */
static esp_err_t esp_mprot_set_intr_matrix(const esp_mprot_mem_t mem_type)
{
    ESP_INTR_DISABLE(ETS_MEMPROT_ERR_INUM);

    switch (mem_type) {
    case MEMPROT_TYPE_IRAM0_SRAM:
        esp_rom_route_intr_matrix(PRO_CPU_NUM, memprot_ll_iram0_get_intr_source_num(), ETS_MEMPROT_ERR_INUM);
        break;
    case MEMPROT_TYPE_DRAM0_SRAM:
        esp_rom_route_intr_matrix(PRO_CPU_NUM, memprot_ll_dram0_get_intr_source_num(), ETS_MEMPROT_ERR_INUM);
        break;
    case MEMPROT_TYPE_IRAM0_RTCFAST:
        esp_rom_route_intr_matrix(PRO_CPU_NUM, memprot_ll_rtcfast_get_intr_source_num(), ETS_MEMPROT_ERR_INUM);
        break;
    default:
        return ESP_ERR_MEMPROT_MEMORY_TYPE_INVALID;
    }

    /* Set the type and priority to cache error interrupts. */
    esprv_intc_int_set_type(ETS_MEMPROT_ERR_INUM, INTR_TYPE_LEVEL);
    esprv_intc_int_set_priority(ETS_MEMPROT_ERR_INUM, SOC_INTERRUPT_LEVEL_MEDIUM);

    ESP_INTR_ENABLE(ETS_MEMPROT_ERR_INUM);

    return ESP_OK;
}

esp_err_t esp_mprot_set_prot(const esp_memp_config_t *memp_config)
{
    if (memp_config == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (memp_config->mem_type_mask == MEMPROT_TYPE_NONE) {
        return ESP_ERR_NO_MEM;
    }

    //debugger connected:
    // 1.check the signal repeatedly to avoid possible glitching attempt
    // 2.leave the Memprot unset to allow debug operations
    if (esp_cpu_dbgr_is_attached()) {
        ESP_FAULT_ASSERT(esp_cpu_dbgr_is_attached());
        return ESP_OK;
    }

    bool use_iram0 = memp_config->mem_type_mask & MEMPROT_TYPE_IRAM0_SRAM;
    bool use_dram0 = memp_config->mem_type_mask & MEMPROT_TYPE_DRAM0_SRAM;
    bool use_rtcfast = memp_config->mem_type_mask & MEMPROT_TYPE_IRAM0_RTCFAST;
    esp_err_t ret = ESP_OK;

    //disable protection (must be unlocked)
    if (use_iram0) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_IRAM0_SRAM, false, DEFAULT_CPU_NUM))
    }
    if (use_dram0) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_DRAM0_SRAM, false, DEFAULT_CPU_NUM))
    }
    if (use_rtcfast) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_IRAM0_RTCFAST, false, DEFAULT_CPU_NUM))
    }

    //panic handling
    if (memp_config->invoke_panic_handler) {
        if (use_iram0) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_intr_matrix(MEMPROT_TYPE_IRAM0_SRAM))
        }
        if (use_dram0) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_intr_matrix(MEMPROT_TYPE_DRAM0_SRAM))
        }
        if (use_rtcfast) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_intr_matrix(MEMPROT_TYPE_IRAM0_RTCFAST))
        }
    }

    //set split lines (must-have for all mem_types). This version sets only the main I/D which is then shared for all PMS regions
    void *line_addr = memp_config->split_addr;
    if (line_addr == NULL) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_default_main_split_addr(MEMPROT_TYPE_IRAM0_SRAM, &line_addr))
    }

    ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_IRAM0_SRAM, MEMPROT_SPLIT_ADDR_IRAM0_LINE_1, line_addr, DEFAULT_CPU_NUM))
    ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_IRAM0_SRAM, MEMPROT_SPLIT_ADDR_IRAM0_LINE_0, line_addr, DEFAULT_CPU_NUM))
    ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_IRAM0_SRAM, MEMPROT_SPLIT_ADDR_IRAM0_DRAM0, line_addr, DEFAULT_CPU_NUM))
    ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_DRAM0_SRAM, MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_0, (void *)(MAP_IRAM_TO_DRAM((uint32_t)line_addr)), DEFAULT_CPU_NUM))
    ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_DRAM0_SRAM, MEMPROT_SPLIT_ADDR_DRAM0_DMA_LINE_1, (void *)(MAP_IRAM_TO_DRAM((uint32_t)line_addr)), DEFAULT_CPU_NUM))

    //set permissions
    if (use_iram0) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_0, MEMPROT_OP_READ | MEMPROT_OP_EXEC, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_1, MEMPROT_OP_READ | MEMPROT_OP_EXEC, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_2, MEMPROT_OP_READ | MEMPROT_OP_EXEC, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_3, MEMPROT_OP_NONE, DEFAULT_CPU_NUM))
    }
    if (use_dram0) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_0, MEMPROT_OP_NONE, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_1, MEMPROT_OP_READ | MEMPROT_OP_WRITE, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_2, MEMPROT_OP_READ | MEMPROT_OP_WRITE, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_DRAM0_3, MEMPROT_OP_READ | MEMPROT_OP_WRITE, DEFAULT_CPU_NUM))
    }
    if (use_rtcfast) {
        //RTCFAST split-line cannot be set manually - always use default
        void *rtc_fast_line;
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_get_default_main_split_addr(MEMPROT_TYPE_IRAM0_RTCFAST, &rtc_fast_line));
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr(MEMPROT_TYPE_IRAM0_RTCFAST, MEMPROT_SPLIT_ADDR_MAIN, rtc_fast_line, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_RTCFAST_LO, MEMPROT_OP_READ | MEMPROT_OP_EXEC, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_area(MEMPROT_PMS_AREA_IRAM0_RTCFAST_HI, MEMPROT_OP_READ | MEMPROT_OP_WRITE, DEFAULT_CPU_NUM))
    }

    //reenable the protection
    if (use_iram0) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_monitor_clear_intr(MEMPROT_TYPE_IRAM0_SRAM, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_IRAM0_SRAM, true, DEFAULT_CPU_NUM))
    }
    if (use_dram0) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_monitor_clear_intr(MEMPROT_TYPE_DRAM0_SRAM, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_DRAM0_SRAM, true, DEFAULT_CPU_NUM))
    }
    if (use_rtcfast) {
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_monitor_clear_intr(MEMPROT_TYPE_IRAM0_RTCFAST, DEFAULT_CPU_NUM))
        ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_en(MEMPROT_TYPE_IRAM0_RTCFAST, true, DEFAULT_CPU_NUM))
    }

    //lock if required
    if (memp_config->lock_feature) {
        if (use_iram0) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr_lock(MEMPROT_TYPE_IRAM0_SRAM, DEFAULT_CPU_NUM))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_lock(MEMPROT_TYPE_IRAM0_SRAM, DEFAULT_CPU_NUM))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_lock(MEMPROT_TYPE_IRAM0_SRAM, DEFAULT_CPU_NUM))
        }
        if (use_dram0) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr_lock(MEMPROT_TYPE_DRAM0_SRAM, DEFAULT_CPU_NUM))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_lock(MEMPROT_TYPE_DRAM0_SRAM, DEFAULT_CPU_NUM))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_lock(MEMPROT_TYPE_DRAM0_SRAM, DEFAULT_CPU_NUM))
        }
        if (use_rtcfast) {
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_split_addr_lock(MEMPROT_TYPE_IRAM0_RTCFAST, DEFAULT_CPU_NUM))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_pms_lock(MEMPROT_TYPE_IRAM0_RTCFAST, DEFAULT_CPU_NUM))
            ESP_MEMPROT_ERR_CHECK(ret, esp_mprot_set_monitor_lock(MEMPROT_TYPE_IRAM0_RTCFAST, DEFAULT_CPU_NUM))
        }
    }

    return ret;
}

esp_err_t esp_mprot_dump_configuration(char **dump_info_string)
{
    if (dump_info_string == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *dump_info_string = calloc(1024, 1);

    if (*dump_info_string == NULL) {
        return ESP_ERR_NO_MEM;
    }

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

    sprintf(*dump_info_string,
            "Split line settings (lock=%u):\n"
            " IRAM0:\n   line ID (main): 0x%08"PRIX32" (cat=0x%08"PRIX32")\n   line I0: 0x%08"PRIX32" (cat=0x%08"PRIX32")\n   line I1: 0x%08"PRIX32" (cat=0x%08"PRIX32")\n"
            " DRAM0:\n   line D0: 0x%08"PRIX32" (cat=0x%08"PRIX32")\n   line D1: 0x%08"PRIX32" (cat=0x%08"PRIX32")\n",
            line_lock, line_ID, line_ID_cat, line_I0, line_I0_cat, line_I1, line_I1_cat, line_D0, line_D0_cat, line_D1, line_D1_cat);

    uint32_t offset = strlen(*dump_info_string);

    void *line_RTC = NULL;
    esp_err_t err = esp_mprot_ll_err_to_esp_err(memprot_ll_get_rtcfast_split_line(MEMP_HAL_WORLD_0, &line_RTC));
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

    bool rtc_line_lock = memprot_ll_get_pif_constraint_lock();

    sprintf((*dump_info_string + offset),
            "PMS area settings:\n"
            " IRAM0 (lock=%u):\n   area 0: r=%u,w=%u,x=%u\n   area 1: r=%u,w=%u,x=%u\n   area 2: r=%u,w=%u,x=%u\n   area 3: r=%u,w=%u,x=%u\n"
            " DRAM0 (lock=%u):\n   area 0: r=%u,w=%u\n   area 1: r=%u,w=%u\n   area 2: r=%u,w=%u\n   area 3: r=%u,w=%u\n"
            " RTCFAST (lock=%u):\n",
            pms_lock_i, ar0i, aw0i, ax0i, ar1i, aw1i, ax1i, ar2i, aw2i, ax2i, ar3i, aw3i, ax3i,
            pms_lock_d, ar0d, aw0d, ar1d, aw1d, ar2d, aw2d, ar3d, aw3d, rtc_line_lock);

    offset = strlen(*dump_info_string);

    bool arl0rtc, awl0rtc, axl0rtc;
    bool arh0rtc, awh0rtc, axh0rtc;

    err = esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_pms_area(&arl0rtc, &awl0rtc, &axl0rtc, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_LOW));
    if (err != ESP_OK) {
        sprintf((*dump_info_string + offset), "   area low: N/A - %s\n", esp_err_to_name(err));
    } else {
        sprintf((*dump_info_string + offset), "   area low: r=%u,w=%u,x=%u\n", arl0rtc, awl0rtc, axl0rtc);
    }

    offset = strlen(*dump_info_string);

    err = esp_mprot_ll_err_to_esp_err(memprot_ll_rtcfast_get_pms_area(&arh0rtc, &awh0rtc, &axh0rtc, MEMP_HAL_WORLD_0, MEMP_HAL_AREA_HIGH));
    if (err != ESP_OK) {
        sprintf((*dump_info_string + offset), "   area high: N/A - %s\n", esp_err_to_name(err));
    } else {
        sprintf((*dump_info_string + offset), "   area high: r=%u,w=%u,x=%u\n", arh0rtc, awh0rtc, axh0rtc);
    }

    offset = strlen(*dump_info_string);

    bool monitor_lock_i = memprot_ll_iram0_get_monitor_lock();
    bool monitor_en_i = memprot_ll_iram0_get_monitor_en();
    bool monitor_clr_i = memprot_ll_iram0_get_monitor_intrclr();
    bool monitor_lock_d = memprot_ll_dram0_get_monitor_lock();
    bool monitor_en_d = memprot_ll_dram0_get_monitor_en();
    bool monitor_clr_d = memprot_ll_dram0_get_monitor_intrclr();
    bool monitor_lock_rtc = memprot_ll_rtcfast_get_monitor_lock();
    bool monitor_en_rtc = memprot_ll_rtcfast_get_monitor_en();
    bool monitor_clr_rtc = memprot_ll_rtcfast_get_monitor_intrclr();

    sprintf((*dump_info_string + offset),
            "Monitor settings:\n"
            " IRAM0 (lock=%u):\n   enabled=%u\n   intr_clr=%u\n"
            " DRAM0 (lock=%u):\n   enabled=%u\n   intr_clr=%u\n"
            " RTCFAST (lock=%u):\n   enabled=%u\n   intr_clr=%u\n",
            monitor_lock_i, monitor_en_i, monitor_clr_i,
            monitor_lock_d, monitor_en_d, monitor_clr_d,
            monitor_lock_rtc, monitor_en_rtc, monitor_clr_rtc);

    return ESP_OK;
}
