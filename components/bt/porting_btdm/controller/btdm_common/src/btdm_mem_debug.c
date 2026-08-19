/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "btdm_mem_debug.h"
#include "btdm_user_cfg.h"
#include <stdint.h>

#if BTDM_MEM_DEBUG

#include <stddef.h>
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include <string.h>

#define BTDM_MEM_DEBUG_MAX 1024 * 5

typedef struct {
    void *ptr;
    uint32_t size;
    uint32_t seq;
    const char *func;
    uint32_t line;
} btdm_mem_debug_entry_t;

static btdm_mem_debug_entry_t s_mem_dbg_tbl[BTDM_MEM_DEBUG_MAX];
static uint32_t s_mem_dbg_seq;
static uint32_t s_mem_dbg_count;
static portMUX_TYPE s_mem_dbg_lock = portMUX_INITIALIZER_UNLOCKED;
static const char *s_mem_dbg_func_name = NULL;
static uint32_t s_mem_dbg_line_num = 0;

void
btdm_mem_debug_record(void *ptr, uint32_t size)
{
    int i;

    if (ptr == NULL || size == 0) {
        return;
    }

    portENTER_CRITICAL(&s_mem_dbg_lock);

    for (i = 0; i < BTDM_MEM_DEBUG_MAX; i++) {
        if (s_mem_dbg_tbl[i].ptr == NULL) {
            s_mem_dbg_seq++;
            s_mem_dbg_tbl[i].ptr = ptr;
            s_mem_dbg_tbl[i].size = size;
            s_mem_dbg_tbl[i].seq = s_mem_dbg_seq;
            s_mem_dbg_tbl[i].func = s_mem_dbg_func_name;
            s_mem_dbg_tbl[i].line = s_mem_dbg_line_num;
            s_mem_dbg_count++;

            s_mem_dbg_func_name = NULL;
            s_mem_dbg_line_num = 0;
            portEXIT_CRITICAL(&s_mem_dbg_lock);
            return;
        }
    }
    s_mem_dbg_func_name = NULL;
    s_mem_dbg_line_num = 0;
    portEXIT_CRITICAL(&s_mem_dbg_lock);
    esp_rom_printf("[BTDM_MEM] record full, drop ptr=%p size=%u\n", ptr, (unsigned)size);
}

void
btdm_mem_debug_clear(void *ptr)
{
    int i;

    if (ptr == NULL) {
        return;
    }

    portENTER_CRITICAL(&s_mem_dbg_lock);

    for (i = 0; i < BTDM_MEM_DEBUG_MAX; i++) {
        if (s_mem_dbg_tbl[i].ptr == ptr) {
            s_mem_dbg_tbl[i].ptr = NULL;
            s_mem_dbg_tbl[i].size = 0;
            s_mem_dbg_tbl[i].seq = 0;
            s_mem_dbg_count--;
            portEXIT_CRITICAL(&s_mem_dbg_lock);
            return;
        }
    }

    portEXIT_CRITICAL(&s_mem_dbg_lock);
    esp_rom_printf("[BTDM_MEM] clear miss ptr=%p\n", ptr);
}

void
btdm_mem_debug_dump(void)
{
    int i;
    void *ptr;
    uint32_t size;
    uint32_t seq;
    uint32_t count = 0;
    uint32_t total_size = 0;
    const char *func = NULL;
    uint32_t line = 0;

    esp_rom_printf("[BTDM_MEM] ===== unfreed dump begin =====\n");

    for (i = 0; i < BTDM_MEM_DEBUG_MAX; i++) {
        portENTER_CRITICAL(&s_mem_dbg_lock);
        ptr = s_mem_dbg_tbl[i].ptr;
        size = s_mem_dbg_tbl[i].size;
        seq = s_mem_dbg_tbl[i].seq;
        func = s_mem_dbg_tbl[i].func;
        line = s_mem_dbg_tbl[i].line;
        portEXIT_CRITICAL(&s_mem_dbg_lock);

        if (ptr != NULL) {
            esp_rom_printf("[BTDM_MEM] ptr=%p size=%u seq=%u, func=%s, line=%u\n", ptr,
                           (unsigned)size, (unsigned)seq, func ? func : "unknown", line);
            count++;
            total_size += size;
        }
    }

    esp_rom_printf("[BTDM_MEM] unfreed count=%u total_size=%u\n",
                   (unsigned)count, (unsigned)total_size);
    esp_rom_printf("[BTDM_MEM] ===== unfreed dump end =====\n");


    portENTER_CRITICAL(&s_mem_dbg_lock);
    memset(s_mem_dbg_tbl, 0, sizeof(s_mem_dbg_tbl));
    s_mem_dbg_seq = 0;
    s_mem_dbg_count = 0;
    portEXIT_CRITICAL(&s_mem_dbg_lock);
}

uint32_t
btdm_mem_debug_get_count(void)
{
    uint32_t count;

    portENTER_CRITICAL(&s_mem_dbg_lock);
    count = s_mem_dbg_count;
    portEXIT_CRITICAL(&s_mem_dbg_lock);

    return count;
}

uint32_t
btdm_mem_debug_get_seq_num(void)
{
    uint32_t sequence;

    portENTER_CRITICAL(&s_mem_dbg_lock);
    sequence = s_mem_dbg_seq;
    portEXIT_CRITICAL(&s_mem_dbg_lock);

    return sequence;
}

void
ext_btdm_mem_debug_hook(const char *func, uint32_t line)
{
    if (s_mem_dbg_func_name == NULL) {
        s_mem_dbg_func_name = func;
        s_mem_dbg_line_num = line;
    }
}

#endif /* BTDM_MEM_DEBUG */
